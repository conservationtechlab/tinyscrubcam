/*
 * Copyright 2021, Cypress Semiconductor Corporation or a subsidiary of 
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software"),
 * is owned by Cypress Semiconductor Corporation
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/** @file
 * Silicon Backplane USB device core support
 *
 * $Id: usbdev_sb.c 506861 2014-10-07 18:36:23Z pnemavat $
 */

#include <typedefs.h>
#include <bcmdefs.h>
#include <bcmdevs.h>
#include <bcmendian.h>
#include <bcmutils.h>
#include <siutils.h>
#include <wiced_osl.h>
#include <sbchipc.h>
#include <sbhnddma.h>
#include <hndsoc.h>
#include <hnddma.h>
#include <hndpmu.h>
#include <osl.h>
#include <proto/ethernet.h>
#include <sbusbd.h>
#include <usbdev.h>
#include <usbstd.h>
#include <stdarg.h>
#include "platform_stdio.h"
#include "platform_usb.h"



#define USB_DEVICE_TEST_RDL          0

#define trace       printf
#define err         printf
#define dbg_printf  printf

/* New define for user to disable USB Device LPM support */
#define USB_LPM_SUPPORT     0

#define dngl_init_timer(context, data, fn)
#define dngl_add_timer(context, data, fn)
#define hex(msg, buf, len)

/* return total length of buffer chain */
uint BCMFASTPATH
pkttotlen(osl_t *osh, void *p)
{
    uint total;
    int len;

    total = 0;
    for (; p; p = PKTNEXT(osh, p)) {
        len = PKTLEN(osh, p);
#ifdef MACOSX
        if (len < 0) {
            /* Bad packet length, just drop and exit */
#ifdef BCMDBG
            printf("wl: pkttotlen bad (%p,%d)\n", p, len);
#endif
            break;
        }
#endif /* MACOSX */
        total += len;
#ifdef BCMLFRAG
        if (BCMLFRAG_ENAB()) {
            if (PKTISFRAG(osh, p)) {
                total += PKTFRAGTOTLEN(osh, p);
            }
        }
#endif
    }

    return (total);
}

/* return the last buffer of chained pkt */
void *
pktlast(osl_t *osh, void *p)
{
    for (; PKTNEXT(osh, p); p = PKTNEXT(osh, p))
        ;

    return (p);
}

/* count segments of a chained packet */
uint BCMFASTPATH
pktsegcnt(osl_t *osh, void *p)
{
    uint cnt;

    for (cnt = 0; p; p = PKTNEXT(osh, p)) {
        cnt++;
#ifdef BCMLFRAG
        if (BCMLFRAG_ENAB()) {
            if (PKTISFRAG(osh, p)) {
                cnt += PKTFRAGTOTNUM(osh, p);
            }
        }
#endif
    }

    return cnt;
}

/* count segments of a chained packet */
uint BCMFASTPATH
pktsegcnt_war(osl_t *osh, void *p)
{
    uint cnt;
    uint8 *pktdata;
    uint len, remain, align64;

    for (cnt = 0; p; p = PKTNEXT(osh, p)) {
        cnt++;
        len = PKTLEN(osh, p);
        if (len > 128) {
            pktdata = (uint8 *)PKTDATA(osh, p); /* starting address of data */
            /* Check for page boundary straddle (2048B) */
            if (((uintptr)pktdata & ~0x7ff) != ((uintptr)(pktdata+len) & ~0x7ff))
                cnt++;

            align64 = (uint)((uintptr)pktdata & 0x3f);  /* aligned to 64B */
            align64 = (64 - align64) & 0x3f;
            len -= align64;     /* bytes from aligned 64B to end */
            /* if aligned to 128B, check for MOD 128 between 1 to 4B */
            remain = len % 128;
            if (remain > 0 && remain <= 4)
                cnt++;      /* add extra seg */
        }
    }

    return cnt;
}

uint8 * BCMFASTPATH
pktdataoffset(osl_t *osh, void *p,  uint offset)
{
    uint total = pkttotlen(osh, p);
    uint pkt_off = 0, len = 0;
    uint8 *pdata = (uint8 *) PKTDATA(osh, p);

    if (offset > total)
        return NULL;

    for (; p; p = PKTNEXT(osh, p)) {
        pdata = (uint8 *) PKTDATA(osh, p);
        pkt_off = offset - len;
        len += PKTLEN(osh, p);
        if (len > offset)
            break;
    }
    return (uint8*) (pdata+pkt_off);
}


/* given a offset in pdata, find the pkt seg hdr */
void *
pktoffset(osl_t *osh, void *p,  uint offset)
{
    uint total = pkttotlen(osh, p);
    uint len = 0;

    if (offset > total)
        return NULL;

    for (; p; p = PKTNEXT(osh, p)) {
        len += PKTLEN(osh, p);
        if (len > offset)
            break;
    }
    return p;
}

/*
 * osl multiple-precedence packet queue
 * hi_prec is always >= the number of the highest non-empty precedence
 */
void * BCMFASTPATH
pktq_penq(struct pktq *pq, int prec, void *p)
{
    struct pktq_prec *q;

    ASSERT(prec >= 0 && prec < pq->num_prec);
    ASSERT(PKTLINK(p) == NULL);         /* queueing chains not allowed */

    ASSERT(!pktq_full(pq));
    ASSERT(!pktq_pfull(pq, prec));

    q = &pq->q[prec];

    if (q->head)
        PKTSETLINK(q->tail, p);
    else
        q->head = p;

    q->tail = p;
    q->len++;

    pq->len++;

    if (pq->hi_prec < prec)
        pq->hi_prec = (uint8)prec;

    return p;
}

void * BCMFASTPATH
pktq_penq_head(struct pktq *pq, int prec, void *p)
{
    struct pktq_prec *q;

    ASSERT(prec >= 0 && prec < pq->num_prec);
    ASSERT(PKTLINK(p) == NULL);         /* queueing chains not allowed */

    ASSERT(!pktq_full(pq));
    ASSERT(!pktq_pfull(pq, prec));

    q = &pq->q[prec];

    if (q->head == NULL)
        q->tail = p;

    PKTSETLINK(p, q->head);
    q->head = p;
    q->len++;

    pq->len++;

    if (pq->hi_prec < prec)
        pq->hi_prec = (uint8)prec;

    return p;
}

void * BCMFASTPATH
pktq_pdeq(struct pktq *pq, int prec)
{
    struct pktq_prec *q;
    void *p;

    ASSERT(prec >= 0 && prec < pq->num_prec);

    q = &pq->q[prec];

    if ((p = q->head) == NULL)
        return NULL;

    if ((q->head = PKTLINK(p)) == NULL)
        q->tail = NULL;

    q->len--;

    pq->len--;

    PKTSETLINK(p, NULL);

    return p;
}

void * BCMFASTPATH
pktq_pdeq_prev(struct pktq *pq, int prec, void *prev_p)
{
    struct pktq_prec *q;
    void *p;

    ASSERT(prec >= 0 && prec < pq->num_prec);

    q = &pq->q[prec];

    if (prev_p == NULL)
        return NULL;

    if ((p = PKTLINK(prev_p)) == NULL)
        return NULL;

    q->len--;

    pq->len--;

    PKTSETLINK(prev_p, PKTLINK(p));
    PKTSETLINK(p, NULL);

    return p;
}

void * BCMFASTPATH
pktq_pdeq_with_fn(struct pktq *pq, int prec, ifpkt_cb_t fn, int arg)
{
    struct pktq_prec *q;
    void *p, *prev = NULL;

    ASSERT(prec >= 0 && prec < pq->num_prec);

    q = &pq->q[prec];
    p = q->head;

    while (p) {
        if (fn == NULL || (*fn)(p, arg)) {
            break;
        } else {
            prev = p;
            p = PKTLINK(p);
        }
    }
    if (p == NULL)
        return NULL;

    if (prev == NULL) {
        if ((q->head = PKTLINK(p)) == NULL)
            q->tail = NULL;
    } else {
        PKTSETLINK(prev, PKTLINK(p));
    }

    q->len--;

    pq->len--;

    PKTSETLINK(p, NULL);

    return p;
}

void * BCMFASTPATH
pktq_pdeq_tail(struct pktq *pq, int prec)
{
    struct pktq_prec *q;
    void *p, *prev;

    ASSERT(prec >= 0 && prec < pq->num_prec);

    q = &pq->q[prec];

    if ((p = q->head) == NULL)
        return NULL;

    for (prev = NULL; p != q->tail; p = PKTLINK(p))
        prev = p;

    if (prev)
        PKTSETLINK(prev, NULL);
    else
        q->head = NULL;

    q->tail = prev;
    q->len--;

    pq->len--;

    return p;
}

void
pktq_pflush(osl_t *osh, struct pktq *pq, int prec, bool dir, ifpkt_cb_t fn, int arg)
{
    struct pktq_prec *q;
    void *p, *prev = NULL;

    q = &pq->q[prec];
    p = q->head;
    while (p) {
        if (fn == NULL || (*fn)(p, arg)) {
            bool head = (p == q->head);
            if (head)
                q->head = PKTLINK(p);
            else
                PKTSETLINK(prev, PKTLINK(p));
            PKTSETLINK(p, NULL);
            PKTFREE(osh, p, dir);
            q->len--;
            pq->len--;
            p = (head ? q->head : PKTLINK(prev));
        } else {
            prev = p;
            p = PKTLINK(p);
        }
    }

    if (q->head == NULL) {
        ASSERT(q->len == 0);
        q->tail = NULL;
    }
}

bool BCMFASTPATH
pktq_pdel(struct pktq *pq, void *pktbuf, int prec)
{
    struct pktq_prec *q;
    void *p;

    ASSERT(prec >= 0 && prec < pq->num_prec);

    /* XXX Should this just assert pktbuf? */
    if (!pktbuf)
        return FALSE;

    q = &pq->q[prec];

    if (q->head == pktbuf) {
        if ((q->head = PKTLINK(pktbuf)) == NULL)
            q->tail = NULL;
    } else {
        for (p = q->head; p && PKTLINK(p) != pktbuf; p = PKTLINK(p))
            ;
        if (p == NULL)
            return FALSE;

        PKTSETLINK(p, PKTLINK(pktbuf));
        if (q->tail == pktbuf)
            q->tail = p;
    }

    q->len--;
    pq->len--;
    PKTSETLINK(pktbuf, NULL);
    return TRUE;
}

void
pktq_init(struct pktq *pq, int num_prec, int max_len)
{
    int prec;

    ASSERT(num_prec > 0 && num_prec <= PKTQ_MAX_PREC);

    /* pq is variable size; only zero out what's requested */
    bzero(pq, OFFSETOF(struct pktq, q) + (sizeof(struct pktq_prec) * num_prec));

    pq->num_prec = (uint16)num_prec;

    pq->max = (uint16)max_len;

    for (prec = 0; prec < num_prec; prec++)
        pq->q[prec].max = pq->max;
}

void
pktq_set_max_plen(struct pktq *pq, int prec, int max_len)
{
    ASSERT(prec >= 0 && prec < pq->num_prec);

    if (prec < pq->num_prec)
        pq->q[prec].max = (uint16)max_len;
}

void * BCMFASTPATH
pktq_deq(struct pktq *pq, int *prec_out)
{
    struct pktq_prec *q;
    void *p;
    int prec;

    if (pq->len == 0)
        return NULL;

    while ((prec = pq->hi_prec) > 0 && pq->q[prec].head == NULL)
        pq->hi_prec--;

    q = &pq->q[prec];

    if ((p = q->head) == NULL)
        return NULL;

    if ((q->head = PKTLINK(p)) == NULL)
        q->tail = NULL;

    q->len--;

    pq->len--;

    if (prec_out)
        *prec_out = prec;

    PKTSETLINK(p, NULL);

    return p;
}

void * BCMFASTPATH
pktq_deq_tail(struct pktq *pq, int *prec_out)
{
    struct pktq_prec *q;
    void *p, *prev;
    int prec;

    if (pq->len == 0)
        return NULL;

    for (prec = 0; prec < pq->hi_prec; prec++)
        if (pq->q[prec].head)
            break;

    q = &pq->q[prec];

    if ((p = q->head) == NULL)
        return NULL;

    for (prev = NULL; p != q->tail; p = PKTLINK(p))
        prev = p;

    if (prev)
        PKTSETLINK(prev, NULL);
    else
        q->head = NULL;

    q->tail = prev;
    q->len--;

    pq->len--;

    if (prec_out)
        *prec_out = prec;

    PKTSETLINK(p, NULL);

    return p;
}

void *
pktq_peek(struct pktq *pq, int *prec_out)
{
    int prec;

    if (pq->len == 0)
        return NULL;

    while ((prec = pq->hi_prec) > 0 && pq->q[prec].head == NULL)
        pq->hi_prec--;

    if (prec_out)
        *prec_out = prec;

    return (pq->q[prec].head);
}

void *
pktq_peek_tail(struct pktq *pq, int *prec_out)
{
    int prec;

    if (pq->len == 0)
        return NULL;

    for (prec = 0; prec < pq->hi_prec; prec++)
        if (pq->q[prec].head)
            break;

    if (prec_out)
        *prec_out = prec;

    return (pq->q[prec].tail);
}

void
pktq_flush(osl_t *osh, struct pktq *pq, bool dir, ifpkt_cb_t fn, int arg)
{
    int prec;

    /* Optimize flush, if pktq len = 0, just return.
     * pktq len of 0 means pktq's prec q's are all empty.
     */
    if (pq->len == 0) {
        return;
    }

    for (prec = 0; prec < pq->num_prec; prec++)
        pktq_pflush(osh, pq, prec, dir, fn, arg);
    if (fn == NULL)
        ASSERT(pq->len == 0);
}

/* Return sum of lengths of a specific set of precedences */
int
pktq_mlen(struct pktq *pq, uint prec_bmp)
{
    int prec, len;

    len = 0;

    for (prec = 0; prec <= pq->hi_prec; prec++)
        if (prec_bmp & (1 << prec))
            len += pq->q[prec].len;

    return len;
}

/* Priority peek from a specific set of precedences */
void * BCMFASTPATH
pktq_mpeek(struct pktq *pq, uint prec_bmp, int *prec_out)
{
    struct pktq_prec *q;
    void *p;
    int prec;

    if (pq->len == 0)
    {
        return NULL;
    }
    while ((prec = pq->hi_prec) > 0 && pq->q[prec].head == NULL)
        pq->hi_prec--;

    while ((prec_bmp & (1 << prec)) == 0 || pq->q[prec].head == NULL)
        if (prec-- == 0)
            return NULL;

    q = &pq->q[prec];

    if ((p = q->head) == NULL)
        return NULL;

    if (prec_out)
        *prec_out = prec;

    return p;
}
/* Priority dequeue from a specific set of precedences */
void * BCMFASTPATH
pktq_mdeq(struct pktq *pq, uint prec_bmp, int *prec_out)
{
    struct pktq_prec *q;
    void *p;
    int prec;

    if (pq->len == 0)
        return NULL;

    while ((prec = pq->hi_prec) > 0 && pq->q[prec].head == NULL)
        pq->hi_prec--;

    while ((pq->q[prec].head == NULL) || ((prec_bmp & (1 << prec)) == 0))
        if (prec-- == 0)
            return NULL;

    q = &pq->q[prec];

    if ((p = q->head) == NULL)
        return NULL;

    if ((q->head = PKTLINK(p)) == NULL)
        q->tail = NULL;

    q->len--;

    if (prec_out)
        *prec_out = prec;

    pq->len--;

    PKTSETLINK(p, NULL);

    return p;
}

/** Wait for a particular clock level to be on the backplane */
uint32
si_pmu_waitforclk_on_backplane(si_t *sih, osl_t *osh, uint32 clk, uint32 delay_val)
{
    ASSERT(sih->cccaps & CC_CAP_PMU);

    if (delay_val)
        SPINWAIT(((R_REG(osh, PMUREG(sih, pmustatus)) & clk) != clk), delay_val);
    return (R_REG(osh, PMUREG(sih, pmustatus)) & clk);
}

/** returns value in [Hz] units */
uint32
BCMINITFN(si_alp_clock)(si_t *sih)
{
    if (PMUCTL_ENAB(sih))
        return si_pmu_alp_clock(sih, si_osh(sih));
    else if (BCM4707_CHIP(CHIPID(sih->chip))) {
        if (sih->chippkg == BCM4709_PKG_ID)
            return NS_ALP_CLOCK;
        else
            return NS_SLOW_ALP_CLOCK;
    }

    return ALP_CLOCK;
}

const unsigned char bcm_ctype[] = {

    _BCM_C,_BCM_C,_BCM_C,_BCM_C,_BCM_C,_BCM_C,_BCM_C,_BCM_C,            /* 0-7 */
    _BCM_C, _BCM_C|_BCM_S, _BCM_C|_BCM_S, _BCM_C|_BCM_S, _BCM_C|_BCM_S, _BCM_C|_BCM_S, _BCM_C,
    _BCM_C, /* 8-15 */
    _BCM_C,_BCM_C,_BCM_C,_BCM_C,_BCM_C,_BCM_C,_BCM_C,_BCM_C,            /* 16-23 */
    _BCM_C,_BCM_C,_BCM_C,_BCM_C,_BCM_C,_BCM_C,_BCM_C,_BCM_C,            /* 24-31 */
    _BCM_S|_BCM_SP,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,        /* 32-39 */
    _BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,            /* 40-47 */
    _BCM_D,_BCM_D,_BCM_D,_BCM_D,_BCM_D,_BCM_D,_BCM_D,_BCM_D,            /* 48-55 */
    _BCM_D,_BCM_D,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,            /* 56-63 */
    _BCM_P, _BCM_U|_BCM_X, _BCM_U|_BCM_X, _BCM_U|_BCM_X, _BCM_U|_BCM_X, _BCM_U|_BCM_X,
    _BCM_U|_BCM_X, _BCM_U, /* 64-71 */
    _BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,            /* 72-79 */
    _BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,            /* 80-87 */
    _BCM_U,_BCM_U,_BCM_U,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,            /* 88-95 */
    _BCM_P, _BCM_L|_BCM_X, _BCM_L|_BCM_X, _BCM_L|_BCM_X, _BCM_L|_BCM_X, _BCM_L|_BCM_X,
    _BCM_L|_BCM_X, _BCM_L, /* 96-103 */
    _BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L, /* 104-111 */
    _BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L, /* 112-119 */
    _BCM_L,_BCM_L,_BCM_L,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_C, /* 120-127 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     /* 128-143 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     /* 144-159 */
    _BCM_S|_BCM_SP, _BCM_P, _BCM_P, _BCM_P, _BCM_P, _BCM_P, _BCM_P, _BCM_P, _BCM_P, _BCM_P,
    _BCM_P, _BCM_P, _BCM_P, _BCM_P, _BCM_P, _BCM_P, /* 160-175 */
    _BCM_P, _BCM_P, _BCM_P, _BCM_P, _BCM_P, _BCM_P, _BCM_P, _BCM_P, _BCM_P, _BCM_P, _BCM_P,
    _BCM_P, _BCM_P, _BCM_P, _BCM_P, _BCM_P, /* 176-191 */
    _BCM_U, _BCM_U, _BCM_U, _BCM_U, _BCM_U, _BCM_U, _BCM_U, _BCM_U, _BCM_U, _BCM_U, _BCM_U,
    _BCM_U, _BCM_U, _BCM_U, _BCM_U, _BCM_U, /* 192-207 */
    _BCM_U, _BCM_U, _BCM_U, _BCM_U, _BCM_U, _BCM_U, _BCM_U, _BCM_P, _BCM_U, _BCM_U, _BCM_U,
    _BCM_U, _BCM_U, _BCM_U, _BCM_U, _BCM_L, /* 208-223 */
    _BCM_L, _BCM_L, _BCM_L, _BCM_L, _BCM_L, _BCM_L, _BCM_L, _BCM_L, _BCM_L, _BCM_L, _BCM_L,
    _BCM_L, _BCM_L, _BCM_L, _BCM_L, _BCM_L, /* 224-239 */
    _BCM_L, _BCM_L, _BCM_L, _BCM_L, _BCM_L, _BCM_L, _BCM_L, _BCM_P, _BCM_L, _BCM_L, _BCM_L,
    _BCM_L, _BCM_L, _BCM_L, _BCM_L, _BCM_L /* 240-255 */
};

