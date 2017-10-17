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
* Copyright (C) 2013-2014 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/

/******************************************************************************
* File Name    : ssif_int.h
* $Rev: 891 $
* $Date:: 2014-06-27 10:40:52 +0900#$
* Description  : SSIF driver interrupt functions header
******************************************************************************/

#ifndef SSIF_INT_H
#define SSIF_INT_H

/*******************************************************************************
Includes <System Includes>, "Project Includes"
*******************************************************************************/
#include "r_typedefs.h"

/******************************************************************************
Macro definitions
******************************************************************************/


/*************************************************************************
 Enumerated Types
*************************************************************************/

/*************************************************************************
 Structures
*************************************************************************/

/******************************************************************************
 Function Prototypes
 *****************************************************************************/
void SSIF_InterruptInit(const uint32_t ssif_ch, const uint8_t int_level);
void SSIF_InterruptShutdown(const uint32_t ssif_ch);

void SSIF_EnableErrorInterrupt(const uint32_t ssif_ch);
void SSIF_DisableErrorInterrupt(const uint32_t ssif_ch);

#endif /* SSIF_INT_H */
