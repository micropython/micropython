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
  There is no (currently) :mod:`_thread`.
* **Event notifies behave like interrupts.**  When the firmware signals an
  event, its notify function can preempt running Python at a raised task-priority
  level (TPL).  The port therefore treats notifies like hardware ISRs: the C
  notify does the minimum and *schedules* Python callbacks to run later at a safe
  point (they never run inside the notify).  See the timing/async design for
  details.
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
targets. The current build setup does not support 32 bit builds, since they are
very rare on modern machines, but there is no reason why they should not work.



Building and running
--------------------

The image is built with a standalone Clang + ``lld-link`` toolchain that emits a
PE32+ UEFI application, and its Python libraries are frozen in via the manifest,
so the resulting ``micropython.efi`` is self-contained.

Development and testing is driven from ``ports/uefi/`` with a single ``Makefile``
that is both the build and the task runner, and everything runs in a reproducible
container:

* ``make build`` -- build ``mpy-cross``, the ``.efi`` image, and stage a QEMU ESP.
* ``make test`` -- boot a headless self-test build under QEMU and return non-zero
  on failure (the authoritative pass/fail signal).
* ``make run`` -- launch the interactive serial REPL under QEMU.
* ``make run-gfx`` -- launch with a graphics display window (host only).

``ARCH=aa64`` (the default, and the fast native target on Apple-Silicon dev
machines) selects aarch64; ``ARCH=x64`` selects x86-64.  See
:ref:`uefi_intro` in the tutorial for a step-by-step walkthrough, and the
``MicroPython-UEFI-DevEnv.md`` document in the port directory for the full
toolchain and automation reference.

Capability tiers
----------------

The port is organised into capability tiers, each building on the previous one:

* **Tier 0 -- interactive REPL.**  Console/serial I/O, timing, the garbage
  collector, exception handling and frozen modules.  Full language, no
  filesystem.
* **Tier 1 -- scripting environment.**  A filesystem (VFS over
  ``EFI_SIMPLE_FILE_SYSTEM``) for ``import``/``open`` off the ESP, wall-clock
  :mod:`time`, ``machine`` basics, and :mod:`asyncio` wired onto ``WaitForEvent``.
  This is the sweet spot: a scriptable firmware environment for validation,
  manufacturing-line automation, bring-up tooling and register poking.
* **Tier 2 -- hardware / firmware introspection.**  The :mod:`uefi` module:
  the protocol/handle database, PCI I/O, NVRAM variables, events and timers,
  device paths, graphics output, the text console, image loading and the boot
  manager.  A live, programmable replacement for ad-hoc UEFI-shell scripts.
* **Tier 3 -- networking.**  The standard :mod:`socket`, :mod:`network` and
  :mod:`ssl` modules over the firmware's native EFI network stack, with
  :mod:`asyncio` integration and TLS via ``EFI_TLS_PROTOCOL``.

What is and isn't available
---------------------------

Available: the full language with double-precision floats; :mod:`machine`
(``mem*``, ``reset``, ``freq``, ``unique_id``, ``idle``, sleep, ``WDT``,
``Timer``); a filesystem with ``import``/``open``/:mod:`os`; :mod:`time`
(monotonic ticks and wall clock); :mod:`asyncio`, :mod:`select` and pollable
``sys.stdin``; :mod:`os.urandom`/:mod:`random` seeded from the firmware RNG; a
broad set of pure standard-library modules (:mod:`re`, :mod:`json`,
:mod:`deflate`, :mod:`binascii`, :mod:`hashlib`, :mod:`framebuf`, ...); and the
port-specific :mod:`uefi` module.

Networking is available through the standard :mod:`socket`, :mod:`network` and
:mod:`ssl` modules (TCP/UDP, client and server, DNS, ICMP ping, and TLS over the
firmware's ``EFI_TLS_PROTOCOL``).

Not available: :mod:`_thread` (no threading model in boot services).

Cautions specific to this environment
-------------------------------------

* **No memory protection.**  A wild pointer via ``machine.mem*`` or a misused
  :mod:`uefi` protocol call can hang or corrupt the platform.  There is no OS to
  catch you.
* **NVRAM is not scratch storage.**  ``SetVariable`` (see :mod:`uefi.variable`)
  writes SPI flash and some firmware rate-limits or is damaged by abuse -- fine
  for a few config keys, wrong for a general datastore.
* **The text console is quirky.**  Prefer the serial REPL; the UEFI text console
  can mangle control characters and cursor handling on some firmware.
* **Secure Boot / Variable Policy.**  Some operations (image loading, variable
  writes) may be refused by platform security policy; the :mod:`uefi` module
  surfaces the firmware status rather than hiding it.
