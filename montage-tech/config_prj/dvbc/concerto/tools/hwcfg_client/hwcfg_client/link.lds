/**************************************************************************
*                                                                         *
*   PROJECT     : MIPS port for uC/OS-II                                  *
*                                                                         *
*   MODULE      : link.xn                                                 *
*                                                                         *
*   AUTHOR      : Michael Anburaj                                         *
*                 URL  : http://geocities.com/michaelanburaj/             *
*                 EMAIL: michaelanburaj@hotmail.com                       *
*                                                                         *
*   PROCESSOR   : MIPS 4Kc (32 bit RISC) - ATLAS board                    *
*                                                                         *
*   TOOL-CHAIN  : SDE & Cygnus                                            *
*                                                                         *
*   DESCRIPTION :                                                         *
*   Linker script file                                                    *
*                                                                         *
**************************************************************************/

OUTPUT_FORMAT("elf32-tradlittlemips", "elf32-tradbigmips", "elf32-tradlittlemips")
OUTPUT_ARCH(mips)

/**** Start point ****/
ENTRY(_start)			/* Entry point of application		*/

MEMORY
{
    /* ram (wx) : ORIGIN = 0x80000000, LENGTH = 64M */
    ram (wx) : ORIGIN = 0x80000000, LENGTH = 128M
}

SECTIONS
{
  /**** Code and read-only data ****/

  /* .text 0x80008000 : */
  .text 0x80100000 :  
  {
    _ftext = ABSOLUTE(.) ;	/* Start of code and read-only data	*/
    *(.init) 		/* Entry point				*/
	. = ALIGN(0x10000);
	*(.avinit)
	*(.text)
    _ecode = ABSOLUTE(.) ;	/* End of code				*/
    *(.rodata)
    . = ALIGN(8);
    _etext = ABSOLUTE(.);	/* End of code and read-only data	*/
  } > ram

  /**** Initialised data ****/

  .data :
  {
    _fdata = ABSOLUTE(.);	/* Start of initialised data		*/
    *(.data)
    *(.data_section) 
    . = ALIGN(8);

    __gp = ABSOLUTE(. + 0x7ff0); /* Base of small data			*/

    *(.lit8) 
    *(.lit4) 
    *(.rel.*)
    *(.sdata) 
    
    . = . + 0x400;
    *(.av_data) 
    *(.av_data.*)     
		. = . + 0x400;

    . = ALIGN(8);
    *(.av_data_section) 
    *(.av_data_section.*)    
    _edata  = ABSOLUTE(.);	/* End of initialised data		*/
  } > ram

  /**** Uninitialised data ****/

  _fbss = .;			/* Start of uninitialised data		*/

  .sbss (NOLOAD): 
  { 
    *(.sbss) 
    *(.sbss_section) 
    *(.scommon)
    *(.scommon_section) 
  }
  .bss (NOLOAD):
  {
  	. = . + 0x400;
    *(.av_bss) 
    . = . + 0x400;
     *(.av_bss_section) 
     
     . = ALIGN(32);
     . = . + 128;
     
    *(.bss)
    *(.bss_section)
    *(COMMON)
		*(.COMMON_section) 
    /* Allocate room for stack */
    .   =  ALIGN(8) ;
    _freemem = .;
    
     */
    .   =  ALIGN(8) ;
    _freemem = .;
      _ap_stack_base = .;
    . = . + 0000;
    . = . + 00000;		/*	qbq*/
    .   =  ALIGN(8) ;    
    _ap_stack_top = .;
    
    . = ALIGN(8);
    _av_stack_base = .;
    . = . + 0000;    
    . = ALIGN(8);
    _av_stack_top = .;
}

  _end = . ;			/* End of unitialised data		*/

  .gptab.sdata : { *(.gptab.data) *(.gptab.sdata) }
  .gptab.sbss : { *(.gptab.bss) *(.gptab.sbss) }

  /DISCARD/ :
  {
    *(.reginfo)
  }

  PROVIDE(etext = _etext);
  PROVIDE (edata = .);
  PROVIDE (end = .);
}


