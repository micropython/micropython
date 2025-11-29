# Enum Implementation Plan for MicroPython

## Executive Summary

Python's `enum` module provides enumeration types through a sophisticated metaclass-based architecture. This document analyzes the CPython implementation and proposes strategies for implementing Enum in MicroPython, considering the current WIP metaclass support on the py-metaclass branch.

## CPython Enum Architecture

### File Information
- **Source:** `cpython/Lib/enum.py` (Python 3.10)
- **Size:** ~1,300 lines
- **License:** PSF (Python Software Foundation)

### Core Components

#### 1. EnumMeta Metaclass
The metaclass that controls enum creation through:

**`__prepare__(metacls, cls, bases, **kwds)`**
- Returns `_EnumDict` instance instead of regular dict
- Tracks member names in definition order
- Validates naming conventions (prevents dunder/sunder abuse)
- Inherits `_generate_next_value_` from base enum if present

**`__new__(metacls, cls, bases, classdict, **kwds)`**
- Extracts member definitions from `_EnumDict._member_names`
- Removes members from classdict before calling `type.__new__`
- Instantiates each member using the enum class's `__new__`
- Builds `_value2member_map_` for reverse lookups
- Handles aliases (members with duplicate values)
- Manages pickle support

**`__call__(cls, value)`**
- Performs value-to-member lookup (e.g., `Color(3)`)
- Supports functional API: `Color = Enum('Color', 'RED GREEN BLUE')`

#### 2. _EnumDict Class
Custom namespace dictionary that:
- Maintains `_member_names` list in definition order
- Rejects duplicate member names
- Handles `auto()` value generation via `_generate_next_value_`
- Prevents private name misuse (deprecated in 3.10+)

#### 3. Enum Base Class
Foundation for all enumerations:

**`__new__(cls, value)`**
- Implements value lookup
- Falls back to `_missing_()` hook if value not found
- Handles both hashable and non-hashable values

**Properties:**
- `name`: Member name (DynamicClassAttribute)
- `value`: Member value (DynamicClassAttribute)

**Methods:**
- `_generate_next_value_(name, start, count, last_values)`: Auto-increment logic
- `__repr__()`, `__str__()`, `__format__()`: String representations
- `__reduce_ex__()`: Pickle support

#### 4. Specialized Types
- **IntEnum**: Enum + int mixin, supports arithmetic
- **Flag**: Bitwise flag operations with `|`, `&`, `^`, `~`
- **IntFlag**: Flag + int mixin
- **StrEnum**: String-based enums (3.11+)

### Key Design Patterns

#### Member Separation Strategy
```python
# During EnumMeta.__new__:
# 1. Extract members from _EnumDict
enum_members = {k: classdict[k] for k in classdict._member_names}

# 2. Remove from classdict
for name in classdict._member_names:
    del classdict[name]

# 3. Create class without members
enum_class = super().__new__(metacls, cls, bases, classdict, **kwds)

# 4. Instantiate and attach members
for member_name, value in enum_members.items():
    enum_member = enum_class.__new__(enum_class, value)
    enum_member._name_ = member_name
    enum_member._value_ = value
    setattr(enum_class, member_name, enum_member)
```

This prevents members from shadowing methods while maintaining attribute access.

#### Value Lookup Mechanism
```python
# Enum.__new__ performs reverse lookup:
def __new__(cls, value):
    # Quick lookup for hashable values
    try:
        return cls._value2member_map_[value]
    except (KeyError, TypeError):
        # Slow path for non-hashable values
        for member in cls._member_map_.values():
            if member._value_ == value:
                return member
    # Fallback to _missing_() hook
    if cls._missing_ is not None:
        return cls._missing_(value)
    raise ValueError(...)
```

## Metaclass Features Required

### Currently Implemented in py-metaclass Branch ✓
1. **Metaclass keyword argument** - `class Color(metaclass=EnumMeta)`
2. **Meta.__init__ invocation** - Called during class creation
3. **Basic type.__new__** - 4-arg form for creating types

### Required but Not Yet Implemented ✗
1. **`__prepare__` method support** - CRITICAL for Enum
   - Must be called before class body execution
   - Returns custom namespace dict
   - MicroPython compile.c needs modification

2. **Inheritance from metaclass-based classes** - CRITICAL
   - Current check at objtype.c:1215 too strict
   - Prevents `class MyEnum(Enum):` where Enum has EnumMeta
   - Needs fix from METACLASS_STATUS.md

