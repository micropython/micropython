.. _uefi_quickref:

Quick reference for the UEFI port
=================================

The UEFI port runs MicroPython as a UEFI (Unified Extensible Firmware Interface)
BIOS **application**. This is the environment that runs on a PC prior to the OS
being loaded. Python code here has access to the full set of Boot Services and
Runtime Services protocols, allowing you to interact with PC hardware at a low level.

While the PC pre-boot environment is not memory-constrained as it is on other
microcontroller ports, prior to OS boot the BIOS offers few services, but it does
allows direct access to physical memory and hardware registers, making MicroPython
a good fit for running at this stage.

The MicroPython UEFI port provides both low level access to the core UEFI functions
for hardware discovery and also higher level wrappers for the major standard PC
components (console, filesystem, timers, events, NVRAM variables, PCI, graphics).
The port supports **x86-64** (Microsoft x64 ABI) and **aarch64** (AAPCS64).

Below is a quick reference.  If it is your first time working with this port it
may be useful to get an overview of the environment and its limitations:

.. toctree::
   :maxdepth: 1

   general.rst
   tutorial/index.rst

Installing MicroPython
----------------------

The port builds to a single ``micropython.efi`` PE32+ image with its Python
libraries frozen in, so it has no external dependencies at run time.  Typically
installation simply involves copying this file to the EFI partition on the target
machine (e.g. ``/boot/efi`` on Linux machines). See the tutorial section
:ref:`uefi_intro` for building the image and running it under QEMU/OVMF or from
the UEFI shell on real hardware. The ``micropython.efi`` can also be set as a
boot target option in using the standard ``Boot####`` mechanism.

General usage
-------------

The MicroPython REPL is available over either the the UEFI **text console**
(``ConOut``/``ConIn``) or the **serial port** if present on the hardware.
Serial is byte-clean and handles control characters reliably; it should be
noted that the text console can mangle control/cursor handling on some firmware
and is natively UTF-16.

Both serial and console interfaces offer TAB completion, arrow-key line editing,
paste mode (ctrl-E) and Ctrl-C interrupts.

The :mod:`machine` module::

    import machine

    machine.freq()          # get the CPU/counter frequency in Hz
    machine.reset()         # reset the platform (ResetSystem)
    machine.unique_id()     # SMBIOS system UUID
    machine.idle()          # sleep until the next event (yields to firmware)

Direct memory access (the environment is ring 0, flat/identity-mapped, so a
``machine.mem*`` access is a bare pointer dereference)::

    machine.mem32[0xFED00000]            # read a 32-bit MMIO register
    machine.mem8[addr] = 0x5a            # write a byte

The port-specific :mod:`uefi` module gives Python access to the firmware itself
-- the protocol/handle database, Boot/Runtime Services, NVRAM variables, events
and timers.  See :mod:`uefi` for the full reference::

    import uefi

    # Enumerate every handle and the protocols it carries (like the shell's `dh`)
    for h in uefi.handle.all_handles():
        print(h, h.protocols())

Delay and timing
----------------

Use the :mod:`time` module::

    import time

    time.sleep(1)           # sleep for 1 second (yields to events/asyncio)
    time.sleep_ms(500)      # sleep for 500 milliseconds
    time.sleep_us(10)       # busy-wait for 10 microseconds (BootServices->Stall)
    start = time.ticks_ms() # get millisecond counter
    delta = time.ticks_diff(time.ticks_ms(), start)

    time.localtime()        # wall-clock time from the firmware RTC (GetTime)

The monotonic timebase comes from a hardware counter (``CNTVCT_EL0`` on aarch64,
``RDTSC`` on x86-64); wall-clock time comes from ``RuntimeServices->GetTime``.

Timers
------

Use the :ref:`machine.Timer <machine.Timer>` class::

    from machine import Timer

    def tick(t):
        print('tick')

    tim = Timer(-1, mode=Timer.PERIODIC, period=1000, callback=tick)
    tim.deinit()

Timer callbacks are *scheduled* (soft) -- they run from the main loop at a safe
point, not inside the firmware notify, so they may allocate and call into the VM
freely.

Filesystem
----------

When the application is launched from a FAT volume (the EFI System Partition or
any other ``EFI_SIMPLE_FILE_SYSTEM``), that **boot volume is auto-mounted at
``/``**, backed by ``EFI_FILE_PROTOCOL``::

    import os

    os.listdir('/')
    os.stat('/hello.txt')
    f = open('/hello.txt')
    print(f.read())
    f.close()

    import mymodule        # imported from /mymodule.py on the ESP

