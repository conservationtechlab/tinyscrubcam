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
 * RDownload USB device
 *
 * $Id: usbdev_rdl.c 431828 2013-10-24 21:15:43Z rsomu $
 */

#define __need_wchar_t
#include <stddef.h>
#ifdef ZLIB
#include <zlib.h>
#include <zutil.h>
#endif
#include <typedefs.h>
#include <bcmendian.h>
#include <bcmutils.h>
#include <osl.h>
#include <trxhdr.h>
#include <usb.h>
#include <usbrdl.h>
#include <siutils.h>
#include <usbdev.h>
//#include <usbdev_dbg.h>
#include <bcmdevs.h>

#define trace       usbdev_isr_debug_printf
#define err         usbdev_isr_debug_printf
#define dbg_printf  usbdev_isr_debug_printf

#define DL_BASE (0x4a0800)
#define OSL_UNCACHED(va)    ((void *)(va))
#define BCM_ID_PRODUCT (0xbd28)

uint32 _memsize = 0x200000; //Useless value!!!

void
rdl_indicate_start(uint32 dlstart)
{
}

void
si_watchdog_ms(si_t *sih, uint32 ms)
{
}

/* XXX could use MEMORY_REMAP but will that work in all cases? */
#define MEM_REMAP_MASK 0xff000000

/* OS specifics */
#if 0
#if _HNDRTE_
#define INV_ICACHE()    blast_icache()
#define FLUSH_DCACHE()  blast_dcache()
#elif _CFE_
#define INV_ICACHE()    cfe_flushcache(CFE_CACHE_INVAL_I)
#define FLUSH_DCACHE()  cfe_flushcache(CFE_CACHE_FLUSH_D)
#else
#error "Need to define INV_ICACHE() & FLUSH_DCACHE()"
#endif
#else
#define INV_ICACHE()
#define FLUSH_DCACHE()
#endif

#define Z_DEFLATED   8      /* Deflated */
#define ASCII_FLAG   0x01   /* bit 0 set: file probably ascii text */
#define HEAD_CRC     0x02   /* bit 1 set: header CRC present */
#define EXTRA_FIELD  0x04   /* bit 2 set: extra field present */
#define ORIG_NAME    0x08   /* bit 3 set: original file name present */
#define COMMENT      0x10   /* bit 4 set: file comment present */
#define RESERVED     0xE0   /* bits 5..7: reserved */
#define GZIP_TRL_LEN 8      /* 8 bytes, |crc|len| */

extern void rdl_indicate_start(uint32 dlstart);
//extern hndrte_dev_t bcmrdl;
extern bool validate_sdr_image(struct trx_header *hdr);

#define EP_BULK_MAX 4
#ifdef BCMUSBDEV_COMPOSITE
#define EP_INTR_MAX 2
#define EP_ISO_MAX  2
#define INTERFACE_BT    0   /* default BT interface 0 */
#define INTERFACE_ISO   1   /* BT/ISO interface 1 */
#define INTERFACE_DFU   2   /* BT/DFU interface 2 */
#define INTERFACE_MAX   9   /* composite device: 1 interface for WLAN, rest for BT */
#define ALT_SETTING_MAX 5   /* alternate settings for BT ISO interface */
#define IAD_INT_NUM 0   /* IAD for grouping BT interface 0 onwards */
#endif /* BCMUSBDEV_COMPOSITE */

#define BT_BULK_OUT_NUM(usbdesc) (usbdesc & 0x8000)
#define BT_BULK_IN_NUM(usbdesc) (usbdesc & 0x8000)

struct dngl_bus {
    osl_t *osh;     /* Driver handle */
    void *ch;   /* Chip handle */
    si_t *sih;      /* sb handle */
    const usb_interface_descriptor_t *data_interface;
    usb_device_descriptor_t device;
#ifndef BCMUSBDEV_COMPOSITE
    usb_string_descriptor_t strings[4];
#endif /* BCMUSBDEV_COMPOSITE */
    usb_config_descriptor_t config;
    usb_config_descriptor_t other_config;
    int confignum;
    int interface;
#ifndef BCMUSBDEV_COMPOSITE
    uint ep_ii_num;         /* interrupt In */
#endif /* BCMUSBDEV_COMPOSITE */
    uint ep_bi[EP_BULK_MAX];    /* bulk in */
#ifndef BCMUSBDEV_COMPOSITE
    uint ep_bi_num;
#endif /* BCMUSBDEV_COMPOSITE */
    uint ep_bo[EP_BULK_MAX];    /* bulk out */
#ifndef BCMUSBDEV_COMPOSITE
    uint ep_bo_num;
#ifdef USB_XDCI
    usb_endpoint_descriptor_t data_endpoints_ss[EP_BULK_MAX*2];
    usb_endpoint_companion_descriptor_t data_endpoints_cp_ss[EP_BULK_MAX*2];
#endif /* USB_XDCI */
    usb_endpoint_descriptor_t data_endpoints[EP_BULK_MAX*2];
    usb_endpoint_descriptor_t data_endpoints_fs[EP_BULK_MAX*2]; /* fullspeed endpoints */
#endif /* BCMUSBDEV_COMPOSITE */
    uint speed;
    bool rndis;
    int nvramoff;
    int nvramsz;
    int state;
    uint32 dlbase;
    uint32 dlfwlen;
    uint32 jumpto;
    uint32 dlcrc;
    uint32 dllen;
    uchar *dlcurrentbase;
    uint32 dlcurrentlen;
#ifdef ZLIB
    bool    zlibinit;
    z_stream d_stream; /* decompression stream */
    int zstatus;
    int trl_idx;
    uint8 trl[GZIP_TRL_LEN];
#endif /* ZLIB */
    uint8 comp_image;
    char var[QUERY_STRING_MAX+1];
#ifdef BCMUSBDEV_COMPOSITE
    usb_string_descriptor_t strings[8];
    uint interface_num;     /* interface count */
    uint ep_ii_num[INTERFACE_MAX];  /* interrupt-in ep per interface */
    uint ep_bi_num[INTERFACE_MAX];  /* bulk-in ep per interface */
    uint ep_bo_num[INTERFACE_MAX];  /* bulk-out ep per interface */
    uint ep_isi[EP_ISO_MAX];    /* iso-in ep */
    uint ep_isi_num[INTERFACE_MAX]; /* iso-in ep per interface */
    uint ep_iso[EP_ISO_MAX];    /* iso-out ep */
    uint ep_iso_num[INTERFACE_MAX]; /* iso-out ep per interface */
    usb_interface_association_descriptor_t *iad;
#ifdef USB_XDCI
    usb_endpoint_descriptor_t data_endpoints_ss[EP_BULK_MAX*INTERFACE_MAX];
    usb_endpoint_companion_descriptor_t data_endpoints_cp_ss[EP_BULK_MAX*INTERFACE_MAX];
    usb_endpoint_descriptor_t intr_endpoints_ss[INTERFACE_MAX];
    usb_endpoint_companion_descriptor_t intr_endpoints_cp_ss[INTERFACE_MAX];
#endif /* USB_XDCI */
    /* high speed endpoints */
    usb_endpoint_descriptor_t intr_endpoints[INTERFACE_MAX];
    usb_endpoint_descriptor_t data_endpoints[EP_BULK_MAX*INTERFACE_MAX];
    /* full speed endpoints */
    usb_endpoint_descriptor_t intr_endpoints_fs[INTERFACE_MAX];
    usb_endpoint_descriptor_t data_endpoints_fs[EP_BULK_MAX*INTERFACE_MAX];
    int int_wlan;
#endif /* BCMUSBDEV_COMPOSITE */
    uint32 hdrlen;
    bool usb30d;
};

#ifdef BCMUSBDEV_COMPOSITE
static void usbdev_set_ep_defaults(struct dngl_bus *bus, int int_num);
static void usbdev_set_iso_alt_ep_maxsize(usb_endpoint_descriptor_t* d, int alt_int);
#endif /* BCMUSBDEV_COMPOSITE */
static int usbd_chk_version(struct dngl_bus *bus);
static bool usbd_hsic(struct dngl_bus *bus);
static uint usbd_ep_attach(struct dngl_bus *bus, const usb_endpoint_descriptor_t *endpoint,
    usb_endpoint_companion_descriptor_t *sscmp,
    int config, int interface, int alternate);
static void usbd_ep_detach(struct dngl_bus *bus, int ep);
#ifdef USB_IFTEST
static int usbd_resume(struct dngl_bus *bus);
#endif

typedef void *rdl_t;

/* default values for config descriptors */
#define DEV_ATTRIBUTES (UC_BUS_POWERED | UC_REMOTE_WAKEUP)
#define DEV_MAXPOWER (200 / UC_POWER_FACTOR)
#define DEV_MAXPOWER_500 (500 / UC_POWER_FACTOR)
#define DEV_MAXPOWER_900 (900 / UC_SSPOWER_FACTOR)

/*
 * Remote downloader personality: upto 4 interfaces:
 *   1 WLAN, upto 3 BT interfaces
 */

#ifdef BCMUSBDEV_COMPOSITE
static usb_device_descriptor_t rdl_device = {
#else
static const usb_device_descriptor_t rdl_device = {
#endif /* BCMUSBDEV_COMPOSITE */
    bLength: USB_DEVICE_DESCRIPTOR_SIZE,
    bDescriptorType: UDESC_DEVICE,
    bcdUSB: UD_USB_2_0,
    bDeviceClass: UDCLASS_VENDOR,
    bDeviceSubClass: 0,
    bDeviceProtocol: 0,
    bMaxPacketSize: USB_2_MAX_CTRL_PACKET,
    idVendor: BCM_DNGL_VID,
    idProduct: BCM_ID_PRODUCT,
    bcdDevice: 0x0001,
    iManufacturer: 1,
    iProduct: 2,
    iSerialNumber: 3,
    bNumConfigurations: 1
};

#define RDL_CONFIGLEN       (USB_CONFIG_DESCRIPTOR_SIZE + \
                USB_INTERFACE_DESCRIPTOR_SIZE + \
                (USB_ENDPOINT_DESCRIPTOR_SIZE * 3))

static usb_config_descriptor_t rdl_config = {
    bLength: USB_CONFIG_DESCRIPTOR_SIZE,
    bDescriptorType: UDESC_CONFIG,
    wTotalLength: RDL_CONFIGLEN,
    bNumInterface: 1,
    bConfigurationValue: 1,
    iConfiguration: 0,
    bmAttributes: DEV_ATTRIBUTES,
    bMaxPower: DEV_MAXPOWER
};

static usb_config_descriptor_t rdl_other_config = {
    bLength: USB_CONFIG_DESCRIPTOR_SIZE,
    bDescriptorType: UDESC_OTHER_SPEED_CONFIGURATION,
    wTotalLength: RDL_CONFIGLEN,
    bNumInterface: 1,
    bConfigurationValue: 1,
    iConfiguration: 0,
    bmAttributes: DEV_ATTRIBUTES,
    bMaxPower: DEV_MAXPOWER
};

#ifdef BCMUSBDEV_COMPOSITE
static usb_interface_association_descriptor_t rdl_iad = {
    bLength: USB_INTERFACE_ASSOCIATION_DESCRIPTOR_SIZE,
    bDescriptorType: UDESC_IAD,
    bFirstInterface: IAD_INT_NUM,
    bInterfaceCount: 0,
    bFunctionClass: UICLASS_WIRELESS,
    bFunctionSubClass: UISUBCLASS_RF,
    bFunctionProtocol: UIPROTO_BLUETOOTH,
    iFunction: 5
};

static usb_interface_descriptor_t rdl_interfaces[] = {
    {
        bLength: USB_INTERFACE_DESCRIPTOR_SIZE,
        bDescriptorType: UDESC_INTERFACE,
        bInterfaceNumber: 0,
        bAlternateSetting: 0,
        bInterfaceClass: UICLASS_WIRELESS,
        bInterfaceSubClass: UISUBCLASS_RF,
        bInterfaceProtocol: UIPROTO_BLUETOOTH,
        iInterface: 6
    },
    {
        bLength: USB_INTERFACE_DESCRIPTOR_SIZE,
        bDescriptorType: UDESC_INTERFACE,
        bInterfaceNumber: 1,
        bAlternateSetting: 0,
        bInterfaceClass: UICLASS_WIRELESS,
        bInterfaceSubClass: UISUBCLASS_RF,
        bInterfaceProtocol: UIPROTO_BLUETOOTH,
        iInterface: 6
    },
    {
        bLength: USB_INTERFACE_DESCRIPTOR_SIZE,
        bDescriptorType: UDESC_INTERFACE,
        bInterfaceNumber: 1,
        bAlternateSetting: 1,
        bInterfaceClass: UICLASS_WIRELESS,
        bInterfaceSubClass: UISUBCLASS_RF,
        bInterfaceProtocol: UIPROTO_BLUETOOTH,
        iInterface: 6
    },
    {
        bLength: USB_INTERFACE_DESCRIPTOR_SIZE,
        bDescriptorType: UDESC_INTERFACE,
        bInterfaceNumber: 1,
        bAlternateSetting: 2,
        bInterfaceClass: UICLASS_WIRELESS,
        bInterfaceSubClass: UISUBCLASS_RF,
        bInterfaceProtocol: UIPROTO_BLUETOOTH,
        iInterface: 6
    },
    {
        bLength: USB_INTERFACE_DESCRIPTOR_SIZE,
        bDescriptorType: UDESC_INTERFACE,
        bInterfaceNumber: 1,
        bAlternateSetting: 3,
        bInterfaceClass: UICLASS_WIRELESS,
        bInterfaceSubClass: UISUBCLASS_RF,
        bInterfaceProtocol: UIPROTO_BLUETOOTH,
        iInterface: 6
    },
    {
        bLength: USB_INTERFACE_DESCRIPTOR_SIZE,
        bDescriptorType: UDESC_INTERFACE,
        bInterfaceNumber: 1,
        bAlternateSetting: 4,
        bInterfaceClass: UICLASS_WIRELESS,
        bInterfaceSubClass: UISUBCLASS_RF,
        bInterfaceProtocol: UIPROTO_BLUETOOTH,
        iInterface: 6
    },
    {
        bLength: USB_INTERFACE_DESCRIPTOR_SIZE,
        bDescriptorType: UDESC_INTERFACE,
        bInterfaceNumber: 1,
        bAlternateSetting: 5,
        bInterfaceClass: UICLASS_WIRELESS,
        bInterfaceSubClass: UISUBCLASS_RF,
        bInterfaceProtocol: UIPROTO_BLUETOOTH,
        iInterface: 6
    },
    {
        bLength: USB_INTERFACE_DESCRIPTOR_SIZE,
        bDescriptorType: UDESC_INTERFACE,
        bInterfaceNumber: 2,
        bAlternateSetting: 0,
        bInterfaceClass: UICLASS_WIRELESS,
        bInterfaceSubClass: UISUBCLASS_RF,
        bInterfaceProtocol: UIPROTO_BLUETOOTH,
        iInterface: 7
    },
    {
        bLength: USB_INTERFACE_DESCRIPTOR_SIZE,
        bDescriptorType: UDESC_INTERFACE,
        bInterfaceNumber: 3,
        bAlternateSetting: 0,
        bInterfaceClass: UICLASS_VENDOR,
        bInterfaceSubClass: UISUBCLASS_ABSTRACT_CONTROL_MODEL,
        bInterfaceProtocol: UIPROTO_DATA_VENDOR,
        iInterface: 2
    }
};
#else
static usb_interface_descriptor_t rdl_interface = {
    bLength: USB_INTERFACE_DESCRIPTOR_SIZE,
    bDescriptorType: UDESC_INTERFACE,
    bInterfaceNumber: 0,
    bAlternateSetting: 0,
    bNumEndpoints: 3,
    bInterfaceClass: UICLASS_VENDOR,
    bInterfaceSubClass: UISUBCLASS_ABSTRACT_CONTROL_MODEL,
    bInterfaceProtocol: 0xff,
    iInterface: 0
};
#endif /* BCMUSBDEV_COMPOSITE */

