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
* Copyright (C) 2017 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
 * File Name     : spibsc_config.h
 * Version       : 1.00
 * Device(s)     : RZ/A1L
 * Tool-Chain    : GNUARM-NONE-EABI-v16.01
 * OS            : None
 * H/W Platform  : Stream it! v2
 * Description   : SPI flash device and data SPI mode select
 *
 *******************************************************************************/
/*******************************************************************************
 * History       : DD.MM.YYYY Version Description
 *               : 24.01.17   1.00    First Release
 *******************************************************************************/

#ifndef R_SPIBSC_CONFIG_H
#define R_SPIBSC_CONFIG_H

#include "spibsc_userdef.h"

/* The following should not be changed  */
#define SDR_SINGLE_4B   (1) /* single device, single bit serial input and output */
#define DDR_SINGLE_4B   (2) /* dual device, single bit serial input and output */
#define SDR_DUAL_4B     (3) /* single device, dual bit serial input and output */
#define DDR_DUAL_4B     (4) /* dual device, dual bit serial input and output */

#ifdef USE_MACRONIX
    #if SPIBSC_MODE == SDR_SINGLE_4B
        #define SPIBSC_BUS_WITDH         (4)
        #define SPIBSC_OUTPUT_ADDR       (SPIBSC_OUTPUT_ADDR_24)
        #define SPIBSC_BUS_MODE          (SPIBSC_SDR_TRANS)
        #define SPIBSC_NB_FLASH          (SPIBSC_CMNCR_BSZ_SINGLE)
        #define SPIBSC_MODE_SDR          (1)
    #endif

    #if SPIBSC_MODE == DDR_SINGLE_4B
        #define SPIBSC_BUS_WITDH         (4)
        #define SPIBSC_OUTPUT_ADDR       (SPIBSC_OUTPUT_ADDR_24)
        #define SPIBSC_BUS_MODE          (SPIBSC_DDR_TRANS)
        #define SPIBSC_NB_FLASH          (SPIBSC_CMNCR_BSZ_SINGLE)
        #define SPIBSC_MODE_SDR          (1)
    #endif

    #if SPIBSC_MODE == SDR_DUAL_4B
        #define SPIBSC_BUS_WITDH         (4)
        #define SPIBSC_OUTPUT_ADDR       (SPIBSC_OUTPUT_ADDR_24)
        #define SPIBSC_BUS_MODE          (SPIBSC_SDR_TRANS)
        #define SPIBSC_NB_FLASH          (SPIBSC_CMNCR_BSZ_DUAL)
    #endif

    #if SPIBSC_MODE == DDR_DUAL_4B
        #define SPIBSC_BUS_WITDH         (4)
        #define SPIBSC_OUTPUT_ADDR       (SPIBSC_OUTPUT_ADDR_24)
        #define SPIBSC_BUS_MODE          (SPIBSC_DDR_TRANS)
        #define SPIBSC_NB_FLASH          (SPIBSC_CMNCR_BSZ_DUAL)
    #endif

    #ifdef SPIBSC_MODE_SDR
    /* Set CKDLY & SPODLY for single bit serial */
    /* ---- AC Input Characteristics Adjustment (CKDLY) ---- */
    #define SPIBSC_CKDLY_SETTING        (SPIBSC_CKDLY_DEFAULT)

    /* ---- AC Output Characteristics Adjustment (SPODLY) ---- */
    #define SPIBSC_SPODLY_SETTING       (SPIBSC_SPODLY_DEFAULT)

    #else
    /* Set CKDLY & SPODLY for dual bit serial */
    /* ---- AC Input Characteristics Adjustment (CKDLY) ---- */
    #define SPIBSC_CKDLY_SETTING        (SPIBSC_CKDLY_TUNING)

    /* ---- AC Output Characteristics Adjustment (SPODLY) ---- */
    #define SPIBSC_SPODLY_SETTING       (SPIBSC_SPODLY_TUNING)

    #endif


#endif

#ifdef USE_SPANSION
    #define SPIBSC_BUS_WITDH         (4)
    #define SPIBSC_OUTPUT_ADDR       (SPIBSC_OUTPUT_ADDR_32)
    #define SPIBSC_BUS_MODE          (SPIBSC_SDR_TRANS)
    #define SPIBSC_NB_FLASH          (SPIBSC_CMNCR_BSZ_SINGLE)
#endif

#endif /* R_SPIBSC_CONFIG_H */

