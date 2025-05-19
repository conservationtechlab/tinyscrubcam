/*******************************************************************************
* File Name: SDIO_HOST_Read_DMA.h
* Version 2.0
*
* Description:
*  This file provides constants, parameter values and API definition for the
*  SDIO_HOST_Read_DMA Component.
*
********************************************************************************
* Copyright 2016-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(SDIO_HOST_Read_DMA_CH_H)
#define SDIO_HOST_Read_DMA_CH_H

#include "cy_device_headers.h"
#include "cyfitter.h"
#include "dma/cy_dma.h"
#include "trigmux/cy_trigmux.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define SDIO_HOST_Read_DMA_DW_BLOCK       (1u)
#define SDIO_HOST_Read_DMA_DW_CHANNEL     (3u)
#define SDIO_HOST_Read_DMA_HW             (DW1)
#define SDIO_HOST_Read_DMA_INTR_MASK      (CY_DMA_INTR_MASK)

/* Channel settings */
#define SDIO_HOST_Read_DMA_PRIORITY       (0u)
#define SDIO_HOST_Read_DMA_DESCRIPTOR_NUM (1u)
#define SDIO_HOST_Read_DMA_PREEMPTABLE    (false)

/***************************************
*        Function Prototypes
***************************************/

                void SDIO_HOST_Read_DMA_Start(void const * srcAddress, void const * dstAddress);
__STATIC_INLINE void SDIO_HOST_Read_DMA_Stop(void);
                void SDIO_HOST_Read_DMA_Init(void);
__STATIC_INLINE void SDIO_HOST_Read_DMA_ChannelEnable(void);
__STATIC_INLINE void SDIO_HOST_Read_DMA_ChannelDisable(void);

__STATIC_INLINE void SDIO_HOST_Read_DMA_SetDescriptor       (cy_stc_dma_descriptor_t const * descriptor);
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetNextDescriptor   (cy_stc_dma_descriptor_t const * descriptor);
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetPriority         (uint32_t priority);
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetSrcAddress       (cy_stc_dma_descriptor_t * descriptor, void const * srcAddress);
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetDstAddress       (cy_stc_dma_descriptor_t * descriptor, void const * dstAddress);
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetXloopDataCount   (cy_stc_dma_descriptor_t * descriptor, uint32_t xCount);
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetYloopDataCount   (cy_stc_dma_descriptor_t * descriptor, uint32_t yCount);
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetXloopSrcIncrement(cy_stc_dma_descriptor_t * descriptor, int32_t srcXincrement);
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetXloopDstIncrement(cy_stc_dma_descriptor_t * descriptor, int32_t dstXincrement);
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetYloopSrcIncrement(cy_stc_dma_descriptor_t * descriptor, int32_t srcYincrement);
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetYloopDstIncrement(cy_stc_dma_descriptor_t * descriptor, int32_t dstYincrement);
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetInterruptType    (cy_stc_dma_descriptor_t * descriptor, cy_en_dma_trigger_type_t interruptType);
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetTriggerInType    (cy_stc_dma_descriptor_t * descriptor, cy_en_dma_trigger_type_t triggerInType);
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetTriggerOutType   (cy_stc_dma_descriptor_t * descriptor, cy_en_dma_trigger_type_t triggerOutType);
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetDataSize         (cy_stc_dma_descriptor_t * descriptor, cy_en_dma_data_size_t dataSize);
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetSrcTransferSize  (cy_stc_dma_descriptor_t * descriptor, cy_en_dma_transfer_size_t srcTransferSize);
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetDstTransferSize  (cy_stc_dma_descriptor_t * descriptor, cy_en_dma_transfer_size_t dstTransferSize);
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetRetrigger        (cy_stc_dma_descriptor_t * descriptor, cy_en_dma_retrigger_t retrigger);
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetDescriptorType   (cy_stc_dma_descriptor_t * descriptor, cy_en_dma_descriptor_type_t descriptorType);
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetChannelState     (cy_stc_dma_descriptor_t * descriptor, cy_en_dma_channel_state_t channelState);

