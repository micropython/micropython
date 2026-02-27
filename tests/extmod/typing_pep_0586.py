# FIXME: This may break the test if __future__ is not available
from __future__ import annotations

try:
    from typing import TYPE_CHECKING
except Exception:
    print("SKIP")
    raise SystemExit

print("# Python 3.8")
print("### PEP 586")

# https://peps.python.org/pep-0586

print("Legal parameters for Literal at type check time")
from typing import Literal, Optional


class Color:
    RED = 1
    GREEN = 2
    BLUE = 3


Literal[26]
Literal[0x1A]  # Exactly equivalent to Literal[26]
Literal[-4]
Literal["hello world"]
Literal[b"hello world"]
Literal["hello world"]
Literal[True]
Literal[Color.RED]  # Assuming Color is some enum
Literal[None]

# ----------

# FIXME: TypeError: 'type' object isn't subscriptable
ReadOnlyMode = Literal["r", "r+"]
WriteAndTruncateMode = Literal["w", "w+", "wt", "w+t"]
WriteNoTruncateMode = Literal["r+", "r+t"]
AppendMode = Literal["a", "a+", "at", "a+t"]

# AllModes = Literal[ReadOnlyMode, WriteAndTruncateMode, WriteNoTruncateMode, AppendMode]

# ----------
# FIXME: TypeError: 'type' object isn't subscriptable
Literal[Literal[Literal[1, 2, 3], "foo"], 5, None]
# Optional[Literal[1, 2, 3, "foo", 5]]

print("Parameters at runtime")


def my_function(x: Literal[1, 2]) -> int:
    return x * 3


x: Literal[1, 2, 3] = 3
y: Literal[my_function] = my_function  # type: ignore


print("Using non-Literals in Literal contexts")


def expects_str(x: str) -> None:
    ...


var: Literal["foo"] = "foo"

# Legal: Literal["foo"] is a subtype of str
expects_str(var)

# ---------------------


def expects_literal(x: Literal["foo"]) -> None:
    ...


def runner(my_str: str) -> None:
    expects_literal(my_str)  # type: ignore


runner("foo")  # type: ignore

print("Intelligent indexing of structured data")
from typing import Tuple, List, Literal

a: Literal[0] = 0
b: Literal[5] = 5

some_tuple: Tuple[int, str, List[bool]] = (3, "abc", [True, False])

# FIXME: NameError: name 'reveal_type' isn't defined
# reveal_type(some_tuple[a])  # Revealed type is 'int'

try:
    some_tuple[b]  # Error: 5 is not a valid index into the tuple # type: ignore
except Exception:
    print("Expected: tuple index out of range")

# -----------------


class Test:
    def __init__(self, param: int) -> None:
        self.myfield = param

    def mymethod(self, val: int) -> str:
        ...


a: Literal["myfield"] = "myfield"
b: Literal["mymethod"] = "mymethod"
c: Literal["blah"] = "blah"

t = Test(24)
# reveal_type(getattr(t, a))  # Revealed type is 'int'
# reveal_type(getattr(t, b))  # Revealed type is 'Callable[[int], str]'

try:
    getattr(t, c)
except AttributeError:
    print("Expected: No attribute named 'blah' in Test")

print("Interactions with overloads")
from typing import overload, IO, Any, Union, Text

# FIXME: TypeError: 'type' object isn't subscriptable
# _PathType = Union[str, bytes, int]
_PathType = str


@overload
def open(
    path: _PathType,
    mode: Literal["r", "w", "a", "x", "r+", "w+", "a+", "x+"],
) -> IO[Text]:
    ...


@overload
def open(
    path: _PathType,
    mode: Literal["rb", "wb", "ab", "xb", "r+b", "w+b", "a+b", "x+b"],
) -> IO[bytes]:
    ...


# Fallback overload for when the user isn't using literal types
@overload
def open(path: _PathType, mode: str) -> IO[Any]:
    pass


print("Interactions with generics")
# Fixme: User Defined Generic Classes unsupported

# from typing import Generic, TypeVar

# A = TypeVar("A", bound=int)
# B = TypeVar("B", bound=int)
# C = TypeVar("C", bound=int)


# requires from __futures__ import annotations
# A simplified definition for Matrix[row, column]
# TypeError: 'type' object isn't subscriptable
# class Matrix(Generic[A, B]):
# class Matrix(Generic[A, B]):
#     def __add__(self, other: Matrix[A, B]) -> Matrix[A, B]: ...
#     def __matmul__(self, other: Matrix[B, C]) -> Matrix[A, C]: ...
#     def transpose(self) -> Matrix[B, A]: ...


# foo: Matrix[Literal[2], Literal[3]] = Matrix()
# bar: Matrix[Literal[3], Literal[7]] = Matrix()

# baz = foo @ bar
# reveal_type(baz)  # Revealed type is 'Matrix[Literal[2], Literal[7]]'


print("Interactions with enums and exhaustiveness checks")

# FIXME: enum module not standard in MicroPython
try:
    from enum import Enum

    class Status(Enum):
        SUCCESS = 0
        INVALID_DATA = 1
        FATAL_ERROR = 2

    def parse_status(s: Union[str, Status]) -> None:
        if s is Status.SUCCESS:
            print("Success!")
        elif s is Status.INVALID_DATA:
            print("The given data is invalid because...")
        elif s is Status.FATAL_ERROR:
            print("Unexpected fatal error...")
        else:
            # 's' must be of type 'str' since all other options are exhausted
            print("Got custom status: " + s)

except ImportError:
    # print("Skipped enum test, enum module not available")
    pass


print("-----")
