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
* @file         riic_dae6.c
* $Rev: 8700 $
* $Date:: 2018-06-29 15:46:04 +0100#$
* @brief        riic_ch1 driver riic device depended functions
******************************************************************************/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

#include    <stdio.h>
#include    <string.h>
#include    <fcntl.h>

#include 	"dev_drv.h"
#include    "r_riic_dae6_drv.h"
#include    "r_devlink_wrapper.h"
#include    "r_riic_drv_sc_cfg.h"
#include 	"r_os_abstraction_api.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

typedef struct {
    semaphore_t    semid;       /*!< Access Semaphore*/
    int_t          hi2c1;       /*!< Channel Handle*/
} riic_ch1_aio_ctrl_t;
/******************************************************************************
Macro definitions
******************************************************************************/

#define DAE6_DEVICE_ADDR	(0x59<<1)

#define DAE6_REG_MAX	0x000FFFFFF

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

static riic_ch1_aio_ctrl_t s_i2c1_ctrl =
{
    NULL,             /* semaphore */
    DEVDRV_ERROR,     /* hi2c1 */

};

static int32_t RIIC_CH1_REG24_Write(const uint8_t riic_addr, const uint32_t reg_addr, const uint8_t *reg_data);
static int32_t RIIC_CH1_REG24_Read(const uint8_t riic_addr, const uint32_t reg_addr, uint8_t* const p_reg_data);

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/


/**************************************************************************//**
* Function Name: riic_dae6_Open
* @brief         Open riic channel 1
*                Description:<br>
*                Open RIIC channel 1 for access to i2c devices
* @param         none
* @retval        DEVDRV_SUCCESS    : Success
* @retval        error code  : Failure
******************************************************************************/
int32_t riic_dae6_Open(void)
{
    int_t ercd = DEVDRV_ERROR;
    st_r_drv_riic_create_t riic_clock;

    /* open the I2C channel1 driver */
    s_i2c1_ctrl.hi2c1 = open(DEVICE_INDENTIFIER "iic0", O_RDWR);

    if (DEVDRV_ERROR == s_i2c1_ctrl.hi2c1)
    {
        ercd = DEVDRV_ERROR;
    }
    else
    {
        /* set the clock frequency for the I2C channel */
        riic_clock.frequency = RIIC_FREQUENCY_100KHZ;
        riic_clock.subAddr_bytes = 3;

        /* Create the I2C channel */
        ercd = control(s_i2c1_ctrl.hi2c1, CTL_RIIC_CREATE, &riic_clock);
    }

    return (ercd);
}
/******************************************************************************
End of function riic_dae6_Open
******************************************************************************/

/**************************************************************************//**
* Function Name: riic_dae6_Close
* @brief         Close riic channel 1
*                Description:<br>
*                Close RIIC channel 1 for access to i2c devices
* @param         none
* @retval        DEVDRV_SUCCESS    : Success
* @retval        error code  : Failure
******************************************************************************/
int32_t riic_dae6_Close(void)
{
    int_t ercd = DEVDRV_SUCCESS;

    /*Close Channel*/
    ercd = close(s_i2c1_ctrl.hi2c1);
    s_i2c1_ctrl.hi2c1 = DEVDRV_ERROR;

    return (ercd);
}

/**************************************************************************//**
* Function Name: riic_dae6_Write
* @brief         Write register to MAX9856
*
*                Description:<br>
*
* @param         addr       :register address 0-28
* @param         data       :register data 0-255
* @retval        DEVDRV_SUCCESS   :Success.
* @retval        error code :Failure.
******************************************************************************/
int32_t riic_dae6_Write(const uint32_t addr, const uint8_t *data)
{
    int_t ercd;

    if (addr > DAE6_REG_MAX)
    {
        ercd = DEVDRV_ERROR;
    }
    else
    {
        ercd = RIIC_CH1_REG24_Write(DAE6_DEVICE_ADDR, addr, data);
    }

    return (ercd);
}
/******************************************************************************
End of function riic_dae6_Write
******************************************************************************/