#ifdef USB_XDCI
/* SS mode supports 4 types of descriptor: device, configuration, BOS and string */
static const usb_bos_descriptor_t rdl_bos_desc = {
    bLength: USB_BOS_DESCRIPTOR_SIZE,
    bDescriptorType: UDESC_BOS,
    wTotalLength: USB_BOS_DESCRIPTOR_LENGTH,
    bNumDeviceCaps: 0x02
};

static const usb_2_extension_descriptor_t rdl_usb2_ext = {
    bLength: USB_2_EXTENSION_DESCRIPTOR_SIZE,
    bDescriptorType: UDESC_CAP,
    bDevCapabilityType: 0x02,
    bmAttributes: 0x02
};

static const usb_ss_device_capability_descriptor_t rdl_ss_cap = {
    bLength: USB_SS_DEVICE_CAPABILITY_DESCRIPTOR_SIZE,
    bDescriptorType: UDESC_CAP,
    bDevCapabilityType: 0x03,
    bmAttributes: 0x00,
    wSpeedsSupport: 0x0e,  /* supoort SS/HS/FS speed */
    bFunctionalitySupport: 0x01,
    bU1DevExitLat: 0x0A,
    wU2DevExitLat: 0x07FF
};

/* SS mode reports the other speed it support via the BOS descritpor and shall not */
/* support the device_qualifier and other_speed_configuration descriptor              */
static const usb_device_qualifier_t rdl_ss_qualifier = {
    bLength: USB_DEVICE_QUALIFIER_SIZE,
    bDescriptorType: UDESC_DEVICE_QUALIFIER,
    bcdUSB: UD_USB_3_0,
    bDeviceClass: UDCLASS_VENDOR,
    bDeviceSubClass: 0,
    bDeviceProtocol: 0,
    bMaxPacketSize0: USB_2_MAX_CTRL_PACKET,
    bNumConfigurations: 1,
    bReserved: 0
};

static usb_endpoint_descriptor_t rdl_ss_ep_intr = {
    bLength: USB_ENDPOINT_DESCRIPTOR_SIZE,
    bDescriptorType: UDESC_ENDPOINT,
    bEndpointAddress: UE_DIR_IN | 1,
    bmAttributes: UE_INTERRUPT,
    wMaxPacketSize: 64,
    bInterval: 4
};
static usb_endpoint_companion_descriptor_t rdl_ss_cp_intr = {
    bLength: USB_ENDPOINT_COMPANION_DESCRIPTOR_SIZE,
    bDescriptorType: UDESC_ENDPOINT_COMPANION,
    bMaxBurst: 0x0,
    bmAttributes: 0,
    wBytesPerInterVal: 64,
};

static usb_endpoint_descriptor_t rdl_ss_ep_bulkin = {
    bLength: USB_ENDPOINT_DESCRIPTOR_SIZE,
    bDescriptorType: UDESC_ENDPOINT,
    bEndpointAddress: UE_DIR_IN | 2,
    bmAttributes: UE_BULK,
    wMaxPacketSize: USB_3_MAX_BULK_PACKET,
    bInterval: 0
};
static usb_endpoint_companion_descriptor_t rdl_ss_cp_bulkin = {
    bLength: USB_ENDPOINT_COMPANION_DESCRIPTOR_SIZE,
    bDescriptorType: UDESC_ENDPOINT_COMPANION,
    bMaxBurst: 1,
    bmAttributes: 0,
    wBytesPerInterVal: 0,
};

static usb_endpoint_descriptor_t rdl_ss_ep_bulkout = {
    bLength: USB_ENDPOINT_DESCRIPTOR_SIZE,
    bDescriptorType: UDESC_ENDPOINT,
    bEndpointAddress: UE_DIR_OUT | 3,
    bmAttributes: UE_BULK,
    wMaxPacketSize: USB_3_MAX_BULK_PACKET,
    bInterval: 0
};
static usb_endpoint_companion_descriptor_t rdl_ss_cp_bulkout = {
    bLength: USB_ENDPOINT_COMPANION_DESCRIPTOR_SIZE,
    bDescriptorType: UDESC_ENDPOINT_COMPANION,
    bMaxBurst: 1,
    bmAttributes: 0,
    wBytesPerInterVal: 0,
};
#endif /* USB_XDCI */

static usb_endpoint_descriptor_t rdl_fs_ep_intr = {
        bLength: USB_ENDPOINT_DESCRIPTOR_SIZE,
        bDescriptorType: UDESC_ENDPOINT,
        bEndpointAddress: UE_DIR_IN | 1,
        bmAttributes: UE_INTERRUPT,
        wMaxPacketSize: 16,
        bInterval: 1
};

static usb_endpoint_descriptor_t rdl_fs_ep_bulkin = {
        bLength: USB_ENDPOINT_DESCRIPTOR_SIZE,
        bDescriptorType: UDESC_ENDPOINT,
        bEndpointAddress: UE_DIR_IN | 2,
        bmAttributes: UE_BULK,
        wMaxPacketSize: 64,
        bInterval: 0
};

static usb_endpoint_descriptor_t rdl_fs_ep_bulkout = {
        bLength: USB_ENDPOINT_DESCRIPTOR_SIZE,
        bDescriptorType: UDESC_ENDPOINT,
        bEndpointAddress: UE_DIR_OUT | 3,
        bmAttributes: UE_BULK,
        wMaxPacketSize: 64,
        bInterval: 0
};

#ifdef BCMUSBDEV_COMPOSITE
#ifdef USB_XDCI
static const usb_endpoint_descriptor_t rdl_ss_ep_isin = {
    bLength: USB_ENDPOINT_DESCRIPTOR_SIZE,
    bDescriptorType: UDESC_ENDPOINT,
    bEndpointAddress: UE_DIR_IN | 4,
    bmAttributes: UE_ISOCHRONOUS,
    wMaxPacketSize: USB_3_MAX_ISO_PACKET,
    bInterval: 1
};
static usb_endpoint_companion_descriptor_t rdl_ss_cp_isin = {
    bLength: USB_ENDPOINT_COMPANION_DESCRIPTOR_SIZE,
    bDescriptorType: UDESC_ENDPOINT_COMPANION,
    bMaxBurst: 0,
    bmAttributes: 0, /* B[0,1]:MULT=>Max packets = (bMasBurst+1)*(Mult+1) */
    wBytesPerInterVal: 1024,
};

static const usb_endpoint_descriptor_t rdl_ss_ep_isout = {
    bLength: USB_ENDPOINT_DESCRIPTOR_SIZE,
    bDescriptorType: UDESC_ENDPOINT,
    bEndpointAddress: UE_DIR_OUT | 5,
    bmAttributes: UE_ISOCHRONOUS,
    wMaxPacketSize: USB_3_MAX_ISO_PACKET,
    bInterval: 1
};
static usb_endpoint_companion_descriptor_t rdl_ss_cp_isout = {
    bLength: USB_ENDPOINT_COMPANION_DESCRIPTOR_SIZE,
    bDescriptorType: UDESC_ENDPOINT_COMPANION,
    bMaxBurst: 0,
    bmAttributes: 0,
    wBytesPerInterVal: 1024,
};
#endif /* USB_XDCI */

static const usb_endpoint_descriptor_t rdl_hs_ep_isin = {
    bLength: USB_ENDPOINT_DESCRIPTOR_SIZE,
    bDescriptorType: UDESC_ENDPOINT,
    bEndpointAddress: UE_DIR_IN | 4,
    bmAttributes: UE_ISOCHRONOUS,
    wMaxPacketSize: USB_MAX_BT_ISO_PACKET,
    bInterval: 1
};

static const usb_endpoint_descriptor_t rdl_hs_ep_isout = {
    bLength: USB_ENDPOINT_DESCRIPTOR_SIZE,
    bDescriptorType: UDESC_ENDPOINT,
    bEndpointAddress: UE_DIR_OUT | 5,
    bmAttributes: UE_ISOCHRONOUS,
    wMaxPacketSize: USB_MAX_BT_ISO_PACKET,
    bInterval: 1
};
#endif /* BCMUSBDEV_COMPOSITE */

static usb_endpoint_descriptor_t rdl_hs_ep_intr = {
        bLength: USB_ENDPOINT_DESCRIPTOR_SIZE,
        bDescriptorType: UDESC_ENDPOINT,
        bEndpointAddress: UE_DIR_IN | 1,
        bmAttributes: UE_INTERRUPT,
        wMaxPacketSize: 16,
        bInterval: 4
};

static usb_endpoint_descriptor_t rdl_hs_ep_bulkin = {
        bLength: USB_ENDPOINT_DESCRIPTOR_SIZE,
        bDescriptorType: UDESC_ENDPOINT,
        bEndpointAddress: UE_DIR_IN | 2,
        bmAttributes: UE_BULK,
        wMaxPacketSize: 512,
        bInterval: 0
};

static usb_endpoint_descriptor_t rdl_hs_ep_bulkout = {
        bLength: USB_ENDPOINT_DESCRIPTOR_SIZE,
        bDescriptorType: UDESC_ENDPOINT,
        bEndpointAddress: UE_DIR_OUT | 3,
        bmAttributes: UE_BULK,
        wMaxPacketSize: 512,
        bInterval: 1
};

#ifdef BCMUSBDEV_COMPOSITE
static const usb_endpoint_descriptor_t rdl_fs_ep_isin = {
    bLength: USB_ENDPOINT_DESCRIPTOR_SIZE,
    bDescriptorType: UDESC_ENDPOINT,
    bEndpointAddress: UE_DIR_IN | 4,
    bmAttributes: UE_ISOCHRONOUS,
    wMaxPacketSize: USB_1_MAX_ISO_PACKET,
    bInterval: 1
};

static const usb_endpoint_descriptor_t rdl_fs_ep_isout = {
    bLength: USB_ENDPOINT_DESCRIPTOR_SIZE,
    bDescriptorType: UDESC_ENDPOINT,
    bEndpointAddress: UE_DIR_OUT | 5,
    bmAttributes: UE_ISOCHRONOUS,
    wMaxPacketSize: USB_1_MAX_ISO_PACKET,
    bInterval: 1
};

static const usb_dfu_functional_descriptor_t rdl_dfu = {
    bLength: USB_DFU_FUNCTIONAL_DESCRIPTOR_SIZE,
    bDescriptorType: UDESC_DFU,
    /* download, execute device requests after FW update without reboot */
    bmAttributes: 0x05,
    wDetachTimeOut: 0x1388,         /* 5 ms timeout */
    wTransferSize: USB_2_MAX_CTRL_PACKET,
    bcdDFUVersion: 0x0100
};
#endif /* BCMUSBDEV_COMPOSITE */