__STATIC_INLINE cy_stc_dma_descriptor_t *   SDIO_HOST_Read_DMA_GetDescriptor(void);
__STATIC_INLINE cy_stc_dma_descriptor_t *   SDIO_HOST_Read_DMA_GetNextDescriptor(void);
__STATIC_INLINE uint32_t                    SDIO_HOST_Read_DMA_GetPriority(void);
__STATIC_INLINE cy_en_dma_intr_cause_t      SDIO_HOST_Read_DMA_GetStatus(void);
__STATIC_INLINE cy_en_trigmux_status_t      SDIO_HOST_Read_DMA_Trigger(uint32_t cycles);
__STATIC_INLINE void *                      SDIO_HOST_Read_DMA_GetSrcAddress       (cy_stc_dma_descriptor_t const * descriptor);
__STATIC_INLINE void *                      SDIO_HOST_Read_DMA_GetDstAddress       (cy_stc_dma_descriptor_t const * descriptor);
__STATIC_INLINE uint32_t                    SDIO_HOST_Read_DMA_GetXloopDataCount   (cy_stc_dma_descriptor_t const * descriptor);
__STATIC_INLINE uint32_t                    SDIO_HOST_Read_DMA_GetYloopDataCount   (cy_stc_dma_descriptor_t const * descriptor);
__STATIC_INLINE int32_t                     SDIO_HOST_Read_DMA_GetXloopSrcIncrement(cy_stc_dma_descriptor_t const * descriptor);
__STATIC_INLINE int32_t                     SDIO_HOST_Read_DMA_GetXloopDstIncrement(cy_stc_dma_descriptor_t const * descriptor);
__STATIC_INLINE int32_t                     SDIO_HOST_Read_DMA_GetYloopSrcIncrement(cy_stc_dma_descriptor_t const * descriptor);
__STATIC_INLINE int32_t                     SDIO_HOST_Read_DMA_GetYloopDstIncrement(cy_stc_dma_descriptor_t const * descriptor);
__STATIC_INLINE cy_en_dma_trigger_type_t    SDIO_HOST_Read_DMA_GetInterruptType    (cy_stc_dma_descriptor_t const * descriptor);
__STATIC_INLINE cy_en_dma_trigger_type_t    SDIO_HOST_Read_DMA_GetTriggerInType    (cy_stc_dma_descriptor_t const * descriptor);
__STATIC_INLINE cy_en_dma_trigger_type_t    SDIO_HOST_Read_DMA_GetTriggerOutType   (cy_stc_dma_descriptor_t const * descriptor);
__STATIC_INLINE cy_en_dma_data_size_t       SDIO_HOST_Read_DMA_GetDataSize         (cy_stc_dma_descriptor_t const * descriptor);
__STATIC_INLINE cy_en_dma_transfer_size_t   SDIO_HOST_Read_DMA_GetSrcTransferSize  (cy_stc_dma_descriptor_t const * descriptor);
__STATIC_INLINE cy_en_dma_transfer_size_t   SDIO_HOST_Read_DMA_GetDstTransferSize  (cy_stc_dma_descriptor_t const * descriptor);
__STATIC_INLINE cy_en_dma_retrigger_t       SDIO_HOST_Read_DMA_GetRetrigger        (cy_stc_dma_descriptor_t const * descriptor);
__STATIC_INLINE cy_en_dma_descriptor_type_t SDIO_HOST_Read_DMA_GetDescriptorType   (cy_stc_dma_descriptor_t const * descriptor);
__STATIC_INLINE cy_en_dma_channel_state_t   SDIO_HOST_Read_DMA_GetChannelState     (cy_stc_dma_descriptor_t const * descriptor);

