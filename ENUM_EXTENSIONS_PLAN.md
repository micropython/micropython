# Enum Extensions Implementation Plan

## Overview

This document outlines the implementation plan for optional enum features beyond the core Enum and IntEnum classes. These features are designed to be conditionally compiled based on ROM size levels, allowing ports to balance functionality against code size constraints.

## Feature Summary

| Feature | Description | Size Estimate | ROM Level | CPython Version |
|---------|-------------|---------------|-----------|-----------------|
| Flag/IntFlag | Bitwise flag operations | ~200 bytes | EXTRA_FEATURES | 3.6+ |
| StrEnum | String-valued enums | ~50 bytes | EXTRA_FEATURES | 3.11+ |
| auto() | Automatic value generation | ~150 bytes | FULL_FEATURES | 3.6+ |
| @unique | Enforce no duplicate values | ~80 bytes | EXTRA_FEATURES | 3.4+ |
| **Metaclass `__init__`** | **Custom class initialization** | **0-100 bytes** | **CORE_FEATURES** | **all** |
| **Metaclass methods** | **Property/method lookup on classes** | **200-400 bytes** | **EXTRA_FEATURES** | **all** |

**Total size impact**: ~480 bytes for enum features, +0-400 bytes for metaclass features
**Note**: Metaclass features enable python-statemachine library compatibility

## Conditional Compilation Flags

### Configuration in py/mpconfig.h

```c
// ===== Metaclass Features (for python-statemachine support) =====

// Whether to support custom metaclass __init__ method invocation
// This allows metaclasses to initialize classes after creation
// Essential for python-statemachine's state/event registration
// Size impact: 0-100 bytes (may already be implemented)
#ifndef MICROPY_PY_METACLASS_INIT
#define MICROPY_PY_METACLASS_INIT (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to support metaclass method and property lookup on classes
// This enables @property and methods on metaclasses to be accessible on classes
// Critical for python-statemachine's .events and .states properties
// Size impact: ~200-400 bytes (descriptor protocol support)
#ifndef MICROPY_PY_METACLASS_PROPERTIES
#define MICROPY_PY_METACLASS_PROPERTIES (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// ===== Enum Extension Features =====

// Whether to support Flag and IntFlag enums for bitwise operations
// This enables combining enum members with | & ^ operators
// Requires MICROPY_PY_METACLASS_OPS
#ifndef MICROPY_PY_ENUM_FLAG
#define MICROPY_PY_ENUM_FLAG (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to support StrEnum for string-valued enums (Python 3.11+)
// Minimal overhead, just a thin wrapper around Enum
#ifndef MICROPY_PY_ENUM_STRENUM
#define MICROPY_PY_ENUM_STRENUM (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to support auto() for automatic value generation
// Requires __prepare__ support for OrderedDict tracking
#ifndef MICROPY_PY_ENUM_AUTO
#define MICROPY_PY_ENUM_AUTO (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_FULL_FEATURES)
#endif

// Whether to support @unique decorator for enforcing unique values
// Simple decorator, minimal overhead
#ifndef MICROPY_PY_ENUM_UNIQUE
#define MICROPY_PY_ENUM_UNIQUE (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif
```

### Dependencies

**Metaclass features (foundational):**
- **Metaclass __init__**: May require changes to class creation in py/objtype.c
- **Metaclass properties**: Requires descriptor protocol support in type_attr()

**Enum extensions:**
- **Flag/IntFlag**: Requires `MICROPY_PY_METACLASS_OPS` (already implemented)
- **auto()**: Requires `__prepare__` metaclass support (needs implementation)
- **StrEnum**: No special dependencies
- **@unique**: No special dependencies

**Measurement strategy:**
Each flag can be independently enabled/disabled to measure exact size impact. Build firmware multiple times with different combinations to isolate each feature's contribution.

## Development and Testing Guide

This section provides complete instructions for implementing and testing each feature. An agent on another computer can use these instructions to implement the features independently.

### Prerequisites

```bash
# Clone MicroPython repository
git clone https://github.com/micropython/micropython.git
cd micropython

# Build mpy-cross compiler (required for freezing modules)
cd mpy-cross
make -j
cd ..
```

### Unix Port Setup

The Unix port is ideal for development and testing because:
- Fast compilation (~10 seconds vs minutes for embedded)
- Easy debugging with gdb
- Full Python test suite available
- Can test without hardware

**Build Unix port with enum support:**

```bash
cd ports/unix
make submodules  # First time only
make -j

# Test that enum module is available
./build-standard/micropython
>>> import sys
>>> sys.path.insert(0, "../../lib/enum")
>>> from enum import Enum, IntEnum
>>> class Color(Enum):
...     RED = 1
...
>>> print(Color.RED)
Color.RED
>>> exit()
```

### Testing Existing Enum Implementation

**Run compatibility tests:**

```bash
# Test with MicroPython Unix port
cd ../../  # Back to repository root
ports/unix/build-standard/micropython test_cpython_compat.py

# Compare with CPython
python3 test_cpython_compat.py

# Both should produce identical output
```

**Run all enum tests:**

```bash
# Run basic enum tests
cd tests
../ports/unix/build-standard/micropython \
    -c "import sys; sys.path.insert(0, '../lib/enum')" \
    run-tests.py basics/class_*.py
```

### Implementing a New Feature (Step by Step)

This example shows how to implement Feature 5 (Metaclass `__init__`):

#### Step 1: Write the Test First

Create `tests/basics/class_metaclass_init.py`:

```python
# Test metaclass __init__ invocation
init_called = []

class Meta(type):
    def __init__(cls, name, bases, attrs):
        super().__init__(name, bases, attrs)
        init_called.append(name)

class Test(metaclass=Meta):
    pass

assert 'Test' in init_called
print("PASS")
```

**Run the test to see current behavior:**

```bash
cd tests
../ports/unix/build-standard/micropython basics/class_metaclass_init.py
```

If it fails, metaclass `__init__` is not currently supported.

#### Step 2: Implement C Code Changes

Edit `py/objtype.c` to add metaclass `__init__` support:

