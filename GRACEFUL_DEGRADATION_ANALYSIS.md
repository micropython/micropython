# Graceful Degradation Analysis for enum.py

## Issue Summary

The enum.py library has **partial** graceful handling when C metaclass features are disabled, but there's a **critical bug** with auto() that needs to be fixed.

---

## Current Status

### Feature Dependency Matrix

| Enum Feature | Requires C Feature | Graceful Fallback | Status |
|--------------|-------------------|-------------------|--------|
| **Enum base class** | None | N/A | ✅ Works always |
| **IntEnum** | None | N/A | ✅ Works always |
| **Flag** | None | N/A | ✅ Works always |
| **IntFlag** | None | N/A | ✅ Works always |
| **StrEnum** | None | N/A | ✅ Works always |
| **@unique** | None | N/A | ✅ Works always |
| **auto()** | `MICROPY_PY_METACLASS_PREPARE` | ❌ **BROKEN** | ❌ **BUG** |

---

## Detailed Analysis

### ✅ **What Works Without C Features**

#### 1. Basic Enum, IntEnum, Flag, IntFlag, StrEnum
These all work perfectly without any C metaclass features because:
- They only use standard Python metaclass features (\_\_new\_\_)
- Operator methods (\_\_or\_\_, \_\_and\_\_, etc.) are defined on the class, not metaclass
- No dependency on `__prepare__`, metaclass operators, or metaclass properties

**Test:**
```python
# Works even with all C features disabled
class Color(Enum):
    RED = 1
    GREEN = 2
    BLUE = 3

class Permission(Flag):
    READ = 1
    WRITE = 2
    EXECUTE = 4

perms = Permission.READ | Permission.WRITE  # Works!
```

#### 2. @unique decorator
Works because it's pure Python that just inspects the created class.

---

### ❌ **Critical Bug: auto() Fails Silently**

#### The Problem

**Code location:** lib/enum/enum.py, lines 92-122

```python
if _prepare_supported:
    # Process auto() values
    auto_members = []
    # ... replace auto() with integers
```

**When `_prepare_supported = False`:**
- auto() instances are NOT replaced with integers
- They remain as auto() objects in the enum member's `_value_`
- This breaks all value operations

**Actual Behavior:**
```python
# With MICROPY_PY_METACLASS_PREPARE disabled:
class Status(Enum):
    PENDING = auto()
    ACTIVE = auto()

print(Status.PENDING._value_)  # Prints: auto()
print(type(Status.PENDING._value_))  # <class 'auto'>

# This is WRONG - _value_ should be an integer!
```

**Why This Happens:**
1. auto() requires `__prepare__` to track member definition order
2. Without `__prepare__`, there's no way to know which auto() was defined first
3. The code silently skips auto() processing, leaving broken enum members

---

## Recommended Fixes

### Option 1: Fail Fast with Clear Error (RECOMMENDED)

**Add error detection in EnumMeta.__new__:**

```python
def __new__(mcs, name, bases, namespace):
    # Check for auto() when __prepare__ is not supported
    if not _prepare_supported:
        for key, value in namespace.items():
            if isinstance(value, auto):
                raise RuntimeError(
                    f"auto() in {name}.{key} requires MICROPY_PY_METACLASS_PREPARE=1. "
                    f"Use explicit integer values instead, or enable __prepare__ support."
                )

    # ... rest of processing
```

**Pros:**
- Clear error message explaining the problem
- Points users to the solution (enable feature or use explicit values)
- Fails at class definition time, not later during usage
- No performance impact when features are enabled

**Cons:**
- auto() becomes unavailable on minimal builds

---

### Option 2: Simple Fallback (Unreliable Order)

**Modify auto() processing to have a fallback:**

```python
def __new__(mcs, name, bases, namespace):
    # Collect auto() members
    auto_members = []
    explicit_values = []

    for key in namespace.keys():
        if not key.startswith("_"):
            value = namespace[key]
            if not callable(value):
                if isinstance(value, auto):
                    auto_members.append(key)  # Just store key
                elif isinstance(value, int):
                    explicit_values.append(value)

    if auto_members:
        # Determine starting value
        if explicit_values:
            auto_value = max(explicit_values) + 1
        else:
            auto_value = 1

        # WARNING: Order is not guaranteed without __prepare__!
        for key in auto_members:
            namespace[key] = auto_value
            auto_value += 1
```