3. **Single-argument `__new__` form** - Required for Enum.__new__
   - Current type.__new__ only handles 4-arg form
   - Needs dual-signature implementation

4. **`DynamicClassAttribute`** - Optional but useful
   - Descriptor that behaves differently for class vs instance access
   - Used for `name` and `value` properties
   - Can be implemented in pure Python

### Additional MicroPython Limitations
1. **`auto()` function** - Requires _EnumDict integration
2. **Pickle support** - Less critical for embedded systems
3. **`__set_name__` descriptor protocol** - Added in Python 3.6
4. **Type annotations** - Not critical for runtime

## Implementation Strategies

### Strategy 1: Full CPython Port (Not Recommended)

**Approach:** Port complete enum.py from CPython 3.10

**Pros:**
- Full compatibility with CPython code
- Complete feature set (IntEnum, Flag, IntFlag, etc.)
- Well-tested implementation

**Cons:**
- **1,300+ lines** - Massive code size for embedded systems
- Requires complete metaclass support (including `__prepare__`)
- Heavy use of advanced features (pickle, typing, etc.)
- Performance overhead
- May never fit in constrained environments

**Verdict:** Impractical for MicroPython's embedded focus

---

### Strategy 2: Minimal Pure Python Implementation

**Approach:** Simplified enum without metaclass, using class decorators

**Example:**
```python
def enum(cls):
    """Decorator that converts a class into an enum."""
    members = {}
    for name in dir(cls):
        if not name.startswith('_'):
            value = getattr(cls, name)
            member = object.__new__(cls)
            member._name_ = name
            member._value_ = value
            members[name] = member
            setattr(cls, name, member)
    cls._members_ = members
    cls._value2member_map_ = {m._value_: m for m in members.values()}
    return cls

@enum
class Color:
    RED = 1
    GREEN = 2
    BLUE = 3
```

**Pros:**
- No metaclass required - works on current MicroPython
- Simple implementation (~50-100 lines)
- Covers 80% of use cases
- Easy to understand and maintain

**Cons:**
- No `__prepare__` support - members must be simple assignments
- Cannot prevent `Color.RED = 4` modification
- No automatic iteration unless `__iter__` added
- Different syntax than CPython (needs decorator)

**Verdict:** Good fallback if metaclass support stalls

---

### Strategy 3: Hybrid - Minimal Metaclass Implementation (Recommended)

**Approach:** Simplified enum using metaclass but without advanced features

**Implementation:** ~200-300 lines covering:
- EnumMeta metaclass with `__new__` (skip `__prepare__` initially)
- Basic Enum class with `__new__`, `name`, `value`
- Simple `_value2member_map_` for lookups
- IntEnum subclass for integer operations

**Features Included:**
- ✓ Member definition: `class Color(Enum): RED = 1`
- ✓ Value lookup: `Color(1)` returns `Color.RED`
- ✓ Iteration: `for c in Color: ...`
- ✓ Comparison: `Color.RED == Color.RED`
- ✓ String representation: `repr(Color.RED)` → `'<Color.RED: 1>'`
- ✓ Attribute access: `Color.RED.name` → `'RED'`
- ✓ Alias detection: Multiple names for same value
- ✓ IntEnum: `class Status(IntEnum): OK = 200`

**Features Excluded:**
- ✗ `auto()` - Requires `__prepare__` or use sentinel values
- ✗ Flag operations - Specialized, can add later
- ✗ Pickle support - Less relevant for embedded
- ✗ `_missing_()` hook - Advanced use case
- ✗ Functional API - Can add incrementally
- ✗ Strict immutability - Rely on convention

**Pros:**
- Reasonable size for embedded systems
- True metaclass implementation matches CPython semantics
- Can evolve toward full compatibility as metaclass support improves
- Covers majority of real-world use cases

**Cons:**
- Requires fixing current metaclass bugs (inheritance, `__new__`)
- Limited without `__prepare__` support
- Missing some advanced features

**Verdict:** Best balance for MicroPython

---

### Strategy 4: Two-Phase Approach (Most Pragmatic)

**Phase 1: Decorator-based (Immediate)**
- Implement Strategy 2 (pure Python decorator)
- Deploy to micropython-lib as `uenum` or `enum`
- Works on current MicroPython releases
- ~50-100 lines

**Phase 2: Metaclass-based (When Ready)**
- Fix metaclass bugs on py-metaclass branch
- Implement Strategy 3 (minimal metaclass)
- Replace decorator version in micropython-lib
- ~200-300 lines
- Add `__prepare__` support when compile.c updated

**Pros:**
- Immediate value with Phase 1
- Clear migration path
- Risk mitigation - don't wait for metaclass completion
- Learn from Phase 1 usage before Phase 2

**Cons:**
- Code churn - two implementations
- API might differ slightly between phases
- Users need to know which version they have

**Verdict:** Best practical approach

## Detailed Implementation Plan

### Phase 1: Decorator-Based Enum (Immediate)

#### File Structure
```
lib/micropython-lib/python-stdlib/enum/
├── enum.py          # Main implementation
├── manifest.py      # Package manifest
└── test_enum.py     # Basic tests
```

#### Core Code (~80 lines)
```python
class EnumMember:
    """Represents a single enum member."""
    __slots__ = ('_name_', '_value_', '__objclass__')

    def __init__(self, name, value, enum_class):
        self._name_ = name
        self._value_ = value
        self.__objclass__ = enum_class

    @property
    def name(self):
        return self._name_

    @property
    def value(self):
        return self._value_

    def __repr__(self):
        return f'<{self.__objclass__.__name__}.{self._name_}: {self._value_!r}>'

    def __str__(self):
        return f'{self.__objclass__.__name__}.{self._name_}'

    def __eq__(self, other):
        if isinstance(other, EnumMember):
            return self is other
        return NotImplemented

    def __hash__(self):
        return hash(self._name_)

def enum(cls):
    """Class decorator to create an enumeration."""
    members = {}
    member_values = {}

    # Extract members
    for name in dir(cls):
        if not name.startswith('_'):
            value = getattr(cls, name)
            if not callable(value):
                # Create member
                member = EnumMember(name, value, cls)
                members[name] = member

                # Track aliases
                if value in member_values:
                    # This is an alias
                    pass
                else:
                    member_values[value] = member

                # Replace class attribute
                setattr(cls, name, member)

    # Add mappings
    cls._member_map_ = members
    cls._value2member_map_ = member_values

    # Add __iter__
    def __iter__(self):
        return iter(members.values())
    cls.__iter__ = classmethod(__iter__)

    # Add __call__ for value lookup
    original_new = cls.__new__
    def __new__(cls, value):
        try:
            return cls._value2member_map_[value]
        except KeyError:
            raise ValueError(f"{value} is not a valid {cls.__name__}")
    cls.__new__ = staticmethod(__new__)

    return cls

# Usage:
@enum
class Color:
    RED = 1
    GREEN = 2
    BLUE = 3
```

#### Testing Plan
```python
# test_enum.py
from enum import enum, EnumMember

@enum
class Color:
    RED = 1
    GREEN = 2
    BLUE = 3

# Test member access
assert Color.RED.name == 'RED'
assert Color.RED.value == 1

# Test value lookup
assert Color(1) is Color.RED

# Test iteration
assert list(Color) == [Color.RED, Color.GREEN, Color.BLUE]

# Test comparison
assert Color.RED == Color.RED
assert Color.RED != Color.GREEN

# Test repr
assert 'RED' in repr(Color.RED)
```

#### Deployment
1. Add to micropython-lib python-stdlib/enum
2. Test on unix port
3. Document limitations vs CPython
4. Release in next micropython-lib update

---

### Phase 2: Metaclass-Based Enum (After Metaclass Fixes)

#### Prerequisites
1. Fix inheritance from metaclass-based classes (objtype.c:1215)
2. Fix single-argument `__new__` form (objtype.c:1034)
3. Optionally: Add `__prepare__` support to compile.c

#### File Structure
```
lib/micropython-lib/python-stdlib/enum/
├── enum.py          # Metaclass implementation
├── manifest.py
└── test_enum.py     # Enhanced tests
```

#### Core Code (~250 lines)

**EnumMeta Metaclass:**
```python
class EnumMeta(type):
    """Metaclass for enumerations."""

    def __new__(metacls, cls, bases, classdict, **kwds):
        # Extract members (those without leading underscore)
        member_names = [k for k in classdict.keys()
                        if not k.startswith('_') and not callable(classdict[k])]

        # Store member values
        members = {name: classdict[name] for name in member_names}

        # Remove members from classdict
        for name in member_names:
            del classdict[name]

        # Add private attributes
        classdict['_member_map_'] = {}
        classdict['_value2member_map_'] = {}

        # Create the enum class
        enum_class = super().__new__(metacls, cls, bases, classdict)

        # Instantiate members
        for name, value in members.items():
            member = object.__new__(enum_class)
            member._name_ = name
            member._value_ = value
            member.__objclass__ = enum_class

            enum_class._member_map_[name] = member

            # Handle aliases
            if value not in enum_class._value2member_map_:
                enum_class._value2member_map_[value] = member

            setattr(enum_class, name, member)

        return enum_class

    def __iter__(cls):
        """Iterate over enum members."""
        return iter(cls._member_map_.values())

    def __call__(cls, value):
        """Lookup member by value."""
        try:
            return cls._value2member_map_[value]
        except KeyError:
            raise ValueError(f"{value} is not a valid {cls.__name__}")
```

