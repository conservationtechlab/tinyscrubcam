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
 * Defines WWD SDIO functions for PSoC 6 MCU
 */

#include <string.h> /* For memcpy */
#include "wwd_platform_common.h"
#include "wwd_bus_protocol.h"
#include "wwd_assert.h"
#include "platform/wwd_platform_interface.h"
#include "platform/wwd_sdio_interface.h"
#include "platform/wwd_bus_interface.h"
#include "RTOS/wwd_rtos_interface.h"
#include "network/wwd_network_constants.h"
#include "platform_cmsis.h"
#include "platform_peripheral.h"
#include "platform_config.h"
#include "wwd_rtos_isr.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define VERIFY_SDIF_API_RESULT( x ) \
    { \
        en_sdio_result_t api_result; \
        api_result = (x); \
        if ( api_result != Ok ) \
        { \
            wiced_assert( "command failed", ( 0 == 1 )); \
            return WWD_SDIO_BUS_UP_FAIL; \
        } \
    }

/******************************************************
 *             Constants
 ******************************************************/

#define BUS_LEVEL_MAX_RETRIES                (5)
#define SDIO_ENUMERATION_TIMEOUT_MS          (500)

/******************************************************
 *             Structures
 ******************************************************/

/******************************************************
 *             Variables
 ******************************************************/

#ifndef WICED_PLATFORM_DOESNT_USE_TEMP_DMA_BUFFER
ALIGNED_PRE(4) static uint8_t temp_dma_buffer[MAX(2*1024, WICED_LINK_MTU+64)] ALIGNED(4);
#endif

/******************************************************
 *             Static Function Declarations
 ******************************************************/

/******************************************************
 *             Function definitions
 ******************************************************/

#ifndef  WICED_DISABLE_MCU_POWERSAVE
static void sdio_oob_irq_handler( void* arg )
{
    UNUSED_PARAMETER(arg);
    WWD_BUS_STATS_INCREMENT_VARIABLE( oob_intrs );
    platform_mcu_powersave_exit_notify( );
    wwd_thread_notify_irq( );
}

wwd_result_t host_enable_oob_interrupt( void )
{
    /* TODO: Setup WiFi OOB interrupt GPIO pin (if required) for PSoC 6 */
    platform_gpio_init( &wifi_sdio_pins[WWD_PIN_SDIO_OOB_IRQ], INPUT_HIGH_IMPEDANCE );
    platform_gpio_irq_enable( &wifi_sdio_pins[WWD_PIN_SDIO_OOB_IRQ], IRQ_TRIGGER_RISING_EDGE, sdio_oob_irq_handler, 0 );
    return WWD_SUCCESS;
}

uint8_t host_platform_get_oob_interrupt_pin( void )
{
    /* TODO: Setup WiFi OOB interrupt GPIO pin (if required) for PSoC 6 */
    return WICED_WIFI_OOB_IRQ_GPIO_PIN;
}
#endif /* ifndef  WICED_DISABLE_MCU_POWERSAVE */

wwd_result_t host_platform_bus_init( void )
{
    wwd_result_t        result = WWD_SUCCESS;
    stc_sdio_irq_cb_t   irq_cbs;
    uint32_t            flags;

    platform_mcu_powersave_disable();

#ifdef WICED_WIFI_USE_GPIO_FOR_BOOTSTRAP_0
    /* TODO: Setup GPIO pin (if required) to put WLAN module into SDIO mode on PSoC 6 */
    platform_gpio_init( &wifi_control_pins[WWD_PIN_BOOTSTRAP_0], OUTPUT_PUSH_PULL );
    platform_gpio_output_low( &wifi_control_pins[WWD_PIN_BOOTSTRAP_0] );
#endif
#ifdef WICED_WIFI_USE_GPIO_FOR_BOOTSTRAP_1
    /* TODO: Setup GPIO pin (if required) to put WLAN module into SDIO mode on PSoC 6 */
    platform_gpio_init( &wifi_control_pins[WWD_PIN_BOOTSTRAP_1], OUTPUT_PUSH_PULL );
    platform_gpio_output_low( &wifi_control_pins[WWD_PIN_BOOTSTRAP_1] );
#endif

    irq_cbs.pfnCardIntCb      = wwd_thread_notify_irq;

    /* Disable global interrupts */
    WICED_SAVE_INTERRUPTS(flags);

    /* WiFi into reset */
    Cy_SysLib_Delay( 10 );
    Cy_GPIO_Write( EnableWifi_0_PORT,EnableWifi_0_NUM, 0 );
    Cy_SysLib_Delay( 10 );

    /* Init SDIO Host */
    SDIO_Init( &irq_cbs );

    /* WiFi out of reset */
    Cy_SysLib_Delay( 10 );
    Cy_GPIO_Write( EnableWifi_0_PORT,EnableWifi_0_NUM, 1 );
    Cy_SysLib_Delay( 10 );

    /* SDIO Host out of reset */
    SDIO_Reset();

    /* Enable global interrupts */
    WICED_RESTORE_INTERRUPTS(flags);

    platform_mcu_powersave_enable();

    return result;
}

