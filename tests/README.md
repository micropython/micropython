# MicroPython Test Suite

This directory contains tests for most parts of MicroPython. The test suite is essential for verifying the correctness and stability of the MicroPython core and its various ports.

For comprehensive documentation on writing tests and a higher-level overview, please refer to the [official documentation](https://docs.micropython.org/en/latest/develop/writingtests.html#writingtests). This README provides specific details about the test runners and conventions within this directory.

## Main Test Runner: `run-tests.py`

The primary script for running the bulk of the functional tests is `run-tests.py`.

**Basic Usage:**

By default, it runs tests against the unix port of MicroPython:
```bash
cd tests/
./run-tests.py
```

**Running on Bare-Metal Targets:**

To test a board running MicroPython firmware, use the `-t` (or `--test-instance`) option to specify the serial port. The script will attempt to automatically detect the target platform and run an appropriate set of tests.
```bash
./run-tests.py -t /dev/ttyACM0
```
Shortcuts like `a<n>` (for `/dev/ttyACM<n>`) and `c<n>` (for `COM<n>`) are available. Use `./run-tests.py --help` for all options, including specifying baud rates, user/password for Telnet, and other connection parameters.

**Types of Tests Handled by `run-tests.py`:**

1.  **`unittest`-based tests:**
    *   Require `unittest` to be installed on the target (e.g., via `mpremote mip install unittest`).
    *   Used for MicroPython-specific behavior, modules not in CPython, and hardware tests.
    *   Pass if `unittest` prints "OK". Other output is ignored for pass/fail determination.
2.  **`.exp` file comparison:**
    *   For features not runnable or comparable with CPython.
    *   Output from MicroPython is compared directly against the content of a corresponding `<test_name>.py.exp` file.
    *   Passes if outputs match exactly.
3.  **CPython comparison:**
    *   For behavior that should precisely match CPython.
    *   The test is run on CPython, its output captured. Then, it's run on MicroPython.
    *   Passes if MicroPython's output matches CPython's. Failed tests save outputs to `.exp` (CPython) and `.out` (MicroPython) in the results directory.

**Skipping Tests:**

*   Tests can conditionally skip themselves by printing `SKIP` followed by a newline and then calling `sys.exit()` (which raises `SystemExit`).
*   `run-tests.py` also uses small scripts in the `feature_check/` directory to detect target capabilities (e.g., float precision, native features, `async` support) and skips irrelevant tests automatically based on these checks or predefined platform skip lists.

**Writing Tests for `run-tests.py`:**

*   Place tests in appropriate subdirectories (e.g., `basics/`, `extmod/`, `float/`, `ports/<port_name>/`).
*   Use `print()` statements for output.
*   For MicroPython-specific tests or when CPython behavior differs, provide a `.py.exp` file.
*   Tests relying on float support should generally go in `float/`.
*   Tests relying on non-builtin imports should generally go in `import/`.

For more detailed information on writing and running these general tests, refer to `docs/develop/writingtests.rst`.

## Specialized Test Runners and Frameworks

This section covers other test runners and frameworks used for specific testing needs.

### Multi-instance Tests: `run-multitests.py`

For testing interactions between multiple MicroPython instances, such as networking or Bluetooth communication, use `run-multitests.py`.

*   **Test Structure:** Test scripts must define functions named `instance0()`, `instance1()`, etc. Each function contains the code to be executed by that specific instance.
*   **Coordination:** The script injects a `multitest` class into each running script, providing methods for coordination:
    *   `multitest.next()`: Signal completion of a stage and wait for other instances.
    *   `multitest.broadcast(msg)`: Send a message to all other instances.
    *   `multitest.wait(msg)`: Wait until a specific broadcast message is received.
    *   `multitest.skip()`: Indicate the test should be skipped.
    *   `multitest.globals(var=value, ...)`: Set global variables available to subsequent stages/instances.
    *   `multitest.get_network_ip()`: Get the IP address of the instance.
    *   `multitest.expect_reboot(resume_func_name, delay_ms)`: Handle device reboots, specifying a function name (e.g., `'instance0_part2'`) to resume execution from after the reboot.
    *   `multitest.output_metric(data)`: Output metrics for collection.
*   **Running:**
    ```bash
    cd tests/

    # Run a network test using two unix instances
    ./run-multitests.py -i micropython -i micropython multi_network/tcp_connect.py

    # Run a bluetooth test using two pyboards
    ./run-multitests.py -i pyb:/dev/ttyACM0 -i pyb:/dev/ttyACM1 multi_bluetooth/ble_connect.py

    # Run with permutations (swaps instance assignments)
    ./run-multitests.py -p 2 -i pyb:a0 -i micropython multi_network/some_test.py
    ```
*   **Instance Types (`-i`):**
    *   `micropython`: Standard MicroPython Unix/Windows port.
    *   `cpython`: Standard CPython 3.
    *   `pyb:<device>`: Pyboard instance on the specified port (e.g., `pyb:/dev/ttyUSB0`, `pyb:a0`).
    *   `exec:<path>`: Custom executable.
    You need to provide as many `-i` arguments as the maximum number of instances required by the tests. Use `./run-multitests.py --help` for all options.

### Native Module Tests: `run-natmodtests.py`

This script tests dynamic native modules (``.mpy`` files compiled with native machine code). These modules are typically found in `examples/natmod/`.

*   **Operation:** It injects a pre-compiled native ``.mpy`` file (e.g., `btree_x64.mpy` from ``examples/natmod/btree/``) into the target's RAM filesystem (via a VFS hook) and runs a corresponding Python test script (e.g., `extmod/btree_bdb.py`) against it.
*   **Architecture:** Auto-detects target architecture (x86, armv7m, xtensa etc.) using ``feature_check/target_info.py`` to load the correct ``.mpy`` file (e.g., `module_$(ARCH).mpy`). This can be overridden with ``-a <arch>`` or ``--arch <arch>``.
*   **Targets:** Runs on a local MicroPython subprocess (e.g., Unix port) or via `pyboard.py` (``-p`` or ``--pyboard`` flag).
*   **Expected Output:** Compares results against ``<test_file>.exp`` or CPython output (for the Python part of the test, if applicable).

**Running Native Module Tests:**
```bash
cd tests/

# Run btree native module tests on the unix port (auto-detects arch)
./run-natmodtests.py extmod/btree_bdb.py

# Run re native module tests on a pyboard, specifying armv7em architecture
./run-natmodtests.py -p -d /dev/ttyACM0 -a armv7em extmod/re_basic.py
```
Use `./run-natmodtests.py --help` for more options.

### Performance Benchmarks: `run-perfbench.py`

This script runs Python-level performance benchmarks located in ``tests/perf_bench/``. Its goal is to measure the execution speed of various Python operations and scripts.

*   **Operation:** Executes benchmark scripts (from `perf_bench/`) on both MicroPython and, for comparison, CPython. Each script is run multiple times to obtain stable timings.
*   **Parameters:**
    *   Requires two main arguments: `N` (approximate target CPU frequency in MHz) and `M` (approximate target heap size in kBytes). These parameters help normalize scores and can influence test behavior or selection.
    *   `-a` or `--average`: Number of times to run each test for averaging.
*   **Metrics:** For each test, it reports:
    *   Runtime average (microseconds, lower is better).
    *   Runtime standard deviation as a percentage.
    *   Score average (units depend on the benchmark, higher is better). A common score is operations/second.
    *   Score standard deviation as a percentage.
*   **Targets:** Can run on local MicroPython subprocesses (Unix, Windows) or via `pyboard.py` (``-p`` or ``--pyboard``).
*   **Output Comparison:**
    *   Prints a table of timings and scores.
    *   Can diff results from two previous runs saved to files using ``-t file1 file2`` (for times) or ``-s file1 file2`` (for scores) to help track performance changes.

**Running Performance Benchmarks:**
```bash
cd tests/

# Run performance benchmarks on unix port (e.g., N=1000MHz, M=1000KB heap, 8 repetitions)
./run-perfbench.py 1000 1000 -a 8 perf_bench/bm_*.py

# Run on a pyboard (e.g., N=168MHz, M=100KB heap)
./run-perfbench.py -p -d /dev/ttyACM0 168 100 perf_bench/bm_float.py
```
When comparing runs, pay attention to the error percentages in the diff output; high variability (common on PCs due to OS interference) can make comparisons less reliable. Running tests longer (higher `N`) can sometimes help. Use `./run-perfbench.py --help` for all options.

### Internal Benchmarks: `run-internalbench.py`

This script runs low-level benchmarks, often testing C code performance or core MicroPython VM operations. Tests are located in ``tests/internal_bench/``.

*   **Operation:** Executes Python scripts that typically perform very specific, micro-level actions. These scripts are expected to print their results in a structured format: `CATEGORY : SUBCATEGORY : METRIC_NAME : VALUE : UNIT`. For example: `core : loop : simple_loop_1000 : 12345 : cycles`.
*   **Expected Output & Comparison:** The script compares the reported `VALUE` against expected values (or acceptable ranges) defined in a separate Python file, ``tests/internalbench_results.py``. This results file contains a dictionary mapping a tuple `(TARGET, CATEGORY, SUBCATEGORY, METRIC_NAME, UNIT)` to an expected value or a `(min_expected, max_expected)` tuple.
*   **Targets:** Runs on local MicroPython subprocesses or via `pyboard.py` (``--pyboard``). The ``--target <name>`` argument specifies which set of results to use from `internalbench_results.py`.
*   **Results:** Metrics are usually low-level, such as clock cycles or execution time in microseconds for fundamental operations.
*   **Reference Mode:** The ``--reference <target_name>`` option allows using the current run's results to generate or update the baseline values for `<target_name>` in `internalbench_results.py`.

**Running Internal Benchmarks:**
```bash
cd tests/

# Run internal benchmarks for the default unix target, comparing against results for 'unix' in internalbench_results.py
./run-internalbench.py --target unix internal_bench/*_*.py

# Run on pyboard and check against 'pyboard' target results
./run-internalbench.py --pyboard --target pyboard internal_bench/*_*.py

# Generate/update reference results in internalbench_results.py for a new or existing target 'myboard'
./run-internalbench.py --pyboard --reference myboard internal_bench/*_*.py
```
Test files are often named like `group-subtest.py`, and the runner may group results by the `group` part. Use `./run-internalbench.py --help` for more options.

## Test Keys/Certificates for SSL/TLS

SSL/TLS tests in `multi_net/` and `net_inet/` use self-signed key/cert pairs. These are randomly generated for testing/demonstration only.

To run these tests on-device, the `.der` files (key and certificate in DER format) should be copied to the device's filesystem, and the device's Real Time Clock (RTC) must be set to a time when the certificates are valid.

**Preparation using `mpremote`:**
```bash
# Ensure mpremote is installed: pip install mpremote
# From the micropython/tests/ directory:
mpremote rtc --set cp multi_net/*.der net_inet/*.der :/
```
This command sets the RTC from the host PC's time and copies all `.der` files from `multi_net/` and `net_inet/` to the root directory of the device's filesystem.

### Generating New Test Key/Certificates (Reference Only)

The keys/certs are included in the repository. This section is for reference if they need to be regenerated.

**RSA Key/Cert Pair:**
1.  Create self-signed key and cert (PEM format):
    ```bash
    openssl req -x509 -newkey rsa:2048 -keyout rsa_key.pem -out rsa_cert.pem -days 3650 -nodes -subj '/CN=micropython.local/O=MicroPython/C=AU'
    ```
2.  Convert to DER format:
    ```bash
    openssl pkey -in rsa_key.pem -out rsa_key.der -outform DER
    openssl x509 -in rsa_cert.pem -out rsa_cert.der -outform DER
    ```

**Elliptic Curve (EC) Key/Cert Pair (e.g., prime256v1):**
1.  Generate EC key (PEM format):
    ```bash
    openssl ecparam -name prime256v1 -genkey -noout -out ec_key.pem
    ```
2.  Convert key to DER format:
    ```bash
    openssl pkey -in ec_key.pem -out ec_key.der -outform DER
    ```
3.  Create self-signed certificate from key (DER format):
    ```bash
    openssl req -new -x509 -key ec_key.pem -out ec_cert.der -outform DER -days 3650 -nodes -subj '/CN=micropython.local/O=MicroPython/C=AU'
    ```
