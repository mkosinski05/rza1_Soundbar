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
 * File Name     : spibsc_ioset_api.c
 * Version       : 1.00
 * Device(s)     : RZ/A1L
 * Tool-Chain    : GNUARM-NONE-EABI-v16.01
 * OS            : None
 * H/W Platform  : Stream it! v2
 * Description   : SPI Flash I/O access setup routine APIs
 *******************************************************************************/
/*******************************************************************************
 * History       : DD.MM.YYYY Version Description
 *               : 24.08.16   1.00    First Release
 *******************************************************************************/

/******************************************************************************
 Includes <System Includes> , "Project Includes"
 ******************************************************************************/
#include "typedefine.h"
#include "spibsc.h"
#include "r_spibsc_ioset_api.h"
#include "rza_io_regrw.h"
#include "iodefine.h"
#include "spibsc_iobitmask.h"

/******************************************************************************
 Typedef definitions
 ******************************************************************************/
extern volatile struct st_spibsc* SPIBSC[SPIBSC_COUNT];

/******************************************************************************
 Macro definitions
 ******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
extern int32_t Userdef_SFLASH_Set_Mode(uint32_t ch_no, uint32_t dual, en_sf_req_t req, uint8_t data_width, uint8_t addr_mode);


/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/


/******************************************************************************
Private global variables and functions
******************************************************************************/
static int32_t SPIBSC_Exread_Mode_Config(uint32_t ch_no, st_spibsc_cfg_t *spibsccfg);


/******************************************************************************
* Function Name: R_SFLASH_Exmode
* Description  :
* Arguments    : uint32_t ch_no
* Return Value :  0 : success
*                -1 : error
******************************************************************************/
int32_t R_SFLASH_Exmode(uint32_t ch_no)
{
    int32_t ret;

    ret = spibsc_exmode(ch_no);

    return ret;

} /* End of function R_SFLASH_Exmode() */

/******************************************************************************
* Function Name: R_SFLASH_Spimode
* Description  :
* Arguments    : uint32_t ch_no
* Return Value :  0 : success
*                -1 : error
******************************************************************************/
int32_t R_SFLASH_Spimode(uint32_t ch_no)
{
    int32_t ret;

    ret = spibsc_spimode(ch_no);

    return ret;
}

/******************************************************************************
* Function Name: R_SFLASH_SpibscStop
* Description  :
* Arguments    : uint32_t ch_no
* Return Value :  0 : success
*                -1 : error
******************************************************************************/
int32_t R_SFLASH_SpibscStop(uint32_t ch_no)
{
    int32_t ret;

    ret = spibsc_stop(ch_no);

    return ret;
}

/******************************************************************************
* Function Name: R_SFLASH_Spimode_Init
* Description  : Initialization in a basic part of a SPI multi-I/O bus controller.
*              : Serial flash memory is set as a Quad operation mode.
* Arguments    : uint32_t ch_no : use channel No
*                uint32_t dual
*                uint8_t  data_width
*                uint8_t  spbr
*                uint8_t  brdv
*                uint8_t  addr_mode
* Return Value :  0 : success
*                -1 : error
******************************************************************************/
int32_t R_SFLASH_Spimode_Init(uint32_t ch_no, uint32_t dual, uint8_t data_width, uint8_t spbr, uint8_t brdv, uint8_t addr_mode)
{
    int32_t ret;
    en_sf_req_t req;

    /* ==== Initialization of SPIBSC ==== */
    ret = spibsc_common_init(ch_no, dual, spbr, brdv, data_width);
    if(ret !=0)
    {
        return ret;
    }

    /* ==== setting serial-flash quad mode ==== */
    if(data_width == SPIBSC_1BIT)
    {
        req = SF_REQ_SERIALMODE;
    }
    else if(data_width == SPIBSC_4BIT)
    {
        req = SF_REQ_QUADMODE;
    }
    else
    {
        return -1;
    }

    ret = Userdef_SFLASH_Set_Mode(ch_no, dual, req, data_width, addr_mode);

    return ret;

} /* End of function R_SFLASH_Spimode_Init() */

/******************************************************************************
* Function Name: R_SFLASH_Exmode_Init
* Description  : An outside address space read mode set as a SPI multi-I/O bus
*                controller.
* Arguments    : uint32_t ch_no : use channel No
*                uint32_t dual
*                st_spibsc_cfg_t *spibsccfg
* Return Value :  0 : success
*                -1 : error
******************************************************************************/
int32_t R_SFLASH_Exmode_Init(uint32_t ch_no, uint32_t dual, st_spibsc_cfg_t *spibsccfg)
{
    int32_t ret;

    if(ch_no > 1)
    {
        return -1;
    }

    if( dual == SPIBSC_CMNCR_BSZ_SINGLE )
    {
        ret = spibsc_bsz_set(ch_no, SPIBSC_CMNCR_BSZ_SINGLE, spibsccfg->udef_data_width);
    }
    else
    {
        ret = spibsc_bsz_set(ch_no, SPIBSC_CMNCR_BSZ_DUAL,   spibsccfg->udef_data_width);
    }
    if(ret != 0)
    {
        return ret;
    }

    ret = spibsc_dr_init(ch_no, spibsccfg);

    return ret;

} /* End of function R_SFLASH_Exmode_Init() */

