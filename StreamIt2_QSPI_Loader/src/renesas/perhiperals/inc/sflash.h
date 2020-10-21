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
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/******************************************************************************
* File Name    : sflash.h
* $Rev: 6130 $
* $Date:: 2018-01-09 13:59:27 +0000#$
* Description  : 
******************************************************************************/
#ifndef R_SFLASH_H
#define R_SFLASH_H

#include "spibsc_config.h"

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/


/******************************************************************************
Typedef definitions
******************************************************************************/


/******************************************************************************
Macro definitions
******************************************************************************/
/* ---- serial flash command[S25FL512 Spansion)(uniform=256KB)] ---- */

#ifdef USE_SPANSION
#define SFLASHCMD_CHIP_ERASE    (0xC7)          /* Bulk Erase (alternate command) */
#define SFLASHCMD_SECTOR_ERASE  (0xD8)          /* Erase 256 kB (3- or 4-byte address) */
#define SFLASHCMD_BYTE_PROGRAM  (0x02)          /* Page Program (3- or 4-byte address) */
#define SFLASHCMD_BYTE_READ     (0x0B)          /* Fast Read (3- or 4-byte address) */
#define SFLASHCMD_DUAL_READ     (0x3B)          /* Read Dual Out (3- or 4-byte address) */
#define SFLASHCMD_QUAD_READ     (0x6B)          /* Read Quad Out (3- or 4-byte address) */
#define SFLASHCMD_DUAL_IO_READ  (0xBB)          /* Dual I/O Read (3- or 4-byte address) */
#define SFLASHCMD_QUAD_IO_READ  (0xEB)          /* Quad I/O Read (3- or 4-byte address) */
#define SFLASHCMD_WRITE_ENABLE  (0x06)          /* Write Enable */
#define SFLASHCMD_READ_STATUS   (0x05)          /* Read Status Register-1 */
#define SFLASHCMD_READ_CONFIG   (0x35)          /* Read Configuration Register-1 */
#define SFLASHCMD_WRITE_STATUS  (0x01)          /* Write Register (Status-1, Configuration-1) */
#define SFLASHCMD_QUAD_PROGRAM  (0x32)          /* Quad Page Program (3- or 4-byte address) */
#define SFLASHCMD_READ_BANK     (0x16)          /* Bank Register Read */
#define SFLASHCMD_WRITE_BANK    (0x17)          /* Bank Register Write */
#define SFLASHCMD_READ_AUTOBOOT (0x14)          /* AutoBoot Register Read */
#define SFLASHCMD_CLEAR_STATUS  (0x30)          /* Clear Status Register-1 - Erase/Prog. Fail Reset */

/* 4-byte address command*/
#define SFLASHCMD_BYTE_READ_4B     (0x0C)       /* Fast Read (4-byte address) */
#define SFLASHCMD_QUAD_IO_READ_4B  (0xEC)       /* Quad I/O Read (4-byte address) */
#define SFLASHCMD_QUAD_READ_4B     (0x6C)       /* Read Quad Out (4-byte address) */
#define SFLASHCMD_BYTE_PROGRAM_4B  (0x12)       /* Page Program (4-byte address) */
#define SFLASHCMD_QUAD_PROGRAM_4B  (0x34)       /* Page Program (4-byte address) */
#define SFLASHCMD_SECTOR_ERASE_4B  (0xDC)       /* Page Program (4-byte address) */


/* ---- serial flash register definitions ---- */
#define STREG_QUAD_BIT			(0x40)			/* Quad mode bit(Status register)RW?!	*/

#define CFREG_QUAD_BIT          (0x02)          /* Quad mode bit(Configuration Register) */
#define CFREG_FREEZE_BIT        (0x01)          /* freeze bit(Configuration Register) */
#define STREG_BPROTECT_BIT      (0x1c)          /* protect bit(Status Register) */
#define STREG_SRWD_BIT          (0x80)          /* Status Register Write Disable(Status Register) */

#define CFREG_LC_BIT            (0xc0)          /* Latency Code bit(Configuration Register) */

#define SET_EXTADD_4BYTE        (0x80)          /* Extended Address Enable bit(Bank Address Register) */

/* Page and Sector size */
#define SF_PAGE_SIZE            (512)           /* Page size of serial flash memory */
#define SF_SECTOR_SIZE          (256 * 1024)    /* Sector size = 256 KB   */
#define SF_NUM_OF_SECTOR        (256)           /* Sector Count : 256 */
#endif

