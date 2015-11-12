GROUP(
  libgcc.a
  libc.a
  libm.a
)

MEMORY
{
  /* Define each memory region */
  MFlashA512 (rx) : ORIGIN = 0x1a000000, LENGTH = 0x80000 /* 512K bytes */
  MFlashB512 (rx) : ORIGIN = 0x1b000000, LENGTH = 0x80000 /* 512K bytes */
  RamLoc32 (rwx) : ORIGIN = 0x10000000, LENGTH = 0x8000 /* 32K bytes */
  RamLoc40 (rwx) : ORIGIN = 0x10080000, LENGTH = 0xa000 /* 40K bytes */
  RamAHB32 (rwx) : ORIGIN = 0x20000000, LENGTH = 0x8000 /* 32K bytes */
  RamAHB16 (rwx) : ORIGIN = 0x20008000, LENGTH = 0x4000 /* 16K bytes */
  RamAHB_ETB16 (rwx) : ORIGIN = 0x2000c000, LENGTH = 0x4000 /* 16K bytes */
}

/* Define a symbol for the top of each memory region */
__top_MFlashA512 = 0x1a000000 + 0x80000;
__top_MFlashB512 = 0x1b000000 + 0x80000;
__top_RamLoc32 = 0x10000000 + 0x8000;
__top_RamLoc40 = 0x10080000 + 0xa000;
__top_RamAHB32 = 0x20000000 + 0x8000;
__top_RamAHB16 = 0x20008000 + 0x4000;
__top_RamAHB_ETB16 = 0x2000c000 + 0x4000;

ENTRY(ResetISR)

