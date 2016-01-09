/*********  ********************************************************************/
/* Copyright (c) 2008 Montage Tech - All Rights Reserved                     */
/*****************************************************************************/
/*!
  \file link_mips.ld

  This file is linker scriptor which links all objects after compiling. All sections are defined here.
  */


OUTPUT_FORMAT("elf32-littlemips")
OUTPUT_ARCH(mips)

/**** Start point ****/
ENTRY(_init)			/* Entry point of application		*/

MEMORY
{
    ram (wx) : ORIGIN = 0x00000000, LENGTH = 0x200000
}

SECTIONS
{
  /**** Code and read-only data ****/

  .text  :
  {
    _ftext = ABSOLUTE(.) ;	/* Start of code and read-only data	*/
    *(.init) 		/* Entry point				*/
    *(.interrupt)
    *(.cache)
    *(.text .text.*)
    *(.gnu.linkonce.t*)
    *(.mips16.fn.*)
    *(.mips16.call.*)
   _ecode = ABSOLUTE(.) ;	/* End of code				*/

    /**(.rodata)*/
    *(.rel.sdata)
    *(.fini)

    . = ALIGN(8);
    _etext = ABSOLUTE(.);	/* End of code and read-only data	*/
  } > ram

  /**** Initialised data ****/

  .rodata :	{ *(.rdata .rodata .rodata.* .gnu.linkonce.r.*) } > ram
	
  .data :
  {
    /* *(.data) */   

	*(.got.plt) *(.got)
	*(.data .data.* .gnu.linkonce.d.*)
	*(.rel.dyn)
	
    . = ALIGN(8);
    _gp = ABSOLUTE(. + 0x7ff0); /* Base of small data			*/


    _fdata = ABSOLUTE(.);	/* Start of initialised data		*/
	
    *(.lit8) 
    *(.lit4) 
    *(.sdata) 
    *(.sdata .sdata.* .gnu.linkonce.s.*)

    . = ALIGN(8);

    _edata  = ABSOLUTE(.);	/* End of initialised data		*/
  } > ram

  /**** Uninitialised data ****/

  _fbss = .;			/* Start of uninitialised data		*/

  .sbss : 
  { 
    *(.sbss .sbss.* .gnu.linkonce.sb.*) 
    *(.scommon)
  }
  .bss :
  {
    *(.bss .bss.* .gnu.linkonce.b.*)
    *(COMMON)

    /* Allocate room for stack */
    .   =  ALIGN(8) ;
    _freemem = .;
  }

  _fbss_end = .;
  _end = . ;			/* End of unitialised data		*/

  .gptab.sdata : { *(.gptab.data) *(.gptab.sdata) }
  .gptab.sbss : { *(.gptab.bss) *(.gptab.sbss) }

  /DISCARD/ :
  {
    *(.reginfo) *(.pdr)  *(.comment) *(.note.GNU-stack)
  }

  PROVIDE(etext = _etext);
  PROVIDE (edata = .);
  PROVIDE (end = .);
  /* Stabs debugging sections.  */
  .stab 0 : { *(.stab) }
  .stabstr 0 : { *(.stabstr) }
  .stab.excl 0 : { *(.stab.excl) }
  .stab.exclstr 0 : { *(.stab.exclstr) }
  .stab.index 0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment 0 : { *(.comment) }
  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }
  /* These must appear regardless of  .  */
  .gptab.sdata : { *(.gptab.data) *(.gptab.sdata) }
  .gptab.sbss : { *(.gptab.bss) *(.gptab.sbss) }
}



