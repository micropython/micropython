# Testing Tools

## run_enum_tests.py

Test runner for CPython's official enum test suite against our MicroPython enum implementation.

### Usage

```bash
# Download CPython enum tests (if not already present)
wget https://raw.githubusercontent.com/python/cpython/main/Lib/test/test_enum.py \
     -O /tmp/test_enum_cpython.py

# Run tests
cd /tmp
python3 /home/corona/mpy/py-metaclass/tools/run_enum_tests.py

# Verbose output
python3 /home/corona/mpy/py-metaclass/tools/run_enum_tests.py -v 2

# Show detailed failures
python3 /home/corona/mpy/py-metaclass/tools/run_enum_tests.py --show-failures
```

### Features

- Patches `sys.modules` to use our enum implementation
- Mocks test support modules
- Selectively loads compatible test classes
- Excludes functional API tests (not implemented)
- Provides categorized failure analysis
- 99.3% compatibility (445/448 tests pass)

### Options

- `-v {0,1,2}` - Verbosity level (default: 1)
  - 0: Quiet
  - 1: Normal progress
  - 2: Verbose with test names
- `--show-failures` - Display detailed failure tracebacks

### Output

The script generates:
- Test execution summary
- Compatibility statistics
- Categorized failure analysis
- Optional detailed tracebacks

### Test Selection

**Included Classes (16):**
- TestHelpers
- TestPlainEnumClass
- TestPlainFlagClass
- TestIntEnumClass
- TestStrEnumClass
- TestIntFlagClass
- TestMixed* classes (int, str, float, date)
- TestOrder
- TestEmptyAndNonLatinStrings
- TestUnique

**Excluded Classes (14):**
- All *Function classes (functional API)
- TestSpecial (advanced hooks)
- TestVerify (verification decorator)

### Results

See project root for:
- `COMPATIBILITY_SUMMARY.md` - Quick stats
- `CPYTHON_TESTS_REPORT.md` - Detailed analysis
- `test_execution.log` - Full execution log
