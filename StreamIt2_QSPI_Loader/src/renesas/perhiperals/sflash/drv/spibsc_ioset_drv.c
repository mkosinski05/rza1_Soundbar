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
 * File Name     : spibsc_ioset_drv.c
 * Version       : 1.00
 * Device(s)     : RZ/A1L
 * Tool-Chain    : GNUARM-NONE-EABI-v16.01
 * OS            : None
 * H/W Platform  : Stream it! v2
 * Description   : SPI Flash I/O access routines - called from API Layer
 *******************************************************************************/
/*******************************************************************************
 * History       : DD.MM.YYYY Version Description
 *               : 24.08.16   1.00    First Release
 *******************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "typedefine.h"
#include "iodefine.h"
#include "rza_io_regrw.h"
#include "spibsc.h"
#include "r_spibsc_ioset_api.h"
#include "spibsc_iobitmask.h"

/******************************************************************************
 Typedef definitions
 ******************************************************************************/
/* Note: Below may create a 'braces around scalar initializer' build warning if SPIBSC_COUNT = 0
 * this is normal behaviour */
volatile struct st_spibsc* SPIBSC[SPIBSC_COUNT] =
{
 SPIBSC_ADDRESS_LIST
};

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
static int32_t Pekoe_io_spibsc_port_setting(int_t data_bus_width, uint32_t bsz);


/******************************************************************************
* Function Name: spibsc_bsz_set
* Description  : Set a data bus width of a SPI multi-I/O bus controller.
* Arguments    : uint32_t ch_no : use channel No
*              : uint32_t bsz : BSZ bit
*              : uint8_t data_witdh
* Return Value :  0 : success
*                -1 : error
******************************************************************************/
int32_t  spibsc_bsz_set(uint32_t ch_no, uint32_t bsz, uint8_t data_width)
{
    if(ch_no > 1 )
    {
        return -1;
    }

    if(rza_io_reg_read_32(&SPIBSC[ ch_no ]->CMNSR, SPIBSC_CMNSR_TEND_SHIFT, SPIBSC_CMNSR_TEND) != SPIBSC_TRANS_END)
    {
        return -1;
    }

    if (rza_io_reg_read_32(&SPIBSC[ ch_no ]->CMNCR, SPIBSC_CMNCR_BSZ_SHIFT, SPIBSC_CMNCR_BSZ) != bsz)
    {
        if (bsz == SPIBSC_CMNCR_BSZ_DUAL)
        {
            /* s-flash x 2 (4bit x 2) */
            if( Pekoe_io_spibsc_port_setting(data_width, bsz) < 0 )
            {
                return -1;
            }
        }

        rza_io_reg_write_32(&SPIBSC[ ch_no ]->CMNCR, bsz, SPIBSC_CMNCR_BSZ_SHIFT, SPIBSC_CMNCR_BSZ);
        rza_io_reg_write_32(&SPIBSC[ ch_no ]->DRCR, SPIBSC_DRCR_RCF_EXE, SPIBSC_DRCR_RCF_SHIFT, SPIBSC_DRCR_RCF);
    }

    return 0;

} /* End of function  spibsc_bsz_set() */


