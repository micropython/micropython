try:
    from typing import TYPE_CHECKING
except ImportError:
    print("SKIP")
    raise SystemExit

print("# Python 3.5")
print("### PEP 484")

# https://peps.python.org/topic/typing/
# https://peps.python.org/pep-0484/

# Currently excludes tests using `Generic[T]` due to MicroPython runtime limitations


print("Running PEP 484 example-based test")


print("Type Definition Syntax")


def greeting(name: str) -> str:
    return "Hello " + name


print("greeting:", greeting("world"))

from typing import List

l: List[int]


print("Type aliases")

Url = str


def retry(url: Url, retry_count: int) -> None:
    print("retry", url, retry_count)


retry("http://example", 3)


print("Callable example")
from typing import Callable, TypeVar, Union


def feeder(get_next_item: Callable[[], str]) -> None:
    try:
        v = get_next_item()
        print("feeder got", v)
    except Exception as e:
        print("feeder runtime exception:", e)


def get_const():
    return "x"


feeder(get_const)


print("TypeVar constrained example")
AnyStr = TypeVar("AnyStr", str, bytes)


def concat(x: AnyStr, y: AnyStr) -> AnyStr:
    return x + y


print("concat:", concat("a", "b"))

# Generic user-defined class
from typing import Generic, TypeVar

T = TypeVar("T")
# FIXME: Crash - inheriting from typing.Generic[T] unsupported at runtime
# try:
#
#     class LoggedVar(Generic[T]):
#         pass
#
#         def __init__(self, value: T, name: str) -> None:
#             self.name = name
#             self.value = value
#
#         def set(self, new: T) -> None:
#             self.value = new
#
#         def get(self) -> T:
#             return self.value
#
# except Exception as e:
#     print("-[ ] FIXME: Difference - Generic[T] base class unsupported:", e)


# Union/Optional examples
def handle_employee(e: Union[str, None]) -> None:
    print("handle_employee called with", e)


handle_employee("John")
handle_employee(None)


# Any example
def use_map(m: dict) -> None:
    print("use_map keys:", list(m.keys()))


use_map({"a": 1})

# NewType example: at runtime NewType returns identity function
try:
    from typing import NewType

    UserId = NewType("UserId", int)
    v = UserId(5)
    print("NewType UserId runtime:", v, type(v))
except Exception as e:
    print("-[ ] FIXME: Difference or Crash - NewType runtime issue:", e)

print("TYPE_CHECKING guard")

from typing import TYPE_CHECKING

# TYPE_CHECKING guard
if TYPE_CHECKING:
    # This block is for type checkers only
    pass
    print("typing.TYPE_CHECKING is True at runtime. ERROR")
else:
    print("typing.TYPE_CHECKING is False at runtime as expected")


print("Forward reference example")


class Tree:
    def __init__(self, left: "Tree" = None, right: "Tree" = None):  # type: ignore
        self.left = left
        self.right = right


tr = Tree()
print("Tree forward refs OK")

# NoReturn example
from typing import NoReturn


def stop() -> NoReturn:
    raise RuntimeError("stop")


try:
    stop()
except RuntimeError:
    print("stop() raised RuntimeError as expected (NoReturn at runtime)")

# Overload example (runtime @overload should not be called directly)
from typing import overload


@overload
def func(x: int) -> int:
    ...


@overload
def func(x: str) -> str:
    ...


def func(x):
    return x


print("overload func for int:", func(1))

# Cast example: at runtime cast returns the value
from typing import cast

print("cast runtime identity:", cast(str, 123))

print("-----")
