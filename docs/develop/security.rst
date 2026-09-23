.. _security:

Security policy
===============

This chapter is MicroPython's security policy. It covers what is in scope, how to report a
vulnerability, how reports are handled, which parts of MicroPython are memory safe, and which
features opt out of that.

Scope
-----

This policy covers the MicroPython core (``py/``, ``extmod/``, ``shared/``), the maintained ports
under ``ports/``, ``mpy-cross``, the tools under ``tools/`` such as ``mpremote``, and the packages
in `micropython-lib <https://github.com/micropython/micropython-lib>`__, whether they are frozen
into an official build or installed with ``mip``. Code you have modified from an official release is
out of scope.

A memory-safety defect in MicroPython that can be triggered from ordinary Python code,
or by untrusted input passed to a standard function, is in scope. We are particularly interested in
reports where untrusted input, such as network data or a file, causes memory corruption.

Vulnerabilities in MicroPython's Python code, such as frozen modules and micropython-lib packages,
are also in scope. Ordinary Python code can't corrupt memory, but it can still mishandle untrusted
input, for example by skipping TLS certificate checks, allowing path traversal, or parsing a
protocol incorrectly.

Issues arising solely from the features listed in :ref:`security_unmanaged`, or from loading
untrusted ``.mpy`` files (see :ref:`security_mpy`), are generally not treated as vulnerabilities in
MicroPython itself. A ``.mpy`` file can contain native machine code with full access to memory and
hardware registers, so on most microcontrollers there is no way to contain it. The only mitigation
is to not load untrusted ``.mpy`` files at all.

Third-party code
~~~~~~~~~~~~~~~~

Issues originating in third-party code vendored under ``lib/`` (eg mbedtls) or in a vendor SDK (eg
ESP-IDF) should be reported to that project first. Several MicroPython-attributed CVEs have been
dependency issues of this kind (CVE-2025-59438 in mbedtls, CVE-2020-12638 in the ESP-IDF Wi-Fi
stack). The upstream project is where the fix is coordinated.

We still want to know about them. If a dependency issue affects an official MicroPython build, link
the upstream report or CVE in a public security report, or report it privately if confidential
handling is warranted. We will assess the affected ports and releases, track the required update,
and publish any MicroPython-specific remediation.

Reporting a vulnerability
-------------------------

Most security issues should be reported publicly using the `security report form
<https://github.com/micropython/micropython/issues/new?template=security.yml>`_. It applies the
``security`` label and lets the issue and its fix be discussed in the open.

If you believe an issue is readily exploitable, has high impact, or otherwise needs confidential
handling, use GitHub's `private vulnerability reporting
<https://github.com/micropython/micropython/security/advisories/new>`_ instead. This opens a draft
security advisory visible only to you and the maintainers, with an optional temporary private fork
for developing a fix before anything is disclosed. If you cannot use GitHub, email
contact@micropython.org. Do not disclose the details publicly while a report is handled privately.
If you are unsure which route is appropriate, use the private advisory.

What to include
~~~~~~~~~~~~~~~

Please include:

* The affected version (shown in the startup banner after boot or a soft reset, ``Ctrl-D``, in the
  REPL) or commit hash, and whether you are running an official build or a modified one.
* The affected port, board, and any non-default build configuration (``mpconfigport.h`` /
  ``mpconfigboard.h`` options, native modules, ``MICROPY_PREVIEW_VERSION_2``).
* A minimal reproducible example pasted directly into the report.
* For sanitizer-based reports, the full sanitizer trace, together with the compiler version and the
  build command and options used to produce it.
* The impact: what an attacker gains, and what access they need to trigger it (eg the ability to
  run arbitrary Python at the REPL, load an untrusted ``.mpy``, or supply input to a specific API).
* Any mitigation or workaround you are aware of.

Please avoid destructive proof-of-concept payloads. Reporting is expected to follow the project
`Code of Conduct <https://github.com/micropython/micropython/blob/master/CODEOFCONDUCT.md>`_.

Handling reports
----------------

The timelines below are best-effort targets and aren't contractual guarantees. We will:

1. Acknowledge your report within 7 days.
2. Triage it, confirming the issue, determining affected versions and assessing severity, then share
   our assessment with you.
3. For public reports, develop and review the fix in the open. For private reports, keep the details
   confidential until the fix is published or the agreed disclosure date is reached.
4. Publish the fix and, where warranted, request a CVE.

Security reports normally remain on the `public issue tracker under the security label
<https://github.com/micropython/micropython/issues?q=label%3Asecurity>`__. This is the
project's public record of security issues. Reports received privately are assessed to decide
whether confidential handling is warranted. When it is not, we may ask the reporter to open a public
issue, or publish the advisory, so the fix can be reviewed in the open.

Disclosure and embargo
~~~~~~~~~~~~~~~~~~~~~~

For reports handled privately, we practise coordinated disclosure and ask that you give us a
reasonable opportunity to fix the issue before disclosing it publicly. Our target embargo is at most
90 days from acknowledgement. We aim to release a fix well inside that window and will agree a
public disclosure date with you. If an issue is being actively exploited, we may bring disclosure
forward.

