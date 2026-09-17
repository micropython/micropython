.. _uefi_general:

General information about the UEFI port
=======================================

The UEFI port runs the MicroPython interpreter as a UEFI (Unified Extensible Firmware Interface)
**application**.  This lets Python run in the firmware environment -- during boot, before an operating
system is loaded -- with direct access to the platform hardware and to the rich
set of services the firmware already provides.

While the PC pre-boot environment is not memory-constrained as it is on other
microcontroller ports, prior to OS boot the BIOS offers few services, but it does
allows direct access to physical memory and hardware registers, making MicroPython
a good fit for running at this stage.

Execution model
---------------

The port runs as a normal UEFI application and deliberately stays **before the UEFI
``ExitBootServices``**.  This keeps the UEFI console, filesystem, timers, events,
NVRAM variables and hardware protocols available as Boot Services and Runtime
Services, which is what makes a useful scripting environment possible.

The consequences of the UEFI execution model shape the whole port:

* **Ring 0, flat, identity-mapped, single address space.**  There is no memory
  protection between Python and the platform.  ``machine.mem8/16/32[addr]`` is a
  bare pointer dereference, and MMIO/port I/O is directly reachable.  This is
  powerful and dangerous in equal measure -- a bad pointer can hang or corrupt
  the platform.
* **Single-threaded and cooperative.**  As with most MicroPython ports, there is no
  preemptive scheduler. Concurrency is however available through :mod:`asyncio`,
  which idles the CPU in the firmware's ``WaitForEvent`` primitive instead of spinning.
  There is no :mod:`_thread`.
* **Event notifies behave like interrupts.**  When the firmware signals an
  event, its notify function can preempt running Python at a raised task-priority
  level (TPL).  The port therefore treats notifies like hardware ISRs: the C
  notify does the minimum and *schedules* Python callbacks to run later at a safe
  point (they never run inside the notify).
* **You are a guest holding the boot timeout.**  The application disables the
  platform watchdog at start-up (although you can restart it if you wish).
  A clean exit returns control to whatever launched the application (for example
  the UEFI shell or boot manager). If the interpreter is installed as a UEFI boot
  option then a non-zero exit (e.g. ``sys.exit(1)``) will cause boot selection to
  proceed to the next phase, and you may implement an OS boot loader entirely in
  Python using the exposed UEFI APIs.

Supported architectures
-----------------------

Two targets are supported and validated:

* **x86-64** -- uses the **Microsoft x64 calling convention** (not System V), as
  mandated for UEFI on this architecture.  This propagates into the low-level
  ABI-sensitive code (exception unwinding, the garbage-collector register scan,
  and the :mod:`uefi` method-call trampoline), which are all built for the MS x64
  ABI.
* **aarch64** -- uses the standard **AAPCS64** calling convention, with no
  Microsoft-ABI divergence.

The floating-point unit is already initialised by the firmware when the
application starts (the UEFI specification requires it), so the port uses
double-precision floats out of the box and never initialises the FPU itself.

The HAL and module code are architecture-agnostic; only the exception-unwinding
assembly, the GC register scan, and the build flags differ between the two
targets.



Building and running
--------------------

The port builds to a single self-contained ``micropython.efi`` -- a PE32+ UEFI
application with the Python standard library and the :mod:`uefi` package frozen
in, so it has no external run-time dependencies.  It can be run under QEMU with
OVMF (x86-64) or AAVMF (aarch64) firmware, or copied to any FAT volume and
launched from the UEFI shell or firmware boot manager on real hardware.

See :ref:`uefi_intro` in the tutorial for a step-by-step walkthrough of building
and running the image.

Capabilities
------------

The port provides:

* **The full MicroPython language**, with double-precision floats, exceptions and
  the garbage collector, over an **interactive REPL** on the UEFI text console or
  serial.
* **A filesystem.**  The volume the application is launched from is auto-mounted
  at ``/`` (a VFS over ``EFI_SIMPLE_FILE_SYSTEM``), giving ``import``/``open`` and
  the :mod:`os` functions; other volumes can be mounted with :mod:`uefi.fs`.
* **:mod:`machine`** -- ``mem*`` memory/MMIO access, ``reset``, ``freq``,
  ``unique_id``, ``idle``, sleep, ``WDT`` and ``Timer``.
* **:mod:`time`** (monotonic ticks and the firmware wall clock), :mod:`asyncio`,
  :mod:`select`, and a pollable ``sys.stdin``.
* **The :mod:`uefi` module** -- the protocol/handle database, PCI I/O, NVRAM
  variables, events and timers, device paths, image loading and the boot manager,
  the text console, and the GOP display as a :mod:`framebuf` ``FrameBuffer``.
* **Networking** through the standard :mod:`socket`, :mod:`network` and :mod:`ssl`
  modules over the firmware's EFI network stack: TCP/UDP (client and server), DNS,
  ICMP ping, wired (:class:`network.LAN`) and WiFi station-mode
  (:class:`network.WLAN`) interfaces, and TLS (vendored mbedtls by default, or the
  firmware's ``EFI_TLS_PROTOCOL`` when built with ``TLS=efi``).
* **:mod:`os.urandom` / :mod:`random`** seeded from the firmware RNG.
* **A broad set of standard-library modules** -- :mod:`re`, :mod:`json`,
  :mod:`deflate`, :mod:`binascii`, :mod:`hashlib`, :mod:`framebuf`, :mod:`struct`,
  :mod:`collections`, :mod:`uctypes` and more.

Run ``help('modules')`` at the REPL to list what a given build includes.

Limitations
-----------

* **No :mod:`_thread`.**  The firmware environment is single-threaded and
  cooperative; use :mod:`asyncio` for concurrency.
* **WiFi is station-only and IPv4.**  :class:`network.WLAN` is a client of an
  existing access point; there is no access-point / SoftAP mode (the firmware
  exposes no such protocol).
* **64-bit only.**  x86-64 and aarch64 are supported; there is no 32-bit build.
* **No native or compiled-code emitters.**  ``@micropython.native`` /
  ``@micropython.viper``, native ``.mpy`` modules and inline assembly are
  unavailable; frozen and ``.mpy`` bytecode is the portable speed path.

Cautions specific to this environment
-------------------------------------

* **NVRAM is not scratch storage.**  ``SetVariable`` (see :mod:`uefi.variable`)
  writes SPI flash and some firmware rate-limits or is damaged by abuse -- fine
  for a few config keys, wrong for a general datastore.
* **Secure Boot / Variable Policy.**  Some operations (image loading, variable
  writes) may be refused by platform security policy; the :mod:`uefi` module
  surfaces the firmware status rather than hiding it.