```c
// Find the type_call() function (around line 1200)
static mp_obj_t type_call(mp_obj_t self_in, size_t n_args, size_t n_kw,
                          const mp_obj_t *args) {
    mp_obj_type_t *self = MP_OBJ_TO_PTR(self_in);

    // ... existing __new__ handling code ...

#if MICROPY_PY_METACLASS_INIT
    // NEW CODE: Call metaclass.__init__ after instance creation
    const mp_obj_type_t *metaclass = self->base.type;
    if (metaclass != &mp_type_type) {
        // Look up __init__ on the metaclass
        mp_obj_t dest[2] = {MP_OBJ_NULL, MP_OBJ_NULL};
        mp_load_method_maybe(MP_OBJ_FROM_PTR(metaclass),
                             MP_QSTR___init__, dest);

        if (dest[0] != MP_OBJ_NULL) {
            // Call __init__(cls, ...)
            // For class creation, this is called by MAKE_FUNCTION with
            // (name, bases, attrs) arguments
            // This is already handled in mp_obj_new_type()
        }
    }
#endif

    return o;
}
```

Better location is in `mp_obj_new_type()` function:

```c
// Around line 1500 in py/objtype.c
mp_obj_t mp_obj_new_type(qstr name, mp_obj_t bases_tuple, mp_obj_t locals_dict) {
    // ... existing type creation code ...

    // NEW CODE: Call metaclass.__init__ if it exists
#if MICROPY_PY_METACLASS_INIT
    mp_obj_t init_fn[2] = {MP_OBJ_NULL, MP_OBJ_NULL};
    mp_load_method_maybe(MP_OBJ_FROM_PTR(metaclass), MP_QSTR___init__, init_fn);

    if (init_fn[0] != MP_OBJ_NULL) {
        // Prepare arguments: __init__(cls, name, bases, dict)
        mp_obj_t args[4] = {
            MP_OBJ_FROM_PTR(o),           // cls
            MP_OBJ_NEW_QSTR(name),        // name
            bases_tuple,                   // bases
            locals_dict                    // attrs dict
        };
        mp_call_method_n_kw(init_fn[0], 4, 0, args);
    }
#endif

    return MP_OBJ_FROM_PTR(o);
}
```

#### Step 3: Add Configuration Flag

Edit `py/mpconfig.h` to add the flag (already shown in Conditional Compilation Flags section above).

#### Step 4: Rebuild and Test

```bash
cd ports/unix
make clean
make -j CFLAGS_EXTRA="-DMICROPY_PY_METACLASS_INIT=1"

# Run the test again
cd ../../tests
../ports/unix/build-standard/micropython basics/class_metaclass_init.py

# Should now print "PASS"
```

#### Step 5: Run Full Test Suite

```bash
cd tests
./run-tests.py

# Verify no regressions
```

#### Step 6: Test on CPython for Compatibility

```bash
python3 basics/class_metaclass_init.py
# Should also print "PASS" - identical behavior
```

#### Step 7: Measure Size Impact

```bash
# Baseline build
cd ../ports/unix
make clean
make -j
size build-standard/micropython > /tmp/baseline_size.txt

# With feature enabled
make clean
make -j CFLAGS_EXTRA="-DMICROPY_PY_METACLASS_INIT=1"
size build-standard/micropython > /tmp/feature_size.txt

# Compare
diff /tmp/baseline_size.txt /tmp/feature_size.txt
```

### Unix Port with Frozen Enum Module

To test enum features with a frozen enum.py:

**Create manifest file:**

```bash
cat > ports/unix/variants/manifest_enum_test.py << 'EOF'
# Include standard Unix modules
include("$(PORT_DIR)/variants/standard/manifest.py")

# Freeze enum module
freeze("$(MPY_DIR)/lib/enum", "enum.py")
EOF
```

**Build with frozen module:**

```bash
cd ports/unix
make clean
make -j FROZEN_MANIFEST=variants/manifest_enum_test.py

# Test - no need for sys.path manipulation
./build-standard/micropython -c "from enum import Enum; print(Enum)"
```

### Complete Build Matrix for Testing

Test all feature combinations systematically:

```bash
#!/bin/bash
# test_all_features.sh

features=(
    "MICROPY_PY_METACLASS_INIT=0"
    "MICROPY_PY_METACLASS_INIT=1"
    "MICROPY_PY_METACLASS_PROPERTIES=0"
    "MICROPY_PY_METACLASS_PROPERTIES=1"
)

cd ports/unix

for flag in "${features[@]}"; do
    echo "Testing with $flag"
    make clean
    make -j CFLAGS_EXTRA="-D$flag"

    # Run test suite
    cd ../../tests
    if ../ports/unix/build-standard/micropython run-tests.py; then
        echo "✓ Tests passed with $flag"
    else
        echo "✗ Tests failed with $flag"
        exit 1
    fi
    cd ../ports/unix
done

echo "All feature combinations passed!"
```

### Debugging C Code Changes

**Use gdb for debugging:**

```bash
cd ports/unix
make -j DEBUG=1

# Run with gdb
gdb --args ./build-standard/micropython -c "class M(type): pass; class C(metaclass=M): pass"

# Set breakpoints
(gdb) break type_call
(gdb) break mp_obj_new_type
(gdb) run

# Step through code
(gdb) next
(gdb) print metaclass->name
(gdb) continue
```

**Add debug printing:**

```c
// In py/objtype.c
#if MICROPY_PY_METACLASS_INIT
    mp_printf(&mp_plat_print, "DEBUG: Calling metaclass.__init__\n");
    mp_obj_print_helper(&mp_plat_print, MP_OBJ_FROM_PTR(metaclass), PRINT_REPR);
    mp_printf(&mp_plat_print, "\n");
#endif
```

### Cross-Port Testing

After Unix port works, test on embedded hardware:

**STM32 Port:**

```bash
cd ports/stm32
make -j BOARD=PYBV10 CFLAGS_EXTRA="-DMICROPY_PY_METACLASS_INIT=1"

# Flash to board
make -j BOARD=PYBV10 deploy-stlink

# Test via serial
mpremote connect /dev/ttyACM0 exec "import sys; sys.path.insert(0, '/flash'); from enum import Enum; class C(Enum): RED=1; print(C.RED)"
```

### Verifying Feature Implementation

**Checklist for each feature:**

1. ✓ Test file created in tests/basics/
2. ✓ Test passes on CPython
3. ✓ Test fails on MicroPython without changes (if new feature)
4. ✓ C code changes implemented with #if guards
5. ✓ Test passes on MicroPython with changes
6. ✓ No regressions in test suite
7. ✓ Size impact measured and documented
8. ✓ Feature can be disabled via config flag
9. ✓ Works on Unix port
10. ✓ Works on at least one embedded port (STM32)

