print("Testing runtime aspects of typing module")


try:
    from typing import TYPE_CHECKING
except ImportError:
    print("SKIP")
    raise SystemExit

print("# Python 3.5+")
print("### Miscellaneous")


print("typing.TYPE_CHECKING")
if TYPE_CHECKING:
    from typing_extensions import TypeGuard

print("typing parameter annotations")
from typing import Any, Dict, List, Union


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


print("typing.Self - Python 3.11")
from typing import Callable, Self


class BaseClass:
    def register(self, callback: Callable[[Self], None]) -> None:
        ...


def cb(x):
    pass


base = BaseClass()
base.register(cb)


print("typing@no_type_check decorator")
from typing import no_type_check


@no_type_check
def quad(r0):
    return r0 * 4


print(quad(1))

print("typing.Protocol")

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

print("typing.NewType")

from typing import NewType

UserId = NewType("UserId", int)
some_id = UserId(524313)

print(some_id)

assert isinstance(some_id, int), "NewType should be instance of the original type"


print("typing.Any")
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

print("typing.AnyStr")

from typing import AnyStr


def concat(a: AnyStr, b: AnyStr) -> AnyStr:
    return a + b


concat("foo", "bar")  # OK, output has type 'str'
concat(b"foo", b"bar")  # OK, output has type 'bytes'
try:
    concat("foo", b"bar")  # Error, cannot mix str and bytes # type: ignore
except TypeError:
    print("TypeError is expected")


print("typing.LiteralString")
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

print("typing.overload")

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


print("typing.Required, NotRequired in TypedDict")
# https://typing.readthedocs.io/en/latest/spec/typeddict.html#required-and-notrequired

from typing import NotRequired, Required, TypedDict


class Movie(TypedDict):
    title: Required[str]
    year: int
    director: NotRequired[str]


m = Movie(title="Life of Brian", year=1979)


print("typing.TypeVar")

from typing import List, TypeVar

T = TypeVar("T")


def first(container: List[T]) -> T:
    return container[0]


list_one: List[str] = ["a", "b", "c"]
print(first(list_one))

list_two: List[int] = [1, 2, 3]
print(first(list_two))


print("typing.Generator")

from typing import Generator


def echo(a: float) -> Generator[int, float, str]:
    yield int(a)
    return "Done"


e = echo(2.4)
v = next(e)
print(v)


print("typing.NoReturn")

from typing import NoReturn


def stop() -> NoReturn:
    raise RuntimeError("no way")


#

print("typing.Final")

from typing import Final

CONST: Final = 42


print(CONST)


print("typing.final")

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


print("typing.TypeVarTuple and typing.Unpack")

from typing import TypeVarTuple, Unpack

Ts = TypeVarTuple("Ts")
tup: tuple[Unpack[Ts]]  # Semantically equivalent, and backwards-compatible # type: ignore


print("typing.Callable, ParamSpec")

# ParamSpec, 3.11 notation
# https://docs.python.org/3/library/typing.html#typing.ParamSpec

try:
    from collections.abc import Callable
except ImportError:
    print("- [ ] FIXME: from collections.abc import Callable")

from typing import Callable  # Workaround for test
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


print("typing.get_origin()")
# https://docs.python.org/3/library/typing.html#typing.get_origin

from typing import get_origin

# FIXME: - cpy_diff - get_origin() unsupported, or always returns None
if not get_origin(str) is None:
    print("- [ ] FIXME: cpy_diff - get_origin(str) should be None")
# assert get_origin(Dict[str, int]) is dict
# assert get_origin(Union[int, str]) is Union

print("typing.get_args()")
# https://docs.python.org/3/library/typing.html#typing.get_args
from typing import get_args, Dict, Union

# FIXME: - cpy_diff - get_args() unsupported, or always returns ()
if not get_args(int) == ():
    print("- [ ] FIXME: cpy_diff - get_args(int) should be ()")

# assert get_args(Dict[int, str]) == (int, str), "get_args(Dict[int, str]) should be (int, str)"
# assert get_args(Union[int, str]) == (int, str), "get_args(Union[int, str]) should be (int, str)"


print("Subscriptables")

from typing import (
    AbstractSet,
    AsyncContextManager,
    AsyncGenerator,
    AsyncIterable,
    AsyncIterator,
    Awaitable,
)
from typing import (
    Callable,
    ChainMap,
    Collection,
    Container,
    ContextManager,
    Coroutine,
    Counter,
    DefaultDict,
)
from typing import (
    Deque,
    Dict,
    FrozenSet,
    Generator,
    Generic,
    Iterable,
    Iterator,
    List,
    Literal,
    Mapping,
)
from typing import (
    MutableMapping,
    MutableSequence,
    MutableSet,
    NamedTuple,
    Optional,
    OrderedDict,
    Self,
)
from typing import Sequence, Set, Tuple, Type, Union


t_01: AbstractSet[Any]
t_02: AsyncContextManager[Any]
t_03: AsyncGenerator[Any]
t_04: AsyncIterable[Any]
t_05: AsyncIterator[Any]
t_06: Awaitable[Any]
t_07: Callable[[], Any]
t_08: ChainMap[Any, Any]
t_09: Collection[Any]
t_10: Container[Any]
t_11: ContextManager[Any]
t_12: Coroutine[Any, Any, Any]
t_13: Counter[Any]
t_14: DefaultDict[Any, Any]
t_15: Deque[Any]
t_16: Dict[Any, Any]
t_17: FrozenSet[Any]
t_18: Generator[Any]
# t_19: Generic[Any]
t_20: Iterable[Any]
t_21: Iterator[Any]
t_22: List[Any]
t_23: Literal[1, 2, 3, "a", b"b", True, None]
t_24: Mapping[Any, Any]
t_25: MutableMapping[Any, Any]
t_26: MutableSequence[Any]
t_27: MutableSet[Any]
t_28: NamedTuple
t_29: Optional[Any]
t_30: OrderedDict[Any, Any]
# t_31: Self[Any]
t_32: Sequence[Any]
t_33: Set[Any]
t_34: Tuple[Any]
t_35: Type[Any]
t_36: Union[Any, Any]


print("-----")