#ifdef USE_MACRONIX
#define SFLASHCMD_CHIP_ERASE    (0xC7)          /* erase whole chip */
#define SFLASHCMD_SECTOR_ERASE  (0x20)          /* erase the selected sector */
#define SFLASHCMD_BYTE_PROGRAM  (0x02)          /* program the selected page */
#define SFLASHCMD_BYTE_READ     (0x0B)          /* n bytes read out until CS# goes high */
#define SFLASHCMD_DUAL_READ     (0x3B)          /* n bytes read out by Dual output until CS# goes high */
#define SFLASHCMD_QUAD_READ     (0x6B)          /* n bytes read out by Quad output until CS# goes high */
#define SFLASHCMD_DUAL_IO_READ  (0xBB)          /* n bytes read out by 2 x I/O until CS# goes high */
#define SFLASHCMD_QUAD_IO_READ  (0xEB)          /* Quad I/O read for bottom 128Mb with 6 dummy cycles */
#define SFLASHCMD_WRITE_ENABLE  (0x06)          /* sets the (WEL) write enable latch bit */
#define SFLASHCMD_READ_STATUS   (0x05)          /* read out the values of the status register */
#define SFLASHCMD_READ_CONFIG   (0x15)          /* read out the values of the configuration register */
#define SFLASHCMD_WRITE_STATUS  (0x01)          /* write new values of the status/configuration register */
#define SFLASHCMD_QUAD_PROGRAM  (0x38)          /* quad input to program the selected page */
#define SFLASHCMD_READ_BANK     (0x00)          /*  */
#define SFLASHCMD_WRITE_BANK    (0x00)          /*  */
#define SFLASHCMD_READ_AUTOBOOT (0x00)          /*  */
#define SFLASHCMD_CLEAR_STATUS  (0x00)          /*  */

/* 4-byte address command*/
#define SFLASHCMD_BYTE_READ_4B     (0x0C)       /* read data byte by 4 byte address */
#define SFLASHCMD_QUAD_IO_READ_4B  (0xEC)       /* read data byte by 4 x I/O with 4 byte address */
#define SFLASHCMD_QUAD_READ_4B     (0x6C)       /* read data byte by Quad Output with 4 byte address */
#define SFLASHCMD_BYTE_PROGRAM_4B  (0x12)       /* program the selected page with 4byte */
#define SFLASHCMD_QUAD_PROGRAM_4B  (0x3E)       /* Quad input to program the selected page with 4byte address */
#define SFLASHCMD_SECTOR_ERASE_4B  (0x21)       /* erase the selected (4KB) sector with 4byte address */


/* DTR commands	*/
/* 3-4 byte address */
#define SFLASHCMD_BYTE_READ_DTR		(0x0D)      /* n bytes read out (Double Transfer Rate DTR) until CS# goes high */
#define SFLASHCMD_QUAD_IO_READ_DTR	(0xED)      /* n bytes read out (DTR) by 4xI/O until CS# goes high */

/* 4 byte address */
#define SFLASHCMD_BYTE_READ_4B_DTR    (0x0E)    /* n bytes read out (DTR) until CS# goes high */
#define SFLASHCMD_QUAD_IO_READ_4B_DTR (0xEE)    /* n bytes read out (DTR) by 4xI/O until CS# goes high */


/* ---- serial flash register definitions ---- */
#define STREG_QUAD_BIT			(0x40)		    /* Quad mode bit(Status register)RW?!	*/
#define STREG_ODS_BIT           (0x06)          /* Output Impedance at VCC/2 15 Ohms selected (Configuration Register) */


#define CFREG_QUAD_BIT          (0x20)          /* Quad mode bit(Configuration Register) */
#define CFREG_FREEZE_BIT        (0x01)          /* freeze bit(Configuration Register) */
#define STREG_BPROTECT_BIT      (0x1c)          /* protect bit(Status Register) */
#define STREG_SRWD_BIT          (0x80)          /* Status Register Write Disable(Status Register) */
#define CFREG_LC_BIT            (0xc0)          /* Latency Code bit(Configuration Register) */
#define SET_EXTADD_4BYTE        (0x80)          /* Extended Address Enable bit(Bank Address Register) */

/* Page and Sector size */
#define SF_PAGE_SIZE            (512)           /* Page size of serial flash memory */
#define SF_SECTOR_SIZE          (64 * 1024)     /* Sector size = 64 KB   */
#define SF_NUM_OF_SECTOR        (1024)          /* Sector Count : 1024 */
#endif

/******************************************************************************
Functions Prototypes
******************************************************************************/


/* R_SFLASH_H */
#endif

/* End of File */
