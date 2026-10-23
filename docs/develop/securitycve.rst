.. _security_cve:

Does this bug need a CVE?
=========================

Fixing a security bug still needs a normal code change, review and release. A CVE adds a public
record that lets people shipping MicroPython identify affected versions and track the fix. This
guidance explains when a bug needs that record. It is a companion to the :ref:`security policy
<security>`.

You don't need to know the CVE process before reporting a possible security bug. Describe the
behaviour, which versions are affected and what input triggers it. The maintainers assess the
report and request a CVE where appropriate.

What a CVE does
---------------

CVE stands for Common Vulnerabilities and Exposures. A CVE record gives a vulnerability a shared
identifier, such as ``CVE-2025-59438``, with a description, affected versions and references to the
fix. Product developers and vulnerability scanners use these records to find security fixes in the
software they ship.

A CVE isn't a replacement for a bug report or pull request. It makes the security impact and
affected releases visible outside the project's issue tracker.

Not every bug needs one. An incorrect result or a Python exception is usually an ordinary bug.
Reading outside a buffer, accepting an invalid certificate or hanging the device in response to a
network message can have security consequences. Someone supplying the input may gain access or
control they shouldn't have.

What makes a bug security-related?
-----------------------------------

Start with two questions: who can supply the input, and what happens when they do?

For example, a malformed network packet that makes a Python parser raise ``ValueError`` is usually
an input-handling bug. The same packet causing a C buffer overflow is a security bug. The caller
should be able to supply data, not overwrite the interpreter's memory.

Ordinary Python code also matters. Python code can call functions and manipulate objects, but it
shouldn't be able to read or overwrite arbitrary memory. If a bug in MicroPython's C code allows a
standard Python operation to access memory outside its objects or buffers, it breaks that
protection. Such a bug can warrant a CVE even when triggering it requires the ability to run Python.

Features that deliberately provide direct memory or machine-code access are different. Code using
``machine.mem32``, viper pointer types such as ``ptr8``, or inline assembler already has that
access. A mistake in such code isn't, by itself, a vulnerability in MicroPython.

The steps below separate these cases.

Outcomes
--------

The assessment can result in:

* **CVE:** a vulnerability in released MicroPython code that needs its own record.
* **Security-related bug without a CVE:** a hardening change or behaviour likely to cause security
  mistakes in applications, but without enough demonstrated impact to warrant a CVE. Fix it
  publicly and explain the behaviour in the changelog.
* **Ordinary bug:** fix it through the normal issue and pull request process, without a CVE.
* **Not shipped:** the faulty code never appeared in a non-preview release.
* **Out of scope:** the code isn't covered by the project's security policy.
* **Upstream CVE:** the defect belongs to a bundled library or vendor SDK. Track its existing
  identifier and the MicroPython releases affected by it.

A security report doesn't have to start with a firm choice between these outcomes. Record what you
know and which part is uncertain.

Step 1: did the faulty code ship?
---------------------------------

A MicroPython CVE needs faulty code in at least one tagged, non-preview release. A bug introduced
and fixed between releases hasn't shipped.

* Identify affected releases and the introducing commit where known. Record the fixing commit if a
  fix exists.
* ``git tag --contains <commit>`` can help identify releases containing a commit. Release tags use
  ``vX.Y`` or ``vX.Y.Z``.
* If the introducing commit is unknown, record the earliest release you can confirm is affected.

The covered code includes:

* the interpreter and its modules: ``py/``, ``extmod/``, ``shared/`` and ``drivers/``
* maintained ports under ``ports/``, as listed in the :ref:`port support documentation
  <support_tiers>`
* ``mpy-cross`` and shipped tools such as ``mpremote``
* micropython-lib packages, whether frozen into firmware or installed with ``mip``

The assessment excludes tests, examples, CI scripts, non-shipped tools, user-modified builds and
ports not maintained upstream. Debug-only builds and options documented as experimental are also
excluded. Code behind a build option is excluded if no in-tree board enables it, unless the option
is documented for users to enable.

For a defect in bundled third-party code or a vendor SDK, see :ref:`security_cve_dependencies`.

Step 2: who can trigger it?
---------------------------

Identify the input needed to reach the bug. This could be a packet, a file, data from a peripheral
or a Python operation. Describe the access needed to supply it, rather than assuming that every
device exposes the same interfaces.

Data arriving through an interface
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Data from another system isn't necessarily trustworthy, even if the connection is wired or the
device normally sits inside the product. Consider:

.. list-table::
   :header-rows: 1
   :widths: 25 35 40

   * - Input
     - Who can supply it
     - Examples of code handling it
   * - Network traffic
     - A peer, server or someone able to alter traffic
     - sockets, TLS, WebREPL, network drivers
   * - Wi-Fi co-processor messages
     - The co-processor firmware, which handles radio traffic
     - SPI, SDIO or UART driver integration
   * - Bluetooth or other radio traffic
     - A nearby transmitter or controller firmware
     - Bluetooth bindings, ESP-NOW receive paths
   * - USB traffic
     - The connected host
     - USB descriptors, CDC, MSC, HID and ``usb.device``
   * - Serial and bus traffic
     - The device or peripheral at the other end
     - UART, SPI, I2C, I2S, CAN and 1-Wire
   * - Files or filesystem images
     - Whoever supplied or modified the file or media
     - parsers, filesystem mounts and storage drivers