SECTIONS
{
    
    .text_Flash2 : ALIGN(4)
    {
       FILL(0xff)
    	*(.text_Flash2*) /* for compatibility with previous releases */
    	*(.text_MFlashB512*) /* for compatibility with previous releases */
    	*(.text.$Flash2*)
    	*(.text.$MFlashB512*)
    	*(.rodata.$Flash2*)
    	*(.rodata.$MFlashB512*)
    } > MFlashB512

    /* MAIN TEXT SECTION */    
    .text : ALIGN(4)
    {
        FILL(0xff)
        __vectors_start__ = ABSOLUTE(.) ;
        KEEP(*(.isr_vector))
        
        /* Global Section Table */
        . = ALIGN(4) ;
        __section_table_start = .;
        __data_section_table = .;
        LONG(LOADADDR(.data));
        LONG(    ADDR(.data));
        LONG(  SIZEOF(.data));
        LONG(LOADADDR(.data_RAM2));
        LONG(    ADDR(.data_RAM2));
        LONG(  SIZEOF(.data_RAM2));
        LONG(LOADADDR(.data_RAM3));
        LONG(    ADDR(.data_RAM3));
        LONG(  SIZEOF(.data_RAM3));
        LONG(LOADADDR(.data_RAM4));
        LONG(    ADDR(.data_RAM4));
        LONG(  SIZEOF(.data_RAM4));
        LONG(LOADADDR(.data_RAM5));
        LONG(    ADDR(.data_RAM5));
        LONG(  SIZEOF(.data_RAM5));
        __data_section_table_end = .;
        __bss_section_table = .;
        LONG(    ADDR(.bss));
        LONG(  SIZEOF(.bss));
        LONG(    ADDR(.bss_RAM2));
        LONG(  SIZEOF(.bss_RAM2));
        LONG(    ADDR(.bss_RAM3));
        LONG(  SIZEOF(.bss_RAM3));
        LONG(    ADDR(.bss_RAM4));
        LONG(  SIZEOF(.bss_RAM4));
        LONG(    ADDR(.bss_RAM5));
        LONG(  SIZEOF(.bss_RAM5));
        __bss_section_table_end = .;
        __section_table_end = . ;
        /* End of Global Section Table */
        

        *(.after_vectors*)
        
        /* Code Read Protect data */
        . = 0x000002FC ;
        PROVIDE(__CRP_WORD_START__ = .) ;
        KEEP(*(.crp))
        PROVIDE(__CRP_WORD_END__ = .) ;
        ASSERT(!(__CRP_WORD_START__ == __CRP_WORD_END__), "Linker CRP Enabled, but no CRP_WORD provided within application");
        /* End of Code Read Protect */
        
    } >MFlashA512
    
    .text : ALIGN(4)    
    {
         *(.text*)
        *(.rodata .rodata.* .constdata .constdata.*)
        . = ALIGN(4);
    	/*
    	_staticfs = .;
    	KEEP(build/fs.o)
        . = ALIGN(4);
        */
        
    } > MFlashA512

    /*
     * for exception handling/unwind - some Newlib functions (in common
     * with C++ and STDC++) use this. 
     */
    .ARM.extab : ALIGN(4)
    {
    	*(.ARM.extab* .gnu.linkonce.armextab.*)
    } > MFlashA512
    __exidx_start = .;
    
    .ARM.exidx : ALIGN(4)
    {
    	*(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > MFlashA512
    __exidx_end = .;
    
    _etext = .;

    
    /* DATA section for RamLoc40 */
   .data_RAM2 : ALIGN(4)
    {
       FILL(0xff)
      /*  __core_m0app_START__ = .; start of slave image 
         KEEP(*(.core_m0app))
       __core_m0app_END__ = .;  end of slave image 
       ASSERT(!(__core_m0app_START__ == __core_m0app_END__), "No slave code for _core_m0app");
       ASSERT( (ABSOLUTE(__core_m0app_START__) == __vectors_start___core_m0app), "M0APP execute address differs from address provided in source image");*/
       *(.ramfunc.$RAM2)
       *(.ramfunc.$RamLoc40)
    	*(.data.$RAM2*)
    	*(.data.$RamLoc40*)
       . = ALIGN(4) ;
    } > RamLoc40 AT>MFlashA512
    
    /* DATA section for RamAHB32 */
    .data_RAM3 : ALIGN(4)
    {
       FILL(0xff)
       *(.ramfunc.$RAM3)
       *(.ramfunc.$RamAHB32)
    	*(.data.$RAM3*)
    	*(.data.$RamAHB32*)
       . = ALIGN(4) ;
    } > RamAHB32 AT>MFlashA512
    
    /* DATA section for RamAHB16 */
    .data_RAM4 : ALIGN(4)
    {
       FILL(0xff)
       *(.ramfunc.$RAM4)
       *(.ramfunc.$RamAHB16)
    	*(.data.$RAM4*)
    	*(.data.$RamAHB16*)
       . = ALIGN(4) ;
    } > RamAHB16 AT>MFlashA512
    
    /* DATA section for RamAHB_ETB16 */
    .data_RAM5 : ALIGN(4)
    {
       FILL(0xff)
       *(.ramfunc.$RAM5)
       *(.ramfunc.$RamAHB_ETB16)
    	*(.data.$RAM5*)
    	*(.data.$RamAHB_ETB16*)
       . = ALIGN(4) ;
    } > RamAHB_ETB16 AT>MFlashA512
    
    /* MAIN DATA SECTION */
    

    .uninit_RESERVED : ALIGN(4)
    {
        KEEP(*(.bss.$RESERVED*))
        . = ALIGN(4) ;
        _end_uninit_RESERVED = .;
    } > RamLoc32

	
	/* Main DATA section (RamLoc32) */
	.data : ALIGN(4)
	{
	   FILL(0xff)
	   _data = . ;
	   *(vtable)
	   *(.ramfunc*)
	   *(.data*)
	   . = ALIGN(4) ;
	   _edata = . ;
	} > RamLoc32 AT>MFlashA512

    /* BSS section for RamLoc40 */
    .bss_RAM2 : ALIGN(4)
    {
    	*(.bss.$RAM2*)
    	*(.bss.$RamLoc40*)
       . = ALIGN(4) ;
    } > RamLoc40
    /* BSS section for RamAHB32 */
    .bss_RAM3 : ALIGN(4)
    {
    	*(.bss.$RAM3*)
    	*(.bss.$RamAHB32*)
       . = ALIGN(4) ;
    } > RamAHB32
    /* BSS section for RamAHB16 */
    .bss_RAM4 : ALIGN(4)
    {
    	*(.bss.$RAM4*)
    	*(.bss.$RamAHB16*)
       . = ALIGN(4) ;
    } > RamAHB16
    /* BSS section for RamAHB_ETB16 */
    .bss_RAM5 : ALIGN(4)
    {
    	*(.bss.$RAM5*)
    	*(.bss.$RamAHB_ETB16*)
       . = ALIGN(4) ;
    } > RamAHB_ETB16

    /* MAIN BSS SECTION */
    .bss : ALIGN(4)
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4) ;
        _ebss = .;
        PROVIDE(end = .);
    } > RamLoc32
        
    /* NOINIT section for RamLoc40 */
    .noinit_RAM2 (NOLOAD) : ALIGN(4)
    {
    	*(.noinit.$RAM2*)
    	*(.noinit.$RamLoc40*)
       . = ALIGN(4) ;
    } > RamLoc40 
    /* NOINIT section for RamAHB32 */
    .noinit_RAM3 (NOLOAD) : ALIGN(4)
    {
    	*(.noinit.$RAM3*)
    	*(.noinit.$RamAHB32*)
       . = ALIGN(4) ;
    } > RamAHB32 
    /* NOINIT section for RamAHB16 */
    .noinit_RAM4 (NOLOAD) : ALIGN(4)
    {
    	*(.noinit.$RAM4*)
    	*(.noinit.$RamAHB16*)
       . = ALIGN(4) ;
    } > RamAHB16 
    /* NOINIT section for RamAHB_ETB16 */
    .noinit_RAM5 (NOLOAD) : ALIGN(4)
    {
    	*(.noinit.$RAM5*)
    	*(.noinit.$RamAHB_ETB16*)
       . = ALIGN(4) ;
    } > RamAHB_ETB16 
    
    /* DEFAULT NOINIT SECTION */
    .noinit (NOLOAD): ALIGN(4)
    {
        _noinit = .;
        *(.noinit*) 
         . = ALIGN(4) ;
        _end_noinit = .;
    } > RamLoc32
    
    PROVIDE(_pvHeapStart = .);
    PROVIDE(_vStackTop = __top_RamLoc32 - 0);
}