### Common Issues and Solutions

**Issue: Test passes but size unchanged**
- Compiler may optimize out unused code
- Solution: Actually call the feature in the test

**Issue: Segfault when running test**
- Missing NULL checks in C code
- Solution: Use gdb to find exact crash location

**Issue: Test works on Unix but fails on STM32**
- Memory alignment issues or stack overflow
- Solution: Reduce memory usage or increase stack size

**Issue: Feature works but breaks other tests**
- Unintended side effects in attribute lookup
- Solution: Add more specific conditions to new code paths

## Feature 1: Flag and IntFlag

### CPython Compatibility

```python
from enum import Flag, IntFlag, auto

class Permission(Flag):
    READ = 1
    WRITE = 2
    EXECUTE = 4

# Bitwise operations
Permission.READ | Permission.WRITE  # Permission.READ|WRITE (value: 3)
Permission.READ & Permission.WRITE  # <Permission: 0>
~Permission.READ                    # Permission.WRITE|EXECUTE (value: 6)

class Status(IntFlag):
    READY = 1
    WAITING = 2
    DONE = 4

# IntFlag works with integers
Status.READY | 8  # Status.READY|8 (value: 9)
```

### Implementation Details

**In enum.py (conditionally compiled):**

```python
#if MICROPY_PY_ENUM_FLAG

class FlagMeta(EnumMeta):
    """Metaclass for Flag enums"""

    def __new__(mcs, name, bases, namespace):
        cls = EnumMeta.__new__(mcs, name, bases, namespace)

        # Generate combination members for all possible flag combinations
        if bases and any(isinstance(b, FlagMeta) for b in bases):
            cls._generate_combinations()

        return cls

    def _generate_combinations(cls):
        """Create pseudo-members for flag combinations"""
        # This is optional optimization for CPython compatibility
        # Can be simplified for MicroPython
        pass


class Flag(Enum, metaclass=FlagMeta):
    """Base class for flag enums (bitwise operations)"""

    def __or__(self, other):
        if isinstance(other, self.__class__):
            return self.__class__(self._value_ | other._value_)
        return NotImplemented

    def __and__(self, other):
        if isinstance(other, self.__class__):
            return self.__class__(self._value_ & other._value_)
        return NotImplemented

    def __xor__(self, other):
        if isinstance(other, self.__class__):
            return self.__class__(self._value_ ^ other._value_)
        return NotImplemented

    def __invert__(self):
        # Calculate all possible values
        all_bits = 0
        for member in self.__class__:
            all_bits |= member._value_
        return self.__class__(all_bits & ~self._value_)

    def __repr__(self):
        # Show combined flags as FLAG1|FLAG2
        if self._value_ == 0:
            return f"<{self.__class__.__name__}: 0>"

        # Find which flags are set
        flags = []
        for member in self.__class__._member_map_.values():
            if member._value_ and (self._value_ & member._value_) == member._value_:
                flags.append(member._name_)

        if flags:
            return f"<{self.__class__.__name__}.{('|'.join(flags))}: {self._value_}>"
        return f"<{self.__class__.__name__}: {self._value_}>"


class IntFlag(int, Flag, metaclass=FlagMeta):
    """Flag enum where members are also integers"""

    def __new__(cls, value):
        obj = object.__new__(cls)
        obj._value_ = value
        return obj

    def __or__(self, other):
        if isinstance(other, self.__class__):
            return self.__class__(self._value_ | other._value_)
        if isinstance(other, int):
            return self.__class__(self._value_ | other)
        return NotImplemented

    def __and__(self, other):
        if isinstance(other, self.__class__):
            return self.__class__(self._value_ & other._value_)
        if isinstance(other, int):
            return self.__class__(self._value_ & other)
        return NotImplemented

    # Include __ror__, __rand__ for right-side operations
    __ror__ = __or__
    __rand__ = __and__

#endif  // MICROPY_PY_ENUM_FLAG
```

### Size Impact

- **Core implementation**: ~180 bytes (Flag/IntFlag classes, operators)
- **Metaclass support**: ~20 bytes (FlagMeta)
- **Total**: ~200 bytes

### Testing

```python
# test_enum_flag.py
from enum import Flag, IntFlag

class Color(Flag):
    RED = 1
    GREEN = 2
    BLUE = 4

# Test bitwise OR
assert (Color.RED | Color.GREEN)._value_ == 3

# Test bitwise AND
assert (Color.RED & Color.GREEN)._value_ == 0
assert (Color.RED & Color.RED) is Color.RED

# Test invert
inverted = ~Color.RED
assert inverted._value_ == 6  # GREEN | BLUE

# Test IntFlag with integers
class Status(IntFlag):
    READY = 1
    DONE = 2

assert (Status.READY | 4)._value_ == 5
```

## Feature 2: StrEnum

### CPython Compatibility

```python
from enum import StrEnum

class Color(StrEnum):
    RED = "red"
    GREEN = "green"
    BLUE = "blue"

# String operations
Color.RED.upper()     # "RED"
Color.RED + "_color"  # "red_color"
Color.RED == "red"    # True
```

### Implementation Details

**In enum.py (conditionally compiled):**

```python
#if MICROPY_PY_ENUM_STRENUM

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
    # No need to override arithmetic/comparison operators

#endif  // MICROPY_PY_ENUM_STRENUM
```

### Size Impact

- **Core implementation**: ~50 bytes
- Very lightweight, just a thin wrapper

### Testing

```python
# test_enum_strenum.py
from enum import StrEnum

class Color(StrEnum):
    RED = "red"
    GREEN = "green"

# Test string operations
assert Color.RED.upper() == "RED"
assert Color.RED + "_color" == "red_color"
assert Color.RED == "red"
assert str(Color.RED) == "red"

# Test enum properties still work
assert Color.RED.name == "RED"
assert Color("red") is Color.RED
```

## Feature 3: auto()

### CPython Compatibility

```python
from enum import Enum, auto

class Color(Enum):
    RED = auto()    # 1
    GREEN = auto()  # 2
    BLUE = auto()   # 3

class Status(Enum):
    PENDING = auto()  # 1
    ACTIVE = 10
    DONE = auto()     # 11 (continues from last value)
```

### Implementation Details

This is the most complex feature as it requires `__prepare__` support.

**In enum.py (conditionally compiled):**

