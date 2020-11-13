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
#include <math.h>
#include "r_typedefs.h"
#include "compiler_settings.h"
#include "r_switch_driver.h"
#include "r_riic_dae6_if.h"
#include "RegisterSet.h"

#include "r_os_abstraction_api.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

typedef enum {
	BUTTON_PRESS_VOLUP = 0,
	BUTTON_PRESS_VOLDWN,
	BUTTON_PRESS_AUDIOSELECT,
	BUTTON_PRESS_PWR,
	BUTTON_PRESS_BTPAIR,
	BUTTON_PRESS_NONE
}but_t;

typedef enum {
	INPUT_TYPE_SPDIF_COAXIAL,
	INPUT_TYPE_SPDIF_OPTICAL,
	INPUT_TYPE_LINE_IN,
	INPUT_TYPE_BT,
	INPUT_TYPE_USB,
	INPUT_TYPE_WIFI,
	INPUT_TYPE_ATMOS_HDMI,
	INPUT_TYPE_SSI = 0,
	INPUT_TYPE_SPDIF = 1,
} intype_t;

typedef union {
	struct {
		uint8_t ch12 : 4;
		uint8_t ch34 : 4;
		uint8_t ch56 : 4;
		uint8_t ch78 : 4;
		uint8_t input : 4;
		uint8_t dae  : 4;
		uint8_t dummy;
	} bit;
	uint32_t dword;
}input_reg_t;

/******************************************************************************
Macro definitions
******************************************************************************/
#define DAE_REG_WR_INPUT_SELECT 0x00020001
#define DAE_REG_RD_INPUT_SELECT 0x00020002
#define DAE_REG_WR_VOLUME_CONTROL SharedVolume_1_VOL

#define SW_POWER_BUTTON			P1_11
#define SW_VOLUME_DOWN_BUTTON	P1_9
#define SW_VOLUME_UP_BUTTON		P1_8
#define SW_AUDIO_SELECT_BUTTON	P1_10
#define SW_BT_PAIRING_BUTTON	P1_12
#define SW_BUTTONS_NUM	4

// LED for the associated with the control switches
#define LED_POWER_PIN				P5_1	// Led 1
#define LED_VOLUME_UP_PIN			P5_8	// Led 2
#define LED_VOLUME_DWN_PIN			P5_2	//
#define LED_BTPAIR_PIN				P5_9
#define LED_AUDIO_INPUT_SELECT_PIN	P5_10

// LED Audio Source
#define LED_LINE_IN_PIN			P5_11
#define LED_SPDIF_PIN			P5_12
#define LED_USB_PIN				P5_13
#define LED_BT_PIN				P5_14
#define LED_WIFI_PIN			P5_15

// LED display current input selects

// Board Enable pins
#define BOARD_LV_EN_PIN			P3_15
#define BOARD_RST_EN_PIN		P2_5
#define BOARD_HV_EN_PIN			P3_14
#define BOARD_LOW				0
#define BOARD_HIGH				1


/*** Mux selector pins ***/
// Analog,Bluetooth,WIFI=0 and HDMI=1 select
#define SEL_OPT_COAX_PIN		P4_1
#define SEL_OPTICAL
#define SEL_COXIAL

#define ATMOS_EN_PIN			P2_6
#define ATMOS_DISABLED			0
#define ATMOS_ENDABLED			1

#define ATMOS_HDMI				1
#define ATMOS_ANALOG			0

#define LED_INIT(led) gpio_init(led); \
					  gpio_dir( led, PIN_OUTPUT); \
					  gpio_write( led, 0);

#define LED_HOLD_TIME 100

#define DEBOUNCE_DELAY	(100)

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

static QueueHandle_t g_switch_queue;

static void task_switch_listenter ( void );

static void switch_callback ( int sense );

static void r_sound_audio_input_select ( void );

static void calculate_and_send_gain ( void );

static PinName g_button_pins[] = {
	SW_VOLUME_UP_BUTTON,
	SW_VOLUME_DOWN_BUTTON,
	SW_AUDIO_SELECT_BUTTON,
	SW_POWER_BUTTON,
	SW_BT_PAIRING_BUTTON
};

static int16_t g_Shared_Gain = -50;

static bool_t g_EnableIterrupts = false;

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

