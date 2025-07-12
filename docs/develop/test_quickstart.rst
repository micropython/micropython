.. _test_quickstart:

Test Suite Quick Start
======================

This guide covers the essential information for running MicroPython tests. For advanced usage and writing tests, see :ref:`test_developers`.

.. note::
   All test commands should be run from the ``tests/`` directory unless otherwise noted.

Running Standard Tests
----------------------

The main test runner ``run-tests.py`` verifies MicroPython functionality against expected behavior.

Basic Usage
~~~~~~~~~~~

**Run all tests on Unix port (default):**

.. code-block:: bash

   cd tests/
   ./run-tests.py

.. note::
   On Windows use ``python run-tests.py``

**Run tests on a microcontroller:**

.. code-block:: bash

   # Using serial port
   ./run-tests.py -t /dev/ttyACM0    # Linux
   ./run-tests.py -t COM3             # Windows

   # Using shortcuts
   ./run-tests.py -t a0    # Maps to /dev/ttyACM0
   ./run-tests.py -t c3    # Maps to COM3

**Run specific tests:**

.. code-block:: bash

   # Single file
   ./run-tests.py basics/int_parse.py

   # Multiple files with wildcards
   ./run-tests.py float/float*.py

   # Entire directories
   ./run-tests.py -d basics -d float

Understanding Test Results
~~~~~~~~~~~~~~~~~~~~~~~~~~

A typical test run produces output like::

   pass  basics/array1.py              ✓ Test passed
   skip  basics/async_await.py         - Test skipped (feature not available)
   FAIL  basics/builtin_abs.py         ✗ Test failed

**When tests fail:**

1. Check the output files:

   - ``<test>.py.out`` - What MicroPython produced
   - ``<test>.py.exp`` - What was expected

2. View failure details:

   .. code-block:: bash

      ./run-tests.py --print-failures

3. Re-run failed tests:

   .. code-block:: bash

      ./run-tests.py --run-failures

4. Clean up failure artifacts:

   .. code-block:: bash

      ./run-tests.py --clean-failures

Performance Benchmarks
----------------------

Measure MicroPython's performance with standardized benchmarks using ``run-perfbench.py``.

Running Benchmarks
~~~~~~~~~~~~~~~~~~

.. code-block:: bash

   # On Unix port (N=estimated CPU MHz, M=estimated heap KB)
   ./run-perfbench.py 1000 1000

   # On a board
   ./run-perfbench.py -p -d /dev/ttyACM0 168 100

Reading Results
~~~~~~~~~~~~~~~

Each benchmark line shows::

   bm_float.py: 12345.67 3.2% 98765.43 1.5%
                ↑              ↑
                runtime (μs)   score
                (lower=better) (higher=better)

- **Runtime**: Average execution time in microseconds (lower is better)
- **Score**: Test-specific metric, often operations/second (higher is better)
- **Percentages**: Standard deviation showing result variability

Comparing Performance
~~~~~~~~~~~~~~~~~~~~~

To track performance changes:

.. code-block:: bash

   # Save baseline
   ./run-perfbench.py 168 100 | tee baseline.txt

   # Run with changes and compare scores
   ./run-perfbench.py 168 100 | tee new.txt
   ./run-perfbench.py -s baseline.txt new.txt

The comparison shows percentage changes and highlights significant differences.

Multi-Instance Tests
--------------------

For tests requiring multiple MicroPython instances (networking, Bluetooth), use ``run-multitests.py``.

.. code-block:: bash

   # Two Unix instances for network testing
   ./run-multitests.py -i micropython -i micropython multi_network/tcp_connect.py

   # Two boards for Bluetooth testing
   ./run-multitests.py -i pyb:/dev/ttyACM0 -i pyb:/dev/ttyACM1 multi_bluetooth/ble_connect.py

The number of ``-i`` arguments must match the instances required by the test.

SSL/TLS Test Setup
------------------

Network tests using SSL require certificates on the device:

.. code-block:: bash

   # From tests/ directory - sets RTC and copies certificates
   mpremote rtc --set cp multi_net/*.der net_inet/*.der :/

This copies test certificates to the device's root filesystem and ensures the RTC is set correctly for certificate validation.

Reporting Test Failures
-----------------------

When reporting issues, please include:

1. **Test name and failure output** - The specific test that failed
2. **MicroPython version**:

   .. code-block:: bash

      micropython -c "import sys; print(sys.version)"

3. **Board/port information** - Which hardware or port you're testing
4. **Complete failure details**:

   .. code-block:: bash

      ./run-tests.py --print-failures

Next Steps
----------

- For writing tests and advanced usage, see :ref:`test_developers`
- For contributing to MicroPython, see the `Contributing Guidelines <https://github.com/micropython/micropython/blob/master/CONTRIBUTING.md>`_