```python
#if MICROPY_PY_ENUM_AUTO

class _AutoValue:
    """Sentinel for automatic value generation"""
    _counter = 0

    def __init__(self):
        _AutoValue._counter += 1
        self.value = _AutoValue._counter


def auto():
    """Generate automatic values for enum members"""
    return _AutoValue()


class EnumMeta(type):
    """Enhanced metaclass with auto() support"""

    @classmethod
    def __prepare__(mcs, name, bases):
        """Return an ordered namespace for tracking member order"""
        # Need to track insertion order for auto()
        return {}  # In MicroPython, dicts are already ordered

    def __new__(mcs, name, bases, namespace):
        # Process auto() values
        auto_counter = 0
        for key, value in list(namespace.items()):
            if isinstance(value, _AutoValue):
                auto_counter += 1
                namespace[key] = auto_counter
            elif not key.startswith('_') and not callable(value):
                # Update auto counter based on manual values
                if isinstance(value, int) and value > auto_counter:
                    auto_counter = value

        # Reset auto counter for next enum
        _AutoValue._counter = 0

        # Continue with normal enum creation
        return type.__new__(mcs, name, bases, namespace)

#endif  // MICROPY_PY_ENUM_AUTO
```

### C Support Required

**In py/objtype.c:**

```c
#if MICROPY_PY_ENUM_AUTO

// Add __prepare__ support to type_call
// This allows metaclasses to return a custom namespace dict
// for class creation (needed for auto() to track insertion order)

static mp_obj_t type_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // ... existing code ...

    // Check if metaclass has __prepare__
    mp_obj_t prepare[2] = {MP_OBJ_NULL};
    mp_load_method_maybe(metaclass, MP_QSTR___prepare__, prepare);

    if (prepare[0] != MP_OBJ_NULL) {
        // Call __prepare__(name, bases) to get namespace
        mp_obj_t namespace = mp_call_method_n_kw(2, 0, prepare, name, bases);
        // Use this namespace instead of empty dict
    }

    // ... continue with normal type creation ...
}

#endif  // MICROPY_PY_ENUM_AUTO
```

### Size Impact

- **Python code**: ~100 bytes (auto() function, _AutoValue class)
- **C code**: ~50 bytes (__prepare__ support in objtype.c)
- **Total**: ~150 bytes

### Testing

```python
# test_enum_auto.py
from enum import Enum, auto

class Color(Enum):
    RED = auto()
    GREEN = auto()
    BLUE = auto()

assert Color.RED.value == 1
assert Color.GREEN.value == 2
assert Color.BLUE.value == 3

# Test with mixed auto/manual values
class Status(Enum):
    PENDING = auto()  # 1
    ACTIVE = 10
    DONE = auto()     # 11

assert Status.PENDING.value == 1
assert Status.ACTIVE.value == 10
assert Status.DONE.value == 11
```

## Feature 4: @unique Decorator

### CPython Compatibility

```python
from enum import Enum, unique

@unique
class Color(Enum):
    RED = 1
    GREEN = 2
    CRIMSON = 1  # ValueError: duplicate values found in Color: RED/CRIMSON
```

### Implementation Details

**In enum.py (conditionally compiled):**

```python
#if MICROPY_PY_ENUM_UNIQUE

def unique(enumeration):
    """
    Class decorator that ensures only one name is bound to each value.
    Raises ValueError if any aliases are found.
    """
    seen = {}
    duplicates = []

    for name, member in enumeration._member_map_.items():
        value = member._value_
        if value in seen:
            duplicates.append((seen[value], name))
        else:
            seen[value] = name

    if duplicates:
        alias_details = ', '.join(f"{first}/{second}" for first, second in duplicates)
        raise ValueError(f"duplicate values found in {enumeration.__name__}: {alias_details}")

    return enumeration

#endif  // MICROPY_PY_ENUM_UNIQUE
```

### Size Impact

- **Implementation**: ~80 bytes
- Simple decorator function, minimal overhead

### Testing

```python
# test_enum_unique.py
from enum import Enum, unique

# Should work fine
@unique
class Color(Enum):
    RED = 1
    GREEN = 2
    BLUE = 3

# Should raise ValueError
try:
    @unique
    class Status(Enum):
        PENDING = 1
        WAITING = 1  # Duplicate!
    assert False, "Should have raised ValueError"
except ValueError as e:
    assert "duplicate values" in str(e)
```

---

## Feature 5: Custom Metaclass `__init__`

### CPython Compatibility

In CPython, when a class is created with a custom metaclass, the metaclass `__init__` method is called after `__new__` to initialize the newly created class. This allows metaclasses to perform additional setup, validation, or registration.

**python-statemachine use case:**
```python
class StateMachineMeta(type):
    def __init__(cls, name, bases, attrs):
        super().__init__(name, bases, attrs)
        # Initialize state machine structure
        cls._states = []
        cls._events = []
        # Process state and event definitions from attrs
        for key, value in attrs.items():
            if isinstance(value, State):
                cls._states.append(value)
                value.name = key
            elif isinstance(value, Event):
                cls._events.append(value)
                value.name = key

class MyStateMachine(metaclass=StateMachineMeta):
    # After class creation, __init__ is called to set up _states and _events
    start = State(initial=True)
    end = State(final=True)
    proceed = Event(start, end)
```

**Standard metaclass pattern:**
```python
class RegistryMeta(type):
    _registry = []

    def __init__(cls, name, bases, attrs):
        super().__init__(name, bases, attrs)
        # Register the class
        if name != 'Base':  # Skip base class
            RegistryMeta._registry.append(cls)

class Base(metaclass=RegistryMeta):
    pass

class Widget(Base):
    pass

class Gadget(Base):
    pass

# After both classes are created, RegistryMeta._registry = [Widget, Gadget]
```

### Implementation Details

**Current status:** Likely already implemented in MicroPython's existing metaclass support.

**C code location:** `py/objtype.c` in `mp_obj_type_t` creation path

**Key implementation points:**

1. **Class creation sequence in `mp_obj_new_type()`:**
   ```c
   // Simplified version of what happens:
   mp_obj_type_t *o = mp_obj_new_type(...) {
       // 1. Allocate type object
       mp_obj_type_t *o = m_new0(mp_obj_type_t, 1);

       // 2. Set base.type to metaclass
       o->base.type = metaclass;

       // 3. Call metaclass.__new__() - creates class
       mp_obj_t new_class = metaclass_new(...);

       // 4. Call metaclass.__init__() - initializes class
       // THIS IS THE KEY: need to verify this happens
       mp_obj_t init_fn = mp_load_method(metaclass, MP_QSTR___init__);
       mp_call_method_n_kw(init_fn, 4, 0, name, bases, dict);

       return new_class;
   }
   ```

