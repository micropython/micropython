.. _test_developers:

Writing and Maintaining Tests
=============================

This covers writing and maintaining MicroPython's own test suite, and
advanced test runner usage. It is not about testing user applications
that run on MicroPython. For running tests, see :ref:`test_quickstart`.

Test Directory Structure
------------------------

- ``basics/`` - Core Python language features
- ``unicode/`` - Unicode string handling
- ``float/`` - Floating-point arithmetic
- ``extmod/`` - Extended modules (json, re, deflate, etc.)
- ``extmod_hardware/`` - Hardware-dependent extended module tests
- ``micropython/`` - MicroPython-specific features
- ``import/`` - Import mechanism
- ``io/`` - Input/output operations
- ``misc/`` - Miscellaneous tests, including the example user C modules
- ``stress/`` - Resource limit tests (memory, recursion)
- ``thread/`` - Threading
- ``cmdline/`` - Command-line and REPL
- ``cpydiff/`` - Documented differences from CPython, rendered into the docs
- ``assets/`` - Data files used by other tests (pre-compiled ``.mpy``, ROMFS images)
- ``inlineasm/<arch>/`` - Inline assembly tests per architecture (thumb, xtensa, rv32)
- ``jni/`` - The unix port's jni module
- ``ports/<port_name>/`` - Port-specific tests
- ``feature_check/`` - Target capability detection scripts
- ``target_wiring/`` - Board-specific hardware pin configuration
- ``net_hosted/`` - Network tests needing only host-to-target connectivity
- ``net_inet/`` - Network tests requiring internet access
- ``multi_bluetooth/``, ``multi_espnow/``, ``multi_extmod/``, ``multi_net/``,
  ``multi_pyb_can/``, ``multi_wlan/`` - Multi-instance tests
- ``perf_bench/`` - Performance benchmarks
- ``internal_bench/`` - Low-level VM and C benchmarks

Writing Standard Tests
----------------------

Tests are Python scripts that produce output via ``print()``. The runner
determines pass/fail from that output, in one of three ways:

**CPython comparison** (preferred for standard Python features): the test
runner executes the same script on CPython and compares the outputs. No
extra files are needed:

.. code-block:: python

   # tests/basics/my_feature.py

   l = [1, 2, 3]
   l.append(4)
   print(l)
   print(l.index(4))

**unittest-based tests** (preferred for MicroPython-only features): the
runner recognises the summary that ``unittest`` prints at the end of a run
(``Ran N tests`` then ``OK`` or ``FAILED``) and uses it to determine
pass/fail, so no expected output is needed. This gives better failure
messages and supports skipping individual test cases. The test must run
``unittest.main()`` for the summary to be printed. On the unix port the
runner provides ``unittest`` automatically from the micropython-lib
submodule; on bare-metal targets it must first be installed with
``mpremote mip install unittest``.

.. code-block:: python

   # tests/extmod/my_module_test.py

   try:
       import my_module
   except ImportError:
       print("SKIP")
       raise SystemExit

   import unittest

   class TestMyModule(unittest.TestCase):
       def test_basic(self):
           self.assertEqual(my_module.add(1, 2), 3)

       def test_edge_case(self):
           self.assertRaises(ValueError, my_module.add, None, 1)

   if __name__ == "__main__":
       unittest.main()

**Expected output file**: a ``<test>.py.exp`` file contains the expected
output. Use this when CPython would produce different output or can't run
the test at all, and a unittest-style test doesn't suit (e.g. the exact
printed output is what's being tested). When running with ``--emit native``,
a ``<test>.py.native.exp`` file takes precedence if present. For example:

.. code-block:: python

   # tests/micropython/heap_locked.py

   try:
       import micropython
       micropython.heap_locked
   except (AttributeError, ImportError):
       print("SKIP")
       raise SystemExit

   micropython.heap_lock()
   print(micropython.heap_locked())
   micropython.heap_unlock()
   print(micropython.heap_locked())

With the corresponding expected output file
``tests/micropython/heap_locked.py.exp``::

   1
   0

Unicode
~~~~~~~

When testing name or string-related functionality, cover both
English/ASCII and non-English/non-ASCII text, including Unicode examples,
so that Unicode support is verified across platforms. Add a comment in
English explaining the meaning and intent of any Unicode text used.

Skipping Tests
~~~~~~~~~~~~~~

