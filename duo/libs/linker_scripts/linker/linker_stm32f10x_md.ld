/*
*****************************************************************************
**
**  File        : linker_stm32f10x_md.ld
**
**  Abstract    : Linker script for STM32F103CB Device with
**                128KByte FLASH, 20KByte RAM
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

MEMORY
{
  RAM (xrw)     : ORIGIN = 0x20000000, LENGTH = 20K
  APP_FLASH (rx)    : ORIGIN = 0x08000000, LENGTH = 128K
}

INCLUDE linker_stm32f10x_common.ld
