.. _renesas-ra_program_in_flash:

Write a program in internal file system
=======================================

Internal file system
--------------------

The FAT file system is created and initialized in the RA MCU's internal
flash when the MicroPython starts at the first time on the board.
The file system is mounted as "/flash", so you can access this flash system
and create a program file into the /flash directory.

As the factory setting, following size is allocated for the file system:

===============  ===================
    Board         File System Size
---------------  -------------------
 EK-RA4M1           36KB ( 36864B)
 EK-RA4W1           64KB ( 65536B)
 EK-RA6M1           64KB ( 65536B)
 EK-RA6M2          128KB (131072B)
 RA4M1 CLICKER      36KB ( 36864B)
===============  ===================

As the factory setting, following 2 files are created in the file system:

* boot.py : executed first when the system starts
* main.py : executed after boot.py completes

Write a program in the internal file system
-------------------------------------------

You can write a program in main.py which is executed automatically
when the MicroPython starts. For example, you can write LED blinking
program like below::

    import os
    os.getcwd()
    f = open('main.py', 'rw+')
    print(f.read())
    f.write('import time\n')
    f.write('from machine import Pin\n')
    f.write('led1 = Pin(Pin.cpu.P106)\n')
    f.write('while True:\n')
    f.write('    led1.on()\n')
    f.write('    time.sleep(1)\n')
    f.write('    led1.off()\n')
    f.write('    time.sleep(1)\n')
    f.close()
    f = open('main.py', 'r')
    print(f.read())
    f.close()

Entering CTRL-D for software reset, the MicroPython reboots, displays
following messages::

    MPY: sync filesystems
    MPY: soft reboot

and main.py is executed and LED1 blinks per 1 second.
If you want to stop the program, please enter CTRL-C.
