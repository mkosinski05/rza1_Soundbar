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
/******************************************************************************
* File Name     : StreamIt2QspiBootStandalone.ld
* Device(s)     : RZ/A1L
* Tool-Chain    : GNUARM-NONE-EABI-v16.01
* H/W Platform  : Stream it! v2
* Description   : Linker file for the QSPI Loader program 
******************************************************************************/
/******************************************************************************
* History       : DD.MM.YYYY Version Description
*               : 10.02.17   1.00    First Release
******************************************************************************/
OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(start)

/* Base Address RAM Memory Table 3-Mbyte on-chip RAM */  
MEMORY 
{
	/* Internal RAM address range H'2000_0000 to H'2001_FFFF is configured as data retention RAM */
	/* Write access to this address range has to be enabled by writing to registers SYSCR3 */
	/* CPG.SYSCR3 = 0x0F;  or memory set S:0xFCFE0408 0 {(unsigned char)0x0F} */

	BOOT_RAM (rwx)       : ORIGIN = 0x202F6000, LENGTH = 0x0000A000       /* Boot RAM area, reservered during startup */
	QSPI_FLASH (rx)      : ORIGIN = 0x18000000, LENGTH = 0x80000          /* Boot flash area, single byte read access */    
}

PROGRAM_STACK_SIZE    = 0x800;       /* Application stack                 */
IRQ_STACK_SIZE	      = 0x400;       /* IRQ mode stack                    */
FIQ_STACK_SIZE	      = 0x100;       /* FRQ mode stack                    */
SVC_STACK_SIZE	      = 0x100;       /* SVC mode stack                    */
ABT_STACK_SIZE	      = 0x100;       /* ABT mode stack                    */
TTB_SIZE              = 0x4000;      /* Level-1 Translation Table for MMU */

ALIGN04               = 0x04;        /* Alignment 4 bytes                 */
ALIGN08               = 0x08;        /* Alignment 8 bytes                 */
ALIGN4000             = 0x4000;      /* Alignment 16384 (16K) bytes       */

