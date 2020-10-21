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
 * File Name     : spibsc_init2.c
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
#include "spibsc_config.h"
#include "r_spibsc_ioset_api.h"
#include "rza_io_regrw.h"
#include "spibsc_userdef.h"
#include "r_boot_config.h"
#include "r_device_ioset_drv.h"

/******************************************************************************
 Typedef definitions
 ******************************************************************************/

/******************************************************************************
 Exported global variables and functions (to be accessed by other files)
 ******************************************************************************/
static st_spibsc_cfg_t m_spibsc_cfg;

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/

static const char signature[] = DEF_USER_PROGRAM_SIGNATURE;

static void spi_init_err_led_flash(void);
static void code_too_big_error_led_flash(void);

/******************************************************************************
 Imported global variables and functions (from other files)
 ******************************************************************************/

/*******************************************************************************
* Function Name: delay
* Description  : Performs a simple loop delay
* Arguments    : delay -
*                    length of delay milliseconds (approx)
* Return Value : none
*******************************************************************************/
static void delay(uint32_t delay)
{
    while (--delay)
    {
        __asm__("nop");
    }
}
/*******************************************************************************
End of function delay
*******************************************************************************/

/******************************************************************************
 * Function Name: check_image_signature
 * Description  : This function checks that the signature text expected is
 *                correct before attempting to jump to the application code.
 * Arguments    : none
 * Return Value : true if signature verified correctly
 ******************************************************************************/
static bool_t check_image_signature(void)
{
    volatile uint8_t p    = 0;
    volatile uint8_t *psrc = (uint8_t *)DEF_USER_SIGNATURE;
    volatile int16_t len = sizeof(signature);
    volatile char c_src;
    volatile char c_sig;

    while(len > 0)
    {
    	c_src = *(psrc);
    	c_sig = signature[p];

        if( c_src != c_sig)
        {
        	len= (-1);
        }
        len--;
        psrc++;
        p++;
    }

    /* If the length remaining is not zero then the signature validation failed */
    /* and this function will return false */
    return (0 == len);
}

/*****************************************************************************
 End of function check_image_signature
 ******************************************************************************/

/******************************************************************************
 * Function Name: boot_demo
 * Description  : Before calling this function the signature has been checked and
 *                a valid user's image has been detected. This function checks the
 *                location of the user's code. If it matches the RAM area then it
 *                needs to be copied into the final RAM location. If not the
 *                code is executed from the provided execution address directly.
 * Arguments    : none
 * Return Value : none
 ******************************************************************************/
static void boot_demo(void)
{
    uint32_t i;
    uint32_t loop_count;
    int32_t  size;
    uint32_t * psource;
    uint32_t * pdestination;
    uint32_t * pexecution_address;
    uint32_t * pend;
    uint32_t * pcode_start_base = DEF_USER_CODE_START;
    uint32_t * pcode_execute_base = DEF_USER_CODE_EXECUTE;
    uint32_t * pcode_end_base = DEF_USER_CODE_END;
	bool_t user_program_address_in_ram;
	bool_t user_code_too_big;

	static void * pbootprotected_ram_start;
	static void * pbootprotected_ram_mirror_start;

	/*  boot_protected_ram_start is specified in the linker prefix ld_ */
	extern char boot_protected_ram_start __asm ("ld_boot_protected_ram_start");
	pbootprotected_ram_start = &boot_protected_ram_start;
	pbootprotected_ram_mirror_start = (pbootprotected_ram_start + DEF_INTERNAL_RAM_MIRROR_OFFSET);

    psource = (uint32_t *) DEF_USER_PROGRAM_SRC;
    pdestination = (uint32_t *) *pcode_start_base;
    pexecution_address = (uint32_t *) *pcode_execute_base;
    pend = (uint32_t *) *pcode_end_base;

    size = (int32_t) ((*pcode_end_base) - (*pcode_start_base));

    user_program_address_in_ram = false;
    user_code_too_big = false;

	/* check if the user's image is within internal RAM address space */
    if (((size > 0)
        && (pdestination >= (uint32_t*) DEF_INTERNAL_RAM_NORM))
    	&& (pdestination <= (uint32_t*) pbootprotected_ram_start))
    {
    	/* check the code end address to ensure that it's within available RAM */
    	if (pend > (uint32_t*) pbootprotected_ram_start)
        {
    		user_code_too_big = true;
        }

    	user_program_address_in_ram = true;
    }

	/* check if the user's image is within internal RAM mirror address space */
    if (((size > 0)
        && (pdestination >= (uint32_t*) DEF_INTERNAL_RAM_MIRROR))
    	&& (pdestination <= (uint32_t*) pbootprotected_ram_mirror_start))
    {
    	/* check the code end address to ensure that it's within available RAM */
    	if (pend > (uint32_t*) pbootprotected_ram_mirror_start)
        {
    		user_code_too_big = true;
        }

    	user_program_address_in_ram = true;
    }

	if (user_code_too_big)
	{
    	/* flash the red LED to indicate that the program won't fit into
    	 * the available RAM. This function never returns. */
		code_too_big_error_led_flash();
	}

	/* if user's program address is within internal RAM, then copy it to RAM */
    if (user_program_address_in_ram)
    {
    	loop_count = (((uint32_t) size + 3) / (sizeof(uint32_t)));
        for (i = 0; i < loop_count; i++)
        {
            (*pdestination++) = (*psource++);
        }
    }

    /* execute the user's program */
    UserProgJmp((uint32_t) pexecution_address);
}

