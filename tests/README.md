# MicroPython Test Suite

This directory contains tests for MicroPython. For detailed documentation on writing tests and advanced usage, see the [official documentation](https://docs.micropython.org/en/latest/develop/writingtests.html).

## Running Tests

### Standard Tests (`run-tests.py`)

The main test runner verifies MicroPython functionality against expected behavior.

**Run all tests on Unix port (default):**
```bash
cd tests/
./run-tests.py
```
Note: On Windows use `python run-tests.py`

**Run tests on a microcontroller:**
```bash
# Using serial port
./run-tests.py -t /dev/ttyACM0    # Linux
./run-tests.py -t COM3             # Windows

# Using shortcuts
./run-tests.py -t a0    # Maps to /dev/ttyACM0
./run-tests.py -t c3    # Maps to COM3
```

**Run specific tests:**
```bash
# Single file
./run-tests.py basics/int_parse.py

# Multiple files with wildcards
./run-tests.py float/float*.py

# Entire directories
./run-tests.py -d basics -d float
```

### Understanding Test Results

```
pass  basics/array1.py
skip  basics/async_await.py
FAIL  basics/builtin_abs.py
```

**When tests fail:**
1. Check the output files:
   - `<test>.py.out` - What MicroPython produced
   - `<test>.py.exp` - What was expected

2. View failure details:
   ```bash
   ./run-tests.py --print-failures
   ```

3. Re-run failed tests:
   ```bash
   ./run-tests.py --run-failures
   ```

### Performance Benchmarks (`run-perfbench.py`)

Measure MicroPython's performance with standardized benchmarks.

**Run benchmarks:**
```bash
# On Unix port (N=estimated CPU MHz, M=estimated heap KB)
./run-perfbench.py 1000 1000

# On a board
./run-perfbench.py -t /dev/ttyACM0 168 100
```

**Reading results:**
```
bm_float.py: 12345.67 3.2000 98765.43 1.5000
             ^        ^      ^        ^
             time     sd%    score    sd%
             (us)            (higher=better)
```

**Compare performance:**
```bash
# Save baseline
./run-perfbench.py 168 100 | tee baseline.txt

# Run with changes and compare
./run-perfbench.py 168 100 | tee new.txt
./run-perfbench.py -s baseline.txt new.txt
```

### Multi-Instance Tests (`run-multitests.py`)

For tests requiring multiple MicroPython instances (networking, Bluetooth).

```bash
# Two Unix instances
./run-multitests.py -t unix -t unix multi_net/tcp_data.py

# Two boards
./run-multitests.py -t /dev/ttyACM0 -t /dev/ttyACM1 multi_bluetooth/ble_gap_connect.py
```

### SSL/TLS Test Setup

Network tests with SSL require certificates on the device:

```bash
# From tests/ directory
mpremote rtc --set cp multi_net/*.der net_inet/*.der :/
```

## Directory Structure

- `basics/` - Core Python functionality
- `extmod/` - Extended modules
- `extmod_hardware/` - Hardware-dependent extended module tests
- `float/` - Floating-point tests
- `target_wiring/` - Board-specific hardware configuration for extmod_hardware tests
- `perf_bench/` - Performance benchmarks
- `multi_*/` - Multi-instance tests (network, Bluetooth)

## Reporting Issues

When reporting test failures, please include:
1. The test name and failure output
2. Your MicroPython version: `micropython -c "import sys; print(sys.version)"`
3. Your board/port information
4. The complete output from `./run-tests.py --print-failures`

For developer documentation on writing tests and advanced usage, see the [full documentation](https://docs.micropython.org/en/latest/develop/writingtests.html).