Where possible, a test should determine at runtime if it should be skipped.
If so, it prints a single line ``SKIP`` before exiting with
``raise SystemExit``, as shown in the examples above. The test runner
parses this output and marks the test as skipped.

This isn't always possible, particularly if the test relies on optional
MicroPython features that prevent parsing the ``.py`` file (i.e. importing
the file raises ``SyntaxError`` before the skip logic can run). In these
cases, there is additional logic for skipping tests based on filename
patterns.

.. _test_filename_patterns:

Filename-Based Skip Patterns
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

These tests are skipped by the runner based on filename patterns:

+-----------------------+----------------------------------------------+
| Pattern               | Requires                                     |
+=======================+==============================================+
| ``native_*``,         | Native or viper code emitter.                |
| ``viper_*``           |                                              |
+-----------------------+----------------------------------------------+
| ``*_endian``          | Byte order to match between host and target. |
+-----------------------+----------------------------------------------+
| ``int_big*``,         | Arbitrary-precision integer support.         |
| ``*_intbig``,         |                                              |
| ``ffi_int*``          |                                              |
+-----------------------+----------------------------------------------+
| ``bytearray*``,       | ``bytearray`` support.                       |
| ``*_bytearray``       |                                              |
+-----------------------+----------------------------------------------+
| ``set_*``,            | ``set`` type support.                        |
| ``frozenset*``,       |                                              |
| ``*_set``             |                                              |
+-----------------------+----------------------------------------------+
| ``*slice*``           | Slice support.                               |
+-----------------------+----------------------------------------------+
| ``async_*``,          | ``async``/``await`` keyword support.         |
| ``asyncio_*``,        |                                              |
| ``*_async``           |                                              |
+-----------------------+----------------------------------------------+
| ``const*``            | ``micropython.const()`` support.             |
+-----------------------+----------------------------------------------+
| ``int_64*``,          | 64-bit integer support.                      |
| ``*_int64``           |                                              |
+-----------------------+----------------------------------------------+
| ``*reverse_op*``      | ``__rOP__`` special methods.                 |
+-----------------------+----------------------------------------------+
| ``string_fstring*``   | f-string support.                            |
+-----------------------+----------------------------------------------+
| ``asm*``              | Inline assembler for the target architecture.|
+-----------------------+----------------------------------------------+

Additional skip lists exist in ``run-tests.py`` for platform-specific and
emitter-specific exclusions (``platform_tests_to_skip``,
``via_mpy_tests_to_skip``, ``emitter_tests_to_skip``).

Advanced run-tests.py Options
-----------------------------

Target Selection
~~~~~~~~~~~~~~~~

The ``-t`` option accepts the following for the test instance:

+---------------------------+--------------------------------------------------+
| Value                     | Behaviour                                        |
+===========================+==================================================+
| ``unix``                  | Use the unix port, specified by                  |
|                           | ``MICROPY_MICROPYTHON`` (defaults to the         |
|                           | standard variant of the unix or windows port     |
|                           | depending on host platform).                     |
+---------------------------+--------------------------------------------------+
| ``webassembly``           | Use the webassembly port, specified by           |
|                           | ``MICROPY_MICROPYTHON_MJS``.                     |
+---------------------------+--------------------------------------------------+
| ``port:<device>``         | Connect to the given serial port device.         |
+---------------------------+--------------------------------------------------+
| ``a<n>``                  | Shortcut for ``/dev/ttyACM<n>``.                 |
+---------------------------+--------------------------------------------------+
| ``u<n>``                  | Shortcut for ``/dev/ttyUSB<n>``.                 |
+---------------------------+--------------------------------------------------+
| ``c<n>``                  | Shortcut for ``COM<n>``.                         |
+---------------------------+--------------------------------------------------+
| ``exec:<command>``        | Execute a command, attach to its stdin/stdout.   |
+---------------------------+--------------------------------------------------+
| ``execpty:<command>``     | Execute a command and attach to the printed      |
|                           | ``/dev/pts/<n>`` device.                         |
+---------------------------+--------------------------------------------------+
| ``<a>.<b>.<c>.<d>``       | Connect to the given IPv4 address.               |
+---------------------------+--------------------------------------------------+
| anything else             | Treated as a serial port path.                   |
+---------------------------+--------------------------------------------------+

Test Filtering
~~~~~~~~~~~~~~

- ``-i REGEX`` - include tests matching pattern (starting from empty set)
- ``-e REGEX`` - exclude tests matching pattern (starting from full set)

Filters are applied as an ordered chain; the last matching regex wins:

.. code-block:: bash

   ./run-tests.py -e viper              # All tests except 'viper' matches
   ./run-tests.py -i float              # Only 'float' matches
   ./run-tests.py -e async -i async_foo # Exclude async, but keep async_foo

Execution Options
~~~~~~~~~~~~~~~~~

- ``--emit <bytecode|native|viper>`` - select code emitter (default: bytecode)
- ``--via-mpy`` - compile ``.py`` files to ``.mpy`` before running
- ``--heapsize <size>`` - set heap size for tests (e.g. ``16k``, ``512k``)
- ``-j N`` - run N tests in parallel (PC targets only)
- ``--begin <file>`` - inject Python code before test execution

Set ``MICROPY_MPYCROSS`` to use a specific ``mpy-cross`` binary with
``--via-mpy``. Set ``MICROPY_MICROPYTHON`` to override the default unix
port binary.

.. code-block:: bash

   ./run-tests.py --emit native
   ./run-tests.py --via-mpy
   ./run-tests.py -j 4
   ./run-tests.py --begin prologue.py --via-mpy --emit native

The ``--begin`` option inserts Python code between VFS setup and module
import on the target. It only takes effect when used with ``--via-mpy``.
This is useful for running larger pre-compiled tests on memory-constrained
boards like ESP8266 where additional configuration (e.g. garbage collection)
is needed before import.

Result Management
~~~~~~~~~~~~~~~~~

- ``-r DIR`` - directory for test results (default: ``results/``)
- ``--print-failures`` - show diff of failed tests
- ``--clean-failures`` - delete ``.exp`` and ``.out`` files from prior runs
- ``--run-failures`` - re-run only previously failed tests

The last three act on the existing results directory and exit without
running any tests.

- ``-c`` - print test output as it is received from the target, rather than
  only on failure. Useful when a test hangs or crashes the target.

Feature Detection
~~~~~~~~~~~~~~~~~

``run-tests.py`` runs scripts in ``feature_check/`` at startup to detect
target capabilities: native architecture, code emitter support, float
precision, byte order, and optional language features (big integers, slices,
async syntax, etc). Results determine which tests are skipped.

Special Test Handling
~~~~~~~~~~~~~~~~~~~~~

Tests in the ``cmdline/`` directory exercise the unix port's command-line
interface (REPL behaviour, verbose output, exit codes) and are executed as
subprocesses rather than via the standard test path. A ``# cmdline:``
comment on the first line supplies extra command-line options:

.. code-block:: python

   # cmdline: -v -v

The expected output for these tests is matched line-by-line as regular
expressions, with a line containing only ``########`` matching any number
of lines.

Writing Multi-Instance Tests
----------------------------

Multi-instance tests coordinate multiple MicroPython instances for testing
communication protocols (networking, Bluetooth).

Each test defines ``instance0()``, ``instance1()``, etc:

.. code-block:: python

   # tests/multi_net/tcp_data.py

   def instance0():
       multitest.globals(IP=multitest.get_network_ip())
       multitest.next()

       import socket
       s = socket.socket()
       s.bind(('0.0.0.0', 8000))
       s.listen(1)
       multitest.broadcast('server ready')

       conn, addr = s.accept()
       data = conn.recv(1024)
       conn.send(data)
       conn.close()
       s.close()

   def instance1():
       multitest.next()
       multitest.wait('server ready')

       import socket
       s = socket.socket()
       s.connect((IP, 8000))
       s.send(b'Hello')
       print(s.recv(1024))
       s.close()

The ``multitest`` module provides:

+---------------------------------------+------------------------------------------+
| Function                              | Description                              |
+=======================================+==========================================+
| ``next()``                            | Synchronise instances at the beginning   |
|                                       | of the test. Each instance function      |
|                                       | should call this exactly once. The test  |
|                                       | runner calls each instance function in   |
|                                       | sequence, and doesn't start the next     |
|                                       | instance until the current one has       |
|                                       | called ``next()``. After the last        |
|                                       | instance has called ``next()`` the       |
|                                       | runner waits for all instances to        |
|                                       | complete.                                |
+---------------------------------------+------------------------------------------+
| ``broadcast(msg)``                    | Send message to all instances.           |
+---------------------------------------+------------------------------------------+
| ``wait(msg)``                         | Block until a specific broadcast is      |
|                                       | received.                                |
+---------------------------------------+------------------------------------------+
| ``globals(**kwargs)``                 | Share variables between instances.       |
+---------------------------------------+------------------------------------------+
| ``get_network_ip()``                  | Get the instance's IP address.           |
+---------------------------------------+------------------------------------------+
| ``expect_reboot(resume, delay_ms=0)`` | Handle device reboots during test.       |
+---------------------------------------+------------------------------------------+
| ``output_metric(data)``               | Emit a metric string for collection by   |
|                                       | the runner.                              |
+---------------------------------------+------------------------------------------+
| ``skip()``                            | Skip the entire test. Can be called by   |
|                                       | any instance.                            |
+---------------------------------------+------------------------------------------+

