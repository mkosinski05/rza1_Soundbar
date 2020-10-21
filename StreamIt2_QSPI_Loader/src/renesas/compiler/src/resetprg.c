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
 *******************************************************************************/
/*******************************************************************************
 * Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.
 *******************************************************************************/
/******************************************************************************
 * File Name     : resetprg.c
 * Device(s)     : RZ/A1L
 * Tool-Chain    : GNUARM-NONE-EABI-v16.01
 * H/W Platform  : Stream it! v2
 * Description   : C library entry point
 *               : Variants of this file must be created for each compiler
 ******************************************************************************/
/*******************************************************************************
 * History       : DD.MM.YYYY Version Description
 *               : 06.07.2016 1.00    Ported from RZA1H
 *******************************************************************************/

#include "typedefine.h"
/* I/O Register root header */
#include "iodefine.h"
/* Common Driver header */
#include "devdrv_common.h"
/* device hardware register functions header */
#include "rza_io_regrw.h"
/* System CPG register configuration header */
#include "stb_init.h"
/* SPI flash device support definition file */
#include "spibsc_userdef.h"

#include "resetprg.h"

#include "spibsc_init1.h"
#include "spibsc_init2.h"
#include "init_spibsc_init1_section.h"
#include "init_spibsc_init2_section.h"


/* System CPG register configuration header */

#if defined(__thumb2__) || (defined(__thumb__) && defined(__ARM_ARCH_6M__))
# define THUMB_V7_V6M
#endif

/* Defined if this target supports the BLX Rm instruction.  */
#if  !defined(__ARM_ARCH_2__) && !defined(__ARM_ARCH_3__) && !defined(__ARM_ARCH_3M__)    && !defined(__ARM_ARCH_4__)    && !defined(__ARM_ARCH_4T__)
# define HAVE_CALL_INDIRECT
#endif


#ifdef HAVE_INITFINI_ARRAY
#define _init    (__libc_init_array)
#define _fini    (__libc_fini_array)
#endif

/*******************************************************************************
 * Function Name: resetprg
 * Description  : Program entry point after basic peripheral init.
 * Arguments    : none
 * Return Value : none
 *******************************************************************************/
void resetprg(void)
{
    volatile uint8_t dummy_buf = 0u;

    CPG.SYSCR3 = 0x0F;      // Enable access to Data Retention RAM (lower 128kB RAM).

    /* (Dummy read)                                                   */
    dummy_buf = CPG.SYSCR3;

    init_spibsc_init1_section(); // LOAD_MODULE2 to internal RAM (executed in QSPI)


    #ifdef QSPI_QUAD_MODE_SUPPORT

	rza_io_reg_write_16(&GPIO.PMC3, 0, GPIO_PMC3_PMC310_SHIFT, GPIO_PMC3_PMC310);
	rza_io_reg_write_16(&GPIO.PM3, 0, GPIO_PM3_PM310_SHIFT, GPIO_PM3_PM310);
	rza_io_reg_write_16(&GPIO.P3, 1, GPIO_P3_P310_SHIFT, GPIO_P3_P310);

	rza_io_reg_write_16(&GPIO.PMC3, 0, GPIO_PMC3_PMC311_SHIFT, GPIO_PMC3_PMC311);
	rza_io_reg_write_16(&GPIO.PM3, 0, GPIO_PM3_PM311_SHIFT, GPIO_PM3_PM311);
	rza_io_reg_write_16(&GPIO.P3, 1, GPIO_P3_P311_SHIFT, GPIO_P3_P311);

	rza_io_reg_write_16(&GPIO.PMC3, 0, GPIO_PMC3_PMC312_SHIFT, GPIO_PMC3_PMC312);
	rza_io_reg_write_16(&GPIO.PM3, 0, GPIO_PM3_PM312_SHIFT, GPIO_PM3_PM312);
	rza_io_reg_write_16(&GPIO.P3, 1, GPIO_P3_P312_SHIFT, GPIO_P3_P312);

	#endif

    rza_io_reg_write_16(&GPIO.PMC3, 0, GPIO_PMC3_PMC313_SHIFT, GPIO_PMC3_PMC313);
    rza_io_reg_write_16(&GPIO.PM3, 0, GPIO_PM3_PM313_SHIFT, GPIO_PM3_PM313);
    rza_io_reg_write_16(&GPIO.P3, 1, GPIO_P3_P313_SHIFT, GPIO_P3_P313);


    spibsc_init1(); // Move the SPI to high speed with cache and bus mode ON (executed in internal RAM)

    /* The initial loader has run and the SPI interface is now at high speed */
    init_spibsc_init2_section(); // LOAD_MODULE3 to internal RAM;

    #ifdef QSPI_QUAD_MODE_SUPPORT

    rza_io_reg_write_16(&GPIO.PMC3, 0, GPIO_PMC3_PMC310_SHIFT, GPIO_PMC3_PMC310);
    rza_io_reg_write_16(&GPIO.PM3, 1, GPIO_PM3_PM310_SHIFT, GPIO_PM3_PM310);
    rza_io_reg_write_16(&GPIO.P3, 0, GPIO_P3_P310_SHIFT, GPIO_P3_P310);

    rza_io_reg_write_16(&GPIO.PMC3, 0, GPIO_PMC3_PMC311_SHIFT, GPIO_PMC3_PMC311);
    rza_io_reg_write_16(&GPIO.PM3, 1, GPIO_PM3_PM311_SHIFT, GPIO_PM3_PM311);
    rza_io_reg_write_16(&GPIO.P3, 0, GPIO_P3_P311_SHIFT, GPIO_P3_P311);

    rza_io_reg_write_16(&GPIO.PMC3, 0, GPIO_PMC3_PMC312_SHIFT, GPIO_PMC3_PMC312);
    rza_io_reg_write_16(&GPIO.PM3, 1, GPIO_PM3_PM312_SHIFT, GPIO_PM3_PM312);
    rza_io_reg_write_16(&GPIO.P3, 0, GPIO_P3_P312_SHIFT, GPIO_P3_P312);

    #endif

    rza_io_reg_write_16(&GPIO.PMC3, 0, GPIO_PMC3_PMC313_SHIFT, GPIO_PMC3_PMC313);
    rza_io_reg_write_16(&GPIO.PM3, 1, GPIO_PM3_PM313_SHIFT, GPIO_PM3_PM313);
    rza_io_reg_write_16(&GPIO.P3, 0, GPIO_P3_P313_SHIFT, GPIO_P3_P313);

    spibsc_init2();

    /* Stops program from running off */
    while (1)
    {
        __asm__("nop");
    }
}
/*******************************************************************************
 End of function resetprg
 *******************************************************************************/
