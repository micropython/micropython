# __prepare__ Implementation Analysis

## Current Status (b31c1de89a599eab17fcca7198e40c66b1871686)

**Result**: `__prepare__` does NOT exist in the original commit.

```bash
$ git show b31c1de89a599eab17fcca7198e40c66b1871686:py/objtype.c | grep -i prepare
# No output - feature does not exist
```

---

## PR #18362 Implementation

### Overview
PR #18362 implements **PEP 3115** (metaclass support) and **PEP 487** (`__init_subclass__`), including optional `__prepare__` support.

### Configuration Flags

```c
// py/mpconfig.h
#ifndef MICROPY_METACLASS
#define MICROPY_METACLASS (0)
#endif

#ifndef MICROPY_METACLASS_PREPARE
#define MICROPY_METACLASS_PREPARE (0)
#endif

#ifndef MICROPY_INIT_SUBCLASS
#define MICROPY_INIT_SUBCLASS (0)
#endif
```

### Key Implementation (py/modbuiltins.c)

The `__prepare__` implementation is in `mp_builtin___build_class__()`:

```c
// Call __prepare__ if it exists on the metaclass
mp_obj_t class_locals;
#if MICROPY_METACLASS_PREPARE
mp_obj_t prepare_dest[2];
mp_load_method_maybe(meta, MP_QSTR___prepare__, prepare_dest);

if (prepare_dest[0] != MP_OBJ_NULL) {
    // __prepare__ exists, call it with (name, bases)
    mp_obj_t prepare_args[4];
    prepare_args[0] = prepare_dest[0];
    prepare_args[1] = prepare_dest[1];
    prepare_args[2] = args[1]; // class name
    prepare_args[3] = mp_obj_new_tuple(n_args - 2, args + 2); // tuple of bases
    class_locals = mp_call_method_n_kw(2, 0, prepare_args);
} else {
    class_locals = mp_obj_new_dict(0);
}
#else
// __prepare__ not supported, always use regular dict
class_locals = mp_obj_new_dict(0);
#endif
```

**Key details:**
1. `__prepare__` is called **before** the class body executes
2. It receives `(metaclass, name, bases)` as arguments (method form)
3. Returns a namespace dict (can be custom, e.g., OrderedDict for tracking insertion order)
4. If `__prepare__` doesn't exist, falls back to regular dict
5. Completely behind `MICROPY_METACLASS_PREPARE` flag (zero cost when disabled)

---

## Size Impact Analysis (from PR #18362)

### Incremental Costs

| Feature | Configuration | Size (Unix) | Delta | % Increase |
|---------|--------------|-------------|-------|------------|
| **Baseline** | All disabled | 177,717 bytes | - | - |
| **METACLASS only** | `MICROPY_METACLASS=1` | 178,341 bytes | **+624 bytes** | +0.32% |
| **METACLASS + PREPARE** | `+MICROPY_METACLASS_PREPARE=1` | 178,493 bytes | **+776 bytes** | +0.40% |

**`__prepare__` incremental cost**: **152 bytes** (776 - 624)

This is more than the 50-100 bytes estimated in ENUM_EXTENSIONS_PLAN.md, but still reasonable.

### For STM32 (from PR):
- Similar overhead expected (~150-180 bytes for `__prepare__` alone)

---

## Comparison with ENUM_EXTENSIONS_PLAN.md

### Plan Estimate vs PR Implementation

| Aspect | ENUM_EXTENSIONS_PLAN.md | PR #18362 Reality |
|--------|-------------------------|-------------------|
| **C Code Location** | `py/objtype.c` in `type_call()` or `mp_obj_new_type()` | **`py/modbuiltins.c`** in `mp_builtin___build_class__()` |
| **Size Estimate** | 50-100 bytes | **152 bytes** |
| **Flag Name** | `MICROPY_PY_ENUM_AUTO` | **`MICROPY_METACLASS_PREPARE`** (separate from enum) |
| **Dependency** | Listed as needed for auto() | **Separate feature** - also used by PEP 3115 |

**Key Insight**: The ENUM_EXTENSIONS_PLAN.md placed `__prepare__` in the wrong file. It should be in **`py/modbuiltins.c`**, not `py/objtype.c`.

---

## Why py/modbuiltins.c, Not py/objtype.c?

### Python Class Creation Flow

1. **Compiler**: Generates `BUILD_CLASS` opcode
2. **VM (py/vm.c)**: Calls builtin `__build_class__`
3. **`py/modbuiltins.c`**: `mp_builtin___build_class__()` function:
   - Resolves metaclass
   - **Calls `__prepare__` to get namespace dict** ← This is where it belongs
   - Executes class body code (populates namespace)
   - Calls metaclass to create class object
4. **`py/objtype.c`**: `mp_obj_new_type()` or metaclass `__new__`:
   - Receives already-populated namespace dict
   - Creates the actual type object
   - **Calls metaclass `__init__`** ← Our implementation

**Conclusion**: `__prepare__` must be called **before** the class body executes, which happens in `__build_class__`, not in type creation.

---

## Suggested Implementation for py-metaclass-enum Branch

### Option 1: Minimal Implementation (Just for auto())

If you only need `__prepare__` for `auto()` support in enums:

**File**: `py/modbuiltins.c`

