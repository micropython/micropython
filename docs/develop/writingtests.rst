.. _writingtests:

Test Suite
==========

The MicroPython test suite resides in the ``tests/`` directory and consists of multiple components:

1. **Standard Tests**: The majority of tests that run on a single instance and verify functionality against CPython or expected outputs.
2. **Multi-instance Tests**: Tests that require multiple MicroPython instances to interact with each other (e.g., networking, Bluetooth).
3. **Native Module Tests**: Tests for dynamic native modules compiled to ``.mpy`` files.
4. **Performance Benchmarks**: Python-level benchmarks for measuring execution speed.
5. **Internal Benchmarks**: Low-level benchmarks for core VM operations and C code.

This document primarily focuses on the standard tests and multi-instance tests.

Directory Structure
-------------------

The ``tests/`` directory is organized into subfolders by functionality:

*   ``basics/``: Core Python language features
*   ``extmod/``: Extended modules (like ``ujson``, ``ure``)
*   ``float/``: Floating-point arithmetic specifics
*   ``micropython/``: MicroPython-specific features and behaviors
*   ``import/``: Import mechanisms
*   ``io/``: Input/Output operations
*   ``stress/``: Tests designed to push resource limits (memory, recursion)
*   ``thread/``: Threading module tests
*   ``cmdline/``: Tests for the command-line interface and REPL
*   ``ports/<port_name>/``: Tests specific to a particular port
*   ``feature_check/``: Scripts used to detect target capabilities
*   ``multi_bluetooth/``, ``multi_network/``: Tests involving multiple instances
*   ``perf_bench/``: Performance benchmarking scripts
*   ``internal_bench/``: Low-level internal benchmarks

Key Test Runner Scripts:

*   ``run-tests.py``: Main script for standard tests
*   ``run-multitests.py``: For tests requiring multiple interacting instances
*   ``run-natmodtests.py``: For testing dynamic native modules
*   ``run-perfbench.py``: For performance benchmarks
*   ``run-internalbench.py``: For low-level internal benchmarks

Writing Standard Tests
----------------------

Standard tests are the most common type. They verify functionality by comparing output against expected results.

1.  **Choose a Directory:** Select the appropriate subfolder for your test based on the functionality it tests.
2.  **Create a ``.py`` file:** Add your test code to a new Python file.
3.  **Use ``print()``:** The test harness works by comparing standard output. Use ``print()`` statements to output results.
4.  **Expected Output:**
    *   **CPython Comparison:** By default, tests are run on both CPython and MicroPython with outputs compared.
    *   **``.exp`` Files:** For MicroPython-specific features, create a ``<testname>.py.exp`` file with the expected output.

Example:

Create ``tests/basics/simple_print.py``:

.. code-block:: python

   # tests/basics/simple_print.py
   print("Hello")
   a = 1 + 2
   print(a)

When run, MicroPython's output will be compared to CPython's output (or ``simple_print.py.exp`` if it exists).

Types of Tests
--------------

The test system supports three main testing approaches:

1. **CPython comparison tests**:
   * Used for behavior that should match CPython
   * Tests are run on both CPython and MicroPython
   * Passes if outputs match exactly

2. **Expected output (``.exp``) tests**:
   * Used for MicroPython-specific features or when CPython behavior differs
   * MicroPython output is compared against contents of a ``.py.exp`` file
   * Passes if outputs match exactly

3. **``unittest``-based tests**:
   * Requires ``unittest`` module to be available on the target
   * Used for hardware testing or other MicroPython-specific behavior
   * Passes if unittest summary shows "OK"

Running Tests (``run-tests.py``)
--------------------------------

The primary script is ``run-tests.py``, executed from the ``tests/`` directory.

**Basic Usage:**

.. code-block:: bash

   # Run default tests for the Unix port
   cd tests/
   ./run-tests.py

**Running on Hardware Targets:**

.. code-block:: bash

   # Run on a specific serial port
   ./run-tests.py -t /dev/ttyACM0
   ./run-tests.py -t COM3

   # Use shortcuts for common serial ports
   ./run-tests.py -t a0  # Maps to /dev/ttyACM0
   ./run-tests.py -t u1  # Maps to /dev/ttyUSB1
   ./run-tests.py -t c3  # Maps to COM3

**Filtering Tests:**