2. **Verification points:**
   - Check if `type_call()` in py/objtype.c calls both `__new__` and `__init__`
   - Trace through `mp_obj_new_type()` to see if `__init__` is invoked
   - Look for `MP_QSTR___init__` lookup after class creation

3. **Expected behavior:**
   - `metaclass.__new__()` creates the class object
   - `metaclass.__init__()` receives the newly created class and initializes it
   - Signature: `__init__(cls, name, bases, attrs)`

**Investigation needed:**
```python
# Test if metaclass __init__ is called
calls = []

class TrackerMeta(type):
    def __new__(mcs, name, bases, attrs):
        calls.append(('__new__', name))
        return super().__new__(mcs, name, bases, attrs)

    def __init__(cls, name, bases, attrs):
        calls.append(('__init__', name))
        super().__init__(name, bases, attrs)

class TestClass(metaclass=TrackerMeta):
    pass

print(calls)
# Expected: [('__new__', 'TestClass'), ('__init__', 'TestClass')]
# If __init__ not called: [('__new__', 'TestClass')]
```

### Size Impact

**Estimated code size:** 0-100 bytes

- **If already implemented:** 0 bytes (no changes needed)
- **If needs implementation:** ~50-100 bytes for `__init__` lookup and call

**Configuration flag:** `MICROPY_PY_METACLASS_INIT` (defined in Conditional Compilation Flags section)

**Conditional compilation in py/objtype.c:**
```c
// In type_call() or mp_obj_new_type(), after calling __new__:
#if MICROPY_PY_METACLASS_INIT
    // Look up and call metaclass.__init__
    mp_obj_t dest[2] = {MP_OBJ_NULL, MP_OBJ_NULL};
    mp_load_method(metaclass, MP_QSTR___init__, dest);
    if (dest[0] != MP_OBJ_NULL) {
        // Call __init__(cls, name, bases, attrs)
        mp_call_method_n_kw(dest[0], 4, 0, cls, name, bases, attrs);
    }
#endif
```

**Size measurement:**
```bash
# Build without metaclass __init__
make -j BOARD=PYBV10 CFLAGS_EXTRA="-DMICROPY_PY_METACLASS_INIT=0"

# Build with metaclass __init__ (default)
make -j BOARD=PYBV10

# Compare sizes to measure impact
```

### Testing

**Test file:** `tests/basics/class_metaclass_init.py`

```python
# Test 1: Basic metaclass __init__ call
init_called = []

class Meta(type):
    def __init__(cls, name, bases, attrs):
        super().__init__(name, bases, attrs)
        init_called.append(name)

class Test(metaclass=Meta):
    pass

assert 'Test' in init_called
print("Test 1: PASS - metaclass __init__ called")

# Test 2: Metaclass __init__ can modify class
class CounterMeta(type):
    def __init__(cls, name, bases, attrs):
        super().__init__(name, bases, attrs)
        cls.instance_count = 0

class Widget(metaclass=CounterMeta):
    def __init__(self):
        Widget.instance_count += 1

assert hasattr(Widget, 'instance_count')
assert Widget.instance_count == 0
w1 = Widget()
assert Widget.instance_count == 1
w2 = Widget()
assert Widget.instance_count == 2
print("Test 2: PASS - metaclass __init__ can modify class")

# Test 3: Arguments passed correctly
received_args = []

class ArgTrackerMeta(type):
    def __init__(cls, name, bases, attrs):
        super().__init__(name, bases, attrs)
        received_args.append({
            'name': name,
            'bases': bases,
            'has_method': 'foo' in attrs
        })

class TestClass(ArgTrackerMeta):
    def foo(self):
        pass

assert len(received_args) == 1
assert received_args[0]['name'] == 'TestClass'
assert received_args[0]['has_method'] == True
print("Test 3: PASS - correct arguments passed to __init__")

print("\nAll metaclass __init__ tests passed!")
```

### Priority

**Level:** HIGH (required for python-statemachine)

**Justification:** Essential for state machine class setup and registration patterns.

---

## Feature 6: Metaclass Method/Property Lookup

### CPython Compatibility

In CPython, methods and properties defined on a metaclass are accessible on classes themselves. This allows metaclasses to provide a class-level API. When accessing an attribute on a class, Python first looks in the class dict, then in the metaclass.

**python-statemachine use case:**
```python
class StateMachineMeta(type):
    @property
    def events(cls):
        """Return list of events defined on this state machine."""
        return [e for e in cls.__dict__.values() if isinstance(e, Event)]

    @property
    def states(cls):
        """Return list of states defined on this state machine."""
        return [s for s in cls.__dict__.values() if isinstance(s, State)]

    def send_event(cls, event_name):
        """Class-level method to send event."""
        event = getattr(cls, event_name)
        return event.trigger()

class TrafficLight(metaclass=StateMachineMeta):
    green = State(initial=True)
    yellow = State()
    red = State()

    slow_down = Event(green, yellow)
    stop = Event(yellow, red)
    go = Event(red, green)

# Access metaclass properties on the class
print(TrafficLight.events)   # Calls StateMachineMeta.events.__get__(TrafficLight, type)
print(TrafficLight.states)   # Calls StateMachineMeta.states.__get__(TrafficLight, type)

# Call metaclass method on the class
TrafficLight.send_event('slow_down')  # Calls StateMachineMeta.send_event(TrafficLight, 'slow_down')
```

**Standard metaclass property pattern:**
```python
class Meta(type):
    @property
    def is_abstract(cls):
        """Check if class is abstract."""
        return hasattr(cls, '__abstract__') and cls.__abstract__

    def subclasses(cls):
        """Return all subclasses."""
        return cls.__subclasses__()

class Base(metaclass=Meta):
    __abstract__ = True

class Concrete(Base):
    pass

# Access via class (not instance)
print(Base.is_abstract)      # True (property access)
print(Concrete.is_abstract)  # False
print(Base.subclasses())     # [Concrete] (method call)
```

### Implementation Details

**Current status:** Partially implemented. MicroPython supports metaclass method lookup but may have limitations with descriptor protocol for `@property`.

**C code location:** `py/objtype.c` in `type_attr()` function

**Key implementation points:**

