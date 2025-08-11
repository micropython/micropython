.. _test_developers:

Test Suite Developer Guide
==========================

This guide provides comprehensive information for developers writing and maintaining MicroPython tests. For a quick start on running tests, see :ref:`test_quickstart`.

Test Suite Organization
-----------------------

The ``tests/`` directory structure:

- ``basics/``: Core Python language features
- ``extmod/``: Extended modules (ujson, ure, etc.)
- ``float/``: Floating-point arithmetic tests
- ``micropython/``: MicroPython-specific features
- ``import/``: Import mechanism tests
- ``io/``: Input/Output operations
- ``stress/``: Resource limit tests (memory, recursion)
- ``thread/``: Threading module tests
- ``cmdline/``: Command-line interface and REPL tests
- ``ports/<port_name>/``: Port-specific tests
- ``feature_check/``: Target capability detection scripts
- ``multi_bluetooth/``, ``multi_network/``: Multi-instance tests
- ``perf_bench/``: Performance benchmarks
- ``internal_bench/``: Low-level internal benchmarks

Writing Standard Tests
----------------------

Test Types
~~~~~~~~~~

MicroPython supports three testing approaches:

1. **CPython Comparison Tests** (preferred for standard Python features):
   
   - Tests run on both CPython and MicroPython
   - Outputs must match exactly
   - Used for standard Python behavior

2. **Expected Output Tests** (``.exp`` files):
   
   - For MicroPython-specific features
   - Compare output against ``<testname>.py.exp``
   - Use when CPython behavior differs or feature doesn't exist

3. **unittest-based Tests** (preferred for MicroPython-specific features):
   
   - Requires ``unittest`` module on target
   - Better error messages and structure
   - Use for hardware testing and MicroPython-specific behavior

Writing Test Files
~~~~~~~~~~~~~~~~~~

**Basic test structure:**

.. code-block:: python

   # tests/basics/my_feature.py
   # Test description comment
   
   # Use print() for output - this is what gets compared
   print("Testing feature X")
   result = some_function()
   print(result)

**Conditional skipping:**

.. code-block:: python

   import sys
   
   # Skip if feature not available
   if not hasattr(sys, 'required_feature'):
       print('SKIP')
       sys.exit()

**Platform-specific considerations:**

.. code-block:: python

   import sys
   
   # Handle endianness differences
   if sys.byteorder == 'little':
       expected = b'\x01\x02'
   else:
       expected = b'\x02\x01'

Advanced run-tests.py Usage
---------------------------

Test Filtering and Selection
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

   # Filter by regular expression
   ./run-tests.py -e viper          # Exclude tests matching 'viper'
   ./run-tests.py -i float          # Include only tests matching 'float'
   
   # Run with specific code emitter
   ./run-tests.py --emit native     # Test native code emitter
   ./run-tests.py --emit viper      # Test viper code emitter
   
   # Compile to .mpy first
   ./run-tests.py --via-mpy
   
   # Parallel execution (PC targets only)
   ./run-tests.py -j 4              # Run 4 tests in parallel

How Test Discovery Works
~~~~~~~~~~~~~~~~~~~~~~~~

``run-tests.py`` uses a sophisticated test discovery and filtering system:

1. **Feature Detection**: Runs scripts in ``feature_check/`` to determine:
   
   - Architecture and platform
   - Available modules and features
   - Code emitter support (native, viper)
   - Float precision
   - Endianness

2. **Automatic Skipping**: Tests are skipped based on:
   
   **Filename patterns:**
   
   - ``native_*``, ``viper_*`` - Skip without native code emitter support
   - ``*_endian`` - Skip when host/target have different byte order
   - ``int_big*``, ``*_intbig`` - Skip without arbitrary-precision integers
   - ``bytearray*``, ``*_bytearray`` - Skip without bytearray support
   - ``set_*``, ``frozenset*``, ``*_set`` - Skip without set type support
   - ``*slice*`` - Skip without slice support (includes specific test list)
   - ``async_*``, ``asyncio_*`` - Skip without async/await support
   - ``const*`` - Skip without const keyword (MicroPython extension)
   - ``*reverse_op*`` - Skip without __rOP__ special methods
   - ``io_*`` - Skip when io module doesn't exist
   - ``string_fstring*`` - Skip without f-string support
   - ``asm*`` - Skip without inline assembly for target architecture
   
   **Other skip conditions:**
   
   - Platform skip lists in ``run-tests.py``
   - Missing required features
   - Explicit ``SKIP`` output from test
   - Command-line filters

3. **Special Test Handling**: Some tests need special treatment:
   
   - Command-line options: ``# cmdline: -X heapsize=16k``
   - Tests listed in ``special_tests`` dictionary
   - Tests requiring specific setup or teardown

Writing Multi-Instance Tests
----------------------------

Multi-instance tests coordinate multiple MicroPython instances for testing communication protocols.

