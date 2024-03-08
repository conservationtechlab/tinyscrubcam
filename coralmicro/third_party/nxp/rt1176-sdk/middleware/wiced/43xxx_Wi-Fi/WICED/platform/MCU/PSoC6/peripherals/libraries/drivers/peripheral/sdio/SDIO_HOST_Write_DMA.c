/*******************************************************************************
* File Name: SDIO_HOST_Write_DMA.c
* Version 2.0
*
* Description:
*  This file provides the source code to the API for the
*  SDIO_HOST_Write_DMA component.
*
********************************************************************************
* Copyright 2016-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "SDIO_HOST_Write_DMA.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* Generated code */
cy_stc_dma_descriptor_config_t SDIO_HOST_Write_DMA_Write_DMA_Desc_config =
{
    .retrigger       = CY_DMA_RETRIG_4CYC,
    .interruptType   = CY_DMA_DESCR,
    .triggerOutType  = CY_DMA_1ELEMENT,
    .channelState    = CY_DMA_CHANNEL_DISABLED,
    .triggerInType   = CY_DMA_X_LOOP,
    .dataSize        = CY_DMA_HALFWORD,
    .srcTransferSize = CY_DMA_TRANSFER_SIZE_DATA,
    .dstTransferSize = CY_DMA_TRANSFER_SIZE_DATA,
    .descriptorType  = CY_DMA_2D_TRANSFER,
    .srcAddress      = NULL,
    .dstAddress      = NULL,
    .srcXincrement   = 2L,
    .dstXincrement   = 0L,
    .xCount          = 10UL,
    .srcYincrement   = 10L,
    .dstYincrement   = 0L,
    .yCount          = 2UL,
    .nextDescriptor  = NULL
};

cy_stc_dma_descriptor_t SDIO_HOST_Write_DMA_Write_DMA_Desc =
{
    .ctl = 0UL,
    .src = 0UL,
    .dst = 0UL,
    .xCtl = 0UL,
    .yCtl = 0UL,
    .nextPtr = 0UL
};


/** SDIO_HOST_Write_DMA_initVar indicates whether the SDIO_HOST_Write_DMA 
*  component has been initialized. The variable is initialized to 0 
*  and set to 1 the first time SDIO_HOST_Write_DMA_Start() is called. This allows 
*  the component to restart without reinitialization after the first 
*  call to the SDIO_HOST_Write_DMA_Start() routine.
*
*  If re-initialization of the component is required, then the 
*  SDIO_HOST_Write_DMA_Init() function can be called before the 
*  SDIO_HOST_Write_DMA_Start() or SDIO_HOST_Write_DMA_ChEnable() function.
*/
uint8 SDIO_HOST_Write_DMA_initVar = 0u;


/*******************************************************************************
* Function Name: SDIO_HOST_Write_DMA_Start
****************************************************************************//**
*
* Based on the settings for descriptor in the customizer this function runs the
* DMA_Descriptor_Init() and then initializes the channel using
* DMA_Chnl_Init(). Enables the SDIO_HOST_Write_DMA block using the DMA_Chnl_Enable().
*  
*******************************************************************************/
void SDIO_HOST_Write_DMA_Start(void const * srcAddress, void const * dstAddress)
{
    if (0U == SDIO_HOST_Write_DMA_initVar)
    {
        SDIO_HOST_Write_DMA_Init();
        SDIO_HOST_Write_DMA_initVar = 1u;
    }
    
    Cy_DMA_Descriptor_SetSrcAddress(&SDIO_HOST_Write_DMA_Write_DMA_Desc, srcAddress);
    Cy_DMA_Descriptor_SetDstAddress(&SDIO_HOST_Write_DMA_Write_DMA_Desc, dstAddress);
    Cy_DMA_Channel_Enable(SDIO_HOST_Write_DMA_HW, SDIO_HOST_Write_DMA_DW_CHANNEL);
}


/*******************************************************************************
* Function Name: SDIO_HOST_Write_DMA_Init
****************************************************************************//**
*
* Based on the settings for descriptor in the customizer this function runs the
* DMA_Descriptor_Init() and then initializes the channel using
* DMA_Chnl_Init().
*  
*******************************************************************************/
void SDIO_HOST_Write_DMA_Init(void)
{
    cy_stc_dma_channel_config_t channelConfig;

    /* Init all descriptors */
    (void)Cy_DMA_Descriptor_Init(&SDIO_HOST_Write_DMA_Write_DMA_Desc, &SDIO_HOST_Write_DMA_Write_DMA_Desc_config);


    channelConfig.descriptor  = &SDIO_HOST_Write_DMA_Write_DMA_Desc;
    channelConfig.preemptable = SDIO_HOST_Write_DMA_PREEMPTABLE;
    channelConfig.priority    = SDIO_HOST_Write_DMA_PRIORITY;
    channelConfig.enable      = false;

    (void)Cy_DMA_Channel_Init(SDIO_HOST_Write_DMA_HW, SDIO_HOST_Write_DMA_DW_CHANNEL, &channelConfig);

    Cy_DMA_Enable(SDIO_HOST_Write_DMA_HW);
}


#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */
