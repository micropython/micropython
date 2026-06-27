.. _test_quickstart:

Running Tests
=============

MicroPython includes a test suite that verifies the interpreter and standard
library work correctly. Running these tests is useful after building
MicroPython from source to check nothing is broken, or when developing
changes to MicroPython itself.

.. note::
   All test commands should be run from the ``tests/`` directory.

Standard Tests
--------------

The test runner ``run-tests.py`` executes tests and compares output against
expected results. By default tests run against CPython to generate expected
output; tests with a ``.exp`` file use that instead.

.. code-block:: bash

   cd tests/

   # Run all tests on unix port (default)
   ./run-tests.py

   # Run on a microcontroller
   ./run-tests.py -t /dev/ttyACM0    # Linux
   ./run-tests.py -t COM3             # Windows
   ./run-tests.py -t a0               # Shortcut for /dev/ttyACM0

   # Run specific tests or directories
   ./run-tests.py basics/int_parse.py
   ./run-tests.py float/float*.py
   ./run-tests.py -d basics -d float

On Windows use ``python run-tests.py``.

Understanding Results
~~~~~~~~~~~~~~~~~~~~~

A test run produces output like::

   pass  basics/array1.py
   skip  basics/async_await.py
   lrge  basics/some_large_test.py
   FAIL  basics/builtin_abs.py

- ``pass`` - Test output matched expected output
- ``skip`` - Test skipped (feature not available on this target)
- ``lrge`` - Test skipped, too large for the target (printed ``SKIP-TOO-LARGE``)
- ``FAIL`` - Test output differs from expected

When tests fail, output files are written to the ``results/`` directory:

- ``<test>.py.out`` - what MicroPython produced
- ``<test>.py.exp`` - what was expected

.. code-block:: bash

   # View all failure diffs
   ./run-tests.py --print-failures

   # Re-run only previously failed tests
   ./run-tests.py --run-failures

   # Clean up .out/.exp artifacts
   ./run-tests.py --clean-failures

   # Re-run a single test and view its diff
   ./run-tests.py basics/builtin_abs.py --print-failures

Test Skipping
~~~~~~~~~~~~~

Tests detect unsupported features at runtime and skip themselves by printing
``SKIP`` then exiting:

.. code-block:: python

   try:
       import required_module
   except ImportError:
       print("SKIP")
       raise SystemExit

Some tests use MicroPython-specific syntax that causes ``SyntaxError`` before
skip logic can run. These are skipped by filename pattern matching in the test
runner. See :ref:`test_developers` for the full list of filename patterns.

Performance Benchmarks
----------------------

``run-perfbench.py`` runs standardised benchmarks. The two positional arguments
are estimated CPU MHz and estimated heap KB for the target.

.. code-block:: bash

   # Unix port
   ./run-perfbench.py 1000 1000

   # On a board
   ./run-perfbench.py -t /dev/ttyACM0 168 100

Each result line shows::

   bm_float.py: 12345.67 3.2000 98765.43 1.5000
                ^        ^      ^        ^
                time     sd%    score    sd%
                (us)            (higher=better)

The sd% columns are standard deviation as a percentage. High sd% values
indicate variability between iterations; results with high sd% should be
treated with caution. To compare runs:

.. code-block:: bash

   ./run-perfbench.py 168 100 | tee baseline.txt
   # ... make changes ...
   ./run-perfbench.py 168 100 | tee new.txt
   ./run-perfbench.py -s baseline.txt new.txt  # Compare scores
   ./run-perfbench.py -m baseline.txt new.txt  # Compare times

The first positional argument (approximate CPU MHz) affects test duration
and score normalisation. The second (approximate heap KB) affects
memory-intensive test behaviour. Specific benchmarks can be selected:

.. code-block:: bash

   ./run-perfbench.py -t /dev/ttyACM0 168 100 perf_bench/bm_float.py

Multi-Instance Tests
--------------------

Tests requiring multiple MicroPython instances (networking, Bluetooth) use
``run-multitests.py``. The number of ``-t`` arguments must match the number
of instances the test defines.

.. code-block:: bash

   # Two unix instances for network testing
   ./run-multitests.py -t unix -t unix multi_net/tcp_data.py

   # Two boards for Bluetooth
   ./run-multitests.py -t /dev/ttyACM0 -t /dev/ttyACM1 multi_bluetooth/ble_gap_connect.py

SSL/TLS Setup
-------------

Network tests using SSL require certificates and a valid RTC on the device:

.. code-block:: bash

   mpremote rtc --set cp multi_net/*.der net_inet/*.der :/

Reporting Failures
------------------

When reporting test failures, include:

1. Test name and failure output (``./run-tests.py --print-failures``)
2. MicroPython version: ``micropython -c "import sys; print(sys.version)"``
3. Board/port information

For writing tests and advanced usage, see :ref:`test_developers`.
