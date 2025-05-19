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
 * WICED 4390x USB Device driver.
 */

#include "typedefs.h"
#include "bcmdevs.h"
#include "sbchipc.h"
#include "sbusbd.h"
#include "siutils.h"
#include "usbdev.h"
#include "platform_peripheral.h"
#include "platform_map.h"
#include "platform_config.h"
#include "platform_usb.h"
#include "platform_appscr4.h"
#include "platform_pinmux.h"
#include "hndsoc.h"
#include "wiced_osl.h"
#include "wiced_platform.h"
#include "wwd_rtos_isr.h"
#include "platform/wwd_platform_interface.h"


/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/
/* BCM4390x USB20 Device register is defined in sbusbd.h */

/* BCM4390x USB20 Device config */
typedef struct
{
    uint32_t                        usb20d_rev;
    uint32_t                        reserve1;
    uint32_t                        reserve2;
} platform_4390x_usb_device_config_t;

/* BCM4390x USB20 Device driver */
typedef struct
{
    usbdev_sb_regs_t*                               core_registers;     /* USB20 Device controller register base */
    ExtIRQn_Type                                    irq_number;         /* USB20 Device remapped IRQ number */
    platform_usb_device_irq_handler_t               irq_handler;        /* USB20 Device IRQ handler */
    uint32_t                                        unit;               /* Device index */
    uint32_t                                        device;             /* Chip ID */
    uint32_t                                        bus;                /* Bus between device and processor */
    void*                                           ch;                 /* Chip handle */
    uint32_t                                        vendor;             /* Vendor */
    uint32_t                                        coreid;             /* Core ID */
    osl_t                                           osh;                /* OSH structure */
} platform_4390x_usb_device_driver_t;

/******************************************************
 *                 Static Variables
 ******************************************************/
/* BCM4390x USB20 Device config data base */
//static platform_4390x_usb_device_config_t bcm4390x_usb_device_config_db = {0}; //Rsvd

/* BCM4390x USB20 Device driver data base */
static platform_4390x_usb_device_driver_t bcm4390x_usb_device_default_driver =
{
   .core_registers          = (usbdev_sb_regs_t*) PLATFORM_USB20D_REGBASE(0),
   .irq_number              = USB_REMAPPED_ExtIRQn,
   .irq_handler             = NULL,
   .unit                    = 0,
   .device                  = BCM47XX_USB20D_ID,
   .bus                     = SI_BUS,
   .ch                      = NULL,
   .vendor                  = VENDOR_BROADCOM,
   .coreid                  = USB20D_CORE_ID,
   .osh                     = {0}
};

static platform_4390x_usb_device_driver_t *bcm4390x_usb_device_driver = &bcm4390x_usb_device_default_driver;

/* BCM4390x USB20 Device DCI (device controller interface) resource list for USB stack driver */
static platform_usb_device_dci_resource_t bcm4390x_usb_device_dci_resource_list[] =
{
    /* BRCM USB20 Device controller */
    {
        .usb_device_dci_type            = USB_DEVICE_CONTROLLER_INTERFACE_BRCM, /* BRCM USB20 Device controller */
        .usb_device_dci_ioaddress       = 0,                                    /* BRCM USB20 Device controller register base */
        .usb_device_dci_irq_number      = 0,                                    /* BRCM USB20 Device IRQ number */
        .usb_device_dci_private_data    = NULL                                  /* BRCM USB20 Device private data: Providing chip handle here */
    }
};

/******************************************************
 *               Function Declarations
 ******************************************************/
static void bcm4390x_usb_device_enable( void );
static void bcm4390x_usb_device_set_dci_resource( platform_4390x_usb_device_driver_t *driver );
static void bcm4390x_usb_device_map_irq( uint8_t target_irq );
static void bcm4390x_usb_device_disable( void );
static void bcm4390x_usb_device_enable_irq( void );
static void bcm4390x_usb_device_disable_irq( void );

/******************************************************
 *               Function Definitions
 ******************************************************/
static void bcm4390x_usb_device_enable( void )
{
    usbdev_sb_regs_t *usb_device_registers = NULL;

    if ( !bcm4390x_usb_device_driver )
    {
        WPRINT_PLATFORM_ERROR( ("Null usb device drv!\n") );
        return;
    }

    /* Obtain usb20d register base  */
    usb_device_registers = (usbdev_sb_regs_t *)(bcm4390x_usb_device_driver->core_registers);

    /* Init usb20d si handle and chip handle for core init */
    bcm4390x_usb_device_driver->ch = ch_attach( (void *)bcm4390x_usb_device_driver,
                                                (uint)(bcm4390x_usb_device_driver->vendor),
                                                (uint)(bcm4390x_usb_device_driver->device),
                                                (osl_t *)&(bcm4390x_usb_device_driver->osh),
                                                (void *)(bcm4390x_usb_device_driver->core_registers),
                                                (uint)(bcm4390x_usb_device_driver->bus) );

    if ( !(bcm4390x_usb_device_driver->ch) )
    {
        WPRINT_PLATFORM_ERROR( ("Null usb device chip handle!!!\n") );
        return;
    }

    UNUSED_PARAMETER( usb_device_registers );
}