void r_sound_init_controls ( bool_t enble_interrupts ) {

	/*** Enable Power Stages ***/
	// Enable power to 3.3v and 1.8v line
	gpio_init ( BOARD_LV_EN_PIN );
	gpio_dir  ( BOARD_LV_EN_PIN, PIN_OUTPUT );
	gpio_write( BOARD_LV_EN_PIN, BOARD_HIGH);
	// Bring up peripherals
	gpio_init ( BOARD_RST_EN_PIN );
	gpio_dir  ( BOARD_RST_EN_PIN, PIN_OUTPUT );
	gpio_write( BOARD_RST_EN_PIN, BOARD_LOW);
	// Enable High Voltage Power Stages
	gpio_init ( BOARD_HV_EN_PIN );
	gpio_dir  ( BOARD_HV_EN_PIN, PIN_OUTPUT );
	gpio_write( BOARD_HV_EN_PIN, BOARD_HIGH);

	/*** Setup Audio Select Pins ***/
	// Set the Multiplexer to the BT,
	gpio_init ( ATMOS_EN_PIN );
	gpio_dir  ( ATMOS_EN_PIN, PIN_OUTPUT );
	gpio_write( ATMOS_EN_PIN, ATMOS_DISABLED);

	gpio_init ( SEL_OPT_COAX_PIN );
	gpio_dir  ( SEL_OPT_COAX_PIN, PIN_OUTPUT );
	gpio_write( SEL_OPT_COAX_PIN, 0);

	// Initialize the I2C DAE interface
	r_riic_dae6_Open();
	R_OS_TaskSleep( 100 );

	// Switch Message Queue
	g_switch_queue = xQueueCreate ( 3, sizeof(void*));

	// Switch Task
	R_OS_CreateTask("Soundbar Control", task_switch_listenter, NULL, R_OS_ABSTRACTION_PRV_DEFAULT_STACK_SIZE, TASK_SWITCH_TASK_PRI);

}
/***********************************************************************************************************************
 End of function r_sound_init_controls
 **********************************************************************************************************************/


/***********************************************************************************************************************
 * Function Name: r_sound_control_select_audio_input
 * Description  : Initialize all the soundbar switches and leds
 *
 * Arguments    : none
 * Return Value : none
 **********************************************************************************************************************/