/*****************************************************************************
 End of function boot_demo
 ******************************************************************************/

/******************************************************************************
 * Function Name: spibsc_init2
 * Description  : changes the WSPI device into the fastest mode possible and
 *                performs actions required (if any) to get the memory space
 *                ready to execute the user application.
 *                RAM execute in RAM user code, will need to call the
 *                appropriate copy routines
 *                XIP execute in external memory no action is required user
 *                code is responsible for configuring RAM, copying data etc.
 * Arguments    : none
 * Return Value : none
 ******************************************************************************/
void spibsc_init2(void)
{

    /* Wait TEND=1 for setting change in SPIBSC. */
    R_SFLASH_WaitTend(SPIBSC_CH);

    /* ==== Sets the SPIBSC ==== */
    R_SFLASH_Set_Config(SPIBSC_CH, &m_spibsc_cfg);


#ifdef QSPI_QUAD_MODE_SUPPORT
    /* Set QSPI device into Quad Mode, do not call function not supported */
    if (R_SFLASH_Exmode_Setting(SPIBSC_CH, SPIBSC_NB_FLASH, &m_spibsc_cfg) != 0)
    {
        spi_init_err_led_flash();
    }
#endif

    /* If the check image signature failed then we just jump to the */
    /* hard-coded QSPI start address */
    /* This allows execution of a user program that has not followed */
    /* the requirements of this boot loader */
    if (!check_image_signature())
    {
    	UserProgJmp(DEF_USER_PROGRAM_SRC);
    }

    boot_demo();
}

/*****************************************************************************
 End of function spibsc_init2
 ******************************************************************************/

/******************************************************************************
 Private Functions
 ******************************************************************************/

/******************************************************************************
 * Function Name: spi_init_err_led_flash
 * Description  : This function is only called if the final boot loader stage
 *                cannot initialise SPI memory.
 *                It will not exit and configures the user LED port as an output
 *                and flashes the LED with a predefined recognisable pattern.
 * Arguments    : none
 * Return Value : none
 ******************************************************************************/
static void spi_init_err_led_flash(void)
{
	R_DEVICE_ConfigureLedOutput();

    /* This function will never exit - the system has failed to boot */
    while (1)
    {
    	R_DEVICE_SetLedState(1);
    	delay(DELAY_2000MS);
    	R_DEVICE_SetLedState(0);
    	delay(DELAY_500MS);
    	R_DEVICE_SetLedState(1);
    	delay(DELAY_500MS);
    	R_DEVICE_SetLedState(0);
    	delay(DELAY_500MS);
    }
}

/*****************************************************************************
 End of function spi_init_err_led_flash
 ******************************************************************************/

/******************************************************************************
 * Function Name: code_too_big_error_led_flash
 * Description  : This function is only called if the boot loader determines
 * 				  that the user program is too large to fit into RAM.
 *                It will not exit and configures the user LED port as an output
 *                and flashes the LED with a predefined recognisable pattern.
 * Arguments    : none
 * Return Value : none
 ******************************************************************************/
static void code_too_big_error_led_flash(void)
{
	R_DEVICE_ConfigureLedOutput();

    /* This function will never exit - the system has failed to boot */
    while (1)
    {
    	R_DEVICE_SetLedState(LED_ON);
    	delay(DELAY_250MS);
    	R_DEVICE_SetLedState(LED_OFF);
    	delay(DELAY_250MS);
    }
}

/*****************************************************************************
 End of function code_too_big_error_led_flash
 ******************************************************************************/

/******************************************************************************
 End  Of File
 ******************************************************************************/