/******************************************************************************
* Function Name: R_SFLASH_Exmode_Setting
* Description  : Initialization in a basic part of a SPI multi-I/O bus controller.
*              : Serial flash memory is set as a Quad operation mode.
* Arguments    : uint32_t ch_no : use channel No
*                uint32_t dual
*                st_spibsc_cfg_t *spibsccfg
* Return Value :  0 : success
*                -1 : error
******************************************************************************/
int32_t R_SFLASH_Exmode_Setting(uint32_t ch_no, uint32_t dual, st_spibsc_cfg_t *spibsccfg)
{
    int32_t ret;
    en_sf_req_t req;

    /* ==== check of SPIBSC register ==== */
    ret = SPIBSC_Exread_Mode_Config(ch_no, spibsccfg);
    if(ret != 0)
    {
        return ret;
    }

    /* ==== Initialization of SPIBSC ==== */
    ret = spibsc_common_init(ch_no,
                             dual,
                             spibsccfg->udef_spbr,
                             spibsccfg->udef_brdv,
                             spibsccfg->udef_data_width);


    if(spibsccfg->udef_speed_mode == SPIBSC_DDR_TRANS)
    {
        /* even edge : read */
        rza_io_reg_write_32(&SPIBSC[ ch_no ]->CMNCR, SPIBSC_CMNCR_CPHAR_ODD, SPIBSC_CMNCR_CPHAR_SHIFT, SPIBSC_CMNCR_CPHAR);
    }

    if(ret != 0)
    {
        return ret;
    }

    /* ==== setting serial-flash quad mode ==== */
    if(spibsccfg->udef_data_width == SPIBSC_1BIT)
    {
        req = SF_REQ_SERIALMODE;
    }
    else if(spibsccfg->udef_data_width == SPIBSC_4BIT)
    {
        req = SF_REQ_QUADMODE;
    }
    else
    {
        return -1;
    }

    ret = Userdef_SFLASH_Set_Mode(ch_no, dual, req, spibsccfg->udef_data_width, spibsccfg->udef_addr_mode);
    if(ret != 0)
    {
        return ret;
    }

    ret = R_SFLASH_Exmode_Init(ch_no, dual, spibsccfg);

    return ret;

}
/*******************************************************************************
End of function R_SFLASH_Exmode_Setting
*******************************************************************************/

/******************************************************************************
* Function Name: R_SFLASH_WaitTend
* Description  : Wait TEND
* Arguments    :  uint32_t ch_no : use channel No
* Return Value :  void
******************************************************************************/
void R_SFLASH_WaitTend(uint32_t ch_no)
{
    spibsc_wait_tend(ch_no);

}
/*******************************************************************************
End of function R_SFLASH_WaitTend
*******************************************************************************/

/******************************************************************************
* Function Name: Userdef_SPIBSC_Set_Config
* Description  : The setting function of SPIBSC.
* Arguments    : uint32_t    ch_no
*                st_spibsc_cfg_t *spibsccfg
* Return Value :  0 : success
*                -1 : error
******************************************************************************/
int32_t R_SFLASH_Set_Config(uint32_t ch_no, st_spibsc_cfg_t *spibsccfg)
{
    int32_t ret;

    Userdef_SPIBSC_Set_Config(ch_no, spibsccfg);
    ret = SPIBSC_Exread_Mode_Config(ch_no, spibsccfg);

    return ret;
}
/*******************************************************************************
End of function R_SFLASH_Set_Config
*******************************************************************************/

/******************************************************************************
* Function Name: SPIBSC_Exread_Mode_Config
* Description  : Called the configuration function
* Arguments    :  uint32_t ch_no : use channel No
*                 st_spibsc_cfg_t *spibsccfg
* Return Value :  0 : success
*                -1 : error
******************************************************************************/
static int32_t SPIBSC_Exread_Mode_Config(uint32_t ch_no, st_spibsc_cfg_t *spibsccfg)
{
    if( ch_no == 0 )
    {
        if((spibsccfg->udef_cmd_width > 2)    || (spibsccfg->udef_addr_width > 2)     ||
           (spibsccfg->udef_opd_enable > 15)  || (spibsccfg->udef_opd_width > 2)      ||
           (spibsccfg->udef_dmycyc_num > 7)   || (spibsccfg->udef_dmycyc_enable > 1)  ||
           (spibsccfg->udef_dmycyc_width > 2) || (spibsccfg->udef_data_width > 2)     ||
           (spibsccfg->udef_brdv > 3))
        {
            return -1;
        }
    }
    else
    {
        if((spibsccfg->udef_cmd_width > 2)    || (spibsccfg->udef_addr_width > 2)     ||
           (spibsccfg->udef_opd_enable > 15 ) || (spibsccfg->udef_opd_width > 2)      ||
           (spibsccfg->udef_dmycyc_num > 7)   || (spibsccfg->udef_dmycyc_enable > 1)  ||
           (spibsccfg->udef_dmycyc_width > 2) || (spibsccfg->udef_data_width > 2)     ||
           (spibsccfg->udef_brdv > 3))
        {
            return -1;
        }
    }

    return 0;

}
/*******************************************************************************
End of function SPIBSC_Exread_Mode_Config
*******************************************************************************/

/*******************************************************************************
End of file
*******************************************************************************/