**Enum Base Class:**
```python
class Enum(metaclass=EnumMeta):
    """Base class for enumerations."""

    def __new__(cls, value):
        # This is called during member instantiation and value lookup
        # During creation: return the instance being created
        # During lookup: handled by EnumMeta.__call__
        return object.__new__(cls)

    @property
    def name(self):
        return self._name_

    @property
    def value(self):
        return self._value_

    def __repr__(self):
        return f'<{self.__class__.__name__}.{self._name_}: {self._value_!r}>'

    def __str__(self):
        return f'{self.__class__.__name__}.{self._name_}'

    def __eq__(self, other):
        if isinstance(other, self.__class__):
            return self is other
        return NotImplemented

    def __hash__(self):
        return hash(self._name_)

# Usage (identical to CPython):
class Color(Enum):
    RED = 1
    GREEN = 2
    BLUE = 3
```

**IntEnum:**
```python
class IntEnum(int, Enum):
    """Enum where members are also integers."""

    def __new__(cls, value):
        obj = int.__new__(cls, value)
        obj._value_ = value
        return obj

# Usage:
class Status(IntEnum):
    OK = 200
    NOT_FOUND = 404

assert Status.OK == 200  # True due to int mixing
assert Status.OK + 1 == 201  # Arithmetic works
```

#### Missing Features (Acceptable Tradeoffs)

**No `auto()` support** (without `__prepare__`):
```python
# Won't work:
class Color(Enum):
    RED = auto()    # NameError: name 'auto' is not defined

# Workaround - use explicit values:
class Color(Enum):
    RED = 1
    GREEN = 2
    BLUE = 3

# Or use helper:
_auto_counter = 0
def _auto():
    global _auto_counter
    _auto_counter += 1
    return _auto_counter

class Color(Enum):
    RED = _auto()
    GREEN = _auto()
```

**No strict immutability:**
```python
# CPython prevents this:
# Color.RED = 5  # AttributeError

# MicroPython minimal enum won't prevent it
# But by convention, don't modify
```

**No `_missing_` hook:**
```python
# CPython allows:
class Color(Enum):
    @classmethod
    def _missing_(cls, value):
        return cls.RED  # Default fallback

# Minimal version: just raise ValueError
```

#### Testing Plan
```python
# Basic functionality
class Color(Enum):
    RED = 1
    GREEN = 2
    BLUE = 3

assert Color.RED.name == 'RED'
assert Color.RED.value == 1
assert Color(1) is Color.RED
assert list(Color) == [Color.RED, Color.GREEN, Color.BLUE]

# Inheritance
class ExtendedColor(Color):
    YELLOW = 4

assert ExtendedColor.RED is Color.RED  # Inherited member
assert ExtendedColor.YELLOW.value == 4

# IntEnum
class Status(IntEnum):
    OK = 200
    ERROR = 500

assert Status.OK == 200
assert Status.OK < Status.ERROR
assert Status.OK + 100 == 300

# Aliases
class Shape(Enum):
    SQUARE = 1
    DIAMOND = 1  # Alias

assert Shape.SQUARE is Shape.DIAMOND
assert Shape(1) is Shape.SQUARE
```

#### Integration
1. Replace decorator version in micropython-lib
2. Update documentation
3. Add to frozen modules for common ports
4. Coordinate with metaclass branch merge

---

## `__prepare__` Support Analysis

### What is `__prepare__`?

The `__prepare__` method allows metaclasses to control the namespace used during class body execution:

```python
class EnumMeta(type):
    @classmethod
    def __prepare__(metacls, name, bases, **kwds):
        return _EnumDict()  # Return custom dict instead of {}
```

When Python encounters:
```python
class Color(Enum):
    RED = 1
    GREEN = 2
```

The execution flow is:
1. Call `EnumMeta.__prepare__('Color', (Enum,), {})` → returns `_EnumDict()`
2. Execute class body with `_EnumDict` as namespace
3. Call `EnumMeta.__new__(EnumMeta, 'Color', (Enum,), <_EnumDict>, {})`