void r_sound_control_select_audio_input ( void )
{
	r_sound_audio_input_select();
}
/***********************************************************************************************************************
 End of function r_sound_control_select_audio_input
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
	gpio_write(LED_POWER_PIN, 1);

	// Stop I2S
	// Stop DAE-x

	// Wait 100 us this will This will allow the led to be visable
	R_OS_TaskSleep(LED_HOLD_TIME);

	// Turn off LED
	gpio_write(LED_POWER_PIN, 0);

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
	gpio_write(LED_VOLUME_UP_PIN, 1);

	// Calculate gain
	if (++g_Shared_Gain > 0 ) {
		g_Shared_Gain = 0;
	}
	calculate_and_send_gain();

	// Wait 100 us this will This will allow the led to be visible
	R_OS_TaskSleep(LED_HOLD_TIME);

	// Turn off LED
	gpio_write(LED_VOLUME_UP_PIN, 0);
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
	gpio_write(LED_VOLUME_DWN_PIN, 1);

	// Calculate gain
	if (--g_Shared_Gain < -100 ) {
		g_Shared_Gain = -100;
	}
	calculate_and_send_gain();


	// Wait 100 us this will This will allow the led to be visable
	R_OS_TaskSleep(LED_HOLD_TIME);

	// Turn off LED
	gpio_write(LED_VOLUME_DWN_PIN, 0);
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

	static intype_t in_select = INPUT_TYPE_LINE_IN;

	input_reg_t data;
	data.dword = 0;
	data.bit.dae = 8;
	data.bit.input = INPUT_TYPE_SPDIF_OPTICAL; // Always set

	// Turn on LED
	gpio_write(LED_AUDIO_INPUT_SELECT_PIN, 1);

	// Clear Source LEDs
	gpio_write(LED_LINE_IN_PIN, 0);
	gpio_write(LED_SPDIF_PIN, 0);
	gpio_write(LED_USB_PIN, 0);
	gpio_write(LED_WIFI_PIN, 0);
	gpio_write(LED_BT_PIN, 0);

	// Get Audio Selection
	switch ( in_select ) {
		case INPUT_TYPE_LINE_IN :
			data.bit.ch12 = INPUT_TYPE_SSI;
			data.bit.ch34 = INPUT_TYPE_SSI;
			data.bit.ch56 = INPUT_TYPE_SSI;
			data.bit.ch78 = INPUT_TYPE_SSI;

			gpio_write(SEL_OPT_COAX_PIN, 1);
			gpio_write(LED_LINE_IN_PIN, 1);
			in_select = INPUT_TYPE_SPDIF_COAXIAL;
			break;

		case INPUT_TYPE_SPDIF_COAXIAL :
			data.bit.input = INPUT_TYPE_SPDIF_OPTICAL; // Coaxial is select by pin not DAE

			data.bit.ch12 = INPUT_TYPE_SPDIF;
			data.bit.ch34 = INPUT_TYPE_SPDIF;
			data.bit.ch56 = INPUT_TYPE_SPDIF;
			data.bit.ch78 = INPUT_TYPE_SSI; // Keep the DAE

			gpio_write(SEL_OPT_COAX_PIN, 0);
			gpio_write(LED_SPDIF_PIN, 1);

			in_select = INPUT_TYPE_SPDIF_OPTICAL;
			break;
		case INPUT_TYPE_SPDIF_OPTICAL :
			data.bit.input = INPUT_TYPE_SPDIF_OPTICAL;

			data.bit.ch12 = INPUT_TYPE_SPDIF;
			data.bit.ch34 = INPUT_TYPE_SPDIF;
			data.bit.ch56 = INPUT_TYPE_SPDIF;
			data.bit.ch78 = INPUT_TYPE_SSI;

			gpio_write(SEL_OPT_COAX_PIN, 1);
			gpio_write(LED_SPDIF_PIN, 1);

			in_select = INPUT_TYPE_USB;
			break;

		case INPUT_TYPE_USB :
			gpio_write(LED_USB_PIN, 1);
			in_select = INPUT_TYPE_BT;
			break;
		case INPUT_TYPE_BT :
			data.bit.ch12 = INPUT_TYPE_SSI;
			data.bit.ch34 = INPUT_TYPE_SSI;
			data.bit.ch56 = INPUT_TYPE_SSI;
			data.bit.ch78 = INPUT_TYPE_SSI;
			gpio_write(LED_BT_PIN, 1);
			in_select = INPUT_TYPE_WIFI;
			break;

		case INPUT_TYPE_WIFI :
			data.bit.ch12 = INPUT_TYPE_SSI;
			data.bit.ch34 = INPUT_TYPE_SSI;
			data.bit.ch56 = INPUT_TYPE_SSI;
			data.bit.ch78 = INPUT_TYPE_SSI;
			gpio_write(LED_WIFI_PIN, 1);
			in_select = INPUT_TYPE_LINE_IN;
			break;
#if 0
		case INPUT_TYPE_ATMOS_HDMI :
			data.bit.input = 0;
			data.bit.ch12 = INPUT_TYPE_SSI;
			data.bit.ch34 = INPUT_TYPE_SSI;
			data.bit.ch56 = INPUT_TYPE_SSI;
			data.bit.ch78 = INPUT_TYPE_SSI;
			gpio_write( ATMOS_EN, ATMOS_HDMI);
			break;
#endif
		default:
			break;
	}
#ifdef BUILD_CONFIG_RELEASE
	if ( in_select != INPUT_TYPE_USB) {
		// Send DAE-x Imput Select Command to i2C command to DAE-x
		r_riic_dae6_Write( DAE_REG_WR_INPUT_SELECT, (uint8_t*)&data);
	}
#endif

	// Wait 100 us this will This will allow the led to be visible
	R_OS_TaskSleep(LED_HOLD_TIME);

	// Turn off LED
	gpio_write(LED_AUDIO_INPUT_SELECT_PIN, 0);



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
	gpio_write(LED_BTPAIR_PIN, 1);

	// Send BT pairing command to ....

	// Wait 100 us this will This will allow the led to be visable
	R_OS_TaskSleep(LED_HOLD_TIME);

	// Turn off LED
	gpio_write(LED_BTPAIR_PIN, 0);

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

	static but_t current_msg = BUTTON_PRESS_NONE;
	static but_t last_msg;
	bool_t ret;
	static uint16_t ms;

	// Setup Control Leds
	LED_INIT ( LED_POWER_PIN );
	LED_INIT ( LED_VOLUME_UP_PIN );
	LED_INIT ( LED_VOLUME_DWN_PIN );
	LED_INIT ( LED_BTPAIR_PIN );
	LED_INIT ( LED_AUDIO_INPUT_SELECT_PIN );

	LED_INIT ( LED_LINE_IN_PIN );
	LED_INIT ( LED_SPDIF_PIN );
	LED_INIT ( LED_USB_PIN );
	LED_INIT ( LED_BT_PIN );
	LED_INIT ( LED_WIFI_PIN );

	/** Initialize switches and register callback **/
