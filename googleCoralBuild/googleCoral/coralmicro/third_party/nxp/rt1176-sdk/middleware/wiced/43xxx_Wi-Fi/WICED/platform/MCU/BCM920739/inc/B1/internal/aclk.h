/*
********************************************************************
* THIS INFORMATION IS PROPRIETARY TO
* BROADCOM CORP.
*-------------------------------------------------------------------
*                                                                        
*           Copyright (c) 2011 Broadcom Corp.
*                      ALL RIGHTS RESERVED                              
*                                                                       
********************************************************************

********************************************************************
*    File Name: aclk.h
*
*    This file defines an aclk driver
*
********************************************************************
*/

#ifndef __ACLK_H__
#define __ACLK_H__

#include "brcm_fw_types.h"
/** \addtogroup  ACLK
* \ingroup HardwareDrivers
*/
/*! @{ */
/**
* Defines an Aclk driver.
*/
#if defined BCM20739
enum
{
    HW_MIA_ACLK_CTL_ACLK0_SHIFT                     = 0,
    HW_MIA_ACLK_CTL_ACLK0_DIV_MASK                  = 0x00000FFF,
    HW_MIA_ACLK_CTL_ACLK0_DIV_SHIFT                 = 0,
    HW_MIA_ACLK_CTL_ACLK0_POST_DIV_MASK             = 0x00007000,
    HW_MIA_ACLK_CTL_ACLK0_POST_DIV_SHIFT            = 12,
    HW_MIA_ACLK_CTL_ACLK0_ENABLE_MASK               = 0x00008000,
    HW_MIA_ACLK_CTL_ACLK0_ENABLE                    = 0x00008000,
    HW_MIA_ACLK_CTL_ACLK0_DISABLE                   = 0x00000000,
    HW_MIA_ACLK_CTL_ACLK0_CLK_SRC_SEL_MASK          = 0x00010000,
    HW_MIA_ACLK_CTL_ACLK0_CLK_SRC_24_MHZ            = 0x00010000,
    HW_MIA_ACLK_CTL_ACLK0_CLK_SRC_1_MHZ             = 0x00000000,
    HW_MIA_ACLK_CTL_ACLK0_MASK_ALL  = (HW_MIA_ACLK_CTL_ACLK0_DIV_MASK
                                       | HW_MIA_ACLK_CTL_ACLK0_POST_DIV_MASK
                                       | HW_MIA_ACLK_CTL_ACLK0_ENABLE_MASK
                                       | HW_MIA_ACLK_CTL_ACLK0_CLK_SRC_SEL_MASK),
    
