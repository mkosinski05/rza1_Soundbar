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
* File Name      : r_device_ioset_drv.c
 * Version       : 1.00
 * Device(s)     : RZ/A1L
 * Tool-Chain    : GNUARM-NONE-EABI-v16.01
 * OS            : None
 * H/W Platform  : Stream it! v2
 * Description   : SPI Flash IO port configuration file.
 *                 The contents of this file should be written to support
 *                 your device.
 *******************************************************************************/
/*******************************************************************************
 * History       : DD.MM.YYYY Version Description
 *               : 10.02.17   1.00    First Release
 *******************************************************************************/

/******************************************************************************
 Includes <System Includes> , "Project Includes"
 ******************************************************************************/
#include "typedefine.h"
#include "iodefine.h"
#include "rza_io_regrw.h"
#include "spibsc.h"
#include "r_spibsc_ioset_api.h"
#include "spibsc_iobitmask.h"
#include "r_device_ioset_drv.h"

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

/*******************************************************************************
* Function Name: set_mode_dual
* Description  : Sets io pins for SPIBSC_CMNCR_BSZ_DUAL configuration
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void set_mode_dual( void )
{
	/* ==== P3_10 : SPBIO01_0 ==== */
	/* port function : SPBIO01_0    */
	rza_io_reg_write_16(&GPIO.PFCAE3, 0, GPIO_PFCAE3_PFCAE310_SHIFT, GPIO_PFCAE3_PFCAE310);
	rza_io_reg_write_16(&GPIO.PFCE3, 0, GPIO_PFCE3_PFCE310_SHIFT, GPIO_PFCE3_PFCE310);
	rza_io_reg_write_16(&GPIO.PFC3, 1, GPIO_PFC3_PFC310_SHIFT, GPIO_PFC3_PFC310);

	/* port mode : Both use mode(The 2nd both use) */
	rza_io_reg_write_16(&GPIO.PMC3, 1, GPIO_PMC3_PMC310_SHIFT, GPIO_PMC3_PMC310);

	/* Input/output control mode : peripheral function */
	rza_io_reg_write_16(&GPIO.PIPC3, 1, GPIO_PIPC3_PIPC310_SHIFT, GPIO_PIPC3_PIPC310);

	/* ==== P3_11 : SPBIO11_0 ==== */
	/* port function : SPBIO11_0    */
	rza_io_reg_write_16(&GPIO.PFCAE3, 0, GPIO_PFCAE3_PFCAE311_SHIFT, GPIO_PFCAE3_PFCAE311);
	rza_io_reg_write_16(&GPIO.PFCE3, 0, GPIO_PFCE3_PFCE311_SHIFT, GPIO_PFCE3_PFCE311);
	rza_io_reg_write_16(&GPIO.PFC3, 1, GPIO_PFC3_PFC311_SHIFT, GPIO_PFC3_PFC311);

	/* port mode : Both use mode(The 2nd both use) */
	rza_io_reg_write_16(&GPIO.PMC3, 1, GPIO_PMC3_PMC311_SHIFT, GPIO_PMC3_PMC311);

	/* Input/output control mode : peripheral function */
	rza_io_reg_write_16(&GPIO.PIPC3, 1, GPIO_PIPC3_PIPC311_SHIFT, GPIO_PIPC3_PIPC311);

	/* ==== P3_12 : SPBIO21_0 ==== */
	/* port function : SPBIO21_0    */
	rza_io_reg_write_16(&GPIO.PFCAE3, 0, GPIO_PFCAE3_PFCAE312_SHIFT, GPIO_PFCAE3_PFCAE312);
	rza_io_reg_write_16(&GPIO.PFCE3, 0, GPIO_PFCE3_PFCE312_SHIFT, GPIO_PFCE3_PFCE312);
	rza_io_reg_write_16(&GPIO.PFC3, 1, GPIO_PFC3_PFC312_SHIFT, GPIO_PFC3_PFC312);

	/* port mode : Both use mode(The 2nd both use) */
	rza_io_reg_write_16(&GPIO.PMC3, 1, GPIO_PMC3_PMC312_SHIFT, GPIO_PMC3_PMC312);

	/* Input/output control mode : peripheral function */
	rza_io_reg_write_16(&GPIO.PIPC3, 1, GPIO_PIPC3_PIPC312_SHIFT, GPIO_PIPC3_PIPC312);

	/* ==== P3_13 : SPBIO31_0 ==== */
	/* port function : SPBIO31_0    */
	rza_io_reg_write_16(&GPIO.PFCAE3, 0, GPIO_PFCAE3_PFCAE313_SHIFT, GPIO_PFCAE3_PFCAE313);
	rza_io_reg_write_16(&GPIO.PFCE3, 0, GPIO_PFCE3_PFCE313_SHIFT, GPIO_PFCE3_PFCE313);
	rza_io_reg_write_16(&GPIO.PFC3, 1, GPIO_PFC3_PFC313_SHIFT, GPIO_PFC3_PFC313);

	/* port mode : Both use mode(The 2nd both use) */
	rza_io_reg_write_16(&GPIO.PMC3, 1, GPIO_PMC3_PMC313_SHIFT, GPIO_PMC3_PMC313);

	/* Input/output control mode : peripheral function */
	rza_io_reg_write_16(&GPIO.PIPC3, 1, GPIO_PIPC3_PIPC313_SHIFT, GPIO_PIPC3_PIPC313);
}
/*******************************************************************************
End of function set_mode_dual
*******************************************************************************/