static void bcm4390x_usb_device_set_dci_resource( platform_4390x_usb_device_driver_t *driver )
{
    uint32_t resource_num = (sizeof(bcm4390x_usb_device_dci_resource_list) / sizeof(bcm4390x_usb_device_dci_resource_list[0]));
    int i;

    if ( !driver )
    {
        WPRINT_PLATFORM_ERROR( ("Null driver!\n") );
        return;
    }

    for (i = 0; i < resource_num; i ++)
    {
        if ( bcm4390x_usb_device_dci_resource_list[i].usb_device_dci_type == USB_DEVICE_CONTROLLER_INTERFACE_BRCM )
        {
            bcm4390x_usb_device_dci_resource_list[i].usb_device_dci_ioaddress = (uint32_t)(driver->core_registers);
            bcm4390x_usb_device_dci_resource_list[i].usb_device_dci_irq_number = driver->irq_number;
            bcm4390x_usb_device_dci_resource_list[i].usb_device_dci_private_data = (void *)(driver->ch);
        }
    }
}

static void bcm4390x_usb_device_map_irq( uint8_t target_irq )
{
    /*
     * Route this bus line to target_irq bit
     * i.e. Relpace target irq with USB20 device irq
     */
    platform_irq_remap_sink( Core10_ExtIRQn, target_irq );
}

static void bcm4390x_usb_device_disable( void )
{
    /* Deinit usb20d si handle and chip handle for core init */
    ch_detach( bcm4390x_usb_device_driver->ch, TRUE );

    return;
}

static void bcm4390x_usb_device_enable_irq( void )
{
    if (bcm4390x_usb_device_driver->irq_handler == NULL)
    {
        WPRINT_PLATFORM_DEBUG( ("No ISR set before IRQ enable!\n") );
    }
    platform_irq_enable_irq( bcm4390x_usb_device_driver->irq_number );
}

static void bcm4390x_usb_device_disable_irq( void )
{
    platform_irq_disable_irq( bcm4390x_usb_device_driver->irq_number );
}

WWD_RTOS_DEFINE_ISR( platform_usb_device_isr )
{
    bcm4390x_usb_device_driver->irq_handler();
}
#ifdef USB_DEVICE_MODE
WWD_RTOS_MAP_ISR( platform_usb_device_isr, USB_HOST_ISR )
#endif

platform_result_t platform_usb_device_init( void )
{
    if (platform_is_board_in_usb_phy_mode() == WICED_FALSE)
    {
        WPRINT_PLATFORM_INFO( ("Detected board strapping is NOT in USB-PHY mode!!!\n") );
        return PLATFORM_ERROR;
    }
    if (platform_is_board_in_usb_host_mode() == WICED_TRUE)
    {
        WPRINT_PLATFORM_INFO( ("Detected board is NOT in USB Device mode!!!\n") );
        WPRINT_PLATFORM_INFO( ("Please plug micro-B USB cord on bcm4390x uAB DRD port before board booting!\n") );
        return PLATFORM_ERROR;
    }

    bcm4390x_usb_device_enable();
    bcm4390x_usb_device_set_dci_resource(bcm4390x_usb_device_driver);

    return PLATFORM_SUCCESS;
}

void platform_usb_device_deinit( void )
{
    bcm4390x_usb_device_disable_irq();
    bcm4390x_usb_device_disable();
}

platform_result_t platform_usb_device_init_irq( platform_usb_device_irq_handler_t irq_handler )
{
    bcm4390x_usb_device_map_irq(bcm4390x_usb_device_driver->irq_number);

    if (irq_handler == NULL)
    {
        WPRINT_PLATFORM_ERROR( ("Null input!\n") );
        return PLATFORM_ERROR;
    }
    bcm4390x_usb_device_driver->irq_handler = irq_handler;

    return PLATFORM_SUCCESS;
}

platform_result_t platform_usb_device_enable_irq( void )
{
    if (bcm4390x_usb_device_driver->irq_handler == NULL)
    {
        WPRINT_PLATFORM_ERROR( ("No irq handler!\n") );
        return PLATFORM_ERROR;
    }
    bcm4390x_usb_device_enable_irq();

    return PLATFORM_SUCCESS;
}

platform_result_t platform_usb_device_disable_irq( void )
{
    bcm4390x_usb_device_disable_irq();
    return PLATFORM_SUCCESS;
}

platform_result_t platform_usb_device_get_dci_resource( platform_usb_device_dci_resource_t *resource_list_buf, uint32_t buf_size, uint32_t *resource_total_num )
{
    uint32_t resource_num = (sizeof(bcm4390x_usb_device_dci_resource_list) / sizeof(bcm4390x_usb_device_dci_resource_list[0]));
    uint32_t resource_list_size;

    *resource_total_num = 0;

    /* Error checking  */
    if ((resource_num == 0) || (resource_num > USB_DEVICE_CONTROLLER_INTERFACE_MAX))
    {
        WPRINT_PLATFORM_ERROR( ("Internal error! Check DCI resource list assign\n") );
        return PLATFORM_ERROR;
    }
    if (resource_list_buf == NULL)
    {
        WPRINT_PLATFORM_ERROR( ("Null input!\n") );
        return PLATFORM_ERROR;
    }

    resource_list_size = (resource_num * sizeof(platform_usb_device_dci_resource_t));
    if (buf_size < resource_list_size)
    {
        WPRINT_PLATFORM_ERROR( ("DCI resource buf size not enough! buf_size=%lu, resource_list_size=%lu\n", buf_size, resource_list_size) );
        return PLATFORM_ERROR;
    }

    /* Get DCI resource list and total resource number */
    memcpy((void *)resource_list_buf, (void *)bcm4390x_usb_device_dci_resource_list, resource_list_size);
    *resource_total_num = resource_num;
    WPRINT_PLATFORM_INFO( ("USB Device support %u DCI resource\n", (unsigned)resource_num) );

    return PLATFORM_SUCCESS;
}