wwd_result_t host_platform_sdio_enumerate( void )
{
    wwd_result_t result;
    uint32_t       loop_count;
    uint32_t       data = 0;

    loop_count = 0;
    do
    {
        /* Send CMD0 to set it to idle state */
        host_platform_sdio_transfer( BUS_WRITE, SDIO_CMD_0, SDIO_BYTE_MODE, SDIO_1B_BLOCK, 0, 0, 0, NO_RESPONSE, NULL );

        /* CMD5. */
        host_platform_sdio_transfer( BUS_READ, SDIO_CMD_5, SDIO_BYTE_MODE, SDIO_1B_BLOCK, 0, 0, 0, NO_RESPONSE, NULL );

        /* Send CMD3 to get RCA. */
        result = host_platform_sdio_transfer( BUS_READ, SDIO_CMD_3, SDIO_BYTE_MODE, SDIO_1B_BLOCK, 0, 0, 0, RESPONSE_NEEDED, &data );
        loop_count++;
        if ( loop_count >= (uint32_t) SDIO_ENUMERATION_TIMEOUT_MS )
        {
            return WWD_TIMEOUT;
        }
    } while ( ( result != WWD_SUCCESS ) && ( host_rtos_delay_milliseconds( (uint32_t) 1 ), ( 1 == 1 ) ) );
    /* If you're stuck here, check the platform matches your hardware */

    /* Send CMD7 with the returned RCA to select the card */
    host_platform_sdio_transfer( BUS_WRITE, SDIO_CMD_7, SDIO_BYTE_MODE, SDIO_1B_BLOCK, data, 0, 0, RESPONSE_NEEDED, NULL );

    return WWD_SUCCESS;
}

wwd_result_t host_platform_bus_deinit( void )
{
    return WWD_SUCCESS;
}