ulong
BCMROMFN(bcm_strtoul)(const char *cp, char **endp, uint base)
{
    ulong result, last_result = 0, value;
    bool minus;

    minus = FALSE;

    while (bcm_isspace(*cp))
        cp++;

    if (cp[0] == '+')
        cp++;
    else if (cp[0] == '-') {
        minus = TRUE;
        cp++;
    }

    if (base == 0) {
        if (cp[0] == '0') {
            if ((cp[1] == 'x') || (cp[1] == 'X')) {
                base = 16;
                cp = &cp[2];
            } else {
                base = 8;
                cp = &cp[1];
            }
        } else
            base = 10;
    } else if (base == 16 && (cp[0] == '0') && ((cp[1] == 'x') || (cp[1] == 'X'))) {
        cp = &cp[2];
    }

    result = 0;

    while (bcm_isxdigit(*cp) &&
           (value = bcm_isdigit(*cp) ? *cp-'0' : bcm_toupper(*cp)-'A'+10) < base) {
        result = result*base + value;
        /* Detected overflow */
        if (result < last_result && !minus)
            return (ulong)-1;
        last_result = result;
        cp++;
    }

    if (minus)
        result = (ulong)(-(long)result);

    if (endp)
        *endp = DISCARD_QUAL(cp, char);

    return (result);
}

//#USBDevice#: Special printf function replacement used in ISR
#define USBDEV_ISR_DEBUG_LOGBUF_SIZE (1024)

int usbdev_isr_debug_printf(const char *format, ...)
{
    va_list args;
    static char printbuf[USBDEV_ISR_DEBUG_LOGBUF_SIZE];
    int len;

    va_start(args, format);
    len = vsnprintf(printbuf, USBDEV_ISR_DEBUG_LOGBUF_SIZE, format, args);
    if ( (len == -1) || (len >= 1024) )
    {
        /* Truncate if overflow */
        printbuf[USBDEV_ISR_DEBUG_LOGBUF_SIZE - 1] = '\0';
        len = (USBDEV_ISR_DEBUG_LOGBUF_SIZE - 1);
    }
    va_end(args);

    platform_stdio_exception_write( printbuf, len );

    return 0;
}

/* pretty hex print a contiguous buffer */
void
prhex(const char *msg, uchar *buf, uint nbytes)
{
    char line[128], *p;
    int len = sizeof(line);
    int nchar;
    uint i;

    if (msg && (msg[0] != '\0'))
        printf("%s:\n", msg);

    p = line;
    for (i = 0; i < nbytes; i++) {
        if (i % 16 == 0) {
            nchar = snprintf(p, len, "  %04d: ", i);    /* line prefix */
            p += nchar;
            len -= nchar;
        }
        if (len > 0) {
            nchar = snprintf(p, len, "%02x ", buf[i]);
            p += nchar;
            len -= nchar;
        }

        if (i % 16 == 15) {
            printf("%s\n", line);       /* flush line */
            p = line;
            len = sizeof(line);
        }
    }

    /* flush last partial line */
    if (p != line)
        printf("%s\n", line);
}

#define USB_ERROR_VAL   0x0001
#define USB_TRACE_VAL   0x0002
#define USB_DBG_VAL     0x0004

static uint32 usbdev_msg_level = USB_ERROR_VAL; /* Print messages even for non-debug drivers */
#define USB_ERR(args)       do {if (usbdev_msg_level & USB_ERROR_VAL) printf args;} while (0)

#ifdef BCMDBG
#define USB_TRACE(args)     do {if (usbdev_msg_level & USB_TRACE_VAL) printf args;} while (0)
#define USB_DBG(args)       do {if (usbdev_msg_level & USB_DBG_VAL) printf args;} while (0)
#else
#define USB_TRACE(args)
#define USB_DBG(args)
#endif

#ifdef BCMDBG
#define HIST_SIZE_USBDEV    (USB_NTXD * 2)
uint32 hist_rxc[HIST_SIZE_USBDEV];
uint32 hist_txq[HIST_SIZE_USBDEV];
uint32 hist_txq_dma[HIST_SIZE_USBDEV];
#endif

/* a given register value maps to a particular pll count */
typedef struct pll_ct {
    uint16 regval;
    uint16 pllct;
} pll_ct_t;

#define MAX_PLL_LOCK_CT 16
/* PLL lock count: how long it takes the PLL to lock */
static pll_ct_t BCMINITDATA(usbphy_pll_lock_ct)[MAX_PLL_LOCK_CT] = {
    {1,  300},
    {2,  400},
    {0,  500},
    {3,  600},
    {4,  700},
    {5,  800},
    {6,  900},
    {7,  1023},
    {8,  1300},
    {9,  1500},
    {10, 1800},
    {11, 2046},
    {12, 2300},
    {13, 2500},
    {14, 2800},
    {15, 4096}
};

#define MAX_PLL_RESET_CT 4
/* PLL reset count: controls how long reset should be issued to the PLL */
static pll_ct_t BCMINITDATA(usbphy_pll_reset_ct)[MAX_PLL_RESET_CT] = {
    {3,  200},
    {2,  150},
    {0,  100},
    {1,  50}
};

static void _ch_bulkrxfill(dngl_task_t *task);
static void _ch_resume_timer(dngl_task_t *task);


/** used during attach phase. It seems that this function has no good purpose. */
bool
ch_match(uint vendor, uint device)
{
    // trace("");
    if (vendor != VENDOR_BROADCOM)
        return FALSE;

    switch (device) {
    case BCM47XX_USBD_ID:
    case BCM47XX_USB20D_ID:
        return TRUE;
    }

    return FALSE;
}

/**
 * When workarounds are needed, or when the FLL frequency needs to be changed, low level write
 * access to USB PHY registers is required.
 */
void
ch_mdio_wreg(struct usbdev_sb *ch, uint16 addr, uint16 data)
{
    uint32 val, sel;
    osl_t *osh = ch->osh;

    sel = (HSIC_MDIO_SLAVE_ADDR << USB_MDIOCTL_ID_SHIFT) |
        (USB_MDIOCTL_SMSEL_CLKEN << USB_MDIOCTL_SMSEL_SHIFT);
    W_REG(osh, &ch->regs->mdio_ctl, sel);

    val = (addr << USB_MDIOCTL_REGADDR_SHIFT) | (data << USB_MDIOCTL_WRDATA_SHIFT) | sel;
    val |= USB_MDIOCTL_WR_EN;
    W_REG(osh, &ch->regs->mdio_ctl, val);

    /* Wait ~1.1us: 64 clks @ 60Mhz */
    OSL_DELAY(2);

    /* Need to repeat toggle write pulse */
    W_REG(osh, &ch->regs->mdio_ctl, sel);
    W_REG(osh, &ch->regs->mdio_ctl, val);

    OSL_DELAY(2);
}

uint16
ch_mdio_rreg(struct usbdev_sb *ch, uint16 addr)
{
    uint32 val, sel;
    osl_t *osh = ch->osh;

    sel = (HSIC_MDIO_SLAVE_ADDR << USB_MDIOCTL_ID_SHIFT) |
        (USB_MDIOCTL_SMSEL_CLKEN << USB_MDIOCTL_SMSEL_SHIFT);
    W_REG(osh, &ch->regs->mdio_ctl, sel);

    val = (addr << USB_MDIOCTL_REGADDR_SHIFT) | sel;
    val |= USB_MDIOCTL_RD_EN;
    W_REG(osh, &ch->regs->mdio_ctl, val);

    /* Wait ~1.1us: 64 clks @ 60Mhz */
    OSL_DELAY(2);

    return R_REG(osh, &ch->regs->mdio_data);
}

#if defined(BCMUSBDEV_COMPOSITE) && defined(USB_IFTEST)
void
ep_config_reset(struct usbdev_sb *ch)
{
    int i;

    for (i = 1; i < DMA_MAX; i++)
        ch->dmaintmask[i] = 0;
}

/** configure intr/iso endpoint registers */
/* is *not* called by anybody. Remove this ? */
void
ep_config(struct usbdev_sb *ch, const usb_endpoint_descriptor_t *endpoint,
    int config, int interface, int alternate)
{
    uint32 epinfo, ep_type;
    uint dma = 0;

    epinfo = EP_EN(UE_GET_ADDR(endpoint->bEndpointAddress)) |
        (UE_GET_XFERTYPE(endpoint->bmAttributes) << 5) |
        EP_MPS(endpoint->wMaxPacketSize) |
        EP_CF(config) | EP_IF(interface) | EP_AI(alternate);

    ep_type = UE_GET_XFERTYPE(endpoint->bmAttributes);
    if (ep_type == UE_INTERRUPT) {
        dma = 1;
        /* both WLAN and BT intr-ep configured */
        if (ch->dmaintmask[dma])
            dma = 2;
    } else if (ep_type == UE_BULK) {
        dma = 3;
        /* both WLAN and BT bulk-ep configured */
        if (ch->dmaintmask[dma])
            dma = 4;
    } else if (ep_type == UE_ISOCHRONOUS) {
        dma = 4;
    }

    if (dma) {
        W_REG(ch->osh, &ch->regs->epinfo[DMA2EP(dma, EP_DIR_IN)],
            epinfo | EP_DIR_IN);
        W_REG(ch->osh, &ch->regs->epinfo[DMA2EP(dma, EP_DIR_OUT)],
            (epinfo + 1) | EP_DIR_OUT);
        ch->dmaintmask[dma] = I_XI | I_RI;
    }
}
#endif /* BCMUSBDEV_COMPOSITE && USB_IFTEST */

/**
 * During initialization, or after a soft reset, or after the host selected an USB interface
 * configuration, endpoints associated with the desired configuration have to be initialized, so the
 * host can start communicating over these endpoints.
 *
 * Hardware associated with the caller supplied endpoint is initialized. A (bidirectional) DMA
 * engine is associated with the endpoint. For RX (host->device, OUT) DMA engines:
 *     a number of rx buffers (ch->tunables[NRXD], initialized with USB_NRXD) is posted.
 */
uint
ep_attach(struct usbdev_sb *ch, const usb_endpoint_descriptor_t *endpoint,
    int config, int interface, int alternate)
{
    int ep, /* physical endpoint */
        i;
    uint32 epinfo, dmaintmask;
    int rxpostbufs = 0;

    ep = 0;
    epinfo = EP_EN(UE_GET_ADDR(endpoint->bEndpointAddress)) |
        (UE_GET_XFERTYPE(endpoint->bmAttributes) << 5) |
        EP_MPS(endpoint->wMaxPacketSize) |
        EP_CF(config) | EP_IF(interface) | EP_AI(alternate);

#ifdef BCMDBG
    usbdev_isr_debug_printf("epinfo %x, Addr %x, MaxPktSize %d", epinfo,
        endpoint->bEndpointAddress, endpoint->wMaxPacketSize);
#endif

    /* Control endpoints require a whole DMA engine because they are bidirectional */
    if (UE_GET_XFERTYPE(endpoint->bmAttributes) == UE_CONTROL)
        dmaintmask = I_XI | I_RI;
    else if (UE_GET_DIR(endpoint->bEndpointAddress) == UE_DIR_IN)
        dmaintmask = I_XI;
    else if (UE_GET_DIR(endpoint->bEndpointAddress) == UE_DIR_OUT)
        dmaintmask = I_RI;
    else {
        ASSERT(0);  /* Must have been control, in or out */
        dmaintmask = 0;
    }

    /* Host can send synch command for Isochronous endpoints */
    if (UE_GET_XFERTYPE(endpoint->bmAttributes) == UE_ISOCHRONOUS &&
        UE_GET_ISO_TYPE(endpoint->bmAttributes) == UE_ISO_SYNC)
        OR_REG(ch->osh, &ch->regs->devcontrol, DC_SC);

    /* DEBUG: increase HS timeout ?
     * OR_REG(ch->osh, &ch->regs->devcontrol, DC_HSTC_MASK);
    */

    /* Find a free (half of a) DMA engine */
    for (i = 0; i < DMA_MAX; i++) {
        if (ch->dmaintmask[i] & dmaintmask)
            continue;

        /* Initialize DMA state */
        if (!ch->di[i]) {
            char name[8];

            snprintf(name, sizeof(name), "usbdma%d", i);
#ifdef BCMDBG
            usbdev_isr_debug_printf("ep_attach: init %s\n", name);
#endif

            /* Init TX ring only for interrupt endpoint */
            if (UE_GET_XFERTYPE(endpoint->bmAttributes) == UE_INTERRUPT) {
                ASSERT(UE_GET_DIR(endpoint->bEndpointAddress) == UE_DIR_IN);
                ch->di[i] = dma_attach(ch->osh, name, ch->sih,
                    USB20DDMAREG(ch, DMA_TX, i),
                    NULL,
                    ch->tunables[NTXD], 0, 0, -1, 0, USB_RXOFFSET, NULL);
                ASSERT(ch->di[i]);

                usbdev_isr_debug_printf("ep_attach: TX Only %s: di[%d]=0x%08x\n", name, i, (uint)ch->di[i]);
            } else {
                int rxbufsz, rxexthdrroom;

                rxpostbufs = (UE_GET_XFERTYPE(endpoint->bmAttributes)
                              == UE_CONTROL) ? ch->tunables[CTL_RXBUFS] :
                              ch->tunables[BULK_RXBUFS];

                /* PKTBUFSZ must be <= 4096+LBBUF for normal hndrte_lbuf.c to work
                 * new partition management can support MEM_PT_BLKSIZE
                 */
#ifndef HNDRTE_PT_GIANT
                ASSERT(ch->tunables[BULK_RXBUF_GIANT] < 4096);
#endif
                rxbufsz = (UE_GET_XFERTYPE(endpoint->bmAttributes) == UE_CONTROL) ?
                    ch->tunables[CTLBUFSZ] :
                    ((ch->tunables[BULK_RXBUF_GIANT] == 0) ?
                    PKTBUFSZ : ch->tunables[BULK_RXBUF_GIANT]);

                rxexthdrroom = -1;  /* hnddma.c default */
#if !defined(WLC_HIGH) && defined(WLC_LOW)
                rxexthdrroom = 0;   /* BMAC needs zero extra headroom */
#endif

                ch->di[i] = dma_attach(ch->osh, name, ch->sih,
                    USB20DDMAREG(ch, DMA_TX, i),
                    USB20DDMAREG(ch, DMA_RX, i),
                    ch->tunables[NTXD], ch->tunables[NRXD],
                    rxbufsz, rxexthdrroom,
                    rxpostbufs, USB_RXOFFSET, NULL);
                ASSERT(ch->di[i]);

#ifdef BCMDBG
                usbdev_isr_debug_printf("ep_attach: TX&RX %s: di[%d]=0x%08x\n", name, i, (uint)ch->di[i]);
#endif

                /* enable chained packet rx for large USB transfers */
                if (UE_GET_XFERTYPE(endpoint->bmAttributes) == UE_BULK) {
                    dma_ctrlflags(ch->di[i],
                                  DMA_CTRL_RXMULTI, DMA_CTRL_RXMULTI);
#ifdef BCMPKTPOOL
                    if (POOL_ENAB(ch->pktpool) &&
                        (rxbufsz <= pktpool_plen(ch->pktpool)))
                            dma_pktpool_set(ch->di[i], ch->pktpool);
#endif
                }
            }

            /* This value is used to set the burstlen for the DMA engine for revisions
             * which support the burstlen configuration. This does not affect the older
             * revisions of the DMA. This is required if the hardware default values
             * have to be overridden.
             */
            dma_burstlen_set(ch->di[i], ch->tunables[RXBURSTLEN],
                             ch->tunables[TXBURSTLEN]);

            ch->txavail[i] = (uint *) dma_getvar(ch->di[i], "&txavail");
            if (rxpostbufs)
                dma_rxparam_get(ch->di[i], (uint16*)&ch->rxoffset[i],
                                (uint16*)&(ch->rxbufsize[i]));
#ifdef BCMDBG
            usbdev_isr_debug_printf("di %d rxbufsize %d rxoffset %d\n", i, ch->rxbufsize[i],
                ch->rxoffset[i]);
#endif

            /* Both endpoints have to be the same type */
            W_REG(ch->osh, &ch->regs->epinfo[DMA2EP(i, EP_DIR_OUT)],
                  UE_GET_XFERTYPE(endpoint->bmAttributes) << 5);
            W_REG(ch->osh, &ch->regs->epinfo[DMA2EP(i, EP_DIR_IN)],
                  UE_GET_XFERTYPE(endpoint->bmAttributes) << 5);
            ch->flowctl_rx[i] = FALSE;
        }

        /* Initialize rx side of DMA engine */
        if (dmaintmask & I_RI) {
            /* Both endpoints have to be the same type */
            if ((R_REG(ch->osh, &ch->regs->epinfo[DMA2EP(i, EP_DIR_IN)]) &
                 EP_TYPE_MASK) != (epinfo & EP_TYPE_MASK))
                continue;

            if (UE_GET_XFERTYPE(endpoint->bmAttributes) == UE_BULK) {
                ch->bulkoutdma = i;
                ch->bulkrxfillct = rxpostbufs;
            }

            dma_rxreset(ch->di[i]);
            dma_rxinit(ch->di[i]);
            ch_rxfill(ch, i);

            ep = DMA2EP(i, EP_DIR_OUT);
            W_REG(ch->osh, &ch->regs->intrcvlazy[i], IRL_FC(1));
            W_REG(ch->osh, &ch->regs->epinfo[ep], epinfo | EP_DIR_OUT);
        }

        /* Initialize tx side of DMA engine */
        if (dmaintmask & I_XI) {
            /* Both endpoints have to be the same type */
            if ((R_REG(ch->osh, &ch->regs->epinfo[DMA2EP(i, EP_DIR_OUT)]) &
                 EP_TYPE_MASK) != (epinfo & EP_TYPE_MASK))
                continue;

            dma_txreset(ch->di[i]);
            dma_txinit(ch->di[i]);

            /* XXX PR4850 WAR: clear the tx FIFO */

            ep = DMA2EP(i, EP_DIR_IN);
            W_REG(ch->osh, &ch->regs->epinfo[ep], epinfo | EP_DIR_IN);
        }

        OR_REG(ch->osh, &ch->regs->dmaint[i].mask, I_ERRORS | dmaintmask);
        ch->dmaintmask[i] |= I_ERRORS | dmaintmask;
        break;
    }
    ASSERT(i < DMA_MAX);

    //trace("done");
    return ep;
} /* ep_attach */

/**
 * On init or reset, the USB configuration changes. Therefore endpoints associated to the 'old'
 * configuration should no longer be available to the host. This function deactivates the hardware
 * associated with the caller supplied endpoint.
 */
void
ep_detach(struct usbdev_sb *ch, int ep)
{
    uint32 i = EP2DMA(ep);
    uint32 epinfo = R_REG(ch->osh, &ch->regs->epinfo[ep]);
    uint32 dir = epinfo & EP_DIR_MASK;
    uint32 type = epinfo & EP_TYPE_MASK;
    uint32 dmaintmask = ch->dmaintmask[i];
    void *p;

    // trace("");
    /* Disable endpoint */
    W_REG(ch->osh, &ch->regs->epinfo[ep], 0);

    /* Reset rx side of DMA engine */
    if (dir == EP_DIR_OUT || type == EP_CONTROL) {
        if (ch->di[i]) {
            dma_rxreset(ch->di[i]);
            dma_rxreclaim(ch->di[i]);
        }

        /* Flush rx queue */
        while ((p = pktdeq(&ch->rxq[i])))
            PKTFREE(ch->osh, p, FALSE);

        dmaintmask &= ~I_RI;
        W_REG(ch->osh, &ch->regs->intrcvlazy[i], 0);
    }

    /* Reset tx side of DMA engine */
    if (dir == EP_DIR_IN || type == EP_CONTROL) {
        if (ch->di[i]) {
            dma_txreset(ch->di[i]);
            dma_txreclaim(ch->di[i], HNDDMA_RANGE_ALL);
        }

        /* Flush tx queue */
        while ((p = pktdeq(&ch->txq[i])))
            PKTFREE(ch->osh, p, TRUE);

        ch->txavail[i] = NULL;
        dmaintmask &= ~I_XI;
    }

    if (!(dmaintmask & (I_XI | I_RI))) {
        dmaintmask = 0;
        if (ch->di[i])
            dma_detach(ch->di[i]);
        ch->di[i] = NULL;
    }

    W_REG(ch->osh, &ch->regs->dmaint[i].mask, dmaintmask);
    ch->dmaintmask[i] = dmaintmask;

    /* rev >= 3 */
    if (ep == 0 && ch->setupdma) {
        if (ch->di[SETUP_DMA]) {
            dma_rxreset(ch->di[SETUP_DMA]);
            dma_rxreclaim(ch->di[SETUP_DMA]);
            dma_detach(ch->di[SETUP_DMA]);
        }
        ch->di[SETUP_DMA] = NULL;
    }
    trace("done");
} /* ep_detach */

/**
 * A specific USB20d core revision requires this function during USB disconnect.
 * For ADFLL:
 *  If target freq in PLL control 5 is not default 37.4Mhz,
 *  then update freq tgt via mdio interface
 */
