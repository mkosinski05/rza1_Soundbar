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
*
* Copyright (C) 2017 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name     : r_boot_config.h
* Device(s)     : RZ/A1L
* Tool-Chain    : GNUARM-NONE-EABI-v16.01
* H/W Platform  : Platform Independent
* Description   : Configuration File for QSPI_Loader Application
******************************************************************************
*******************************************************************************
* History       : DD.MM.YYYY Version Description
*               : 26.01.2017 1.00    Ported from RZA1H
*******************************************************************************/

/* Multiple inclusion prevention macro */
#ifndef R_BOOT_CONFIG_H
#define R_BOOT_CONFIG_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
/******************************************************************************
 Macro definitions
 ******************************************************************************/
#define SPIBSC_CH                   (0)

/* Execution point user's application */
#define DEF_USER_PROGRAM_SRC        (0x18080000)

#define DEF_DEVICE_VECTOR_TBL_SIZE  (0x20)

#define DEF_USER_PROGRAM_SIGNATURE (".BootLoad_ValidProgramTest.")

/* Address range check defines for user's application */
/* QSPI Loader is copied to BOOT_RAM, which is specified in the linker */
#define DEF_INTERNAL_RAM_NORM           (0x20020000)
#define DEF_INTERNAL_RAM_MIRROR_OFFSET  (0x40000000)
#define DEF_INTERNAL_RAM_MIRROR        	(DEF_INTERNAL_RAM_NORM + DEF_INTERNAL_RAM_MIRROR_OFFSET)

#define DEF_USER_CODE_START            ((uint32_t*)(DEF_USER_PROGRAM_SRC + DEF_DEVICE_VECTOR_TBL_SIZE + 0x00))
#define DEF_USER_CODE_END              ((uint32_t*)(DEF_USER_PROGRAM_SRC + DEF_DEVICE_VECTOR_TBL_SIZE + 0x04))
#define DEF_USER_CODE_EXECUTE          ((uint32_t*)(DEF_USER_PROGRAM_SRC + DEF_DEVICE_VECTOR_TBL_SIZE + 0x08))
#define DEF_USER_SIGNATURE             ((uint32_t*)(DEF_USER_PROGRAM_SRC + DEF_DEVICE_VECTOR_TBL_SIZE + 0x0c))

/* Note that these timing values are approximate */
/* The optimiser does strange things with the result that */
/* time delays are not consistent between function calls */
/* For accurate delays a hardware timer should be used */
#define DELAY_250MS                    (1750000u)
#define DELAY_500MS                    (DELAY_250MS * 2U)
#define DELAY_2000MS                   (DELAY_500MS * 4U)

#define DEF_SPIBSC_DUAL_MODE           (SPIBSC_CMNCR_BSZ_DUAL)

extern void UserProgJmp(uint32_t addr);

/* R_BOOT_CONFIG_H */
#endif

/* End of File */