#ifdef BCMUSBDEV_COMPOSITE
static usb_device_qualifier_t rdl_fs_qualifier = {
#else
static const usb_device_qualifier_t rdl_fs_qualifier = {
#endif /* BCMUSBDEV_COMPOSITE */
    bLength: USB_DEVICE_QUALIFIER_SIZE,
    bDescriptorType: UDESC_DEVICE_QUALIFIER,
    bcdUSB: UD_USB_2_0,
    bDeviceClass: UDCLASS_VENDOR,
    bDeviceSubClass: 0,
    bDeviceProtocol: 0,
    bMaxPacketSize0: USB_2_MAX_CTRL_PACKET,
    bNumConfigurations: 1,
    bReserved: 0
};

#ifdef BCMUSBDEV_COMPOSITE
static usb_device_qualifier_t rdl_hs_qualifier = {
#else
static const usb_device_qualifier_t rdl_hs_qualifier = {
#endif /* BCMUSBDEV_COMPOSITE */
    bLength: USB_DEVICE_QUALIFIER_SIZE,
    bDescriptorType: UDESC_DEVICE_QUALIFIER,
    bcdUSB: UD_USB_2_0,
    bDeviceClass: UDCLASS_VENDOR,
    bDeviceSubClass: 0,
    bDeviceProtocol: 0,
    bMaxPacketSize0: USB_2_MAX_CTRL_PACKET,
    bNumConfigurations: 1,
    bReserved: 0
};


/* UTF16 (not NULL terminated) strings */
static const usb_string_descriptor_t rdl_strings[] = {
    {
#ifdef BCMUSBDEV_COMPOSITE
        bLength: 0x08,
#else
        bLength: 0x04,
#endif /* BCMUSBDEV_COMPOSITE */
        bDescriptorType: UDESC_STRING,
        bString: { 0x409 }
    },
    {
        /* USB spec: bLength is bString size + 2 */
        bLength: sizeof(L"Broadcom"),
        bDescriptorType: UDESC_STRING,
        bString: { 'B', 'r', 'o', 'a', 'd', 'c', 'o', 'm' }
    },
    {
        /* USB spec: bLength is bString size + 2 */
        bLength: sizeof(L"Remote Download Wireless Adapter"),
        bDescriptorType: UDESC_STRING,
        bString: {
            'R', 'e', 'm', 'o', 't', 'e', ' ',
            'D', 'o', 'w', 'n', 'l', 'o', 'a', 'd', ' ',
            'W', 'i', 'r', 'e', 'l', 'e', 's', 's', ' ',
            'A', 'd', 'a', 'p', 't', 'e', 'r'
        }
    },
    {
        /* USB spec: bLength is bString size + 2 */
        bLength: sizeof(L"000000000001"),
        bDescriptorType: UDESC_STRING,
        bString: { '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1' }
    },
#ifdef BCMUSBDEV_COMPOSITE
    {
        bLength: sizeof(L"Composite Wireless Adapter"),
        bDescriptorType: UDESC_STRING,
        bString: {
            'C', 'o', 'm', 'p', 'o', 's', 'i', 't', 'e', ' ',
            'W', 'i', 'r', 'e', 'l', 'e', 's', 's', ' ',
            'A', 'd', 'a', 'p', 't', 'e', 'r'
        }
    },
    {
        bLength: sizeof(L"Bluetooth MI"),
        bDescriptorType: UDESC_STRING,
        bString: {
            'B', 'l', 'u', 'e', 't', 'o', 'o', 't', 'h', ' ', 'M', 'I'
        }
    },
    {
        bLength: sizeof(L"Bluetooth Interface"),
        bDescriptorType: UDESC_STRING,
        bString: {
            'B', 'l', 'u', 'e', 't', 'o', 'o', 't', 'h', ' ',
            'I', 'n', 't', 'e', 'r', 'f', 'a', 'c', 'e'
        }
    },
    {
        bLength: sizeof(L"DFU Interface"),
        bDescriptorType: UDESC_STRING,
        bString: {
            'D', 'F', 'U', ' ', 'I', 'n', 't', 'e', 'r', 'f', 'a', 'c', 'e'
        }
    }
#endif /* BCMUSBDEV_COMPOSITE */
};

/* indirect calls to in-line functions to save code space */
static int
usbdev_htol_usb_config_descriptor(const usb_config_descriptor_t *d, uchar *buf)
{
    return htol_usb_config_descriptor(d, buf);
}

#ifdef BCMUSBDEV_COMPOSITE
static int
usbdev_htol_usb_iad(const usb_interface_association_descriptor_t *d, uchar *buf)
{
    return htol_usb_interface_association_descriptor(d, buf);
}

static int
usbdev_htol_usb_dfu_functional_descriptor(const usb_dfu_functional_descriptor_t *d, uchar *buf)
{
    return htol_usb_dfu_functional_descriptor(d, buf);
}
#endif /* BCMUSBDEV_COMPOSITE */

static int
usbdev_htol_usb_interface_descriptor(const usb_interface_descriptor_t *d, uchar *buf)
{
    return htol_usb_interface_descriptor(d, buf);
}

static int
usbdev_htol_usb_endpoint_descriptor(const usb_endpoint_descriptor_t *d, uchar *buf)
{
    return htol_usb_endpoint_descriptor(d, buf);
}

static int
usbdev_htol_usb_device_qualifier(const usb_device_qualifier_t *d, uchar *buf)
{
    return htol_usb_device_qualifier(d, buf);
}

#ifdef USB_XDCI
static int
usbdev_htol_usb_ed_companion_descriptor(const usb_endpoint_companion_descriptor_t *d, uchar *buf)
{
    return htol_usb_endpoint_companion_descriptor(d, buf);
}

static int
usbdev_htol_usb_bos_descriptor(const usb_bos_descriptor_t *d, uchar *buf)
{
    return htol_usb_bos_descriptor(d, buf);
}
static int
usbdev_htol_usb_ss_device_capacity_descritor(
    const usb_ss_device_capability_descriptor_t *d, uchar *buf)
{
    return htol_usb_ss_device_capability_descriptor(d, buf);
}

static int
usbdev_htol_usb_2_extension_descritor(const usb_2_extension_descriptor_t *d, uchar *buf)
{
    return htol_usb_2_extension_descriptor(d, buf);
}
void usbdev_attach_xdci(struct dngl_bus *bus)
{
}
void usbdev_detach_xdci(struct dngl_bus *bus)
{
}
#endif /* USB_XDCI */

static int check_headers(struct dngl_bus *dev, unsigned char *headers);
#ifdef ZLIB
static void init_zlib_state(struct dngl_bus *dev);
#endif

/* Reset download/decompression state */
static void
usbdev_rdl_dl_state_reset(struct dngl_bus *dev)
{
    if (dev->state == DL_WAITING)
        return;

    /* reset state */
    dev->state = DL_WAITING;
    dev->dlbase = 0;
    dev->jumpto = 0;
    dev->nvramsz = 0;
    dev->nvramoff = 0;
    dev->dlcurrentbase = NULL;
    dev->dlcurrentlen = 0;
    dev->dlcrc = 0;
    dev->dllen = 0;
    bzero(dev->var, QUERY_STRING_MAX+1);
}

#ifdef ZLIB
static void
init_zlib_state(struct dngl_bus *dev)
{
    if (dev->zlibinit) {
        inflateEnd(&(dev->d_stream));
        dev->zlibinit = FALSE;
    }

    dev->zstatus = 0;
    dev->trl_idx = 0;

    /* Initialise the decompression struct */
    dev->d_stream.next_in = NULL;
    dev->d_stream.avail_in = 0;
    dev->d_stream.next_out = (uchar*)0x80001000;
    dev->d_stream.avail_out = 0x4000000;    /* XXX: 4M max assumed */
    dev->d_stream.zalloc = (alloc_func)0;
    dev->d_stream.zfree = (free_func)0;
    if (inflateInit2(&(dev->d_stream), -(DEF_WBITS)) != Z_OK) {
        err("Err: inflateInit2: %d");
        dev->state = DL_START_FAIL;
    } else
        dev->zlibinit = TRUE;
}
#endif /* ZLIB */

typedef struct {
    int index;
    char *name;
} cusstr_t;

#ifdef USB_IFTEST
static void
rdl_initiate_resume(void *dev)
{
    usbd_resume((struct dngl_bus *)dev);
}
#endif /* USB_IFTEST */

#ifdef BCMUSBDEV_COMPOSITE
static void
usbdev_set_iso_alt_ep_maxsize(usb_endpoint_descriptor_t* d, int alt_setting)
{
    ASSERT(alt_setting >= INTERFACE_ISO);

    switch (alt_setting) {
    case 1:
        d->wMaxPacketSize = USB_MAX_ALT_INT_0_ISO_PACKET;
        break;

    case 2:
        d->wMaxPacketSize = USB_MAX_ALT_INT_1_ISO_PACKET;
        break;

    case 3:
        d->wMaxPacketSize = USB_MAX_ALT_INT_2_ISO_PACKET;
        break;

    case 4:
        d->wMaxPacketSize = USB_MAX_ALT_INT_3_ISO_PACKET;
        break;

    case 5:
        d->wMaxPacketSize = USB_MAX_ALT_INT_4_ISO_PACKET;
        break;

    case 6:
        d->wMaxPacketSize = USB_MAX_ALT_INT_5_ISO_PACKET;
        break;

    default:
        err("invalid size, setting max ISO mps %d", USB_MAX_BT_ISO_PACKET);
        d->wMaxPacketSize = USB_MAX_BT_ISO_PACKET;
        break;
    }
}

/* Setup endpoint defaults for WLAN and optional BT interface */
static void
usbdev_set_ep_defaults(struct dngl_bus *dev, int int_num)
{
    int i, j, k;
    int offset = 0;

    /* BT interface #0: intr-in, bulk-in, bulk-out */
    /* rdl_config.bNumInterface is total interface no, no include ALT */
    if (int_num > 1) {
        /* mutiple interface and WLAN at interface 0 */
        if (dev->int_wlan == 0) {
            offset = 1;
        }
    }
    j = int_num - 1 + offset;
    if (j > (INTERFACE_BT + offset)) {
        i = (INTERFACE_BT + offset);
        dev->ep_ii_num[i] = 1;
        dev->ep_bi_num[i] = 1;
        dev->ep_bo_num[i] = 1;
        dev->ep_isi_num[i] = 0;
        dev->ep_iso_num[i] = 0;
    }

    /* BT interface #1/alt settings: iso-in, iso-out */
    if (j > (INTERFACE_ISO + offset)) {
        k = INTERFACE_ISO + offset;
        for (i = k; i <= k + ALT_SETTING_MAX; i++) {
            dev->ep_ii_num[i] = 0;
            dev->ep_bi_num[i] = 0;
            dev->ep_bo_num[i] = 0;
            dev->ep_isi_num[i] = 1;
            dev->ep_iso_num[i] = 1;
        }
    }

    /* BT interface #2: DFU/0 endpoints */
    if (j > (INTERFACE_DFU + offset)) {
        i = INTERFACE_DFU + ALT_SETTING_MAX + offset;
        dev->ep_ii_num[i] = 0;
        dev->ep_bi_num[i] = 0;
        dev->ep_bo_num[i] = 0;
        dev->ep_isi_num[i] = 0;
        dev->ep_iso_num[i] = 0;
    }

    /* WLAN interface #dynamic: intr-in, bulk-in, bulk-out */
    if (j >= INTERFACE_BT) {
        if (j > INTERFACE_ISO)
            i = j + ALT_SETTING_MAX;
        else
            i = j;

        if (dev->int_wlan == 0) {
            i = 0;
        }

        dev->ep_ii_num[i] = 1;
        dev->ep_bi_num[i] = 1;
        dev->ep_bo_num[i] = 1;
        dev->ep_isi_num[i] = 0;
        dev->ep_iso_num[i] = 0;
    }
}
#endif /* BCMUSBDEV_COMPOSITE */

static void
usbdev_attach_interface_ep_init(struct dngl_bus *dev)
{
    const char *value;
    int val;
    uint i, j;
    uint NEWRDL_CONFIGLEN;
    uint ep_i_num = 1;      /* input endpoint number */
    uint ep_o_num = 1;      /* output endpoint number */

#ifdef BCMUSBDEV_COMPOSITE
    uint ep_i_num_alt = 1;  /* input endpoint number */
    uint ep_o_num_alt = 1;  /* output endpoint number */
    int usbdesc = 0;
    int int_wlan;
    int intf_iso;
    int intf_bt;
    uint k, l, iso_mps_idx = 0;
    uint ep_sum = 0;        /* total number of endpoints */

    /* init interface: support multiple interfaces, IAD with OTP override */
    dev->interface_num = 1;
    intf_iso = INTERFACE_ISO;
    intf_bt = INTERFACE_BT;
    if ((value = getvar(NULL, "usbdesc_composite")) != NULL)
        usbdesc = bcm_strtoul(value, NULL, 0);

    int_wlan = 0;
    if (usbdesc) {
        i = (usbdesc & USB_IF_NUM_MASK);    /* BT interface/s */
        i ++;               /* add WLAN interface */
        if (i > 1) {
            if (i <= (INTERFACE_MAX - ALT_SETTING_MAX)) {
                dev->interface_num = i;
            } else {
                dev->interface_num = INTERFACE_MAX - ALT_SETTING_MAX;
                dbg_printf("invalid OTP interface setting %d, limiting to %d(max)",
                    i-1, dev->interface_num);
            }
        }
        if (usbdesc & USB_WL_INTF_MASK) {
            /* to handle WLAN = 0 BT_INTF = 1..... */
            int_wlan = 0;
            intf_iso = INTERFACE_ISO + 1;
            intf_bt = INTERFACE_BT + 1;
            if (dev->interface_num > 1) {
                /* WL at interface 0, BT 0, 1, 2.. */
                rdl_interfaces[1].bInterfaceNumber = intf_bt;
                rdl_interfaces[1].bAlternateSetting = 0;
                rdl_interfaces[1].bInterfaceClass = UICLASS_WIRELESS;
                rdl_interfaces[1].bInterfaceSubClass = UISUBCLASS_RF;
                rdl_interfaces[1].bInterfaceProtocol = UIPROTO_BLUETOOTH;
                rdl_interfaces[1].iInterface = 6;

                for (i = 2; i < (INTERFACE_MAX-1); i++) {
                    rdl_interfaces[i].bInterfaceNumber = intf_iso;
                    rdl_interfaces[i].bAlternateSetting = i-2;
                    rdl_interfaces[i].bInterfaceClass = UICLASS_WIRELESS;
                    rdl_interfaces[i].bInterfaceSubClass = UISUBCLASS_RF;
                    rdl_interfaces[i].bInterfaceProtocol = UIPROTO_BLUETOOTH;
                    rdl_interfaces[i].iInterface = 6;
                }
                rdl_interfaces[8].bInterfaceNumber = INTERFACE_DFU + 1;
                rdl_interfaces[8].bAlternateSetting = 0;
                rdl_interfaces[8].bInterfaceClass = UICLASS_WIRELESS;
                rdl_interfaces[8].bInterfaceSubClass = UISUBCLASS_RF;
                rdl_interfaces[8].bInterfaceProtocol = UIPROTO_BLUETOOTH;
                rdl_interfaces[8].iInterface = 7;
            }
        }
        else {
            int_wlan = dev->interface_num - 1;
        }
    }
    dev->int_wlan = int_wlan;

    if (int_wlan > INTERFACE_ISO)
        int_wlan += ALT_SETTING_MAX;

    if (int_wlan < (INTERFACE_MAX - 1)) {
        if (int_wlan > INTERFACE_ISO) {
            i = int_wlan;
        } else {
            i = dev->int_wlan;
        }
        rdl_interfaces[i].bInterfaceNumber = dev->int_wlan;
        rdl_interfaces[i].bInterfaceClass = UICLASS_VENDOR;
        rdl_interfaces[i].bInterfaceSubClass = UISUBCLASS_ABSTRACT_CONTROL_MODEL;
        rdl_interfaces[i].bInterfaceProtocol = UIPROTO_DATA_VENDOR;
        rdl_interfaces[i].iInterface = 2;
    }
    rdl_config.bNumInterface = dev->interface_num;
    rdl_other_config.bNumInterface = dev->interface_num;

    /* update settings if this is a composite device */
    if (dev->interface_num > 1) {
        /* composite without IAD */
        rdl_device.bDeviceClass = UDCLASS_WIRELESS;
        rdl_device.bDeviceSubClass = UDSUBCLASS_RF;
        rdl_device.bDeviceProtocol = UDPROTO_BLUETOOTH;
        rdl_hs_qualifier.bDeviceClass = UDCLASS_WIRELESS;
        rdl_hs_qualifier.bDeviceSubClass = UDSUBCLASS_RF;
        rdl_hs_qualifier.bDeviceProtocol = UDPROTO_BLUETOOTH;
        rdl_fs_qualifier.bDeviceClass = UDCLASS_WIRELESS;
        rdl_fs_qualifier.bDeviceSubClass = UDSUBCLASS_RF;
        rdl_fs_qualifier.bDeviceProtocol = UDPROTO_BLUETOOTH;
        rdl_device.iProduct = 4;
    }

    if (dev->interface_num > 2) {
        /* composite with IAD */
        if (usbdesc & USB_IAD_MASK) {
            dbg_printf("IAD enabled");
            rdl_device.bDeviceClass = UDCLASS_MISC;
            rdl_device.bDeviceSubClass = UDSUBCLASS_COMMON;
            rdl_device.bDeviceProtocol = UDPROTO_IAD;
            rdl_hs_qualifier.bDeviceClass = UDCLASS_MISC;
            rdl_hs_qualifier.bDeviceSubClass = UDSUBCLASS_COMMON;
            rdl_hs_qualifier.bDeviceProtocol = UDPROTO_IAD;
            rdl_fs_qualifier.bDeviceClass = UDCLASS_MISC;
            rdl_fs_qualifier.bDeviceSubClass = UDSUBCLASS_COMMON;
            rdl_fs_qualifier.bDeviceProtocol = UDPROTO_IAD;
            rdl_device.iProduct = 4;
            rdl_iad.bInterfaceCount = rdl_config.bNumInterface - 1;
            if (dev->int_wlan == 0) {
                /* WLAN at interface 0, push rad_iad.bFirstInterface */
                rdl_iad.bFirstInterface = IAD_INT_NUM + 1;
            }
        }
        dev->interface_num += ALT_SETTING_MAX;
    }

    /* Device Class Override */
    if ((val = (usbdesc & USB_DEVCLASS_MASK) >> USB_DEVCLASS_SHIFT)) {
        if (val == USB_DEVCLASS_BT) {
            dbg_printf("devclass BT override");
            rdl_device.bDeviceClass = UDCLASS_WIRELESS;
            rdl_device.bDeviceSubClass = UDSUBCLASS_RF;
            rdl_device.bDeviceProtocol = UDPROTO_BLUETOOTH;
            rdl_hs_qualifier.bDeviceClass = UDCLASS_WIRELESS;
            rdl_hs_qualifier.bDeviceSubClass = UDSUBCLASS_RF;
            rdl_hs_qualifier.bDeviceProtocol = UDPROTO_BLUETOOTH;
            rdl_fs_qualifier.bDeviceClass = UDCLASS_WIRELESS;
            rdl_fs_qualifier.bDeviceSubClass = UDSUBCLASS_RF;
            rdl_fs_qualifier.bDeviceProtocol = UDPROTO_BLUETOOTH;
            rdl_device.iProduct = 4;
        } else if (val == USB_DEVCLASS_WLAN) {
            dbg_printf("devclass WLAN override");
            rdl_device.bDeviceClass = UDCLASS_VENDOR;
            rdl_device.bDeviceSubClass = 0;
            rdl_device.bDeviceProtocol = 0;
            rdl_hs_qualifier.bDeviceClass = UDCLASS_VENDOR;
            rdl_hs_qualifier.bDeviceSubClass = 0;
            rdl_hs_qualifier.bDeviceProtocol = 0;
            rdl_fs_qualifier.bDeviceClass = UDCLASS_VENDOR;
            rdl_fs_qualifier.bDeviceSubClass = 0;
            rdl_fs_qualifier.bDeviceProtocol = 0;
            rdl_device.iProduct = 2;
        }
    }

    /* init endpoints: support multiple EP and OTP override */
    ASSERT(rdl_config.bNumInterface <= (INTERFACE_MAX - ALT_SETTING_MAX));
    usbdev_set_ep_defaults(dev, rdl_config.bNumInterface);
#ifdef BCMUSBDEV_BULKIN_2EP
    if (!usbdesc)
        dev->ep_bi_num[int_wlan] = 2;
#endif


    if (usbdesc) {
        /* BT bulk endpoints # */
        if (BT_BULK_IN_NUM(usbdesc)) {
            dev->ep_bi_num[intf_bt] ++;
        }
        if (BT_BULK_OUT_NUM(usbdesc)) {
            dev->ep_bo_num[intf_bt] ++;
        }
        val = (usbdesc & USB_WLANIF_INTR_EP_MASK) >> USB_WLANIF_INTR_EP_SHIFT;
        if (val) {
            dev->ep_ii_num[int_wlan] = 0;
        } else {
            dev->ep_ii_num[int_wlan] = 1;
        }

        val = ((usbdesc & USB_BI_EP_MASK) >> USB_BI_EP_SHIFT);
        if (val > 0 && val < EP_BULK_MAX)
            dev->ep_bi_num[int_wlan] += val;

        val = ((usbdesc & USB_B0_EP_MASK) >> USB_B0_EP_SHIFT);
        if (val > 0 && val < EP_BULK_MAX)
            dev->ep_bo_num[int_wlan] += val;

        /* Max 4-in and 4-out endpoints
         * Adjust endpoint allocation if OTP setting exceeds the limit
         */
        if (rdl_config.bNumInterface > 2) {
            dev->ep_ii_num[int_wlan] = 0;
            if (dev->ep_bi_num[int_wlan] > 2)
                dev->ep_bi_num[int_wlan] = 2;
            if (dev->ep_bo_num[int_wlan] > 2)
                dev->ep_bo_num[int_wlan] = 2;
        } else if (rdl_config.bNumInterface > 1) {
            if (dev->ep_ii_num[int_wlan]) {
                dev->ep_bi_num[int_wlan] = 1;
            } else if (dev->ep_bi_num[int_wlan] > 2) {
                dev->ep_bi_num[int_wlan] = 2;
            }
            if (dev->ep_bo_num[int_wlan] > 3)
                dev->ep_bo_num[int_wlan] = 3;
        }
    }

    ep_sum = 0;
    for (k = 0; k < dev->interface_num; k++) {
        rdl_interfaces[k].bNumEndpoints = dev->ep_ii_num[k]
            + dev->ep_bi_num[k] + dev->ep_bo_num[k]
            + dev->ep_isi_num[k] + dev->ep_iso_num[k];
        ep_sum += rdl_interfaces[k].bNumEndpoints;
    }

    NEWRDL_CONFIGLEN = USB_CONFIG_DESCRIPTOR_SIZE +
        (USB_INTERFACE_DESCRIPTOR_SIZE * dev->interface_num) +
        (USB_ENDPOINT_DESCRIPTOR_SIZE * ep_sum);

#ifdef USB_XDCI
    if (usbd_chk_version(dev) > UD_USB_2_1_DEV) {
        NEWRDL_CONFIGLEN += USB_ENDPOINT_COMPANION_DESCRIPTOR_SIZE * ep_sum;
    }
#endif /* USB_XDCI */
    if (usbdesc & USB_IAD_MASK) {
        if (rdl_iad.bInterfaceCount > 1) {
            NEWRDL_CONFIGLEN += USB_INTERFACE_ASSOCIATION_DESCRIPTOR_SIZE;
        } else {
            dbg_printf("Invalid OTP IAD setting, interface count is %d(<2)",
                rdl_iad.bInterfaceCount);
        }
    }
    if (dev->interface_num == INTERFACE_MAX)
        NEWRDL_CONFIGLEN += USB_DFU_FUNCTIONAL_DESCRIPTOR_SIZE;

    dev->data_interface = rdl_interfaces;
    dev->iad = &rdl_iad;
    dev->interface = dev->int_wlan;

    dbg_printf("Endpoint: intr %d, bulkin %d, bulkout %d",
        dev->ep_ii_num[int_wlan], dev->ep_bi_num[int_wlan],
        dev->ep_bo_num[int_wlan]);

    trace("ei %d, ebi %d, ebo %d", dev->ep_ii_num[int_wlan],
        dev->ep_bi_num[int_wlan], dev->ep_bo_num[int_wlan]);

    /* init config */
    rdl_config.wTotalLength = NEWRDL_CONFIGLEN;
    rdl_other_config.wTotalLength = NEWRDL_CONFIGLEN;

    if ((CHIPID(dev->sih->chip) == BCM43242_CHIP_ID) ||
        (CHIPID(dev->sih->chip) == BCM43243_CHIP_ID) ||
        BCM4350_CHIP(dev->sih->chip) ||
        0) {
        rdl_config.bMaxPower = DEV_MAXPOWER_500;
        rdl_other_config.bMaxPower = DEV_MAXPOWER_500;
    }

    bcopy(&rdl_config, &dev->config, sizeof(dev->config));
    bcopy(&rdl_other_config, &dev->other_config, sizeof(dev->other_config));

    /* init device */
    bcopy(&rdl_device, &dev->device, sizeof(dev->device));

#ifdef USB_XDCI
        if (usbd_chk_version(dev) == UD_USB_2_1_DEV) {
            dev->device.bcdUSB = UD_USB_2_1;
            dev->device.bMaxPacketSize = USB_2_MAX_CTRL_PACKET;
        }
        else if (usbd_chk_version(dev) == UD_USB_3_DEV) {
            dev->device.bcdUSB = UD_USB_3_0;
            dev->device.bMaxPacketSize = USB_3_MAX_CTRL_PACKET_FACTORIAL;
            dev->config.bMaxPower = (uByte)DEV_MAXPOWER_900;
        }
#endif /* USB_XDCI */

    /* build EP list for hs and fs:in EPs first, then out EPs
     * fixup the ep_num. The DMA channel and ep number will be allocated inside ep_attach()
     */
    j = 0;
    l = 0;

    /* handle WLAN = 0, BT = 1 ...... */

    if (usbdesc)
        iso_mps_idx = (usbdesc & USB_ISO_MPS_MASK) >> USB_ISO_MPS_SHIFT;
    for (k = 0; k < dev->interface_num; k++) {
        if (rdl_interfaces[k].bAlternateSetting) {
            ep_i_num = ep_i_num_alt;
            ep_o_num = ep_o_num_alt;
        } else {
            ep_i_num_alt = ep_i_num;
            ep_o_num_alt = ep_o_num;
        }

        for (i = 0; i < dev->ep_ii_num[k]; i++) {
#ifdef USB_XDCI
            dev->intr_endpoints_ss[i+l] = rdl_ss_ep_intr;
            dev->intr_endpoints_cp_ss[i+l] = rdl_ss_cp_intr;
#endif /* USB_XDCI */
            dev->intr_endpoints[i+l] = rdl_hs_ep_intr;
            dev->intr_endpoints_fs[i+l] = rdl_fs_ep_intr;

#ifdef USB_XDCI
            dev->intr_endpoints_ss[i+l].bEndpointAddress =
            UE_SET_ADDR((dev->intr_endpoints_ss[i+l].bEndpointAddress), ep_i_num);
#endif /* USB_XDCI */
            dev->intr_endpoints[i+l].bEndpointAddress =
            UE_SET_ADDR((dev->intr_endpoints[i+l].bEndpointAddress), ep_i_num);
            dev->intr_endpoints_fs[i+l].bEndpointAddress =
            UE_SET_ADDR((dev->intr_endpoints_fs[i+l].bEndpointAddress), ep_i_num);
            /* make the in and out start from the same after interrupt endpoints */
            ep_i_num ++;
            ep_o_num = ep_i_num;
        }
        l += i;

        for (i = 0; i < dev->ep_bi_num[k]; i++) {
#ifdef USB_XDCI
            dev->data_endpoints_ss[i+j] = rdl_ss_ep_bulkin;
            dev->data_endpoints_cp_ss[i+j] = rdl_ss_cp_bulkin;
#endif /* USB_XDCI */
            dev->data_endpoints[i+j] = rdl_hs_ep_bulkin;
            dev->data_endpoints_fs[i+j] = rdl_fs_ep_bulkin;
#ifdef USB_XDCI
            dev->data_endpoints_ss[i+j].bEndpointAddress =
            UE_SET_ADDR((dev->data_endpoints_ss[i+j].bEndpointAddress), ep_i_num);
#endif /* USB_XDCI */
            dev->data_endpoints[i+j].bEndpointAddress =
            UE_SET_ADDR((dev->data_endpoints[i+j].bEndpointAddress), ep_i_num);
            dev->data_endpoints_fs[i+j].bEndpointAddress =
            UE_SET_ADDR((dev->data_endpoints_fs[i+j].bEndpointAddress), ep_i_num);
            ep_i_num++;
        }
        j += i;

        for (i = 0; i < dev->ep_bo_num[k]; i++) {
#ifdef USB_XDCI
            dev->data_endpoints_ss[i + j] = rdl_ss_ep_bulkout;
            dev->data_endpoints_cp_ss[i + j] = rdl_ss_cp_bulkout;
#endif /* USB_XDCI */
            dev->data_endpoints[i + j] = rdl_hs_ep_bulkout;
            dev->data_endpoints_fs[i + j] = rdl_fs_ep_bulkout;
#ifdef USB_XDCI
            dev->data_endpoints_ss[i + j].bEndpointAddress =
            UE_SET_ADDR((dev->data_endpoints_ss[i + j].bEndpointAddress), ep_o_num);
#endif /* USB_XDCI */
            dev->data_endpoints[i + j].bEndpointAddress =
            UE_SET_ADDR((dev->data_endpoints[i + j].bEndpointAddress), ep_o_num);
            dev->data_endpoints_fs[i + j].bEndpointAddress =
            UE_SET_ADDR((dev->data_endpoints_fs[i + j].bEndpointAddress), ep_o_num);
            ep_o_num ++;
        }
        j += i;

        for (i = 0; i < dev->ep_isi_num[k]; i++) {
#ifdef USB_XDCI
            dev->data_endpoints_ss[i + j] = rdl_ss_ep_isin;
            dev->data_endpoints_cp_ss[i + j] = rdl_ss_cp_isin;
            if (iso_mps_idx)
                usbdev_set_iso_alt_ep_maxsize(&dev->data_endpoints_ss[i + j],
                    iso_mps_idx);
            else
                usbdev_set_iso_alt_ep_maxsize(&dev->data_endpoints_ss[i + j], k);
#endif /* USB_XDCI */
            dev->data_endpoints[i + j] = rdl_hs_ep_isin;
            if (iso_mps_idx)
                usbdev_set_iso_alt_ep_maxsize(&dev->data_endpoints[i + j],
                    iso_mps_idx);
            else
                usbdev_set_iso_alt_ep_maxsize(&dev->data_endpoints[i + j], k);
            dev->data_endpoints_fs[i + j] = rdl_fs_ep_isin;
            if (iso_mps_idx)
                usbdev_set_iso_alt_ep_maxsize(&dev->data_endpoints_fs[i + j],
                    iso_mps_idx);
            else
                usbdev_set_iso_alt_ep_maxsize(&dev->data_endpoints_fs[i + j], k);
#ifdef USB_XDCI
            dev->data_endpoints_ss[i + j].bEndpointAddress =
            UE_SET_ADDR((dev->data_endpoints_ss[i + j].bEndpointAddress), ep_i_num);
#endif /* USB_XDCI */
            dev->data_endpoints[i + j].bEndpointAddress =
            UE_SET_ADDR((dev->data_endpoints[i + j].bEndpointAddress), ep_i_num);
            dev->data_endpoints_fs[i + j].bEndpointAddress =
            UE_SET_ADDR((dev->data_endpoints_fs[i + j].bEndpointAddress), ep_i_num);
            ep_i_num ++;
        }
        j += i;

        for (i = 0; i < dev->ep_iso_num[k]; i++) {
#ifdef USB_XDCI
            dev->data_endpoints_ss[i + j] = rdl_ss_ep_isout;
            dev->data_endpoints_cp_ss[i + j] = rdl_ss_cp_isout;
            if (iso_mps_idx)
                usbdev_set_iso_alt_ep_maxsize(&dev->data_endpoints_ss[i + j],
                    iso_mps_idx);
            else
                usbdev_set_iso_alt_ep_maxsize(&dev->data_endpoints_ss[i + j], k);
#endif /* USB_XDCI */
            dev->data_endpoints[i + j] = rdl_hs_ep_isout;
            if (iso_mps_idx)
                usbdev_set_iso_alt_ep_maxsize(&dev->data_endpoints[i + j],
                    iso_mps_idx);
            else
                usbdev_set_iso_alt_ep_maxsize(&dev->data_endpoints[i + j], k);
            dev->data_endpoints_fs[i + j] = rdl_fs_ep_isout;
            if (iso_mps_idx)
                usbdev_set_iso_alt_ep_maxsize(&dev->data_endpoints_fs[i + j],
                    iso_mps_idx);
            else
                usbdev_set_iso_alt_ep_maxsize(&dev->data_endpoints_fs[i + j], k);
#ifdef USB_XDCI
            dev->data_endpoints_ss[i + j].bEndpointAddress =
            UE_SET_ADDR((dev->data_endpoints_ss[i + j].bEndpointAddress), ep_o_num);
#endif /* USB_XDCI */
            dev->data_endpoints[i + j].bEndpointAddress =
            UE_SET_ADDR((dev->data_endpoints[i + j].bEndpointAddress), ep_o_num);
            dev->data_endpoints_fs[i + j].bEndpointAddress =
            UE_SET_ADDR((dev->data_endpoints_fs[i + j].bEndpointAddress), ep_o_num);
            ep_o_num++;
        }
        j += i;
    }
#else
    /* init endpoint: support multiple EP, and OTP override */
    dev->ep_ii_num = 1;
    dev->ep_bi_num = 1;
    dev->ep_bo_num = 1;
#ifdef BCMUSBDEV_BULKIN_2EP
    dev->ep_bi_num = 2;
#endif


    /* only support multiple BULK EPs for now, only one Interrupt EP */
    if ((value = getvar(NULL, "usbepnum")) != NULL) {
        i = bcm_strtoul(value, NULL, 0);
        val = (i & 0x0f);
        if (val > 0 && val <= EP_BULK_MAX) {
            dev->ep_bi_num = val;
        }
        val = (i & 0xf0) >> 4;
        if (val > 0 && val <= EP_BULK_MAX) {
            dev->ep_bo_num = val;
        }
    }

    rdl_interface.bNumEndpoints = dev->ep_ii_num + dev->ep_bi_num + dev->ep_bo_num;

    NEWRDL_CONFIGLEN = USB_CONFIG_DESCRIPTOR_SIZE + USB_INTERFACE_DESCRIPTOR_SIZE +
        (USB_ENDPOINT_DESCRIPTOR_SIZE * rdl_interface.bNumEndpoints);

#ifdef USB_XDCI
    if (usbd_chk_version(dev) > UD_USB_2_1_DEV) {
        NEWRDL_CONFIGLEN += USB_ENDPOINT_COMPANION_DESCRIPTOR_SIZE
        * rdl_interface.bNumEndpoints;
    }
#endif /* USB_XDCI */

    /* init interface */
    dev->data_interface = &rdl_interface;

    dbg_printf("Endpoint: intr %d, bulkin %d, bulkout %d totconflen %d",
        dev->ep_ii_num, dev->ep_bi_num, dev->ep_bo_num, NEWRDL_CONFIGLEN);

    trace("ei %d, ebi %d, ebo %d", dev->ep_ii_num, dev->ep_bi_num, dev->ep_bo_num);

    /* init config */
    rdl_config.wTotalLength = NEWRDL_CONFIGLEN;
    rdl_other_config.wTotalLength = NEWRDL_CONFIGLEN;
    bcopy(&rdl_config, &dev->config, sizeof(dev->config));
    bcopy(&rdl_other_config, &dev->other_config, sizeof(dev->other_config));

    /* init device */
    bcopy(&rdl_device, &dev->device, sizeof(usb_device_descriptor_t));

#ifdef USB_XDCI
    if (usbd_chk_version(dev) == UD_USB_2_1_DEV) {
        dev->device.bcdUSB = UD_USB_2_1;
        dev->device.bMaxPacketSize = USB_2_MAX_CTRL_PACKET;
    }
    else if (usbd_chk_version(dev) == UD_USB_3_DEV) {
        dev->device.bcdUSB = UD_USB_3_0;
        dev->device.bMaxPacketSize = USB_3_MAX_CTRL_PACKET_FACTORIAL;
        dev->config.bMaxPower = (uByte)DEV_MAXPOWER_900;
    }
#endif /* USB_XDCI */
    ep_i_num = 2;
    /* build EP list for hs and fs: bulk in EPs first, then bulk out EPs
     * fixup the ep_num. The DMA channel and ep number will be allocated inside ep_attach()
     */
    for (i = 0; i < dev->ep_bi_num; i++) {
#ifdef USB_XDCI
        dev->data_endpoints_cp_ss[i] = rdl_ss_cp_bulkin;
        dev->data_endpoints_ss[i] = rdl_ss_ep_bulkin;
        dev->data_endpoints_ss[i].bEndpointAddress =
            UE_SET_ADDR((dev->data_endpoints_ss[i].bEndpointAddress), ep_i_num);
#endif /* USB_XDCI */
        dev->data_endpoints[i] = rdl_hs_ep_bulkin;
        dev->data_endpoints_fs[i] = rdl_fs_ep_bulkin;
        dev->data_endpoints[i].bEndpointAddress =
            UE_SET_ADDR((dev->data_endpoints[i].bEndpointAddress), ep_i_num);
        dev->data_endpoints_fs[i].bEndpointAddress =
            UE_SET_ADDR((dev->data_endpoints_fs[i].bEndpointAddress), ep_i_num);
        ep_i_num++;
    }
    j = i;
    ep_o_num = ep_i_num;

    for (i = 0; i < dev->ep_bo_num; i++) {
#ifdef USB_XDCI
        dev->data_endpoints_cp_ss[i + j] = rdl_ss_cp_bulkout;
        dev->data_endpoints_ss[i + j] = rdl_ss_ep_bulkout;
        dev->data_endpoints_ss[i + j].bEndpointAddress =
            UE_SET_ADDR((dev->data_endpoints_ss[i + j].bEndpointAddress), ep_o_num);
#endif /* USB_XDCI */
        dev->data_endpoints[i + j] = rdl_hs_ep_bulkout;
        dev->data_endpoints_fs[i + j] = rdl_fs_ep_bulkout;
        dev->data_endpoints[i + j].bEndpointAddress =
            UE_SET_ADDR((dev->data_endpoints[i + j].bEndpointAddress), ep_o_num);
        dev->data_endpoints_fs[i + j].bEndpointAddress =
            UE_SET_ADDR((dev->data_endpoints_fs[i + j].bEndpointAddress), ep_o_num);
        ep_o_num++;
    }
#endif /* BCMUSBDEV_COMPOSITE */
}

struct dngl_bus *
usbdev_attach(osl_t *osh, void *ch, si_t *sih)
{
    struct dngl_bus *dev;
    const char *value;
    int i;
    char **cp;
    int val;
    /* bootloader nvram override options */
    cusstr_t *s, custom_strings[] = {
        { 1, "manf" },
        { 2, "productname" },
        { 3, "rdlsn" },
        { 0, NULL }
        };
    char *usb_bl_vid_nvoptions[] = {
        "manfid",   /* standard cis tuple */
        "subvendid",    /* Broadcom cis tuple */
        NULL
        };

    char *usb_bl_pid_nvoptions[] = {
        "rdlid",
        NULL
        };

    trace("RDL");

    if (!(dev = MALLOC(osh, sizeof(struct dngl_bus)))) {
        err("out of memory");
        return NULL;
    }
    bzero(dev, sizeof(struct dngl_bus));
    dev->osh = osh;
    dev->ch = ch;
    dev->sih = sih;

#ifdef USB_XDCI
    dev->usb30d = FALSE;
    if (bcmrdl.flags & (1 << RTEDEVFLAG_USB30)) {
        dev->usb30d = TRUE;
    }
#endif /* USB_XDCI */
    if ((value = getvar(NULL, "rdlrndis")) != NULL) {
        dev->rndis = (bool) bcm_strtoul(value, NULL, 0);
    }

    if (dev->rndis) {
        err("RNDIS de-supported");
        goto fail;
    }

    usbdev_attach_interface_ep_init(dev);

    for (i = 0; i < ARRAYSIZE(dev->strings); i++) {
        bcopy(&rdl_strings[i], &dev->strings[i], sizeof(usb_string_descriptor_t));
    }

    /* Convert custom ASCII strings to UTF16 (not NULL terminated) */
    for (s = custom_strings; s->name; s++) {
        if ((value = getvar(NULL, s->name)) != NULL) {
            dbg_printf("found %s string %s", value, s->name);
            for (i = 0; i < strlen(value) && i < (USB_MAX_STRING_LEN); i++)
                dev->strings[s->index].bString[i] = (uWord) value[i];
            dev->strings[s->index].bLength = (i * sizeof(uWord)) + 2;
        }
    }

    /* Support custom manf and product IDs for PnP (both CIS-srom & nvram style) */
    for (cp = usb_bl_vid_nvoptions; *cp != NULL; ++cp) {
        if (((value = getvar(NULL, *cp)) != NULL) && (val = bcm_strtoul(value, NULL, 0))) {
            dbg_printf("found manf string %s, val 0x%x", value, val);
            dev->device.idVendor = val;
            break;
        }
    }

    /* bootloader has a pnp id separate from the downloaded driver */
    for (cp = usb_bl_pid_nvoptions; *cp != NULL; ++cp) {
        if (((value = getvar(NULL, *cp)) != NULL) && (val = bcm_strtoul(value, NULL, 0))) {
            dbg_printf("found rdlid string %s, val 0x%x", value, val);
            dev->device.idProduct = val;
            break;
        }
    }

    /* remote wakeup capability override */
    if ((value = getvar(NULL, "rdlrwu")) != NULL) {
        i = bcm_strtoul(value, NULL, 0);
        dbg_printf("found rdlrwu val %d", i);
        if (i == 0) {   /* honor disable since default is on */
            dev->config.bmAttributes &= ~UC_REMOTE_WAKEUP;
            dev->other_config.bmAttributes &= ~UC_REMOTE_WAKEUP;
        }
    }

#ifdef USB_IFTEST
    if (dev->config.bmAttributes & UC_REMOTE_WAKEUP) {
        hndrte_cons_addcmd("rmwk", (cons_fun_t)rdl_initiate_resume, (uintptr)ch);
    }
#endif /* USB_IFTEST */

    /* HSIC is self-powered so change the descriptors to reflect this */
    if (usbd_hsic(dev)) {
        dev->config.bmAttributes &= ~UC_BUS_POWERED;
        dev->config.bmAttributes |= UC_SELF_POWERED;
        dev->config.bMaxPower = 0;
        dev->other_config.bmAttributes &= ~UC_BUS_POWERED;
        dev->other_config.bmAttributes |= UC_SELF_POWERED;
        dev->other_config.bMaxPower = 0;
    }

    dev->state = -1;
    usbdev_rdl_dl_state_reset(dev);

    trace("done");
    return dev;
fail:
    if (dev)
        MFREE(osh, dev, sizeof(struct dngl_bus));

    return NULL;
}

void
usbdev_detach(struct dngl_bus *dev)
{
    //trace("");

    MFREE(dev->osh, dev, sizeof(struct dngl_bus));
    /* ensure cache state is in sync after code copy */
    FLUSH_DCACHE();
    INV_ICACHE();

    trace("done");
}

static void
usbdev_setup_ep_report(struct dngl_bus *dev, uchar **cur, uint speed)
{
    uint i;
    usb_endpoint_descriptor_t *data_endpoints;
#ifdef USB_XDCI
    usb_endpoint_companion_descriptor_t *data_endpoints_cp = NULL;
#endif /* USB_XDCI */

    trace("cur %p", *cur);

#ifdef BCMUSBDEV_COMPOSITE
    uint j, k, l;
    usb_endpoint_descriptor_t *intr_endpoints;
#ifdef USB_XDCI
    usb_endpoint_companion_descriptor_t *intr_endpoints_cp = NULL;
#endif

    j = 0;
    l = 0;
#ifdef USB_XDCI
    if (speed == UD_USB_SS) {
        intr_endpoints = dev->intr_endpoints_ss;
        data_endpoints = dev->data_endpoints_ss;
        intr_endpoints_cp = dev->intr_endpoints_cp_ss;
        data_endpoints_cp = dev->data_endpoints_cp_ss;

    }
    else
#endif /* USB_XDCI */
    {
        intr_endpoints = (speed) ? dev->intr_endpoints : dev->intr_endpoints_fs;
        data_endpoints = (speed) ? dev->data_endpoints : dev->data_endpoints_fs;
    }

    for (k = 0; k < dev->interface_num; k++) {
        uint32 intf_dfu = INTERFACE_DFU;

        if (dev->int_wlan == 0 && dev->interface_num > 1)
            intf_dfu = INTERFACE_DFU + 1;

        if ((rdl_iad.bInterfaceCount > 1) && (k == rdl_iad.bFirstInterface))
            *cur += usbdev_htol_usb_iad(dev->iad, *cur);
        *cur += usbdev_htol_usb_interface_descriptor(&dev->data_interface[k], *cur);

        if (rdl_interfaces[k].bInterfaceNumber == intf_dfu &&
            rdl_interfaces[k].bInterfaceNumber != dev->int_wlan) {
            *cur += usbdev_htol_usb_dfu_functional_descriptor(&rdl_dfu, *cur);
        }

        for (i = 0; i < dev->ep_ii_num[k]; i++) {
            *cur += usbdev_htol_usb_endpoint_descriptor(&intr_endpoints[i+l], *cur);
#ifdef USB_XDCI
            if (speed == UD_USB_SS)
                *cur += usbdev_htol_usb_ed_companion_descriptor(
                    &intr_endpoints_cp[i+l], *cur);
#endif /* USB_XDCI */
        }
        l += i;

        for (i = 0; i < dev->ep_bi_num[k]; i++) {
            *cur += usbdev_htol_usb_endpoint_descriptor(&data_endpoints[i+j], *cur);
#ifdef USB_XDCI
            if (speed == UD_USB_SS)
                *cur += usbdev_htol_usb_ed_companion_descriptor(
                    &data_endpoints_cp[i+j], *cur);
#endif /* USB_XDCI */
        }
        j += i;

        for (i = 0; i < dev->ep_bo_num[k]; i++) {
            *cur += usbdev_htol_usb_endpoint_descriptor(&data_endpoints[i+j], *cur);
#ifdef USB_XDCI
            if (speed == UD_USB_SS)
                *cur += usbdev_htol_usb_ed_companion_descriptor(
                    &data_endpoints_cp[i+j], *cur);
#endif /* USB_XDCI */
        }
        j += i;

        for (i = 0; i < dev->ep_isi_num[k]; i++) {
            *cur += usbdev_htol_usb_endpoint_descriptor(&data_endpoints[i+j], *cur);
#ifdef USB_XDCI
            if (speed == UD_USB_SS)
                *cur += usbdev_htol_usb_ed_companion_descriptor(
                    &data_endpoints_cp[i+j], *cur);
#endif /* USB_XDCI */

        }
        j += i;

        for (i = 0; i < dev->ep_iso_num[k]; i++) {
            *cur += usbdev_htol_usb_endpoint_descriptor(&data_endpoints[i+j], *cur);
#ifdef USB_XDCI
            if (speed == UD_USB_SS)
                *cur += usbdev_htol_usb_ed_companion_descriptor(
                    &data_endpoints_cp[i+j], *cur);
#endif
        }
        j += i;
    }
#else
    *cur += usbdev_htol_usb_interface_descriptor(dev->data_interface, *cur);

    for (i = 0; i < dev->ep_ii_num; i++) {
#ifdef USB_XDCI
        if (speed == UD_USB_SS) {
            data_endpoints = &rdl_ss_ep_intr;
            data_endpoints_cp = &rdl_ss_cp_intr;
        }
        else
#endif /* USB_XDCI */
        data_endpoints = (speed) ? &rdl_hs_ep_intr : &rdl_fs_ep_intr;
        *cur += usbdev_htol_usb_endpoint_descriptor(data_endpoints, *cur);
#ifdef USB_XDCI
        if (speed == UD_USB_SS) {
            *cur += usbdev_htol_usb_ed_companion_descriptor(data_endpoints_cp, *cur);
        }
#endif /* USB_XDCI */
    }

    for (i = 0; i < dev->ep_bi_num + dev->ep_bo_num; i++) {
#ifdef USB_XDCI
        if (speed == UD_USB_SS) {
            data_endpoints_cp = &dev->data_endpoints_cp_ss[i];
            data_endpoints = &dev->data_endpoints_ss[i];
        }
        else
#endif /* USB_XDCI */
        data_endpoints = (speed) ? &dev->data_endpoints[i] : &dev->data_endpoints_fs[i];
        *cur += usbdev_htol_usb_endpoint_descriptor(data_endpoints, *cur);
#ifdef USB_XDCI
        if (speed == UD_USB_SS)
            *cur += usbdev_htol_usb_ed_companion_descriptor(data_endpoints_cp, *cur);
#endif /* USB_XDCI */

    }
#endif /* BCMUSBDEV_COMPOSITE */

    trace("cur %p after adding EPs", *cur);
}

void *
usbdev_setup(struct dngl_bus *dev, int ep, void *p, int *errp, int *dir)
{
    usb_device_request_t dr;
    rdl_state_t *rdl_state;
    bootrom_id_t *id;
    void *p0 = NULL;
    int i;
    uint speed;

    trace("RDL");
    ASSERT(ep == 0);

    ltoh_usb_device_request(PKTDATA(dev->osh, p), &dr);

    *dir = dr.bmRequestType & UT_READ;
    /* Get standard descriptor */
    /* open it for all UT_READ_DEVICE, UT_READ_INTERFACE, UT_READ_ENDPOINT */
    if ((dr.bmRequestType&0xF0) == UT_READ_DEVICE) {
        uchar *cur = NULL;
        int bufsize = 256;
        if (dr.bRequest == UR_GET_DESCRIPTOR) {
            uchar request = (dr.wValue >> 8) & 0xff;
#ifdef BCMUSBDEV_COMPOSITE
            if (rdl_config.wTotalLength > bufsize)
                bufsize *= 2;
#endif /* BCMUSBDEV_COMPOSITE */
            if (!(p0 = PKTGET(dev->osh, bufsize, TRUE))) {
                err("ep%d: out of txbufs", ep);
                goto stall;
            }
            cur = PKTDATA(dev->osh, p0);

            switch (request) {
            case UDESC_DEVICE:
                {
                    const char *otp_val;
                    dbg_printf("ep%d: UDESC_DEVICE", ep);
                    /* PR:89633 - Dis-arm watchdog timer */
                    if ((otp_val = getvar(NULL, "bldr_to")) != NULL) {
                        si_watchdog_ms(dev->sih, 0);
                    }
                    cur += htol_usb_device_descriptor(&dev->device, cur);
                }
                break;
            case UDESC_CONFIG:
            case UDESC_OTHER_SPEED_CONFIGURATION:
                dbg_printf("ep%d: %s", ep, request == UDESC_CONFIG ? "UDESC_CONFIG" :
                    "UDESC_OTHER_SPEED_CONFIGURATION");
                if (request == UDESC_CONFIG) {
                    cur += usbdev_htol_usb_config_descriptor(
                        &dev->config, cur);
                    speed = dev->speed;
                } else {
                    cur += usbdev_htol_usb_config_descriptor(
                        &dev->other_config, cur);
                    speed = !dev->speed;
                }
                if (request == UDESC_CONFIG ||
                    usbd_chk_version(dev) >= UD_USB_2_0_DEV) {
                    if (dr.wLength > 9)
                        usbdev_setup_ep_report(dev, &cur, speed);
                    break;
                } else {
                    goto stall;
                }
            case UDESC_INTERFACE:
                i = dr.wValue & 0xff;
                dbg_printf("ep%d: UDESC_INTERFACE %d", ep, i);
#ifdef BCMUSBDEV_COMPOSITE
                if ((i >= 0) && (i < rdl_config.bNumInterface)) {
                    uint32 intf_iso = INTERFACE_ISO;
                    if (dev->int_wlan == 0) {
                        intf_iso += 1;
                    }
                    if (i > intf_iso)
                        cur += usbdev_htol_usb_interface_descriptor(
                        &dev->data_interface[i + ALT_SETTING_MAX], cur);
                    else
                        cur += usbdev_htol_usb_interface_descriptor(
                            &dev->data_interface[i], cur);
                }

#else
                if (i == dev->data_interface->bInterfaceNumber)
                    cur += usbdev_htol_usb_interface_descriptor(
                        dev->data_interface, cur);
#endif /* BCMUSBDEV_COMPOSITE */
                else {
                    err("UDESC_INTERFACE query failed");
                    goto stall;
                }
                break;
            case UDESC_STRING:
                i = dr.wValue & 0xff;
                dbg_printf("ep%d: UDESC_STRING %d langid 0x%x", ep, i, dr.wIndex);
#ifdef BCMUSBDEV_COMPOSITE
                if (i > 7) {
#else
                if (i > 3) {
#endif /* BCMUSBDEV_COMPOSITE */
                    err("UDESC_STRING: index %d out of range", i);
                    goto stall;
                }
                cur += htol_usb_string_descriptor(&dev->strings[i], cur);
                break;
            case UDESC_DEVICE_QUALIFIER:
                dbg_printf("ep%d: UDESC_DEVICE_QUALIFIER", ep);
                /* return qualifier info for "other" (i.e., not current) speed */
                if (dev->speed) {
                    cur += usbdev_htol_usb_device_qualifier(
                            &rdl_hs_qualifier, cur);
                } else {
                    cur += usbdev_htol_usb_device_qualifier(
                            &rdl_fs_qualifier, cur);
                }
                break;
#ifdef BCMUSBDEV_COMPOSITE
            case UDESC_DFU:
                dbg_printf("ep%d: UDESC_DFU", ep);
                cur += usbdev_htol_usb_dfu_functional_descriptor(&rdl_dfu, cur);
                break;
            case UDESC_DEBUG:
                dbg_printf("ep%d: UDESC_DEBUG, not supported", ep);
                goto stall;
                break;
#endif /* BCMUSBDEV_COMPOSITE */
#ifdef USB_XDCI
            case UDESC_BOS:
                cur += usbdev_htol_usb_bos_descriptor(&rdl_bos_desc, cur);
                cur += usbdev_htol_usb_2_extension_descritor(&rdl_usb2_ext, cur);
                cur += usbdev_htol_usb_ss_device_capacity_descritor(&rdl_ss_cap,
                    cur);
                break;
#endif /* USB_XDCI */
            default:
                err("ep%d: unhandled desc type %d", ep, (dr.wValue >> 8) & 0xff);
                goto stall;
            }
        } else if (dr.bRequest == UR_GET_CONFIG) {
            if (!(p0 = PKTGET(dev->osh, 4, TRUE))) {
                err("ep%d: out of txbufs", ep);
                goto stall;
            }
            cur = PKTDATA(dev->osh, p0);
            *cur++ = (char) dev->confignum;
            dbg_printf("ep%d: get config returning config %d", ep, dev->confignum);
        } else if (dr.bRequest == UR_GET_INTERFACE) {
            if (!(p0 = PKTGET(dev->osh, 4, TRUE))) {
                err("ep%d: out of txbufs", ep);
                goto stall;
            }
            cur = PKTDATA(dev->osh, p0);

            *cur++ = dev->interface;
            dbg_printf("ep%d: get interface returning interface %d", ep, dev->interface);
        } else {
            err("ep%d: unhandled read device bRequest 0x%x, wValue 0x%x", ep,
                   dr.bRequest, dr.wValue);
            goto stall;
        }

        PKTSETLEN(dev->osh, p0, cur - PKTDATA(dev->osh, p0));
    }

    else if (dr.bmRequestType == UT_WRITE_INTERFACE &&
             dr.bRequest == UR_SET_INTERFACE) {
        /* dummy handler to satisfy WHQL interface query */
        dev->interface = dr.wValue;
        err("ep%d: set interface set interface %d", ep, dev->interface);
    }

    /* Send encapsulated command */
    else if (dr.bmRequestType == UT_WRITE_VENDOR_INTERFACE) {
        dbg_printf("ep%d: ctrl out %d", ep, dr.bRequest);

        if (dr.bRequest == DL_GO) {
            dbg_printf("DFU GO 0x%x", (uint32)dev->jumpto);
            rdl_indicate_start(dev->jumpto);
        } else if (dr.bRequest == DL_GO_PROTECTED) {
            dbg_printf("DFU GO PROTECTED @ 0x%x", (uint32)dev->jumpto);
            /* watchdog reset after 2 seconds */
            si_watchdog_ms(dev->sih, 2000);
            rdl_indicate_start(dev->jumpto);
        } else if (dr.bRequest == DL_REBOOT) {
            dbg_printf("DL_REBOOT");
            /* watchdog reset after 2 seconds */
            si_watchdog_ms(dev->sih, 2000);
        } else if (dr.bRequest == DL_EXEC) {
            uint32 *addr;
            exec_fn_t exec_fn;

            dbg_printf("DL_EXEC");
            addr = (uint32 *) ((dr.wIndex << 16) | dr.wValue);
            exec_fn = (exec_fn_t)OSL_UNCACHED(addr);
            exec_fn((void *)(dev->sih));
        }
#ifdef USB_XDCI
        else if (dr.bRequest == DL_CHGSPD) {
                if (dev->usb30d) {
                    xdci_indicate_speed(dev->ch, dr.wIndex);
                }
        }
#endif /* USB_XDCI */
    }
    /* Get encapsulated response */
    else if (dr.bmRequestType == UT_READ_VENDOR_INTERFACE) {
        dbg_printf("ep%d: ctrl in %d", ep, dr.bRequest);

        if (!(p0 = PKTGET(dev->osh, 256, TRUE))) {
            err("ep%d: out of txbufs", ep);
            goto stall;
        }
        bzero(PKTDATA(dev->osh, p0), 256);

        if (dr.bRequest == DL_CHECK_CRC) {
            /* check the crc of the DL image */
            dbg_printf("DFU CHECK CRC");
        } else if (dr.bRequest == DL_GO) {
            dbg_printf("DFU GO 0x%x", (uint32)dev->jumpto);
#ifdef BCM_SDRBL
            /* In SDR case if image isgnature is not valid,
             * then dont allow to run the image.
             */
            if (dev->state == DL_RUNNABLE)
#endif /* BCM_SDRBL */
#ifdef USB_XDCI
                if (dev->usb30d) {
                    xdci_indicate_start(dev->ch, dev->jumpto, dr.wIndex);
                }
                else
#endif
                rdl_indicate_start(dev->jumpto);

        } else if (dr.bRequest == DL_GO_PROTECTED) {
            dbg_printf("DFU GO PROTECTED 0x%x", (uint32)dev->jumpto);
            /* watchdog reset after 2 seconds */
            si_watchdog_ms(dev->sih, 2000);
#ifdef BCM_SDRBL
            /* In SDR case if image isgnature is not valid,
             * then dont allow to run the image.
             */
            if (dev->state == DL_RUNNABLE)
#endif /* BCM_SDRBL */
            rdl_indicate_start(dev->jumpto);
        } else if (dr.bRequest == DL_START) {
            dbg_printf("DL_START");
            /* Go to start state */
            usbdev_rdl_dl_state_reset(dev);
        } else if (dr.bRequest == DL_REBOOT) {
            dbg_printf("DL_REBOOT");
            /* watchdog reset after 2 seconds */
            si_watchdog_ms(dev->sih, 2000);
        }

        if (dr.bRequest == DL_GETVER) {
            dbg_printf("DL_GETVER");
            id = (bootrom_id_t*)PKTDATA(dev->osh, p0);
            id->chip = dev->sih->chip;
            id->chiprev = dev->sih->chiprev;
            id->ramsize = _memsize;
            id->remapbase = DL_BASE;
            id->boardtype = getintvar(NULL, "boardtype");
            id->boardrev = getintvar(NULL, "boardrev");
            PKTSETLEN(dev->osh, p0, sizeof(bootrom_id_t));
        } else if (dr.bRequest == DL_GETSTATE) {
            dbg_printf("DL_GETSTATE");
            /* return only the state struct */
            rdl_state = (rdl_state_t*)PKTDATA(dev->osh, p0);
            rdl_state->state = dev->state;
            rdl_state->bytes = dev->dlcurrentlen;
            PKTSETLEN(dev->osh, p0, sizeof(rdl_state_t));
        } else if ((dr.bRequest & DL_HWCMD_MASK) == DL_RDHW) {
            hwacc_t *hwacc;
            uint32 *addr;

            dbg_printf("DL_RDHW");

            hwacc = (hwacc_t *)PKTDATA(dev->osh, p0);
            hwacc->addr = (dr.wIndex << 16) | dr.wValue;
            addr = (uint32 *)OSL_UNCACHED(hwacc->addr);
            if (dr.bRequest == DL_RDHW32) {
                hwacc->data = *addr;
                hwacc->len = 4;
            } else if (dr.bRequest == DL_RDHW16) {
                hwacc->data = *(uint16 *)addr;
                hwacc->len = 2;
            } else if (dr.bRequest == DL_RDHW8) {
                hwacc->data = *(uint8 *)addr;
                hwacc->len = 1;
            }
            hwacc->cmd = DL_RDHW;
            PKTSETLEN(dev->osh, p0, sizeof(hwacc_t));
            dbg_printf("DL_RDHW: addr 0x%x => 0x%x", hwacc->addr, hwacc->data);
        } else if (dr.bRequest == DL_GET_NVRAM) {
            uint16 len = 1;
            if (*(dev->var)) {
                const char *var = getvar(NULL, dev->var);
                if (var) {
                    len = strlen(var) + 1;
                    if (PKTLEN(dev->osh, p0) < len) {
                        PKTFREE(dev->osh, p0, TRUE);
                        if (!(p0 = PKTGET(dev->osh, len, TRUE))) {
                            err("ep%d: out of txbufs", ep);
                            goto stall;
                        }
                        bzero(PKTDATA(dev->osh, p0), len);
                    }
                    strncpy((char*)PKTDATA(dev->osh, p0), var, len);
                }
            }
            PKTSETLEN(dev->osh, p0, len);
        }
    }
    else {
        goto stall;
    }

    /* Free request packet and trim return packet */
    if (p0) {
        dr.wLength = MIN(dr.wLength, PKTLEN(dev->osh, p0));
        PKTSETLEN(dev->osh, p0, dr.wLength);
        return p0;
    }

    /* No return packet */
    return p;

stall:
    dbg_printf("Stall: type %x req %x val %x idx %x len %x\n", dr.bmRequestType,
           dr.bRequest, dr.wValue, dr.wIndex, dr.wLength);

    if (p0)
        PKTFREE(dev->osh, p0, TRUE);
    err("ep%d: stall\n", ep);

    *errp = 1;
    return NULL;
}

#if defined(DL_NVRAM) && !defined(BCMTRXV2)
/**
 * points at location where downloaded nvram should be copied to. This location is determined
 * dynamically in the bootloader start up assembly code, but is fixed for a specific bootloader.
 */
extern uchar *_dlvars;
extern uint *_dlvarsz;
extern uint _memsize;

static void
usbdev_nvramdl(struct dngl_bus *dev, unsigned char *pdata, uint32 len, unsigned char *dlnvramp)
{
    int nvram_len;

    nvram_len = MIN(dev->nvramsz, (pdata + len) - dlnvramp);

    dbg_printf("len %d, nvram_len %d", len, nvram_len);

    if (nvram_len) {
        bcopy(dlnvramp, _dlvars + dev->nvramoff, nvram_len);
        dev->nvramsz -= nvram_len;
    }
    dev->nvramoff += nvram_len;
    if (dev->nvramsz == 0) {
        int memorygap = (int)((_memsize + (dev->dlbase & MEM_REMAP_MASK)) - 4) -
                              ((int)_dlvars + dev->nvramoff);
        dbg_printf("memorygap %d; _memsize 0x%x; nvramoff %d; _dlvars 0x%x",
               memorygap, _memsize, dev->nvramoff, (uint32)_dlvars);
        /* shift the vars up in memory to eliminate memory waste */
        if (memorygap > 0)
            memmove(_dlvars + memorygap, _dlvars, dev->nvramoff);
        dbg_printf("DL_RUNNABLE");
        dev->state = DL_RUNNABLE;
    }
}
#endif /* DL_NVRAM && !BCMTRXV2 */

#ifdef ZLIB
/* decompress data */
static int
copy_decompress(struct dngl_bus *dev, unsigned char *pdata, uint32 len)
{
    int trl_frag, i;
    uint32 uncmp_len = 0, dec_crc;

    dev->d_stream.avail_in = len;
    dev->d_stream.next_in = pdata;
    if (dev->zstatus != Z_STREAM_END)
        dev->zstatus = inflate(&(dev->d_stream), Z_SYNC_FLUSH);
    if (dev->zstatus == Z_STREAM_END) {
        dbg_printf("dev->zstatus == Z_STREAM_END");
        /* If the decompression completed then collate */
        /* the trailer info */
        trl_frag = MIN((GZIP_TRL_LEN - dev->trl_idx),
                       dev->d_stream.avail_in);
        for (i = 0; i < trl_frag; i++)
            dev->trl[dev->trl_idx++] = *dev->d_stream.next_in++;
        if (dev->trl_idx != GZIP_TRL_LEN)
            dbg_printf("trl_idx %d, stream %d", dev->trl_idx,
                dev->d_stream.avail_in);
        /* Once we have all the trailer info then check the crc */
        if (dev->trl_idx == GZIP_TRL_LEN) {
            /* Get the orig. files len and crc32 value */
            uncmp_len = dev->trl[4];
            uncmp_len |= dev->trl[5]<<8;
            uncmp_len |= dev->trl[6]<<16;
            uncmp_len |= dev->trl[7]<<24;
            /* Do a CRC32 on the uncompressed data */
            dbg_printf("chip ver %x, rev %d\n", dev->sih->chip, dev->sih->chiprev);
            dec_crc = hndcrc32((uchar *)dev->dlbase, uncmp_len,
                CRC32_INIT_VALUE);
            if (hndcrc32(dev->trl, 4, dec_crc) != CRC32_GOOD_VALUE) {
                    dbg_printf("decompression: bad crc check\n");
                dev->state = DL_BAD_CRC;
#if defined(DL_NVRAM) && !defined(BCMTRXV2)
            } else if (dev->nvramsz) {
                if (dev->nvramsz > DL_NVRAM) {
                    dev->state = DL_NVRAM_TOOBIG;
                    return -1;
                } else {
                    uint varsz;
                    dbg_printf("accepting nvram download");
                    /* size is 32-bit word count */
                    varsz = ROUNDUP(dev->nvramsz, 4)/4;
                    /* upper 16 bits is ~size */
                    *_dlvarsz = (~varsz << 16) | varsz;
                    usbdev_nvramdl(dev, pdata, len, dev->d_stream.next_in);
                }
#endif /* DL_NVRAM && !BCMTRXV2 */
            } else
                dev->state = DL_RUNNABLE;
        }
    } else {
        dbg_printf("%d ", dev->zstatus);
    }

    return 0;
}
#else
#define copy_decompress(dev, pdata, len) (-1)
#endif /* ZLIB */

/* uncompressed image: copy the data to dlcurrentbase directly */
static int
copy_direct(struct dngl_bus *dev, unsigned char *pdata, uint32 len)
{
    int downloaded = dev->dlcurrentlen - dev->hdrlen;
#ifdef BCM_SDRBL
    struct trx_header *hdr = (struct trx_header *) (_rambottom
                 - sizeof(struct trx_header) - 4);
#endif /* BCM_SDRBL */

    if ((downloaded + len) >= dev->dlfwlen) {
        bcopy(pdata, dev->dlcurrentbase, dev->dlfwlen - downloaded);
#if defined(DL_NVRAM) && !defined(BCMTRXV2)
        if (dev->nvramsz) {
            if (dev->nvramsz > DL_NVRAM) {
                dev->state = DL_NVRAM_TOOBIG;
                return -1;
            } else {
                uint varsz;
                /* size is 32-bit word count */
                varsz = ROUNDUP(dev->nvramsz, 4)/4;
                /* upper 16 bits is ~size */
                *_dlvarsz = (~varsz << 16) | varsz;
                usbdev_nvramdl(dev, pdata, len, pdata +
                               (dev->dlfwlen - downloaded));
            }
        } else
            dev->state = DL_RUNNABLE;
#else
#ifdef BCM_SDRBL
        if (validate_sdr_image(hdr)) {
            dev->state = DL_RUNNABLE;
        } else {
            dev->state = DL_BAD_CRC;
            dev->jumpto = 0xFFFFFFFF;
        }
#else
        dev->state = DL_RUNNABLE;
#endif /* BCM_SDRBL */
#endif /* DL_NVRAM && !BCMTRXV2 */
    } else
        bcopy(pdata, dev->dlcurrentbase, len);

    return 0;
}

void
usbdev_rx(struct dngl_bus *dev, int ep, void *p)
{
    unsigned char *pdata = PKTDATA(dev->osh, p);
    uint32 len = PKTLEN(dev->osh, p);
    int hdrlen, rv;

    trace("ep %d %d", ep, len);

    /* handle control OUT data */
    if (ep == 0) {
        unsigned int cmd = *((unsigned int *) pdata);

        if (len < sizeof(unsigned int)) {
            err("ep%d: pkt tossed len %d", ep, len);
            goto toss;
        }

        dbg_printf("ep%d: Ctrl-out cmd %d; packet len %d", ep, cmd, len);
        switch (cmd) {
        case DL_WRHW:
            if (len == sizeof(hwacc_t)) {
                hwacc_t *hwacc;
                uint32 *addr;

                hwacc = (hwacc_t *)pdata;
                dbg_printf("DL_WRHW: addr 0x%x <= 0x%x; len %d", hwacc->addr,
                    hwacc->data, hwacc->len);
                addr = (uint32 *)OSL_UNCACHED(hwacc->addr);
                switch (hwacc->len) {
                case 1:
                    *((uint8 *)addr) = (uint8)(hwacc->data & 0xff);
                    break;
                case 2:
                    *((uint16 *)addr) = (uint16)(hwacc->data & 0xffff);
                    break;
                case 4:
                    *addr = hwacc->data;
                    break;
                default:
                    err("ep%d: invalid WRHW cmd len: %d", ep, hwacc->len);
                    break;
                }
            } else {
                /* toss other pkts from the ctrl e/p */
                err("ep%d: WRHW invalid pkt len %d", ep, len);
            }
            break;
         case DL_WRHW_BLK:
            {
                hwacc_blk_t *hwacc;
                uint32 *addr;

                hwacc = (hwacc_blk_t *)pdata;
                dbg_printf("DL_WRHW_BLK: addr 0x%x <= 0x%x...; len %d", hwacc->addr,
                     hwacc->data[0], hwacc->len);
                addr = (uint32 *)OSL_UNCACHED(hwacc->addr);
                memcpy(addr, hwacc->data, hwacc->len);
            }
            break;

        case DL_GET_NVRAM:
            {
                /* save the nvram query string
                 * response will be sent in usbdev_setup
                 */
                nvparam_t *nv = (nvparam_t *)pdata;
                uint8 len = strlen(nv->var);

                if (len <= QUERY_STRING_MAX)
                    strncpy(dev->var, nv->var, len+1);

                else
                    bzero(dev->var, QUERY_STRING_MAX);
            }
            break;

        default:
            err("ep%d: invalid cmd: %d", ep, cmd);
            break;
        }
        goto toss;
    }

    switch (dev->state) {

    case DL_WAITING:
        /* Process trx/gzip header */
        hdrlen = check_headers(dev, pdata);
        dbg_printf("DL_WAITING: process headers");
#ifdef ZLIB
        if (dev->comp_image)
            init_zlib_state(dev);
#endif
        if (hdrlen < 0) {
            err("Error checking headers");
            dev->state = DL_BAD_HDR;
        } else {
            dev->state = DL_READY;
            /* Skip the headers */
            pdata += hdrlen;
            dev->dlcurrentlen += hdrlen;
            len -= hdrlen;
            dev->hdrlen = hdrlen;
#ifdef ZLIB
            /* Set the decompression base addr */
            dev->d_stream.next_out = dev->dlcurrentbase;
#endif
        }
        /* deliberate fall through */
    case DL_READY:
        /* if decompression is done simply download
         * the rest of the file (if its padded).
         */
        dbg_printf("DL_READY: got %d bytes", len);
        if (dev->state == DL_READY) {
            if (!dev->nvramoff) {
                if (dev->comp_image)
                    rv = copy_decompress(dev, pdata, len);
                else
                    rv = copy_direct(dev, pdata, len);
                if (rv < 0)
                    break;
            }
#if defined(DL_NVRAM) && !defined(BCMTRXV2)
            else
                usbdev_nvramdl(dev, pdata, len, pdata);
#endif /* DL_NVRAM && !BCMTRXV2 */
        }
    /* deliberate fall through */
    default:
        /* update status of dl data */
        dev->dlcurrentbase += len;
        dev->dlcurrentlen += len;
        if (dev->dlcurrentlen == dev->dllen) {
            if (dev->state != DL_RUNNABLE) {
                err("inflate incomplete");
                if (dev->state != DL_NVRAM_TOOBIG)
                    dev->state = DL_BAD_CRC;
            }
        }
    }

    dbg_printf(" pkt len 0x%x loaded 0x%x", PKTLEN(dev->osh, p), dev->dlcurrentlen);
toss:
    PKTFREE(dev->osh, p, FALSE);
    trace("done, state %d", dev->state);
}

void
usbdev_txstop(struct dngl_bus *dev, int ep)
{
    //trace("");
}

void
usbdev_txstart(struct dngl_bus *dev, int ep)
{
    //trace("");
}

uint
usbdev_mps(struct dngl_bus *dev)
{
    //trace("");
    return dev->device.bMaxPacketSize;
}

uint
usbdev_setcfg(struct dngl_bus *dev, int config)
{
    uint i;
    const usb_endpoint_descriptor_t *data_endpoints;
    usb_endpoint_companion_descriptor_t *ss_cmpnt = NULL;

#ifdef BCMUSBDEV_COMPOSITE
    int j, k;
    int bi, bo;
#endif /* BCMUSBDEV_COMPOSITE */

    //trace("");

    if (config == dev->config.bConfigurationValue || config == 0)
    {
        /* Attach configuration endpoints */
        dev->confignum = config;
    }

    if (config == 1) {
#ifdef BCMUSBDEV_COMPOSITE
        j = bi = bo = 0;
#ifdef USB_XDCI
        if (dev->speed == UD_USB_SS) {
            data_endpoints = dev->data_endpoints_ss;
            ss_cmpnt = dev->data_endpoints_cp_ss;
        }
        else
#endif /* USB_XDCI */

        if (dev->speed == UD_USB_HS)
            data_endpoints = dev->data_endpoints;
        else
            data_endpoints = dev->data_endpoints_fs;

        for (k = 0; k < dev->interface_num; k++) {
            for (i = 0; i < dev->ep_bi_num[k]; i++) {
                if (!dev->data_interface[k].bAlternateSetting) {
                    if (!dev->ep_bi[bi]) {
                        dev->ep_bi[bi++] =
                        usbd_ep_attach(dev, &data_endpoints[i+j],
                        &ss_cmpnt[i+j],
                        dev->config.bConfigurationValue,
                        dev->data_interface[k].bInterfaceNumber,
                        dev->data_interface[k].bAlternateSetting);
                    }
                }
            }
            j += i;

            for (i = 0; i < dev->ep_bo_num[k]; i++) {
                if (!dev->data_interface[k].bAlternateSetting) {
                    if (!dev->ep_bo[bo]) {
                        dev->ep_bo[bo++] =
                        usbd_ep_attach(dev, &data_endpoints[i+j],
                        &ss_cmpnt[i+j],
                        dev->config.bConfigurationValue,
                        dev->data_interface[k].bInterfaceNumber,
                        dev->data_interface[k].bAlternateSetting);
                    }
                }
            }
            j += i + dev->ep_isi_num[k] + dev->ep_iso_num[k];
        }

#ifdef USB_XDCI
        if (dev->usb30d) {
            for (k = 0; k < dev->interface_num; k++) {
                if (dev->speed == UD_USB_SS) {
                    data_endpoints = dev->intr_endpoints_ss;
                    ss_cmpnt = dev->intr_endpoints_cp_ss;
                }
                else
                if (dev->speed == UD_USB_HS)
                    data_endpoints = dev->intr_endpoints;
                else
                    data_endpoints = dev->intr_endpoints_fs;

                for (i = 0; i < dev->ep_ii_num[k]; i++) {
                    if (!dev->data_interface[k].bAlternateSetting) {
                        usbd_ep_attach(dev, &data_endpoints[i],
                            &ss_cmpnt[i],
                            dev->config.bConfigurationValue,
                            dev->data_interface[k].bInterfaceNumber,
                            dev->data_interface[k].bAlternateSetting);
                    }
                }
            }
        }
#endif /* USB_XDCI */

#else
        /*
        Call ep_attach for bulk IN endpoint,
        so that the endpoint configuration is not overrided
        while attaching bulk OUT endpoint. Otherwise the
        endpoint number 2 is configured with invalid value.
        */
        for (i = 0; i < dev->ep_bi_num; i++) {
#ifdef USB_XDCI
            if (dev->speed == UD_USB_SS) {
                data_endpoints = &dev->data_endpoints_ss[i];
                ss_cmpnt = &dev->data_endpoints_cp_ss[i];
            }
            else
#endif /* USB_XDCI */
            data_endpoints = (dev->speed) ?
                &dev->data_endpoints[i] :
                &dev->data_endpoints_fs[i];

            if (!dev->ep_bi[i])
                dev->ep_bi[i] = usbd_ep_attach(dev, data_endpoints,
                ss_cmpnt,
                dev->config.bConfigurationValue,
                dev->data_interface->bInterfaceNumber,
                dev->data_interface->bAlternateSetting);
        }

        /* Attach configuration endpoints */
        for (i = 0; i < dev->ep_bo_num; i++) {
            if (dev->ep_bo[i])
                continue;

            /* BULK OUT EP is after Bulk IN EP */
#ifdef USB_XDCI
            if (dev->speed == UD_USB_SS) {
                data_endpoints = &dev->data_endpoints_ss[dev->ep_bi_num];
                ss_cmpnt = &dev->data_endpoints_cp_ss[dev->ep_bi_num];
            }
            else
#endif /* USB_XDCI */
            data_endpoints = (dev->speed) ?
                &dev->data_endpoints[dev->ep_bi_num] :
                &dev->data_endpoints_fs[dev->ep_bi_num];
            dev->ep_bo[i] = usbd_ep_attach(
                dev, data_endpoints, ss_cmpnt,
                dev->config.bConfigurationValue,
                dev->data_interface->bInterfaceNumber,
                dev->data_interface->bAlternateSetting);
            //trace("done dev_rx %d", i);
        }
#endif /* BCMUSBDEV_COMPOSITE */
    }

#ifdef USB_XDCI
    if (dev->usb30d) {
        return dev->config.bConfigurationValue;
    }
#endif

    return dev->config.bmAttributes;
}

int
usbdev_getcfg(struct dngl_bus *dev)
{
    return dev->confignum;
}

void
usbdev_speed(struct dngl_bus *dev, int high)
{
    usbdev_isr_debug_printf("%s", high ? "high" : "full");
    dev->speed = high;
}

void
usbdev_reset(struct dngl_bus *dev)
{
    uint i;
#ifdef BCMUSBDEV_COMPOSITE
    int k;
    int bi, bo;
#endif /* BCMUSBDEV_COMPOSITE */
    //trace("");

#ifdef BCMUSBDEV_COMPOSITE
    bi = bo = 0;
    for (k = 0; k < dev->interface_num; k++) {
        for (i = 0; i < dev->ep_bo_num[k]; i++) {
            if (!dev->data_interface[k].bAlternateSetting) {
                if (dev->ep_bo[bo]) {
                    usbd_ep_detach(dev, dev->ep_bo[bo]);
                    dev->ep_bo[bo++] = 0;
                }
            }
        }

        for (i = 0; i < dev->ep_bi_num[k]; i++) {
            if (!dev->data_interface[k].bAlternateSetting) {
                if (dev->ep_bi[bi]) {
                    usbd_ep_detach(dev, dev->ep_bi[bi]);
                    dev->ep_bi[bi++] = 0;
                }
            }
        }
    }
#else
    /* Detach configuration endpoints */
    for (i = 0; i < dev->ep_bo_num; i++) {
        if (dev->ep_bo[i]) {
            usbd_ep_detach(dev, dev->ep_bo[i]);
            dev->ep_bo[i] = 0;
        }
    }

    for (i = 0; i < dev->ep_bi_num; i++) {
        if (dev->ep_bi[i]) {
            usbd_ep_detach(dev, dev->ep_bi[i]);
            dev->ep_bi[i] = 0;
        }
    }
#endif /* BCMUSBDEV_COMPOSITE */
    usbdev_rdl_dl_state_reset(dev);
}

void
usbdev_sof(struct dngl_bus *dev)
{
    //trace("");
}

void
usbdev_suspend(struct dngl_bus *dev)
{
    //trace("");
}

void
usbdev_resume(struct dngl_bus *dev)
{
    //trace("");
}

/* check_headers:
 *      parse and verify the trx and gzip headers.
 *
 *      RETURN: the length of the header scetion
 *
 */

static int check_headers(struct dngl_bus *dev, unsigned char *headers)
{
    struct trx_header *trx;
    unsigned char *pdata = headers;
    uint32  trxhdr_size;

    /* Extract trx header and init data */
    trx = (struct trx_header *)headers;
    trxhdr_size = SIZEOF_TRX(trx);

    if (trx->magic == TRX_MAGIC) {
        /* get download address from trx header */
        dev->dlbase = DL_BASE;
        dev->dlcurrentbase = (uchar*)dev->dlbase;
        /* get the firmware len */
        dev->dlfwlen = trx->offsets[TRX_OFFSETS_DLFWLEN_IDX];

        dbg_printf("trx flags=0x%04x, version=0x%04x",
            trx->flag_version & 0xFFFF, trx->flag_version >> 16);
#ifndef BCM_SDRBL
#ifdef DATA_START
        if ((trx->flag_version & TRX_UNCOMP_IMAGE) &&
            dev->dlfwlen > (DATA_START & ~(MEM_REMAP_MASK))) {
            dev->state = DL_IMAGE_TOOBIG;
            dbg_printf("rdl: image len %d greater than max image size %d\n",
                dev->dlfwlen, (DATA_START & ~(MEM_REMAP_MASK)));
            return -1;
        }
#endif /* DATA_START */
#endif /* ! BCM_SDRBL */
        /* get start address from trx header */
        dev->jumpto = trx->offsets[TRX_OFFSETS_JUMPTO_IDX];
        dev->dlcrc = trx->crc32;
        dev->dllen = trx->len;
        dev->state = DL_READY;

#ifndef BCMTRXV2
        dev->nvramsz = trx->offsets[TRX_OFFSETS_NVM_LEN_IDX];
#else /* BCMTRXV2 */
        /* With trxv2 header, we will copy the TRXV2 header
         * to top of RAM. TRX header should provide the FW
         * with required size/offset information for NVRAM.
         * No plans to copy nvram to top of RAM, FW on bootup
         * is expected to detect the TRX header from top of RAM
         * and thereby figureout the offsets of nvram region.
         * In simple terms DL_NVRAM mechanism will be discontinued
         * with TRXV2 header.
         * In new image format, we download entire trx file,
         * as is. Bootloader does not handle nvram specially,
         * It is upto the downloaded FW to detect and
         * treat various regions from TRX header placed at top
         * of RAM.
         */
        if (ISTRX_V1(trx)) {
            dev->nvramsz = trx->offsets[TRX_OFFSETS_NVM_LEN_IDX];
        } else {
            dev->dlfwlen += trx->offsets[TRX_OFFSETS_NVM_LEN_IDX]
                    + trx->offsets[TRX_OFFSETS_DSG_LEN_IDX]
                    + trx->offsets[TRX_OFFSETS_CFG_LEN_IDX];
            /* Copy the TRX header to top of RAM */
            memcpy((void *)(_rambottom - trxhdr_size - 4),
                trx, trxhdr_size);
        }
#endif /* BCMTRXV2 */
        dbg_printf("usbdev_rx, dlbase 0x%x", dev->dlbase);
        dbg_printf("usbdev_rx, jumpto 0x%x", dev->jumpto);
        dbg_printf("usbdev_rx, dllen 0x%x", dev->dllen);
        dbg_printf("usbdev_rx, dlcrc 0x%x", dev->dlcrc);
    }
    else {
        dev->state = DL_BAD_HDR;
        err("rdl: trx bad hdr");
        return -1;
    }

    headers += trxhdr_size;

    if (!(trx->flag_version & TRX_UNCOMP_IMAGE)) {
#ifdef ZLIB
        uint32 len;
        int method; /* method byte */
        int flags;  /* flags byte */
        uint8 gz_magic[2] = {0x1f, 0x8b}; /* gzip magic header */

        dev->comp_image = 1;
        /* Extract the gzip header info */
        if ((*headers++ != gz_magic[0]) || (*headers++ != gz_magic[1]))
            return -1;

        method = (int) *headers++;
        flags = (int) *headers++;

        if (method != Z_DEFLATED || (flags & RESERVED) != 0)
            return -1;

        /* Discard time, xflags and OS code: */
        for (len = 0; len < 6; len++)
            headers++;

        if ((flags & EXTRA_FIELD) != 0) { /* skip the extra field */
            len = (uint32) *headers++;
            len += ((uint32)*headers++) << 8;
            /* len is garbage if EOF but the loop below will quit anyway */
            while (len-- != 0)
                headers++;
        }
        if ((flags & ORIG_NAME) != 0) { /* skip the original file name */
            while (*headers++ && (*headers != 0));
        }
        if ((flags & COMMENT) != 0) {   /* skip the .gz file comment */
            while (*headers++ && (*headers != 0));
        }
        if ((flags & HEAD_CRC) != 0) {  /* skip the header crc */
            for (len = 0; len < 2; len++)
                headers++;
        }
        headers++;  /* XXX: I need this, why ? */
#else
        err("USB downloader doesn't support compressed images");
        return -1;
#endif /* ZLIB */
    }

    /* return the length of the headers */
    return (int)((int)headers - (int)pdata);
}

static int usbd_chk_version(struct dngl_bus *bus)
{
    int ver;
#ifdef USB_XDCI
    if (bus->usb30d) {
        ver = xdci_usb30(bus->ch);
    }
    else
#endif /* USB_XDCI */
    if (ch_usb20(bus->ch))
        ver = UD_USB_2_0_DEV;
    else
        ver = UD_USB_1_1_DEV;
    return ver;
}

static bool usbd_hsic(struct dngl_bus *bus)
{
#ifdef USB_XDCI
    if (bus->usb30d) {
        return FALSE;
    }
    else
#endif /* USB_XDCI */
    return ch_hsic(bus->ch);
}

#ifdef USB_IFTEST
static int usbd_resume(struct dngl_bus *bus)
{
#ifdef USB_XDCI
    if (bus->usb30d) {
        return xdci_resume(bus->ch);
    }
    else
#endif /* USB_XDCI */
    return ch_resume(bus->ch);
}
#endif /* USB_IFTEST */

static uint usbd_ep_attach(struct dngl_bus *bus, const usb_endpoint_descriptor_t *endpoint,
    usb_endpoint_companion_descriptor_t *sscmp,
    int config, int interface, int alternate)
{
#ifdef USB_XDCI
    if (bus->usb30d) {
        return xdci_ep_attach(bus->ch, endpoint, sscmp, config, interface, alternate);
    }
    else
#endif /* USB_XDCI */
        return ep_attach(bus->ch, endpoint, config, interface, alternate);

}

static void usbd_ep_detach(struct dngl_bus *bus, int ep)
{
#ifdef USB_XDCI
    if (bus->usb30d) {
        xdci_ep_detach(bus->ch, ep);
    }
    else
#endif /* USB_XDCI */
    ep_detach(bus->ch, ep);
}
