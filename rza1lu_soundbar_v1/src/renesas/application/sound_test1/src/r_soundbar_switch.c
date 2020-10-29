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
* @file         r_soundbar_switch.c
* Rev: 
* Date:: 2020
* @brief        
******************************************************************************/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "r_typedefs.h"
#include "compiler_settings.h"
#include "r_switch_driver.h"
#include "r_riic_dae6_if.h"

#include "r_os_abstraction_api.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

typedef enum {
	BUTTON_PRESS_NONE,
	BUTTON_PRESS_PWR,
	BUTTON_PRESS_VOLUP,
	BUTTON_PRESS_VOLDWN,
	BUTTON_PRESS_BTPAIR,
	BUTTON_PRESS_AUDIOSELECT

}but_t;

typedef enum {
	INPUT_TYPE_COAXIAL,
	INPUT_TYPE_OPTICAL,
	INPUT_TYPE_ANALOG,
	INPUT_TYPE_HDMI
} intype_t;

typedef struct {
	uint8_t dae  : 4;
	uint8_t input : 4;
	uint8_t ch78 : 4;
	uint8_t ch56 : 4;
	uint8_t ch34 : 4;
	uint8_t ch12 : 4;
}input_reg_t;

/******************************************************************************
Macro definitions
******************************************************************************/
#define DAE_REG_WR_INPUT_SELECT 0x00220001
#define DAE_REG_RD_INPUT_SELECT 0x00220002

#define SW_POWER_BUTTON			P1_11
#define SW_VOLUME_DOWN_BUTTON	P1_9
#define SW_VOLUME_UP_BUTTON		P1_8
#define SW_AUDIO_SELECT_BUTTON	P1_10
#define SW_BT_PAIRING_BUTTON	P1_12
#define SW_BUTTONS_NUM	4

#define LED_POWER				P5_1
#define LED_VOLUME_UP			P5_2
#define LED_VOLUME_DWN			P5_8
#define LED_BTPAIR				P5_9
#define LED_AUDIO_INPUT_SELECT	P5_10

#define LED_INIT(led) gpio_init(led); \
					  gpio_dir( led, PIN_OUTPUT);

#define LED_HOLD_TIME 100

but_t g_msg[] = {BUTTON_PRESS_PWR, BUTTON_PRESS_VOLUP, BUTTON_PRESS_VOLDWN, BUTTON_PRESS_BTPAIR, BUTTON_PRESS_AUDIOSELECT};

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

static os_msg_queue_handle_t g_switch_queue;

static void switch_callback ( uint32_t sense );
/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/


/***********************************************************************************************************************
 * Function Name: r_sound_init_controls
 * Description  : Initialize all the soundbar switches and leds
 *
 * Arguments    : none
 * Return Value : none
 **********************************************************************************************************************/

void r_sound_init_controls ( void ) {

	/** Initialize switches and register callback **/
	R_SWITCH_Init(SW_POWER_BUTTON, 			switch_callback);
	R_SWITCH_Init(SW_VOLUME_UP_BUTTON, 		switch_callback);
	R_SWITCH_Init(SW_VOLUME_DOWN_BUTTON, 	switch_callback);
	R_SWITCH_Init(SW_AUDIO_SELECT_BUTTON, 	switch_callback);
	R_SWITCH_Init(SW_BT_PAIRING_BUTTON, 	switch_callback);


	/** Initialize LED switch indicators **/
	LED_INIT(LED_POWER);
	LED_INIT(LED_VOLUME_UP);
	LED_INIT(LED_VOLUME_DWN);
	LED_INIT(LED_BTPAIR);
	LED_INIT(LED_AUDIO_INPUT_SELECT);

	R_OS_CreateMessageQueue ( 3, &g_switch_queue);

}
/***********************************************************************************************************************
 End of function r_sound_init_controls
 **********************************************************************************************************************/


/***********************************************************************************************************************
 * Function Name: r_sound_power
 * Description  : Initialize all the soundbar switches and leds
 *
 * Arguments    : none
 * Return Value : none
 **********************************************************************************************************************/
static void r_sound_power ( void ) {
	// Turn on LED
	gpio_write(LED_POWER, 1);

	// Stop I2S
	// Stop DAE-x

	// Wait 100 us this will This will allow the led to be visable
	R_OS_TaskSleep(LED_HOLD_TIME);

	// Turn off LED
	gpio_write(LED_POWER, 0);

}
/***********************************************************************************************************************
 End of function r_sound_power
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Function Name: r_sound_volumeup
 * Description  : Initialize all the soundbar switches and leds
 *
 * Arguments    : none
 * Return Value : none
 **********************************************************************************************************************/
static void r_sound_volumeup ( void ) {
	// Turn on LED
	gpio_write(LED_VOLUME_UP, 1);

	// Calculate gain
	// Send Volume Command to i2C command to DAE-x

	// Wait 100 us this will This will allow the led to be visable
	R_OS_TaskSleep(LED_HOLD_TIME);

	// Turn off LED
	gpio_write(LED_VOLUME_UP, 0);
}
/***********************************************************************************************************************
 End of function r_sound_volumeup
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: r_sound_volumedown
 * Description  : Initialize all the soundbar switches and leds
 *
 * Arguments    : none
 * Return Value : none
 **********************************************************************************************************************/
