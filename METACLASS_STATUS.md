# Metaclass Implementation Status (WIP)

**Branch:** py-metaclass
**Commit:** b31c1de89a - "py: WIP getting metaclass working."
**Date:** 2020-06-02
**Status:** Work In Progress - Incomplete

## Summary

This branch implements partial metaclass support for MicroPython. Basic infrastructure is in place but several critical features remain unimplemented, causing 3 test failures.

## Implemented Features ✓

### 1. Metaclass Keyword Argument Support
- **File:** py/modbuiltins.c
- **Change:** `__build_class__` now accepts `metaclass` keyword argument
- **Details:**
  - Function signature changed from `VAR` to `KW` to accept kwargs
  - Checks kwargs for `MP_QSTR_metaclass` and uses it if present
  - Falls back to `type` or base class type if not specified

### 2. Type.__new__ Method Skeleton
- **File:** py/objtype.c
- **Change:** Added `type.__new__()` static method
- **Details:**
  - Implements 4-argument form: `type.__new__(metacls, name, bases, dict)`
  - Creates new type and sets its base.type to the metaclass
  - Accessible via attribute lookup on type objects

### 3. Basic Metaclass Instantiation
- **Working:** Defining a class with `metaclass=Meta` works
- **Working:** Meta.__init__ gets called during class creation
- **Working:** First-level metaclass usage (e.g., creating class Foo with Meta)

### 4. Improved Error Messages
- **File:** py/objtype.c
- **Change:** Better TypeError messages with descriptive text
- **Examples:**
  - "bases not a tuple"
  - "locals not a dict"
  - "bases not a type"
  - "incorrect subclass"

### 5. Debug Infrastructure
- **File:** py/objtype.c
- **Change:** Added DEBUG_printf statements for debugging:
  - mp_obj_instance_make_new()
  - mp_obj_instance_attr()
  - mp_obj_instance_call()
  - type_make_new()
  - type_call()
  - type_attr() for __new__ access

## Not Yet Implemented / Broken ✗

### 1. Inheritance from Metaclass-Based Classes
- **Status:** BROKEN
- **Error:** `TypeError: bases not a type`
- **Location:** py/objtype.c:1215-1218
- **Issue:**
  ```c
  for (size_t i = 0; i < bases_len; i++) {
      if (!mp_obj_is_type(bases_items[i], &mp_type_type)) {
          // Somehow we need to loosen this restriction for metaclasses to work....
          mp_raise_TypeError(MP_ERROR_TEXT("bases not a type"));
      }
  ```
- **Impact:** Cannot create a class that inherits from a class with a custom metaclass
- **Example:** `class Foo2(Foo):` fails when Foo has metaclass=Meta
- **Note:** Original developer left comment indicating awareness of this limitation

### 2. Single-Argument __new__ Form
- **Status:** BROKEN
- **Error:** `TypeError: function takes 4 positional arguments but 1 were given`
- **Location:** py/objtype.c:1034-1042
- **Issue:** `type___new__()` only implements 4-arg form for type creation
  ```c
  static mp_obj_t type___new__(size_t n_args, const mp_obj_t *args) {
      (void)n_args;
      mp_obj_t type = mp_obj_new_type(mp_obj_str_get_qstr(args[1]), args[2], args[3]);
      ((mp_obj_type_t *)MP_OBJ_TO_PTR(type))->base.type = args[0];
      return type;
  }
  static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(type___new___obj, 4, 4, type___new__);
  ```
- **Missing:** Single-argument form for instance allocation: `cls.__new__(cls)`
- **Impact:**
  - `A.__new__(A)` fails
  - `object.__new__(Foo)` fails
  - Any manual instance allocation fails