    HW_MIA_ACLK_CTL_ACLK1_SHIFT                     = 0,          
    HW_MIA_ACLK_CTL_ACLK1_DIV_MASK                  = 0x00000FFF, 
    HW_MIA_ACLK_CTL_ACLK1_DIV_SHIFT                 = 0,          
    HW_MIA_ACLK_CTL_ACLK1_POST_DIV_MASK             = 0x00007000, 
    HW_MIA_ACLK_CTL_ACLK1_POST_DIV_SHIFT            = 12,         
    HW_MIA_ACLK_CTL_ACLK1_ENABLE_MASK               = 0x00008000, 
    HW_MIA_ACLK_CTL_ACLK1_ENABLE                    = 0x00008000, 
    HW_MIA_ACLK_CTL_ACLK1_DISABLE                   = 0x00000000, 
    HW_MIA_ACLK_CTL_ACLK1_CLK_SRC_SEL_MASK          = 0x00010000, 
    HW_MIA_ACLK_CTL_ACLK1_CLK_SRC_24_MHZ            = 0x00010000, 
    HW_MIA_ACLK_CTL_ACLK1_CLK_SRC_1_MHZ             = 0x00000000, 
    HW_MIA_ACLK_CTL_ACLK1_MASK_ALL = (HW_MIA_ACLK_CTL_ACLK1_DIV_MASK
                                      | HW_MIA_ACLK_CTL_ACLK1_POST_DIV_MASK
                                      | HW_MIA_ACLK_CTL_ACLK1_ENABLE_MASK
                                      | HW_MIA_ACLK_CTL_ACLK1_CLK_SRC_SEL_MASK),
};
#else
enum
{
    HW_MIA_ACLK_CTL_ACLK0_SHIFT                     = 0,
    HW_MIA_ACLK_CTL_ACLK0_DIV_MASK                  = 0x000000FF,
    HW_MIA_ACLK_CTL_ACLK0_DIV_SHIFT                 = 0,
    HW_MIA_ACLK_CTL_ACLK0_POST_DIV_MASK             = 0x00000700,
    HW_MIA_ACLK_CTL_ACLK0_POST_DIV_SHIFT            = 8,
    HW_MIA_ACLK_CTL_ACLK0_ENABLE_MASK               = 0x00001000,
    HW_MIA_ACLK_CTL_ACLK0_ENABLE                    = 0x00001000,
    HW_MIA_ACLK_CTL_ACLK0_DISABLE                   = 0x00000000,
    HW_MIA_ACLK_CTL_ACLK0_CLK_SRC_SEL_MASK          = 0x00008000,
    HW_MIA_ACLK_CTL_ACLK0_CLK_SRC_24_MHZ            = 0x00008000,
    HW_MIA_ACLK_CTL_ACLK0_CLK_SRC_1_MHZ             = 0x00000000,
    HW_MIA_ACLK_CTL_ACLK0_MASK_ALL  = (HW_MIA_ACLK_CTL_ACLK0_DIV_MASK
                                       | HW_MIA_ACLK_CTL_ACLK0_POST_DIV_MASK
                                       | HW_MIA_ACLK_CTL_ACLK0_ENABLE_MASK
                                       | HW_MIA_ACLK_CTL_ACLK0_CLK_SRC_SEL_MASK),
    
    HW_MIA_ACLK_CTL_ACLK1_SHIFT                     = 16,
    HW_MIA_ACLK_CTL_ACLK1_DIV_MASK                  = 0x00FF0000,
    HW_MIA_ACLK_CTL_ACLK1_DIV_SHIFT                 = 16,
    HW_MIA_ACLK_CTL_ACLK1_POST_DIV_MASK             = 0x07000000,
    HW_MIA_ACLK_CTL_ACLK1_POST_DIV_SHIFT            = 24,
    HW_MIA_ACLK_CTL_ACLK1_ENABLE_MASK               = 0x10000000,
    HW_MIA_ACLK_CTL_ACLK1_ENABLE                    = 0x10000000,
    HW_MIA_ACLK_CTL_ACLK1_DISABLE                   = 0x00000000,
    HW_MIA_ACLK_CTL_ACLK1_CLK_SRC_SEL_MASK          = (int)0x80000000,
    HW_MIA_ACLK_CTL_ACLK1_CLK_SRC_24_MHZ            = (int)0x80000000,
    HW_MIA_ACLK_CTL_ACLK1_CLK_SRC_1_MHZ             = 0x00000000,
    HW_MIA_ACLK_CTL_ACLK1_MASK_ALL = (HW_MIA_ACLK_CTL_ACLK1_DIV_MASK
                                      | HW_MIA_ACLK_CTL_ACLK1_POST_DIV_MASK
                                      | HW_MIA_ACLK_CTL_ACLK1_ENABLE_MASK
                                      | HW_MIA_ACLK_CTL_ACLK1_CLK_SRC_SEL_MASK),
};
#endif
enum CLK_SRC_SEL
{
    ACLK0,
    ACLK1,
};

enum CLK_SRC_FREQ_SEL
{
    ACLK_FREQ_1_MHZ,
    ACLK_FREQ_24_MHZ
};

#ifdef __cplusplus
extern "C" {
#endif

void aclk_configure(UINT32 frequency, UINT32 src, UINT32 freqSel);
void aclk_disableClock(UINT32 src);
void aclk_updateClkGating(void);

#ifdef __cplusplus
}
#endif

/* @}  */
#endif