/******************************************************************************
* Function Name: spibsc_common_init
* Description  : Initialize the operation mode independent part of a SPI 
*                multi-I/O bus controller.
* Arguments    : uint32_t ch_no : use channel No
*                uint32_t bsz : BSZ bit
*                uint8_t spbr
*                uint8_t brdv
*                uint8_t data_width
* Return Value :  0 : success
*                -1 : error
******************************************************************************/
int32_t spibsc_common_init(uint32_t ch_no, uint32_t bsz, uint8_t spbr, uint8_t brdv, uint8_t data_width)
{
    if(ch_no > 1 )
    {
        return -1;
    }

    /* PORT setting of SPIBSC */
    if( Pekoe_io_spibsc_port_setting( data_width, bsz) < 0)
    {
        return -1;
    }

    if(rza_io_reg_read_32(&SPIBSC[ ch_no ]->CMNSR, SPIBSC_CMNSR_TEND_SHIFT, SPIBSC_CMNSR_TEND) != SPIBSC_TRANS_END)
    {
        return -1;
    }

#if F_HIGH /* Pin status at the time of an idol?FHIGH */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->CMNCR, SPIBSC_OUTPUT_HIGH, SPIBSC_CMNCR_MOIIO3_SHIFT, SPIBSC_CMNCR_MOIIO3);
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->CMNCR, SPIBSC_OUTPUT_HIGH, SPIBSC_CMNCR_MOIIO2_SHIFT, SPIBSC_CMNCR_MOIIO2);
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->CMNCR, SPIBSC_OUTPUT_HIGH, SPIBSC_CMNCR_MOIIO1_SHIFT, SPIBSC_CMNCR_MOIIO1);
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->CMNCR, SPIBSC_OUTPUT_HIGH, SPIBSC_CMNCR_MOIIO0_SHIFT, SPIBSC_CMNCR_MOIIO0);

    rza_io_reg_write_32(&SPIBSC[ ch_no ]->CMNCR, SPIBSC_OUTPUT_HIGH, SPIBSC_CMNCR_IO3FV_SHIFT, SPIBSC_CMNCR_IO3FV);
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->CMNCR, SPIBSC_OUTPUT_HIGH, SPIBSC_CMNCR_IO2FV_SHIFT, SPIBSC_CMNCR_IO2FV);
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->CMNCR, SPIBSC_OUTPUT_HIGH, SPIBSC_CMNCR_IO0FV_SHIFT, SPIBSC_CMNCR_IO0FV);
#else /* Pin status at the time of an idol?FHiZ */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->CMNCR, SPIBSC_OUTPUT_HiZ, SPIBSC_CMNCR_MOIIO3_SHIFT, SPIBSC_CMNCR_MOIIO3);
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->CMNCR, SPIBSC_OUTPUT_HiZ, SPIBSC_CMNCR_MOIIO2_SHIFT, SPIBSC_CMNCR_MOIIO2);
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->CMNCR, SPIBSC_OUTPUT_HiZ, SPIBSC_CMNCR_MOIIO1_SHIFT, SPIBSC_CMNCR_MOIIO1);
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->CMNCR, SPIBSC_OUTPUT_HiZ, SPIBSC_CMNCR_MOIIO0_SHIFT, SPIBSC_CMNCR_MOIIO0);

    rza_io_reg_write_32(&SPIBSC[ ch_no ]->CMNCR, SPIBSC_OUTPUT_HiZ, SPIBSC_CMNCR_IO3FV_SHIFT, SPIBSC_CMNCR_IO3FV);
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->CMNCR, SPIBSC_OUTPUT_HiZ, SPIBSC_CMNCR_IO2FV_SHIFT, SPIBSC_CMNCR_IO2FV);
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->CMNCR, SPIBSC_OUTPUT_HiZ, SPIBSC_CMNCR_IO0FV_SHIFT, SPIBSC_CMNCR_IO0FV);
#endif

    /* swap by 8-bit unit(Defaults) */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->CMNCR, 1, SPIBSC_CMNCR_SFDE_SHIFT, SPIBSC_CMNCR_SFDE);

    /* S-flash mode 0 */
    /* even edge : write */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->CMNCR, SPIBSC_CMNCR_CPHAT_EVEN, SPIBSC_CMNCR_CPHAT_SHIFT, SPIBSC_CMNCR_CPHAT);

    /* even edge : read */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->CMNCR, SPIBSC_CMNCR_CPHAR_EVEN, SPIBSC_CMNCR_CPHAR_SHIFT, SPIBSC_CMNCR_CPHAR);

    /* SPBSSL   : low active */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->CMNCR, SPIBSC_CMNCR_SSLP_LOW, SPIBSC_CMNCR_SSLP_SHIFT, SPIBSC_CMNCR_SSLP);

    /* SPBCLK   : low at negate */    
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->CMNCR, SPIBSC_CMNCR_CPOL_LOW, SPIBSC_CMNCR_CPOL_SHIFT, SPIBSC_CMNCR_CPOL);

    spibsc_bsz_set(ch_no, bsz, data_width);

    /* next access delay */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->SSLDR, SPIBSC_DELAY_1SPBCLK, SPIBSC_SSLDR_SPNDL_SHIFT, SPIBSC_SSLDR_SPNDL);

    /* SPBSSL negate delay */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->SSLDR, SPIBSC_DELAY_1SPBCLK, SPIBSC_SSLDR_SLNDL_SHIFT, SPIBSC_SSLDR_SLNDL);

    /* clock delay */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->SSLDR, SPIBSC_DELAY_1SPBCLK, SPIBSC_SSLDR_SCKDL_SHIFT, SPIBSC_SSLDR_SCKDL);

    /* ---- Bit rate Setting ---- */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->SPBCR, spbr, SPIBSC_SPBCR_SPBR_SHIFT, SPIBSC_SPBCR_SPBR);
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->SPBCR, brdv, SPIBSC_SPBCR_BRDV_SHIFT, SPIBSC_SPBCR_BRDV);

    SPIBSC[ ch_no ]->CKDLY  = SPIBSC_CKDLY_SETTING;
    SPIBSC[ ch_no ]->SPODLY = SPIBSC_SPODLY_SETTING;

    return 0;

}
/*******************************************************************************
End of function spibsc_common_init
*******************************************************************************/

/******************************************************************************
* Function Name: spibsc_wait_tend
* Description  : Wait TEND
* Arguments    : uint32_t ch_no : use channel No
* Return Value : none
******************************************************************************/
void spibsc_wait_tend(uint32_t ch_no)
{
    while(rza_io_reg_read_32(&SPIBSC[ ch_no ]->CMNSR, SPIBSC_CMNSR_TEND_SHIFT, SPIBSC_CMNSR_TEND) != SPIBSC_TRANS_END)
    {
        /* wait for transfer-end */
    }

} /* End of function spibsc_wait_tend() */