static void
ch_hsic_fll_update(struct usbdev_sb *ch)
{
    uint32 freq;
    uint16 div;

    freq = si_pmu_pllcontrol(ch->sih, PMU15_PLL_PLLCTL5, 0, 0);
    freq &= PMU15_PLL_PC5_FREQTGT_MASK;
    if ((freq == PMU15_FREQTGT_480_DEFAULT) || (freq == PMU15_FREQTGT_492_DEFAULT)) {
        /* Don't need to update PLL control 5 */
        return;
    }

    /* Put into extended write mode to access register extensions */
    ch_mdio_wreg(ch, HSIC_MDIO_REG_TST_CTL2, HSIC_MDIO_REG_TST_CTL2_XWR_EN);

    div = ch_mdio_rreg(ch, HSIC_MDIO_REGEX_DIV_R0);
    div &= ~HSIC_MDIO_REGEX_DIV_R0_FREQ_MASK;
    div |= (freq >> 16);

    ch_mdio_wreg(ch, HSIC_MDIO_REGEX_DIV_R0, div);
    ch_mdio_wreg(ch, HSIC_MDIO_REGEX_DIV_R1, (freq & HSIC_MDIO_REGEX_DIV_R1_FREQ_MASK));

    /* Toggle core enable */
    ch_mdio_wreg(ch, HSIC_MDIO_REGEX_CTL0, HSIC_MDIO_REGEX_CTL0_RST);
    OSL_DELAY(100);
    ch_mdio_wreg(ch, HSIC_MDIO_REGEX_CTL0, HSIC_MDIO_REGEX_CTL0_DIS);
    OSL_DELAY(100);
}

#ifdef BCM_BOOTLOADER

/**
 * In case a bug in the bootloader causes the USB PHY interface to be misconfigured, a workaround
 * is possible that makes use of NVRAM (OTP) variables.
 */
static void
ch_mdio_reglist_update(struct usbdev_sb *ch)
{
    int i;
    uint16 extmode = 0;
    uint32 otp_val;
    const char *otp_str;
    char mdiodesc[12];

    for (i = 0; i < USB_MDIO_ADDR_MAX; i++) {
        snprintf(mdiodesc, sizeof(mdiodesc), "mdio%d", i);
        if ((otp_str = getvar(NULL, mdiodesc)) != NULL) {
            otp_val = (uint32)bcm_strtoul(otp_str, NULL, 0);
            ch_mdio_wreg(ch, i, otp_val);
        }
    }

    for (i = 0; i < USB_MDIO_ADDR_MAX; i++) {
        snprintf(mdiodesc, sizeof(mdiodesc), "mdioex%d", i);
        if ((otp_str = getvar(NULL, mdiodesc)) != NULL) {
            if (extmode == 0) {
                extmode = ch_mdio_rreg(ch, HSIC_MDIO_REG_TST_CTL2);
                extmode |= HSIC_MDIO_REG_TST_CTL2_XWR_EN;

                /* Enter extended write mode to access register extensions */
                ch_mdio_wreg(ch, HSIC_MDIO_REG_TST_CTL2, extmode);
            }

            otp_val = (uint32)bcm_strtoul(otp_str, NULL, 0);
            ch_mdio_wreg(ch, i, otp_val);
        }
    }

    if (extmode != 0) {
        extmode &= ~HSIC_MDIO_REG_TST_CTL2_XWR_EN;

        /* Exit extended write mode */
        ch_mdio_wreg(ch, HSIC_MDIO_REG_TST_CTL2, extmode);
    }
}
#endif /* BCM_BOOTLOADER */

void ch_waitfor_ht(struct usbdev_sb *ch, bool request_ht, const char *fn)
{
    /* XXX interrupts do not propagate if there is no clock
     * on the backplane.  We get around this by forcing HT clock
     * on the backplane at all times (ILP & ALP does not work).
     * Also, the core cannot meet USB timing unless HT is available.
     * Further, our USB implementation requires backplane clock >= 90MHz,
     * meaning HT ON THE BACKPLANE.
     */
    if (request_ht)
        OR_REG(ch->osh, &ch->regs->clkctlstatus, CCS_FORCEHT);
    if (CHIPID(sih->chip) == BCM43909_CHIP_ID) {
        if (si_pmu_waitforclk_on_backplane(ch->sih, ch->osh, PST_APPS_SBCLKST_HT,
                                           PMU_MAX_TRANSITION_DLY) != PST_APPS_SBCLKST_HT) {
            err("%s: HT is not on the backplane!", fn);
            ASSERT(0);
        }
    } else {
        if (si_pmu_waitforclk_on_backplane(ch->sih, ch->osh, PST_SBCLKST_HT,
                                           PMU_MAX_TRANSITION_DLY) != PST_SBCLKST_HT) {
            err("%s: HT is not on the backplane!", fn);
            ASSERT(0);
        }
    }
}


/** used for USB20d core (re)initialization. Sole caller is ch_init(). */
uint32
#ifdef HARD_DISCONNECT
ch_reset(struct usbdev_sb *ch)
#else
BCMATTACHFN(ch_reset)(struct usbdev_sb *ch)
#endif
{
    char *otp_str;
    uint32 devcontrol;
    uint32 utmi_ctl = 0x02000000; /* USB PHY initialization */
    uint32 delay;

    //dbg_printf("@@@ch_reset: +++\n");
    dbg_printf("@@@ch_reset: chipid=%u, chiprev=%u\n", CHIPID(ch->sih->chip), CHIPREV(ch->sih->chiprev));

    /* PR10028 WAR: Disable currently selected USB core first */
    si_core_disable(ch->sih, si_core_cflags(ch->sih, 0, 0));

    /* Reset core */
    si_core_reset(ch->sih, 0, 0);

    /* Must wait at least 400 ns after core reset */
    OSL_DELAY(1);

    if (PMUCTL_ENAB(ch->sih))
        ch_waitfor_ht(ch, TRUE, __FUNCTION__);

    dbg_printf("@@@ch_reset: init 40nm USBPHY\n");

    /* init 40nm USBPHY */
    if (ch->rev >= 14 && !ch->hsic) {
        switch (CHIPID(ch->sih->chip)) {
        case BCM4360_CHIP_ID:
        case BCM4352_CHIP_ID:
        case BCM43526_CHIP_ID:
            if (CHIPREV(ch->sih->chiprev) <= 2) {
                /* chip rev a0..a2 */
                utmi_ctl = 0;
            } /* fall through */
        case BCM43143_CHIP_ID: /* fall through */
        case BCM43242_CHIP_ID:
        case BCM43243_CHIP_ID:
        case BCM4345_CHIP_ID:
        case BCM4350_CHIP_ID:
        case BCM4354_CHIP_ID:
        case BCM43556_CHIP_ID:
        case BCM43558_CHIP_ID:
        case BCM43566_CHIP_ID:
        case BCM43568_CHIP_ID:
        case BCM43569_CHIP_ID:
            /* keep utmi_ctl at 0x02000000 */
            break;
        case BCM43909_CHIP_ID:
            utmi_ctl = 0x7f80000;
            break;
        default:
            /*
             * If the assert below triggers, check if new chip has
               a 40nm USB phy. If so, add a 'case' statement. If not,
               extend the if statement above.
             */
            ASSERT(0);
            break;
        }

        /* UTMI/LDO Control OTP override */
        if (ch->got_utmi_ctl)
            utmi_ctl = ch->utmi_ctl;
#if defined(BCM_BOOTLOADER) && defined(BCMTCAM)
        tcam_stub_pre();
#endif /* BCM_BOOTLOADER && BCMTCAM */
        /* default setup for 40nm usbphy */
        if (BCM4350_CHIP(ch->sih->chip)) {
            /* 4350 has new SSPHY, provide OTP overrides */
            utmi_ctl = 0xd000;
            if ((otp_str = getvar(NULL, "usbssphy_utmi_ctl0")) != NULL)
                utmi_ctl = (uint32)(bcm_strtoul(otp_str, NULL, 0));
            W_REG(ch->osh, &ch->regs->utmi_ctl, utmi_ctl);

            utmi_ctl = 0x500c;
            if ((otp_str = getvar(NULL, "usbssphy_utmi_ctl1")) != NULL)
                utmi_ctl = (uint32)(bcm_strtoul(otp_str, NULL, 0));
            W_REG(ch->osh, &ch->regs->utmi_ctl, utmi_ctl);

            delay = 100;
            if ((otp_str = getvar(NULL, "usbssphy_sleep0")) != NULL)
                delay = (uint32)(bcm_strtoul(otp_str, NULL, 0));
            OSL_DELAY(delay);

            utmi_ctl = 0x500d;
            if ((otp_str = getvar(NULL, "usbssphy_utmi_ctl2")) != NULL)
                utmi_ctl = (uint32)(bcm_strtoul(otp_str, NULL, 0));
            W_REG(ch->osh, &ch->regs->utmi_ctl, utmi_ctl);

            delay = 150;
            if ((otp_str = getvar(NULL, "usbssphy_sleep1")) != NULL)
                delay = (uint32)(bcm_strtoul(otp_str, NULL, 0));
            OSL_DELAY(delay);
        } else {
            W_REG(ch->osh, &ch->regs->utmi_ctl, utmi_ctl | 0x5000);
            /* power up LDO */
            W_REG(ch->osh, &ch->regs->utmi_ctl, utmi_ctl | 0x5003);
            OSL_DELAY(70);
            /* disable PHY_ISO/IDDQ */
            W_REG(ch->osh, &ch->regs->utmi_ctl, utmi_ctl | 0xd003);
            OSL_DELAY(125);
            /* take PLL out of reset */
            W_REG(ch->osh, &ch->regs->utmi_ctl, utmi_ctl | 0xd007);
            /* PLL lock delay + RCAL delay */
            OSL_DELAY(485);
        }
    }
#if defined(BCM_BOOTLOADER) && defined(BCMTCAM)
    tcam_stub_post();
#endif /* BCM_BOOTLOADER && BCMTCAM */
    devcontrol = R_REG(ch->osh, &ch->regs->devcontrol);

    /* XXX 4325A0 hack: set ULPI for chip that by definition is ULPI only */
    if ((ch->rev == 4) && (ch->id == USB20D_CORE_ID)) {
        devcontrol |= DC_UL;
        W_REG(ch->osh, &ch->regs->devcontrol, devcontrol);
        OSL_DELAY(1);
        (void) R_REG(ch->osh, &ch->regs->devcontrol);
    }

    if (ch->id == USB20D_CORE_ID) {
        /* need to set the PLL lock and PLL reset counts to conform to LPM timing */
        if (ch->rev >= 9 && ch->hsic) {
            uint32 alp_khz = si_alp_clock(ch->sih);
            uint32 pll_ct, hsicphyctrl1, lock_and_reset;
            int i;
            if (alp_khz) {
                alp_khz /= 1000;

                /* look up PLL lock count register val in table */
                /* find value greater than (35us / (1/xtal_freq_in_mhz)) */
                pll_ct = 3500000 / (100000000 / alp_khz);
                for (i = 0; i < MAX_PLL_LOCK_CT; ++i) {
                    if (usbphy_pll_lock_ct[i].pllct > pll_ct ||
                        i == (MAX_PLL_LOCK_CT - 1))
                        break;
                }
                lock_and_reset =
                    usbphy_pll_lock_ct[i].regval << PLL_LOCK_CT_SHIFT;

                /* look up PLL reset count register val in table */
                /* find value less than (5us / (1/xtal_freq_in_mhz)) */
                pll_ct = 500000 / (100000000 / alp_khz);
                for (i = 0; i < MAX_PLL_RESET_CT; ++i) {
                    if (usbphy_pll_reset_ct[i].pllct < pll_ct ||
                        i == (MAX_PLL_RESET_CT - 1))
                        break;
                }
                lock_and_reset |=
                    usbphy_pll_reset_ct[i].regval << PLL_RESET_CT_SHIFT;

                /* clear existing bits */
                hsicphyctrl1 = R_REG(ch->osh, &ch->regs->hsicphyctrl1) &
                    ~(PLL_LOCK_CT_MASK | PLL_RESET_CT_MASK);

                /* write the new bits */
                hsicphyctrl1 |= lock_and_reset;
                hsicphyctrl1 |= (1 << HSIC_PULLDISABLE_SHIFT);

                if (ch->usbflags) {
                    if (USB_PULLENABLE_ENAB(ch->usbflags))
                        hsicphyctrl1 &= ~(1 << HSIC_PULLDISABLE_SHIFT);
                }
                W_REG(ch->osh, &ch->regs->hsicphyctrl1, hsicphyctrl1);
                hsicphyctrl1 = R_REG(ch->osh, &ch->regs->hsicphyctrl1);
                dbg_printf("alp_khz %d; hsicphyctrl1 0x%x", alp_khz, hsicphyctrl1);
            }
            /* If the HSICPhyCtrl needs to be overridden using the OTP,
             * it is checked for presence of Tuple in the OTP and the register
             * is set accordingly.
             */
            if (ch->got_hsicphyctrl1) {
                W_REG(ch->osh, &ch->regs->hsicphyctrl1, ch->hsicphyctrl1);
                (void)R_REG(ch->osh, &ch->regs->hsicphyctrl1);
            }
            if (ch->got_hsicphyctrl2) {
                W_REG(ch->osh, &ch->regs->hsicphyctrl2, ch->got_hsicphyctrl2);
                (void)R_REG(ch->osh, &ch->regs->hsicphyctrl2);
            }
        }
#if defined(BCM_BOOTLOADER) && defined(BCMTCAM)
        tcam_stub_pre();
#endif /* BCM_BOOTLOADER && BCMTCAM */
        /* Power up PLL, UTMI and Analog */
        /* PR22173 : clear soft disconnect */
        /* PR24585 : clear SYNCH_FRAME support */
        devcontrol &= ~(DC_PL | DC_UP | DC_AP | DC_DC | DC_SC);
        W_REG(ch->osh, &ch->regs->devcontrol, devcontrol);
        (void) R_REG(ch->osh, &ch->regs->devcontrol);

        /* PR 62546: Need wait longer after powerup USB */
        OSL_DELAY(1000);    /* wait 1ms */

        /* Set UTMISoftReset */
        /* PR77323 WAR: "Use clkgateAsyncEn on the 60MHz phy clock path" */
        if (ch->rev >= 9) {
            devcontrol |= DC_UR;
            W_REG(ch->osh, &ch->regs->devcontrol, devcontrol);
            (void) R_REG(ch->osh, &ch->regs->devcontrol);
            OSL_DELAY(400);
        }

        /* Clear Phy reset */
        devcontrol &= ~DC_PR;
        W_REG(ch->osh, &ch->regs->devcontrol, devcontrol);
        (void) R_REG(ch->osh, &ch->regs->devcontrol);
        OSL_DELAY(200);

        /* Default is High Speed */
        devcontrol &= ~DC_SS_MASK;
        /* nvram FS mode override */
        if (ch->fsmode)
            devcontrol |= DC_SS(DC_SS_FS);
        W_REG(ch->osh, &ch->regs->devcontrol, devcontrol);
        (void) R_REG(ch->osh, &ch->regs->devcontrol);
    } else {
        /* USB 1.1: Clear PLL reset */
        devcontrol &= ~DC_PL;
        W_REG(ch->osh, &ch->regs->devcontrol, devcontrol);
        (void) R_REG(ch->osh, &ch->regs->devcontrol);
    }

    /* XXX PR62546: Need wait longer after powerup USB */
    OSL_DELAY(1000);    /* wait 1ms */

    /* XXX JIRA HW4350-62: 4350 SSPHY HW limitation: time interval between
     * ~DC_RS and DC_US should be < 50us
     * move usbdev_reset before ~DC_RS to reduce interval
     */
    if (BCM4350_CHIP(ch->sih->chip)) {
        /* Reset device endpoints */
        dbg_printf(" call usbdev_reset");
        usbdev_reset(ch->bus);
    }

    /* Clear device reset */
    devcontrol &= ~DC_RS;

    W_REG(ch->osh, &ch->regs->devcontrol, devcontrol);
    (void) R_REG(ch->osh, &ch->regs->devcontrol);

    if ((ch->rev >= 1) || (ch->id == USB20D_CORE_ID)) {
        if (BCM4350_CHIP(ch->sih->chip)) {
            delay = 0;
            if ((otp_str = getvar(NULL, "usbssphy_sleep2")) != NULL)
                delay = (uint32)(bcm_strtoul(otp_str, NULL, 0));
            OSL_DELAY(delay);
        } else {
            /* XXX PR62546: Need wait longer after powerup USB */
            OSL_DELAY(1000);    /* wait 1ms */
        }

        /* Clear UTMISoftReset */
        /* PR77323 WAR: "Use clkgateAsyncEn on the 60MHz phy clock path" */
        if (ch->rev >= 9) {
            devcontrol &= ~DC_UR;
            W_REG(ch->osh, &ch->regs->devcontrol, devcontrol);
            (void) R_REG(ch->osh, &ch->regs->devcontrol);
            OSL_DELAY(1);
        }

        /* Clear app reset */
        devcontrol &= ~DC_AR;
        W_REG(ch->osh, &ch->regs->devcontrol, devcontrol);
        (void) R_REG(ch->osh, &ch->regs->devcontrol);

    }

    /* Write command address register */
    W_REG(ch->osh, &ch->regs->commandaddr, ~0);

    #if (USB_DEVICE_TEST_RDL)
    if (!BCM4350_CHIP(ch->sih->chip)) {
        /* Reset device endpoints */
        dbg_printf(" call usbdev_reset");
        usbdev_reset(ch->bus);
    }
    #endif

    //dbg_printf("@@@ch_reset: ---\n");

    return devcontrol;
} /* ch_reset */

/**
 * Reinitialize device from any state, initializes all USB channels. After calling this function,
 * only endpoint 0 is available to the host. Called during dongle initialization and fatal USBd core
 * error recovery.
 */