/******************************************************************************
 * Function Name: R_DEVICE_SpibscPortSetting
 * Description  : Port Setting of SPIBSC
 * Arguments    : uint32_t ch_no : use channel No
 *                int_t data_bus_width
 *                uint32_t bsz : BSZ bit
 * Return Value :  0 : success
 *                -1 : error
 ******************************************************************************/
int32_t R_DEVICE_SpibscPortSetting (int_t data_bus_width, uint32_t bsz)
{

    /* ==== P4_4 : SPBCLK_0 ==== */
    /* port function : SPBCLK_0     */
    rza_io_reg_write_16(&GPIO.PFCAE4, 0, GPIO_PFCAE4_PFCAE44_SHIFT, GPIO_PFCAE4_PFCAE44);
    rza_io_reg_write_16(&GPIO.PFCE4, 0, GPIO_PFCE4_PFCE44_SHIFT, GPIO_PFCE4_PFCE44);
    rza_io_reg_write_16(&GPIO.PFC4, 1, GPIO_PFC4_PFC44_SHIFT, GPIO_PFC4_PFC44);

    /* port mode : Both use mode(The 2nd both use) */
    rza_io_reg_write_16(&GPIO.PMC4, 1, GPIO_PMC4_PMC44_SHIFT, GPIO_PMC4_PMC44);

    /* Input/output control mode : peripheral function */
    rza_io_reg_write_16(&GPIO.PIPC4, 1, GPIO_PIPC4_PIPC44_SHIFT, GPIO_PIPC4_PIPC44);

    /* ==== P4_5 : SPBSSL_0 ==== */
    /* port function : SPBSSL_0     */
    rza_io_reg_write_16(&GPIO.PFCAE4, 0, GPIO_PFCAE4_PFCAE45_SHIFT, GPIO_PFCAE4_PFCAE45);
    rza_io_reg_write_16(&GPIO.PFCE4, 0, GPIO_PFCE4_PFCE45_SHIFT, GPIO_PFCE4_PFCE45);
    rza_io_reg_write_16(&GPIO.PFC4, 1, GPIO_PFC4_PFC45_SHIFT, GPIO_PFC4_PFC45);

    /* port mode : Both use mode(The 2nd both use) */
    rza_io_reg_write_16(&GPIO.PMC4, 1, GPIO_PMC4_PMC45_SHIFT, GPIO_PMC4_PMC45);

    /* Input/output control mode : peripheral function */
    rza_io_reg_write_16(&GPIO.PIPC4, 1, GPIO_PIPC4_PIPC45_SHIFT, GPIO_PIPC4_PIPC45);

    /* ==== P4_6 : SPBIO00_0 ==== */
    /* port function : SPBIO00_0    */
    rza_io_reg_write_16(&GPIO.PFCAE4, 0, GPIO_PFCAE4_PFCAE46_SHIFT, GPIO_PFCAE4_PFCAE46);
    rza_io_reg_write_16(&GPIO.PFCE4, 0, GPIO_PFCE4_PFCE46_SHIFT, GPIO_PFCE4_PFCE46);
    rza_io_reg_write_16(&GPIO.PFC4, 1, GPIO_PFC4_PFC46_SHIFT, GPIO_PFC4_PFC46);

    /* port mode : Both use mode(The 2nd both use) */
    rza_io_reg_write_16(&GPIO.PMC4, 1, GPIO_PMC4_PMC46_SHIFT, GPIO_PMC4_PMC46);

    /* Input/output control mode : peripheral function */
    rza_io_reg_write_16(&GPIO.PIPC4, 1, GPIO_PIPC4_PIPC46_SHIFT, GPIO_PIPC4_PIPC46);

    /* ==== P4_7 : SPBIO10_0 ==== */
    /* port function : SPBIO10_0    */
    rza_io_reg_write_16(&GPIO.PFCAE4, 0, GPIO_PFCAE4_PFCAE47_SHIFT, GPIO_PFCAE4_PFCAE47);
    rza_io_reg_write_16(&GPIO.PFCE4, 0, GPIO_PFCE4_PFCE47_SHIFT, GPIO_PFCE4_PFCE47);
    rza_io_reg_write_16(&GPIO.PFC4, 1, GPIO_PFC4_PFC47_SHIFT, GPIO_PFC4_PFC47);

    /* port mode : Both use mode(The 2nd both use) */
    rza_io_reg_write_16(&GPIO.PMC4, 1, GPIO_PMC4_PMC47_SHIFT, GPIO_PMC4_PMC47);

    /* Input/output control mode : peripheral function */
    rza_io_reg_write_16(&GPIO.PIPC4, 1, GPIO_PIPC4_PIPC47_SHIFT, GPIO_PIPC4_PIPC47);

    if (SPIBSC_4BIT == data_bus_width)
    {
        /* ==== P4_2 : SPBIO20_0 ==== */
        /* port function : SPBIO20_0    */
        rza_io_reg_write_16(&GPIO.PFCAE4, 0, GPIO_PFCAE4_PFCAE42_SHIFT, GPIO_PFCAE4_PFCAE42);
        rza_io_reg_write_16(&GPIO.PFCE4, 0, GPIO_PFCE4_PFCE42_SHIFT, GPIO_PFCE4_PFCE42);
        rza_io_reg_write_16(&GPIO.PFC4, 1, GPIO_PFC4_PFC42_SHIFT, GPIO_PFC4_PFC42);

        /* port mode : Both use mode(The 2nd both use) */
        rza_io_reg_write_16(&GPIO.PMC4, 1, GPIO_PMC4_PMC42_SHIFT, GPIO_PMC4_PMC42);

        /* Input/output control mode : peripheral function */
        rza_io_reg_write_16(&GPIO.PIPC4, 1, GPIO_PIPC4_PIPC42_SHIFT, GPIO_PIPC4_PIPC42);

        /* ==== P4_3 : SPBIO30_0 ==== */
        /* port function : SPBIO30_0    */
        rza_io_reg_write_16(&GPIO.PFCAE4, 0, GPIO_PFCAE4_PFCAE43_SHIFT, GPIO_PFCAE4_PFCAE43);
        rza_io_reg_write_16(&GPIO.PFCE4, 0, GPIO_PFCE4_PFCE43_SHIFT, GPIO_PFCE4_PFCE43);
        rza_io_reg_write_16(&GPIO.PFC4, 1, GPIO_PFC4_PFC43_SHIFT, GPIO_PFC4_PFC43);

        /* port mode : Both use mode(The 2nd both use) */
        rza_io_reg_write_16(&GPIO.PMC4, 1, GPIO_PMC4_PMC43_SHIFT, GPIO_PMC4_PMC43);

        /* Input/output control mode : peripheral function */
        rza_io_reg_write_16(&GPIO.PIPC4, 1, GPIO_PIPC4_PIPC43_SHIFT, GPIO_PIPC4_PIPC43);

        if (SPIBSC_CMNCR_BSZ_DUAL == bsz)
        {
        	set_mode_dual();
        }
    }

    return (0);
}
/*******************************************************************************
End of function R_DEVICE_SpibscPortSetting
*******************************************************************************/