static void r_sound_volumedown ( void ) {
	// Turn on LED
	gpio_write(LED_VOLUME_DWN, 1);

	// Calculate gain
	// Send Volume Command to i2C command to DAE-x

	// Wait 100 us this will This will allow the led to be visable
	R_OS_TaskSleep(LED_HOLD_TIME);

	// Turn off LED
	gpio_write(LED_VOLUME_DWN, 0);
}
/***********************************************************************************************************************
 End of function r_sound_volumedown
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Function Name: r_sound_audio_input_select
 * Description  : Initialize all the soundbar switches and leds
 *
 * Arguments    : none
 * Return Value : none
 **********************************************************************************************************************/
static void r_sound_audio_input_select ( void ) {

	static intype_t in_select = INPUT_TYPE_COAXIAL;

	input_reg_t data = {8,0,0,0,0,0};

	// Turn on LED
	gpio_write(LED_AUDIO_INPUT_SELECT, 1);

	// Get Audio Selection
	switch ( in_select ) {
		case INPUT_TYPE_COAXIAL :
			data.input = INPUT_TYPE_COAXIAL;
			data.ch78 = 0;
			break;
		case INPUT_TYPE_OPTICAL :
			data.input = INPUT_TYPE_OPTICAL;
			data.ch78 = 1;
			break;
		case INPUT_TYPE_ANALOG :
			data.input = INPUT_TYPE_COAXIAL;
			data.ch78 = 2;
			break;
		case INPUT_TYPE_HDMI :
			data.input = 0;
			data.ch78 = 0;
			break;
		default:
			break;
	}
	// Send DAE-x Imput Select Command to i2C command to DAE-x
	r_riic_dae6_Write( DAE_REG_WR_INPUT_SELECT, &data);

	// Wait 100 us this will This will allow the led to be visible
	R_OS_TaskSleep(LED_HOLD_TIME);

	// Turn off LED
	gpio_write(LED_AUDIO_INPUT_SELECT, 0);



}
/***********************************************************************************************************************
 End of function r_sound_audio_input_select
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: r_sound_bt_pairing
 * Description  : Initialize all the soundbar switches and leds
 *
 * Arguments    : none
 * Return Value : none
 **********************************************************************************************************************/
static void r_sound_bt_pairing ( void ) {
	// Turn on LED
	gpio_write(LED_BTPAIR, 1);

	// Send BT pairing command to ....

	// Wait 100 us this will This will allow the led to be visable
	R_OS_TaskSleep(LED_HOLD_TIME);

	// Turn off LED
	gpio_write(LED_BTPAIR, 0);

}
/***********************************************************************************************************************
 End of function r_sound_bt_pairing
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Function Name: task_switch_listenter
 * Description  : Initialize all the soundbar switches and leds
 *
 * Arguments    : none
 * Return Value : none
 **********************************************************************************************************************/

static void task_switch_listenter ( void ) {

	but_t value = BUTTON_PRESS_NONE;
	os_msg_t msg;
	bool_t ret;

	while (1) {
		// Waitl for message Queue
		ret = R_OS_GetMessageQueue( g_switch_queue, msg, R_OS_ABSTRACTION_PRV_EV_WAIT_INFINITE, true );

		// Check for valid message
		if ( ret ) {

			// Perform task
			switch ( value ) {
				case BUTTON_PRESS_PWR:
					r_sound_power();
					break;
				case BUTTON_PRESS_VOLUP:
					r_sound_volumeup();
					break;
				case BUTTON_PRESS_VOLDWN:
					r_sound_volumedown();
					break;
				case BUTTON_PRESS_BTPAIR:
					r_sound_bt_pairing();
					break;
				case BUTTON_PRESS_AUDIOSELECT:
					r_sound_audio_input_select();
					break;
				default:
					break;
			}
		}
	}

}
/***********************************************************************************************************************
 End of function task_switch_listenter
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Function Name: switch_callback
 * Description  : Initialize all the soundbar switches and leds
 *
 * Arguments    : none
 * Return Value : none
 **********************************************************************************************************************/
static void switch_callback ( uint32_t sense )
{
	os_msg_t msg;
	switch ( sense ) {
		case SW_POWER_BUTTON:
			msg = &g_msg[0];
			break;
		case SW_VOLUME_DOWN_BUTTON:
			msg = &g_msg[1];
			break;
		case SW_VOLUME_UP_BUTTON:
			msg = &g_msg[2];
			break;
		case SW_AUDIO_SELECT_BUTTON:
			msg = &g_msg[3];
			break;
		case SW_BT_PAIRING_BUTTON:
			msg = &g_msg[4];
			break;
		default:
			break;
	}

	R_OS_PutMessageQueue( g_switch_queue, msg);
}
/***********************************************************************************************************************
 End of function switch_callback
 **********************************************************************************************************************/