void
#ifdef HARD_DISCONNECT
ch_init(struct usbdev_sb *ch, bool disconnect)
#else
BCMATTACHFN(ch_init)(struct usbdev_sb *ch, bool disconnect)
#endif
{
/* LDOBG voltage is controlled by utmi_ctl1[26:23] */
#define LDOBG_MASK (0x0f << 23)
#define LDOBG_FIELD(_v) ((_v) << 23)
    uint32 devcontrol;
    uint32 lpmcontrol;
    int i;
    uint32 epinfo;
    uint32 delay;
    char *otp_str;
    usb_endpoint_descriptor_t default_endpoint = {
        bLength: USB_ENDPOINT_DESCRIPTOR_SIZE,
        bDescriptorType: UDESC_ENDPOINT,
        bEndpointAddress: 0,
        bmAttributes: UE_CONTROL,
        wMaxPacketSize: 0,
        bInterval: 0
    };

    //dbg_printf("@@@ch_init: +++\n");
    UNUSED_PARAMETER(default_endpoint);
    UNUSED_PARAMETER(lpmcontrol);

    /*
     * HT must be on the backplane; the processor should have requested it but it may
     * not be there yet due to ramp up from sleep mode, so we wait for it.
     */
    if (PMUCTL_ENAB(ch->sih))
        ch_waitfor_ht(ch, FALSE, __FUNCTION__);
#if defined(BCM_BOOTLOADER) && defined(BCMTCAM)
        tcam_stub_pre();
#endif /* BCM_BOOTLOADER && BCMTCAM */
        if (disconnect) {
            devcontrol = ch_reset(ch);
            if (BCM4350_CHIP(ch->sih->chip)) {
                if (!ch->hsic) {
                    /* Clear "self power" bit...
                     * add SP BFL bit if we ever do a SP device
                     */
                    devcontrol &= ~DC_SP;
                } else
                    devcontrol |= DC_SP;

                W_REG(ch->osh, &ch->regs->devcontrol, devcontrol);
                (void) R_REG(ch->osh, &ch->regs->devcontrol);

                /* Enable LPM for USB20d core rev 9 and higher */
#if (USB_LPM_SUPPORT)
                if ((ch->id == USB20D_CORE_ID) && (ch->rev >= 9)) {
                    devcontrol |= DC_LPM;
                    W_REG(ch->osh, &ch->regs->devcontrol, devcontrol);
                    (void) R_REG(ch->osh, &ch->regs->devcontrol);

                    lpmcontrol = DEF_LPMCONTROL | LPM_INT_THRESH;
                    W_REG(ch->osh, &ch->regs->lpmcontrol, lpmcontrol);
                    (void) R_REG(ch->osh, &ch->regs->lpmcontrol);
                    ch->lpmsleep = FALSE;
                    ch->lpmminmask = 0x0;
                }
#endif

                if (ch->got_usbdevctrl) {
                    W_REG(ch->osh, &ch->regs->devcontrol, ch->usbdevctrl);
                    devcontrol = R_REG(ch->osh, &ch->regs->devcontrol);
                }

                delay = 0;
                if ((otp_str = getvar(NULL, "usbssphy_sleep3")) != NULL)
                    delay = (uint32)(bcm_strtoul(otp_str, NULL, 0));
                OSL_DELAY(delay);

                ch_mdio_wreg(ch, 0x0, 0x0);

                devcontrol |= DC_US;
                W_REG(ch->osh, &ch->regs->devcontrol, devcontrol);
                (void) R_REG(ch->osh, &ch->regs->devcontrol);
            }
    #ifdef HT_POWERSAVE
            /* need HT for proper reconnect */
            if (ch->disconnected)
                ch_waitfor_ht(ch, TRUE, __FUNCTION__);
    #endif /* HT_POWERSAVE */
            if (ch->rev == 13)
                ch_hsic_fll_update(ch);

    #ifdef BCM_BOOTLOADER
            if (ch->rev >= 10)
                ch_mdio_reglist_update(ch);
    #endif
        } else {
            dbg_printf("no disconnect\n");

            switch (CHIPID(ch->sih->chip)) {
            case BCM4360_CHIP_ID:
            case BCM43526_CHIP_ID:
                if (CHIPREV(ch->sih->chiprev) == 2) {
                    uint32 utmi_ctl; /* USB PHY initialization */

                    /* chip rev a2 */
                    /* LDOBG voltage is controlled by utmi_ctl1[26:23] */
                    devcontrol = R_REG(ch->osh, &ch->regs->utmi_ctl);
                    utmi_ctl = (ch->regs->utmi_ctl &
                        (~LDOBG_MASK)) | LDOBG_FIELD(0);
                    W_REG(ch->osh, &ch->regs->utmi_ctl, utmi_ctl);
                }
                break;

            default:
                break;
            }

    #ifndef HT_POWERSAVE
            if (PMUCTL_ENAB(ch->sih))
                ch_waitfor_ht(ch, TRUE, __FUNCTION__);
    #endif
            devcontrol = R_REG(ch->osh, &ch->regs->devcontrol);
        }
#if defined(BCM_BOOTLOADER) && defined(BCMTCAM)
        tcam_stub_post();
#endif /* BCM_BOOTLOADER && BCMTCAM */
    for (i = 0; i < DMA_MAX; ++i) {
        ch->dmaintmask[i] = 0;
        ch->ctrloutexpected[i].tag = 0xff;
    }

    default_endpoint.wMaxPacketSize = usbdev_mps(ch->bus);

    #if (USB_DEVICE_TEST_RDL)
    /* Reset default endpoint */
    dbg_printf(" call ep_attach");
    ep_detach(ch, 0);
    ep_attach(ch, &default_endpoint, 0, 0, 0);
    #endif

    /* rev 3 or higher */
    if (ch->setupdma) {
        char name[8];
        snprintf(name, sizeof(name), "usbdma%d", SETUP_DMA);

        if (!ch->di[SETUP_DMA]) {
            ch->di[SETUP_DMA] = dma_attach(ch->osh, name, ch->sih, NULL,
                USB20DREV_LT(ch->rev, 7) ?
                (volatile void *)&ch->regs->sddmaregs.rcv :
                (volatile void *)&ch->regs->dma64regs[SETUP_DMA].dmarcv,
                0, SETUP_DMA_DEPTH * 2,
                (USB_DEVICE_REQUEST_SIZE + USB_RXOFFSET + 1), -1,
                SETUP_DMA_DEPTH, USB_RXOFFSET, NULL);
        }
        if (USB20DREV_GE(ch->rev, 7) && USB20DREV_LE(ch->rev, 10)) {
            dma_ctrlflags(ch->di[SETUP_DMA], DMA_CTRL_USB_BOUNDRY4KB_WAR,
                DMA_CTRL_USB_BOUNDRY4KB_WAR);
        }
        ASSERT(ch->di[SETUP_DMA]);

        /* This value is used to set the burstlen for the DMA engine for revisions
         * which support the burstlen configuration. This does not affect the older
         * revisions of the DMA. This is required if the hardware default values
         * have to be overridden.
         */
        dma_burstlen_set(ch->di[SETUP_DMA], ch->tunables[RXBURSTLEN],
                         ch->tunables[TXBURSTLEN]);

        dma_rxreset(ch->di[SETUP_DMA]);
        dma_rxreclaim(ch->di[SETUP_DMA]);
        dma_rxinit(ch->di[SETUP_DMA]);
        dma_rxfill(ch->di[SETUP_DMA]);
        W_REG(ch->osh, &ch->regs->sdintrcvlazy, IRL_FC(1));
        OR_REG(ch->osh, &ch->regs->sdintmask, I_ERRORS | I_RI);
    }

    /*
     * PR24862/24863 WAR: Race between SetCfg int and 1st Setup to config'd
     * ep: preconfig ep's so they will at least NAK packets directed to them
     * before SetCfg is detected and processed.
     */
    for (i = 1; i < DMA_MAX; ++i) {
        epinfo = EP_EN(UE_GET_ADDR(i)) |
                (UE_GET_XFERTYPE(UE_BULK) << 5) |
                 EP_MPS(64) |
                 EP_CF(1) | EP_IF(1) | EP_AI(0);
        W_REG(ch->osh, &ch->regs->epinfo[DMA2EP(i, EP_DIR_IN)], epinfo | EP_DIR_IN);
        W_REG(ch->osh, &ch->regs->epinfo[DMA2EP(i, EP_DIR_OUT)], epinfo | EP_DIR_OUT);
    }

#if defined(BCM_BOOTLOADER) && defined(USB_IFTEST)
     if (!ch->hsic) {
        /*
        Enable the Interrupt endpoint to handle basic endpoint requests.
        (Set feature / Clear feature)
        */
        epinfo = EP_EN(UE_GET_ADDR(1)) |
        (UE_GET_XFERTYPE(UE_INTERRUPT)<<5) |
        EP_MPS(16) |
        EP_CF(1) |
        EP_IF(1) |
        EP_AI(0);
        W_REG(ch->osh, &ch->regs->epinfo[DMA2EP(1, EP_DIR_IN)], epinfo | EP_DIR_IN);
        W_REG(ch->osh, &ch->regs->epinfo[DMA2EP(1, EP_DIR_OUT)], epinfo | EP_DIR_OUT);

        /*
         * Avoid the usage of DMA 1 for Bulk endpoint DMA,
         * as we dont allocate separate DMA for Interrupt endpoint.
         */
        ch->dmaintmask[1] = I_RI | I_XI;

#ifdef BCMUSBDEV_COMPOSITE
        /* config for iso endpoints */
        i = 4;
        epinfo = EP_EN(UE_GET_ADDR(i)) |
            (UE_GET_XFERTYPE(UE_ISOCHRONOUS)<<5) |
            EP_MPS(1024) |
            EP_CF(1) | EP_IF(2) | EP_AI(0);
        W_REG(ch->osh, &ch->regs->epinfo[DMA2EP(i, EP_DIR_IN)], epinfo | EP_DIR_IN);
        W_REG(ch->osh, &ch->regs->epinfo[DMA2EP(i, EP_DIR_OUT)], epinfo | EP_DIR_OUT);
        ch->dmaintmask[i] = I_RI | I_XI;
#endif /* BCMUSBDEV_COMPOSITE */
    }
#endif /* BCM_BOOTLOADER && USB_IFTEST */

    ch->up = TRUE;

    /* XXX must be multiple of 4, POR default is 0x100, max is (0x200-xx)
     *   TODO tuning, The bigger the number, the few data underflow error(ch_txrecompose)
     * Experiment shows on 4322usb, under stress run,
     *   < 0x1a0, dozens of errors per second
     *   0x1a0, dozens of errors per minute
     *   0x1b0, a few errors per minute
     */
    if ((ch->rev == 5) && (ch->id == USB20D_CORE_ID)) {
        W_REG(ch->osh, &ch->regs->txfifowtermark, 0x1b0);
    }

    /* Enable all USB interrupts */
    ch->usbintmask = DEF_USBINTMASK;

    W_REG(ch->osh, &ch->regs->usbintmask, ch->usbintmask);
#if defined(BCM_BOOTLOADER) && defined(BCMTCAM)
    tcam_stub_pre();
#endif /* BCM_BOOTLOADER && BCMTCAM */
    if (!BCM4350_CHIP(ch->sih->chip)) {
        if (!ch->hsic) {
            /* Clear "self power" bit...add SP BFL bit if we ever do a SP device */
            devcontrol &= ~DC_SP;
        } else
            devcontrol |= DC_SP;
        W_REG(ch->osh, &ch->regs->devcontrol, devcontrol);
        (void) R_REG(ch->osh, &ch->regs->devcontrol);

        /* Enable LPM for USB20d core rev 9 and higher */
#if (USB_LPM_SUPPORT)
        if ((ch->id == USB20D_CORE_ID) && (ch->rev >= 9)) {
            devcontrol |= DC_LPM;
            W_REG(ch->osh, &ch->regs->devcontrol, devcontrol);
            (void) R_REG(ch->osh, &ch->regs->devcontrol);

            lpmcontrol = DEF_LPMCONTROL | LPM_INT_THRESH;
            W_REG(ch->osh, &ch->regs->lpmcontrol, lpmcontrol);
            (void) R_REG(ch->osh, &ch->regs->lpmcontrol);
            ch->lpmsleep = FALSE;
            ch->lpmminmask = 0x0;
        }
#endif

        if (!disconnect) {
            if ((CHIPID(ch->sih->chip) == BCM4360_CHIP_ID) ||
                (CHIPID(ch->sih->chip) == BCM43526_CHIP_ID)) {
                if (CHIPREV(ch->sih->chiprev) == 2) {
                    ch_mdio_wreg(ch, 0x0, 0x10);
                }
            }
            return;
        }

        if (ch->got_usbdevctrl) {
            W_REG(ch->osh, &ch->regs->devcontrol, ch->usbdevctrl);
            devcontrol = R_REG(ch->osh, &ch->regs->devcontrol);
        }

        /* need to wait at least 7ms before signaling CONNECT */
        OSL_DELAY(7 * 1000);

        if (((CHIPID(ch->sih->chip) == BCM4360_CHIP_ID) ||
            (CHIPID(ch->sih->chip) == BCM43526_CHIP_ID)) &&
            (CHIPREV(ch->sih->chiprev) == 2)) {
            ch_mdio_wreg(ch, 0x0, 0x10);
        }
        else if ((CHIPID(ch->sih->chip) == BCM4360_CHIP_ID) ||
            (CHIPID(ch->sih->chip) == BCM4352_CHIP_ID) ||
            (CHIPID(ch->sih->chip) == BCM43526_CHIP_ID) ||
            BCM4350_CHIP(ch->sih->chip) ||
            0) {
            ch_mdio_wreg(ch, 0x0, 0x0);
        }

        dbg_printf("USB set DeviceReady\n");

        /* Let the USB bus know we are here */
        devcontrol |= DC_US;
        W_REG(ch->osh, &ch->regs->devcontrol, devcontrol);
        (void) R_REG(ch->osh, &ch->regs->devcontrol);
#if defined( CYW43907_USB20D_HW_SEND_ZLP )
        /* Enable USB20D to send ZLP packet on DMA completion. */
        AND_REG(ch->osh, &ch->regs->workaround, ~USB20D_WAR_DISABLE_HW_ZLP);
        ch->drv_ctrl_flags |= CH_CTRL_HW_ZLP_WAR;
#endif
    }
#if defined(BCM_BOOTLOADER) && defined(BCMTCAM)
    tcam_stub_post();
#endif /* BCM_BOOTLOADER && BCMTCAM */

    //dbg_printf("@@@ch_init: ---\n");
} /* ch_init */

#ifdef BCMDBG
/** serial console supports 'usbsdump' command for debug builds */
void
do_usbdevdump_cmd(uint32 arg, uint argc, char *argv[])
{
    uint i;
    struct usbdev_sb *ch = (struct usbdev_sb *)(uintptr)arg;

    printf("usbdev_sb module\n");

    for (i = 0; i < DMA_MAX; ++i) {
        printf("dma %d flowctl_rx %d flowctl_tx %d txpend %d\n",
            i, ch->flowctl_rx_cnt[i], ch->flowctl_tx_cnt[i], ch->txpktpend[i]);
    }

    printf("rxc hist: ");
    for (i = 0; i < HIST_SIZE_USBDEV; i++)
        if (hist_rxc[i])
            printf("%d: %d ", i, hist_rxc[i]);
    printf("\n");
    printf("txq hist: ");
    for (i = 0; i < HIST_SIZE_USBDEV; i++)
        if (hist_txq[i])
            printf("%d: %d ", i, hist_txq[i]);
    printf("\n");
    printf("txq_dma hist: ");
    for (i = 0; i < HIST_SIZE_USBDEV; i++)
        if (hist_txq_dma[i])
            printf("%d: %d ", i, hist_txq_dma[i]);
    printf("\n");

    if (strcmp(argv[1], "clear") == 0) {
        bzero(hist_rxc, HIST_SIZE_USBDEV * sizeof(uint32));
        bzero(hist_txq, HIST_SIZE_USBDEV * sizeof(uint32));
        bzero(hist_txq_dma, HIST_SIZE_USBDEV * sizeof(uint32));
        return;
    }
}
#endif  /* BCMDBG */

/**
 * USBd hardware and software have several compile time parameters that can be fine tuned for a
 * specific chip or for a build.
 */
static void
BCMATTACHFN(ch_tunables_init)(struct usbdev_sb *ch)
{
    ch->tunables[CTL_RXBUFS] = USBCTL_RXBUFS;
    ch->tunables[BULK_RXBUFS] = USBBULK_RXBUFS; /* # DMA rxpost buffers for data traffic */
    ch->tunables[BULK_RXBUF_GIANT] = USBBULK_RXBUF_GIANT;
    ch->tunables[NTXD] = USB_NTXD;
    ch->tunables[NRXD] = USB_NRXD;
    ch->tunables[CTLBUFSZ] = USBCTLBUFSZ;
    ch->tunables[TXQ_LEN] = USB_TXQ_LEN;
    ch->tunables[RXQ_LEN] = USB_RXQ_LEN;
    ch->tunables[DATAHIWAT] = USB_TXQ_DATAHIWAT;
    ch->tunables[DATALOWAT] = USB_TXQ_DATALOWAT;
    ch->tunables[RXBND] = USB_RXBND;
    ch->tunables[RXBURSTLEN] = D64_USBBURSTLEN;
    ch->tunables[TXBURSTLEN] = D64_USBBURSTLEN;
}

#if 0
static const char BCMATTACHDATA(rstr_usbfs)[] = "usbfs";
static const char BCMATTACHDATA(rstr_hsicphyctrl1)[] = "hsicphyctrl1";
static const char BCMATTACHDATA(rstr_hsicphyctrl2)[] = "hsicphyctrl2";
static const char BCMATTACHDATA(rstr_usbdevctrl)[] = "usbdevctrl";
static const char BCMATTACHDATA(rstr_usbflags)[] = "usbflags";
static const char BCMATTACHDATA(rstr_usbutmi_ctl)[] = "usbutmi_ctl";
#endif /* #if UNUSED_GLOBAL_VARIABLE */

/**
 * As part of initialization, data structures have to be allocated and initialized.
 * Does not initialize the USB20d core.
 */
struct usbdev_sb *
BCMATTACHFN(ch_attach)(void *drv, uint vendor, uint device, osl_t *osh, void *regs, uint bus)
{
    struct usbdev_sb *ch;
    int i;
    char *vars;
    uint vars_len;
    char *otp_str;
    void *return_regs;

    //dbg_printf("@@@ch_attach: +++\n");

    if (!ch_match(vendor, device))
        return NULL;

    /* Allocate chip state */
    if (!(ch = MALLOCZ_PERSIST(osh, sizeof(struct usbdev_sb)))) {
        err("out of memory, malloced %d bytes", MALLOCED(osh));
        return NULL;
    }
    //#USBDevice#: We porting MALLOCZ_PERSIST to MALLOC, which is w/o bzero inside!!!
    /* Have to reset this memory after malloc */
    bzero((char *)ch, sizeof(struct usbdev_sb));

    ch->drv = drv;
    ch->osh = osh;
    ch->regs = regs;
#ifndef BCMUSB_NODISCONNECT
    ch->disconnect = 1;
#endif
#ifdef BCMPKTPOOL
    ch->pktpool = SHARED_POOL;
#endif /* BCMPKTPOOL */
#ifdef MSGTRACE
    ch->msgtrace = TRUE;
#endif
#ifdef LOGTRACE
    ch->logtrace = TRUE;
#endif
    ch_tunables_init(ch);

    /* Initialize packet queues */
    for (i = 0; i < DMA_MAX; i++) {
        pktqinit(&ch->txq[i], ch->tunables[TXQ_LEN]);
        pktqinit(&ch->rxq[i], ch->tunables[RXQ_LEN]);
        ch->txpktpend[i] = 0;
        ch->txpktpend_bytes[i] = 0;
    }

    /* Attach to bus */
    if (!(ch->sih = si_attach(device, osh, regs, bus, NULL, &vars, &vars_len)))
        goto fail;

    if (BUSTYPE(bus) == PCI_BUS)
        si_pci_setup(ch->sih, (1 << si_coreidx(ch->sih)));

    //#USBDevice#: Need to do si_setcore right after si_attach!!!
    if ((return_regs = si_setcore(ch->sih, USB20D_CORE_ID, 0)) == NULL) {
        err("@@@ch_attach: Could not setcore to USB20D\n");
        goto fail;
    }
    if ((void *)(ch->regs) != return_regs) {
        err("@@@ch_attach: Could not setcore to USB20D\n");
        goto fail;
    }

    ch->id = si_coreid(ch->sih);
    ch->rev = si_corerev(ch->sih);
    dbg_printf("@@@ch_attach: id=0x%x, rev=%u\n", ch->id, ch->rev);

    if ((ch->id == USB20D_CORE_ID) && (ch->rev > 2))
        ch->setupdma = TRUE;
    else
        /* XXX tx data must be word aligned for all USB11D & USB20D rev <= 2 */
        ch->needtxaligned = TRUE;

    /* core is dma64 capable. (corerev >= 7) */
    if ((ch->id == USB20D_CORE_ID) && (ch->rev > 6))
        ch->dma64cap = TRUE;

    ch->fatal = (I_PC | I_PD | I_DE | I_RO);

    /* determine if operating in HSIC mode */
    #if (USB_DEVICE_TEST_RDL)
    if ((si_core_sflags(ch->sih, SFLAG_HSIC, 0) & SFLAG_HSIC) == SFLAG_HSIC)
        ch->hsic = 1;
    #endif

    /* Initialize device */
    #if (USB_DEVICE_TEST_RDL)
    if (!(ch->bus = usbdev_attach(osh, ch, ch->sih)))
        goto fail;
    #endif

    /* Check if force using Full-Speed mode */
#ifdef USB_DEVICE_FS_MODE_ONLY
    ch->fsmode = TRUE;
    dbg_printf("%s: FS mode only!\n", __FUNCTION__);
#endif

    /* cache nvram values for register overrides */
    if ((otp_str = getvar(NULL, rstr_hsicphyctrl1)) != NULL) {
        ch->hsicphyctrl1 = (uint32)bcm_strtoul(otp_str, NULL, 0);
        ch->got_hsicphyctrl1 = TRUE;
    }
    if ((otp_str = getvar(NULL, rstr_hsicphyctrl2)) != NULL) {
        ch->hsicphyctrl2 = (uint32)bcm_strtoul(otp_str, NULL, 0);
        ch->got_hsicphyctrl2 = TRUE;
    }
    if ((otp_str = getvar(NULL, rstr_usbdevctrl)) != NULL) {
        ch->usbdevctrl = (uint32)bcm_strtoul(otp_str, NULL, 0);
        ch->got_usbdevctrl = TRUE;
    }
    if ((otp_str = getvar(NULL, rstr_usbflags)) != NULL)
        ch->usbflags = (uint32)bcm_strtoul(otp_str, NULL, 0);

    /* UTMI/LDO Control OTP override (high 16 bits) */
    if ((otp_str = getvar(NULL, rstr_usbutmi_ctl)) != NULL) {
        ch->utmi_ctl = (uint32)(bcm_strtoul(otp_str, NULL, 0) << 16);
        ch->got_utmi_ctl = TRUE;
    }

    //ch->bulkrxfill = dngl_init_timer(ch, NULL, _ch_bulkrxfill);
    //ch->resume_timer = dngl_init_timer(ch, NULL, _ch_resume_timer);
    (void )(_ch_bulkrxfill);
    (void )(_ch_resume_timer);


    //dbg_printf("@@@ch_attach: ---\n");
    return ch;

fail:
    if (ch)
        MFREE_PERSIST(ch->osh, ch, sizeof(struct usbdev_sb));
    return NULL;
} /* ch_attach */