__STATIC_INLINE uint32_t SDIO_HOST_Read_DMA_GetInterruptStatus(void);
__STATIC_INLINE void     SDIO_HOST_Read_DMA_ClearInterrupt(void);
__STATIC_INLINE void     SDIO_HOST_Read_DMA_SetInterrupt(void);
__STATIC_INLINE uint32_t SDIO_HOST_Read_DMA_GetInterruptMask(void);
__STATIC_INLINE void     SDIO_HOST_Read_DMA_SetInterruptMask(uint32_t interrupt);
__STATIC_INLINE uint32_t SDIO_HOST_Read_DMA_GetInterruptStatusMasked(void);


/***************************************
*    Global Variables
***************************************/
extern uint8_t SDIO_HOST_Read_DMA_initVar;

extern cy_stc_dma_descriptor_config_t SDIO_HOST_Read_DMA_Read_DMA_Desc_config;
extern cy_stc_dma_descriptor_t SDIO_HOST_Read_DMA_Read_DMA_Desc;



/***************************************
*    In-line Function Implementation
***************************************/

/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_Stop
****************************************************************************//**
* Invokes the Cy_DMA_Channel_Disable() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void SDIO_HOST_Read_DMA_Stop(void)
{
    Cy_DMA_Channel_Disable(SDIO_HOST_Read_DMA_HW, SDIO_HOST_Read_DMA_DW_CHANNEL);
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_ChannelEnable
****************************************************************************//**
* Invokes the Cy_DMA_Channel_Enable() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void SDIO_HOST_Read_DMA_ChannelEnable(void)
{
    Cy_DMA_Channel_Enable(SDIO_HOST_Read_DMA_HW, SDIO_HOST_Read_DMA_DW_CHANNEL);
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_ChannelDisable
****************************************************************************//**
* Invokes the Cy_DMA_Channel_Disable() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void SDIO_HOST_Read_DMA_ChannelDisable(void)
{
    Cy_DMA_Channel_Disable(SDIO_HOST_Read_DMA_HW, SDIO_HOST_Read_DMA_DW_CHANNEL);
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_SetDescriptor
****************************************************************************//**
* Invokes the Cy_DMA_Channel_SetDescriptor() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetDescriptor(cy_stc_dma_descriptor_t const * descriptor)
{
    Cy_DMA_Channel_SetDescriptor(SDIO_HOST_Read_DMA_HW, SDIO_HOST_Read_DMA_DW_CHANNEL, descriptor);
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_GetDescriptor
****************************************************************************//**
* Invokes the Cy_DMA_Channel_GetCurrentDescriptor() PDL driver function.
*******************************************************************************/
__STATIC_INLINE cy_stc_dma_descriptor_t * SDIO_HOST_Read_DMA_GetDescriptor(void)
{
    return(Cy_DMA_Channel_GetCurrentDescriptor(SDIO_HOST_Read_DMA_HW, SDIO_HOST_Read_DMA_DW_CHANNEL));
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_SetNextDescriptor
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_SetNextDescriptor() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetNextDescriptor(cy_stc_dma_descriptor_t const * descriptor)
{
    Cy_DMA_Descriptor_SetNextDescriptor(Cy_DMA_Channel_GetCurrentDescriptor(SDIO_HOST_Read_DMA_HW, SDIO_HOST_Read_DMA_DW_CHANNEL), descriptor);
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_GetNextDescriptor
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_GetNextDescriptor() PDL driver function.
*******************************************************************************/
__STATIC_INLINE cy_stc_dma_descriptor_t * SDIO_HOST_Read_DMA_GetNextDescriptor(void)
{
    return(Cy_DMA_Descriptor_GetNextDescriptor(Cy_DMA_Channel_GetCurrentDescriptor(SDIO_HOST_Read_DMA_HW, SDIO_HOST_Read_DMA_DW_CHANNEL)));
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_SetPriority
****************************************************************************//**
* Invokes the Cy_DMA_Channel_SetPriority() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetPriority(uint32_t priority)
{
    Cy_DMA_Channel_SetPriority(SDIO_HOST_Read_DMA_HW, SDIO_HOST_Read_DMA_DW_CHANNEL, priority);
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_GetPriority
****************************************************************************//**
* Invokes the Cy_DMA_Channel_GetPriority() PDL driver function.
*******************************************************************************/
__STATIC_INLINE uint32_t SDIO_HOST_Read_DMA_GetPriority(void)
{
    return (Cy_DMA_Channel_GetPriority(SDIO_HOST_Read_DMA_HW, SDIO_HOST_Read_DMA_DW_CHANNEL));
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_SetSrcAddress
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_SetSrcAddress() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetSrcAddress(cy_stc_dma_descriptor_t * descriptor, void const * srcAddress)
{
    Cy_DMA_Descriptor_SetSrcAddress(descriptor, srcAddress);
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_GetSrcAddress
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_GetSrcAddress() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void * SDIO_HOST_Read_DMA_GetSrcAddress(cy_stc_dma_descriptor_t const * descriptor)
{
    return(Cy_DMA_Descriptor_GetSrcAddress(descriptor));
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_SetDstAddress
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_SetDstAddress() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetDstAddress(cy_stc_dma_descriptor_t * descriptor, void const * dstAddress)
{
    Cy_DMA_Descriptor_SetDstAddress(descriptor, dstAddress);
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_GetDstAddress
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_GetDestAddr() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void * SDIO_HOST_Read_DMA_GetDstAddress(cy_stc_dma_descriptor_t const * descriptor)
{
    return(Cy_DMA_Descriptor_GetDstAddress(descriptor));
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_SetDataSize
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_SetDataSize() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetDataSize(cy_stc_dma_descriptor_t * descriptor, cy_en_dma_data_size_t dataSize)
{
    Cy_DMA_Descriptor_SetDataSize(descriptor, dataSize);
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_GetDataSize
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_GetDataSize() PDL driver function.
*******************************************************************************/
__STATIC_INLINE cy_en_dma_data_size_t SDIO_HOST_Read_DMA_GetDataSize(cy_stc_dma_descriptor_t const * descriptor)
{
    return(Cy_DMA_Descriptor_GetDataSize(descriptor));
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_SetSrcTransferSize
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_SetSrcTransferSize() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetSrcTransferSize(cy_stc_dma_descriptor_t * descriptor, cy_en_dma_transfer_size_t srcTransferSize)
{
    Cy_DMA_Descriptor_SetSrcTransferSize(descriptor, srcTransferSize);
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_GetSrcTransferSize
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_GetSrcTransferSize() PDL driver function.
*******************************************************************************/
__STATIC_INLINE cy_en_dma_transfer_size_t SDIO_HOST_Read_DMA_GetSrcTransferSize(cy_stc_dma_descriptor_t const * descriptor)
{
    return(Cy_DMA_Descriptor_GetSrcTransferSize(descriptor));
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_SetDstTransferSize
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_SetDstTransferSize() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetDstTransferSize(cy_stc_dma_descriptor_t * descriptor, cy_en_dma_transfer_size_t dstTransferSize)
{
    Cy_DMA_Descriptor_SetDstTransferSize(descriptor, dstTransferSize);
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_SetRetrigger
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_SetRetrigger() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetRetrigger(cy_stc_dma_descriptor_t * descriptor, cy_en_dma_retrigger_t retrigger)
{
    Cy_DMA_Descriptor_SetRetrigger(descriptor, retrigger);
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_SetDescriptorType
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_SetDescriptorType() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetDescriptorType(cy_stc_dma_descriptor_t * descriptor, cy_en_dma_descriptor_type_t descriptorType)
{
    Cy_DMA_Descriptor_SetDescriptorType(descriptor, descriptorType);
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_SetChannelState
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_SetChannelState() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetChannelState(cy_stc_dma_descriptor_t * descriptor, cy_en_dma_channel_state_t channelState)
{
    Cy_DMA_Descriptor_SetChannelState(descriptor, channelState);
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_GetDstTransferSize
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_GetDstTransferSize() PDL driver function.
*******************************************************************************/
__STATIC_INLINE cy_en_dma_transfer_size_t SDIO_HOST_Read_DMA_GetDstTransferSize(cy_stc_dma_descriptor_t const * descriptor)
{
    return(Cy_DMA_Descriptor_GetDstTransferSize(descriptor));
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_GetRetrigger
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_GetRetrigger() PDL driver function.
*******************************************************************************/
__STATIC_INLINE cy_en_dma_retrigger_t SDIO_HOST_Read_DMA_GetRetrigger(cy_stc_dma_descriptor_t const * descriptor)
{
    return(Cy_DMA_Descriptor_GetRetrigger(descriptor));
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_GetDescriptorType
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_GetDescriptorType() PDL driver function.
*******************************************************************************/
__STATIC_INLINE cy_en_dma_descriptor_type_t SDIO_HOST_Read_DMA_GetDescriptorType(cy_stc_dma_descriptor_t const * descriptor)
{
    return(Cy_DMA_Descriptor_GetDescriptorType(descriptor));
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_GetChannelState
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_GetChannelState() PDL driver function.
*******************************************************************************/
__STATIC_INLINE cy_en_dma_channel_state_t SDIO_HOST_Read_DMA_GetChannelState(cy_stc_dma_descriptor_t const * descriptor)
{
    return(Cy_DMA_Descriptor_GetChannelState(descriptor));
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_SetXloopDataCount
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_SetXloopDataCount() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetXloopDataCount(cy_stc_dma_descriptor_t * descriptor, uint32_t xCount)
{
    Cy_DMA_Descriptor_SetXloopDataCount(descriptor, xCount);
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_GetXloopDataCount
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_GetXloopDataCount() PDL driver function.
*******************************************************************************/
__STATIC_INLINE uint32_t SDIO_HOST_Read_DMA_GetXloopDataCount(cy_stc_dma_descriptor_t const * descriptor)
{
    return (Cy_DMA_Descriptor_GetXloopDataCount(descriptor));
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_SetXloopSrcIncrement
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_SetXloopSrcIncrement() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetXloopSrcIncrement(cy_stc_dma_descriptor_t * descriptor, int32_t srcXincrement)
{
    Cy_DMA_Descriptor_SetXloopSrcIncrement(descriptor, srcXincrement);
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_GetXloopSrcIncrement
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_GetXloopSrcIncrement() PDL driver function.
*******************************************************************************/
__STATIC_INLINE int32_t SDIO_HOST_Read_DMA_GetXloopSrcIncrement(cy_stc_dma_descriptor_t const * descriptor)
{
    return (Cy_DMA_Descriptor_GetXloopSrcIncrement(descriptor));
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_SetXloopDstIncrement
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_SetXloopDstIncrement() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetXloopDstIncrement(cy_stc_dma_descriptor_t * descriptor, int32_t dstXincrement)
{
    Cy_DMA_Descriptor_SetXloopDstIncrement(descriptor, dstXincrement);
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_GetXloopDstIncrement
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_GetXloopDstIncrement() PDL driver function.
*******************************************************************************/
__STATIC_INLINE int32_t SDIO_HOST_Read_DMA_GetXloopDstIncrement(cy_stc_dma_descriptor_t const * descriptor)
{
    return (Cy_DMA_Descriptor_GetXloopDstIncrement(descriptor));
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_SetYloopDataCount
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_SetYloopDataCount() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetYloopDataCount(cy_stc_dma_descriptor_t * descriptor, uint32_t yCount)
{
    Cy_DMA_Descriptor_SetYloopDataCount(descriptor, yCount);
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_GetYloopDataCount
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_GetYloopDataCount() PDL driver function.
*******************************************************************************/
__STATIC_INLINE uint32_t SDIO_HOST_Read_DMA_GetYloopDataCount(cy_stc_dma_descriptor_t const * descriptor)
{
    return (Cy_DMA_Descriptor_GetYloopDataCount(descriptor));
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_SetYloopSrcIncrement
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_SetYloopSrcIncrement() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetYloopSrcIncrement(cy_stc_dma_descriptor_t * descriptor, int32_t srcYincrement)
{
    Cy_DMA_Descriptor_SetYloopSrcIncrement(descriptor, srcYincrement);
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_GetYloopSrcIncrement
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_GetYloopSrcIncrement() PDL driver function.
*******************************************************************************/
__STATIC_INLINE int32_t SDIO_HOST_Read_DMA_GetYloopSrcIncrement(cy_stc_dma_descriptor_t const * descriptor)
{
    return (Cy_DMA_Descriptor_GetYloopSrcIncrement(descriptor));
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_SetYloopDstIncrement
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_SetYloopDstIncrement() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetYloopDstIncrement(cy_stc_dma_descriptor_t * descriptor, int32_t dstYincrement)
{
    Cy_DMA_Descriptor_SetYloopDstIncrement(descriptor, dstYincrement);
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_SetInterruptType
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_SetInterruptType() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetInterruptType(cy_stc_dma_descriptor_t * descriptor, cy_en_dma_trigger_type_t interruptType)
{
    Cy_DMA_Descriptor_SetInterruptType(descriptor, interruptType);
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_SetTriggerInType
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_SetTriggerInType() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetTriggerInType(cy_stc_dma_descriptor_t * descriptor, cy_en_dma_trigger_type_t triggerInType)
{
    Cy_DMA_Descriptor_SetTriggerInType(descriptor, triggerInType);
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_SetTriggerOutType
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_SetTriggerOutType() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetTriggerOutType(cy_stc_dma_descriptor_t * descriptor, cy_en_dma_trigger_type_t triggerOutType)
{
    Cy_DMA_Descriptor_SetTriggerOutType(descriptor, triggerOutType);
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_GetYloopDstIncrement
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_GetYloopDstIncrement() PDL driver function.
*******************************************************************************/
__STATIC_INLINE int32_t SDIO_HOST_Read_DMA_GetYloopDstIncrement(cy_stc_dma_descriptor_t const * descriptor)
{
    return (Cy_DMA_Descriptor_GetYloopDstIncrement(descriptor));
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_GetInterruptType
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_GetInterruptType() PDL driver function.
*******************************************************************************/
__STATIC_INLINE cy_en_dma_trigger_type_t SDIO_HOST_Read_DMA_GetInterruptType(cy_stc_dma_descriptor_t const * descriptor)
{
    return (Cy_DMA_Descriptor_GetInterruptType(descriptor));
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_GetTriggerInType
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_GetTriggerInType() PDL driver function.
*******************************************************************************/
__STATIC_INLINE cy_en_dma_trigger_type_t SDIO_HOST_Read_DMA_GetTriggerInType(cy_stc_dma_descriptor_t const * descriptor)
{
    return (Cy_DMA_Descriptor_GetTriggerInType(descriptor));
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_GetTriggerOutType
****************************************************************************//**
* Invokes the Cy_DMA_Descriptor_GetTriggerOutType() PDL driver function.
*******************************************************************************/
__STATIC_INLINE cy_en_dma_trigger_type_t SDIO_HOST_Read_DMA_GetTriggerOutType(cy_stc_dma_descriptor_t const * descriptor)
{
    return (Cy_DMA_Descriptor_GetTriggerOutType(descriptor));
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_GetInterruptStatus()
****************************************************************************//**
* Invokes the Cy_DMA_Channel_GetInterruptStatus() PDL driver function.
*******************************************************************************/
__STATIC_INLINE uint32_t SDIO_HOST_Read_DMA_GetInterruptStatus(void)
{
    return (Cy_DMA_Channel_GetInterruptStatus(SDIO_HOST_Read_DMA_HW, SDIO_HOST_Read_DMA_DW_CHANNEL));
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_GetStatus
****************************************************************************//**
* Invokes the Cy_DMA_Channel_GetStatus() PDL driver function.
*******************************************************************************/
__STATIC_INLINE cy_en_dma_intr_cause_t SDIO_HOST_Read_DMA_GetStatus(void)
{
    return (Cy_DMA_Channel_GetStatus(SDIO_HOST_Read_DMA_HW, SDIO_HOST_Read_DMA_DW_CHANNEL));
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_ClearInterrupt
****************************************************************************//**
* Invokes the Cy_DMA_Channel_ClearInterrupt() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void SDIO_HOST_Read_DMA_ClearInterrupt(void)
{
    Cy_DMA_Channel_ClearInterrupt(SDIO_HOST_Read_DMA_HW, SDIO_HOST_Read_DMA_DW_CHANNEL);
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_SetInterrupt
****************************************************************************//**
* Invokes the Cy_DMA_Channel_SetInterrupt() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetInterrupt(void)
{
    Cy_DMA_Channel_SetInterrupt(SDIO_HOST_Read_DMA_HW, SDIO_HOST_Read_DMA_DW_CHANNEL);
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_GetInterruptMask
****************************************************************************//**
* Invokes the Cy_DMA_Channel_GetInterruptMask() PDL driver function.
*******************************************************************************/
__STATIC_INLINE uint32_t SDIO_HOST_Read_DMA_GetInterruptMask(void)
{
    return (Cy_DMA_Channel_GetInterruptMask(SDIO_HOST_Read_DMA_HW, SDIO_HOST_Read_DMA_DW_CHANNEL));
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_SetInterruptMask
****************************************************************************//**
* Invokes the Cy_DMA_Channel_SetInterruptMask() PDL driver function.
*******************************************************************************/
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetInterruptMask(uint32_t interrupt)
{
    Cy_DMA_Channel_SetInterruptMask(SDIO_HOST_Read_DMA_HW, SDIO_HOST_Read_DMA_DW_CHANNEL, interrupt);
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_GetInterruptStatusMasked
****************************************************************************//**
* Invokes the Cy_DMA_Channel_GetInterruptStatusMasked() PDL driver function.
*******************************************************************************/
__STATIC_INLINE uint32_t SDIO_HOST_Read_DMA_GetInterruptStatusMasked(void)
{
    return (Cy_DMA_Channel_GetInterruptStatusMasked(SDIO_HOST_Read_DMA_HW, SDIO_HOST_Read_DMA_DW_CHANNEL));
}


/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_Trigger
****************************************************************************//**
*
* Invokes the Cy_TrigMux_SwTrigger() PDL driver function.
*
* \param cycles
*  The number of "PeriClk" cycles during which the trigger remains activated.
*  The valid range of cycles is 1 ... 254.
*  Also there are special values:
*  CY_TRIGGER_INFINITE - trigger remains activated untill user deactivates it by
*  calling this function with CY_TRIGGER_DEACTIVATE parameter.
*  CY_TRIGGER_DEACTIVATE - it is used to deactivate the trigger activated by
*  calling this function with CY_TRIGGER_INFINITE parameter.
*
* \return A status:
* - CY_TRIGMUX_SUCCESS: The trigger is succesfully activated/deactivated.
* - CY_TRIGMUX_INVALID_STATE: The trigger is already activated/not active.
*
*******************************************************************************/
__STATIC_INLINE cy_en_trigmux_status_t SDIO_HOST_Read_DMA_Trigger(uint32_t cycles)
{
    return (Cy_TrigMux_SwTrigger((uint32_t)SDIO_HOST_Read_DMA_DW__TR_IN, cycles));
}


#if(CY_DMA_BWC)
/* Definitions to support backward compatibility with the component version 1.0,
*  they are strongly not recommended to use in new designs */

#define SDIO_HOST_Read_DMA_ChEnable                (SDIO_HOST_Read_DMA_ChannelEnable)
#define SDIO_HOST_Read_DMA_ChDisable               (SDIO_HOST_Read_DMA_ChannelDisable)
#define SDIO_HOST_Read_DMA_SetDataElementSize      (SDIO_HOST_Read_DMA_SetDataSize)
#define SDIO_HOST_Read_DMA_GetDataElementSize      (SDIO_HOST_Read_DMA_GetDataSize)
#define SDIO_HOST_Read_DMA_SetXloopNumDataElements (SDIO_HOST_Read_DMA_SetXloopDataCount)
#define SDIO_HOST_Read_DMA_GetXloopNumDataElements (SDIO_HOST_Read_DMA_GetXloopDataCount)
#define SDIO_HOST_Read_DMA_SetYloopNumDataElements (SDIO_HOST_Read_DMA_SetYloopDataCount)
#define SDIO_HOST_Read_DMA_GetYloopNumDataElements (SDIO_HOST_Read_DMA_GetYloopDataCount)
#define SDIO_HOST_Read_DMA_SetInterruptMask()      (SDIO_HOST_Read_DMA_SetInterruptMask(SDIO_HOST_Read_DMA_INTR_MASK))
#define SDIO_HOST_Read_DMA_GetInterruptCause       (SDIO_HOST_Read_DMA_GetStatus)

#define SDIO_HOST_Read_DMA_PREEMTAMBLE             (SDIO_HOST_Read_DMA_PREEMPTABLE)
/*******************************************************************************
* Function Name: SDIO_HOST_Read_DMA_SetSrcDstTransferWidth
****************************************************************************//**
* This is a legacy API function, it is left here just for backward compatibility
*******************************************************************************/
__STATIC_INLINE void SDIO_HOST_Read_DMA_SetSrcDstTransferWidth(cy_stc_dma_descriptor_t * descriptor, uint32_t options)
{
    uint32_t ctlRegVal = descriptor->ctl & ((uint32_t)~(DW_DESCR_STRUCT_DESCR_CTL_SRC_TRANSFER_SIZE_Msk | \
            DW_DESCR_STRUCT_DESCR_CTL_DST_TRANSFER_SIZE_Msk));

    descriptor->ctl = ctlRegVal |
        ((options << DW_DESCR_STRUCT_DESCR_CTL_SRC_TRANSFER_SIZE_Pos) &
        (DW_DESCR_STRUCT_DESCR_CTL_SRC_TRANSFER_SIZE_Msk | DW_DESCR_STRUCT_DESCR_CTL_DST_TRANSFER_SIZE_Msk));
}


/*******************************************************************************
* Function Name: DMA_1_GetSrcDstTransferWidth
****************************************************************************//**
* This is a legacy API function, it is left here just for backward compatibility
*******************************************************************************/
__STATIC_INLINE uint32_t SDIO_HOST_Read_DMA_GetSrcDstTransferWidth(cy_stc_dma_descriptor_t const * descriptor)
{
    uint32_t ctlRegVal = descriptor->ctl & (DW_DESCR_STRUCT_DESCR_CTL_SRC_TRANSFER_SIZE_Msk |
        DW_DESCR_STRUCT_DESCR_CTL_DST_TRANSFER_SIZE_Msk);

    return(ctlRegVal >> DW_DESCR_STRUCT_DESCR_CTL_SRC_TRANSFER_SIZE_Pos);
}

#endif /* CY_DMA_BWC */


#if defined(__cplusplus)
}
#endif

#endif/* (SDIO_HOST_Read_DMA_CH_H) */

/* [] END OF FILE */
