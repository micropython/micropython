.. _uefi_scripting:

Writing firmware scripts
========================

Once the REPL is up, the UEFI port behaves like a normal MicroPython system: you
can import modules off the boot volume, read and write files, keep time, and run
cooperative :mod:`asyncio` tasks.  This chapter shows the everyday scripting
surface.

Files and imports
-----------------

The boot volume is mounted at ``/``.  Reading and writing files uses the standard
built-ins::

    with open('/config.txt') as f:
        for line in f:
            print(line.rstrip())

    with open('/log.txt', 'a') as f:
        f.write('booted\n')

The :mod:`os` module provides directory and metadata operations::

    import os
    os.listdir('/')
    os.mkdir('/data')
    os.stat('/config.txt')
    os.getcwd()
    os.chdir('/data')
    os.statvfs('/')

Placing a ``.py`` file on the volume makes it importable::

    # /helpers.py
    def greet(name):
        return 'hello ' + name

::

    >>> import helpers
    >>> helpers.greet('uefi')
    'hello uefi'

Timing
------

Monotonic timing comes from a hardware counter; wall-clock time from the firmware
RTC::

    import time

    t0 = time.ticks_us()
    # ... do something ...
    print('took', time.ticks_diff(time.ticks_us(), t0), 'us')

    print(time.localtime())   # (year, month, day, hour, minute, second, ...)
    print(time.time())        # seconds since the Unix epoch

``time.sleep``/``sleep_ms`` yield to the firmware (and to any running
:mod:`asyncio` tasks and timers) while waiting; ``sleep_us`` is a precise
busy-wait for short delays.

Timers
------

A :ref:`machine.Timer <machine.Timer>` fires a Python callback periodically or
once.  The callback is scheduled to run at a safe point in the main loop, so it
can allocate and do real work::

    from machine import Timer

    count = 0
    def tick(t):
        global count
        count += 1
        print('tick', count)

    tim = Timer(-1, mode=Timer.PERIODIC, period=500, callback=tick)
    time.sleep(3)     # ~6 ticks print during this sleep
    tim.deinit()

Asyncio
-------

:mod:`asyncio` runs cooperatively on top of the firmware's ``WaitForEvent`` idle
primitive, so tasks sleep the CPU rather than spinning::

    import asyncio

    async def worker(name, delay):
        for i in range(3):
            await asyncio.sleep_ms(delay)
            print(name, i)

    async def main():
        await asyncio.gather(
            worker('a', 100),
            worker('b', 250),
        )

    asyncio.run(main())

Because ``sys.stdin`` is a pollable stream, ``asyncio`` can also read the REPL
input without blocking the loop.  UEFI events, timers and hot-plug notifications
can be awaited directly -- see :mod:`uefi.event` and the
:ref:`introspection tutorial <uefi_introspection>`.

A note on safety
----------------

There is no memory protection in this environment: a stray ``machine.mem*``
write, or a misused firmware protocol, can hang or corrupt the platform.  Develop
against QEMU first, and treat writes to NVRAM variables (which wear flash) and to
hardware registers with care.