/** Reset and free chip private state */
void
BCMATTACHFN(ch_detach)(struct usbdev_sb *ch, bool disable)
{
    //dbg_printf("@@@ch_detach: +++\n");

    ch->up = FALSE;

    #if (USB_DEVICE_TEST_RDL)
    /* Reset endpoints */
    usbdev_reset(ch->bus);
    ep_detach(ch, 0);

    /* Free device state */
    usbdev_detach(ch->bus);
    #endif

    /* stop requesting HT */
    if (PMUCTL_ENAB(ch->sih)) {
        AND_REG(ch->osh, &ch->regs->clkctlstatus, ~CCS_FORCEHT);
        /* will spin for PMU_MAX_TRANSITION_DLY if another core is asserting HT */
        SPINWAIT((R_REG(ch->osh, &ch->regs->clkctlstatus) & CCS_HTAVAIL),
                 PMU_MAX_TRANSITION_DLY);
    }

    /* Put the core back into reset */
    if (disable)
        si_core_disable(ch->sih, 0);

    /* Detach from SB bus */
    si_detach(ch->sih);

    /* Free chip state */
    MFREE_PERSIST(ch->osh, ch, sizeof(struct usbdev_sb));

    dbg_printf("@@@ch_detach: completed!\n");
    //dbg_printf("@@@ch_detach: ---\n");
}

#ifndef BCM_BOOTLOADER

/** hide 'struct usbdev_sb' internals for callers outside of this file. Returns dongle handle. */
void *
ch_dngl(struct usbdev_sb *ch)
{
    return usbdev_dngl(ch->bus);
}

/** hide 'struct usbdev_sb' internals for callers outside of this file */
struct dngl_bus *
ch_bus(struct usbdev_sb *ch)
{
    return ch->bus;
}

#endif /* !BCM_BOOTLOADER */

/**
 * If the host selects a USB configuration ('n'), the hardware needs to be configured to present the
 * desired USB configuration to the host.
 */
void
ch_setcfg(struct usbdev_sb *ch, int n)
{
    uint bmAttributes;
    uint32 devcontrol = R_REG(ch->osh, &ch->regs->devcontrol);

    // trace("");

    /* Enable all endpoints for this configuration */
    bmAttributes = usbdev_setcfg(ch->bus, n);

    /* Enable additional devcontrol options for this configuration */
    if (bmAttributes & UC_SELF_POWERED)
        devcontrol |= DC_SP;
    if (bmAttributes & UC_REMOTE_WAKEUP)
        devcontrol |= DC_RW;
    W_REG(ch->osh, &ch->regs->devcontrol, devcontrol);

    //trace("done");
}

#ifdef BCMDBG_ERR
static void
ch_txq_dump(struct usbdev_sb *ch, int i)
{
    void *p;
    uint cnt = pktq_len(&ch->txq[i]);
    uint j;

    for (j = 0; j < cnt; j++) {
        p = pktdeq(&ch->txq[i]);

        USB_ERR(("%s q[%d] pkt len %d\n", __FUNCTION__, i, pkttotlen(ch->osh, p)));

        pktenq(&ch->txq[i], p);
    }
}
#endif /* BCMDBG_ERR */

/* XXX from experiment, keep DMA busy, flowcontrol whenever DMA is overbusy */
#define USB_DMA_RING_BYTES_WATERMARK    15000

/**
 * in order to send the next item in the tx queue (a software entity) towards the host (IN data),
 * the next buffer has to be dequeued and appended to the transmit DMA ring.
 * Called by ch_tx() and ch_dpc(). Each p could have multiple segments inside (chained pkts)
 */
void
ch_sendnext(struct usbdev_sb *ch, int i)
{
    int ep = DMA2EP(i, EP_DIR_IN);
    void *p;
    bool ischain = FALSE;
    int cnt = 0;
    uint len = 0;

    /* !!! constantly check "real time" DMA availability after each queuing */
    while (*ch->txavail[i] > 0 && (p = pktdeq(&ch->txq[i]))) {

        ischain = (PKTNEXT(ch->osh, p) != NULL);
        if (ischain) {
            /* if txavail is not enough, requeue to head to avoid drop by dma_txfast */
            cnt = pktsegcnt(ch->osh, p);
            if (*ch->txavail[i] < cnt) {
#ifdef BCMDBG_ERR
                ch->stuckcnt++;
                if (ch->stuckcnt == 10)
                    ch_txq_dump(ch, i);
#endif

                /* avoid same msg flooding if usb tx is stuck */
                if ((ch->pendpkt != ch->txpktpend[i]) &&
                    (ch->pendbyte != ch->txpktpend_bytes[i])) {
                    USB_DBG(("%s: dma[%d] full, newpktseg %d"
                        "pending pkt %d bytes %d\n", __FUNCTION__, i, cnt,
                        ch->txpktpend[i], ch->txpktpend_bytes[i]));
                    ch->pendpkt = ch->txpktpend[i];
                    ch->pendbyte = ch->txpktpend_bytes[i];
                }

                pktenq_head(&ch->txq[i], p);
                break;
            }
        }

#ifdef BCMDBG_ERR
        ch->stuckcnt = 0;
#endif
        //prhex("Tx", PKTDATA(ch->osh, p), PKTLEN(ch->osh, p));

        len = pkttotlen(ch->osh, p);
        ch->txpktpend[i]++;
        ch->txpktpend_bytes[i] += len;

#ifdef BCMDBG
        if (i == 2) {   /* XXX DBG only, msg flooding could timeout USB */
            USB_DBG(("+%d ", ch->txpktpend[i]));
        }
#endif

        if (dma_txfast(ch->di[i], p, TRUE)) {
            err("dma_txfast error!");
        }
    }

    /* check the txq watermarks to activate/deactive upper layer tx flowcontrol */
    cnt = pktq_len(&ch->txq[i]);
    if ((cnt >= USB_TXQ_DATAHIWAT) && (ch->flowctl_tx[i] == FALSE)) {
        ch->flowctl_tx[i] = TRUE;
        usbdev_txstop(ch->bus, ep);
        ch->flowctl_tx_cnt[i]++;
    } else if ((cnt <= USB_TXQ_DATALOWAT) && (ch->flowctl_tx[i] == TRUE)) {
        ch->flowctl_tx[i] = FALSE;
        usbdev_txstart(ch->bus, ep);
    }

    //trace("done");
} /* ch_sendnext */

/** Enqueue IN data (so transmit to host) */
int
ch_tx(struct usbdev_sb *ch, int ep, void *p)
{
    int i = EP2DMA(ep);

    if (!ch->di[i]) {
        usbdev_isr_debug_printf("ep%d: tx detached", ep);
        PKTFREE(ch->osh, p, TRUE);
        return FALSE;
    }

    if (ch->suspended) {
        usbdev_isr_debug_printf("ep%d: device suspended", ep);
        PKTFREE(ch->osh, p, TRUE);
        return FALSE;
    }

    if (ch->needtxaligned && !ISALIGNED(PKTDATA(ch->osh, p), 4)) {
        uint len = PKTLEN(ch->osh, p);
        uint offset = (uint) PKTDATA(ch->osh, p) & 3;
        //uchar *src, *dst;

        //src = PKTDATA(ch->osh, p);
        //dst = PKTPUSH(ch->osh, p, offset);
        PKTDATA(ch->osh, p);
        PKTPUSH(ch->osh, p, offset);
        PKTSETLEN(ch->osh, p, len);
        //ASSERT(ISALIGNED(dst, 4));

        //while (len--)
        //  *dst++ = *src++;
    }

    /* XXX - FIXME: don't drop special control packets (should use pkt flag) */
    if (pktq_full(&ch->txq[i]) &&
        ntoh16(((struct ether_header *)PKTDATA(ch->drv, p))->ether_type)
        != ETHER_TYPE_BRCM) {
        usbdev_isr_debug_printf("ep%d: txq full", ep);
        PKTFREE(ch->osh, p, TRUE);
        return FALSE;
    } else {
        pktenq(&ch->txq[i], p);
        ch_sendnext(ch, i);
    }

    //trace("done");
    return TRUE;
} /* ch_tx */

/**
 * Forward a USB packet received from the host to the higher USB layer (the device handler) for
 * processing.
 * NOTE: this function assumes there is only ONE pending pkt in rxq except partial one
 *   due to rxrecompose, where the completed partial pkt and newly received partial pkt
 *   could present in rxq. For recompose case, the partial pkts(chains) will be chained together
 */
void
ch_sendup(struct usbdev_sb *ch, int ep)
{
    usbdev_sb_rxh_t *rxh = NULL, *rxh0;
    void *p = NULL, *ptail = NULL, *p0 = NULL, *p0tail = NULL;
    int i = EP2DMA(ep);
    uint16 flags = 0;

    usbdev_isr_debug_printf("@ch_sendup: ep%d", ep);

    /* Reconstitute packet */
    while ((p0 = pktdeq(&ch->rxq[i]))) {
        /* Strip off rx header */
        rxh0 = (usbdev_sb_rxh_t *) PKTDATA(ch->osh, p0);
        PKTPULL(ch->osh, p0, sizeof(usbdev_sb_rxh_t));
        ASSERT(ISALIGNED(rxh0, 4));

        hex("rx", PKTDATA(ch->osh, p0), PKTLEN(ch->osh, p0));

        flags = ltoh16(rxh0->flags);
        /* PR22175:  discard Bad packet */
        if (flags & RXF_BAD) {
            /* XXX RXF_BAD & 0 len is expected behavior from our chips */
            if (rxh0->len != 0)
                usbdev_isr_debug_printf("ep%d: bad packet", ep);
            /* XXX Increment bad packet counter */
            PKTFREE(ch->osh, p0, FALSE);
            continue;
        }

        if (!p) {
            p = p0;
            ptail = pktlast(ch->osh, p);
            rxh = rxh0;
        } else {
            int plen = PKTLEN(ch->osh, ptail);
            int p0len = PKTLEN(ch->osh, p0);

            /* Data Error occurred; append rest of partial packet */

            p0tail = pktlast(ch->osh, p0);
            if (p0len > PKTTAILROOM(ch->osh, ptail) || (p0 != p0tail)) {
                /* chain the new data to the old */
                PKTSETNEXT(ch->osh, ptail, p0);
                usbdev_isr_debug_printf("ch_sendup: p %p +p0 %p(len %d) totlen %d\n",
                    p, p0, pkttotlen(ch->osh, p0), pkttotlen(ch->osh, p));
            } else {
                /* bcopy(PKTDATA(ch->osh, p0), PKTDATA(ch->osh, p) + plen, p0len);
                 * PKTSETLEN(ch->osh, p, plen + p0len);
                 */
                bcopy(PKTDATA(ch->osh, p0), PKTDATA(ch->osh, ptail) + plen, p0len);
                PKTSETLEN(ch->osh, ptail, plen + p0len);
                PKTFREE(ch->osh, p0, FALSE);
            }
            /* Update the ptail pointer */
            ptail = pktlast(ch->osh, p);
#if 0 /* old version to handle non-chain pkts */
            if (p0len > PKTTAILROOM(ch->osh, p)) {
                void *p1;
                /* Alloc a packet that will fit all the data */
                if ((p1 = PKTGET(ch->osh, plen + p0len, FALSE)) == NULL) {
                    usbdev_isr_debug_printf("PKTGET pkt size %d + headroom %d failed",
                        plen, p0len);
                    PKTFREE(ch->osh, p, TRUE);
                    return;
                }
                /* Transfer priority */
                PKTSETPRIO(p1, PKTPRIO(p));
                bcopy(PKTDATA(ch->osh, p), PKTDATA(ch->osh, p1), plen);
                PKTFREE(ch->osh, p, FALSE);
                p = p1;
            }
            bcopy(PKTDATA(ch->osh, p0), PKTDATA(ch->osh, p) + plen, p0len);
            PKTSETLEN(ch->osh, p, plen + p0len);
            PKTFREE(ch->osh, p0, FALSE);
#endif  /* obsolete */
        }
    }
    if (!p || !rxh)
        return;

    /* Setup packet */
    if (flags & RXF_SETUP) {
        int err = 0;
        int dir;
        uint32 epstatus = R_REG(ch->osh, &ch->regs->epstatus);

        //ASSERT((epinfo & EP_TYPE_MASK) == EP_CONTROL);
        ASSERT(ltoh16(rxh->len) == 8);

        /* Pass setup packet to device */
        #if (USB_DEVICE_TEST_RDL)
        p0 = usbdev_setup(ch->bus, ep, p, &err, &dir);
        #else
        UNUSED_PARAMETER(dir);
        UNUSED_PARAMETER(err);
        UNUSED_PARAMETER(p0);
        #endif
        PKTFREE(ch->osh, p, FALSE);

        /* Error processing request */
        if (!p0) {
            if (err) {
                /* Stall the Control transfer */
                W_REG(ch->osh, &ch->regs->epstatus, epstatus | EPS_STALL(ep));
            }
        } else if (p0 != p) {
            /* Control IN: Request processed and data to send */

            /* Setup interrupt or successful tx disables or stops tx engine */
            if (dma_txstopped(ch->di[i]) || !dma_txenabled(ch->di[i])) {
                /* Re-enable tx engine */
                dma_txreset(ch->di[i]);
                dma_txreclaim(ch->di[i], HNDDMA_RANGE_ALL);
                dma_txinit(ch->di[i]);
            }

            /* Post tx data */
            ch_tx(ch, ep, p0);
        }
        else if (!err) {
                OR_REG(ch->osh, &ch->regs->epstatus, EPS_DONE(ep));
        }
    } else {    /* Other packet */
#ifdef BCMDBG_MEM
#ifdef _HNDRTE_
        ASSERT(lb_sane(p));
#endif
#endif  /* BCMDBG_MEM */

        /* PR23814/23816 WAR: SW sees status OUT packet if prev IN was multiple of MPS */
        if (((PKTLEN(ch->osh, p) == 0) && (ep == 0)) &&
            (((ch->id == USB11D_CORE_ID) && (ch->rev < 2)) ||
             ((ch->id == USB20D_CORE_ID) && (ch->rev < 2))))
            PKTFREE(ch->osh, p, FALSE);
        else {
            /* Send up packet */
            #if (USB_DEVICE_TEST_RDL)
            usbdev_rx(ch->bus, ep, p);
            #else
            //#USBDevice#: Send up packet in other functions.
            #endif
        }
    }

    //trace("done");
} /* ch_sendup */

/**
 * The USBd core can signal a data error per endpoint, for instance when it doesn't receive an
 * ACK from the host. If that happens, the endpoints DMA engine stops. This function brings the DMA
 * engine back into a functional state by resetting it and re-evaluating the tx and rx queues.
 *
 * Parameter 'ep' is a physical endpoint.
 */
void
ch_dma_dataerr(struct usbdev_sb *ch, int ep)
{
    /* EndPointConfigX registers contain ep direction, max packet size, etc per endpoint */
    uint32 epinfo = R_REG(ch->osh, &ch->regs->epinfo[ep]);
    /*
     * EpBytesNoErrorX registers contain the number of bytes transmitted or received without
     * error on the endpoint.
     */
    uint32 offset = R_REG(ch->osh, &ch->regs->epbytes[ep]);
    int i = EP2DMA(ep);

    /* Determine whether the data error belongs to an IN or OUT endpoint  */
    /* and then take appropriate action to resolve the problem.  For end- */
    /* points other than EP0, the transfer direction (IN/OUT) can be      */
    /* inferred from the epinfo register.  Since EP0 is a bidirectional   */
    /* endpoint, it can do both IN and OUT transfers; however, the epinfo */
    /* register always reports EP0 as an IN endpoint.  To circumvent the  */
    /* ambiguity in transfer direction for EP0, we check the Rx DMA       */
    /* engine if it is enabled but stopped, and if not, then we check the */
    /* Tx DMA engine if it is enabled but stopped. This allows handling   */
    /* of EP0 data errors in both the Tx & Rx path.                       */

    if ((((epinfo & EP_DIR_MASK) == EP_DIR_OUT) || (ep == 0)) &&
        ((ch->dmaintmask[i]) & I_RI) &&
        (dma_rxstopped(ch->di[i]) || !dma_rxenabled(ch->di[i]) ||
         dma_rxtxerror(ch->di[i], FALSE))) {
        err("rx data err ep %d, DMA %d; dmaintstatus 0x%x", ep, i,
            R_REG(ch->osh, &ch->regs->dmaint[i].status));
        ch_rxrecompose(ch, ep, i, offset);
    } /* EP_DIR_OUT */
    else if ((((epinfo & EP_DIR_MASK) == EP_DIR_IN) || (ep == 0)) &&
             ((ch->dmaintmask[i]) & I_XI) &&
             (dma_txstopped(ch->di[i]) || !dma_txenabled(ch->di[i]) ||
              dma_rxtxerror(ch->di[i], TRUE))) {
#if defined(RNDIS) || defined(BCMCDC)
        proto_pr46794WAR(ch_dngl(ch));
#endif
        err("tx data err ep %d, DMA %d; dmaintstatus 0x%x", ep, i,
            R_REG(ch->osh, &ch->regs->dmaint[i].status));
        ch_txrecompose(ch, ep, i, offset, TRUE);
    } else {
        /* EP_DIR_IN */
        err("unhandled data error on ep%d", ep);
    }

    trace("done");
}

/**
 * workaround via-via invoked by higher USB level (usbdev_rte.c)
 * pr46794: phantom tx data error on control endpoint w/subsquent rx of OUT packet w/incorrect len
 */
void
ch_pr46794WAR(struct usbdev_sb *ch)
{
    // trace("");
    dma_txreset(ch->di[0]);

    ch_txrecompose(ch, 0, 0, 0, FALSE);
}


/**
 * When the USB DMA engine signals an error, the specific type of error has to be distilled to
 * determine if the error needs to be handled.
 */
int
ch_dmaerrors(struct usbdev_sb *ch, uint32 dmaintstatus, int i)
{
    if (dmaintstatus & I_PC)
        err("usbdma%d: descriptor error", i);
    if (dmaintstatus & I_PD)
        err("usbdma%d: data error", i);
    if (dmaintstatus & I_DE)
        err("usbdma%d: descriptor protocol error", i);
    if (dmaintstatus & I_RU)
        dbg_printf("usbdma%d: receive descriptor underflow", i);
    if (dmaintstatus & I_RO)
        err("usbdma%d: receive fifo overflow", i);
    if (dmaintstatus & I_XU)
        err("usbdma%d: transmit fifo underflow", i);

    if ((ch->id == USB11D_CORE_ID) && (ch->rev == 0)) {

        /* PR17338 WAR: Treat SError glitch as USB data error */
        err("PR17338 WAR: Treat SError glitch as USB data error");
        ch_dma_dataerr(ch, DMA2EP(i, EP_DIR_OUT));

    } else if (dmaintstatus & ch->fatal) {
        err("fatal error 0x%x", (dmaintstatus & ch->fatal));
#ifndef DONGLEBUILD
        /* Big hammer */
        ch_init(ch, TRUE);
        err("usbdma%d: big hammer ch_init() done for dmaintstatus 0x%X", i, dmaintstatus);
#endif
        return 1;
    }

    return BCME_OK;
}

/**
 * Used for error recovery. Called by tx_recompose(). Skips over data already transmitted indicated
 * by offset and returns the packet ptr for retransmission.
 */
void *ch_retxprep(struct usbdev_sb *ch, void *p, uint32 offset)
{
    void *p0, *next;
    int len;
    uint32 txnok = offset; /* offset to all bytes transmitted w/o errors */

    p0 = p;
    if (txnok > PKTLEN(ch->osh, p0)) {
        /* Must be a chained packet. Traverse to pkt with partial transmission */
        for (; p0; p0 = next) {
            len = PKTLEN(ch->osh, p0);
            next = PKTNEXT(ch->osh, p0);

            if (txnok <= len)   /* Found pkt w/ partial transimission */
                break;

            txnok -= len;

            /* cut the chain, free transmitted segment */
            PKTSETNEXT(ch->osh, p0, NULL);
            PKTFREE(ch->osh, p0, TRUE);
        }

        ASSERT(p0 != NULL); /* offset exceeds pkt chain; offset corrupted? */
    } else if (txnok == PKTLEN(ch->osh, p0)) {
        /* XXX PR4916/PR988 WAR: Posting zero length buffers is unsupported, skip 4322 */
        if (!((ch->rev == 5) && (ch->id == USB20D_CORE_ID))) {
            ((char *) PKTDATA(ch->osh, p0))[--txnok] = 0;
        }
    }

    /* Trim the partial packet */
    PKTPULL(ch->osh, p0, txnok);

    /* XXX for zero length frag, free it for 4322(usb2.0 rev5). Otherwise, host will timeout */
    if (PKTLEN(ch->osh, p0) == 0) {
        if (((ch->rev == 5) && (ch->id == USB20D_CORE_ID))) {
            next = PKTNEXT(ch->osh, p0);
            PKTSETNEXT(ch->osh, p0, NULL);
            PKTFREE(ch->osh, p0, TRUE);
            p0 = next;
        }
    }

    return p0;
}