**Pros:**
- auto() works on minimal builds
- Better than silently failing

**Cons:**
- Member order is **unpredictable** (dict iteration order is undefined pre-Python 3.7)
- Could surprise users with inconsistent behavior
- Different behavior between builds (with/without __prepare__)

---

### Option 3: Hybrid Approach

**Fail fast by default, but allow fallback with warning:**

```python
# At module level
_AUTO_FALLBACK_ENABLED = False  # Can be set by user

def __new__(mcs, name, bases, namespace):
    if not _prepare_supported:
        for key, value in namespace.items():
            if isinstance(value, auto):
                if not _AUTO_FALLBACK_ENABLED:
                    raise RuntimeError(
                        f"auto() requires MICROPY_PY_METACLASS_PREPARE=1. "
                        f"Set enum._AUTO_FALLBACK_ENABLED=True for unreliable fallback."
                    )
                else:
                    import sys
                    print(f"WARNING: auto() order undefined without __prepare__",
                          file=sys.stderr)
                    break  # Only warn once
```

**Pros:**
- Safe by default (fails fast)
- Expert users can opt-in to fallback if needed
- Clear documentation of the limitation

**Cons:**
- More complex

---

## Detection Logic Analysis

### Current Detection Code (lines 13-18)

```python
try:
    # Check if __prepare__ is supported
    import sys
    _prepare_supported = hasattr(type, '__prepare__') or sys.implementation.name == 'micropython'
except:
    _prepare_supported = False
```

**Problem:** This always returns `True` on MicroPython, regardless of whether `MICROPY_PY_METACLASS_PREPARE` is enabled!

### Improved Detection

```python
def _check_prepare_support():
    """Check if __prepare__ is actually functional"""
    try:
        # Test if __prepare__ actually works
        class _TestMeta(type):
            _prepare_called = False
            @classmethod
            def __prepare__(mcs, name, bases):
                _TestMeta._prepare_called = True
                return {}

        class _Test(metaclass=_TestMeta):
            pass

        return _TestMeta._prepare_called
    except:
        return False

_prepare_supported = _check_prepare_support()
```

**This actually tests if __prepare__ is called**, not just whether MicroPython is detected.

---

## Impact Assessment

### With Recommended Fix (Option 1)

**Minimal builds (MICROPY_PY_METACLASS_PREPARE=0):**
- ✅ Enum, IntEnum, Flag, IntFlag, StrEnum: Work perfectly
- ✅ @unique: Works perfectly
- ❌ auto(): Raises clear error with solution

**Full builds (MICROPY_PY_METACLASS_PREPARE=1):**
- ✅ Everything works including auto()

### Without Fix (Current State)

**Minimal builds:**
- ✅ Enum, IntEnum, Flag, IntFlag, StrEnum: Work perfectly
- ✅ @unique: Works perfectly
- ❌ auto(): **Silently creates broken enum members** ← **BUG**

---

## Recommendation

**Implement Option 1 (Fail Fast) because:**

1. **Correctness over convenience**: Better to fail clearly than silently create broken enums
2. **Minimal code impact**: Just add one check, ~10 lines of code
3. **Clear user guidance**: Error message tells users exactly what to do
4. **No performance impact**: Only checked during class creation (not hot path)
5. **Consistent with MicroPython philosophy**: Features behind config flags

**Implementation:**
```python
def __new__(mcs, name, bases, namespace):
    # Check for auto() without __prepare__ support
    if not _prepare_supported:
        for key, value in namespace.items():
            if not key.startswith("_") and isinstance(value, auto):
                raise RuntimeError(
                    f"auto() requires MICROPY_PY_METACLASS_PREPARE to be enabled. "
                    f"Either enable the feature in py/mpconfig.h, or use explicit "
                    f"integer values instead of auto()."
                )

    # Rest of processing...
```

---

## Summary

| Aspect | Current State | With Fix |
|--------|--------------|----------|
| **Basic Enum features** | ✅ Work without C features | ✅ No change |
| **Flag/IntFlag** | ✅ Work without C features | ✅ No change |
| **auto()** | ❌ **Silent failure, broken members** | ✅ Clear error message |
| **Detection** | ⚠️ False positive on MicroPython | ✅ Accurate test |
| **User experience** | ❌ Confusing broken behavior | ✅ Clear guidance |

**Action Required:** Implement Option 1 fix to prevent silent failures with auto().
