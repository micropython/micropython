The Flash Driver
================

Flash is a re-programmable memory that retains program and data
storage even with power off.

User can write or read several bytes from any valid address in a flash.

As to the erase/lock/unlock command, the input parameter of address should
be a bytes address aligned with the page start, otherwise, the command will fail
to be executed. At the meantime, the number of pages that can be locked or unlocked
at once depends on region size of the flash. User can get the real number
from the function return value which could be different for the different devices.

Features
--------

* Initialization/de-initialization
* Writing/Reading bytes
* Locking/Unlocking/Erasing pages
* Notifications about errors or being ready for a new command

Applications
------------

* Mini disk which can retain program and data storage
* Boot loader
* Non volatile storage

Dependencies
------------

The peripheral which controls a re-programmable flash memory.

Concurrency
-----------

N/A

Limitations
-----------

User should pay attention to set a proper stack size in their application,
since the driver manages a temporary buffer in stack to cache unchanged data
when calling flash write and erase function.
Due to flash memory architecture of SAMD21/D20/L21/L22/C20/C21/D09/D10/D11/R21,
write operation erazes row content before each write.

Known issues and workarounds
----------------------------

N/A
