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
 * TRX image file header format.
 *
 * $Id: trxhdr.h 349211 2012-08-07 09:45:24Z chandrum $
 */

#ifndef _TRX_HDR_H
#define _TRX_HDR_H

#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TRX_MAGIC   0x30524448  /* "HDR0" */
#define TRX_MAX_LEN 0x3B0000    /* Max length */
#define TRX_NO_HEADER   1       /* Do not write TRX header */
#define TRX_GZ_FILES    0x2     /* Contains up to TRX_MAX_OFFSET individual gzip files */
#define TRX_EMBED_UCODE 0x8 /* Trx contains embedded ucode image */
#define TRX_ROMSIM_IMAGE    0x10    /* Trx contains ROM simulation image */
#define TRX_UNCOMP_IMAGE    0x20    /* Trx contains uncompressed rtecdc.bin image */
#define TRX_BOOTLOADER      0x40    /* the image is a bootloader */

#define TRX_V1      1
#define TRX_V1_MAX_OFFSETS  3       /* V1: Max number of individual files */

#ifndef BCMTRXV2
#define TRX_VERSION TRX_V1      /* Version 1 */
#define TRX_MAX_OFFSET TRX_V1_MAX_OFFSETS
#endif

/* BMAC Host driver/application like bcmdl need to support both Ver 1 as well as
 * Ver 2 of trx header. To make it generic, trx_header is structure is modified
 * as below where size of "offsets" field will vary as per the TRX version.
 * Currently, BMAC host driver and bcmdl are modified to support TRXV2 as well.
 * To make sure, other applications like "dhdl" which are yet to be enhanced to support
 * TRXV2 are not broken, new macro and structure defintion take effect only when BCMTRXV2
 * is defined.
 */
struct trx_header {
    uint32 magic;       /* "HDR0" */
    uint32 len;     /* Length of file including header */
    uint32 crc32;       /* 32-bit CRC from flag_version to end of file */
    uint32 flag_version;    /* 0:15 flags, 16:31 version */
#ifndef BCMTRXV2
    uint32 offsets[TRX_MAX_OFFSET]; /* Offsets of partitions from start of header */
#else
    uint32 offsets[1];  /* Offsets of partitions from start of header */
#endif
};

#ifdef BCMTRXV2
#define TRX_VERSION     TRX_V2      /* Version 2 */
#define TRX_MAX_OFFSET  TRX_V2_MAX_OFFSETS

#define TRX_V2      2
/* V2: Max number of individual files
 * To support SDR signature + Config data region
 */
#define TRX_V2_MAX_OFFSETS  5
#define SIZEOF_TRXHDR_V1    (sizeof(struct trx_header)+(TRX_V1_MAX_OFFSETS-1)*sizeof(uint32))
#define SIZEOF_TRXHDR_V2    (sizeof(struct trx_header)+(TRX_V2_MAX_OFFSETS-1)*sizeof(uint32))
#define TRX_VER(trx)        (trx->flag_version>>16)
#define ISTRX_V1(trx)       (TRX_VER(trx) == TRX_V1)
#define ISTRX_V2(trx)       (TRX_VER(trx) == TRX_V2)
/* For V2, return size of V2 size: others, return V1 size */
#define SIZEOF_TRX(trx)     (ISTRX_V2(trx) ? SIZEOF_TRXHDR_V2: SIZEOF_TRXHDR_V1)
#else
#define SIZEOF_TRX(trx)     (sizeof(struct trx_header))
#endif /* BCMTRXV2 */

/* Compatibility */
typedef struct trx_header TRXHDR, *PTRXHDR;

#ifdef __cplusplus
} /*extern "C" */
#endif

#endif /* _TRX_HDR_H */