### 3. object.__new__ Integration
- **Status:** BROKEN (dependent on #2)
- **Impact:** Cannot use `object.__new__(cls)` for instance allocation
- **Test:** tests/basics/object_new.py fails

### 4. Class Method __new__ Access
- **Status:** BROKEN (dependent on #2)
- **Impact:** Cannot call `instance.__new__(cls)` as expected
- **Test:** tests/basics/class_new.py fails at line 34: `a = a.__new__(A)`

## Test Results

**Test Suite:** tests/basics/
**Total:** 539 tests (16,794 testcases)
**Passed:** 536 tests
**Skipped:** 14 tests
**Failed:** 3 tests

### Failed Tests Detail

#### tests/basics/metaclass.py
```python
class Meta(type):
    def __init__(cls, name, bases, dct):
        super().__init__(name, bases, dct)  # Works

class Foo(metaclass=Meta):  # Works
    pass

class Foo2(Foo):  # FAILS - "bases not a type"
    x = 1
```

**Failure Point:** Line 30, when defining Foo2
**Error:** TypeError: bases not a type
**Root Cause:** Issue #1 above

#### tests/basics/class_new.py
```python
class A:
    def __new__(cls):
        return super(cls, A).__new__(cls)

a = A.__new__(A)  # FAILS - "function takes 4 positional arguments but 1 were given"
```

**Failure Point:** Line 26
**Root Cause:** Issue #2 above

#### tests/basics/object_new.py
```python
class Foo:
    def __new__(cls):
        raise RuntimeError
    def __init__(self):
        self.attr = "something"

o = object.__new__(Foo)  # FAILS - "function takes 4 positional arguments but 1 were given"
```

**Failure Point:** Line 25
**Root Cause:** Issue #2 above

## Technical Implementation Details

### Changes to py/modbuiltins.c
```c
// Before: MP_DEFINE_CONST_FUN_OBJ_VAR(mp_builtin___build_class___obj, 2, mp_builtin___build_class__);
// After:  MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin___build_class___obj, 2, mp_builtin___build_class__);

static mp_obj_t mp_builtin___build_class__(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    // Extract metaclass from kwargs
    mp_map_elem_t *key_elem = mp_map_lookup(kwargs, MP_OBJ_NEW_QSTR(MP_QSTR_metaclass), MP_MAP_LOOKUP);
    if (key_elem != NULL) {
        meta = key_elem->value;
    } else {
        // Fallback to type or base class type
    }
}
```

### Changes to py/objtype.c

**Added type.__new__ method:**
```c
static mp_obj_t type___new__(size_t n_args, const mp_obj_t *args) {
    mp_obj_t type = mp_obj_new_type(mp_obj_str_get_qstr(args[1]), args[2], args[3]);
    ((mp_obj_type_t *)MP_OBJ_TO_PTR(type))->base.type = args[0];
    return type;
}
```

**Restriction in mp_obj_new_type():**
```c
// Line 1214-1218
for (size_t i = 0; i < bases_len; i++) {
    if (!mp_obj_is_type(bases_items[i], &mp_type_type)) {
        // TODO: Loosen this for metaclasses
        mp_raise_TypeError(MP_ERROR_TEXT("bases not a type"));
    }
}
```

## Required Fixes

### Fix #1: Allow Metaclass-Based Classes as Bases
**Location:** py/objtype.c:1215-1218

**Current Check:**
```c
if (!mp_obj_is_type(bases_items[i], &mp_type_type))
```

**Proposed Solution:**
- Check if base class's type is itself a type (i.e., is it a metaclass?)
- Allow base if `mp_obj_is_subclass_fast(MP_OBJ_FROM_PTR(mp_obj_get_type(bases_items[i])), &mp_type_type)`
- This would allow any class whose metaclass inherits from type

### Fix #2: Implement Dual-Signature __new__
**Location:** py/objtype.c:1034-1042

**Current:**
- Only 4-arg form: `type.__new__(metacls, name, bases, dict)`

**Required:**
- 1-arg form: `cls.__new__(cls)` - allocate instance of cls
- 4-arg form: `metacls.__new__(metacls, name, bases, dict)` - create new type
- Need to distinguish based on n_args
- 1-arg should call `mp_obj_new_instance()` or equivalent
- 4-arg should continue current behavior

**Suggested Implementation:**
```c
static mp_obj_t type___new__(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    if (n_args == 1 || (n_args == 2 && kwargs->used == 0)) {
        // Instance allocation: cls.__new__(cls) or type.__new__(cls)
        const mp_obj_type_t *native_base;
        return MP_OBJ_FROM_PTR(mp_obj_new_instance(MP_OBJ_TO_PTR(args[0]), &native_base));
    } else if (n_args == 4) {
        // Type creation: metacls.__new__(metacls, name, bases, dict)
        mp_obj_t type = mp_obj_new_type(mp_obj_str_get_qstr(args[1]), args[2], args[3]);
        ((mp_obj_type_t *)MP_OBJ_TO_PTR(type))->base.type = args[0];
        return type;
    } else {
        mp_raise_TypeError(MP_ERROR_TEXT("__new__ takes 1 or 4 arguments"));
    }
}
```

### Fix #3: Update Function Object Macro
**Location:** py/objtype.c:1042

**Current:**
```c
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(type___new___obj, 4, 4, type___new__);
```

**Required:**
```c
static MP_DEFINE_CONST_FUN_OBJ_KW(type___new___obj, 1, type___new__);
```

## Cosmetic Differences

### Dict Key Ordering
**Issue:** MicroPython and CPython produce dict keys in different orders
**Impact:** Cosmetic only - test output differs but functionality identical
**Example:**
- CPython: `{'__module__': '__main__', '__qualname__': 'Foo'}`
- MicroPython: `{'__qualname__': 'Foo', '__module__': '__main__'}`

### Class Repr Format
**Issue:** Class name formatting differs slightly
**Impact:** Cosmetic only
**Example:**
- CPython: `<class '__main__.Foo'>`
- MicroPython: `<class 'Foo'>`

## Build Information

**mpy-cross:** 408,806 bytes
**unix port:** 781,110 bytes text + 70,712 bytes data
**Build time:** ~45 seconds (mpy-cross) + ~90 seconds (unix port)
**Dependencies:** lib/mbedtls, lib/berkeley-db-1.xx, lib/micropython-lib

## Next Steps for Completion

1. **Fix base class validation** - Allow metaclass-based classes as bases
2. **Implement dual-signature __new__** - Support both 1-arg and 4-arg forms
3. **Test inheritance chains** - Verify multi-level metaclass inheritance
4. **Handle edge cases:**
   - Multiple inheritance with mixed metaclasses
   - Metaclass conflicts
   - Native type bases with custom metaclasses
5. **Add comprehensive tests** - Cover all metaclass scenarios
6. **Documentation** - Update docs/library/builtins.rst

## References

- **Commit:** b31c1de89a599eab17fcca7198e40c66b1871686
- **Files Modified:**
  - py/modbuiltins.c (metaclass kwarg support)
  - py/objtype.c (type.__new__, debug output, error messages)
  - tests/basics/metaclass.py (new test, currently failing)
- **CPython Documentation:** https://docs.python.org/3/reference/datamodel.html#metaclasses
