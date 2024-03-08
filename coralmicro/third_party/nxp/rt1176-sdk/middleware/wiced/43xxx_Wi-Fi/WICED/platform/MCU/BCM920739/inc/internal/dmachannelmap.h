//==================================================================================================
//                        THIS INFORMATION IS PROPRIETARY TO BROADCOM CORP
//--------------------------------------------------------------------------------------------------
//                               Copyright (c) 2012 Broadcom Corp.
//                                      ALL RIGHTS RESERVED
//==================================================================================================
//! \file
//!
//! BCM20739 DMA channel map.
//
//==================================================================================================

#if DMA_NUM_CHANNELS != 5
#error "Mapping below is inconsistent with DMA_NUM_CHANNELS in the chip_features.xml spreadsheet"
#endif

//==================================================================================================
// Channel assignments and sharing property declarations

//! Flag indicating that DMA channel 0, used by the Bluetooth baseband, is not shared.
#define DMA_CHANNEL_0_SHARED        FALSE

//! DMA channel assignment for the Bluetooth baseband.
#define DMA_CHANNEL_BT_BASEBAND     0

//! Flag indicating that DMA channel 1 is shared.  It is used by a variety of transports for device
//! to host data.
#define DMA_CHANNEL_1_SHARED        FALSE

//! DMA channel assignment for UART transmit (device to host).
#define DMA_CHANNEL_UART_TX         1

//! DMA channel assignment for UART receive (host to device).
#define DMA_CHANNEL_UART_RX         2   

//! DMA channel assignment for USB receive (host to device).
#define DMA_CHANNEL_USB_RX          2

//! Flag indicating that DMA channel 2 is shared.  It is used by a variety of transports for host to
//! device data.
#define DMA_CHANNEL_2_SHARED        FALSE

//! DMA channel assignment for USB transmit (device to host).
#define DMA_CHANNEL_USB_TX          1

#define DMA_CHANNEL_3_SHARED        FALSE

//! DMA channel assignment for ADC Audio receive 
#define DMA_CHANNEL_ADC_AUDIO       3

//! DMA channel assignment for ADC Audio receive
#define DMA_CHANNEL2_ADC_AUDIO      4

//==================================================================================================

//! Peripheral index in the DMA controller for the PTU as DMA source.  This is the index at which
//! the Peripheral Transport Unit's DMA request lines are connected for transfers from PTU to memory
//! (receive).  This index is used in the 4-bit SrcPeripheral and DestPeripheral fields in the
//! dmaccNconfig_adr registers.  This mapping is defined by and must match the hardware, and
//! dictates which DMA request line will be used by a DMA controller channel when it is active.
#define DMAC_PERIPHERAL_INDEX_PTU_TO_MEMORY     0

//! Peripheral index in the DMA controller for the PTU as DMA destination.  This is the index at
//! which the Peripheral Transport Unit's DMA request lines are connected for transfers from memory
//! to PTU (transmit).  This index is used in the 4-bit SrcPeripheral and DestPeripheral fields in
//! the dmaccNconfig_adr registers.  This mapping is defined by and must match the hardware, and
//! dictates which DMA request line will be used by a DMA controller channel when it is active.
#define DMAC_PERIPHERAL_INDEX_MEMORY_TO_PTU     1

//! Peripheral index in the DMA controller for the Bluetooth baseband.  This is the index at which
//! the its DMA request lines are connected for transfers either to or from memory (receive or
//! transmit).  This index is used in the 4-bit SrcPeripheral and DestPeripheral fields in the
//! dmaccNconfig_adr registers.  This mapping is defined by and must match the hardware, and
//! dictates which DMA request line will be used by a DMA controller channel when it is active.
#define DMAC_PERIPHERAL_BT_BASEBAND             3

//! Peripheral index in the DMA controller for the PTU Aux as DMA destination.  This is the index at
//! which the Peripheral Transport Unit Aux's DMA request lines are connected for transfers from memory
//! to PTU (transmit).  This index is used in the 4-bit SrcPeripheral and DestPeripheral fields in
//! the dmaccNconfig_adr registers.  This mapping is defined by and must match the hardware, and
//! dictates which DMA request line will be used by a DMA controller channel when it is active.
#define DMAC_PERIPHERAL_INDEX_MEMORY_TO_PTU_AUX     10

//! Peripheral index in the DMA controller for the PTU Aux as DMA source.  This is the index at which
//! the Peripheral Transport Unit's DMA request lines are connected for transfers from PTU Aux to memory
//! (receive).  This index is used in the 4-bit SrcPeripheral and DestPeripheral fields in the
//! dmaccNconfig_adr registers.  This mapping is defined by and must match the hardware, and
//! dictates which DMA request line will be used by a DMA controller channel when it is active.
#define DMAC_PERIPHERAL_INDEX_PTU_AUX_TO_MEMORY     11