/******************************************************************************
* Function Name: spibsc_dr_init
* Description  : The setting which makes a SPI multi-I/O bus controller activate
*                an outside address space read mode.
* Arguments    : uint32_t ch_no : use channel No
*              : st_spibsc_cfg_t *spibsccfg
* Return Value :  0 : success
*                -1 : error
******************************************************************************/
int32_t spibsc_dr_init(uint32_t ch_no, st_spibsc_cfg_t *spibsccfg)
{
    if(ch_no > 1 )
    {
        return -1;
    }

    if(rza_io_reg_read_32(&SPIBSC[ ch_no ]->CMNSR, SPIBSC_CMNSR_TEND_SHIFT, SPIBSC_CMNSR_TEND) != SPIBSC_TRANS_END)
    {
        return -1;
    }

    /* SPI I/O mode */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->CMNCR, SPIBSC_CMNCR_MD_EXTRD, SPIBSC_CMNCR_MD_SHIFT, SPIBSC_CMNCR_MD);

    rza_io_reg_write_32(&SPIBSC[ ch_no ]->DRCR, SPIBSC_BURST_4, SPIBSC_DRCR_RBURST_SHIFT, SPIBSC_DRCR_RBURST);
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->DRCR, SPIBSC_BURST_ENABLE, SPIBSC_DRCR_RBE_SHIFT, SPIBSC_DRCR_RBE);

    /* Keep SSL after read if not continuous address it negated */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->DRCR, SPIBSC_SPISSL_KEEP, SPIBSC_DRCR_SSLE_SHIFT, SPIBSC_DRCR_SSLE);

    /* ---- Command ---- */
    /* Command */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->DRCMR, spibsccfg->udef_cmd, SPIBSC_DRCMR_CMD_SHIFT, SPIBSC_DRCMR_CMD);

    /* Single */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->DRENR, spibsccfg->udef_cmd_width, SPIBSC_DRENR_CDB_SHIFT, SPIBSC_DRENR_CDB);

    /* Enable */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->DRENR, SPIBSC_OUTPUT_ENABLE, SPIBSC_DRENR_CDE_SHIFT, SPIBSC_DRENR_CDE);

    /* ---- Option Command ---- */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->DRCMR, 0x00, SPIBSC_DRCMR_OCMD_SHIFT, SPIBSC_DRCMR_OCMD);

    /* Single */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->DRENR, SPIBSC_1BIT, SPIBSC_DRENR_OCDB_SHIFT, SPIBSC_DRENR_OCDB);

    /* Disable */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->DRENR, SPIBSC_OUTPUT_DISABLE, SPIBSC_DRENR_OCDE_SHIFT, SPIBSC_DRENR_OCDE);

    /* ---- Address ---- */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->DRENR, spibsccfg->udef_addr_width, SPIBSC_DRENR_ADB_SHIFT, SPIBSC_DRENR_ADB);
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->DRENR, spibsccfg->udef_addr_mode,  SPIBSC_DRENR_ADE_SHIFT, SPIBSC_DRENR_ADE);

    /* EAV */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->DREAR, 0, SPIBSC_DREAR_EAV_SHIFT, SPIBSC_DREAR_EAV);

    /* ---- Option Data ---- */
    /* Option Data */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->DROPR, spibsccfg->udef_opd3, SPIBSC_DROPR_OPD3_SHIFT, SPIBSC_DROPR_OPD3);

    /* Option Data */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->DROPR, spibsccfg->udef_opd2, SPIBSC_DROPR_OPD2_SHIFT, SPIBSC_DROPR_OPD2);

    /* Option Data */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->DROPR, spibsccfg->udef_opd1, SPIBSC_DROPR_OPD1_SHIFT, SPIBSC_DROPR_OPD1);

    /* Option Data */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->DROPR, spibsccfg->udef_opd0, SPIBSC_DROPR_OPD0_SHIFT, SPIBSC_DROPR_OPD0);

    /* Single */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->DRENR, spibsccfg->udef_opd_width, SPIBSC_DRENR_OPDB_SHIFT, SPIBSC_DRENR_OPDB);

    /* Disable */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->DRENR, spibsccfg->udef_opd_enable, SPIBSC_DRENR_OPDE_SHIFT, SPIBSC_DRENR_OPDE);

    /* ---- Data ---- */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->DRENR, spibsccfg->udef_data_width, SPIBSC_DRENR_DRDB_SHIFT, SPIBSC_DRENR_DRDB);

    /* dummycycle enable */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->DRENR, spibsccfg->udef_dmycyc_enable, SPIBSC_DRENR_DME_SHIFT, SPIBSC_DRENR_DME);

    /* Set data read dummycycle */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->DRDMCR, spibsccfg->udef_dmycyc_width, SPIBSC_DRDMCR_DMDB_SHIFT, SPIBSC_SMDMCR_DMDB);
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->DRDMCR, spibsccfg->udef_dmycyc_num, SPIBSC_DRDMCR_DMCYC_SHIFT, SPIBSC_DRDMCR_DMCYC);

    /* address:SDR/DDR */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->DRDRENR, spibsccfg->udef_speed_mode, SPIBSC_DRDRENR_ADDRE_SHIFT, SPIBSC_DRDRENR_ADDRE);

    /* option data:SDR/DDR */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->DRDRENR, spibsccfg->udef_speed_mode, SPIBSC_DRDRENR_OPDRE_SHIFT, SPIBSC_DRDRENR_OPDRE);

    /* data read: SDR/DDR */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->DRDRENR, spibsccfg->udef_speed_mode, SPIBSC_DRDRENR_DRDRE_SHIFT, SPIBSC_DRDRENR_DRDRE);


    return 0;

} /* End of function spibsc_dr_init() */

