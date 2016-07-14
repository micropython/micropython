/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * AT91SAM7X256 memory setup.
 */
__und_stack_size__	= 0x0004;
__abt_stack_size__	= 0x0004;
__fiq_stack_size__	= 0x0010;
__irq_stack_size__	= 0x0080;
__svc_stack_size__	= 0x0004;
__sys_stack_size__	= 0x0400;
__stacks_total_size__	= __und_stack_size__ + __abt_stack_size__ + __fiq_stack_size__ + __irq_stack_size__ + __svc_stack_size__ + __sys_stack_size__;

MEMORY
{
    flash : org = 0x100000, len = 256k
    ram : org = 0x200020, len = 64k - 0x20
}

__ram_start__		= ORIGIN(ram);
__ram_size__		= LENGTH(ram);
__ram_end__		= __ram_start__ + __ram_size__;

SECTIONS
{
	. = 0;

	.text : ALIGN(16) SUBALIGN(16)
	{
        _text = .;
        KEEP(*(vectors))
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
        *(.glue_7t)
        *(.glue_7)
        *(.gcc*)
        *(.ctors)
        *(.dtors)
    } > flash

    .ARM.extab : {*(.ARM.extab* .gnu.linkonce.armextab.*)}

    __exidx_start = .;
    .ARM.exidx : {*(.ARM.exidx* .gnu.linkonce.armexidx.*)} > flash
    __exidx_end = .;

    .eh_frame_hdr : {*(.eh_frame_hdr)}

    .eh_frame : ONLY_IF_RO {*(.eh_frame)}

    . = ALIGN(4);
    _etext = .;
    _textdata = _etext;

    .data :
    {
        _data = .;
        *(.data)
        . = ALIGN(4);
        *(.data.*)
        . = ALIGN(4);
        *(.ramtext)
        . = ALIGN(4);
        _edata = .;
    } > ram AT > flash

    .bss :
    {
        _bss_start = .;
        *(.bss)
        . = ALIGN(4);
        *(.bss.*)
        . = ALIGN(4);
        *(COMMON)
        . = ALIGN(4);
        _bss_end = .;
    } > ram    
}

PROVIDE(end = .);
_end = .;

__heap_base__              = _end;
__heap_end__               = __ram_end__ - __stacks_total_size__;
__main_thread_stack_base__ = __ram_end__ - __stacks_total_size__;
