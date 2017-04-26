/* GNU linker script for ESP8266, common sections and symbols */

/* define the top of RAM */
_heap_end = ORIGIN(dram0_0_seg) + LENGTH(dram0_0_seg);

PHDRS
{
    dport0_0_phdr PT_LOAD;
    dram0_0_phdr PT_LOAD;
    dram0_0_bss_phdr PT_LOAD;
    iram1_0_phdr PT_LOAD;
    irom0_0_phdr PT_LOAD;
}

ENTRY(firmware_start)
EXTERN(_DebugExceptionVector)
EXTERN(_DoubleExceptionVector)
EXTERN(_KernelExceptionVector)
EXTERN(_NMIExceptionVector)
EXTERN(_UserExceptionVector)

PROVIDE(_memmap_vecbase_reset = 0x40000000);

/* Various memory-map dependent cache attribute settings: */
_memmap_cacheattr_wb_base = 0x00000110;
_memmap_cacheattr_wt_base = 0x00000110;
_memmap_cacheattr_bp_base = 0x00000220;
_memmap_cacheattr_unused_mask = 0xFFFFF00F;
_memmap_cacheattr_wb_trapnull = 0x2222211F;
_memmap_cacheattr_wba_trapnull = 0x2222211F;
_memmap_cacheattr_wbna_trapnull = 0x2222211F;
_memmap_cacheattr_wt_trapnull = 0x2222211F;
_memmap_cacheattr_bp_trapnull = 0x2222222F;
_memmap_cacheattr_wb_strict = 0xFFFFF11F;
_memmap_cacheattr_wt_strict = 0xFFFFF11F;
_memmap_cacheattr_bp_strict = 0xFFFFF22F;
_memmap_cacheattr_wb_allvalid = 0x22222112;
_memmap_cacheattr_wt_allvalid = 0x22222112;
_memmap_cacheattr_bp_allvalid = 0x22222222;
PROVIDE(_memmap_cacheattr_reset = _memmap_cacheattr_wb_trapnull);