/**
 * Called as part of an error flow (eg, tx underflow). DMA engine is reinitialized. Before doing so,
 * unconsumed DMA packets are saved and restored later on.
 *
 * According to HW folks, the Dma_Err can occur on data underflow. The USB bus analyzer shows CRC16
 * and timeout error.
 * When it happens, the DMA will stall and rely on driver to reset that DMA channel. To do that,
 * driver will have to save the pending DMA descriptors first. The USB dma tx happens on each
 * packet(512bytes). So it can stall in the mid of one dma descriptor or a mid of multiple(chained)
 * dma descriptors. USB register reg->epbytes has the exact number of bytes transmitted.
 * Driver recover steps are:
 *
 * - go through DMA ring, free successfully transmitted descriptors, find the stall-in-the-mid
 *      descriptor(s)
 * - get the corresponding pkt pointer for that descriptor(s)
 * - use reg->epbytes to walk through the pkt(or chained pkts) to find the one stalled
 *        o throw away the bytes, which has been successfully transmitted in that descriptor by
 *              PULL the valid data offset in the pkt buffer
 *        o save the remaining data (and chained descriptors)
 *        o reset DMA
 *        o requeue saved descriptors
 */
void
ch_txrecompose(struct usbdev_sb *ch, int ep, int i, uint32 offset, bool keepfirst)
{
    struct pktq txq_dma;        /* temp queue for descriptor ring items */
    struct pktq txq_tmp;    /* temp queue for SW tx queue backup */
    void *p;
    uint32 usbintstatus;

//    USB_ERR(("ep%d: %s %d\n", ep, __FUNCTION__, offset));

    ch->dma_txrecompose_cnt++;

    pktqinit(&txq_tmp, ch->tunables[TXQ_LEN]);
    pktqinit(&txq_dma, ch->tunables[NTXD]);

    /* Reclaim already transmitted packets */
    dma_txreclaim(ch->di[i], HNDDMA_RANGE_TRANSFERED);

    /* Disable tx engine */
    dma_txreset(ch->di[i]);

    /* Back up SW queue packets */
    while ((p = pktdeq(&ch->txq[i]))) {
        pktenq(&txq_tmp, p);
    }

    /* Reclaim the partial packet */
    if (!(p = dma_getnexttxp(ch->di[i], TRUE)))
        goto txdone;

    if (keepfirst) {
        err("ep%d: tx data error (%d/%d)", ep, offset, PKTLEN(ch->osh, p));
        p = ch_retxprep(ch, p, offset);
        ASSERT(p != NULL);

        if (p != NULL)
            pktenq(&txq_dma, p);
    } else {
#ifdef BCMDBG
        prpkt("toss pkt", ch->osh, p);
#endif
        PKTFREE(ch->osh, p, TRUE);
    }

    /* Save remaining packets to retransmit */
    while ((p = dma_getnexttxp(ch->di[i], TRUE)))
        pktenq(&txq_dma, p);

txdone:

    /* Clear the data error int before re-enabling the dma engine */
    usbintstatus = R_REG(ch->osh, &ch->regs->usbintstatus);
    W_REG(ch->osh, &ch->regs->usbintstatus, usbintstatus & I_DATAERR(ep));

    /* Re-enable tx engine */
    dma_txinit(ch->di[i]);

    /* dma has been flushed, so do the pending couters */
    ch->txpktpend[i] = 0;
    ch->txpktpend_bytes[i] = 0;

    /* Repost packets to retransmit */
    while ((p = pktdeq(&txq_dma))) {
        if (PKTLEN(ch->osh, p) == 0) {
            USB_ERR(("%s send risky 0 length pkt !!!!!\n", __FUNCTION__));
        }
        ch_tx(ch, ep, p);
    }

    /* restore SW queue AFTERWARD to ensure ch_tx() enqueue success */
    while ((p = pktdeq(&txq_tmp))) {
        if (pktq_full(&ch->txq[i])) {
            err("ep%d: txq full", ep);
            PKTFREE(ch->osh, p, TRUE);
            ASSERT(0);
        } else {
            pktenq(&ch->txq[i], p);
        }
    }
} /* ch_txrecompose */

/** USB packet(s) received from host must be passed up */
void
ch_rx_q_and_send(struct usbdev_sb *ch, int ep, int i, void *p, uint16 flags)
{
    if (ep == 0 && !(flags & RXF_SETUP) && !(flags & RXF_BAD)) {
        /* rev 3 or higher: pkt tag must match expected tag from Setup */

        /* go ahead for setup handshake */
        OR_REG(ch->osh, &ch->regs->epstatus, EPS_DONE(i));
        if (ch->setupdma) {
            if (ch->ctrloutexpected[i].tag !=
                (flags & SETUP_TAG_MASK) >> SETUP_TAG_SHIFT) {
                err("ctrlout data tag %d mismatch w/Setup tag %d",
                    (flags & SETUP_TAG_MASK) >> SETUP_TAG_SHIFT,
                    ch->ctrloutexpected[i].tag);
                PKTFREE(ch->osh, p, FALSE);
                return;
            }
        }
        /* XXX SGS - comment out for now until PR89837 can be analyzed */
#if 0
#if defined(BCMUSBDEV) && !defined(BCM_BOOTLOADER)
        /* for hosts that don't issue setcfg after download */
        if (!ch->disconnect && usbdev_getcfg(ch->bus) == 0)
            ch_setcfg(ch, 1);
#endif
#endif /* if 0 */
    }

    pktenq(&ch->rxq[i], p);
    ch_sendup(ch, ep);
}

/** Rx error handling. Called by ch_rxrecompose(). */
void*
ch_dma_rx_giantframe(struct usbdev_sb *ch, hnddma_t *di, void *p, uint16 resid,
    int16 rxbufsize)
{
    uint seg_len = 0;
    void *p_partial;
    void *tail = pktlast(ch->osh, p);

    while ((resid > 0) && (p_partial = dma_getnextrxp(di, TRUE))) {
        PKTSETNEXT(ch->osh, tail, p_partial);
        seg_len = MIN(resid, (int)rxbufsize);
        PKTSETLEN(ch->osh, p_partial, seg_len);

        tail = p_partial;
        resid -= seg_len;

        USB_DBG(("ch_dma_rx_giantframe: chaining frag 0x%p(pktlen %d) resid %d\n",
            p_partial, seg_len, resid));
    }

    if (resid > 0) {
        USB_ERR(("%s: wrong dma ring\n", __FUNCTION__));
        ASSERT(resid == 0);
    }
    return p;
}

/**
 * Handles the aftermath of a transfer error in the host -> dongle direction. Possible causes of
 * transfer error:
 *
 * a. CRC check failed on an individual received USB packet
 * b. Host->chip USB FIFO was not empty on packet reception (backplane busy ? DMA problem?)
 * c. Shortage of rx DMA buffers (= rx descriptor underflow)
 *
 * The USB core will send a NAK to the host and the DMA engine connected to the endpoint on which
 * the transfer error occurred will be halted by hardware.
 *
 * All receive endpoints (except the one for setup) share a single 512B rxfifo in the usb20d
 * controller. To minimize NAKing the host, the FW should post empty rx dma buffer for each receive
 * endpoint fast enough to avoid descriptor underflow conditions.
 *
 *
 * Each USB transfer starts at a DMA buffer and spans one or more DMA buffers.
 *
 * 1) reset DMA rx channel, which encountered the DmaErr
 * 2) manually process dma rx ring, for complete frames,
 *    queue and send up immediately
 * 3) continue drain the ring by ignoring curr pointer since it may not be updated
 *  when the DmaErr occurs due to partial pkt reception in HW
 *    for the pkt received partially, use the HW good_bytes to extract
 *    frag(or multiple frags), fixup the rx header len field in header frag
 *    queue, but not sendup, wait the next dma rx to get remaining partial pkt
 *    ch_dma_recv()->ch_sendup() will reassembly the complete pkt and send up.
 * 4) restart DMA rx channel
 *
 * Parameters:
 *    'ep' : physical (so not logical) endpoint number
 *    'i'  : DMA engine number
 *    'good_bytes' : number of received bytes as reported by register EpBytesNoErrorX[ep]
 *
 */
void
ch_rxrecompose(struct usbdev_sb *ch, int ep, int i, uint32 good_bytes)
{
    hnddma_t *di = ch->di[i];
    usbdev_sb_rxh_t *rxh = NULL;
    void *p = NULL;
    uint16 rxoffset, rxbufsize;
    uint pkt_len, frag0_len, len;
    int resid = 0;

    uint32 usbintstatus;
    uint16 flags;

    /* PKTLEN(osh, head) is not set yet when dma is in error */
    /*
     * Size and offset were specified during dma_attach. Size is the DMA buffer size. Each DMA
     * buffer consists of a usbdev_sb_rxh_t header followed by data, 'rxoffset' points at this
     * data.
     */
    rxbufsize = ch->rxbufsize[i];
    rxoffset = ch->rxoffset[i];
    ch->dma_rxrecompose_cnt++;

    err("ep%d: %s: dma=%d good_bytes %d !", ep, __FUNCTION__, i, good_bytes);
    USB_ERR(("ep%d: %s: dma=%d good_bytes %d !\n", ep, __FUNCTION__, i, good_bytes));

    /* Disable rx engine, hw curr pointer will be reset to 0 */
    dma_rxreset(di);

    /*
     * Since hw_curr is not longer valid, use "forceall" to walk through DMA ring.
     * dma_getnextrxp() returns entries on the ring, in the order in which they were placed on
     * the ring.
     */
    while ((p = dma_getnextrxp(di, TRUE))) {

        rxh = (usbdev_sb_rxh_t *)PKTDATA(ch->osh, p);
        flags = ltoh16(rxh->flags);
        len = ltoh16(rxh->len);

        if (len == 0) {
            /*
             * found either a partial USB transfer, or the buffer after a completed
             * transfer
             */
            break;
        }

        /* good frames, extract all dma buffer and send up */
        pkt_len = MIN((rxoffset + len), rxbufsize);
        PKTSETLEN(ch->osh, p, pkt_len);
        resid = len - (rxbufsize - rxoffset);
        if (resid > 0)
            p = ch_dma_rx_giantframe(ch, di, p, resid, rxbufsize);
        USB_DBG(("ch_rxrecompose: sendup p %p, len %d flags 0x%x\n", p, len, flags));
        ch_rx_q_and_send(ch, ep, i, p, flags);
    }

    if (p == NULL) {
        USB_ERR(("%s: bad ring\n", __FUNCTION__));
        goto restart_dma;
    }
    if (good_bytes == 0) {
        USB_ERR(("%s: good_bytes is 0\n", __FUNCTION__));
        PKTFREE(ch->osh, p, FALSE);
        goto restart_dma;
    }

    /* PARTIAL frame has been obtained, processing start here */
    rxh->len = htol16(good_bytes);  /* fixup descriptor length, the head frag PKTLEN */
    frag0_len = MIN((rxoffset + good_bytes), rxbufsize);
    PKTSETLEN(ch->osh, p, frag0_len);
    resid = good_bytes - (rxbufsize - USB_RXOFFSET);
    if (resid > 0)
        p = ch_dma_rx_giantframe(ch, di, p, resid, rxbufsize);

    /* extra safety checking, max agg frames from host is BCM_RPC_TP_HOST_AGG_MAX_BYTE */
    if (pkttotlen(ch->osh, p) > USB_RX_MAX_VALID_PKTSZ) {
        USB_ERR(("%s: UNEXPECTED frame len (%d), toss\n", __FUNCTION__,
            pkttotlen(ch->osh, p)));
        goto restart_dma;
    }

    pktenq(&ch->rxq[i], p);
    USB_DBG(("ch_rxrecompose: queue up p %p flags 0x%x, frag0len %d, len %d\n",
           p, flags, frag0_len, good_bytes));

restart_dma:

    /* Clear the data error int before re-enabling the dma engine */
    usbintstatus = R_REG(ch->osh, &ch->regs->usbintstatus);
    W_REG(ch->osh, &ch->regs->usbintstatus, usbintstatus & I_DATAERR(ep));

    /* Re-enable rx engine */
    dma_rxreclaim(di);
    dma_rxinit(di);

    /* Repost packets */
    ch_rxfill(ch, i);
} /* ch_rxrecompose */

/** Handle transmit (dongle->host traffic) interrupt generated by DMA */
void
ch_dma_dotxstatus(struct usbdev_sb *ch, int queue)
{
    uint totlen = 0;
    void *p;

    /* Completed tx packets are dequeued from DMA ring and then freed */
    while ((p = dma_getnexttxp(ch->di[queue], FALSE))) {
        totlen = pkttotlen(ch->osh, p);
#if defined(MSGTRACE) || defined(LOGTRACE)
        if (PKTMSGTRACE(p)) {
#ifdef LOGTRACE
            int ack = 0;
#endif
            PKTSETMSGTRACE(p, FALSE);
#ifdef MSGTRACE
#ifdef LOGTRACE
            ack =
#endif
                msgtrace_sent();
#endif /* MSGTRACE */

#ifdef LOGTRACE
            if (ack == 0)
                ack = logtrace_sent();
#endif
        }
#endif /* MSGTRACE || LOGTRACE */
        PKTFREE(ch->osh, p, TRUE);

        ch->txpktpend_bytes[queue] -= totlen;
        if (ch->txpktpend_bytes[queue] < 0) {
            ch->txpktpend_bytes[queue] = 0;
            USB_ERR(("%s: txpend_bytes %d!\n", __FUNCTION__,
                ch->txpktpend_bytes[queue]));
        }

        ch->txpktpend[queue]--;
        if (ch->txpktpend[queue] < 0) {
            ch->txpktpend[queue] = 0;
            USB_ERR(("%s: pktpend is negative!\n", __FUNCTION__));
        }

#ifdef BCMDBG
        if (queue == 2) {   /* XXX DBG only, msg flooding could timeout USB */
            USB_DBG(("-%d ", ch->txpktpend[queue]));
        }
#endif
    }

    /* old version: dma_txreclaim(ch->di[queue], FALSE); */
} /* ch_dma_dotxstatus */

/**
 * Handle interrupt from endpoint rx (host->dongle) dma engine by removing the packet from the
 * rx DMA ring, passing the received packet up to higher layers and filling the DMA ring with fresh
 * buffers.
 */
bool
ch_dma_recv(struct usbdev_sb *ch, int queue)
{
    void *p;
    uint rxcount = 0;
    bool resched = FALSE;

    usbdev_sb_rxh_t *rxh;
    int ep = DMA2EP(queue, EP_DIR_OUT);
    uint16 flags = 0;

    while ((p = dma_rx(ch->di[queue]))) {
#ifdef BCMDBG_POOL
        PKTPOOLSETSTATE(p, POOL_RXDH);
#endif
        rxh = (usbdev_sb_rxh_t *) PKTDATA(ch->osh, p);
        flags = ltoh16(rxh->flags);

        ch_rx_q_and_send(ch, ep, queue, p, flags);

        /* BOUNDING: reschedule ch_dpc() to process more later unless USB_RXBND = -1 */
        if (ep != 0 && ++rxcount >= (uint)ch->tunables[RXBND]) {
            resched = TRUE;
            break;
        }
    }
    if (!ch->flowctl_rx[queue])
        ch_rxfill(ch, queue);

#ifdef BCMBG
    /* fill in histogram */
    hist_rxc[rxcount]++;
    hist_txq[MIN(HIST_SIZE_USBDEV, pktq_len(&ch->txq[2]))]++;
    hist_txq_dma[MIN(HIST_SIZE_USBDEV, ch->txpktpend[2])]++;
#endif
    return resched;
}

/**
 * To avoid rx overflow, feed fresh rx buffers to (bulk-out) rx DMA ring on a periodic timer event
 */
static void
_ch_bulkrxfill(dngl_task_t *task)
{
    struct usbdev_sb *ch = (struct usbdev_sb *) task->context;
    ch->bulktimeractive = FALSE;
    ch_rxfill(ch, ch->bulkoutdma);
}
static void
_ch_resume_timer(dngl_task_t *task)
{
    struct usbdev_sb *ch = (struct usbdev_sb *) task->context;
    if (ch->suspended && !ch->disconnected)
        ch_resume(ch);
}


/** multiple code paths try to avoid rx overflow by feeding the rx DMA rings with fresh buffers */
void
ch_rxfill(struct usbdev_sb *ch, int dma)
{
    if (ch->rxbufsize[dma]) {
        // trace("");
        dma_rxfill(ch->di[dma]);

    /* PR 91427 : Disabling zero delay timer since causing issues in bmac */
/*
        if (dma == ch->bulkoutdma && dma_rxactive(ch->di[dma]) != ch->bulkrxfillct) {
            if (!ch->bulktimeractive && dngl_add_timer(ch->bulkrxfill, 0, FALSE))
                ch->bulktimeractive = TRUE;
        }
*/
    }
}

/**
 * The DMA engines that are a part of the USBd core generate several events that require further
 * firmware processing. Sunny day events are for example transmit and receive interrupts. More grim
 * are transmit underflow, receive overflow.
 */
int
ch_dma(struct usbdev_sb *ch, int i)
{
    bool resched = FALSE;
    uint32 dmaintstatus = R_REG(ch->osh, &ch->regs->dmaint[i].status);

    // trace("");

    /* Clear DMA interrupt */
    W_REG(ch->osh, &ch->regs->dmaint[i].status, dmaintstatus);

    dmaintstatus |= ch->resched_status[i];
    ch->resched_status[i] = 0;

    if (dmaintstatus & I_ERRORS) {
        if (ch_dmaerrors(ch, dmaintstatus, i))
            return BCME_ERROR;
    }

    if (!(dmaintstatus & I_XU) && dmaintstatus & I_XI) {
        ch_dma_dotxstatus(ch, i);
    }

    if ((dmaintstatus & I_RI) || (dmaintstatus & I_RU)) {
        resched = ch_dma_recv(ch, i);
        if (resched) {
            ch->resched_status[i] |= I_RI;
        }
    }

    //trace("done");
    return (resched ? 1 : 0);
}

/**
 * The USBd core generates an interrupt when it receives a setup token from the host. The interrupt
 * originates from the dedicated SETUP token DMA engine (#5).
 */
void
ch_setup(struct usbdev_sb *ch, int i)
{
    int ep = DMA2EP(i, EP_DIR_OUT);

    /* PR2391 WAR: Clear stall bit */
    AND_REG(ch->osh, &ch->regs->epstatus, ~EPS_STALL(ep));

    /* Check for lost setup packet */
    if (R_REG(ch->osh, &ch->regs->epstatus) & EPS_SETUP_LOST(ep)) {
        usbdev_isr_debug_printf("ep%d: lost setup", ep);
    }

    /* Set speed */
    if (ch->id == USB20D_CORE_ID)
    {
        #if (USB_DEVICE_TEST_RDL)
        usbdev_speed(ch->bus, ((R_REG(ch->osh, &ch->regs->devstatus) & DS_DS_MASK) == DS_DS_HS));
        #else
        //#USBDevice#: Update speed after setup with other method.
        #endif
    }
}

/**
 * Starting with rev 3:
 * Pull Setup Device Requests off of the Setup DMA engine. Discard
 * duplicate requests made to the same endpoint, only accepting the
 * last request for that endpoint.
 */
