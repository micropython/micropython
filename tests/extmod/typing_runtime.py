print("Testing runtime aspects of typing module")

try:
    from typing import TYPE_CHECKING
except ImportError:
    print("SKIP")
    raise SystemExit


print("Testing : typing.TYPE_CHECKING - Python 3.5.2")
if TYPE_CHECKING:
    from typing_extensions import TypeGuard

print("Testing : typing parameter annotations")
from typing import Any, Dict, List


def add_numbers(a: int, b: int) -> int:
    return a + b


def greet(name: str) -> None:
    print(f"Hello, {name}!")


def get_average(numbers: List[float]) -> float:
    return sum(numbers) / len(numbers)


def process_data(data: Dict[str, Any]) -> None:
    # process the data
    pass


a = add_numbers(3, 5)
greet("Alice")
avg = get_average([1.0, 2.5, 3.5])
process_data({"key": "value", "number": 42})


print("Testing : typing.Self - Python 3.11")
from typing import Callable, Self


class BaseClass:
    def register(self, callback: Callable[[Self], None]) -> None:
        ...


def cb(x):
    pass


base = BaseClass()
base.register(cb)


print("Testing : typing@no_type_check decorator")
from typing import no_type_check


@no_type_check
def quad(r0):
    return r0 * 4


print(quad(1))

print("Testing : typing.Protocol")

from typing import Protocol


class Adder(Protocol):
    def add(self, x, y):
        ...


class IntAdder:
    def add(self, x, y):
        return x + y


class FloatAdder:
    def add(self, x, y):
        return x + y


def add(adder: Adder) -> None:
    print(adder.add(2, 3))


add(IntAdder())
add(FloatAdder())

print("Testing : typing.NewType")

from typing import NewType

UserId = NewType("UserId", int)
some_id = UserId(524313)

print(some_id)

assert isinstance(some_id, int), "NewType should be instance of the original type"


print("Testing : typing.Any")
from typing import Any

a: Any = None
a = []  # OK
a = 2  # OK

s: str = ""
s = a  # OK


def foo(item: Any) -> int:
    # Passes type checking; 'item' could be any type,
    # and that type might have a 'bar' method
    item.bar()
    return 42


def hash_b(item: Any) -> int:
    try:
        # Passes type checking
        item.magic()
        return foo(item)
    except AttributeError:
        # just ignore any error for this test
        pass
    return 21
    ...


print(hash_b(42))
print(hash_b("foo"))

print("Testing : typing.AnyStr")

from typing import AnyStr


def concat(a: AnyStr, b: AnyStr) -> AnyStr:
    return a + b


concat("foo", "bar")  # OK, output has type 'str'
concat(b"foo", b"bar")  # OK, output has type 'bytes'
try:
    concat("foo", b"bar")  # Error, cannot mix str and bytes
except TypeError:
    print("TypeError is expected")


print("Testing : typing.LiteralString")
from typing import LiteralString


def run_query(sql: LiteralString) -> None:
    ...


def caller(arbitrary_string: str, literal_string: LiteralString) -> None:
    run_query("SELECT * FROM students")  # OK
    run_query(literal_string)  # OK
    run_query("SELECT * FROM " + literal_string)  # OK
    run_query(arbitrary_string)  # type: ignore  # type checker error
    run_query(f"SELECT * FROM students WHERE name = {arbitrary_string}")  # type: ignore  # type checker error

    assert isinstance(literal_string, str), "literal_string should be a string"
    assert isinstance(arbitrary_string, str), "arbitrary_string should be a string"


some_str = "a" * 1000
literal_str = "drop * from tables"

caller(some_str, literal_str)

print("Testing : typing.overload")

from typing import overload


@overload
def bar(x: int) -> str:
    ...


@overload
def bar(x: str) -> int:
    ...


def bar(x):
    return x


print(bar(42))


print("Testing : typing.Required, NotRequired in TypedDict")

# Specification: https://typing.readthedocs.io/en/latest/spec/typeddict.html#required-and-notrequired

from typing import NotRequired, Required, TypedDict


class Movie(TypedDict):
    title: Required[str]
    year: int
    director: NotRequired[str]


m = Movie(title="Life of Brian", year=1979)


print("Testing : typing.TypeVar")

from typing import List, TypeVar

T = TypeVar("T")


def first(container: List[T]) -> T:
    return container[0]


list_one: List[str] = ["a", "b", "c"]
print(first(list_one))

list_two: List[int] = [1, 2, 3]
print(first(list_two))


print("Testing : typing.Generator")

from typing import Generator


def echo(a: float) -> Generator[int, float, str]:
    yield int(a)
    return "Done"


e = echo(2.4)
v = next(e)
print(v)


print("Testing : typing.NoReturn")

from typing import NoReturn


def stop() -> NoReturn:
    raise RuntimeError("no way")


#

print("Testing : typing.Final")

from typing import Final

CONST: Final = 42


print(CONST)


print("Testing : typing.final")

from typing import final


class Base:
    @final
    def done(self) -> None:
        ...


class Sub(Base):
    def done(self) -> None:  # type: ignore # Error reported by type checker
        ...


@final
class Leaf:
    ...


class Other(Leaf):  # type: ignore # Error reported by type checker
    ...


other = Other()


print("Testing : typing.TypeVarTuple and typing.Unpack")

from typing import TypeVarTuple, Unpack

Ts = TypeVarTuple("Ts")
tup: tuple[Unpack[Ts]]  # Semantically equivalent, and backwards-compatible


print("Testing : typing.Callable, ParamSpec")

# ParamSpec, 3.11 notation
# https://docs.python.org/3/library/typing.html#typing.ParamSpec

# FIXME: from collections.abc import Callable
from typing import Callable
from typing import TypeVar, ParamSpec

T = TypeVar("T")
P = ParamSpec("P")


def add_logging(f: Callable[P, T]) -> Callable[P, T]:
    """A type-safe decorator to add logging to a function."""

    def inner(*args: P.args, **kwargs: P.kwargs) -> T:
        print(f"{f.__name__} was called")
        return f(*args, **kwargs)

    return inner


@add_logging
def add_two(x: float, y: float) -> float:
    """Add two numbers together."""
    return x + y


x = add_two(1, 2)
print(x)
assert x == 3, "add_two(1, 2) == 3"


print("Testing : typing.")
print("Testing : typing.")
print("Testing : typing.")
print("Testing : typing.")
print("Testing : typing.")
