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
/**************************************************************************//**
* @file         sound_dae6.c
* Rev: 
* Date:: 2020
* @brief        
******************************************************************************/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_typedefs.h"
#include "sound_dae6.h"
#include "r_riic_dae6_drv.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/
#define DAE6_INPUT_READ	0x00020002
#define DAE6_INPUT_WRITE 0x00020001

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
Exported global functions (to be accessed by other files)
******************************************************************************/

/**************************************************************************//**
 * Function Name: sound_dae6_init
 * @brief         Set to mute DAE-6 initialization.
 *                Description:<br>
 *
 * @param         none
 * @retval        DEVDRV_SUCCESS    : Success
 * @retval        error code  : Failure
 ******************************************************************************/
int_t sound_dae6_init (void)
{

	return riic_dae6_Open();
}

/**************************************************************************//**
 * Function Name: sound_dae6_uinit
 * @brief         Set to mute DAE-6 initialization.
 *                Description:<br>
 *
 * @param         none
 * @retval        DEVDRV_SUCCESS    : Success
 * @retval        error code  : Failure
 ******************************************************************************/
int_t sound_dae6_uinit (void)
{
	return riic_dae6_Close();

}

/**************************************************************************//**
 * Function Name: sound_dae6_set_mute
 * @brief         Set to mute DAE-6 volume.
 *                Description:<br>
 *
 * @param         none
 * @retval        DEVDRV_SUCCESS    : Success
 * @retval        error code  : Failure
 ******************************************************************************/
int_t sound_dae6_set_mute (void)
{
	return 0;
}

/**************************************************************************//**
 * Function Name: sound_dae6_set_volume
 * @brief         Set to mute DAE-6 volume.
 *                Description:<br>
 *
 * @param         none
 * @retval        DEVDRV_SUCCESS    : Success
 * @retval        error code  : Failure
 ******************************************************************************/
int_t sound_dae6_set_volume (uint32_t volume )
{
	return 0;
}

/**************************************************************************//**
 * Function Name: sound_dae6_set_input
 * @brief         Set to mute DAE-6 volume.
 *                Description:<br>
 *
 * @param         none
 * @retval        DEVDRV_SUCCESS    : Success
 * @retval        error code  : Failure
 ******************************************************************************/
int_t sound_dae6_set_input (dae_in_block_t *input )
{
	int_t ret;
	uint8_t data[3] = {0,0,0};

	data[0] |= input->inputSelect;
	data[1] |= (input->channel8 << 4) & 0x30;

	data[1] |= input->channel6 & 0x01;

	data[2] |= (input->channel4 << 4) & 0x10;

	data[2] |= input->channel2 &0x01;

	ret = riic_dae6_Write( DAE6_INPUT_WRITE, &data[0]);

	return ret;
}

/**************************************************************************//**
 * Function Name: sound_dae6_get_input
 * @brief         Set to mute DAE-6 volume.
 *                Description:<br>
 *
 * @param         none
 * @retval        DEVDRV_SUCCESS    : Success
 * @retval        error code  : Failure
 ******************************************************************************/
int_t sound_dae6_get_input ( dae_in_block_t *input )
{
	uint8_t data[3];
	int_t ret = -1;

	if ( riic_dae6_Read( DAE6_INPUT_READ,  &data[0]) >= 0 ) {

		input->inputSelect = data[0] & 0x0001;
		input->channel8 = (data[1] & 0x30)>>4;
		input->channel7 = (data[1] & 0x30)>>4;
		input->channel6 = (data[1] & 0x01);
		input->channel5 = (data[1] & 0x01);
		input->channel4 = (data[2] & 0x10)>>4;
		input->channel3 = (data[2] & 0x10)>>4;
		input->channel2 = (data[2] & 0x01);
		input->channel1 = (data[2] & 0x01);
		ret = 0;
	}

	return ret;
}