```c
// Around line 50-60 in mp_builtin___build_class__()

static mp_obj_t mp_builtin___build_class__(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // ... existing metaclass resolution code ...

    // Create class namespace dict
    mp_obj_t class_locals;

    #if MICROPY_PY_ENUM_AUTO
    // Check if metaclass has __prepare__
    mp_obj_t prepare_dest[2] = {MP_OBJ_NULL, MP_OBJ_NULL};
    mp_load_method_maybe(meta, MP_QSTR___prepare__, prepare_dest);

    if (prepare_dest[0] != MP_OBJ_NULL) {
        // Call __prepare__(name, bases)
        mp_obj_t prepare_args[4];
        prepare_args[0] = prepare_dest[0];  // method function
        prepare_args[1] = prepare_dest[1];  // self (metaclass or None)
        prepare_args[2] = args[1];          // class name
        prepare_args[3] = mp_obj_new_tuple(n_args - 2, args + 2); // bases tuple
        class_locals = mp_call_method_n_kw(2, 0, prepare_args);
    } else {
        class_locals = mp_obj_new_dict(0);
    }
    #else
    class_locals = mp_obj_new_dict(0);
    #endif

    // ... rest of function continues using class_locals ...
}
```

**Size estimate**: ~150-180 bytes

**Benefits**:
- Minimal implementation
- Reuses existing `MICROPY_PY_ENUM_AUTO` flag
- No new config flags needed

**Drawbacks**:
- Flag name (`MICROPY_PY_ENUM_AUTO`) doesn't accurately describe what it enables
- If someone wants `__prepare__` without enum, they can't get it

### Option 2: Proper Implementation (Following PR #18362)

Adopt the PR's approach with separate flags:

**File**: `py/mpconfig.h`

```c
// Whether to support metaclasses with the PEP 3115 metaclass= syntax
// This is already implemented in your branch!
#ifndef MICROPY_PY_METACLASS_INIT
#define MICROPY_PY_METACLASS_INIT (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to support metaclass __prepare__ method (PEP 3115)
// Allows metaclasses to customize the namespace dict before class creation
#ifndef MICROPY_PY_METACLASS_PREPARE
#define MICROPY_PY_METACLASS_PREPARE (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_FULL_FEATURES)
#endif
```

**File**: `py/modbuiltins.c` - Same implementation as Option 1, but guarded by `MICROPY_PY_METACLASS_PREPARE`

**File**: `lib/enum/enum.py`

```python
#if MICROPY_PY_METACLASS_PREPARE and MICROPY_PY_ENUM_AUTO
class EnumMeta(type):
    @classmethod
    def __prepare__(mcs, name, bases):
        """Return an ordered namespace for tracking member order"""
        # In MicroPython, dicts are already ordered
        return {}

    def __new__(mcs, name, bases, namespace):
        # Process auto() values in namespace...
        # ...
#endif
```

**Size estimate**: ~150-180 bytes for C code

**Benefits**:
- Accurate flag naming
- Follows upstream conventions (easier to merge/maintain)
- Enables `__prepare__` for other use cases beyond enum
- Cleaner separation of concerns

**Drawbacks**:
- More configuration flags
- Slightly more complex (but more maintainable)

---

## Recommendation

**Use Option 2 (Proper Implementation)** for the following reasons:

1. **Upstream alignment**: PR #18362 is likely to be merged eventually. Using the same flag names and structure makes future merges easier.

2. **Correct abstraction**: `__prepare__` is a general metaclass feature, not enum-specific. It should have its own flag.

3. **Size is acceptable**: 150-180 bytes for a useful feature is reasonable at FULL_FEATURES level.

4. **Future-proof**: Other libraries beyond enum might benefit from `__prepare__` (e.g., ORM frameworks, configuration systems).

5. **Clean implementation**: The PR #18362 code is well-tested and production-ready.

---

## Implementation Steps for py-metaclass-enum Branch

### Step 1: Add configuration flag
**File**: `py/mpconfig.h`

Add after the existing `MICROPY_PY_METACLASS_INIT` definition:

```c
// Whether to support metaclass __prepare__ method (PEP 3115)
// Allows metaclasses to customize the namespace dict before class creation
// Required for enum.auto() to track insertion order
#ifndef MICROPY_PY_METACLASS_PREPARE
#define MICROPY_PY_METACLASS_PREPARE (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_FULL_FEATURES)
#endif
```

### Step 2: Add QSTR for __prepare__
**File**: `py/qstrdefs.h`

Check if `MP_QSTR___prepare__` exists. If not, add:
```c
Q(__prepare__)
```

### Step 3: Modify __build_class__
**File**: `py/modbuiltins.c`

Find the `mp_builtin___build_class__()` function and add the `__prepare__` support as shown in Option 2 above.

### Step 4: Update enum.py
**File**: `lib/enum/enum.py`

Add `__prepare__` to EnumMeta when `MICROPY_PY_ENUM_AUTO` is enabled.

### Step 5: Test
Create `tests/basics/class_metaclass_prepare.py`:

```python
# Test __prepare__ with OrderedDict-like behavior
class Meta(type):
    @classmethod
    def __prepare__(mcs, name, bases):
        print("__prepare__ called for", name)
        # Return regular dict (MicroPython dicts are already ordered)
        return {}

class Test(metaclass=Meta):
    x = 1
    y = 2

print("Test created successfully")
```

### Step 6: Measure size impact
```bash
# Before
cd ports/unix && make clean && make -j
size build-standard/micropython | awk 'NR==2 {print $1}'

# After (with __prepare__)
make clean
make -j CFLAGS_EXTRA="-DMICROPY_PY_METACLASS_PREPARE=1"
size build-standard/micropython | awk 'NR==2 {print $1}'
```

---

## Summary

- **Current branch**: `__prepare__` does NOT exist
- **Required location**: `py/modbuiltins.c` (NOT `py/objtype.c`)
- **Estimated size**: ~150-180 bytes
- **Recommendation**: Implement using PR #18362 approach with `MICROPY_PY_METACLASS_PREPARE` flag
- **Benefit**: Enables `auto()` for enum and other advanced metaclass patterns

The implementation is straightforward and follows established patterns from PR #18362, which has comprehensive testing and size measurements.
