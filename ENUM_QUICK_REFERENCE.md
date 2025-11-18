# MicroPython Enum - Quick Reference

## Import

```python
from enum import Enum, IntEnum, Flag, IntFlag, StrEnum, auto, unique
```

## Basic Enum

```python
class Color(Enum):
    RED = 1
    GREEN = 2
    BLUE = 3

# Access
Color.RED                    # <Color.RED: 1>
Color(1)                     # <Color.RED: 1>
Color['RED']                 # <Color.RED: 1>

# Attributes
Color.RED.name              # 'RED'
Color.RED.value             # 1

# Iteration
list(Color)                 # [<Color.RED: 1>, <Color.GREEN: 2>, <Color.BLUE: 3>]
for color in Color:
    print(color)

# Comparison
Color.RED == Color.RED      # True
Color.RED is Color.RED      # True (singleton)
Color.RED == 1             # False (not equal to value)
```

## auto() Values

```python
class Status(Enum):
    PENDING = auto()        # 1
    ACTIVE = auto()         # 2
    DONE = auto()           # 3

# Can mix
class Mixed(Enum):
    FIRST = auto()          # 1
    SECOND = 5              # 5
    THIRD = auto()          # 6
```

## IntEnum (Integer Behavior)

```python
class HttpStatus(IntEnum):
    OK = 200
    NOT_FOUND = 404

# Integer operations
HttpStatus.OK + 1           # 201
HttpStatus.OK < 300         # True
int(HttpStatus.OK)          # 200

# Still an enum
HttpStatus.OK.name          # 'OK'
```

## StrEnum (String Behavior)

```python
class Mode(StrEnum):
    READ = 'r'
    WRITE = 'w'

# String operations
Mode.READ + 'b'             # 'rb'
Mode.READ.upper()           # 'R'
Mode.READ.startswith('r')   # True

# Still an enum
Mode.READ.name              # 'READ'
```

## Flag (Bitwise Operations)

```python
class Permission(Flag):
    READ = 1
    WRITE = 2
    EXECUTE = 4

# Combine with |
read_write = Permission.READ | Permission.WRITE  # <Permission.READ|WRITE: 3>

# Check with &
if perms & Permission.READ:
    print("Can read")

# Remove with ^
perms = Permission.READ | Permission.WRITE
perms = perms ^ Permission.WRITE  # Remove WRITE

# Invert with ~
all_except_exec = ~Permission.EXECUTE

# Check membership
Permission.READ in Permission.READ | Permission.WRITE  # True
3 in Permission  # True if named combo exists
```

## IntFlag (Flag + Integer)

```python
class Mask(IntFlag):
    A = 1
    B = 2
    C = 4

# Both flag and integer operations
combo = Mask.A | Mask.B     # 3
combo + 1                   # 4
combo & Mask.A             # <Mask.A: 1>
```

## @unique Decorator

```python
from enum import unique

@unique
class Status(Enum):
    PENDING = 1
    ACTIVE = 2
    DONE = 1  # ValueError: duplicate values found: DONE -> PENDING
```

## Type Mixins

```python
# Float enum
class Precision(float, Enum):
    LOW = 0.1
    HIGH = 0.01

Precision.LOW / 2           # 0.05

# Date enum
from datetime import date

class Holiday(date, Enum):
    NEW_YEAR = date(2025, 1, 1)
    CHRISTMAS = date(2025, 12, 25)

Holiday.NEW_YEAR.year       # 2025
```

## Introspection

```python
class Color(Enum):
    RED = 1
    GREEN = 2

# All members
Color.__members__           # {'RED': <Color.RED: 1>, 'GREEN': <Color.GREEN: 2>}

# Iterate members
for name, member in Color.__members__.items():
    print(f"{name}: {member.value}")

# Check type
isinstance(Color.RED, Color)    # True
type(Color.RED)                 # <enum 'Color'>
```

## Pickling

```python
import pickle

class Status(Enum):
    PENDING = 1
    ACTIVE = 2

# Serialize
data = pickle.dumps(Status.PENDING)

# Deserialize
status = pickle.loads(data)
assert status is Status.PENDING  # True (restored identity)
```

## Common Patterns

### State Machine
```python
class State(Enum):
    IDLE = 1
    RUNNING = 2
    STOPPED = 3

current_state = State.IDLE

if current_state == State.IDLE:
    current_state = State.RUNNING
```

### Configuration
```python
class LogLevel(IntEnum):
    DEBUG = 10
    INFO = 20
    WARNING = 30
    ERROR = 40

def log(message, level):
    if level >= LogLevel.WARNING:
        print(f"WARNING: {message}")
```

### Permissions
```python
class Perm(Flag):
    READ = 1
    WRITE = 2
    EXECUTE = 4

admin = Perm.READ | Perm.WRITE | Perm.EXECUTE
user = Perm.READ

def can_write(perms):
    return bool(perms & Perm.WRITE)
```

### HTTP Methods
```python
class Method(StrEnum):
    GET = 'GET'
    POST = 'POST'
    PUT = 'PUT'
    DELETE = 'DELETE'

# Use directly as strings
request(url, method=Method.GET)
```

## Known Limitations

### ✗ Functional API
```python
# NOT SUPPORTED
Status = Enum('Status', 'PENDING ACTIVE')

# Use class syntax instead
class Status(Enum):
    PENDING = 1
    ACTIVE = 2
```

### ⚠️ Flag Combination Membership
```python
class Perm(Flag):
    READ = 1
    WRITE = 2
    EXEC = 4

# Works
Perm(7)  # <Perm.READ|WRITE|EXEC: 7>

# Doesn't work (returns False, should be True)
7 in Perm

# Workaround
try:
    Perm(value)
    # Valid
except ValueError:
    # Invalid
```

### ✗ Advanced Hooks
```python
# NOT SUPPORTED
class MyEnum(Enum):
    _missing_ = ...         # Custom value lookup
    _ignore_ = ...          # Exclude attributes
    _generate_next_value_ = ...  # Custom auto()
```

## Testing

```python
# CPython enum test suite: 99.3% compatible (445/448 tests)
# See CPYTHON_TESTS_REPORT.md for details
```

## Memory Usage

```python
# Lazy loading - minimal initial footprint
from enum import Enum       # ~2KB

# Full import
from enum import *          # ~8KB (vs CPython ~15KB)
```

## More Information

- **Summary:** `COMPATIBILITY_SUMMARY.md`
- **Full Report:** `CPYTHON_TESTS_REPORT.md`
- **Test Artifacts:** `TEST_ARTIFACTS.md`
- **Implementation:** `lib/enum/`
