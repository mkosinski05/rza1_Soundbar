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
 * File Name     : init_spibsc_init2_section.c
 * Version       : 1.00
 * Device(s)     : RZ/A1L
 * Tool-Chain    : GNUARM-NONE-EABI-v16.01
 * OS            : None
 * H/W Platform  : Stream it! v2
 * Description   : Initialise section 2 load module 3, high speed SPI transfer.
 * Note          :
 *******************************************************************************/
/*******************************************************************************
 * History       : DD.MM.YYYY Version Description
 *               : 24.08.16   1.00    First Release
 *******************************************************************************/

/*******************************************************************************
 Includes   <System Includes> , "Project Includes"
 *******************************************************************************/
#include "typedefine.h"

#include "rza_io_regrw.h"
#include "iodefine.h"

/******************************************************************************
 Typedef definitions
 ******************************************************************************/

/******************************************************************************
 Macro definitions
 ******************************************************************************/

/******************************************************************************
 Imported global variables and functions (from other files)
 ******************************************************************************/
extern uint32_t g_ld_code_spibsc_init_2_start;
extern uint32_t g_ld_code_spibsc_init_2_end;
extern uint32_t g_ld_load_module3_addr;

/******************************************************************************
 Exported global variables and functions (to be accessed by other files)
 ******************************************************************************/

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/

/******************************************************************************
 * Function Name: init_spibsc_init2_section
 * Description  : The initialisation copy function of the section.
 * Arguments    : none
 * Return Value : none
 ******************************************************************************/
void init_spibsc_init2_section(void)
{
    /* Variables for program copy of code section 3 */
    uint32_t i;
    uint32_t loop_num;
    uint32_t *pram_start;
    uint32_t *pram_end;
    uint32_t region_size;
    uint32_t *psrc_start;

    /* Initialisation of LOAD_MODULE3 section */
    /* Global variables are specified in the load file (.ld) */
    pram_start = &g_ld_code_spibsc_init_2_start;
    pram_end   = &g_ld_code_spibsc_init_2_end;

    /* Calculate the length of the copied section */
    /* Note: The size is in long words */
    region_size = (uint32_t) (pram_end - pram_start);

    /* Calculate the number of copies for the size of the load section */
    /* add one extra to be on the safe side */
    loop_num = region_size + 1;

    /* Point src_start to start of code in ROM */
    psrc_start = &g_ld_load_module3_addr;

    /* The DMAC controller could be configured here, but little point as
     * we cannot continue until the copy completes. This is an option for
     * future development if other initialisation tasks are required in
     * the customer's system. */

    /* Copy the next load module */
    for (i = 0; i < loop_num; i++)
    {
        *pram_start++ = *psrc_start++;
    }
}

/*****************************************************************************
 End of function init_spibsc_init2_section
 ******************************************************************************/

/******************************************************************************
 End  Of File
 ******************************************************************************/