wwd_result_t host_platform_sdio_transfer( wwd_bus_transfer_direction_t direction, sdio_command_t command, sdio_transfer_mode_t mode, sdio_block_size_t block_size, uint32_t argument, /*@null@*/ uint32_t* data, uint16_t data_size, sdio_response_needed_t response_expected, /*@out@*/ /*@null@*/ uint32_t* response )
{
    wwd_result_t result;
    uint16_t attempts = 0;
    stc_sdio_cmd_t cmd = { 0 };
    uint32_t cmdResponse;

    wiced_assert("Bad args", !((command == SDIO_CMD_53) && (data == NULL)));

    if ( response != NULL )
    {
        *response = 0;
    }

    platform_mcu_powersave_disable();

restart:
    ++attempts;

    /* Check if we've tried too many times */
    if (attempts >= (uint16_t) BUS_LEVEL_MAX_RETRIES)
    {
        result = WWD_SDIO_RETRIES_EXCEEDED;
        goto exit;
    }

    /* Prepare the data transfer register */
    if ( command == SDIO_CMD_53 )
    {
        cmd.u32CmdIdx       = (uint32_t)command;
        cmd.u32Arg          = argument;
        cmd.pu8Data         = (uint8_t *)data;
        cmd.pu32Response    = &cmdResponse;

        if( mode == SDIO_BLOCK_MODE )
        {
            cmd.u16BlockSize    = block_size;
            cmd.u16BlockCnt     = (uint16_t)( data_size + block_size - 1 ) / block_size;
        }
        else
        {
            cmd.u16BlockSize    = data_size;
            cmd.u16BlockCnt     = 1;
        }

        cmd.bRead           = ( direction == BUS_WRITE ) ? false : true;

#ifndef WICED_PLATFORM_DOESNT_USE_TEMP_DMA_BUFFER
        if (( direction == BUS_READ ) && ( mode == SDIO_BLOCK_MODE ))
        {
            /* In this mode, we may be reading more than the requested size to round
             * up to the nearest multiple of block size. So, providing temp buffer
             * instead of the original buffer to avoid memory corruption
             */
            cmd.pu8Data         = (uint8_t *)temp_dma_buffer;
        }
#endif

        if( SDIO_SendCommandAndWait( &cmd ) != Ok )
        {
            goto restart;
        }

#ifndef WICED_PLATFORM_DOESNT_USE_TEMP_DMA_BUFFER
        if (( direction == BUS_READ ) && ( mode == SDIO_BLOCK_MODE ))
        {
            memcpy( data, temp_dma_buffer, (size_t) data_size );
        }
#endif
    }
    else
    {
        cmd.u32CmdIdx       = (uint32_t) command;
        cmd.u32Arg          = argument;
        cmd.pu8Data         = NULL;
        cmd.u16BlockSize    = 0x0;
        cmd.u16BlockCnt     = 0x0;
        cmd.bRead           = ( direction == BUS_WRITE ) ? false : true;
        cmd.pu32Response    = &cmdResponse;

        if ( SDIO_SendCommandAndWait( &cmd ) != Ok )
        {
            goto restart;
        }
    }

    if (( response != NULL ) && ( response_expected == RESPONSE_NEEDED))
    {
        *response = cmdResponse;
    }

    result = WWD_SUCCESS;

exit:
    platform_mcu_powersave_enable();
    return result;
}

wwd_result_t host_platform_enable_high_speed_sdio( void )
{
    platform_mcu_powersave_disable();

#if defined (SLOW_SDIO_CLOCK)
    SDIO_SetSdClkFrequency(16666666);
#else
    SDIO_SetSdClkFrequency(25000000);
#endif /* defined (SLOW_SDIO_CLOCK) */
    platform_mcu_powersave_enable();

    return WWD_SUCCESS;
}

wwd_result_t host_platform_bus_enable_interrupt( void )
{

    SDIO_EnableChipInt();
    return  WWD_SUCCESS;
}

wwd_result_t host_platform_bus_disable_interrupt( void )
{
    SDIO_DisableChipInt();
    return  WWD_SUCCESS;
}

#ifdef WICED_PLATFORM_MASKS_BUS_IRQ
wwd_result_t host_platform_unmask_sdio_interrupt( void )
{
    return host_platform_bus_enable_interrupt();
}
#endif

void host_platform_bus_buffer_freed( wwd_buffer_dir_t direction )
{
    UNUSED_PARAMETER( direction );
}

/******************************************************
 *             IRQ Handler Definitions
 ******************************************************/

WWD_RTOS_DEFINE_ISR( SD_IRQ )
{
    WWD_BUS_STATS_INCREMENT_VARIABLE( sdio_intrs );
    SDIO_IRQ();
}

WWD_RTOS_DEFINE_ISR(READ_DMA_IRQ)
{
    SDIO_READ_DMA_IRQ();
}

WWD_RTOS_DEFINE_ISR(WRITE_DMA_IRQ)
{
    SDIO_WRITE_DMA_IRQ();
}

/******************************************************
 *             IRQ Handler Mapping
 ******************************************************/

WWD_RTOS_MAP_ISR(SD_IRQ, udb_interrupts_0_IRQn_Handler)
WWD_RTOS_MAP_ISR(READ_DMA_IRQ, cpuss_interrupts_dw1_3_IRQn_Handler)
WWD_RTOS_MAP_ISR(WRITE_DMA_IRQ, cpuss_interrupts_dw1_1_IRQn_Handler)