/*******************************************************************************
* Function Name: R_DEVICE_ConfigureLedOutput
* Description  : Perform any actions required to configure output LED(s)
* Arguments    : none
* Return Value : none
*******************************************************************************/
void R_DEVICE_ConfigureLedOutput(void)
{
    /* Configure the port pin for LED0 as an output */
    rza_io_reg_write_16(&GPIO.PMC7, 0, GPIO_PMC7_PMC78_SHIFT, GPIO_PMC7_PMC78);
    rza_io_reg_write_16(&GPIO.P7, 1, GPIO_P7_P78_SHIFT, GPIO_P7_P78);
    rza_io_reg_write_16(&GPIO.PM7, 0, GPIO_PM7_PM78_SHIFT, GPIO_PM7_PM78);
    rza_io_reg_write_16(&GPIO.PIPC7, 0, GPIO_PIPC7_PIPC78_SHIFT, GPIO_PIPC7_PIPC78);
}
/*******************************************************************************
End of function R_DEVICE_ConfigureLedOutput
*******************************************************************************/

/*******************************************************************************
* Function Name: R_DEVICE_SetLedState
* Description  : Sets the error LED (on or off)
* Arguments    : state -
*                    sets LED state, currently only 2 states are supported
*	                 LED_ON   = 1  switch ON on
*	                 LED_OFF  = 0  switch LED off
* Return Value : none
*******************************************************************************/
void R_DEVICE_SetLedState(led_states_t state)
{
	rza_io_reg_write_16(&GPIO.P7, state, GPIO_P7_P78_SHIFT, GPIO_P7_P78);
}
/*******************************************************************************
End of function R_DEVICE_SetLedState
*******************************************************************************/

/* End of File */
