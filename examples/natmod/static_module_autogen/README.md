# Dual-Mode Module Examples

This directory contains examples and infrastructure for creating MicroPython modules that can be compiled as both user C modules (built into firmware) and native modules (dynamic .mpy files).

## Quick Start

### Build as Native Module (.mpy)
```bash
make clean
make dualmode_simple.mpy
# Test with: micropython -m dualmode_simple
```

### Build as User C Module (firmware)
```bash
cd ../../../ports/unix
make USER_C_MODULES=$(pwd)/../../examples/natmod/static_module_autogen
./build-standard/micropython -m dualmode_simple
```

## Files

- `mp_dualmode.h` - Header with macros for dual-mode compatibility
- `dualmode_simple.c` - Simple example module
- `dualmode_example.c` - Complex example with class
- `test_dualmode.py` - Test suite
- `build_and_test.sh` - Automated build/test script
- `DUALMODE_MODULES.md` - Detailed documentation

## Key Features

1. **Single Source** - Write once, compile both ways
2. **Minimal Overhead** - Macros expand to efficient code
3. **Type Safety** - Proper handling of ROM vs runtime objects
4. **Build Integration** - Works with existing MicroPython build system

See `DUALMODE_MODULES.md` for detailed documentation.