int
ch_devreq(struct usbdev_sb *ch)
{
    uint32 dmaintstatus = R_REG(ch->osh, &ch->regs->sdintstatus);
    void *p;
    usbdev_sb_rxh_t *rxh;
    int i;
    uint8 dma;
    volatile usb_device_request_t *req;
    struct {
        void *p;
        uint8 dma;
    } reqs[SETUP_DMA_DEPTH];
    uint8 ctrlouttag = 0xff;
    uint32 ctrloutlen = 0;
    uint16 flags;

    // trace("");

    /* Clear DMA interrupt */
    W_REG(ch->osh, &ch->regs->sdintstatus, dmaintstatus);

    if (dmaintstatus & I_ERRORS) {
        usbdev_isr_debug_printf("dma errors 0x%x", dmaintstatus);
        if (ch_dmaerrors(ch, dmaintstatus, SETUP_DMA))
            return BCME_ERROR;
    }

    memset(reqs, 0xff, sizeof(reqs));
    while ((p = dma_rx(ch->di[SETUP_DMA]))) {
#ifdef BCMDBG_POOL
        PKTPOOLSETSTATE(p, POOL_RXDH);
#endif
        if (PKTLEN(ch->osh, p) < 4) {
            usbdev_isr_debug_printf("bad pktlen %d", PKTLEN(ch->osh, p));
            PKTFREE(ch->osh, p, FALSE);
            continue;
        }
        rxh = (usbdev_sb_rxh_t *) PKTDATA(ch->osh, p);
        flags = ltoh16(rxh->flags);
        if (flags & RXF_BAD) {
            PKTFREE(ch->osh, p, FALSE);
            continue;
        }
        if (ltoh16(rxh->len) != 8) {
            usbdev_isr_debug_printf("bad req len %d", ltoh16(rxh->len));
            PKTFREE(ch->osh, p, FALSE);
            continue;
        }

        dma = EP2DMA((flags & EP_ID_MASK) >> EP_ID_SHIFT);

        /* find empty slot or existing slot for same dma engine */
        for (i = 0; i < SETUP_DMA_DEPTH; ++i)
        {
            if (reqs[i].dma == 0xff || reqs[i].dma == dma)
                break;
        }

        if(i >= SETUP_DMA_DEPTH)
        {
            usbdev_isr_debug_printf("No empty DMA slot!");
            return BCME_ERROR;
        }

        /* replace an existing entry */
        if (reqs[i].dma == dma)
            PKTFREE(ch->osh, reqs[i].p, FALSE);

        reqs[i].dma = dma;
        reqs[i].p = p;

        req = (volatile usb_device_request_t *) &rxh[1];
        /* for control out, save the expected tag to be matched when data arrives */
        if (!(req->bmRequestType & UT_READ)) {
            ctrlouttag = (flags & SETUP_TAG_MASK) >> SETUP_TAG_SHIFT;
            ctrloutlen = req->wLength;
        }
        ch->ctrloutexpected[dma].tag = ctrlouttag;
        ch->ctrloutexpected[dma].len = ctrloutlen;
    }

    /* Send up the valid Setup Device Requests */
    for (i = 0; i < SETUP_DMA_DEPTH; ++i) {
        if (reqs[i].dma == 0xff)
        {
            usbdev_isr_debug_printf("empty dma depth %d", i);
            break;
        }
        ch_setup(ch, reqs[i].dma);
        pktenq(&ch->rxq[reqs[i].dma], reqs[i].p);
        ch_sendup(ch, DMA2EP(reqs[i].dma, EP_DIR_OUT));
    }

    dma_rxfill(ch->di[SETUP_DMA]);

    return BCME_OK;
} /* ch_devreq */

/** hides struct details for callers outside of this file */
int
ch_usb20(struct usbdev_sb *ch)
{
    return (ch->id == USB20D_CORE_ID);
}

/**
 * Device initiated resume. Several code paths require the host to be taken out of 'suspend' state.
 * One of the events that triggers this function is an event (e.g., wireless reception) that causes
 * the firmware to send packet(s) to the host. Another event is debug/test related: the user
 * requested to take the bus out of suspend.
 */
int
ch_resume(struct usbdev_sb *ch)
{
    uint8 swait = 0;
    bool oob_wakeup = FALSE;

    if (ch->signaledwakeup || !usbdev_oob_connected(ch->bus))
        return BCME_OK;

    /* make sure we have clocks */
    ch_leave_suspend(ch);

    /* have we enabled Remote Wake-up? */
    if (!(R_REG(ch->osh, &ch->regs->devcontrol) & DC_RW))
        goto error;

    if (!(R_REG(ch->osh, &ch->regs->devstatus) & DS_SP))
        goto error;

    /* did the host issue SET_FEATURE with DEVICE_REMOTE_WAKEUP? */
    if (!(R_REG(ch->osh, &ch->regs->devstatus) & DS_RWF)) {
#ifndef BCM_BOOTLOADER
        oob_wakeup = usbdev_oobresume(ch->bus, TRUE);
        if (!oob_wakeup)
#endif /* BCM_BOOTLOADER */
            goto error;
    } else
    OR_REG(ch->osh, &ch->regs->devcontrol, DC_RM);

    ch->signaledwakeup = TRUE;

    /* After Device sets resume bit, wait at most 15ms
     * for Host to clear suspend bit
     */
    if (!oob_wakeup) {
        while ((R_REG(ch->osh, &ch->regs->devstatus) & DS_SP) &&
               (swait++ < 15) && usbdev_oob_connected(ch->bus))
        OSL_DELAY(1000); /* 1ms */

    AND_REG(ch->osh, &ch->regs->devcontrol, ~DC_RM);
    }
    return BCME_OK;

error:
    /* stop requesting HT */
    if (!ch->disconnected && PMUCTL_ENAB(ch->sih))
        AND_REG(ch->osh, &ch->regs->clkctlstatus, ~CCS_FORCEHT);
    return BCME_ERROR;
}

/**
 * Function name is a bit misleading since it has nothing to do with an 'USB suspend'. It could
 * better be named 'enable_ht_clock'. Is, amongst other reasons, called in the context of LPM
 * (Link Power Management).
 */
void
ch_leave_suspend(struct usbdev_sb *ch)
{
    if (PMUCTL_ENAB(ch->sih))
        ch_waitfor_ht(ch, TRUE, __FUNCTION__);
}

void
ch_leave_suspend_wrapper(struct usbdev_sb *ch)
{
    ch_leave_suspend(ch);
}

bool
ch_txpending(struct usbdev_sb *ch)
{
    int i;

    /* leave suspend if there are any packets pending */
    for (i = 0; i < DMA_MAX; ++i) {
        if (NULL != ch->di[i]) {
            if ((ch->dmaintmask[i] & I_XI) && dma_txpending(ch->di[i]))
                return TRUE;
        }
    }

    return FALSE;
}
/**
 * function name is a bit of a misnomer since usbd core already entered suspend state when this
 * function is called.
 */
void
ch_enter_suspend(struct usbdev_sb *ch)
{
    /* immediately exit suspend if there are any packets pending */
    if (!ch->disconnected && ch_txpending(ch)) {
        /* delay an extra 2ms, for a total of 5ms Idle Suspend */
        dngl_add_timer(ch->resume_timer, 2, FALSE);
        return;
    }
    /* stop requesting HT */
    if (!ch->disconnected && PMUCTL_ENAB(ch->sih)) {
        AND_REG(ch->osh, &ch->regs->clkctlstatus, ~CCS_FORCEHT);
        SPINWAIT((R_REG(ch->osh, &ch->regs->clkctlstatus) & CCS_HTAVAIL),
            PMU_MAX_TRANSITION_DLY);
    }
}

/**
 * To enhance dongle->host throughput in certain scenario's, USB transmit gets buffers otherwise
 * used by USB receive. Invoked via usbdev_rte.c.
 */
void
ch_rxflowcontrol(struct usbdev_sb *ch, int ep, bool state)
{
    int dma = EP2DMA(ep);

    if (ch->flowctl_rx[dma] != state) {
        if (state == ON)
            ch->flowctl_rx_cnt[dma]++;
        ch->flowctl_rx[dma] = state; /* if state==1, rx will not be 'refilled' anymore */
    }

    if (state == OFF)
        ch_rxfill(ch, dma);
}

/** interrupts are switched off prior to DPC processing */
void
ch_intrsoff(struct usbdev_sb *ch)
{
    //#USBDevice#: +++ TODO
    /*
     * Temp WAR to disbale USB device interrupt from ARM source due to ch->usbintmask
     * can NOT control DMA level interrupt!!!
     */
    {
        volatile uint32 *cr4_isr_mask = (uint32 *)PLATFORM_APPSCR4_REGBASE(0x18);

        //usbdev_isr_debug_printf("### OffCR4:");
        AND_REG(ch->osh, cr4_isr_mask, ~(1L << USB_REMAPPED_ExtIRQn));
        //usbdev_isr_debug_printf("offIsrMask=0x%08x\r\n", R_REG(ch->osh, cr4_isr_mask));
    }
    //#USBDevice#: --- TODO

    W_REG(ch->osh, &ch->regs->usbintmask, 0);
    ch->usbintmask = 0;
}

/** interrupts are switched back on when DPC processing has completed */
void
ch_intrson(struct usbdev_sb *ch)
{
    ch->usbintmask = DEF_USBINTMASK;
    W_REG(ch->osh, &ch->regs->usbintmask, ch->usbintmask);

    //#USBDevice#: +++ TODO
    /*
     * Temp WAR to disbale USB device interrupt from ARM source due to ch->usbintmask
     * can NOT control DMA level interrupt!!!
     */
    {
        volatile uint32 *cr4_isr_mask = (uint32 *)PLATFORM_APPSCR4_REGBASE(0x18);

        //usbdev_isr_debug_printf("### OnCR4:");
        OR_REG(ch->osh, cr4_isr_mask, (1L << USB_REMAPPED_ExtIRQn));
        //usbdev_isr_debug_printf("onIsrMask=0x%08x\r\n", R_REG(ch->osh, cr4_isr_mask));
    }
    //#USBDevice#: --- TODO
}

uint32
ch_intstatus(struct usbdev_sb *ch)
{
    uint32 usbintstatus = R_REG(ch->osh, &ch->regs->usbintstatus);

    usbintstatus &= DEF_USBINTMASK;
    return usbintstatus;
}

/**
 * The DPC not only has to handle the interrupt that caused it to be called, but also any
 * additional interrupt events that occurred between interrupt and DPC handling. Events are
 * accumulated in variable ch->intstatus.
 */
void
ch_intrsupd(struct usbdev_sb *ch)
{
    uint32 intstatus;

    // trace("");
    ASSERT(ch->intstatus != 0);

    /* update interrupt status in software */
    intstatus = ch_intstatus(ch);
    ch->intstatus |= intstatus;
}

/**
 * A bit of a misnomer since this function does not dispatch interrupt events. Instead it checks if
 * the USBd core has any pending irq events. Better name would be 'ch_irq_asserted'.
 */
bool
ch_dispatch(struct usbdev_sb *ch)
{
    uint32 usbintstatus;

    usbintstatus = ch_intstatus(ch);

    /* Top level DMA status bits in usbintstatus can be active even if */
    /*  usbintmask is zero because it is dependent on second-level DMA */
    /*  intstatus0-4/intmask0-4. */
    /* This causes race condition between timer resched dpc() vs. isr() */
    /* Not ours */
    if ((!usbintstatus) || (ch->usbintmask == 0))
        return FALSE;

    // trace("");

    if (!ch->up) {
        err("got spurious interrupt 0x%x", usbintstatus);
        W_REG(ch->osh, &ch->regs->usbintstatus, usbintstatus);
        return TRUE;
    }

    ASSERT(ch->intstatus == 0);
    ch->intstatus = usbintstatus;

    return TRUE;
}

/**
 * After the USBd core caused an interrupt, the interrupt is disabled, next the event(s) that caused
 * the interrupt have to be handled.
 */
bool
ch_dpc(struct usbdev_sb *ch)
{
    uint32 usbintstatus;
    int i;
    uint32 devstatus;
    bool resched = FALSE;

checkagain:
    usbintstatus = ch->intstatus;
    ch->intstatus = 0;

    /* Dispatch to handlers */
    if (usbintstatus & I_RESET) {
        usbdev_isr_debug_printf("@Reset\r\n");
        W_REG(ch->osh, &ch->regs->usbintstatus, I_RESET);
        usbdev_reset(ch->bus);
        goto cleanup;
    }
    if (usbintstatus & I_LPM_SLEEP) {
        /* received LPM transaction with certain HIRD value */
        uint32 lpm_data_hird;

        usbdev_isr_debug_printf("@LPM\r\n");
        W_REG(ch->osh, &ch->regs->usbintstatus, I_LPM_SLEEP);
        if (process_lpm_settings(ch) == BCME_OK) {
            ch->lpmsleep = !ch->lpmsleep;
            ch->suspended = ch->lpmsleep;
            /* Take action only when the HIRD is more than the Deep Sleep threshold */
            lpm_data_hird =
                ((R_REG(ch->osh, &ch->regs->usbsetting)) & USB_LPM_DATA_HIRD_MASK) >>
                USB_LPM_DATA_HIRD_SHIFT;
            if (lpm_data_hird >= LPM_DS_THRESH) {
                /* Deep Sleep */
                if (ch->lpmsleep) {
                    enter_lpm_sleep(ch);
                } else { /* Resume from LPM Sleep */
                    leave_lpm_sleep(ch);
                }
            }
        }
    }
    if (usbintstatus & I_SUS_RES) {
        /*
         * This field is set to 1 when there is a change of the device suspend state, which
         * is captured in the DevStatus.Suspend field. The device enters suspend state when
         * the bus has been continuously idle for over 6ms. The device leaves suspend state
         * when one of the following happens: bus is no longer idle (e.g. bus resume or
         * reset), FW initiates remote wake via DevControl.UsbResume, usb20d is reset via
         * DevControl.DevReset.
         */
        W_REG(ch->osh, &ch->regs->usbintstatus, I_SUS_RES);
        devstatus = R_REG(ch->osh, &ch->regs->devstatus);
        if (devstatus & DS_SP) {
            usbdev_isr_debug_printf("@Suspend\r\n");
            ch->suspended = TRUE;
            ch_enter_suspend(ch);
            usbdev_suspend(ch->bus);
        } else {
            ch_leave_suspend(ch);
            ch->suspended = FALSE;
            ch->signaledwakeup = FALSE;
            usbdev_resume(ch->bus);
            if (ch->disconnected) {
#ifdef HT_POWERSAVE
                /* remove HT requested at beginning of reconnect */
                AND_REG(ch->osh, &ch->regs->clkctlstatus, ~CCS_FORCEHT);
#endif
                ch->disconnected = FALSE;
            }
        }
    }
    if (usbintstatus & I_SOF) {
        /* dbg("SOF");  */
        W_REG(ch->osh, &ch->regs->usbintstatus, I_SOF);
        usbdev_sof(ch->bus);
    }
    if (usbintstatus & I_CFG) {
        /* host issued a USB Set Configuration request */
        int n = (R_REG(ch->osh, &ch->regs->usbsetting) & USB_CF_MASK) >> USB_CF_SHIFT;

        usbdev_isr_debug_printf("@Set Configuration\r\n");
        W_REG(ch->osh, &ch->regs->usbintstatus, I_CFG);
        ch_setcfg(ch, n);
        goto cleanup;
    }
    if (usbintstatus & I_DATAERR_MASK) {
        /*
         * Set when the descriptor processor reports a data transfer error.
         *
         * Clear the interrupt only after disabling DMA since the hardware keys off of the
         * status bit to start accepting new packets.
         */
        usbdev_isr_debug_printf("@Data Error\r\n");
        for (i = 0; i < EP_MAX; i++) {
            if (usbintstatus & I_DATAERR(i)) {
                /* Host prematurely ended control transfer */
                if (usbintstatus & I_TXDONE(EP2DMA(i))) {
                    usbdev_isr_debug_printf("ep%d: tx done\r\n", i);
                    W_REG(ch->osh, &ch->regs->usbintstatus,
                          usbintstatus & I_DATAERR(i));
                } else
                    ch_dma_dataerr(ch, i);
            }
        }
    }
    if (usbintstatus & I_SETUP_MASK) {
        usbdev_isr_debug_printf("@Setup\r\n");
        /* PR24862/24863 WAR: Race between SetCfg int and 1st Setup to config'd ep */
        if (R_REG(ch->osh, &ch->regs->usbintstatus) & I_CFG) {
            int n = ((R_REG(ch->osh, &ch->regs->usbsetting) & USB_CF_MASK) >>
                     USB_CF_SHIFT);

            usbdev_isr_debug_printf("@Late Set Configuration\r\n");
            W_REG(ch->osh, &ch->regs->usbintstatus, I_CFG);
            ch_setcfg(ch, n);
            ch->intstatus = ch_intstatus(ch);
            goto checkagain;
        }
        W_REG(ch->osh, &ch->regs->usbintstatus, usbintstatus & I_SETUP_MASK);
        if (usbintstatus & I_DEV_REQ) {
            usbdev_isr_debug_printf("@Setup Data\r\n");
            if (ch_devreq(ch))
                goto cleanup;

        } else if (!ch->setupdma) {
            /* When prior to rev 3 */
            for (i = 0; i < DMA_MAX; i++) {
                if (usbintstatus & I_SETUP(i)) {
                    ch_setup(ch, i);
                    if (ch_dma(ch, i))
                        goto cleanup;
                }
            }
        }
    }

    if (usbintstatus & I_DMA_MASK) {
        /* 'CoreIntStat::InterruptPresent' bitfield in prog guide */
        int ret = 0;
        usbdev_isr_debug_printf("@DMA\r\n");
        for (i = 0; i < DMA_MAX; i++) {
            if (usbintstatus & I_DMA(i)) {

                ret = ch_dma(ch, i);

                if (ret > 0) {
                    usbdev_isr_debug_printf("@resched\r\n");
                    ch->intstatus |= I_DMA(i);
                    resched = TRUE;
                } else if (ret < 0) {
                    /* FIX: this is no-op. should check ret == 0 */
                    goto cleanup;
                }
            }
        }
    }

    /* Run the tx queues */
    for (i = 0; i < DMA_MAX; i++)
        if (pktq_len(&ch->txq[i]))
            ch_sendnext(ch, i);

cleanup:
    return resched;
} /* ch_dpc */

/** used by bootloader only. Unclear why this function is needed. */
si_t *
ch_get_sih(struct usbdev_sb *ch)
{
    return (ch->sih);
}

#ifdef BCMDBG

int
ch_loopback(struct usbdev_sb *ch, char *buf, uint count)
{
    void *p, *p0 = NULL;
    uint32 usbintstatus, dmaintstatus;
    int i = 0;
    int timeout = 1000; /* us */
    uint currid;

    currid = si_coreid(ch->sih);
    if (currid != ch->id)
        si_setcore(ch->sih, ch->id, 0);

    if (!ch->di[0]) {
        err("ep0: must be initialized");
        return -22;
    }

    /* Loopback test on endpoint 0 */
    dma_fifoloopbackenable(ch->di[i]);

    /* Send packet */
    if (!(p = (PKTGET(ch->osh, count, TRUE)))) {
        dbg_printf("ep0: out of txbufs");
        return -12;
    }
    bcopy(buf, PKTDATA(ch->osh, p), count);

    pktenq(&ch->txq[i], p);
    ch_sendnext(ch, i);

    /* Wait for packet */
    do {
        usbintstatus = R_REG(ch->osh, &ch->regs->usbintstatus);
        /* Handle DMA interrupts */
        if ((usbintstatus & I_DMA(i))) {
            dmaintstatus = R_REG(ch->osh, &ch->regs->dmaint[i].status);
            /* Handle DMA receive interrupt */
            if (dmaintstatus & I_RI)
                if ((p0 = dma_rx(ch->di[i])))
                    break;
            /* Handle DMA errors */
            if (dmaintstatus & I_ERRORS) {
                ch_dma(ch, i);
                break;
            }
        }
        OSL_DELAY(1);
    } while (timeout--);

    if (!p0) {
        /* No loopback packet received */
        err("ep0: %s failed", __FUNCTION__);
    } else {
        /* PR19463 WAR: Force length (loopback rx header is bogus) */
        PKTSETLEN(ch->osh, p0, USB_RXOFFSET + PKTLEN(ch->osh, p));
        /* Strip off rx header */
        PKTPULL(ch->osh, p0, sizeof(usbdev_sb_rxh_t));
        if (bcmp(PKTDATA(ch->osh, p), PKTDATA(ch->osh, p0), PKTLEN(ch->osh, p))) {
            err("ep0: data mismatch");
            prhex("tx", PKTDATA(ch->osh, p), PKTLEN(ch->osh, p));
            prhex("rx", PKTDATA(ch->osh, p0), PKTLEN(ch->osh, p0));
        } else
            err("ep0: %s succeeded", __FUNCTION__);
        PKTFREE(ch->osh, p0, FALSE);
    }

    /* Reset */
    ch_init(ch, TRUE);

    if (currid != ch->id)
        si_setcore(ch->sih, currid, 0);
    return (int) count;
} /* ch_loopback */

