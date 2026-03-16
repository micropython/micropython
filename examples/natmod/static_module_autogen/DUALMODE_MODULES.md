# Dual-Mode Modules for MicroPython

This directory demonstrates how to create C modules that can work as both:
1. **User C modules** - compiled directly into the firmware
2. **Native modules** - compiled as dynamic .mpy files

## Overview

The key innovation is the `mp_dualmode.h` header file which provides macros that adapt based on the compilation context. This allows the same source code to be used for both module types with minimal conditional compilation.

## Key Differences Bridged

### 1. Module Registration
- **User C modules**: Use `MP_REGISTER_MODULE` which is processed at build time
- **Native modules**: Need `mpy_init` function with runtime registration
- **Solution**: Macros that generate appropriate code for each context

### 2. QSTR Handling
- **User C modules**: QSTRs are compile-time constants via `MP_ROM_QSTR`
- **Native modules**: QSTRs are runtime values via `MP_OBJ_NEW_QSTR`
- **Solution**: `MP_DUALMODE_ROM_QSTR` macro that adapts to context

### 3. Module Globals Table
- **User C modules**: Use `mp_rom_map_elem_t` with key-value pairs
- **Native modules**: Can use `mp_rom_obj_t` array with alternating keys/values
- **Solution**: Macros that generate the appropriate structure

### 4. Memory Allocation
- **User C modules**: Use `mp_obj_malloc`
- **Native modules**: Use `m_new_obj`
- **Solution**: Conditional compilation where needed

## Files in This Directory

### Core Infrastructure
- `mp_dualmode.h` - Header with dual-mode macros
- `dualmode_simple.c` - Simple example module
- `dualmode_example.c` - Complex example with class
- `micropython.cmake` - CMake config for user C modules
- `micropython.mk` - Makefile config for user C modules
- `Makefile.natmod` - Makefile for building native modules
- `build_and_test.sh` - Build and test automation script
- `test_dualmode.py` - Test suite for both modules

### Usage

#### Building as Native Modules (.mpy files)
```bash
# Using the build script
./build_and_test.sh --native

# Or manually
make -f Makefile.natmod
# Creates dualmode_simple.mpy and dualmode_example.mpy
```

#### Building as User C Modules (in firmware)
```bash
# Using the build script
./build_and_test.sh --firmware

# Or manually for Unix port
cd ../../../ports/unix
make USER_C_MODULES=/path/to/this/dir
```

#### Testing Both Modes
```bash
# Test everything
./build_and_test.sh --both

# Or test individually
micropython test_dualmode.py
```

## Writing Dual-Mode Modules

### Basic Pattern

```c
#include "mp_dualmode.h"

// Define QSTRs
MP_DUALMODE_QSTR_DEF(mymodule)
MP_DUALMODE_QSTR_DEF(myfunc)

// Define functions
static mp_obj_t myfunc(mp_obj_t arg) {
    // Implementation
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(myfunc_obj, myfunc);

// Define module globals
MP_DUALMODE_MODULE_GLOBALS_TABLE(mymodule) = {
    MP_DUALMODE_MODULE_ATTR(MP_QSTR___name__, MP_DUALMODE_ROM_QSTR(MP_QSTR_mymodule))
    MP_DUALMODE_MODULE_ATTR(MP_QSTR_myfunc, MP_DUALMODE_ROM_PTR(&myfunc_obj))
MP_DUALMODE_MODULE_END()

// Generate init function for native module
MP_DUALMODE_INIT_MODULE(mymodule)

// Define module for user C module
MP_DUALMODE_MODULE_FINALIZE(mymodule, MP_QSTR_mymodule)
```

### Using the Convenience Macros

```c
#include "mp_dualmode.h"

// Functions definitions...

// Simpler module definition
MP_DUALMODE_MODULE_BEGIN(mymodule, mymodule)
    MP_DUALMODE_MODULE_ATTR(MP_QSTR_func1, MP_DUALMODE_ROM_PTR(&func1_obj))
    MP_DUALMODE_MODULE_ATTR(MP_QSTR_CONST1, MP_DUALMODE_ROM_INT(42))
MP_DUALMODE_MODULE_COMPLETE(mymodule, mymodule)

// Finalize for user C module
MP_DUALMODE_MODULE_FINALIZE(mymodule, MP_QSTR_mymodule)
```

## Limitations and Considerations

### 1. Type Objects
Creating type objects (classes) is more complex in native modules because the type structure must be initialized at runtime. The `dualmode_example.c` shows how to handle this with conditional compilation.

### 2. Memory Management
Be careful with memory allocation patterns. Use the appropriate allocation function for each context.

### 3. Build System Integration
The current approach requires:
- Proper QSTR extraction for both contexts
- Separate build configurations
- May need adjustments for different MicroPython ports

### 4. Performance
Native modules have a slight overhead due to runtime registration, but this is typically negligible.

### 5. Debugging
Debugging may be different between the two modes:
- User C modules: Can use firmware debugging tools
- Native modules: More limited debugging options

## Advanced Features

### Conditional Behavior
You can detect which mode you're in:

```c
#ifdef MICROPY_ENABLE_DYNRUNTIME
    // Native module specific code
#else
    // User C module specific code
#endif
```

### Module Information
The examples show how to expose module version and mode information to Python code, useful for debugging and version management.

## Future Improvements

1. **Automated QSTR extraction** for native modules
2. **Better type object support** with helper macros
3. **Build system integration** to automatically build both versions
4. **Testing framework** specifically for dual-mode modules

## Comparison with Alternatives

### Pure Native Module Approach
- Pro: No conditional compilation needed
- Con: Can't be included in firmware, more boilerplate

### Pure User C Module Approach  
- Pro: Simpler, no runtime overhead
- Con: Must rebuild firmware, can't be distributed as .mpy

### Generated Init Function Approach
- Pro: Very clean module code
- Con: Requires code generation step, more complex build

### This Dual-Mode Approach
- Pro: Single source, works both ways, minimal overhead
- Con: Some conditional compilation, slightly more complex than single-mode

## Conclusion

The dual-mode approach provides a practical way to write modules that can be deployed either as part of the firmware or as dynamic modules. This is particularly useful for:

- Module development and testing
- Creating modules that users can choose how to deploy
- Maintaining compatibility across different MicroPython configurations
- Distributing modules that work on systems with and without dynamic loading

The overhead is minimal and the benefits of maintaining a single codebase often outweigh the slight additional complexity.