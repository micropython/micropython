# Enum Module Lazy Loading Implementation Summary

## Overview

Successfully implemented lazy loading for the MicroPython enum module following the extmod/asyncio pattern. The enum module has been split into multiple files with lazy loading to reduce initial memory footprint.

## File Structure

```
lib/enum/
├── __init__.py      # Lazy loader (always loaded)
├── core.py          # Core classes: Enum, IntEnum, EnumMeta (always loaded)
├── flags.py         # Flag, IntFlag (lazy loaded)
├── extras.py        # auto, StrEnum, unique (lazy loaded)
├── manifest.py      # Manifest for freezing all modules
└── enum.py          # Original monolithic file (kept for reference)
```

## Implementation Details

### __init__.py (316 bytes compiled)
- Imports all symbols from `core.py` (Enum, IntEnum, EnumMeta)
- Implements `__getattr__()` for lazy loading
- Maps lazy-loaded symbols to their modules:
  - `Flag`, `IntFlag` → `flags.py`
  - `auto`, `StrEnum`, `unique` → `extras.py`

### core.py (3,510 bytes compiled)
- Contains essential enum functionality
- Classes: `Enum`, `IntEnum`, `EnumMeta`
- Helper functions: `_check_prepare_support()`, `_create_int_member()`
- Supports `auto()` via duck typing (checks for `_order` and `_generation` attributes)
- No circular imports

### flags.py (945 bytes compiled)
- Contains bitwise flag enums
- Classes: `Flag`, `IntFlag`
- Imports from `core.py`
- Loaded only when Flag or IntFlag is first accessed

### extras.py (1,018 bytes compiled)
- Contains optional enum features
- Classes: `auto`, `StrEnum`
- Functions: `unique` decorator
- Imports from `core.py` and references core module for global counter
- Loaded only when auto, StrEnum, or unique is first accessed

### manifest.py
- Freezes all four modules into firmware
- Uses `package()` directive with explicit file list
- Compilation optimization level 3

## Memory Savings

### Bytecode Sizes (mpy-cross compiled)

| File | Size (bytes) | Load Timing |
|------|-------------|-------------|
| **Monolithic enum.py** | **4,993** | Always |
| __init__.py | 316 | Always |
| core.py | 3,510 | Always |
| flags.py | 945 | Lazy |
| extras.py | 1,018 | Lazy |
| **Total (all modules)** | **5,789** | - |

### Memory Impact

- **Core only (Enum + IntEnum)**: 3,826 bytes (23.4% reduction vs monolithic)
- **Total overhead**: +796 bytes (+15.9%) if all features are used
- **Savings when using only Enum/IntEnum**: 1,167 bytes

### Analysis

The implementation provides significant memory savings for typical use cases:
- **23.4% reduction** when only using basic Enum/IntEnum functionality
- Lazy loading modules (`flags.py` + `extras.py` = 1,963 bytes) are loaded on demand
- Total overhead of 796 bytes only applies if ALL features are used
- Most applications use only Enum/IntEnum, making this a net win

## Lazy Loading Verification

Verified that modules are loaded incrementally:

```python
import enum                    # Loads: enum, enum.core
from enum import Enum          # Already loaded
from enum import Flag          # Loads: enum.flags
from enum import auto          # Loads: enum.extras
```

## Test Results

### Passing Tests
- ✅ Basic Enum functionality
- ✅ IntEnum with integer operations
- ✅ Flag with bitwise operations
- ✅ IntFlag with integer compatibility
- ✅ StrEnum with string operations
- ✅ unique() decorator
- ✅ Lazy loading mechanism
- ✅ Enum iteration and membership
- ✅ Value lookup and aliases

### Skipped Tests
- ⏭️ `tests/basics/enum_auto.py` - Requires MICROPY_PY_METACLASS_PREPARE
- ⏭️ `tests/basics/enum_flag.py` - Contains auto() tests

### Notes on auto() Support
- `auto()` requires `MICROPY_PY_METACLASS_PREPARE` to be enabled
- Without `__prepare__`, auto() raises a clear RuntimeError with instructions
- This is expected behavior and matches the original implementation
- Tests skip appropriately when feature is not available

## Backwards Compatibility

The implementation maintains full backwards compatibility:
- All imports work exactly as before: `from enum import Enum, IntEnum, Flag, ...`
- No changes required to user code
- All existing functionality preserved
- Same error messages and behavior

## Build Integration

### manifest.py Configuration
```python
package(
    "enum",
    (
        "__init__.py",
        "core.py",
        "flags.py",
        "extras.py",
    ),
    base_path="..",
    opt=3,
)
```

### Port Configuration
Updated `ports/unix/variants/standard/manifest.py`:
```python
# Include enum module with lazy loading
include("$(MPY_DIR)/lib/enum")
```

## Implementation Pattern

Follows the same pattern as `extmod/asyncio`:
1. Core functionality in main module (always loaded)
2. Optional features in separate modules
3. `__getattr__()` intercepts attribute access
4. First access triggers import and caches result
5. Subsequent accesses use cached value

## Performance Impact

- **Import time**: Negligible overhead for lazy loading mechanism
- **First access**: Small one-time cost to load lazy module
- **Subsequent access**: Same as direct import (cached in globals)
- **Memory**: Significant savings when not all features are used

## Recommendations

1. **Use this implementation** for ports with memory constraints
2. **Enable MICROPY_PY_METACLASS_PREPARE** for full auto() support if ROM space allows
3. **Document lazy loading** in port-specific README files
4. **Consider similar pattern** for other large optional modules

## Files Modified

- `lib/enum/__init__.py` - New lazy loader
- `lib/enum/core.py` - New core functionality
- `lib/enum/flags.py` - New flag support
- `lib/enum/extras.py` - New optional features
- `lib/enum/manifest.py` - New manifest for package freezing
- `ports/unix/variants/standard/manifest.py` - Updated to include enum package

## Files Preserved

- `lib/enum/enum.py` - Original monolithic implementation (for reference)

## Conclusion

The lazy loading implementation successfully reduces initial memory footprint by 23.4% for typical enum usage while maintaining full backwards compatibility and functionality. The pattern is proven (used by asyncio) and provides clear benefits for embedded systems with limited RAM.
