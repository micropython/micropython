/*
*****************************************************************************
**
**  File        : linker_stm32f2xx_dfu.ld
**
**  Abstract    : Linker script for STM32F205xx Device with
**                1MByte FLASH, 128KByte SRAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : STMicroelectronics STM32
**
**  Environment : Eclipse-CDT and GNU Tools ARM Embedded.
**
*****************************************************************************
*/

/* default stack sizes.

These are used by the startup in order to allocate stacks for the different modes.
*/

__Stack_Size = 2048 ;

PROVIDE ( _Stack_Size = __Stack_Size ) ;

__Stack_Init = _estack - __Stack_Size ;

/*"PROVIDE" allows to easily override these values from an object file or the commmand line.*/
PROVIDE ( _Stack_Init = __Stack_Init ) ;

/*
There will be a link error if there is not this amount of RAM free at the end.
*/
_Minimum_Stack_Size = 0x1400;

/* include the memory spaces definitions sub-script */
/*
Linker subscript for STM32F205xx definitions with 1024K Flash and 128K RAM
*/

/* Memory Spaces Definitions */

MEMORY
{
  RAM (xrw)     : ORIGIN = 0x20000000, LENGTH = 128K
  APP_FLASH (rx)    : ORIGIN = 0x08020000, LENGTH = 1024K-0x20000
  DCT1_FLASH (rx) : ORIGIN = 0x08004000, LENGTH = 16K
  DCT2_FLASH (rx) : ORIGIN = 0x08008000, LENGTH = 16K
  INCLUDE backup_ram_memory.ld
}

INCLUDE linker_stm32f2xx_common.ld