``open()``, ``import`` from disk, and the :mod:`os` filesystem functions
(``listdir``/``ilistdir``/``stat``/``mkdir``/``remove``/``rmdir``/``getcwd``/
``chdir``/``statvfs``) are all supported.

Other ``EFI_SIMPLE_FILE_SYSTEM`` volumes (USB sticks, other partitions -- the
shell's ``FS0:``/``FS1:``) can be mounted with :mod:`uefi.fs`, which wraps each as
a VFS for :func:`os.mount`::

    import os
    import uefi.fs as fs

    fs.mount(fs.volumes()[0], '/usb')   # or fs.mount_all() for every volume
    print(os.listdir('/usb'))
    fs.umount('/usb')

Asyncio
-------

:mod:`asyncio` works cooperatively, driven by the firmware's ``WaitForEvent``
idle primitive, so tasks idle efficiently (the CPU sleeps) instead of spinning::

    import asyncio

    async def main():
        await asyncio.sleep(1)
        print('done')

    asyncio.run(main())

``sys.stdin`` is a pollable stream, so ``asyncio`` streams over the REPL input
work as well.  UEFI events and timers can be awaited via the :mod:`uefi` module
(:class:`uefi.Event`, :class:`uefi.event.Timer`, :class:`uefi.event.ProtocolWatch`).

Networking
----------

Networking runs over the firmware's native EFI network stack (no vendored TCP/IP),
using the standard MicroPython :mod:`network`, :mod:`socket` and :mod:`ssl`
modules.  Both a **wired** interface (:class:`network.LAN`) and **WiFi**
(:class:`network.WLAN`, station mode) are supported, and both share the same L3
path (DHCP, sockets, DNS, TLS).

Bring an interface up and configure IPv4 (DHCP or static) with :mod:`network`.
The wired interface is :class:`network.LAN` over ``EFI_SIMPLE_NETWORK_PROTOCOL``
and ``EFI_IP4_CONFIG2_PROTOCOL``::

    import network

    nic = network.LAN()
    nic.active(True)
    nic.ipconfig(dhcp4=True)             # or ipconfig(addr4='192.168.1.10/24', gw4='192.168.1.1')
    print(nic.isconnected(), nic.ipconfig('addr4'), nic.config('mac'))

``network.ping(ip)`` sends an ICMP echo (numeric IPv4), and ``network.ipconfig(
dns='8.8.8.8')`` overrides the resolver used by ``socket.getaddrinfo``.

WiFi
~~~~

:class:`network.WLAN` drives WiFi in **station mode** (WPA2-PSK) over the firmware's
``EFI_WIRELESS_MAC_CONNECTION_II`` (WiFi2) and supplicant protocols.  It reuses the
same DHCP/socket/TLS path as :class:`~network.LAN`::

    import network, time

    wlan = network.WLAN(network.WLAN.IF_STA)
    wlan.active(True)                       # bind the IP stack (does not touch the radio)

    for ssid, bssid, chan, rssi, security, hidden in wlan.scan():
        print(ssid, 'quality', rssi, 'sec', security)

    wlan.connect('your-ssid', 'your-password')   # non-blocking; open network omits the key
    wlan.ipconfig(dhcp4=True)
    while wlan.status() == network.STAT_CONNECTING:
        time.sleep_ms(250)                  # association can take 10-20 s
    if wlan.isconnected():
        print(wlan.ipconfig('addr4'))       # sockets/ssl now work as on the wired LAN

``connect()`` is non-blocking -- it starts the scan/association and returns; poll
``status()`` until it leaves ``STAT_CONNECTING``.  The status codes are
``STAT_IDLE``, ``STAT_CONNECTING``, ``STAT_GOT_IP``, ``STAT_NO_AP_FOUND``,
``STAT_WRONG_PASSWORD`` and ``STAT_CONNECT_FAIL`` (available both on the
:class:`~network.WLAN` class and as :mod:`network` module constants).  ``scan()``
returns ``(ssid, bssid, channel, rssi, security, hidden)`` tuples, and
``config('mac'|'ssid'|'channel'|'hostname')`` / ``config(hostname=...)`` query and
set interface parameters.

.. note::

    WiFi is **station-only and IPv4**: there is no AP/SoftAP mode (UEFI exposes no
    such protocol), and because EFI WiFi2 carries no BSSID or channel in a scan
    result, ``bssid`` is ``b''``, ``channel`` is ``0``, and ``rssi`` /
    ``status('rssi')`` is a 0--100 link-quality figure rather than dBm.  Pinning a
    specific AP by ``bssid`` is not supported.  There is no 802.11 in QEMU, so WiFi
    only runs on real hardware with UEFI WiFi2 drivers.

:mod:`socket` provides TCP and UDP over ``EFI_TCP4``/``EFI_UDP4``, with name
resolution via ``EFI_DNS4``.  Both **client and server** sockets are supported
(``bind``/``listen``/``accept``)::

    import socket

    # client
    addr = socket.getaddrinfo('example.com', 80)[0][-1]
    s = socket.socket()
    s.connect(addr)
    s.send(b'GET / HTTP/1.0\r\n\r\n')
    print(s.recv(512))
    s.close()

    # server
    srv = socket.socket()
    srv.bind(('0.0.0.0', 8080))
    srv.listen(1)
    conn, peer = srv.accept()

TLS is available through :mod:`ssl` / ``tls`` (``ssl.wrap_socket`` and
``ssl.SSLContext``).  The backend is chosen at **build time** (``TLS=`` on the
``make`` command line): ``mbedtls`` (the default) uses the vendored
``lib/mbedtls`` and is fully freestanding; ``efi`` instead drives the firmware's
``EFI_TLS_PROTOCOL``, which ships no crypto of its own so it makes the image
~187 KB smaller but needs the network firmware (``wrap_socket`` raises
``OSError(ENODEV)`` without it); ``none`` omits TLS entirely.  Whichever backend is
built presents the same API -- client and server endpoints
(``PROTOCOL_TLS_CLIENT`` / ``PROTOCOL_TLS_SERVER``), a full handshake, and
certificate and hostname verification (``CERT_NONE``/``OPTIONAL``/``REQUIRED``)::

    import socket, ssl

    addr = socket.getaddrinfo('example.com', 443)[0][-1]
    s = socket.socket()
    s.connect(addr)
    s = ssl.wrap_socket(s, server_hostname='example.com')
    s.send(b'GET / HTTP/1.0\r\n\r\n')
    print(s.read(512))

See the networking scripts in the port's ``samples/`` directory (``net_info``,
``net_ping``, ``net_get``, ``net_https``, ``net_server``, ``net_boot`` and
``wifi_connect``) for full programs; run them with ``make run`` after building the
network firmware (``wifi_connect`` needs real WiFi hardware).

