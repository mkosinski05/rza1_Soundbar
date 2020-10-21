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
 * File Name     : spibsc_userdef.h
 * Version       : 1.00
 * Device(s)     : RZ/A1L
 * Tool-Chain    : GNUARM-NONE-EABI-v16.01
 * OS            : None
 * H/W Platform  : Stream it! v2
 * Description   : SPI flash device support definition file
 *                 select USE_MACRONIX to support MACRONIX device(s)
 *                 select USE_SPANSION to support SPANSION device(s)
 *******************************************************************************/
/*******************************************************************************
 * History       : DD.MM.YYYY Version Description
 *               : 24.01.17   1.00    First Release
 *******************************************************************************/

#ifndef R_SPIBSC_USERDEF_H
#define R_SPIBSC_USERDEF_H

/* Choose one out of the two         */
#define USE_MACRONIX
/* #define USE_SPANSION */

#undef QSPI_DEVICE_CHOSEN

#ifdef USE_SPANSION
    /* QSPI_QUAD_MODE_SUPPORT not supported when
     * specifying  SDR_SINGLE_4B
     */
    #define SPIBSC_MODE (SDR_SINGLE_4B)
    #define QSPI_DEVICE_CHOSEN
    #undef QSPI_QUAD_MODE_SUPPORT
#endif

#ifdef USE_MACRONIX
    #define QSPI_DEVICE_CHOSEN
    #define QSPI_QUAD_MODE_SUPPORT

/* Choose one out of the four
 * (only for Macronix,
 * Spansion are in SDR_SINGLE_4B)
 */
    /* #define SPIBSC_MODE (SDR_SINGLE_4B) */
    /* #define SPIBSC_MODE (DDR_SINGLE_4B) */
    /* #define SPIBSC_MODE (SDR_DUAL_4B) */
    #define SPIBSC_MODE (DDR_DUAL_4B)
#endif


#ifndef QSPI_DEVICE_CHOSEN
#error "QSPI device configuration MUST BE selected"
#endif

/* R_SPIBSC_USERDEF_H */
#endif