1. **Attribute lookup path in `type_attr()`:**
   ```c
   static void type_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
       mp_obj_type_t *self = MP_OBJ_TO_PTR(self_in);

       // 1. Look up attribute in the class itself
       struct class_lookup_data lookup = {
           .obj = NULL,
           .attr = attr,
           .slot_offset = 0,
           .dest = dest,
           .is_type = true,
       };
       mp_obj_class_lookup(&lookup, self);

       if (dest[0] != MP_OBJ_NULL) {
           return;  // Found in class
       }

       // 2. Look up in metaclass
       // THIS IS THE KEY: need to check metaclass for methods/properties
       mp_obj_type_t *metaclass = self->base.type;
       if (metaclass != &mp_type_type) {
           mp_obj_class_lookup(&lookup, metaclass);

           if (dest[0] != MP_OBJ_NULL) {
               // Found in metaclass
               // If it's a property descriptor, need to call __get__
               if (mp_obj_is_type(dest[0], &mp_type_property)) {
                   // Call property.__get__(self_in, type(self_in))
                   const mp_obj_t *proxy = mp_obj_property_get(dest[0]);
                   if (proxy[0] != mp_const_none) {
                       dest[0] = mp_call_function_2(proxy[0], self_in,
                                                      MP_OBJ_FROM_PTR(&mp_type_type));
                       dest[1] = MP_OBJ_NULL;  // No setter
                   }
               }
               // If it's a regular method, bind it to the class
               else if (mp_obj_is_callable(dest[0])) {
                   dest[1] = self_in;  // self = the class
               }
               return;
           }
       }
   }
   ```

2. **Descriptor protocol for @property:**
   - Properties use the descriptor protocol: `__get__(self, obj, objtype)`
   - When accessing `TrafficLight.events`, should call `events.__get__(TrafficLight, type)`
   - MicroPython may need enhancement to invoke `__get__` on properties found in metaclass

3. **Method binding:**
   - Methods found in metaclass should be bound with class as first argument
   - `TrafficLight.send_event('x')` becomes `send_event(TrafficLight, 'x')`

**Investigation needed:**
```python
# Test if metaclass properties work
class Meta(type):
    @property
    def test_property(cls):
        return f"Property on {cls.__name__}"

    def test_method(cls):
        return f"Method on {cls.__name__}"

class TestClass(metaclass=Meta):
    pass

# Check property access
try:
    result = TestClass.test_property
    print(f"Property access: {result}")
except Exception as e:
    print(f"Property access failed: {e}")

# Check method access
try:
    result = TestClass.test_method()
    print(f"Method access: {result}")
except Exception as e:
    print(f"Method access failed: {e}")
```

### Size Impact

**Estimated code size:** 200-400 bytes

- Property descriptor handling: ~150-200 bytes
- Enhanced metaclass lookup: ~50-100 bytes
- Method binding logic: ~50-100 bytes

**Configuration flag:** `MICROPY_PY_METACLASS_PROPERTIES` (defined in Conditional Compilation Flags section)

**Conditional compilation in py/objtype.c (type_attr function):**
```c
static void type_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    mp_obj_type_t *self = MP_OBJ_TO_PTR(self_in);

    // ... standard class attribute lookup ...

#if MICROPY_PY_METACLASS_PROPERTIES
    // Enhanced metaclass attribute lookup with descriptor support
    mp_obj_type_t *metaclass = self->base.type;
    if (metaclass != &mp_type_type) {
        struct class_lookup_data lookup = {
            .obj = NULL,
            .attr = attr,
            .slot_offset = 0,
            .dest = dest,
            .is_type = true,
        };
        mp_obj_class_lookup(&lookup, metaclass);

        if (dest[0] != MP_OBJ_NULL) {
            // Handle property descriptors
            if (mp_obj_is_type(dest[0], &mp_type_property)) {
                const mp_obj_t *proxy = mp_obj_property_get(dest[0]);
                if (proxy[0] != mp_const_none) {
                    // Invoke property.__get__(cls, type)
                    dest[0] = mp_call_function_2(proxy[0], self_in,
                                                  MP_OBJ_FROM_PTR(&mp_type_type));
                    dest[1] = MP_OBJ_NULL;
                }
            }
            // Handle method binding
            else if (mp_obj_is_callable(dest[0])) {
                dest[1] = self_in;  // Bind class as first argument
            }
            return;
        }
    }
#endif
}
```

**Size measurement:**
```bash
# Build without metaclass properties
make -j BOARD=PYBV10 CFLAGS_EXTRA="-DMICROPY_PY_METACLASS_PROPERTIES=0"

# Build with metaclass properties (default for EXTRA_FEATURES)
make -j BOARD=PYBV10

# Compare sizes to measure impact
arm-none-eabi-size build-PYBV10/firmware.elf
```

### Testing

**Test file:** `tests/basics/class_metaclass_property.py`

```python
# Test 1: Metaclass property access
class Meta(type):
    @property
    def class_id(cls):
        return f"{cls.__name__}_id"

class Widget(metaclass=Meta):
    pass

assert Widget.class_id == "Widget_id"
print("Test 1: PASS - metaclass property access")

# Test 2: Metaclass method access
class Meta2(type):
    def get_name(cls):
        return cls.__name__

    def combine(cls, *parts):
        return f"{cls.__name__}:{':'.join(parts)}"

class Gadget(metaclass=Meta2):
    pass

assert Gadget.get_name() == "Gadget"
assert Gadget.combine('a', 'b', 'c') == "Gadget:a:b:c"
print("Test 2: PASS - metaclass method access")

# Test 3: Property with computed value
class CounterMeta(type):
    _instance_counts = {}

    @property
    def instance_count(cls):
        return CounterMeta._instance_counts.get(cls.__name__, 0)

    def __call__(cls, *args, **kwargs):
        CounterMeta._instance_counts[cls.__name__] = \
            CounterMeta._instance_counts.get(cls.__name__, 0) + 1
        return super().__call__(*args, **kwargs)

class Thing(metaclass=CounterMeta):
    pass

assert Thing.instance_count == 0
t1 = Thing()
assert Thing.instance_count == 1
t2 = Thing()
assert Thing.instance_count == 2
print("Test 3: PASS - property with computed value")

# Test 4: Multiple metaclass levels
class BaseMeta(type):
    @property
    def level(cls):
        return "base"

class DerivedMeta(BaseMeta):
    @property
    def derived_level(cls):
        return "derived"

class MyClass(metaclass=DerivedMeta):
    pass

assert MyClass.level == "base"
assert MyClass.derived_level == "derived"
print("Test 4: PASS - multiple metaclass levels")

print("\nAll metaclass property tests passed!")
```