### Why Enum Needs It

**Purpose:** Track member names in definition order

**Without `__prepare__`:**
```python
# Class body executes with regular dict
namespace = {}
namespace['RED'] = 1      # Just a dict assignment
namespace['GREEN'] = 2    # No way to know this is a member vs method

# EnumMeta.__new__ receives:
# classdict = {'RED': 1, 'GREEN': 2, 'method': <function>}
# Problem: Can't distinguish members from methods!
```

**With `__prepare__`:**
```python
# Class body executes with _EnumDict
namespace = _EnumDict()
namespace['RED'] = 1      # _EnumDict.__setitem__ called, tracks name
namespace['GREEN'] = 2    # _EnumDict.__setitem__ called, tracks name

# EnumMeta.__new__ receives:
# classdict = _EnumDict with ._member_names = ['RED', 'GREEN']
# Solution: Explicit member list!
```

### MicroPython Implementation Challenge

**Location:** `py/compile.c` - class compilation

**Current Behavior:**
```c
// In compile_classdef (approximate):
mp_obj_t namespace = mp_obj_new_dict(0);  // Always create dict
compile_scope(classdef_body, namespace);   // Execute with dict
// No metaclass.__prepare__ call
```

**Required Changes:**
```c
// 1. Check if metaclass has __prepare__
mp_obj_t metacls = get_metaclass(bases, kwds);
mp_obj_t prepare_attr = mp_load_attr_maybe(metacls, MP_QSTR___prepare__);

// 2. Call it if present
mp_obj_t namespace;
if (prepare_attr != MP_OBJ_NULL) {
    namespace = mp_call_function_n_kw(prepare_attr, n_args, n_kw, args);
} else {
    namespace = mp_obj_new_dict(0);
}

// 3. Execute class body with custom namespace
compile_scope(classdef_body, namespace);

// 4. Pass to metaclass.__new__
```

**Complexity:** Moderate - requires changes to compilation flow

**Workarounds Without `__prepare__`:**

**Option 1: Explicit member list**
```python
class Color(Enum):
    _members_ = ['RED', 'GREEN', 'BLUE']
    RED = 1
    GREEN = 2
    BLUE = 3
```

**Option 2: Naming convention**
```python
class Color(Enum):
    # Only uppercase names are members
    RED = 1
    GREEN = 2

    def method(self):  # Lowercase - not a member
        pass
```

**Option 3: Type inspection**
```python
# In EnumMeta.__new__:
members = {k: v for k, v in classdict.items()
           if not k.startswith('_') and not callable(v)}
```

Option 3 is used in Phase 2 minimal implementation.

### Recommendation

**Short-term:** Use type inspection workaround
**Long-term:** Implement `__prepare__` in compile.c for full compatibility

## Size Estimates

### Phase 1: Decorator-Based
- **Core enum.py:** ~80 lines / ~2 KB
- **With IntEnum:** ~120 lines / ~3 KB
- **Memory footprint:** Minimal - just member objects

### Phase 2: Metaclass-Based
- **Core (Enum + EnumMeta):** ~180 lines / ~5 KB
- **With IntEnum:** ~220 lines / ~6 KB
- **With Flag:** ~300 lines / ~8 KB
- **Memory footprint:** Metaclass overhead + member objects

### Full CPython Port
- **Complete enum.py:** ~1,300 lines / ~40 KB
- **Memory footprint:** Significant - many advanced features

### Comparison to Other Modules
- **argparse:** ~2,000 lines (already in micropython-lib)
- **json:** ~100 lines (in extmod/modjson.c)
- **collections:** ~50 lines (py/objdeque.c + Python)

**Verdict:** Phase 2 size (~220 lines) is reasonable for micropython-lib

## Dependencies on Metaclass Fixes

### Critical Path

**For Phase 1 (Decorator):** None - works today

**For Phase 2 (Metaclass):**

1. **Fix: Inheritance from metaclass-based classes**
   - **File:** py/objtype.c:1215-1218
   - **Issue:** Cannot inherit from Enum (which has EnumMeta)
   - **Priority:** P0 - Blocking
   - **Effort:** Low - relax type check

2. **Fix: Single-argument `__new__`**
   - **File:** py/objtype.c:1034-1042
   - **Issue:** type.__new__ only handles 4-arg form
   - **Priority:** P1 - Needed for member instantiation
   - **Effort:** Medium - dual signature handling