/*******************************************************************************
* Function Name: spibsc_exmode
* Description  : Sets SPIBSC_CMNCR_MD_EXTRD in register CMNCR for specified
*                channel, if SPIBSC_CMNCR_MD_EXTRD is allready set do nothing
* Arguments    : uint32_t ch_no
* Return Value :  0 : success
*              : -1 : error
*******************************************************************************/
int32_t spibsc_exmode(uint32_t ch_no)
{
    if(rza_io_reg_read_32(&SPIBSC[ ch_no ]->CMNCR, SPIBSC_CMNCR_MD_SHIFT, SPIBSC_CMNCR_MD) != SPIBSC_CMNCR_MD_EXTRD)
    {
        if(spibsc_stop(ch_no) < 0)
        {
            return -1;
        }

        rza_io_reg_write_32(&SPIBSC[ ch_no ]->CMNCR, SPIBSC_CMNCR_MD_EXTRD, SPIBSC_CMNCR_MD_SHIFT, SPIBSC_CMNCR_MD);
    }

    return 0;

} /* End of function spibsc_exmode() */

/*******************************************************************************
* Function Name: int32_t spibsc_spimode(void);
* Description  :
* Arguments    : uint32_t ch_no
* Return Value :  0 :
*              : -1 : error
*******************************************************************************/
int32_t spibsc_spimode(uint32_t ch_no)
{
    if(rza_io_reg_read_32(&SPIBSC[ ch_no ]->CMNCR, SPIBSC_CMNCR_MD_SHIFT, SPIBSC_CMNCR_MD) != SPIBSC_CMNCR_MD_SPI)
    {
        if(spibsc_stop(ch_no) < 0)
        {
            return -1;
        }

        /* SPI Mode */
        rza_io_reg_write_32(&SPIBSC[ ch_no ]->CMNCR, SPIBSC_CMNCR_MD_SPI, SPIBSC_CMNCR_MD_SHIFT, SPIBSC_CMNCR_MD);
    }

    return 0;

} /* End of function spibsc_spimode() */

/*******************************************************************************
* Function Name: int32_t spibsc_stop(void);
* Description  :
* Arguments    : uint32_t ch_no
* Return Value :  0 :
*              : -1 : error
*******************************************************************************/
int32_t spibsc_stop(uint32_t ch_no)
{
    if(ch_no > 1 )
    {
        return -1;
    }

    rza_io_reg_write_32(&SPIBSC[ ch_no ]->DRCR, 1, SPIBSC_DRCR_SSLN_SHIFT, SPIBSC_DRCR_SSLN);
    while(1)
    {
        if(rza_io_reg_read_32(&SPIBSC[ ch_no ]->CMNSR, SPIBSC_CMNSR_SSLF_SHIFT, SPIBSC_CMNSR_SSLF) == SPIBSC_SSL_NEGATE)
        {
            break;
        }
    }
    return 0;
}
/*******************************************************************************
End of function spibsc_stop
*******************************************************************************/


