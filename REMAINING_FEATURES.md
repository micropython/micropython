# Remaining Features from ENUM_EXTENSIONS_PLAN.md

## Status Overview

### ✅ Completed Features

#### Metaclass Support (Foundation)
1. **✅ Metaclass `__init__`** - Custom class initialization
   - Implemented in: `py/objtype.c` (lines 1554-1578)
   - Config flag: `MICROPY_PY_METACLASS_INIT`
   - Commit: 54c126b403
   - Size: ~25 lines of C code
   - Status: **DONE**

2. **✅ Metaclass properties** - Property/method lookup on classes
   - Implemented in: `py/objtype.c` (lines 1200-1230)
   - Config flag: `MICROPY_PY_METACLASS_PROPERTIES`
   - Commit: 54c126b403
   - Size: ~35 lines of C code
   - Status: **DONE**

3. **✅ Metaclass operators** - Operator support on metaclasses
   - Implemented in: `py/objtype.c` (lines 1271-1399)
   - Config flag: `MICROPY_PY_METACLASS_OPS`
   - Commit: d1716d6cc8
   - Size: ~120 lines of C code
   - Status: **DONE**

4. **✅ Metaclass `__prepare__`** - Namespace customization (PEP 3115)
   - Implemented in: `py/modbuiltins.c` (lines 69-90)
   - Config flag: `MICROPY_PY_METACLASS_PREPARE`
   - Commit: 5c3e108e4b
   - Size: ~40 lines of C code
   - Status: **DONE**

#### Enum Features
5. **✅ Enum** - Base enum class
   - Implemented in: `lib/enum/enum.py`
   - Lines: 216-265
   - Status: **DONE**

6. **✅ IntEnum** - Integer-valued enum
   - Implemented in: `lib/enum/enum.py`
   - Lines: 267-387
   - Status: **DONE**

7. **✅ auto()** - Automatic value generation
   - Implemented in: `lib/enum/enum.py`
   - Lines: 26-44 (auto class), 91-122 (processing in EnumMeta.__new__)
   - Requires: `MICROPY_PY_METACLASS_PREPARE`
   - Commit: 35d4b41986
   - Status: **DONE**

8. **✅ @unique** - Enforce no duplicate values
   - Implemented in: `lib/enum/enum.py`
   - Lines: 389-408
   - Status: **DONE**

---

## ❌ Missing Features

### 1. Flag and IntFlag (Priority: High)

**Description**: Bitwise flag enums for combining enum members with `|`, `&`, `^` operators

**Size Estimate**: ~200 bytes Python code

**ROM Level**: EXTRA_FEATURES

**CPython Version**: 3.6+

**Dependencies**:
- ✅ `MICROPY_PY_METACLASS_OPS` (already implemented)

**Implementation Required**:
```python
# In lib/enum/enum.py
class Flag(Enum):
    """Enum subclass that supports bitwise operations"""

    def __or__(self, other):
        if not isinstance(other, self.__class__):
            return NotImplemented
        return self.__class__(self._value_ | other._value_)

    def __and__(self, other):
        if not isinstance(other, self.__class__):
            return NotImplemented
        return self.__class__(self._value_ & other._value_)

    def __xor__(self, other):
        if not isinstance(other, self.__class__):
            return NotImplemented
        return self.__class__(self._value_ ^ other._value_)

    def __invert__(self):
        # Return complement of all defined flags
        all_flags = 0
        for member in self.__class__:
            all_flags |= member._value_
        return self.__class__(all_flags & ~self._value_)

class IntFlag(int, Flag):
    """Flag enum that is also compatible with integers"""
    # Similar to IntEnum, but inherits from Flag instead of Enum
```

**Test Plan**: tests/basics/enum_flag.py
- Test bitwise OR, AND, XOR operations
- Test invert (~) operation
- Test IntFlag integer compatibility
- Test repr/str formatting

**Effort**: ~2-3 hours

---