**Expected output file:** `tests/basics/class_metaclass_property.py.exp`
```
Test 1: PASS - metaclass property access
Test 2: PASS - metaclass method access
Test 3: PASS - property with computed value
Test 4: PASS - multiple metaclass levels

All metaclass property tests passed!
```

### Priority

**Level:** HIGH (required for python-statemachine)

**Justification:** Critical for state machine introspection API (.events, .states properties).

### Workaround if Not Implemented

If metaclass properties don't work, python-statemachine can use classmethods:

```python
class StateMachineMeta(type):
    # Instead of @property, use regular methods
    def get_events(cls):
        return [e for e in cls.__dict__.values() if isinstance(e, Event)]

    def get_states(cls):
        return [s for s in cls.__dict__.values() if isinstance(s, State)]

class MyStateMachine(metaclass=StateMachineMeta):
    pass

# Access via method calls instead of property access
events = MyStateMachine.get_events()  # Instead of .events
states = MyStateMachine.get_states()  # Instead of .states
```

However, this breaks CPython compatibility for the library API.

---

## Implementation Strategy

### Phase 0: Custom Metaclass `__init__` (Foundational)

1. Write test to verify if metaclass `__init__` is currently working
2. If not working, add implementation in py/objtype.c
3. Ensure `__init__` is called after `__new__` during class creation
4. Add test cases covering initialization patterns
5. Measure any size impact
6. Document behavior

**Estimated effort**: 2-4 hours (0 hours if already working)

### Phase 1: Metaclass Method/Property Lookup (Foundational)

1. Write test to verify if metaclass properties currently work
2. If partial, enhance type_attr() in py/objtype.c for descriptor support
3. Add `MICROPY_PY_METACLASS_PROPERTIES` configuration flag
4. Implement property.__get__() invocation for metaclass properties
5. Ensure method binding works for metaclass methods
6. Write comprehensive tests
7. Measure size impact on STM32
8. Update documentation

**Estimated effort**: 6-10 hours (2-4 hours if mostly working)

### Phase 2: Flag/IntFlag (Highest Priority)

1. Add `MICROPY_PY_ENUM_FLAG` to mpconfig.h
2. Implement Flag and IntFlag classes in enum.py
3. Add conditional compilation guards
4. Write comprehensive tests
5. Measure size impact on STM32
6. Update documentation

**Estimated effort**: 4-6 hours

### Phase 3: StrEnum (Low Complexity)

1. Add `MICROPY_PY_ENUM_STRENUM` to mpconfig.h
2. Implement StrEnum class in enum.py
3. Write tests
4. Measure size impact
5. Update documentation

**Estimated effort**: 2-3 hours

### Phase 4: @unique Decorator (Low Complexity)

1. Add `MICROPY_PY_ENUM_UNIQUE` to mpconfig.h
2. Implement unique() decorator in enum.py
3. Write tests
4. Update documentation

**Estimated effort**: 2-3 hours

### Phase 5: auto() (Highest Complexity)

1. Add `MICROPY_PY_ENUM_AUTO` to mpconfig.h
2. Implement __prepare__ support in py/objtype.c
3. Implement auto() in enum.py with _AutoValue
4. Write comprehensive tests
5. Measure size impact
6. Update documentation

**Estimated effort**: 8-12 hours (due to C changes)

## Size Budget Analysis

### Baseline

Current implementation: 370,812 bytes (STM32 PYBV10)

### With All Extensions

| Configuration | Size | Change | % Change |
|---------------|------|--------|----------|
| Baseline + enum.py | 370,812 | - | - |
| + Metaclass __init__ | 370,812-370,912 | 0-100 | 0-0.027% |
| + Metaclass properties | 371,012-371,212 | 200-400 | +0.054-0.108% |
| + Flag/IntFlag | 371,212-371,412 | 400-600 | +0.108-0.162% |
| + StrEnum | 371,262-371,462 | 450-650 | +0.122-0.175% |
| + @unique | 371,342-371,542 | 530-730 | +0.143-0.197% |
| + auto() | 371,492-371,692 | 680-880 | +0.184-0.237% |

**Total overhead**: 680-880 bytes (0.18-0.24% increase over baseline)
**python-statemachine overhead**: 200-500 bytes (metaclass features only)

### Recommendations by ROM Level

| ROM Level | Recommended Features | Total Size |
|-----------|---------------------|------------|
| CORE_FEATURES | Enum, IntEnum + Metaclass __init__ | +2,536-2,636 bytes |
| EXTRA_FEATURES | + Metaclass properties + Flag + StrEnum + @unique | +2,936-3,186 bytes |
| FULL_FEATURES | + auto() | +3,216-3,416 bytes |

**For python-statemachine support specifically:**
- Minimum: CORE_FEATURES + metaclass __init__ (if not already working)
- Recommended: EXTRA_FEATURES for full API compatibility

## Testing Plan

### Unit Tests

**Metaclass Features:**
- `tests/basics/class_metaclass_init.py`: Metaclass __init__ invocation
- `tests/basics/class_metaclass_property.py`: Property and method lookup on classes

**Enum Extensions:**
- `test_enum_flag.py`: Flag/IntFlag bitwise operations
- `test_enum_strenum.py`: String enum operations
- `test_enum_unique.py`: Duplicate detection
- `test_enum_auto.py`: Automatic value generation
- `test_enum_combined.py`: Features working together

### CPython Compatibility Tests

Run all tests on both CPython 3.11+ and MicroPython to verify identical behavior.

### Size Regression Tests

Build STM32 firmware with each feature enabled/disabled and verify size impact matches estimates.

## Size Measurement Methodology

To accurately measure the size impact of each feature independently:

### 1. Baseline Build

```bash
cd ports/stm32
make BOARD=PYBV10 clean
make -j BOARD=PYBV10 FROZEN_MANIFEST=boards/manifest_enum.py
arm-none-eabi-size build-PYBV10/firmware.elf
# Record text size as BASELINE
```

### 2. Measure Each Metaclass Feature

