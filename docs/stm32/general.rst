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
        
        # set PA2 high
        stm.mem32[stm.GPIOA + stm.GPIO_BSRR] = 1 << 2

        # read PA3
        value = (stm.mem32[stm.GPIOA + stm.GPIO_IDR] >> 3) & 1
    
We also have the following functions:-

* rfcore_status() - Returns the status as an integer (the first word of the device info table)

* rfcore_fw_version(id) -  Get the version of the firmware running on the second CPU. Pass                                id=0 to get the FUS version, and id=1 to get the WS version.                                    Returns a 5-tuple with the full version number.
     
*rfcore_sys_hci(ogf, ocf, data, timeout_ms=0) - execute a HCI command on the SYS channel.  
                                                The execution is synchronous. Returns a bytes                                                   object with the result of the SYS command.