/**************************************************************************//**
* Function Name: riic_dae6_Read
* @brief         Read register from MAX9856
*
*                Description:<br>
*
* @param         addr       :register address 0-28
* @param         p_data     :store address of register data 0-255
* @retval        DEVDRV_SUCCESS   :Success.
* @retval        error code :Failure.
******************************************************************************/
int32_t riic_dae6_Read(const uint32_t addr, uint8_t* const p_data)
{
    int_t ercd;

    if (addr > DAE6_REG_MAX)
    {
        ercd = DEVDRV_ERROR;
    }
    else if (NULL == p_data)
    {
        ercd = DEVDRV_ERROR;
    }
    else
    {
        ercd = RIIC_CH1_REG24_Read(DAE6_DEVICE_ADDR, addr, p_data);
    }

    return (ercd);
}
/******************************************************************************
End of function riic_dae6_Read
******************************************************************************/


/**************************************************************************//**
* Function Name: RIIC_CH1_REG8_Write
* @brief         Write data to target device register.
*
*                Description:<br>
*
* @param[in]     riic_addr       : riic 8bit address 0-255
* @param[in]     reg_addr        : register address  0-255
* @param[in]     reg_data        : register data     0-255
* @retval        DEVDRV_SUCCESS   :Success.
* @retval        error code :Failure.
******************************************************************************/
static int32_t RIIC_CH1_REG24_Write(const uint8_t riic_addr, const uint32_t reg_addr, const uint8_t *reg_data)
{
    int32_t riic_ret = DEVDRV_ERROR;
    st_r_drv_riic_config_t i2c_write;

    uint8_t sub_addr[3];

	// Big Endian
    sub_addr[0] = (uint8_t)((reg_addr >> 16) & 0x000000FF);
   sub_addr[1] = (uint8_t)((reg_addr >> 8) & 0x000000FF);
   sub_addr[2] = (uint8_t)(reg_addr & 0x000000FF);

    /*Set RIIC Address*/
    i2c_write.device_address = riic_addr;
    i2c_write.sub_address = &sub_addr[0];

    /* Assign Data to Write */
    i2c_write.number_of_bytes = 3u;
    i2c_write.p_data_buffer = reg_data;

    /*Write Data*/
    riic_ret = control(s_i2c1_ctrl.hi2c1, CTL_RIIC_WRITE, &i2c_write);

    return (riic_ret);
}
/******************************************************************************
End of function RIIC_CH1_REG8_Write
******************************************************************************/

/**************************************************************************//**
* Function Name: RIIC_CH1_REG8_Read
* @brief         Read data from target device register.
*
*                Description:<br>
*
* @param[in]     riic_addr       : riic 8bit address 0-255
* @param[in]     reg_addr        : register address  0-255
* @param[in,out] p_reg_data      : store address of register data 0-255
* @retval        DEVDRV_SUCCESS   :Success.
* @retval        error code :Failure.
******************************************************************************/
static int32_t RIIC_CH1_REG24_Read(const uint8_t riic_addr, const uint32_t reg_addr, uint8_t* const p_reg_data)
{
    int32_t riic_ret= DEVDRV_ERROR;
    st_r_drv_riic_config_t i2c_read;
    uint8_t sub_addr[3];

    /// Big Endian
    sub_addr[0] = (uint8_t)((reg_addr >> 16) & 0x000000FF);
    sub_addr[1] = (uint8_t)((reg_addr >> 8) & 0x000000FF);
    sub_addr[2] = (uint8_t)(reg_addr & 0x000000FF);

    /*Set RIIC address*/
    i2c_read.device_address = riic_addr;
    i2c_read.sub_address = &sub_addr[0];

    /*Set location to read to*/
    i2c_read.number_of_bytes = 3u;
    i2c_read.p_data_buffer = p_reg_data;

    riic_ret = control(s_i2c1_ctrl.hi2c1, CTL_RIIC_READ, &i2c_read);

    return (riic_ret);
}