A memory-corruption bug caused by peripheral data isn't excluded just because a particular product
solders that peripheral onto its board. The CVE should name the interface so each product developer
can decide whether their device is exposed.

Distinguish data MicroPython interprets from data it only passes to application code. If a UART
returns bytes and the application's parser mishandles them, the defect is in the application. If
MicroPython's C driver copies those bytes beyond its buffer, the defect is in MicroPython.

The same distinction applies to API arguments. Network data passed to ``json.loads`` is a normal use
of the API. If an application requests an enormous buffer and receives ``MemoryError``, that isn't a
vulnerability.

Python code and deliberately unsafe features
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Ordinary Python means standard types and modules without features that deliberately bypass memory
safety. Memory corruption reachable from ordinary Python is covered, as are crashes in the core VM
under Step 4.

The following inputs need different treatment:

* **eval, exec and the REPL:** these execute Python by design. Giving someone access isn't itself an
  interpreter bug, although an unexpectedly exposed REPL or insecure shipped default can be one.
* **Unmanaged features:** direct memory access, ``uctypes`` addresses and descriptors, native and
  viper emitters, inline assembler, native modules and raw flash access deliberately permit
  operations the VM cannot contain. Problems arising solely from that access aren't normally
  MicroPython vulnerabilities.
* **Untrusted .mpy files:** MicroPython doesn't validate these before execution, and they can
  contain native code. A product must protect them from modification as it would its firmware.
  Loading one isn't a safe way to run untrusted input.
* **Untrusted Python source given to mpy-cross:** compiling source shouldn't corrupt memory or
  execute attacker-supplied code on the host. Either result warrants assessment for a CVE.
* **Physical attacks outside normal interfaces:** reflashing, debug probes and fault injection
  aren't normally covered. They are covered if they bypass protection a port promises to provide.
  Assess normal peripheral input as interface data, not as a physical attack.

Running code by design doesn't excuse a separate memory-corruption bug or core VM crash. Apply
Step 4 to the actual defect, not just the name of the API used to reach it.

Step 3: what does it do?
------------------------

Describe the result in terms of the code's behaviour. You don't need to demonstrate a working
exploit to report memory corruption.

Memory corruption and unintended reads
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

These defects let input escape the interpreter's normal object and buffer checks:

* **Out-of-bounds read or write:** an index, length or copy reaches outside the allocated buffer. A
  read can expose unrelated data; a write can damage objects or interpreter state.
* **Integer overflow in a size calculation:** a calculation such as ``n * size`` wraps, causing the
  code to allocate a smaller buffer than needed. A subsequent copy can then exceed the buffer.
  Assess the arithmetic error and the out-of-bounds access as one defect.
* **Use-after-free:** C code keeps using an object or buffer after it has been freed. This can
  happen when the garbage collector cannot see a retained pointer, or an interrupt or callback
  outlives a buffer.
* **Type confusion:** C code treats an object as a different type and reads or writes fields using
  the wrong layout.
* **Unchecked C stack exhaustion:** input depth drives recursion beyond the C stack. A Python
  exception raised by a working stack check is different; it prevents this failure.

Bounds checks, overflow checks, type checks and new garbage-collector roots are useful signals when
reviewing a fix. They don't establish security impact on their own; inspect the faulty operation and
how it can be reached.

Crashes
~~~~~~~

A hard fault, failed C ``assert``, NULL pointer dereference or abort is a crash. An uncaught Python
exception isn't: the interpreter is still enforcing its checks.

A crash in the core VM from ordinary Python warrants a CVE under this guidance, without needing an
exploit. A port or driver crash from Python alone is normally an ordinary bug unless there is
evidence of memory corruption. A crash from external input also needs the denial-of-service
assessment below.

A sanitizer report can help distinguish these cases. AddressSanitizer reporting a
heap-buffer-overflow or use-after-free is evidence of memory corruption. A NULL pointer dereference
alone doesn't establish that.

Authentication, verification and defaults
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A bug can warrant a CVE without corrupting memory. Examples include accepting a certificate that
should be rejected, bypassing authentication or disclosing data to a caller who shouldn't receive
it.

A shipped default can also be a vulnerability if it grants access users weren't told to expect.
Examples include a service with no password or a known password, an unexpected REPL, or disabled
certificate verification when the documentation says certificates are checked. A documented setting
that the application deliberately selects isn't the same as a faulty default.

Python modules can have these bugs too. Path traversal means a supplied filename escapes the
intended directory; injection means supplied data is treated as a command or code. Assess the actual
consequence and a plausible application use, rather than treating every parsing or logic error as a
vulnerability.

