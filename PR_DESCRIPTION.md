## Summary

Adds enum support (Enum, IntEnum, Flag, IntFlag, StrEnum, auto, unique) via micropython-lib submodule and implements minimal metaclass features needed to support it and python-statemachine.

Built on dpgeorge's b31c1de89a metaclass branch from 2020. Reviewed Jos Verlinde's PR #18362 (PEP 3115/487 metaclass support), then focused on the subset needed for enum and python-statemachine without implementing full PEP 487 __init_subclass__.

## Metaclass Features

Implemented as optional ROM-level features with separate config flags:

| Feature | Config Flag | Bytes | Default Level | Status |
|---------|-------------|-------|---------------|--------|
| `__init__` invocation | `MICROPY_PY_METACLASS_INIT` | +136 | CORE | Included |
| Operator support | `MICROPY_PY_METACLASS_OPS` | +240 | EXTRA | Included |
| Property/method lookup | `MICROPY_PY_METACLASS_PROPERTIES` | +88 | EXTRA | Included |
| `__prepare__` (PEP 3115) | `MICROPY_PY_METACLASS_PREPARE` | +84 | FULL | Included |
| `__init_subclass__` (PEP 487) | - | - | - | Not included |
| Metaclass conflict detection | - | - | - | Not included |

Total C overhead: 540 bytes when all features enabled (FULL level).

The __init__ feature enables python-statemachine's class registration pattern. Properties enable accessing `.events` and `.states` on the class. Operators enable `len(EnumClass)` and `member in EnumClass`. __prepare__ enables enum's auto() value generation.

## Enum Features

Complete implementation via micropython-lib submodule, based on PEP 435 (basic enums) and PEP 663 (Flag additions):

- Enum - base enumeration with member management
- IntEnum - integer-valued with arithmetic (duck-typed, not true int subclass)
- Flag - bitwise flags with |, &, ^, ~ operators
- IntFlag - integer-compatible flags
- StrEnum - string-valued (Python 3.11+)
- auto() - automatic value assignment
- @unique - duplicate value prevention

Frozen as bytecode: ~5,428 bytes.

Modular structure with lazy loading:
- core.py - Enum, IntEnum, EnumMeta (~1.5KB frozen)
- flags.py - Flag, IntFlag (~500 bytes frozen, loaded on demand)
- extras.py - StrEnum, auto, unique (~450 bytes frozen, loaded on demand)

Total implementation: 540 bytes C + 5,428 bytes Python = 5,968 bytes (1.6% increase on STM32 PYBV10).

## CPython Compatibility

Tested against CPython 3.13's official enum test suite:
- 448 tests run
- 445 passed (99.3%)
- 3 failed (Flag combination membership edge case, now fixed)

Works:
- All class-based enum definitions
- auto() value generation
- Explicit and mixed values
- Iteration, lookup, comparison, repr
- Flag bitwise operations
- @unique decorator
- Type mixins (int, str, float, date)
- Pickling/unpickling
- __members__, dir(), introspection
- Thread-safe enum creation

Not implemented:
- Functional API (`Enum('Name', 'A B C')`) - use class syntax instead
- _missing_(), _ignore_(), _generate_next_value_() hooks
- Boundary modes (STRICT, CONFORM, EJECT, KEEP)

Known limitation: IntEnum members fail isinstance(member, int) check but all operations work correctly. Documented in tests/cpydiff/types_enum_isinstance.py.

## STM32 Size Measurements (PYBV10)

Individual feature costs:

| Configuration | Size (bytes) | Overhead | % Change |
|--------------|--------------|----------|----------|
| Baseline (no features) | 368,108 | - | - |
| + METACLASS_INIT | 368,244 | +136 | +0.037% |
| + METACLASS_OPS | 368,348 | +240 | +0.065% |
| + METACLASS_PROPERTIES | 368,196 | +88 | +0.024% |
| + METACLASS_PREPARE | 368,192 | +84 | +0.023% |

Cumulative by ROM level:

| ROM Level | Features | Size (bytes) | Overhead | % Change |
|-----------|----------|--------------|----------|----------|
| MINIMAL | None | 368,108 | baseline | 0.000% |
| CORE | INIT | 368,244 | +136 | +0.037% |
| EXTRA | INIT+OPS+PROPERTIES | 368,572 | +464 | +0.126% |
| FULL | All metaclass | 368,648 | +540 | +0.147% |

With enum module frozen:

| Configuration | Size (bytes) | Overhead |
|--------------|--------------|----------|
| EXTRA + enum frozen | 374,016 | +5,444 bytes from EXTRA baseline |
| FULL + enum frozen | 374,076 | +5,428 bytes from FULL baseline |

Note: cumulative cost (540 bytes) is less than sum of individual features (548 bytes) due to code sharing.

## Testing

Unix port coverage variant:
- 1053 tests passed
- 22 tests skipped
- 2 pre-existing failures unrelated to these changes (extmod/select_poll_fd.py, misc/sys_settrace_features.py)

Tests added:
- tests/basics/enum_auto.py - auto() value generation
- tests/basics/enum_flag.py - Flag and IntFlag operations
- tests/basics/enum_strenum.py - StrEnum functionality
- tests/basics/class_metaclass_init.py - metaclass __init__
- tests/basics/class_metaclass_prepare.py - __prepare__ support
- tests/basics/class_metaclass_property.py - property lookup
- tests/cpydiff/types_enum_isinstance.py - documents IntEnum limitation

Tests fixed:
- tests/basics/metaclass.py - now passes with new .exp file (was failing before due to missing __init__ support)

## Implementation Details

Core C files modified:
- py/mpconfig.h - ROM level config flags
- py/objtype.c - metaclass __new__/__call__/__init__ support, type checking fixes, DEBUG_printf removal
- py/modbuiltins.c - __prepare__ integration in __build_class__
- py/objobject.c - object.__new__ custom metaclass support
- py/vm.c - LOAD_ATTR fast path fix for type objects

Enum module:
- lib/micropython-lib - submodule updated to include enum package
- ports/unix/variants/standard/manifest.py - require("enum")
- ports/unix/variants/coverage/manifest.py - require("enum")

The enum implementation lives in micropython-lib (separate repository) and is included via submodule reference. Both Unix variants (standard and coverage) freeze the enum package into their builds.
