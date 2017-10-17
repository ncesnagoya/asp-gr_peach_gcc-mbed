/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
* Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**************************************************************************//**
* @file         dma_ver.c
* $Rev: 1645 $
* $Date:: 2015-05-21 10:35:06 +0900#$
* @brief        DMA Driver get verion function
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "cmsis_os.h"
#include "dma_if.h"

/******************************************************************************
Macro definitions
******************************************************************************/

/* Ex. V2.12 -> MAJOR=2, MINOR=12 */
#define DMA_DRV_VER_MAJOR  (2u)
#define DMA_DRV_VER_MINOR  (2u)

#define DMA_DRV_VER_MASK   (0xFFu)
#define DMA_DRV_VER_SHIFT  (8u)

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/

/**************************************************************************//**
* Function Name: R_DMA_GetVersion
* @brief         Get DMA driver version.
*
*                Description:<br>
*                
* @param         none
* @retval        driver version -
*                    upper 8bit=MAJOR
*                    lower 8bit=MINOR
******************************************************************************/
uint16_t R_DMA_GetVersion(void)
{
    const uint16_t version =
        ((DMA_DRV_VER_MAJOR & DMA_DRV_VER_MASK) << DMA_DRV_VER_SHIFT)
        | (DMA_DRV_VER_MINOR & DMA_DRV_VER_MASK);

    return version;
}
/******************************************************************************
End of function R_DMA_GetVersion
******************************************************************************/