static void
ch_dumpusbint(uint32 val, struct bcmstrbuf *b)
{
    int i;

    // trace("");
    for (i = I_SETUP_SHIFT; i < (I_SETUP_SHIFT + DMA_MAX); i++)
        if (val & (1L << i))
            bcm_bprintf(b, "SETUP%d ", i - I_SETUP_SHIFT);
    for (i = I_DATAERR_SHIFT; i < (I_DATAERR_SHIFT + EP_MAX); i++)
        if (val & (1L << i))
            bcm_bprintf(b, "DATAERR%d ", i - I_DATAERR_SHIFT);
    if (val & I_SUS_RES) bcm_bprintf(b, "SUS_RES ");
    if (val & I_RESET) bcm_bprintf(b, "RESET ");
    if (val & I_SOF) bcm_bprintf(b, "SOF ");
    if (val & I_CFG) bcm_bprintf(b, "CFG ");
    for (i = I_DMA_SHIFT; i < (I_DMA_SHIFT + DMA_MAX); i++)
        if (val & (1L << i))
            bcm_bprintf(b, "DMA%d ", i - I_DMA_SHIFT);
    for (i = I_TXDONE_SHIFT; i < (I_TXDONE_SHIFT + DMA_MAX); i++)
        if (val & (1L << i))
            bcm_bprintf(b, "TXDONE%d ", i - I_TXDONE_SHIFT);

}

static void
ch_dumpdmaint(uint32 val, struct bcmstrbuf *b)
{
    if (val & I_PC) bcm_bprintf(b, "PC ");
    if (val & I_PD) bcm_bprintf(b, "PD ");
    if (val & I_DE) bcm_bprintf(b, "DE ");
    if (val & I_RU) bcm_bprintf(b, "RU ");
    if (val & I_RO) bcm_bprintf(b, "RO ");
    if (val & I_XU) bcm_bprintf(b, "XU ");
    if (val & I_RI) bcm_bprintf(b, "RI ");
    if (val & I_XI) bcm_bprintf(b, "XI ");
}

void
ch_dumpregs(struct usbdev_sb *ch, struct bcmstrbuf *b)
{
    int i, j;
    uint32 val;
    uint currid;
    uint32 be;

    currid = si_coreid(ch->sih);
    if (currid != ch->id)
        si_setcore(ch->sih, ch->id, 0);

    val = R_REG(ch->osh, &ch->regs->devcontrol);
    bcm_bprintf(b, "devcontrol 0x%x ", val);
    if (val & DC_RS) bcm_bprintf(b, "RS ");
    if (val & DC_PL) bcm_bprintf(b, "PL ");
    if (val & DC_US) bcm_bprintf(b, "US ");
    if (val & DC_ST) bcm_bprintf(b, "ST ");
    if (val & DC_RM) bcm_bprintf(b, "RM ");
    if (val & DC_SD) bcm_bprintf(b, "SD ");
    if (val & DC_SC) bcm_bprintf(b, "SC ");
    if (val & DC_SP) bcm_bprintf(b, "SP ");
    if (val & DC_RW) bcm_bprintf(b, "RW ");
    bcm_bprintf(b, "\n");

    val = R_REG(ch->osh, &ch->regs->devstatus);
    bcm_bprintf(b, "devstatus 0x%x ", val);
    if (val & DS_SP) bcm_bprintf(b, "SP ");
    if (val & DS_RS) bcm_bprintf(b, "RS ");
    bcm_bprintf(b, "\n");

    bcm_bprintf(b, "biststatus 0x%x commandaddr 0x%x\n",
                   R_REG(ch->osh, &ch->regs->biststatus),
                   R_REG(ch->osh, &ch->regs->commandaddr));

    val = R_REG(ch->osh, &ch->regs->usbsetting);
    bcm_bprintf(b, "usbsetting 0x%x CF %d IF %d AI %d\n", val,
                   (val & USB_CF_MASK) >> USB_CF_SHIFT,
                   (val & USB_IF_MASK) >> USB_IF_SHIFT,
                   (val & USB_AI_MASK) >> USB_AI_SHIFT);
    bcm_bprintf(b, "usbframe 0x%x\n",
                   R_REG(ch->osh, &ch->regs->usbframe));

    val = R_REG(ch->osh, &ch->regs->txfifowtermark);
    bcm_bprintf(b, "txfifowatermark 0x%x\n", val);

    val = R_REG(ch->osh, &ch->regs->usbintstatus);
    bcm_bprintf(b, "usbintstatus 0x%x ", val);
    ch_dumpusbint(val, b);
    bcm_bprintf(b, "\n");
    val = R_REG(ch->osh, &ch->regs->usbintmask);
    bcm_bprintf(b, "usbintmask 0x%x ", val);
    ch_dumpusbint(val, b);
    bcm_bprintf(b, "\n");

    for (i = 0; i < DMA_MAX; i++) {
        if (ch->di[i]) {
            val = R_REG(ch->osh, &ch->regs->dmaint[i].status);
            bcm_bprintf(b, "dmaintstatus%d 0x%x ", i, val);
            ch_dumpdmaint(val, b);
            bcm_bprintf(b, "\n");
            val = R_REG(ch->osh, &ch->regs->dmaint[i].mask);
            bcm_bprintf(b, "dmaintmask%d 0x%x ", i, val);
            ch_dumpdmaint(val, b);
            bcm_bprintf(b, "\n");
            /* XXX bcm_binit() to move up in chain of functions where b is allocated
             * when all sprintf's are replaced by bcm_bprintf's
             */
            dma_dump(ch->di[i], b, FALSE);
        }
    }
    if (ch->setupdma) {
        i = SETUP_DMA;
        if (ch->di[i]) {
            val = R_REG(ch->osh, &ch->regs->sdintstatus);
            bcm_bprintf(b, "dmaintstatus%d 0x%x ", i, val);
            ch_dumpdmaint(val, b);
            bcm_bprintf(b, "\n");
            val = R_REG(ch->osh, &ch->regs->sdintmask);
            bcm_bprintf(b, "dmaintmask%d 0x%x ", i, val);
            ch_dumpdmaint(val, b);
            bcm_bprintf(b, "\n");
            /* XXX bcm_binit() to move up in chain of functions where buf is allocated
             * when all sprintf's are replaced by bcm_bprintf's
             */
            dma_dump(ch->di[i], b, FALSE);
        }
    }

    val = R_REG(ch->osh, &ch->regs->epstatus);
    bcm_bprintf(b, "epstatus 0x%x ", val);
    for (i = EPS_STALL_SHIFT; i < (EPS_STALL_SHIFT + EP_MAX); i++)
        if (val & (1L << i))
            bcm_bprintf(b, "STALL%d ", i - EPS_STALL_SHIFT);
    for (i = EPS_SETUP_LOST_SHIFT; i < (EPS_SETUP_LOST_SHIFT + DMA_MAX); i++)
        if (val & (1L << i))
            bcm_bprintf(b, "SETUP_LOST%d ", i - EPS_SETUP_LOST_SHIFT);

    bcm_bprintf(b, "\n");

    for (i = 0; i < EP_MAX; i++) {
        bcm_bprintf(b, "epbytes%d 0x%x\n", i, R_REG(ch->osh, &ch->regs->epbytes[i]));
        val = R_REG(ch->osh, &ch->regs->epinfo[i]);
        bcm_bprintf(b, "epinfo%d 0x%x EN %d %s %s CF %d IF %d AI %d MPS %d\n", i, val,
                       (val & EP_EN_MASK) >> EP_EN_SHIFT,
                       (val & EP_DIR_IN) ? "IN" : "OUT",
                       (val & EP_INTR) ? "INTR" : (val & EP_BULK) ? "BULK" :
                       (val & EP_ISO) ? "ISO" : "CONTROL",
                       (val & EP_CF_MASK) >> EP_CF_SHIFT,
                       (val & EP_IF_MASK) >> EP_IF_SHIFT,
                       (val & EP_AI_MASK) >> EP_AI_SHIFT,
                       (val & EP_MPS_MASK) >> EP_MPS_SHIFT);
    }

    for (i = 0; i < DMA_MAX; i++) {
        /* Retrieve transmit DMA pointers for this channel */
        W_REG(ch->osh, &ch->regs->dmafifo.fifoaddr, (1 << 16) | (i << 19));
        val = R_REG(ch->osh, &ch->regs->dmafifo.fifodatalow);
        bcm_bprintf(b, "txfifo%d rp %d wp %d ",
                       i, (val >> 3) & 0x7, val & 0x7);
        /* Retrieve transmit DMA data for this channel */
        for (j = 0; j < 8; j++) {
            W_REG(ch->osh, &ch->regs->dmafifo.fifoaddr, (i * 8) + j);
            val = R_REG(ch->osh, &ch->regs->dmafifo.fifodatalow);
            be = R_REG(ch->osh, &ch->regs->dmafifo.fifodatahigh) & 0xf;
            if (!(be & 1)) val &= ~0xff;
            if (!(be & 2)) val &= ~0xff00;
            if (!(be & 4)) val &= ~0xff0000;
            if (!(be & 8)) val &= ~0xff000000;
            bcm_bprintf(b, "0x%x ", val);
        }
        bcm_bprintf(b, "\n");
    }

    /* Retrieve receive DMA pointers */
    W_REG(ch->osh, &ch->regs->dmafifo.fifoaddr, (5 << 16));
    val = R_REG(ch->osh, &ch->regs->dmafifo.fifodatalow);
    bcm_bprintf(b, "rxfifo rp %d wp %d ", val & 0xf, (val >> 4) & 0xf);
    /* Retrieve receive DMA data */
    for (j = 0; j < 16; j++) {
        W_REG(ch->osh, &ch->regs->dmafifo.fifoaddr, (4 << 16) | j);
        val = R_REG(ch->osh, &ch->regs->dmafifo.fifodatalow);
        be = R_REG(ch->osh, &ch->regs->dmafifo.fifodatahigh) & 0xf;
        if (!(be & 1)) val &= ~0xff;
        if (!(be & 2)) val &= ~0xff00;
        if (!(be & 4)) val &= ~0xff0000;
        if (!(be & 8)) val &= ~0xff000000;
        bcm_bprintf(b, "0x%x ", val);
    }
    bcm_bprintf(b, "\n");

    if (currid != ch->id)
        si_setcore(ch->sih, currid, 0);
} /* ch_dumpregs */
#else
int
ch_loopback(struct usbdev_sb *ch, char *buf, uint count)
{
    return (int) count;
}

void
ch_dumpregs(struct usbdev_sb *ch, struct bcmstrbuf *b)
{
}
#endif /* BCMDBG */

/** setup packet received from host, send caller supplied response packet back */
void
ch_tx_resp(struct usbdev_sb *ch, int ep, void *p)
{
    int i = EP2DMA(ep);

    /* Setup interrupt or successful tx disables or stops tx engine */
    if (dma_txstopped(ch->di[i]) || !dma_txenabled(ch->di[i])) {
        /* Re-enable tx engine */
        dma_txreset(ch->di[i]);
        dma_txreclaim(ch->di[i], HNDDMA_RANGE_ALL);
        dma_txinit(ch->di[i]);
    }

    /* Post tx data */
    if (!ch_tx(ch, ep, p) && (ep == 0))
        OR_REG(ch->osh, &ch->regs->epstatus, EPS_STALL(ep) | EPS_DONE(ep));
}

/** hide struct details for callers outside of this file */
bool
ch_hsic(struct usbdev_sb *ch)
{
     return ch->hsic;
}

#ifdef BCMUSB_NODISCONNECT
/** hide register access for callers outside of this file */
bool
ch_hispeed(struct usbdev_sb *ch)
{
    if (ch->id == USB20D_CORE_ID)
        return (R_REG(ch->osh, &ch->regs->devstatus) & DS_DS_MASK) == DS_DS_HS;
    else
        return FALSE;
}
#endif /* BCMUSB_NODISCONNECT */

/**
 * When the USBd core indicated it received a LPM transaction from the host, remote wakeup may need
 * to be enabled.
 */
int
process_lpm_settings(struct usbdev_sb *ch)
{
    uint32 lpm_usb_setting;
    uint32 lpm_data_hs;
    uint32 lpm_data_rw;
    uint32 lpm_data_link_state;

    lpm_usb_setting = R_REG(ch->osh, &ch->regs->usbsetting);
    lpm_data_hs =
        (lpm_usb_setting & USB_LPM_DATA_HANDSHAKE_MASK) >> USB_LPM_DATA_HANDSHAKE_SHIFT;
    lpm_data_rw = (lpm_usb_setting & USB_LPM_DATA_REMOTEWK_MASK) >> USB_LPM_DATA_REMOTEWK_SHIFT;
    lpm_data_link_state = (lpm_usb_setting & USB_LPM_DATA_LS_MASK) >> USB_LPM_DATA_LS_SHIFT;

    /* Make sure that the LPM handshake is successfully done with the host controller */
    if (lpm_data_hs != USB_LPM_HANDSHAKE_ACK && lpm_data_link_state != USB_LPM_L1_SLEEP)
        return BCME_ERROR;

    /* Enable/Disable Remote Wakeup Capability according to what Host sets in the LPM token */
    if (lpm_data_rw) {
        OR_REG(ch->osh, &ch->regs->devcontrol, DC_RW);
    } else {
        AND_REG(ch->osh, &ch->regs->devcontrol, ~DC_RW);
    }
    return BCME_OK;
}

/**
 * Put certain chips in a lower power consumption state after receiving LPM transaction from the
 * host.
 * Parameter 'hird': Host Initiated Resume Duration, an indication of the max time the host will
 * wait before resuming the device again.
 */
void
enter_lpm_sleep(struct usbdev_sb *ch)
{
    if (PMUCTL_ENAB(ch->sih)) {
        ch->lpmminmask = R_REG(ch->osh, PMUREG(ch->sih, min_res_mask));

        switch (CHIPID(ch->sih->chip)) {
        case BCM4330_CHIP_ID :
        /* Set the min mask such that the XTAL_PU does not shut off */
            OR_REG(ch->osh, PMUREG(ch->sih, min_res_mask), PMURES_BIT(RES4330_XTAL_PU));
            ch_enter_suspend(ch);
            OR_REG(ch->osh, &ch->regs->clkctlstatus, CCS_FORCEILP);
            OR_REG(ch->osh, &ch->regs->clkctlstatus, CCS_FORCEHWREQOFF);
            break;
#ifndef BCM_BOOTLOADER
        case BCM4350_CHIP_ID:
        case BCM4354_CHIP_ID:
        case BCM43556_CHIP_ID:
        case BCM43558_CHIP_ID:
        case BCM43566_CHIP_ID:
        case BCM43568_CHIP_ID:
        case BCM43569_CHIP_ID:
            /* LPM cases for 4350 HSIC */
            /* Set the min mask such that the XTAL_PU does not shut off */
            if (ch_hsic(ch)) {
                OR_REG(ch->osh, PMUREG(ch->sih, min_res_mask),
                    PMURES_BIT(RES4350_XTAL_PU));
                ch_enter_suspend(ch);
                OR_REG(ch->osh, &ch->regs->clkctlstatus, CCS_FORCEILP);
                OR_REG(ch->osh, &ch->regs->clkctlstatus, CCS_FORCEHWREQOFF);
            }
            break;
#endif /* BCM_BOOTLOADER */
        }
    }
} /* enter_lpm_sleep */

/**
 * USBd core indicated it received a LPM transaction from the host requesting the dongle to wake up.
 */
void
leave_lpm_sleep(struct usbdev_sb *ch)
{
    if (PMUCTL_ENAB(ch->sih)) {
        switch (CHIPID(ch->sih->chip)) {
        case BCM4330_CHIP_ID :
            AND_REG(ch->osh, &ch->regs->clkctlstatus, ~CCS_FORCEHWREQOFF);
            AND_REG(ch->osh, &ch->regs->clkctlstatus, ~CCS_FORCEILP);
            ch_leave_suspend(ch);
            /* Change the min mask back to the origianl value */
            if (ch->lpmminmask) {
                W_REG(ch->osh, PMUREG(ch->sih, min_res_mask), ch->lpmminmask);
            }
            break;
#ifndef BCM_BOOTLOADER
        case BCM4350_CHIP_ID :
        case BCM4354_CHIP_ID :
        case BCM43556_CHIP_ID :
        case BCM43558_CHIP_ID :
        case BCM43566_CHIP_ID:
        case BCM43568_CHIP_ID:
        case BCM43569_CHIP_ID:
            if (ch_hsic(ch)) {
                AND_REG(ch->osh, &ch->regs->clkctlstatus, ~CCS_FORCEHWREQOFF);
                AND_REG(ch->osh, &ch->regs->clkctlstatus, ~CCS_FORCEILP);
                ch_leave_suspend(ch);
                /* Change the min mask back to the original value */
                if (ch->lpmminmask) {
                    W_REG(ch->osh, PMUREG(ch->sih, min_res_mask),
                        ch->lpmminmask);
                }
            }
            break;
#endif /* BCM_BOOTLOADER */
        }
    }
} /* leave_lpm_sleep */

/* do a soft disconnect on the bus to enable a re-connect */
void
ch_disconnect(struct usbdev_sb *ch)
{
    /* Reset endpoints */
    usbdev_reset(ch->bus);

#ifdef HARD_DISCONNECT
{
    uint32 devcontrol = R_REG(ch->osh, &ch->regs->devcontrol);
    ch->disconnected = TRUE;

    /* big hammer will kill default endpoint */
    ep_detach(ch, 0);

    W_REG(ch->osh, &ch->regs->clkctlstatus, CCS_FORCEHWREQOFF);
    (void) R_REG(ch->osh, &ch->regs->clkctlstatus);
    OSL_DELAY(200);

    /* Set Phy reset */
    devcontrol |= DC_PR;
    W_REG(ch->osh, &ch->regs->devcontrol, devcontrol);
    (void) R_REG(ch->osh, &ch->regs->devcontrol);
    OSL_DELAY(200);

    /* Set UTMISoftReset */
    /* PR77323 WAR: "Use clkgateAsyncEn on the 60MHz phy clock path" */
    if (ch->rev >= 9) {
        devcontrol |= DC_UR;
        W_REG(ch->osh, &ch->regs->devcontrol, devcontrol);
        (void) R_REG(ch->osh, &ch->regs->devcontrol);
        OSL_DELAY(400);
    }

    /* Power down PLL, UTMI and Analog */
    /* PR22173 : clear soft disconnect */
    devcontrol |= (DC_PL | DC_UP | DC_AP);
    W_REG(ch->osh, &ch->regs->devcontrol, devcontrol);
    (void) R_REG(ch->osh, &ch->regs->devcontrol);

    OSL_DELAY(1000);    /* wait 1ms */

    /* drop ForceHT request from core */
    ch_enter_suspend(ch);

    /* if we were Suspended, we are no longer */
    usbdev_resume(ch->bus);
    ch->suspended = FALSE;

    OSL_DELAY(1000);    /* wait 1ms */

    /* Disable the USB core */
    si_core_disable(ch->sih, si_core_cflags(ch->sih, 0, 0));
}
#else
    /*
     * set "soft disconnect"
     *
     * When this signal is asserted, the device enters the disconnect state if it is idle or
     * becomes idle. Software must assert this signal until the DevStatus.PhyMode changes to
     * non-driving mode (2'b01). In order to create the disconnect condition, both the phy and
     * the application clocks must be running.
     */
    OR_REG(ch->osh, &ch->regs->devcontrol, DC_DC);
    /* clear DevReady */
    AND_REG(ch->osh, &ch->regs->devcontrol, ~DC_US);
#endif /* HARD_DISCONNECT */
} /* ch_disconnect */

void
ch_devready(struct usbdev_sb *ch, uint32 delay)
{
#ifdef HARD_DISCONNECT
    ch_init(ch, TRUE, FALSE);
#else
    /* clear soft disconnect, if set */
    AND_REG(ch->osh, &ch->regs->devcontrol, ~DC_DC);
    if (delay)
        OSL_DELAY(delay);
    /* set DevReady */
    OR_REG(ch->osh, &ch->regs->devcontrol, DC_US);
    (void) R_REG(ch->osh, &ch->regs->devcontrol);
#endif /* HARD_DISCONNECT */
}

#ifdef WL_WOWL_MEDIA
void ch_wowldown(struct usbdev_sb *ch)
{
    uint32 devcontrol = 0;

    if (ch != NULL) {
        devcontrol = (DC_RS);
        W_REG(ch->osh, &ch->regs->devcontrol, devcontrol);
    }
}
#endif

#ifdef BCMUSBDEV_COMPOSITE
void ch_stall_ep(struct usbdev_sb *ch, int8 hw_ep)
{
    uint32 epstatus = R_REG(ch->osh, &ch->regs->epstatus);
    W_REG(ch->osh, &ch->regs->epstatus, epstatus | EPS_STALL(hw_ep));
}
#endif /* BCMUSBDEV_COMPOSITE */