Denial of service
~~~~~~~~~~~~~~~~~

Denial of service means preventing the device from doing its job, for example by hanging it or
repeatedly resetting it. It warrants a CVE when input through an exposed interface reliably causes
that failure. The input must be reasonably sized for the use case, and causing the failure must cost
the sender much less effort than the device spends handling it.

A small request that leaks memory on each connection can qualify if repeated requests eventually
exhaust the device's memory. Sending more data than a link or device can handle doesn't qualify on
its own. Neither do radio jamming, a catchable exception or a large allocation failing normally.
Restarting the device doesn't remove a vulnerability if the same input can immediately stop it
again.

Step 4: putting it together
---------------------------

Use the input from Step 2 and the effect from Step 3 to select the outcome. Apply the release and
scope checks first.

.. list-table::
   :header-rows: 1
   :widths: 30 40 30

   * - Who can trigger it (Step 2)
     - What it does (Step 3)
     - Outcome
   * - Data through an interface
     - Memory corruption, info disclosure, auth / cert bypass
     - **CVE**
   * - Data through an interface
     - Crash, hang or reset meeting the DoS test
     - **CVE**, usually lower severity
   * - Data through an interface, via micropython-lib / frozen Python
     - Path traversal, injection, skipped verification, with plausible impact
     - **CVE**
   * - Shipped default
     - Insecure default as described above
     - **CVE**
   * - Ordinary Python code
     - Memory corruption anywhere: out-of-bounds reads / writes, use-after-free, type confusion
     - **CVE**, severity reflects needing code execution
   * - Ordinary Python code
     - Crash in the core VM (``py/``, Python-facing ``extmod/``): hard fault, failed assert, NULL
       deref, abort
     - **CVE**, low severity, no exploit needed
   * - Ordinary Python code
     - Crash in a port or driver, with nothing showing corruption
     - **Ordinary bug** (or security-related bug if it looks like corruption)
   * - Ordinary Python code
     - Uncaught exception, resource exhaustion, deep recursion caught by the stack check
     - **Ordinary bug**
   * - Untrusted source given to ``mpy-cross``
     - Memory corruption or code execution on the host
     - **CVE**
   * - Unmanaged features, ``.mpy``, ``eval`` / REPL, app-supplied arguments
     - Anything other than memory corruption or a core VM crash
     - **Ordinary bug**
   * - Anything
     - API behaviour likely to lead to security bugs in apps, or a hardening fix with no
       demonstrated impact
     - **Security-related bug**
   * - Anything
     - Bug entirely in a dependency
     - **Upstream CVE**, see :ref:`security_cve_dependencies`

In the table, info disclosure means revealing data the caller shouldn't receive. Auth / cert bypass
means bypassing authentication or certificate verification. DoS means denial of service.

The access needed to trigger a vulnerability affects its severity, not just whether it deserves a
record. A bug requiring Python execution is different from one reachable by any network peer. Record
that distinction so product developers can assess their own exposure.

.. _security_cve_dependencies:

Dependencies
------------

If the defect is entirely in mbedTLS, lwIP, NimBLE, ESP-IDF or another dependency, use the upstream
vulnerability record rather than requesting a second MicroPython CVE for the same bug. If there
isn't a record yet, coordinate with the upstream project.

MicroPython still needs to identify which ports and releases bundled the affected dependency, and
which release updated it. A separate MicroPython CVE is appropriate if the integration creates a
distinct defect. Examples include an unsafe library configuration, a missing check the library
requires or a bug introduced by a local patch.

What to do with the fix
-----------------------

For an ordinary bug, use the normal issue and pull request process. Use the ``security-related``
issue label for bugs with possible security impact, whether or not they ultimately need a CVE. The
label flags the issue for assessment; maintainers decide whether to request a CVE. Follow the
:ref:`security policy <security>` for issues needing private handling.

Explain the security-relevant behaviour in the report and changelog. Give the maintainers enough
information to assess it:

* the defect and the input or operation that reaches it
* what access is needed to supply that input, and what the resulting failure permits
* affected releases, ports, boards and build options
* the introducing and fixing commits, where known
* relevant source locations, existing reports and regression tests
* whether a fix is available and which release includes it

Check for an existing CVE or GitHub security advisory before requesting another. One root cause
normally gets one CVE, even when several reports or crash sites describe it. Separate defects may
need separate records.

**If the bug is already fixed:** it can still need a CVE. Include the fix and any changelog entry
describing the defect. If that entry was the first public disclosure, use its release date as the
disclosure date, even if it didn't call the bug a security issue.

The maintainers request CVEs through GitHub's security advisory workflow. The record should explain
the affected code and versions closely enough that someone shipping MicroPython can decide whether
it applies to their product.

You don't need a severity score or a vulnerability classification to start the report. CVSS
(Common Vulnerability Scoring System) describes severity, and CWE (Common Weakness Enumeration)
describes the type of defect. These are recorded after the impact is understood; they don't decide
whether the bug needs a CVE.
