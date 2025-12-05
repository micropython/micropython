# CPython Enum Test Artifacts

This directory contains test results and reports from running CPython's official enum test suite against our MicroPython enum implementation.

## Files

### Reports

- **`COMPATIBILITY_SUMMARY.md`** - Quick overview with key statistics and compatibility grade
- **`CPYTHON_TESTS_REPORT.md`** - Comprehensive analysis with detailed test results, feature matrix, and recommendations
- **`test_execution.log`** - Full verbose test execution output (570 lines, 47KB)

### Test Files (in `/tmp/`)

- **`test_enum_cpython.py`** - CPython 3.13 enum test suite (5,307 lines)
- **`run_enum_tests.py`** - Test runner with module patching and selective loading

## Quick Start

### View Summary
```bash
cat COMPATIBILITY_SUMMARY.md
```

### View Full Report
```bash
less CPYTHON_TESTS_REPORT.md
```

### Re-run Tests
```bash
cd /tmp
python3 run_enum_tests.py -v 1
```

### View Detailed Execution
```bash
less test_execution.log
```

## Test Results

**Compatibility: 99.3%** (445/448 tests passed)

- ✅ **Enum:** 100% (34/34 tests)
- ✅ **IntEnum:** 100% (37/37 tests)
- ✅ **StrEnum:** 100% (37/37 tests)
- ⚠️ **Flag:** 97% (37/38 tests) - Missing combo `__contains__`
- ⚠️ **IntFlag:** 97% (38/39 tests) - Missing combo `__contains__`

## Failed Tests

All 3 failures are the same issue: Flag combination membership testing

```python
class MyFlag(Flag):
    A = 1
    B = 2
    C = 4

# Fails in our implementation:
assert 7 in MyFlag  # Expected True, got False
```

This is a CPython 3.13+ feature. Our implementation correctly handles all other Flag operations.

## Test Coverage

### Included (448 tests from 16 classes)
- TestHelpers (5)
- TestPlainEnumClass (34)
- TestPlainFlagClass (38)
- TestIntEnumClass (37)
- TestStrEnumClass (37)
- TestIntFlagClass (39)
- TestMixedIntClass (34)
- TestMixedStrClass (34)
- TestMixedIntFlagClass (37)
- TestMixedDateClass (34)
- TestMinimalDateClass (36)
- TestMixedFloatClass (34)
- TestMinimalFloatClass (36)
- TestOrder (7)
- TestEmptyAndNonLatinStrings (3)
- TestUnique (3)

### Excluded (14 classes)
- All functional API test classes (TestPlainEnumFunction, etc.)
- TestSpecial (uses `_missing_()`, `_ignore_`)
- TestVerify (uses `@verify()` decorator)

## Methodology

1. **Module Patching:**
   - Replaced `sys.modules['enum']` with our implementation
   - Mocked `test.support` modules
   - Added compatibility stubs for advanced features

2. **Selective Loading:**
   - Only loaded class-based enum tests
   - Skipped functional API tests (not implemented)
   - Included all helper and utility tests

3. **Execution:**
   - Standard unittest.TextTestRunner
   - Verbose output with categorized failures
   - Full traceback collection

## Date

Test execution: 2025-11-13
Python version: 3.12.3
Test source: CPython 3.13 Lib/test/test_enum.py