### 2. StrEnum (Priority: Medium)

**Description**: String-valued enums where members are also strings

**Size Estimate**: ~50 bytes Python code

**ROM Level**: EXTRA_FEATURES

**CPython Version**: 3.11+

**Dependencies**: None

**Implementation Required**:
```python
# In lib/enum/enum.py
class StrEnum(str, Enum):
    """Enum where members are also strings"""

    def __new__(cls, value):
        if not isinstance(value, str):
            raise TypeError(f"StrEnum values must be strings, not {type(value).__name__}")
        obj = str.__new__(cls, value)
        obj._value_ = value
        return obj

    def __str__(self):
        return self._value_

    # Inherit all string methods from str base class
```

**Test Plan**: tests/basics/enum_strenum.py
- Test string operations (upper(), lower(), split(), etc.)
- Test string concatenation
- Test equality with strings
- Test name/value attributes still work

**Effort**: ~1-2 hours

---

## Implementation Priority Recommendation

Based on the plan and current state:

### Recommended Order:
1. **Flag/IntFlag** (High priority, high demand, enables bitwise patterns)
2. **StrEnum** (Low complexity, quick win, modern Python feature)

### Rationale:
- **Flag/IntFlag**: Most requested feature, enables important use cases (permissions, status flags)
- **StrEnum**: Very simple (~50 lines), modern Python 3.11+ feature, good for completeness

---

## Size Budget Summary

| Feature | Status | Size (bytes) | Config Flag |
|---------|--------|--------------|-------------|
| Metaclass `__init__` | ✅ Done | ~100 | `MICROPY_PY_METACLASS_INIT` |
| Metaclass properties | ✅ Done | ~200 | `MICROPY_PY_METACLASS_PROPERTIES` |
| Metaclass operators | ✅ Done | ~200 | `MICROPY_PY_METACLASS_OPS` |
| Metaclass `__prepare__` | ✅ Done | ~150 | `MICROPY_PY_METACLASS_PREPARE` |
| auto() | ✅ Done | ~50 Python | (uses __prepare__) |
| @unique | ✅ Done | ~40 Python | (always included) |
| **Flag/IntFlag** | ❌ TODO | ~200 Python | `MICROPY_PY_ENUM_FLAG` |
| **StrEnum** | ❌ TODO | ~50 Python | `MICROPY_PY_ENUM_STRENUM` |

**Total Implemented**: ~650 bytes (C) + ~90 bytes (Python)
**Total Remaining**: ~250 bytes (Python only)

---

## Current Completeness

### By Category:

**Metaclass Support**: 100% ✅
- ✅ `__init__` support
- ✅ Property/method lookup
- ✅ Operator support
- ✅ `__prepare__` support

**Core Enum Features**: 100% ✅
- ✅ Enum base class
- ✅ IntEnum
- ✅ auto() value generation
- ✅ @unique decorator

**Extended Enum Features**: 0% ❌
- ❌ Flag (bitwise operations)
- ❌ IntFlag (integer-compatible flags)
- ❌ StrEnum (string enums)

**Overall Completion**: 8/11 features = **73% complete**

---

## Next Steps

If you want to complete the enum implementation:

```bash
# 1. Implement Flag/IntFlag
#    - Add classes to lib/enum/enum.py
#    - Add tests to tests/basics/enum_flag.py
#    - Update __all__ export

# 2. Implement StrEnum
#    - Add class to lib/enum/enum.py
#    - Add tests to tests/basics/enum_strenum.py
#    - Update __all__ export

# 3. Add conditional compilation
#    - Add MICROPY_PY_ENUM_FLAG flag to py/mpconfig.h
#    - Add MICROPY_PY_ENUM_STRENUM flag to py/mpconfig.h
#    - Wrap implementations with #if guards

# 4. Test and measure
#    - Run test suite
#    - Measure size impact
#    - Document results
```

**Estimated total effort**: 3-5 hours for both features