Running multi-instance tests:

.. code-block:: bash

   # Mix of instance types
   ./run-multitests.py -t unix -t cpython test.py
   ./run-multitests.py -t /dev/ttyACM0 -t /dev/ttyACM1 test.py

   # Test permutations (swap instance assignments)
   ./run-multitests.py -p 2 -t /dev/ttyACM0 -t /dev/ttyACM1 test.py

Hardware Tests and target_wiring
--------------------------------

Tests requiring physical hardware (UART loopback, SPI, pin bridges) use the
``target_wiring`` module for board-specific pin and peripheral configuration.

Configuration files in ``tests/target_wiring/`` (e.g. ``rp2.py``, ``esp32.py``)
export variables like ``uart_loopback_args`` and other peripheral
configuration values. The test runner detects the target platform, loads
the appropriate wiring file, and injects it as ``import target_wiring``
before running tests. See the wiring files in ``tests/target_wiring/``
(e.g. ``rp2.py``, ``esp32.py``) for examples of available variables.

.. code-block:: bash

   # Auto-detect target wiring based on board
   ./run-tests.py -d extmod_hardware -t /dev/ttyACM0

   # Explicit wiring file
   ./run-tests.py --target-wiring target_wiring/rp2.py -t /dev/ttyACM0

Native Module Tests
-------------------

``run-natmodtests.py`` tests dynamic native modules (``.mpy`` files with
machine code). Pre-compiled modules from ``examples/natmod/`` are injected
into the target's RAM via a VFS and imported.

.. code-block:: bash

   # Auto-detect architecture
   ./run-natmodtests.py extmod/btree1.py

   # Specify architecture
   ./run-natmodtests.py -a armv7em extmod/re1.py

   # Run on a board
   ./run-natmodtests.py -t /dev/ttyACM0 extmod/btree1.py

Internal Benchmarks
-------------------

Low-level benchmarks for VM operations and C code performance. Tests use
the ``bench`` module and output execution time as a float (seconds).

.. code-block:: python

   # tests/internal_bench/loop_count-1-range.py
   import bench

   def test(num):
       for i in range(num):
           pass

   bench.run(test)

.. code-block:: bash

   # Run on unix port (default)
   ./run-internalbench.py

   # Run on a board
   ./run-internalbench.py -t /dev/ttyACM0

   # Run specific files
   ./run-internalbench.py internal_bench/loop_count-1-range.py

   # Adjust iteration count for slow targets
   ./run-internalbench.py -I 50000 -t /dev/ttyACM0

SSL/TLS Test Certificates
-------------------------

SSL tests in ``multi_net/`` and ``net_inet/`` use self-signed key/cert
pairs included in those directories. These don't generally need to be
regenerated, but for reference:

.. code-block:: bash

   # RSA key/cert pair
   openssl req -x509 -newkey rsa:2048 -keyout rsa_key.pem -out rsa_cert.pem \
       -days 3650 -nodes -subj '/CN=micropython.local/O=MicroPython/C=AU'
   openssl pkey -in rsa_key.pem -out rsa_key.der -outform DER
   openssl x509 -in rsa_cert.pem -out rsa_cert.der -outform DER

   # EC key/cert pair
   openssl ecparam -name prime256v1 -genkey -noout -out ec_key.pem
   openssl pkey -in ec_key.pem -out ec_key.der -outform DER
   openssl req -new -x509 -key ec_key.pem -out ec_cert.der -outform DER \
       -days 3650 -nodes -subj '/CN=micropython.local/O=MicroPython/C=AU'

Serial Reliability Testing
--------------------------

``serial_test.py`` measures serial port throughput and reliability with
echo, read and write tests over a range of transfer sizes. It defaults to
``a0`` (``/dev/ttyACM0``):

.. code-block:: bash

   ./serial_test.py -t /dev/ttyACM0