3. **Feature: `__prepare__` support**
   - **File:** py/compile.c
   - **Issue:** No custom namespace support
   - **Priority:** P2 - Nice to have, workarounds exist
   - **Effort:** High - compilation flow changes

### Parallel Development

While waiting for metaclass fixes, develop Phase 1 decorator version:

```
Timeline:
Week 1-2:   Implement decorator-based enum
Week 3:     Testing and documentation
Week 4:     Submit to micropython-lib
Week 5+:    Fix metaclass bugs in parallel
Week 10+:   Implement Phase 2 when metaclass ready
```

## Compatibility Matrix

| Feature | CPython | Phase 1 (Decorator) | Phase 2 (Metaclass) | Phase 2 + `__prepare__` |
|---------|---------|---------------------|---------------------|------------------------|
| Basic members | ✓ | ✓ | ✓ | ✓ |
| Value lookup | ✓ | ✓ | ✓ | ✓ |
| Iteration | ✓ | ✓ | ✓ | ✓ |
| name/value properties | ✓ | ✓ | ✓ | ✓ |
| Aliases | ✓ | ✓ | ✓ | ✓ |
| IntEnum | ✓ | ⚠ Manual | ✓ | ✓ |
| Inheritance | ✓ | ✗ | ✓ | ✓ |
| `auto()` | ✓ | ✗ | ⚠ Workaround | ✓ |
| Functional API | ✓ | ✗ | ⚠ Future | ✓ |
| Flag/IntFlag | ✓ | ✗ | ⚠ Future | ✓ |
| `_missing_` hook | ✓ | ✗ | ⚠ Future | ✓ |
| Strict immutability | ✓ | ✗ | ✗ | ⚠ Partial |
| Pickle | ✓ | ✗ | ✗ | ⚠ Future |
| Size (lines) | 1,300 | 120 | 220 | 280 |

**Legend:**
- ✓ Fully supported
- ⚠ Partial or workaround available
- ✗ Not supported

## Recommended Next Steps

### Immediate (Week 1-2)
1. Implement Phase 1 decorator-based enum
2. Create test suite covering basic functionality
3. Document API and limitations
4. Submit PR to micropython-lib

### Short-term (Week 3-8)
1. Fix metaclass inheritance bug (objtype.c:1215)
2. Fix `__new__` dual-signature support (objtype.c:1034)
3. Test metaclass fixes with basic examples
4. Merge py-metaclass branch to main

### Medium-term (Week 9-16)
1. Implement Phase 2 metaclass-based enum
2. Replace Phase 1 in micropython-lib
3. Add IntEnum support
4. Comprehensive testing on multiple ports

### Long-term (Future)
1. Add `__prepare__` support to compile.c
2. Implement `auto()` properly
3. Add Flag/IntFlag if demand exists
4. Consider freezing into common ports

## Success Criteria

### Phase 1 Complete When:
- [ ] Decorator-based enum works on unix port
- [ ] Basic tests pass (member access, value lookup, iteration)
- [ ] Documentation complete
- [ ] Merged into micropython-lib

### Phase 2 Complete When:
- [ ] Metaclass bugs fixed
- [ ] True metaclass enum works on unix port
- [ ] IntEnum implemented and tested
- [ ] Inheritance from Enum works
- [ ] Compatible with CPython code (basic usage)
- [ ] Size < 300 lines
- [ ] Merged into micropython-lib

### Full Success:
- [ ] Enum available in MicroPython
- [ ] Used in MicroPython ecosystem (USB device classes, etc.)
- [ ] Performance acceptable for embedded use
- [ ] Migration path from decorator to metaclass smooth

## References

- **CPython Implementation:** https://github.com/python/cpython/blob/3.10/Lib/enum.py
- **PEP 435:** Adding an Enum type to the Python standard library
- **MicroPython metaclass status:** METACLASS_STATUS.md (this repo)
- **MicroPython-lib:** https://github.com/micropython/micropython-lib

## Conclusion

Enum is achievable in MicroPython through a phased approach:

1. **Phase 1** provides immediate value with minimal implementation
2. **Phase 2** delivers true metaclass-based enum when infrastructure ready
3. **Total effort:** ~300 lines of Python code + metaclass bug fixes

The key is not waiting for perfect metaclass support before providing enum functionality. Start with decorator-based approach, evolve to metaclass when ready.

**Recommendation:** Proceed with Phase 1 immediately, plan Phase 2 in parallel with metaclass fixes.