**Metaclass __init__ (if not already working):**
```bash
make BOARD=PYBV10 clean
make -j BOARD=PYBV10 FROZEN_MANIFEST=boards/manifest_enum.py \
     CFLAGS_EXTRA="-DMICROPY_PY_METACLASS_INIT=1"
arm-none-eabi-size build-PYBV10/firmware.elf
# text_size - BASELINE = metaclass __init__ overhead
```

**Metaclass properties:**
```bash
make BOARD=PYBV10 clean
make -j BOARD=PYBV10 FROZEN_MANIFEST=boards/manifest_enum.py \
     CFLAGS_EXTRA="-DMICROPY_PY_METACLASS_PROPERTIES=1"
arm-none-eabi-size build-PYBV10/firmware.elf
# text_size - BASELINE = metaclass properties overhead
```

### 3. Measure Enum Extensions (in enum.py)

Each enum extension flag controls conditional Python code in lib/enum/enum.py:

```python
# In lib/enum/enum.py
import sys

# Check if Flag/IntFlag enabled
if hasattr(sys.implementation, '_mpy') and \
   sys.implementation._mpy & (1 << 15):  # MICROPY_PY_ENUM_FLAG bit
    class Flag(Enum):
        # ... Flag implementation ...
```

Size is measured by freezing enum.py with different feature sets enabled.

### 4. Combined Build Matrix

Build with all combinations to verify independence:

| Build | Flags | Purpose |
|-------|-------|---------|
| 1 | All disabled | Baseline |
| 2 | +METACLASS_INIT | Isolate __init__ |
| 3 | +METACLASS_PROPERTIES | Isolate properties |
| 4 | +ENUM_FLAG | Isolate Flag |
| 5 | +ENUM_STRENUM | Isolate StrEnum |
| 6 | +ENUM_UNIQUE | Isolate @unique |
| 7 | +ENUM_AUTO | Isolate auto() |
| 8 | All enabled | Full overhead |

### 5. Automated Measurement Script

```bash
#!/bin/bash
# measure_sizes.sh

BOARD=PYBV10
PORT_DIR=ports/stm32
BASELINE=0

declare -A features=(
    ["metaclass_init"]="-DMICROPY_PY_METACLASS_INIT=1"
    ["metaclass_properties"]="-DMICROPY_PY_METACLASS_PROPERTIES=1"
    ["enum_flag"]="-DMICROPY_PY_ENUM_FLAG=1"
    ["enum_strenum"]="-DMICROPY_PY_ENUM_STRENUM=1"
    ["enum_unique"]="-DMICROPY_PY_ENUM_UNIQUE=1"
    ["enum_auto"]="-DMICROPY_PY_ENUM_AUTO=1"
)

# Build baseline
cd $PORT_DIR
make BOARD=$BOARD clean
make -j BOARD=$BOARD FROZEN_MANIFEST=boards/manifest_enum.py
BASELINE=$(arm-none-eabi-size build-$BOARD/firmware.elf | awk 'NR==2 {print $1}')
echo "Baseline: $BASELINE bytes"

# Measure each feature
for feature in "${!features[@]}"; do
    make BOARD=$BOARD clean
    make -j BOARD=$BOARD FROZEN_MANIFEST=boards/manifest_enum.py \
         CFLAGS_EXTRA="${features[$feature]}"
    SIZE=$(arm-none-eabi-size build-$BOARD/firmware.elf | awk 'NR==2 {print $1}')
    DELTA=$((SIZE - BASELINE))
    echo "$feature: +$DELTA bytes"
done
```

### 6. Python-only Size Measurement

For features implemented purely in Python (Flag, StrEnum, auto(), @unique), use mpy-cross to measure bytecode size:

```bash
# Compile enum.py with all features
mpy-cross -o enum_full.mpy lib/enum/enum.py \
    -DMICROPY_PY_ENUM_FLAG=1 \
    -DMICROPY_PY_ENUM_STRENUM=1 \
    -DMICROPY_PY_ENUM_UNIQUE=1 \
    -DMICROPY_PY_ENUM_AUTO=1

# Compile enum.py with minimal features
mpy-cross -o enum_minimal.mpy lib/enum/enum.py \
    -DMICROPY_PY_ENUM_FLAG=0 \
    -DMICROPY_PY_ENUM_STRENUM=0 \
    -DMICROPY_PY_ENUM_UNIQUE=0 \
    -DMICROPY_PY_ENUM_AUTO=0

# Compare bytecode sizes
ls -l enum_*.mpy
```

### 7. Verification

After measurement, verify that:
1. Individual feature sizes sum approximately to total overhead
2. Size impact matches estimates (±50 bytes acceptable)
3. Disabling unused features reduces size as expected
4. No unexpected interactions between features

## Documentation Updates

### Files to Update

1. **CPYTHON_COMPATIBILITY.md**: Add sections for each new feature
2. **SIZE_REPORT.md**: Update with new size measurements
3. **enum.py docstrings**: Document all new classes and functions
4. **README or USAGE.md**: Add examples for each feature

## Backwards Compatibility

All features are additive and behind conditional compilation flags. Existing code using Enum/IntEnum will continue to work unchanged. Ports can selectively enable features based on ROM constraints.

## Future Enhancements (Out of Scope)

The following CPython features are not planned for implementation due to complexity or size constraints:

- `_missing_()` hook - Custom handling for missing values
- `_ignore_` attribute - Excluding members from the enum
- Functional API - `Status = Enum('Status', 'READY DONE')` syntax
- Complex auto() customization via `_generate_next_value_`
- Multi-bit flag aliases (e.g., `READ_WRITE = READ | WRITE`)

## Summary

This implementation plan provides a clear path to add optional enum and metaclass features while maintaining:

1. **CPython compatibility** for all supported features
2. **Minimal size impact** (~680-880 bytes total, 200-500 bytes for python-statemachine support)
3. **Flexible configuration** via ROM level flags
4. **Clean separation** of features for maintenance
5. **python-statemachine support** via metaclass enhancements

The phased approach prioritizes foundational metaclass features (Phase 0-1) needed for python-statemachine, followed by high-value enum extensions (Phase 2-4), and defers the complex auto() feature (Phase 5) until C support is ready.

### Key Priorities

**For python-statemachine support:**
- Phase 0: Verify/implement metaclass `__init__` (essential)
- Phase 1: Implement metaclass property/method lookup (critical for .events/.states API)

**For enum feature completeness:**
- Phase 2: Flag/IntFlag (high demand)
- Phase 3: StrEnum (low complexity)
- Phase 4: @unique (utility feature)
- Phase 5: auto() (requires C changes)