/******************************************************************************
* Function Name: spibsc_transfer
* Description  : Transmission setting of a SPI multi-I/O bus controller.
* Arguments    : uint32_t ch_no : use channel No
*                 st_spibsc_spimd_reg_t *regset :
*                    The pointer to a structure for the transfer
* Return Value :  0 : success
*                -1 : error
******************************************************************************/
int32_t spibsc_transfer(uint32_t ch_no, st_spibsc_spimd_reg_t * regset)
{
    if(ch_no > 1 )
    {
        return -1;
    }

    if(rza_io_reg_read_32(&SPIBSC[ ch_no ]->CMNCR, SPIBSC_CMNCR_MD_SHIFT, SPIBSC_CMNCR_MD) != SPIBSC_CMNCR_MD_SPI)
    {
        if(rza_io_reg_read_32(&SPIBSC[ ch_no ]->CMNSR, SPIBSC_CMNSR_SSLF_SHIFT, SPIBSC_CMNSR_SSLF) != SPIBSC_SSL_NEGATE)
        {
            return -1;
        }

        /* SPI Mode */
        rza_io_reg_write_32(&SPIBSC[ ch_no ]->CMNCR, SPIBSC_CMNCR_MD_SPI, SPIBSC_CMNCR_MD_SHIFT, SPIBSC_CMNCR_MD);

    }

    if(rza_io_reg_read_32(&SPIBSC[ ch_no ]->CMNSR, SPIBSC_CMNSR_TEND_SHIFT, SPIBSC_CMNSR_TEND) != SPIBSC_TRANS_END)
    {
        return -1;
    }

    /* ---- Command ---- */
    /* Enable/Disable */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->SMENR, regset->cde, SPIBSC_SMENR_CDE_SHIFT, SPIBSC_SMENR_CDE);
    if (regset->cde != SPIBSC_OUTPUT_DISABLE)
    {
        /* Command */
        rza_io_reg_write_32(&SPIBSC[ ch_no ]->SMCMR, regset->cmd, SPIBSC_SMCMR_CMD_SHIFT, SPIBSC_SMCMR_CMD);
        /* Single/Dual/Quad */
        rza_io_reg_write_32(&SPIBSC[ ch_no ]->SMENR, regset->cdb, SPIBSC_SMENR_CDB_SHIFT, SPIBSC_SMENR_CDB);
    }

    /* ---- Option Command ---- */
    /* Enable/Disable */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->SMENR, regset->ocde, SPIBSC_SMENR_OCDE_SHIFT, SPIBSC_SMENR_OCDE);
    
    if (regset->ocde != SPIBSC_OUTPUT_DISABLE)
    {
        /* Option Command */
        rza_io_reg_write_32(&SPIBSC[ ch_no ]->SMCMR, regset->ocmd, SPIBSC_SMCMR_OCMD_SHIFT, SPIBSC_SMCMR_OCMD);
        /* Single/Dual/Quad */
        rza_io_reg_write_32(&SPIBSC[ ch_no ]->SMENR, regset->ocdb, SPIBSC_SMENR_OCDB_SHIFT, SPIBSC_SMENR_OCDB);
    }

    /* ---- Address ---- */
    /* Enable/Disable */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->SMENR, regset->ade, SPIBSC_SMENR_ADE_SHIFT, SPIBSC_SMENR_ADE);
    
    if (regset->ade != SPIBSC_OUTPUT_DISABLE)
    {
        /* Address */
        rza_io_reg_write_32(&SPIBSC[ ch_no ]->SMADR, regset->addr, SPIBSC_SMADR_ADR_SHIFT, SPIBSC_SMADR_ADR);

        /* Single/Dual/Quad */
        rza_io_reg_write_32(&SPIBSC[ ch_no ]->SMENR, regset->adb, SPIBSC_SMENR_ADB_SHIFT, SPIBSC_SMENR_ADB);
    }

    /* ---- Option Data ---- */
    /* Enable/Disable */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->SMENR, regset->opde, SPIBSC_SMENR_OPDE_SHIFT, SPIBSC_SMENR_OPDE);
    
    if (regset->opde != SPIBSC_OUTPUT_DISABLE)
    {
        /* Option Data */
        rza_io_reg_write_32(&SPIBSC[ ch_no ]->SMOPR, regset->opd[0], SPIBSC_SMOPR_OPD3_SHIFT, SPIBSC_SMOPR_OPD3);

        /* Option Data */
        rza_io_reg_write_32(&SPIBSC[ ch_no ]->SMOPR, regset->opd[1], SPIBSC_SMOPR_OPD2_SHIFT, SPIBSC_SMOPR_OPD2);

        /* Option Data */
        rza_io_reg_write_32(&SPIBSC[ ch_no ]->SMOPR, regset->opd[2], SPIBSC_SMOPR_OPD1_SHIFT, SPIBSC_SMOPR_OPD1);

        /* Option Data */
        rza_io_reg_write_32(&SPIBSC[ ch_no ]->SMOPR, regset->opd[3], SPIBSC_SMOPR_OPD0_SHIFT, SPIBSC_SMOPR_OPD0);

        /* Single/Dual/Quad */
        rza_io_reg_write_32(&SPIBSC[ ch_no ]->SMENR, regset->opdb, SPIBSC_SMENR_OPDB_SHIFT, SPIBSC_SMENR_OPDB);
    }

    /* ---- Dummy ---- */
     /* Enable/Disable */
     rza_io_reg_write_32(&SPIBSC[ ch_no ]->SMENR, regset->dme, SPIBSC_SMENR_DME_SHIFT, SPIBSC_SMENR_DME);
     if (regset->dme != SPIBSC_DUMMY_CYC_DISABLE)
     {
         rza_io_reg_write_32(&SPIBSC[ ch_no ]->SMDMCR, regset->dmdb, SPIBSC_SMDMCR_DMDB_SHIFT, SPIBSC_SMDMCR_DMDB);

         /* Dummy Cycle */
         rza_io_reg_write_32(&SPIBSC[ ch_no ]->SMDMCR, regset->dmcyc, SPIBSC_SMDMCR_DMCYC_SHIFT, SPIBSC_SMDMCR_DMCYC);
     }

    /* ---- Data ---- */
    /* Enable/Disable */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->SMENR, regset->spide, SPIBSC_SMENR_SPIDE_SHIFT, SPIBSC_SMENR_SPIDE);
    if (regset->spide != SPIBSC_OUTPUT_DISABLE)
    {
        SPIBSC[ ch_no ]->SMWDR0.UINT32 = regset->smwdr[0];
        SPIBSC[ ch_no ]->SMWDR1.UINT32 = regset->smwdr[1];  /* valid in two serial-flash */

        /* Single/Dual/Quad */
        rza_io_reg_write_32(&SPIBSC[ ch_no ]->SMENR, regset->spidb, SPIBSC_SMENR_SPIDB_SHIFT, SPIBSC_SMENR_SPIDB);
    }

    rza_io_reg_write_32(&SPIBSC[ ch_no ]->SMCR, regset->sslkp, SPIBSC_SMCR_SSLKP_SHIFT, SPIBSC_SMCR_SSLKP);

    if ((regset->spidb != SPIBSC_1BIT) && (regset->spide != SPIBSC_OUTPUT_DISABLE))
    {
        if ((regset->spire  == SPIBSC_SPIDATA_ENABLE) && (regset->spiwe  == SPIBSC_SPIDATA_ENABLE))
        {
            /* not set in same time */
            return -1;
        }
    }
    
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->SMCR, regset->spire, SPIBSC_SMCR_SPIRE_SHIFT, SPIBSC_SMCR_SPIRE);
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->SMCR, regset->spiwe, SPIBSC_SMCR_SPIWE_SHIFT, SPIBSC_SMCR_SPIWE);

    /* SDR Transmission/DDR Transmission Setting */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->SMDRENR, regset->addre, SPIBSC_SMDRENR_ADDRE_SHIFT, SPIBSC_SMDRENR_ADDRE);
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->SMDRENR, regset->opdre, SPIBSC_SMDRENR_OPDRE_SHIFT, SPIBSC_SMDRENR_OPDRE);
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->SMDRENR, regset->spidre, SPIBSC_SMDRENR_SPIDRE_SHIFT, SPIBSC_SMDRENR_SPIDRE);

    /* execute after setting SPNDL bit */
    rza_io_reg_write_32(&SPIBSC[ ch_no ]->SMCR, SPIBSC_SPI_ENABLE, SPIBSC_SMCR_SPIE_SHIFT, SPIBSC_SMCR_SPIE);

    /* wait for transfer-start */
    R_SFLASH_WaitTend(ch_no);

    regset->smrdr[0] = SPIBSC[ ch_no ]->SMRDR0.UINT32;
    regset->smrdr[1] = SPIBSC[ ch_no ]->SMRDR1.UINT32;          /* valid in two serial-flash  */

    return 0;

} /* End of function spibsc_transfer() */

