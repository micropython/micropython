.. _stm32_general:

General Information about the STM32 port
========================================

STM32 is a family of microcontroller ICs based on 32-bit RISC architecture
such as Cortex-M33F, Cortex-M7F, Cortex-M4F, Cortex-M3, Cortex-M0+, and
Cortex-M0 cores.

The STM microcontroller boards provides the facility of accessing the memory
directly, to perform any read or write function.

It exposes three objects::

* stm.mem8 : access 8 bits of memory

* stm.mem16 : access 16 bits of memory

* stm.mem32 : access a 32 bit word memory

They can be used in combination with the offset constants to read, write and
control registers of the MCU hardware peripherals and all other areas of MCU's
memory.

Example:-

..code-block:: python3

    stm.mem32[stm.TIM3 + stm.TIM_CR1] 
    #return the contents of Timer 3's Control Register 1 
    #(yes Timer 3 - one of the ones that uPy won't let you access with pyb.Timer
    stm.mem32[stm.TIM3 + stm.TIM_CR1] = value
    #yes ... set the value of Timer 3's Control Register 1

rfcore_status, rfcore_fw_version, rfcore_sys_hci  