CVEs
~~~~

When a security issue is identified through a public report, private report, testing, normal
development, or retrospective review, we assess whether it warrants a CVE. Where appropriate, we
request one through GitHub's security advisory workflow, which assigns CVEs under its own CVE
Numbering Authority, and include it with the published fix when assigned. An upstream CVE normally
remains the identifier for a dependency vulnerability. MicroPython may request a separate CVE when
its integration, configuration, or release handling creates a distinct vulnerability that users
need to track separately.

CVEs are assigned against tagged releases. A fix that has only landed on an unreleased ``-preview``
or development snapshot is tracked as a fix commit until it ships in a release.

Credit
~~~~~~

We do not run a paid bug-bounty programme, but we credit reporters with the published fix or CVE
unless you ask us not to.

Supported versions
------------------

Security fixes are applied to the latest release and the current development branch. MicroPython
has a single release line with no long-term-support branches, so a deployed device gets fixes by
upgrading. See :ref:`releaseandversioning`.

The security model
------------------

MicroPython compiles Python code to bytecode and runs it in a virtual machine (VM) written in C.
Ordinary Python code has no pointers and cannot read or write arbitrary memory addresses. The VM
checks every access, so a bug in the application raises an exception (``IndexError``,
``MemoryError``, ``RuntimeError``, and so on) instead of corrupting memory.

For code that doesn't use the features below, memory-safety bugs can therefore only come from
MicroPython's own C code (the compiler, VM, object model, VFS, and the modules that access
hardware), not from the application.

.. _security_unmanaged:

Features that bypass memory safety
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following features exist to give Python code direct access to memory, hardware registers, flash
or machine code. The VM does not check what this code does, so it can read or write anything C code
could. Code that uses these features should be reviewed as if it were C, and securing it is the
firmware author's responsibility:

* ``machine.mem8`` / ``machine.mem16`` / ``machine.mem32``: direct, unchecked memory and register
  access.
* ``@micropython.native`` and ``@micropython.viper``: native code emitters. ``viper`` exposes raw
  ``ptr``, ``ptr8``, ``ptr16`` and ``ptr32`` types with no bounds checking.
* ``@micropython.asm_thumb`` and ``@micropython.asm_rv32``: inline assembly.
* Native C extension modules (``dynruntime.h`` / ``.mpy`` native modules).
* Raw flash and block device access, such as ``esp32.Partition``, ``rp2.Flash`` and ``pyb.Flash``:
  writes can overwrite the firmware, frozen code or the filesystem.

.. _security_mpy:

Loading untrusted bytecode
~~~~~~~~~~~~~~~~~~~~~~~~~~

MicroPython does not check that a ``.mpy`` file is valid before running it. A malformed or modified
``.mpy`` file can crash MicroPython or corrupt memory, so loading one from an untrusted source is
equivalent to running untrusted native code. A product that stores or receives ``.mpy`` files must
protect them from modification, as it would a firmware image.

Frozen modules are built into the firmware image, so they can only be changed by someone who can
update the firmware. ``.mpy`` files in ROMFS or on the filesystem, including those deployed with
``mpy-cross`` and ``mpremote``, can be changed by anyone with REPL, ``mpremote`` or file access to
the device, so they are only as trusted as that access.

Isolation
~~~~~~~~~

MicroPython does not use a Memory Protection Unit to isolate Python tasks from each other or from
the interpreter, and its stack-overflow protection (``MICROPY_STACK_CHECK``) is a software counter
check. Devices requiring hardware-enforced isolation between mutually distrusting components must
not rely on the VM boundary alone to provide it.

Development practices
---------------------

All changes are submitted as pull requests and reviewed before merge, including for memory handling
and untrusted input. CI runs the test suite under AddressSanitizer and UndefinedBehaviorSanitizer on
the unix port for core changes (see ``.github/workflows/`` for the current set of checks). The
project does not currently run continuous fuzzing.

Guidance for product developers
-------------------------------

If you are shipping a product on MicroPython:

* **Control what code the device runs.** Most MicroPython security reports need an attacker to run
  Python code of their choosing. A device that disables the REPL and only runs code from protected
  storage avoids that. Exposing a REPL, or accepting ``.mpy`` files or scripts that anyone can
  write, makes attacks much easier.
* **Review uses of features that bypass memory safety.** Code in the application or build that uses
  the features listed in :ref:`security_unmanaged` should be reviewed like C code.
* **Pin and track releases.** MicroPython has a single release line and fixes aren't backported, so
  getting a fix onto a device means upgrading to a newer release. See :ref:`releaseandversioning`.
* **Watch for security fixes.** There's no list of downstream vendors that get notified. Fixes are
  published through the `security label
  <https://github.com/micropython/micropython/issues?q=label%3Asecurity>`__, GitHub security
  advisories and the release notes, so watch those. If a private report affects a particular port or
  board, we may bring in its maintainers or vendor before disclosure.