Test Structure
~~~~~~~~~~~~~~

.. code-block:: python

   # tests/multi_network/tcp_echo.py
   
   def instance0():
       # Server instance
       multitest.globals(IP=multitest.get_network_ip())
       multitest.next()
       
       import socket
       s = socket.socket()
       s.bind(('0.0.0.0', 8000))
       s.listen(1)
       multitest.broadcast('server ready')
       
       conn, addr = s.accept()
       data = conn.recv(1024)
       conn.send(data)  # Echo back
       conn.close()
       s.close()
   
   def instance1():
       # Client instance
       multitest.next()
       multitest.wait('server ready')
       
       import socket
       s = socket.socket()
       s.connect((IP, 8000))
       s.send(b'Hello')
       print(s.recv(1024))
       s.close()

Coordination Methods
~~~~~~~~~~~~~~~~~~~~

The ``multitest`` helper provides:

- ``next()``: Synchronize instances at stages
- ``broadcast(msg)``: Send message to all instances
- ``wait(msg)``: Wait for specific broadcast
- ``globals(**kwargs)``: Share variables between stages
- ``get_network_ip()``: Get instance's IP address
- ``expect_reboot(resume_func, delay_ms)``: Handle device reboots
- ``skip()``: Skip test from any instance

Running Multi-Instance Tests
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

   # Different instance combinations
   ./run-multitests.py -i micropython -i cpython test.py
   ./run-multitests.py -i pyb:a0 -i pyb:a1 test.py
   
   # Test permutations (swap instance assignments)
   ./run-multitests.py -p 2 -i inst1 -i inst2 test.py

Native Module Tests
-------------------

Testing dynamic native modules (``.mpy`` files with machine code).

How It Works
~~~~~~~~~~~~

1. Pre-compiled ``.mpy`` files in ``examples/natmod/``
2. Script injects module into target's RAM via VFS
3. Test runs against the loaded module
4. Architecture auto-detected or specified with ``-a``

Running Native Module Tests
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

   # Auto-detect architecture
   ./run-natmodtests.py extmod/btree_bdb.py
   
   # Specify architecture
   ./run-natmodtests.py -a armv7em extmod/re_basic.py
   
   # Run on pyboard
   ./run-natmodtests.py -p -d /dev/ttyACM0 extmod/btree_bdb.py

Internal Benchmarks
-------------------

Low-level benchmarks for VM operations and C code performance.

Writing Internal Benchmarks
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Tests output structured metrics:

.. code-block:: python

   # tests/internal_bench/loop_simple.py
   import time
   
   start = time.ticks_us()
   for i in range(1000):
       pass
   elapsed = time.ticks_diff(time.ticks_us(), start)
   
   print(f"core : loop : simple_loop_1000 : {elapsed} : us")

Results are validated against ``internalbench_results.py``.

Running and Updating Benchmarks
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

   # Run against existing baselines
   ./run-internalbench.py --target unix internal_bench/*.py
   
   # Generate new baselines
   ./run-internalbench.py --reference myboard internal_bench/*.py

Performance Benchmarks
----------------------

Advanced Usage
~~~~~~~~~~~~~~

.. code-block:: bash

   # Run specific benchmarks with custom parameters
   ./run-perfbench.py -p 168 100 -a 10 perf_bench/bm_float.py
   
   # Compare different aspects
   ./run-perfbench.py -t baseline.txt new.txt  # Compare times
   ./run-perfbench.py -s baseline.txt new.txt  # Compare scores

Understanding Results
~~~~~~~~~~~~~~~~~~~~~

- **Error percentages**: High values indicate variability
- **N parameter**: Affects test duration and score normalization
- **M parameter**: Affects memory-intensive test behavior

Best Practices
--------------

1. **Test Isolation**: Each test should be independent
2. **Deterministic Output**: Avoid timing-dependent output
3. **Resource Awareness**: Consider memory constraints on embedded targets
4. **Clear Failure Messages**: Make failures easy to diagnose
5. **Documentation**: Comment complex test logic
6. **Cross-platform**: Test on multiple architectures when possible

Debugging Test Failures
-----------------------

1. **Examine outputs**:
   
   .. code-block:: bash
   
      # View specific failure
      cat results/test_name.py.out
      cat results/test_name.py.exp
      
      # Diff outputs
      diff results/test_name.py.exp results/test_name.py.out

2. **Run individual test**:
   
   .. code-block:: bash
   
      # With verbose output
      ./run-tests.py -v test_name.py

3. **Check feature support**:
   
   .. code-block:: bash
   
      # Run feature check directly
      micropython feature_check/float.py

Contributing Tests
------------------

When submitting new tests:

1. Place in appropriate directory
2. Include clear comments explaining what's tested
3. Test on multiple platforms if possible
4. Ensure deterministic output
5. Follow existing naming conventions
6. Update skip lists if platform-specific