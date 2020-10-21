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
 * Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.
 *******************************************************************************/
/*******************************************************************************
 * File Name     : spibsc_init1.c
 * Version       : 1.00
 * Device(s)     : RZ/A1L
 * Tool-Chain    : GNUARM-NONE-EABI-v16.01
 * OS            : None
 * H/W Platform  : Stream it! v2
 * Description   : Flash boot setting
 *******************************************************************************/
/*******************************************************************************
 * History       : DD.MM.YYYY Version Description
 *               : 24.08.16   1.00    First Release
 *******************************************************************************/

/******************************************************************************
 Includes <System Includes> , "Project Includes"
 ******************************************************************************/
#include "typedefine.h"
#include "iodefine.h"
#include "spibsc.h"
#include "rza_io_regrw.h"
#include "spibsc_iobitmask.h"

/******************************************************************************
 Typedef definitions
 ******************************************************************************/

/******************************************************************************
 Macro definitions
 ******************************************************************************/

/******************************************************************************
 Imported global variables and functions (from other files)
 ******************************************************************************/

/******************************************************************************
 Exported global variables and functions (to be accessed by other files)
 ******************************************************************************/

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/

/******************************************************************************
 * Function Name: spibsc_init1
 * Description  : This is the first code that runs in RAM following the load.
 *                It stops the QPSI interface, reconfigures it to high speed
 *                and then restarts the access for the copy of the main program
 *                loader.
 * Arguments    : none
 * Return Value : none
 ******************************************************************************/
void spibsc_init1(void)
{
    /* Temporary (unused) variable for synchronisation read. */
    volatile uint16_t temp = 0;

    /* SPI Multi I/O bus halted */
    CPG.STBCR9 = 0xFF;
    temp = CPG.STBCR9;

    /* SPI Multi I/O bus restarted */
    CPG.STBCR9 = 0xF7;
    temp = CPG.STBCR9;

    /* Release pin function for memory control without changing pin state */
    if (rza_io_reg_read_16((uint16_t *) &CPG.DSFR, CPG_DSFR_IOKEEP_SHIFT, CPG_DSFR_IOKEEP) == 1)
    {
        rza_io_reg_write_16((uint16_t *) &CPG.DSFR, 0, CPG_DSFR_IOKEEP_SHIFT, CPG_DSFR_IOKEEP);

        /* Perform a read after the write for synchronisation */
        temp = rza_io_reg_read_16((uint16_t *) &CPG.DSFR, CPG_DSFR_IOKEEP_SHIFT, CPG_DSFR_IOKEEP);
    }

    /* SPIBSC stop accessing the SPI in bus mode */
    rza_io_reg_write_32(&SPIBSC0.DRCR, 1, SPIBSC_DRCR_SSLN_SHIFT, SPIBSC_DRCR_SSLN);

    /* Wait for the setting to be accepted. */
    temp = 1;
    while(temp)
    {
        if(rza_io_reg_read_32(&SPIBSC0.CMNSR, SPIBSC_CMNSR_SSLF_SHIFT, SPIBSC_CMNSR_SSLF) == SPIBSC_SSL_NEGATE)
        {
        	temp = 0;
        }
    }

    /* Wait for the final transfers to end (TEND=1) for setting change in SPIBSC. */
    while (rza_io_reg_read_32(&SPIBSC0.CMNSR, SPIBSC_CMNSR_TEND_SHIFT, SPIBSC_CMNSR_TEND) != SPIBSC_TRANS_END)
    {
        /* wait for transfer-end */
    }

    /* The next access delayed setting : 1SPBCLK   */
    /* SPBSSL negate delayed setting   : 1.5SPBCLK */
    /* Clock delayed setting           : 1SPBCLK   */
    SPIBSC0.SSLDR = 0;

    /* bit rate setting register(SPBCR)  33.33[MHz] */
    rza_io_reg_write_32(&SPIBSC0.SPBCR, 0, SPIBSC_SPBCR_BRDV_SHIFT, SPIBSC_SPBCR_BRDV);
    rza_io_reg_write_32(&SPIBSC0.SPBCR, 2, SPIBSC_SPBCR_SPBR_SHIFT, SPIBSC_SPBCR_SPBR);

    /* Data read control register(DRCR) set to enable the Read Cache */
    rza_io_reg_write_32(&SPIBSC0.DRCR, 1, SPIBSC_DRCR_RBE_SHIFT, SPIBSC_DRCR_RBE);

}

/*****************************************************************************
 End of function spibsc_init1
 ******************************************************************************/

/******************************************************************************
 End  Of File
 ******************************************************************************/
