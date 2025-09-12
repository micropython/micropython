# Native Module Static Definition System

## Overview

This document describes a proposed enhancement to MicroPython's native module (natmod) system that would allow native modules to use the same static module definition pattern as user C modules (usercmodule). This would reduce boilerplate code and improve consistency across the codebase.

## Current State Analysis

### User C Modules (usercmodule)

User C modules use a static definition pattern:

```c
// Define module globals table
static const mp_rom_map_elem_t module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_module_name) },
    { MP_ROM_QSTR(MP_QSTR_function), MP_ROM_PTR(&function_obj) },
    { MP_ROM_QSTR(MP_QSTR_Class), MP_ROM_PTR(&type_Class) },
};
static MP_DEFINE_CONST_DICT(module_globals, module_globals_table);

// Define module object
const mp_obj_module_t module_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&module_globals,
};

// Register module
MP_REGISTER_MODULE(MP_QSTR_module_name, module_user_cmodule);
```

Key characteristics:
- Module structure is statically defined at compile time
- `MP_REGISTER_MODULE` macro is parsed by `py/makemoduledefs.py`
- Module attributes are defined in a const table
- No runtime initialization code needed

### Native Modules (natmod)

Native modules use a dynamic initialization pattern:

```c
// Module initialization function
mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    MP_DYNRUNTIME_INIT_ENTRY
    
    // Register each attribute individually
    mp_store_global(MP_QSTR_function, MP_OBJ_FROM_PTR(&function_obj));
    mp_store_global(MP_QSTR_Class, MP_OBJ_FROM_PTR(&type_Class));
    
    MP_DYNRUNTIME_INIT_EXIT
}
```

Key characteristics:
- Module name defined in Makefile (`MOD = module_name`)
- Attributes registered dynamically in `mpy_init`
- More boilerplate code required
- Runtime initialization overhead

## Problem Statement

The current native module system has several limitations:

1. **Inconsistency**: Different patterns for defining modules makes code harder to understand and maintain
2. **Boilerplate**: Manual attribute registration is repetitive and error-prone
3. **Maintenance**: Changes to module structure require updates in multiple places
4. **Code Sharing**: Difficult to share module definitions between usercmodule and natmod

## Proposed Solution

### Goal

Enable native modules to use the same static module definition pattern as user C modules while maintaining backward compatibility with existing native modules.

### Approach

1. **Code Generation**: Create a preprocessing tool that parses static module definitions and generates the required `mpy_init` function
2. **Build System Integration**: Integrate the preprocessing step into the native module build process
3. **Runtime Support**: Extend dynruntime to efficiently handle static module definitions
4. **Backward Compatibility**: Support both old and new patterns transparently

### Benefits

- **Consistency**: Same module definition pattern across all module types
- **Less Boilerplate**: Automatic generation of initialization code
- **Better Maintenance**: Module structure defined in one place
- **Code Reuse**: Easier to convert between module types
- **Performance**: Potential optimizations for static definitions

## Implementation Components

### 1. Parser Tool (`py/make_natmod_init.py`)

A Python script that:
- Parses C source files for `MP_REGISTER_MODULE` declarations
- Extracts module name and module object references
- Locates associated globals tables
- Generates appropriate `mpy_init` function

### 2. Build System Updates (`py/dynruntime.mk`)

Modifications to:
- Add preprocessing step before compilation
- Extract module name from source instead of Makefile
- Handle generated initialization code

### 3. Runtime Extensions (`py/dynruntime.h`)

New macros and functions to:
- Support static globals dictionary traversal
- Optimize attribute access for static modules
- Maintain compatibility with dynamic registration

### 4. Migration Path

- Existing native modules continue to work unchanged
- New modules can use static definition pattern
- Gradual migration of existing modules over time

## Next Steps

See [implementation-plan.md](natmod-implementation-plan.md) for detailed technical implementation steps.