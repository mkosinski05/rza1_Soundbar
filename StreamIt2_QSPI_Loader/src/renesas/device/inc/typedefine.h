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
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name     : typedefine.h
* Description   : Defines exact width integer types.
*******************************************************************************/
/*******************************************************************************
* History       : DD.MM.YYYY Version Description
*               : 18.06.2013 1.00
*******************************************************************************/
#ifndef TYPE_DEFINE_H
#define TYPE_DEFINE_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
Typedef definitions
*******************************************************************************/
typedef signed   char      _SBYTE;
typedef unsigned char      _UBYTE;
typedef signed   short     _SWORD;
typedef unsigned short     _UWORD;
typedef signed   int       _SINT;
typedef unsigned int       _UINT;
typedef signed   long      _SDWORD;
typedef unsigned long      _UDWORD;
typedef signed   long long _SQWORD;
typedef unsigned long long _UQWORD;

/* in case <stdio.h> has defined it. */
#ifndef NULL
#define NULL 0
#endif

/******************************************************************************
Macro definitions
******************************************************************************/
#if !defined(__bool_true_false_are_defined) && !defined(__cplusplus)

#define     false 0
#define     true  1

#endif

#define     UNUSED_PARAM(param)             (void)(param)

#define     UNUSED_VARIABLE(param)          (void)(param)

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef char                char_t;
typedef unsigned int        bool_t;
typedef int                 int_t;
typedef signed char         int8_t;
typedef signed short        int16_t;
typedef signed long         int32_t;
typedef signed long long    int64_t;
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned long       uint32_t;
typedef unsigned long long  uint64_t;
typedef float               float32_t;
typedef double              float64_t;
typedef long double         float128_t;


#define     UNUSED_PARAM(param)             (void)(param)

#define     UNUSED_VARIABLE(param)          (void)(param)


#endif