/******************************************************************************
* Function Name: Pekoe_io_spibsc_port_setting
* Description  : Port Settiong of SPIBSC
* Arguments    : uint32_t ch_no : use channel No
*                int_t data_bus_width
*                uint32_t bsz : BSZ bit
* Return Value :  0 : success
*                -1 : error
******************************************************************************/
static int32_t Pekoe_io_spibsc_port_setting(int_t data_bus_width, uint32_t bsz)
{

        /* ==== P4_4 : SPBCLK_0 ==== */
        /* port function : SPBCLK_0     */
        rza_io_reg_write_16(&GPIO.PFCAE4, 0, GPIO_PFCAE4_PFCAE44_SHIFT, GPIO_PFCAE4_PFCAE44);
        rza_io_reg_write_16(&GPIO.PFCE4,  0, GPIO_PFCE4_PFCE44_SHIFT,   GPIO_PFCE4_PFCE44);
        rza_io_reg_write_16(&GPIO.PFC4,   1, GPIO_PFC4_PFC44_SHIFT,     GPIO_PFC4_PFC44);

        /* port mode : Both use mode(The 2nd both use) */
        rza_io_reg_write_16(&GPIO.PMC4,   1, GPIO_PMC4_PMC44_SHIFT,     GPIO_PMC4_PMC44);

        /* Input/output control mode : peripheral function */
        rza_io_reg_write_16(&GPIO.PIPC4,  1, GPIO_PIPC4_PIPC44_SHIFT,   GPIO_PIPC4_PIPC44);

        /* ==== P4_5 : SPBSSL_0 ==== */
        /* port function : SPBSSL_0     */
        rza_io_reg_write_16(&GPIO.PFCAE4, 0, GPIO_PFCAE4_PFCAE45_SHIFT, GPIO_PFCAE4_PFCAE45);
        rza_io_reg_write_16(&GPIO.PFCE4,  0, GPIO_PFCE4_PFCE45_SHIFT,   GPIO_PFCE4_PFCE45);
        rza_io_reg_write_16(&GPIO.PFC4,   1, GPIO_PFC4_PFC45_SHIFT,     GPIO_PFC4_PFC45);

        /* port mode : Both use mode(The 2nd both use) */
        rza_io_reg_write_16(&GPIO.PMC4,   1, GPIO_PMC4_PMC45_SHIFT,     GPIO_PMC4_PMC45);

        /* Input/output control mode : peripheral function */
        rza_io_reg_write_16(&GPIO.PIPC4,  1, GPIO_PIPC4_PIPC45_SHIFT,   GPIO_PIPC4_PIPC45);

        /* ==== P4_6 : SPBIO00_0 ==== */
        /* port function : SPBIO00_0    */
        rza_io_reg_write_16(&GPIO.PFCAE4, 0, GPIO_PFCAE4_PFCAE46_SHIFT, GPIO_PFCAE4_PFCAE46);
        rza_io_reg_write_16(&GPIO.PFCE4,  0, GPIO_PFCE4_PFCE46_SHIFT,   GPIO_PFCE4_PFCE46);
        rza_io_reg_write_16(&GPIO.PFC4,   1, GPIO_PFC4_PFC46_SHIFT,     GPIO_PFC4_PFC46);

        /* port mode : Both use mode(The 2nd both use) */
        rza_io_reg_write_16(&GPIO.PMC4,   1, GPIO_PMC4_PMC46_SHIFT,     GPIO_PMC4_PMC46);

        /* Input/output control mode : peripheral function */
        rza_io_reg_write_16(&GPIO.PIPC4,  1, GPIO_PIPC4_PIPC46_SHIFT,   GPIO_PIPC4_PIPC46);

        /* ==== P4_7 : SPBIO10_0 ==== */
        /* port function : SPBIO10_0    */
        rza_io_reg_write_16(&GPIO.PFCAE4, 0, GPIO_PFCAE4_PFCAE47_SHIFT, GPIO_PFCAE4_PFCAE47);
        rza_io_reg_write_16(&GPIO.PFCE4,  0, GPIO_PFCE4_PFCE47_SHIFT,   GPIO_PFCE4_PFCE47);
        rza_io_reg_write_16(&GPIO.PFC4,   1, GPIO_PFC4_PFC47_SHIFT,     GPIO_PFC4_PFC47);

        /* port mode : Both use mode(The 2nd both use) */
        rza_io_reg_write_16(&GPIO.PMC4,   1, GPIO_PMC4_PMC47_SHIFT,     GPIO_PMC4_PMC47);

        /* Input/output control mode : peripheral function */
        rza_io_reg_write_16(&GPIO.PIPC4,  1, GPIO_PIPC4_PIPC47_SHIFT,   GPIO_PIPC4_PIPC47);

        if(data_bus_width == SPIBSC_4BIT)
        {
            /* ==== P4_2 : SPBIO20_0 ==== */
            /* port function : SPBIO20_0    */
            rza_io_reg_write_16(&GPIO.PFCAE4, 0, GPIO_PFCAE4_PFCAE42_SHIFT, GPIO_PFCAE4_PFCAE42);
            rza_io_reg_write_16(&GPIO.PFCE4,  0, GPIO_PFCE4_PFCE42_SHIFT,   GPIO_PFCE4_PFCE42);
            rza_io_reg_write_16(&GPIO.PFC4,   1, GPIO_PFC4_PFC42_SHIFT,     GPIO_PFC4_PFC42);

            /* port mode : Both use mode(The 2nd both use) */
            rza_io_reg_write_16(&GPIO.PMC4,   1, GPIO_PMC4_PMC42_SHIFT,     GPIO_PMC4_PMC42);

            /* Input/output control mode : peripheral function */
            rza_io_reg_write_16(&GPIO.PIPC4,  1, GPIO_PIPC4_PIPC42_SHIFT,   GPIO_PIPC4_PIPC42);

            /* ==== P4_3 : SPBIO30_0 ==== */
            /* port function : SPBIO30_0    */
            rza_io_reg_write_16(&GPIO.PFCAE4, 0, GPIO_PFCAE4_PFCAE43_SHIFT, GPIO_PFCAE4_PFCAE43);
            rza_io_reg_write_16(&GPIO.PFCE4,  0, GPIO_PFCE4_PFCE43_SHIFT,   GPIO_PFCE4_PFCE43);
            rza_io_reg_write_16(&GPIO.PFC4,   1, GPIO_PFC4_PFC43_SHIFT,     GPIO_PFC4_PFC43);

            /* port mode : Both use mode(The 2nd both use) */
            rza_io_reg_write_16(&GPIO.PMC4,   1, GPIO_PMC4_PMC43_SHIFT,     GPIO_PMC4_PMC43);

            /* Input/output control mode : peripheral function */
            rza_io_reg_write_16(&GPIO.PIPC4,  1, GPIO_PIPC4_PIPC43_SHIFT,   GPIO_PIPC4_PIPC43);

            if(bsz == SPIBSC_CMNCR_BSZ_DUAL)
            {
                /* ==== P3_10 : SPBIO01_0 ==== */
                /* port function : SPBIO01_0    */
                rza_io_reg_write_16(&GPIO.PFCAE3, 0, GPIO_PFCAE3_PFCAE310_SHIFT, GPIO_PFCAE3_PFCAE310);
                rza_io_reg_write_16(&GPIO.PFCE3,  0, GPIO_PFCE3_PFCE310_SHIFT,   GPIO_PFCE3_PFCE310);
                rza_io_reg_write_16(&GPIO.PFC3,   1, GPIO_PFC3_PFC310_SHIFT,     GPIO_PFC3_PFC310);

                /* port mode : Both use mode(The 2nd both use) */
                rza_io_reg_write_16(&GPIO.PMC3,   1, GPIO_PMC3_PMC310_SHIFT,     GPIO_PMC3_PMC310);

                /* Input/output control mode : peripheral function */
                rza_io_reg_write_16(&GPIO.PIPC3,  1, GPIO_PIPC3_PIPC310_SHIFT,   GPIO_PIPC3_PIPC310);

                /* ==== P3_11 : SPBIO11_0 ==== */
                /* port function : SPBIO11_0    */
                rza_io_reg_write_16(&GPIO.PFCAE3, 0, GPIO_PFCAE3_PFCAE311_SHIFT, GPIO_PFCAE3_PFCAE311);
                rza_io_reg_write_16(&GPIO.PFCE3,  0, GPIO_PFCE3_PFCE311_SHIFT,   GPIO_PFCE3_PFCE311);
                rza_io_reg_write_16(&GPIO.PFC3,   1, GPIO_PFC3_PFC311_SHIFT,     GPIO_PFC3_PFC311);

                /* port mode : Both use mode(The 2nd both use) */
                rza_io_reg_write_16(&GPIO.PMC3,   1, GPIO_PMC3_PMC311_SHIFT,     GPIO_PMC3_PMC311);

                /* Input/output control mode : peripheral function */
                rza_io_reg_write_16(&GPIO.PIPC3,  1, GPIO_PIPC3_PIPC311_SHIFT,   GPIO_PIPC3_PIPC311);

                /* ==== P3_12 : SPBIO21_0 ==== */
                /* port function : SPBIO21_0    */
                rza_io_reg_write_16(&GPIO.PFCAE3, 0, GPIO_PFCAE3_PFCAE312_SHIFT, GPIO_PFCAE3_PFCAE312);
                rza_io_reg_write_16(&GPIO.PFCE3,  0, GPIO_PFCE3_PFCE312_SHIFT,   GPIO_PFCE3_PFCE312);
                rza_io_reg_write_16(&GPIO.PFC3,   1, GPIO_PFC3_PFC312_SHIFT,     GPIO_PFC3_PFC312);

                /* port mode : Both use mode(The 2nd both use) */
                rza_io_reg_write_16(&GPIO.PMC3,   1, GPIO_PMC3_PMC312_SHIFT,     GPIO_PMC3_PMC312);

                /* Input/output control mode : peripheral function */
                rza_io_reg_write_16(&GPIO.PIPC3,  1, GPIO_PIPC3_PIPC312_SHIFT,   GPIO_PIPC3_PIPC312);

                /* ==== P3_13 : SPBIO31_0 ==== */
                /* port function : SPBIO31_0    */
                rza_io_reg_write_16(&GPIO.PFCAE3, 0, GPIO_PFCAE3_PFCAE313_SHIFT, GPIO_PFCAE3_PFCAE313);
                rza_io_reg_write_16(&GPIO.PFCE3,  0, GPIO_PFCE3_PFCE313_SHIFT,   GPIO_PFCE3_PFCE313);
                rza_io_reg_write_16(&GPIO.PFC3,   1, GPIO_PFC3_PFC313_SHIFT,     GPIO_PFC3_PFC313);

                /* port mode : Both use mode(The 2nd both use) */
                rza_io_reg_write_16(&GPIO.PMC3,   1, GPIO_PMC3_PMC313_SHIFT,     GPIO_PMC3_PMC313);

                /* Input/output control mode : peripheral function */
                rza_io_reg_write_16(&GPIO.PIPC3,  1, GPIO_PIPC3_PIPC313_SHIFT,   GPIO_PIPC3_PIPC313);
            }
        }

    return 0;

}
/*******************************************************************************
End of function Pekoe_io_spibsc_port_setting
*******************************************************************************/

/* End of File */