#if 1
	R_SWITCH_Init(SW_POWER_BUTTON, 			switch_callback);
	R_SWITCH_Init(SW_VOLUME_UP_BUTTON, 		switch_callback);
	R_SWITCH_Init(SW_VOLUME_DOWN_BUTTON, 	switch_callback);
	R_SWITCH_Init(SW_AUDIO_SELECT_BUTTON, 	switch_callback);
	R_SWITCH_Init(SW_BT_PAIRING_BUTTON, 	switch_callback);
#else
	R_SWITCH_Init(SW_POWER_BUTTON, 			NULL);
	R_SWITCH_Init(SW_VOLUME_UP_BUTTON, 		NULL);
	R_SWITCH_Init(SW_VOLUME_DOWN_BUTTON, 	NULL);
	R_SWITCH_Init(SW_AUDIO_SELECT_BUTTON, 	NULL);
	R_SWITCH_Init(SW_BT_PAIRING_BUTTON, 	NULL);
#endif

	// Initialize DAE
	r_sound_audio_input_select();
	calculate_and_send_gain();


	while (1) {
#if 1
		// Waitl for message Queue
		ret = xQueueReceive( g_switch_queue, (void*)&current_msg, R_OS_ABSTRACTION_PRV_EV_WAIT_INFINITE );
		last_msg = current_msg;

		// Filter debouce
		while(ret ) {
			// Read Queue for addtional message every 10ms till queue is empty
			ret = xQueueReceive( g_switch_queue, (void*)&current_msg, 10 );
		}

#else

		R_OS_TaskSleep( 250 );

		for ( int b = BUTTON_PRESS_VOLUP; b < BUTTON_PRESS_NONE; b++ ) {

			if ( R_SWITCH_Poll ( g_button_pins[b] )) {
				current_msg = BUTTON_PRESS_NONE;
			} else {
				current_msg = b;
			}


#endif
		// Perform task
		switch ( last_msg ) {
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
static void switch_callback ( int sense )
{
	but_t msg = BUTTON_PRESS_NONE;

	switch ( sense ) {
		case BUTTON_PRESS_PWR:
			msg = BUTTON_PRESS_PWR;
			break;
		case BUTTON_PRESS_VOLUP:
			msg = BUTTON_PRESS_VOLUP;
			break;
		case BUTTON_PRESS_VOLDWN:
			msg = BUTTON_PRESS_VOLDWN;
			break;
		case BUTTON_PRESS_BTPAIR:
			msg = BUTTON_PRESS_BTPAIR;
			break;
		case BUTTON_PRESS_AUDIOSELECT:
			msg = BUTTON_PRESS_AUDIOSELECT;
			break;
		default:
			break;
	}

	xQueueSendFromISR( (QueueHandle_t)g_switch_queue, (void*)&msg, 0UL);
}
/***********************************************************************************************************************
 End of function switch_callback
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: calculate_and_send_gain
 * Description  : Calculates the hex value of the shared gain based on Normal Polarity ( -100 db to 0 db).
 *
 * Arguments    : none
 * Return Value : none
 **********************************************************************************************************************/
static void calculate_and_send_gain ( void ) {

	float TWO2TWEENTYTHIRD = 8388608;
	float value = 0;
	int32_t data = 0;

	// Calculate the New Shared Volume Gain
	value = g_Shared_Gain / 20;
	value = pow(10.0, value);
	value *= TWO2TWEENTYTHIRD * -1;
	data = (int32_t)value;


	// Send Volume Command to i2C command to DAE-x
	// Send DAE-x Imput Select Command to i2C command to DAE-x
	r_riic_dae6_Write( DAE_REG_WR_VOLUME_CONTROL, (uint8_t*)&data);


}
/***********************************************************************************************************************
 End of function calculate_and_send_gain
 **********************************************************************************************************************/
