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
* @file         r_riic_dae6_if.c
* Rev: 
* Date:: 2020
* @brief        
******************************************************************************/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "mcu_board_select.h"
#include "r_errno.h"
#include "r_os_abstraction_api.h"
#include "r_riic_dae6_if.h"
#include "dev_drv.h"

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef struct riic_ch1_ctrl
{
    bool_t           is_open;
    uint32_t         semid;
} riic_ch1_ctrl_t;

/******************************************************************************
Macro definitions
******************************************************************************/
#define RIIC_CH1_API_TMOUT (500u)

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static riic_ch1_ctrl_t s_riic1_ctrl =
{
    false,  /* is_open */
    0       /* semid   */
};
/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/

/**
 * @brief   Open RIIC CH1 and initialize synchronous resource
 *
 * @retval        DEVDRV_SUCCESS: Success.
 * @retval        error code:     Failure.
 */
int32_t r_riic_dae6_Open(void)
{
    int32_t ercd = DEVDRV_SUCCESS;

    if (false != s_riic1_ctrl.is_open)
    {
        ercd = DEVDRV_ERROR;
    }
    else
    {
        R_OS_CreateSemaphore(&s_riic1_ctrl.semid, 1);

        if (0 == s_riic1_ctrl.semid)
        {
            ercd = DEVDRV_ERROR;
        }

        if (DEVDRV_SUCCESS == ercd)
        {
            ercd = riic_dae6_Open();
        }

        if (DEVDRV_SUCCESS == ercd)
        {
            s_riic1_ctrl.is_open = true;
        }
        else
        {
            s_riic1_ctrl.is_open = false;

            if (0 != s_riic1_ctrl.semid)
            {
                R_OS_DeleteSemaphore(&s_riic1_ctrl.semid);
            }
        }
    }

    return (ercd);
}

/**
 * @brief    Close RIIC CH1 and free synchronous resource
 *
 * @retval        DEVDRV_SUCCESS: Success.
 * @retval        error code:     Failure.
 */
int32_t r_riic_dae6_Close(void)
{
    int32_t ercd = DEVDRV_SUCCESS;
    bool_t sem_token = false;

    if (false == s_riic1_ctrl.is_open)
    {
        ercd = DEVDRV_ERROR;
    }
    else
    {
        sem_token = R_OS_WaitForSemaphore(&s_riic1_ctrl.semid, RIIC_CH1_API_TMOUT);

        if (!sem_token)
        {
            ercd = DEVDRV_ERROR;
        }
        else
        {
            ercd = riic_dae6_Close();

            if (DEVDRV_SUCCESS == ercd)
            {
                s_riic1_ctrl.is_open = false;
                R_OS_DeleteSemaphore(&s_riic1_ctrl.semid);
            }
        }
    }

    return (ercd);
}
/**
 * @brief         Write register to MAX9856
 *
 * @param[in]     addr:  Register address 0-28
 * @param[in]     dat:   Register data 0-255
 *
 * @retval        DEVDRV_SUCCESS:   Success.
 * @retval        error code:       Failure.
 */
int32_t r_riic_dae6_Write(const uint32_t addr, const uint8_t *dat)
{
    int32_t ercd = DEVDRV_SUCCESS;
    bool_t sem_token;

    if (false == s_riic1_ctrl.is_open)
    {
        ercd = DEVDRV_ERROR;
    }
    else
    {
        sem_token = R_OS_WaitForSemaphore(&s_riic1_ctrl.semid, RIIC_CH1_API_TMOUT);
        if (!sem_token)
        {
            ercd = DEVDRV_ERROR;
        }
        else
        {
            ercd = riic_dae6_Write(addr, dat);
            R_OS_ReleaseSemaphore(&s_riic1_ctrl.semid);
        }
    }

    return (ercd);
}
/**
 * @brief         Read register from MAX9856
 *
 * @param[in]     addr:    register address 0-28
 * @param[out]    p_dat:   store address of register data 0-255
 *
 * @retval        DEVDRV_SUCCESS:  Success.
 * @retval        error code:      Failure.
 */
int32_t r_riic_dae6_Read(const uint32_t addr, uint8_t* const p_dat)
{
    int32_t ercd = DEVDRV_SUCCESS;
    bool_t sem_token;

    if (false == s_riic1_ctrl.is_open)
    {
        ercd = DEVDRV_ERROR;
    }
    else
    {
        sem_token = R_OS_WaitForSemaphore(&s_riic1_ctrl.semid, RIIC_CH1_API_TMOUT);

        if (!sem_token)
        {
            ercd = DEVDRV_ERROR;
        }
        else
        {
            ercd = riic_dae6_Read(addr, p_dat);
            R_OS_ReleaseSemaphore(&s_riic1_ctrl.semid);
        }
    }

    return (ercd);
}
/**
 * @brief         Get RIIC_CH1 driver version.

 * @retval        driver version:
 *                    upper 8bit=MAJOR
 *                    lower 8bit=MINOR
 */
#if 0
uint16_t r_riic_dae6_GetVersion(void)
{
    const uint16_t version =
        ((RIIC_MAX9856_DRV_VER_MAJOR & RIIC_MAX9856_DRV_VER_MASK) << RIIC_MAX9856_DRV_VER_SHIFT)
        | (RIIC_MAX9856_DRV_VER_MINOR & RIIC_MAX9856_DRV_VER_MASK);

    return version;
}
#endif