SECTIONS
{

    .dport0.rodata : ALIGN(4)
    {
        _dport0_rodata_start = ABSOLUTE(.);
        *(.dport0.rodata)
        *(.dport.rodata)
        _dport0_rodata_end = ABSOLUTE(.);
    } >dport0_0_seg :dport0_0_phdr

    .dport0.literal : ALIGN(4)
    {
        _dport0_literal_start = ABSOLUTE(.);
        *(.dport0.literal)
        *(.dport.literal)
        _dport0_literal_end = ABSOLUTE(.);
    } >dport0_0_seg :dport0_0_phdr

    .dport0.data : ALIGN(4)
    {
        _dport0_data_start = ABSOLUTE(.);
        *(.dport0.data)
        *(.dport.data)
        _dport0_data_end = ABSOLUTE(.);
    } >dport0_0_seg :dport0_0_phdr

    .irom0.text : ALIGN(4)
    {
        _irom0_text_start = ABSOLUTE(.);
        *(.irom0.literal .irom.literal .irom.text.literal .irom0.text .irom.text)

        /* we put some specific text in this section */

        *py/argcheck.o*(.literal* .text*)
        *py/asm*.o*(.literal* .text*)
        *py/bc.o*(.literal* .text*)
        *py/binary.o*(.literal* .text*)
        *py/builtin*.o*(.literal* .text*)
        *py/compile.o*(.literal* .text*)
        *py/emit*.o*(.literal* .text*)
        *py/persistentcode*.o*(.literal* .text*)
        *py/formatfloat.o*(.literal* .text*)
        *py/frozenmod.o*(.literal* .text*)
        *py/gc.o*(.literal* .text*)
        *py/reader*.o*(.literal* .text*)
        *py/lexer*.o*(.literal* .text*)
        *py/malloc*.o*(.literal* .text*)
        *py/map*.o*(.literal* .text*)
        *py/mod*.o*(.literal* .text*)
        *py/mpprint.o*(.literal* .text*)
        *py/mpstate.o*(.literal* .text*)
        *py/mpz.o*(.literal* .text*)
        *py/native*.o*(.literal* .text*)
        *py/nlr*.o*(.literal* .text*)
        *py/obj*.o*(.literal* .text*)
        *py/opmethods.o*(.literal* .text*)
        *py/parse*.o*(.literal* .text*)
        *py/qstr.o*(.literal* .text*)
        *py/repl.o*(.literal* .text*)
        *py/runtime.o*(.literal* .text*)
        *py/scheduler.o*(.literal* .text*)
        *py/scope.o*(.literal* .text*)
        *py/sequence.o*(.literal* .text*)
        *py/showbc.o*(.literal* .text*)
        *py/smallint.o*(.literal* .text*)
        *py/stackctrl.o*(.literal* .text*)
        *py/stream.o*(.literal* .text*)
        *py/unicode.o*(.literal* .text*)
        *py/vm.o*(.literal* .text*)
        *py/vstr.o*(.literal* .text*)
        *py/warning.o*(.literal* .text*)

        *extmod/*.o*(.literal* .text*)

        *lib/oofatfs/*.o*(.literal*, .text*)
        */libaxtls.a:(.literal*, .text*)
        *lib/berkeley-db-1.xx/*.o(.literal*, .text*)
        *lib/libm/*.o*(.literal*, .text*)
        *lib/mp-readline/*.o(.literal*, .text*)
        *lib/netutils/*.o*(.literal*, .text*)
        *lib/timeutils/*.o*(.literal*, .text*)
        *lib/utils/*.o*(.literal*, .text*)

        *stmhal/pybstdio.o(.literal*, .text*)

        build/main.o(.literal* .text*)
        *gccollect.o(.literal* .text*)
        *gchelper.o(.literal* .text*)
        *help.o(.literal* .text*)
        *lexerstr32.o(.literal* .text*)
        *utils.o(.literal* .text*)
        *modpyb.o(.literal*, .text*)
        *machine_pin.o(.literal*, .text*)
        *machine_pwm.o(.literal*, .text*)
        *machine_rtc.o(.literal*, .text*)
        *machine_adc.o(.literal*, .text*)
        *machine_uart.o(.literal*, .text*)
        *modpybi2c.o(.literal*, .text*)
        *modmachine.o(.literal*, .text*)
        *machine_wdt.o(.literal*, .text*)
        *machine_spi.o(.literal*, .text*)
        *machine_hspi.o(.literal*, .text*)
        *hspi.o(.literal*, .text*)
        *modesp.o(.literal* .text*)
        *modnetwork.o(.literal* .text*)
        *moduos.o(.literal* .text*)
        *modutime.o(.literal* .text*)
        *modlwip.o(.literal* .text*)
        *modsocket.o(.literal* .text*)
        *modonewire.o(.literal* .text*)

        /* we put as much rodata as possible in this section */
        /* note that only rodata accessed as a machine word is allowed here */
        *py/qstr.o(.rodata.const_pool)
        *.o(.rodata.mp_type_*) /* catches type: mp_obj_type_t */
        *.o(.rodata.*_locals_dict*) /* catches types: mp_obj_dict_t, mp_map_elem_t */
        *.o(.rodata.mp_module_*) /* catches types: mp_obj_module_t, mp_obj_dict_t, mp_map_elem_t */
        */frozen.o(.rodata.mp_frozen_sizes) /* frozen modules */
        */frozen.o(.rodata.mp_frozen_content) /* frozen modules */

        /* for -mforce-l32 */
        build/*.o(.rodata*)

        _irom0_text_end = ABSOLUTE(.);
    } >irom0_0_seg :irom0_0_phdr

    .text : ALIGN(4)
    {
        _stext = .;
        _text_start = ABSOLUTE(.);
        *(.UserEnter.text)
        . = ALIGN(16);
        *(.DebugExceptionVector.text)
        . = ALIGN(16);
        *(.NMIExceptionVector.text)
        . = ALIGN(16);
        *(.KernelExceptionVector.text)
        LONG(0)
        LONG(0)
        LONG(0)
        LONG(0)
        . = ALIGN(16);
        *(.UserExceptionVector.text)
        LONG(0)
        LONG(0)
        LONG(0)
        LONG(0)
        . = ALIGN(16);
        *(.DoubleExceptionVector.text)
        LONG(0)
        LONG(0)
        LONG(0)
        LONG(0)
        . = ALIGN (16);
        *(.entry.text)
        *(.init.literal)
        *(.init)
        *(.literal .text .literal.* .text.* .iram0.literal .iram0.text .iram0.text.*.literal .iram0.text.*)
        *(.stub .gnu.warning .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
        *(.fini.literal)
        *(.fini)
        *(.gnu.version)
        _text_end = ABSOLUTE(.);
        _etext = .;
    } >iram1_0_seg :iram1_0_phdr

    .lit4 : ALIGN(4)
    {
        _lit4_start = ABSOLUTE(.);
        *(*.lit4)
        *(.lit4.*)
        *(.gnu.linkonce.lit4.*)
        _lit4_end = ABSOLUTE(.);
    } >iram1_0_seg :iram1_0_phdr

    .data : ALIGN(4)
    {
        _data_start = ABSOLUTE(.);
        *(.data)
        *(.data.*)
        *(.gnu.linkonce.d.*)
        *(.data1)
        *(.sdata)
        *(.sdata.*)
        *(.gnu.linkonce.s.*)
        *(.sdata2)
        *(.sdata2.*)
        *(.gnu.linkonce.s2.*)
        *(.jcr)
        _data_end = ABSOLUTE(.);
    } >dram0_0_seg :dram0_0_phdr

    .rodata : ALIGN(4)
    {
        _rodata_start = ABSOLUTE(.);
        *(.sdk.version)
        *(.rodata)
        *(.rodata.*)
        *(.gnu.linkonce.r.*)
        *(.rodata1)
        __XT_EXCEPTION_TABLE__ = ABSOLUTE(.);
        *(.xt_except_table)
        *(.gcc_except_table)
        *(.gnu.linkonce.e.*)
        *(.gnu.version_r)
        *(.eh_frame)
        /*  C++ constructor and destructor tables, properly ordered:  */
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
        /*  C++ exception handlers table:  */
        __XT_EXCEPTION_DESCS__ = ABSOLUTE(.);
        *(.xt_except_desc)
        *(.gnu.linkonce.h.*)
        __XT_EXCEPTION_DESCS_END__ = ABSOLUTE(.);
        *(.xt_except_desc_end)
        *(.dynamic)
        *(.gnu.version_d)
        . = ALIGN(4);        /* this table MUST be 4-byte aligned */
        _bss_table_start = ABSOLUTE(.);
        LONG(_bss_start)
        LONG(_bss_end)
        _bss_table_end = ABSOLUTE(.);
        _rodata_end = ABSOLUTE(.);
    } >dram0_0_seg :dram0_0_phdr

    .bss ALIGN(8) (NOLOAD) : ALIGN(4)
    {
        . = ALIGN (8);
        _bss_start = ABSOLUTE(.);
        *(.dynsbss)
        *(.sbss)
        *(.sbss.*)
        *(.gnu.linkonce.sb.*)
        *(.scommon)
        *(.sbss2)
        *(.sbss2.*)
        *(.gnu.linkonce.sb2.*)
        *(.dynbss)
        *(.bss)
        *(.bss.*)
        *(.gnu.linkonce.b.*)
        *(COMMON)
        . = ALIGN (8);
        _bss_end = ABSOLUTE(.);
        _heap_start = ABSOLUTE(.);
    } >dram0_0_seg :dram0_0_bss_phdr
}

/* get ROM code address */
INCLUDE "eagle.rom.addr.v6.ld"
