.. currentmodule:: stm

:mod:`stm` --- functionality specific to STM32 MCUs
===================================================

.. module:: stm
    :synopsis: functionality specific to STM32 MCUs

This module provides functionality specific to STM32 microcontrollers, including
direct access to peripheral registers.

Memory access
-------------

The module exposes three objects used for raw memory access.

.. data:: mem8

    Read/write 8 bits of memory.

.. data:: mem16

    Read/write 16 bits of memory.

.. data:: mem32

    Read/write 32 bits of memory.

Use subscript notation ``[...]`` to index these objects with the address of
interest.

These memory objects can be used in combination with the peripheral register
constants to read and write registers of the MCU hardware peripherals, as well
as all other areas of address space.


Peripheral register constants
-----------------------------

The module defines constants for registers which are generated from CMSIS header
files, and the constants available depend on the microcontroller series that is
being compiled for.  Examples of some constants include:

.. data:: GPIOA

    Base address of the GPIOA peripheral.

.. data:: GPIOB

    Base address of the GPIOB peripheral.

.. data:: GPIO_BSRR

    Offset of the GPIO bit set/reset register.

.. data:: GPIO_IDR

    Offset of the GPIO input data register.

.. data:: GPIO_ODR

    Offset of the GPIO output data register.

Constants that are named after a peripheral, like ``GPIOA``, are the absolute
address of that peripheral.  Constants that have a prefix which is the name of a
peripheral, like ``GPIO_BSRR``, are relative offsets of the register.  Accessing
peripheral registers requires adding the absolute base address of the peripheral
and the relative register offset.  For example ``GPIOA + GPIO_BSRR`` is the
full, absolute address of the ``GPIOA->BSRR`` register.

Example use:

.. code-block:: python3

    # set PA2 high
    stm.mem32[stm.GPIOA + stm.GPIO_BSRR] = 1 << 2

    # read PA3
    value = (stm.mem32[stm.GPIOA + stm.GPIO_IDR] >> 3) & 1


Functions specific to STM32WBxx MCUs
------------------------------------

These functions are available on STM32WBxx microcontrollers, and interact with
the second CPU, the RF core.

.. function:: rfcore_status()

    Returns the status of the second CPU as an integer (the first word of device
    info table).

.. function:: rfcore_fw_version(id)

    Get the version of the firmware running on the second CPU.  Pass in 0 for
    *id* to get the FUS version, and 1 to get the WS version.

    Returns a 5-tuple with the full version number.

.. function:: rfcore_sys_hci(ogf, ocf, data, timeout_ms=0)

    Execute a HCI command on the SYS channel.  The execution is synchronous.

    Returns a bytes object with the result of the SYS command.
