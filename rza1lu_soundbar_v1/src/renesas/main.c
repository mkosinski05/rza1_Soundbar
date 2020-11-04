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
 * Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
 *******************************************************************************/
/*******************************************************************************
 * File Name    : main.c
 * Device(s)    : RZ/A1LU
 * Tool-Chain   : GCC for Renesas RZ
 * OS           : FreeRTOS Version See src/freertos/include/FreeRTOS.h
 * H/W Platform : Stream it! v2 board
 * Description  : WebEngine
 *                This demonstration run on the Renesas Stream it! v2 platform.
 *******************************************************************************/

/* Dependencies */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "r_typedefs.h"
#include "compiler_settings.h"

#include "iodefine_cfg.h"
#include "control.h"
#include "version.h"

#include "r_gpio_if.h"
#include "gpio_iobitmask.h"
#include "sound_dae6.h"
#include "r_soundtest1.h"
#include "r_switch_driver.h"

#include "command.h"
#include "r_task_priority.h"

#include "dskManager.h"
#include "r_fatfs_abstraction.h"
#include "ff.h"

#include "r_soundtest1.h"
#include "main.h"

static st_iostr_t gs_iostr[3];
static FIL *m_pFP;

#if GUI_DEMO_ENABLE
extern int_t gui_main(void);
#endif

void R_MSG_WarningConfig(pst_comset_t p_com, char_t *msg);
int_t g_led_handle = ( -1);
/* USB Global Variables */
bool_t dma_available = false;
event_t g_control_audioStream;

#define GUI_DEMO_ENABLE 0
#define SOUND_TEST2_DEMO_ENABLE 1

#define LED_PORT 5
#define LED_PINS 0xFF06
#define LED0 P5_1
//#define LED0 P7_8

#define SW_POWER_BUTTON			P1_11
#define SW_VOLUME_DOWN_BUTTON	P1_9
#define SW_VOLUME_UP_BUTTON		P1_8
#define SW_AUDIO_SELECT_BUTTON	P1_10
#define SW_BT_PAIRING_BUTTON	P1_12

#define SOUND_AUDIOTRACK_DEMO	"thunder.wav"

/***********************************************************************************************************************
 * Function Name: blink_task
 * Description  : LED blinking task
 * Arguments    : void *parameters
 * Return Value : none
 ***********************************************************************************************************************/
static void blink_task (void *parameters)
{
    UNUSED_PARAM(parameters);


    if (g_led_handle < 0)
    {
        printf("gs_stdio_led_handle led driver failed to OPEN \r\n");
    }

    /* Endless loop */
    while (1)
    {

    	gpio_write(LED0, 0);
        R_OS_TaskSleep(300);

        gpio_write(LED0, 1);
        R_OS_TaskSleep(700);
    }

}

#if GUI_DEMO_ENABLE
/***********************************************************************************************************************
 End of function blink_task
 ***********************************************************************************************************************/
static void gui_task (void *parameters){
	int_t usb0_handle = ( -1);
	dae_in_block_t in;

	/* Assign the streams for the serial console */
	gs_iostr[0].p_in = stdin;
	gs_iostr[0].p_out = stdout;

	usb0_handle = open( DEVICE_INDENTIFIER "usb0", O_RDWR);

	if ( sound_dae6_init() >= 0) {
		sound_dae6_get_input(&in);
	}

	R_OS_TaskSleep( 1000 );
	gui_main();
}
#endif

void dummy_func ( uint32_t sense) {
	// Callback IRQ
	R_OS_SetEvent(&g_control_audioStream);
}

static void sound_task ( void *parameters) {

	// Register Button
	r_sound_init_controls();

	// Initialize Audio
	r_soundtst_PlayBack_init();

	while(1) {
		//R_OS_WaitForEvent(&g_control_audioStream, R_OS_ABSTRACTION_PRV_EV_WAIT_INFINITE);
		R_OS_TaskSleep(1000);
	}
}

static void sound_task1 ( void *parameters) {
	int_t usb0_handle = ( -1);
	dae_in_block_t in;

	 R_OS_CreateEvent( &g_control_audioStream);
	 R_OS_ResetEvent(&g_control_audioStream);

	/* Assign the streams for the serial console */
	gs_iostr[0].p_in = stdin;
	gs_iostr[0].p_out = stdout;

	usb0_handle = open( DEVICE_INDENTIFIER "usb0", O_RDWR);

	// Wait for USB to connect to Device
	R_OS_TaskSleep( 2000 );

	if ( 0 < dskMountAllDevices() ) {

		f_chdrive("A:\\");
	}
	// Register Power Button
	// Power button will be used to start SSI
	R_SWITCH_Init(SW_POWER_BUTTON, dummy_func);

	// Initialize Audio
	r_soundtst_PlaySample_init();
	// Load Default Audio Track
	m_pFP = R_FAT_OpenFile ( SOUND_AUDIOTRACK_DEMO, FA_READ );
	r_soundtst_LoadSample(m_pFP);

	while(1) {
		R_OS_WaitForEvent(&g_control_audioStream, R_OS_ABSTRACTION_PRV_EV_WAIT_INFINITE);
		r_soundtst_PlaySample();
		r_soundtst_IsDone();
	}
}
/***********************************************************************************************************************
 End of function console_task
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: main
 * Description  : Main entry point
 * Arguments    : none
 * Return Value : 0
 ***********************************************************************************************************************/
int_t main (void)
{
    /* OS variables */
    os_task_t *p_os_task;
    gpio_init(LED0);
    gpio_dir( LED0, PIN_OUTPUT);

    /* Create a task to blink the LED */
    p_os_task = R_OS_CreateTask("Blink", blink_task, NULL, R_OS_ABSTRACTION_PRV_DEFAULT_STACK_SIZE, TASK_BLINK_TASK_PRI);

    /* NULL signifies that no task was created by R_OS_CreateTask */
    if (NULL == p_os_task)
    {
        /* Debug message */
    }
#if GUI_DEMO_ENABLE
    p_os_task = R_OS_CreateTask("gui", gui_task, NULL, R_OS_ABSTRACTION_PRV_DEFAULT_STACK_SIZE, TASK_BLINK_TASK_PRI);
#else
    p_os_task = R_OS_CreateTask("D2Audio", sound_task, NULL, R_OS_ABSTRACTION_PRV_DEFAULT_STACK_SIZE, TASK_BLINK_TASK_PRI);
#endif
#if SOUND_TEST2_DEMO_ENABLE
#endif

    /* Need to determine system state is running */
    if (R_OS_GetNumberOfTasks())
    {

        while (1)
        {
            R_OS_TaskSleep(10000);
        }
    }

    /* Shut down all system resources */
    if (( -1) != g_led_handle)
    {
        close(g_led_handle);
    }

    /* This function should never return */
    while (1)
    {
        /* R_COMPILER_Nop() */
        ;
    }

    return 0;
}
/***********************************************************************************************************************
 End of function main
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: abort
 * Description  : abort handler may be called by libraries
 * Arguments    : none
 * Return Value : 0
 ***********************************************************************************************************************/
void abort (void)
{
    while (1)
    {
        R_COMPILER_Nop();
    }
}
/***********************************************************************************************************************
 End of function abort
 ***********************************************************************************************************************/
