/*
*****************************************************************************
**
**  File        : linker_stm32f10x_hd_dfu.ld
**
**  Abstract    : Linker script for stm32F103RC Device with
**                256KByte FLASH, 48KByte RAM
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
  APP_FLASH (rx)    : ORIGIN = 0x08005000, LENGTH = 256K-0x5000
}

INCLUDE linker_stm32f10x_common.ld