Graphics
--------

When the firmware publishes a Graphics Output Protocol (GOP) display,
:class:`uefi.Display` exposes it as a :mod:`framebuf` ``FrameBuffer``::

    import uefi
    from uefi.display import rgb565

    d = uefi.Display()
    d.fb.fill(0)
    d.fb.text('hello UEFI', 20, 20, rgb565(255, 255, 255))
    d.show()

Draw with any :mod:`framebuf` method on ``d.fb``, then call ``d.show()``.  See
:mod:`uefi.display`, and the ``samples/`` directory in the port (``clock.py``,
``draw.py``) run with ``make run-gfx``.

Randomness
----------

:func:`os.urandom` and the :mod:`random` module are seeded from
``EFI_RNG_PROTOCOL`` where the firmware provides it, falling back to ``RDRAND``
(x86-64) and finally a counter-seeded PRNG.

Built-in modules
----------------

Besides the port-specific :mod:`uefi` module and the standard :mod:`machine`,
:mod:`time`, :mod:`os` and :mod:`asyncio`, the build includes :mod:`network`,
:mod:`socket`, :mod:`ssl`, :mod:`re`, :mod:`json`, :mod:`deflate`,
:mod:`binascii`, :mod:`hashlib`, :mod:`heapq`, :mod:`cmath`, :mod:`math`,
:mod:`random`, :mod:`framebuf`, :mod:`platform`, :mod:`errno`, :mod:`select`,
:mod:`struct`, :mod:`io`, :mod:`collections` and :mod:`uctypes`.  Run
``help('modules')`` at the REPL to list what is available in a given build.

Not available
-------------

* :mod:`_thread` -- the boot-services environment is cooperative and
  single-threaded; use :mod:`asyncio` for concurrency.