.. code-block:: bash

   # Run all tests in specific directories
   ./run-tests.py -d basics
   ./run-tests.py -d extmod -d float

   # Run specific files
   ./run-tests.py basics/int*.py
   ./run-tests.py float/float_parse.py

   # Filter by regex
   ./run-tests.py -e viper      # Exclude tests matching 'viper'
   ./run-tests.py -i asyncio    # Include only tests matching 'asyncio'

**Other Useful Options:**

*   ``--emit {bytecode | native | viper}``: Run tests using a specific code emitter
*   ``--via-mpy``: Compile tests to ``.mpy`` files first before running
*   ``-j N`` or ``--jobs N``: Run N tests in parallel (for PC targets)
*   ``--print-failures``: Show the diff for tests that failed in the last run
*   ``--run-failures``: Re-run only the tests that failed in the last run
*   ``--clean-failures``: Remove the ``.exp`` and ``.out`` files from failures

How The Test System Works
-------------------------

1.  **Target Detection:** Determines the platform and architecture.
2.  **Feature Detection:** Identifies capabilities of the target MicroPython build.
3.  **Test Selection:** Gathers test files based on command-line arguments or defaults.
4.  **Skipping Tests:** Excludes tests based on:
    *   Command-line filters
    *   Missing features
    *   Platform-specific skip lists
    *   Emitter type restrictions
    *   Tests explicitly printing ``SKIP``
5.  **Execution:**
    *   PC targets: Runs MicroPython as a subprocess
    *   Board targets: Uses ``pyboard.py`` to connect via serial/network
6.  **Output Comparison:** Compares target's output with expected output
7.  **Reporting:** Shows results and summarizes pass/fail statistics

Writing Advanced Tests
----------------------

**Skipping Tests Conditionally**

If a test needs to skip itself conditionally:

.. code-block:: python

   import sys
   if not hasattr(sys, 'feature_needed'):
       print('SKIP')
       sys.exit()

**Special Test Handling**

Some tests need special options:

* Add comments in the test file: ``# cmdline: -X heapsize=...``
* Tests needing redirection are listed in ``special_tests`` in ``run-tests.py``

**Platform Considerations**

* **Floating Point:** Be mindful of precision differences. Use ``math.isclose()`` with appropriate tolerances.
* **Endianness:** Check ``sys.byteorder`` for byte order sensitive operations.
* **Memory/Stack:** Avoid large allocations or deep recursion on constrained devices.

Multi-instance Tests
--------------------

For testing interactions like networking or Bluetooth, ``run-multitests.py`` orchestrates multiple instances.

**Writing Multi-instance Tests:**

1.  Define functions named ``instance0()``, ``instance1()``, etc. for each instance's code.
2.  Use the ``multitest`` helper for coordination:
    *   ``multitest.next()``: Complete a stage and wait for other instances
    *   ``multitest.broadcast(msg)``: Send a message to all other instances
    *   ``multitest.wait(msg)``: Wait for a specific broadcast message
    *   ``multitest.skip()``: Indicate the test should be skipped
    *   ``multitest.globals(var=value, ...)``: Set global variables
    *   ``multitest.get_network_ip()``: Get the IP address
    *   ``multitest.expect_reboot(resume_func_name, delay_ms)``: Handle device reboots

**Running Multi-instance Tests:**

.. code-block:: bash

   # Run a network test using two unix instances
   ./run-multitests.py -i micropython -i micropython multi_network/tcp_connect.py

   # Run a bluetooth test using two pyboards
   ./run-multitests.py -i pyb:/dev/ttyACM0 -i pyb:/dev/ttyACM1 multi_bluetooth/ble_connect.py

Provide as many ``-i`` arguments as instances required by the tests.

Test Certificates for SSL/TLS Tests
-----------------------------------

Network tests using SSL/TLS require test certificates to be available on the device. These certificates are included in the repository in ``multi_network/`` and ``net_inet/`` directories.

**Preparing a Device for SSL/TLS Tests:**

Use ``mpremote`` to set the device's RTC and copy the certificate files:

.. code-block:: bash

   # From the micropython/tests/ directory:
   mpremote rtc --set cp multi_net/*.der net_inet/*.der :/

This sets the device's Real Time Clock (using the host's time) and copies all ``.der`` files to the root directory of the device's filesystem.

These certificates are self-signed and generated for testing purposes only.
