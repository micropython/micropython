.. _esp8266_general:

General information about the ESP8266 port
==========================================

ESP8266 is a popular WiFi-enabled System-on-Chip (SoC) by Espressif Systems.

Multitude of boards
-------------------

There is a multitude of modules and boards from different sources which carry
the ESP8266 chip. MicroPython tries to provide a generic port which would run on
as many boards/modules as possible, but there may be limitations. Adafruit
Feather HUZZAH board is taken as a reference board for the port (for example,
testing is performed on it). If you have another board, please make sure you
have a datasheet, schematics and other reference materials for your board
handy to look up various aspects of your board functioning.

To make a generic ESP8266 port and support as many boards as possible,
the following design and implementation decision were made:

* GPIO pin numbering is based on ESP8266 chip numbering, not some "logical"
  numbering of a particular board. Please have the manual/pin diagram of your board
  at hand to find correspondence between your board pins and actual ESP8266 pins.
  We also encourage users of various boards to share this mapping via MicroPython
  forum, with the idea to collect community-maintained reference materials
  eventually.
* All pins which make sense to support, are supported by MicroPython
  (for example, pins which are used to connect SPI flash
  are not exposed, as they're unlikely useful for anything else, and
  operating on them will lead to board lock-up). However, any particular
  board may expose only subset of pins. Consult your board reference manual.
* Some boards may lack external pins/internal connectivity to support
  ESP8266 deepsleep mode.


Technical specifications and SoC datasheets
-------------------------------------------

The datasheets and other reference material for ESP8266 chip are available
from the vendor site: http://bbs.espressif.com/viewtopic.php?f=67&t=225 .
They are the primary reference for the chip technical specifications, capabilities,
operating modes, internal functioning, etc.

For your convenience, some of technical specifications are provided below:

* Architecture: Xtensa lx106
* CPU frequency: 80MHz overclockable to 160MHz
* Total RAM available: 96KB (part of it reserved for system)
* BootROM: 64KB
* Internal FlashROM: None
* External FlashROM: code and data, via SPI Flash. Normal sizes 512KB-4MB.
* GPIO: 16 + 1 (GPIOs are multiplexed with other functions, including
  external FlashROM, UART, deep sleep wake-up, etc.)
* UART: One RX/TX UART (no hardware handshaking), one TX-only UART.
* SPI: 2 SPI interfaces (one used for FlashROM).
* I2C: No native external I2C (bitbang implementation available on any pins).
* I2S: 1.
* Programming: using BootROM bootloader from UART. Due to external FlashROM
  and always-available BootROM bootloader, ESP8266 is not brickable.


Scarcity of runtime resources
-----------------------------

ESP8266 has very modest resources (first of all, RAM memory). So, please
avoid allocating too big container objects (lists, dictionaries) and
buffers. There is also no full-fledged OS to keep track of resources
and automatically clean them up, so that's the task of a user/user
application: please be sure to close open files, sockets, etc. as soon
as possible after use.


Boot process
------------

See :doc:`/reference/reset_boot`.

Known Issues
------------

Real-time clock
~~~~~~~~~~~~~~~

RTC in ESP8266 has very bad accuracy, drift may be seconds per minute. As
a workaround, to measure short enough intervals you can use
``time.time()``, etc. functions, and for wall clock time, synchronize from
the net using included ``ntptime.py`` module.

Due to limitations of the ESP8266 chip the internal real-time clock (RTC)
will overflow every 7:45h.  If a long-term working RTC time is required then
``time()`` or ``localtime()`` must be called at least once within 7 hours.
MicroPython will then handle the overflow.

Simultaneous operation of STA_IF and AP_IF
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Simultaneous operation of STA_IF and AP_IF interfaces is supported.

However, due to restrictions of the hardware, there may be performance
issues in the AP_IF, if the STA_IF is not connected and searching.
An application should manage these interfaces and for example
deactivate the STA_IF in environments where only the AP_IF is used.

Sockets and WiFi buffers overflow
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Socket instances remain active until they are explicitly closed. This has two
consequences. Firstly they occupy RAM, so an application which opens sockets
without closing them may eventually run out of memory. Secondly not properly
closed socket can cause the low-level part of the vendor WiFi stack to emit
``Lmac`` errors. This occurs if data comes in for a socket and is not
processed in a timely manner. This can overflow the WiFi stack input queue
and lead to a deadlock. The only recovery is by a hard reset.

The above may also happen after an application terminates and quits to the REPL
for any reason including an exception. Subsequent arrival of data provokes the
failure with the above error message repeatedly issued. So, sockets should be
closed in any case, regardless whether an application terminates successfully
or by an exception, for example using try/finally::

    sock = socket(...)
    try:
        # Use sock
    finally:
        sock.close()


SSL/TLS limitations
~~~~~~~~~~~~~~~~~~~

ESP8266 uses `axTLS <http://axtls.sourceforge.net/>`_ library, which is one
of the smallest TLS libraries with compatible licensing. However, it
also has some known issues/limitations:

1. No support for Diffie-Hellman (DH) key exchange and Elliptic-curve
   cryptography (ECC). This means it can't work with sites which require
   the use of these features (it works ok with the typical sites that use
   RSA certificates).
2. Half-duplex communication nature. axTLS uses a single buffer for both
   sending and receiving, which leads to considerable memory saving and
   works well with protocols like HTTP. But there may be problems with
   protocols which don't follow classic request-response model.

Besides axTLS's own limitations, the configuration used for MicroPython is
highly optimized for code size, which leads to additional limitations
(these may be lifted in the future):

3. Optimized RSA algorithms are not enabled, which may lead to slow
   SSL handshakes.
4. Session Reuse is not enabled, which means every connection must undergo
   the full, expensive SSL handshake.

Besides axTLS specific limitations described above, there's another generic
limitation with usage of TLS on the low-memory devices:

5. The TLS standard specifies the maximum length of the TLS record (unit
   of TLS communication, the entire record must be buffered before it can
   be processed) as 16KB. That's almost half of the available ESP8266 memory,
   and inside a more or less advanced application would be hard to allocate
   due to memory fragmentation issues. As a compromise, a smaller buffer is
   used, with the idea that the most interesting usage for SSL would be
   accessing various REST APIs, which usually require much smaller messages.
   The buffers size is on the order of 5KB, and is adjusted from time to
   time, taking as a reference being able to access https://google.com .
   The smaller buffer however means that some sites can't be accessed using
   it, and it's not possible to stream large amounts of data. axTLS does
   have support for TLS's Max Fragment Size extension, but no HTTPS website
   does, so use of the extension is really only effective for local
   communication with other devices.

There are also some not implemented features specifically in MicroPython's
``ssl`` module based on axTLS:

6. Certificates are not validated (this makes connections susceptible
   to man-in-the-middle attacks).
7. There is no support for client certificates (scheduled to be fixed in
   1.9.4 release).