SECTIONS
{
    /* Initial code section code will run from the load address which is the 
       start of QSPI defined for QSPI boot.
    */
    .reset :
    {
        /* The start object file begins with the vector table, and so must be located here. */
        *start.o (.text)
        *start.o (.rodata)
        *start.o (.data)
        
        /* Reallocate the vector base register to the start of the boot loader. */
        *vbar_init.o (.text)
        *vbar_init.o (.rodata)
        *vbar_init.o (.data) 
        ld_code_reset_handler_start = .;
		PROVIDE(ld_code_reset_handler_start = .);
		*reset_handler.o (.text)
		PROVIDE(ld_code_reset_handler_end = .);
 		/* Power ON Reset. */
		*resetprg.o (.text)
        *resetprg.o (.rodata)
        *resetprg.o (.data)
        
        /* The QSPI re-configuration needs to be copied to RAM by this code (single SPI)*/
        /* The section that gets copied is load2 */
        *init_spibsc_init1_section.o (.text)
        *init_spibsc_init1_section.o (.rodata)
        *init_spibsc_init1_section.o (.data)
        
        /* Code to copy section 2 to RAM should be placed here in ROM */
		*init_spibsc_init2_section.o (.text)
		*init_spibsc_init2_section.o (.rodata)
		*init_spibsc_init2_section.o (.data)

        /* Any following code must be aligned to a 4 byte boundary. */
		. = ALIGN(0x4);
        ld_load_module1_end_addr = .;
        ld_load_module1_size = SIZEOF(.reset);         
  	} >  QSPI_FLASH

    /* These __ labels are required by the toolchain */
    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > QSPI_FLASH
    __exidx_end = .;
    
    /* load2, Copied from QSPI_FLASH to BOOT_RAM,where execution starts.
       Code is physically stored (before copy) at the address specified 
       straight after reset module in QSPI_FLASH 
    */      
	.load2 : AT (ld_load_module1_end_addr)
	{
        g_ld_load_module2_addr = ld_load_module1_end_addr;        
		ld_boot_protected_ram_start = .;
        /* Any following code must be aligned to a 4 byte boundary. */
        . = ALIGN(0x4);
  
        /* The load of this code is completed by the built in boot code, at slow speed. */
        /* This short code section runs first to reconfigure the SPI to high speed. */
        /* The following location is available in the program to be copied by init_init1.*/
        g_ld_code_spibsc_init_1_start = .;
        PROVIDE(g_ld_code_spibsc_init_1_start = .);

        /* The QSPI re-configuration needs to be copied to RAM by this code (single SPI)*/
        /* The section that gets copied is LoadModule 2 */
        /* (g_ld_code_spibsc_init_1_start to g_ld_code_spibsc_init_1_end*/
        /* Reconfiguration Functions. To speed up the SPI only.*/
		*spibsc_init1.o (.text)
		*spibsc_init1.o (.rodata)
		*spibsc_init1.o (.data)
        
        /* Code for accessing the IO registers. This is used by both loaded sections. */
		*rza_io_regrw.o (.text)
		*rza_io_regrw.o (.rodata)
		*rza_io_regrw.o (.data)

		. = ALIGN(0x8);
        *(.text)

        /* The following location is available in the program to be copied by init_init1.*/
        g_ld_code_spibsc_init_1_end = .;
		PROVIDE(g_ld_code_spibsc_init_1_end = .);

        /* Any following code must be aligned to a 4 byte boundary. */
		. = ALIGN(0x4);
	} > BOOT_RAM 

    
    /* load3, Copied from QSPI_FLASH to BOOT_RAM,where execution starts.
       Code is physically stored (before copy) at the address specified 
       straight after load2 module in QSPI_FLASH 
    */          
	.load3 :  AT (ld_load_module1_end_addr + SIZEOF (.load2))
	{
        /* Any following code must be aligned to a 4 byte boundary. */
        . = ALIGN(0x4);

        /* The load of this code is completed by the Module 2 code, at high speed. */
        /* This code section runs to reconfigure the SPI to the final start DQSPI. */
        /*e code then looks for valid user code. */

        /* Make this location available in the program to be copied by init_init2.*/
        g_ld_load_module3_addr = ld_load_module1_end_addr + SIZEOF (.load2);
        g_ld_code_spibsc_init_2_start = .;
        PROVIDE(g_ld_code_spibsc_init_2_start = .);

        /* Reconfiguration Functions and application detection. */
		*spibsc_init2.o (.text)
		*spibsc_init2.o (.rodata)
		*spibsc_init2.o (.data)
        /* Simple assembler function to jump to users code. */
		*user_prog_jmp.o (.text)
		*user_prog_jmp.o (.rodata)
		*user_prog_jmp.o (.data)
		*spibsc_flash_api.o (.text)
		*spibsc_flash_api.o (.rodata)
		*spibsc_flash_api.o (.data)
		*spibsc_flash_userdef.o (.text)
		*spibsc_flash_userdef.o (.rodata)
		*spibsc_flash_userdef.o (.data)
		*spibsc_ioset_api.o (.text)
		*spibsc_ioset_api.o (.rodata)
		*spibsc_ioset_api.o (.data)
		*spibsc_ioset_drv.o (.text)
		*spibsc_ioset_drv.o (.rodata)
		*spibsc_ioset_drv.o (.data)
		*spibsc_ioset_userdef.o (.text)
		*spibsc_ioset_userdef.o (.rodata)
		*spibsc_ioset_userdef.o (.data)

        /* The following location is available in the program to be copied by init_init2.*/
        g_ld_code_spibsc_init_2_end = .;
		PROVIDE(g_ld_code_spibsc_init_2_end = .);

        /* Any following code must be aligned to a 4 byte boundary. */
        . = ALIGN(0x4);
        ld_load_module3_end_addr = ld_load_module1_end_addr + SIZEOF (.load2) + SIZEOF(.load3);        
	} > BOOT_RAM 

    /* constant data section */
	.rodata  : AT (ld_load_module3_end_addr + SIZEOF(.load4))
    {
        *(.rodata)
        *(.rodata.*)
        . = ALIGN(0x8);
        address_start_data_ROM = .;
        *(.data)
        *(.data.*)
        *(.got.plt) *(.got)        
        . = ALIGN(0x8);
        address_end_data_ROM = .;
        PROVIDE(end = .);
	} >  BOOT_RAM

    /* ram data section */
    .data address_end_data_ROM : 
    {
		address_start_data_RAM = .;
		.  += (address_end_data_ROM - address_start_data_ROM);
		address_end_data_RAM = .;
    } >  BOOT_RAM

    /* initialised data section */
    .bss :
    {
        _bss = .;
        PROVIDE(ld_bss_start = .);         
        *(.bss)
        *(.bss.**)
        *(COMMON)
        PROVIDE(ld_bss_end = .);
        _ebss = .;
        _end = .;
    } >  BOOT_RAM

	.irq_stack (NOLOAD) : ALIGN(0x100)
	{
		.	= ALIGN(ALIGN08);
		irq_stack_start = .;
		.  += IRQ_STACK_SIZE;
		irq_stack_end = .;
        
		.	= ALIGN(ALIGN08);
		fiq_stack_start = .;
		.  += FIQ_STACK_SIZE;
		fiq_stack_end = .;

		.	= ALIGN(ALIGN08);
		PROVIDE(svc_stack_start = .);		
		.  += SVC_STACK_SIZE;
		svc_stack_end = .;
        
		.	= ALIGN(ALIGN08);
		abt_stack_start = .;
		.  += ABT_STACK_SIZE;
		abt_stack_end = .;

		.	= ALIGN(ALIGN08);
		program_stack_start = .;
		.  += PROGRAM_STACK_SIZE;
		program_stack_end = .; 
	} >  BOOT_RAM	
    
	.ttb_mmu1 (NOLOAD) : ALIGN(0x4000)
	{
		ttb_mmu1_base = .;
		.  += TTB_SIZE;
		.	= ALIGN(0x4);
		ttb_mmu1_end = .;
	} >  BOOT_RAM
}
