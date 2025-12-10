try:
    from typing import TYPE_CHECKING
except Exception:
    print("SKIP")
    raise SystemExit

print("# Python 3.8")
print("### PEP 544 - Protocols: Structural subtyping (static duck typing)")

# https://peps.python.org/topic/typing/
# Currently excludes tests using `Generic[T]` due to MicroPython runtime limitations

print("Defining a protocol")
# https://peps.python.org/pep-0544/#defining-a-protocol

from typing import Protocol, Iterable


class SupportsClose(Protocol):
    def close(self) -> None:
        ...


class Resource:
    ...

    def close(self) -> None:
        self.file.close()  # type: ignore
        self.lock.release()  # type: ignore


def close_all(things: Iterable[SupportsClose]) -> None:
    for t in things:
        t.close()


# FIXME: Difference or Crash - Resource requires file and lock attributes
# r = Resource()
# close_all([f, r])  # OK!
# try:
#     close_all([1])  # Error: 'int' has no 'close' method
# except Exception:
#     print("Expected: 'int' has no 'close' method")

print("Protocol members")
# https://peps.python.org/pep-0544/#protocol-members


from typing import Protocol
from abc import abstractmethod


class Example(Protocol):
    def first(self) -> int:  # This is a protocol member
        return 42

    @abstractmethod
    def second(self) -> int:  # Method without a default implementation
        raise NotImplementedError


# ---------

from typing import Protocol, List


class Template(Protocol):
    name: str  # This is a protocol member
    value: int = 0  # This one too (with default)

    def method(self) -> None:
        self.temp: List[int] = []  # Error in type checker # type: ignore


class Concrete_1:
    def __init__(self, name: str, value: int) -> None:
        self.name = name
        self.value = value

    def method(self) -> None:
        return


var: Template = Concrete_1("value", 42)  # OK


print("Explicitly declaring implementation")


class PColor(Protocol):
    @abstractmethod
    def draw(self) -> str:
        ...

    def complex_method(self) -> int:
        # some complex code here
        ...


class NiceColor(PColor):
    def draw(self) -> str:
        return "deep blue"


class BadColor(PColor):
    def draw(self) -> str:
        return super().draw()  # Error, no default implementation # type: ignore


class ImplicitColor:  # Note no 'PColor' base here
    def draw(self) -> str:
        return "probably gray"

    def complex_method(self) -> int:
        # class needs to implement this
        ...


nice: NiceColor
another: ImplicitColor


def represent(c: PColor) -> None:
    print(c.draw(), c.complex_method())


# -----------------------------------

from typing import Protocol, Tuple


class RGB(Protocol):
    rgb: Tuple[int, int, int]

    @abstractmethod
    def intensity(self) -> int:
        return 0


class Point(RGB):
    def __init__(self, red: int, green: int, blue: str) -> None:
        self.rgb = red, green, blue  # Error, 'blue' must be 'int' # type: ignore


print("Merging and extending protocols")
# https://peps.python.org/pep-0544/#merging-and-extending-protocols

from typing import Sized, Protocol

# FIXME: TypeError: multiple bases have instance lay-out conflict - CRASH
# Is this a MicroPython multiple inheritance limitation?
try:

    class SizedAndClosable_1(Sized, Protocol):
        def close(self) -> None:
            ...
except Exception as e:
    print("-[ ] FIXME: Difference or Crash - multiple bases have instance lay-out conflict:", e)


class SupportsClose_2(Protocol):
    def close(self) -> None:
        ...


# FIXME: TypeError: multiple bases have instance lay-out conflict - CRASH
try:

    class SizedAndClosable_2(Sized, SupportsClose_2, Protocol):
        pass
except Exception as e:
    print("-[ ] FIXME: Difference or Crash - multiple bases have instance lay-out conflict:", e)

print("Generic protocols")
# https://peps.python.org/pep-0544/#generic-protocols

# FIXME: Micropython does not support User Defined Generic Classes
# TypeError: 'type' object isn't subscriptable
# from typing import TypeVar, Protocol, Iterator
# T = TypeVar("T")
# class Iterable(Protocol[T]):
#     @abstractmethod
#     def __iter__(self) -> Iterator[T]: ...


print("Recursive protocols")

from typing import Generic, TypeVar

T = TypeVar("T")


class Traversable(Protocol):
    def leaves(self) -> Iterable["Traversable"]:
        ...


class SimpleTree:
    def leaves(self) -> List["SimpleTree"]:
        ...


root: Traversable = SimpleTree()  # OK


# FIXME: CPY_DIFF : Micropython does not support User Defined Generic Classes
# TypeError: 'type' object isn't subscriptable
# class Tree(Generic[T]):
#     def leaves(self) -> List["Tree[T]"]: ...
#
# def walk(graph: Traversable) -> None:
#     ...
# tree: Tree[float] = Tree()
# tree: Tree = Tree()
# walk(tree)  # OK, 'Tree[float]' is a subtype of 'Traversable'


print("Self-types in protocols")

C = TypeVar("C", bound="Copyable")  # type: ignore


class Copyable(Protocol):
    def copy(self: C) -> C:
        ...


class One:
    def copy(self) -> "One":
        ...


T = TypeVar("T", bound="Other")


class Other:
    def copy(self: T) -> T:
        ...


c: Copyable
c = One()  # OK # type: ignore
c = Other()  # Also OK # type: ignore


print("Callback protocols")

from typing import Optional, List, Protocol


class Combiner(Protocol):
    def __call__(self, *vals: bytes, maxlen: Optional[int] = None) -> List[bytes]:
        ...


def good_cb(*vals: bytes, maxlen: Optional[int] = None) -> List[bytes]:
    ...


def bad_cb(*vals: bytes, maxitems: Optional[int]) -> List[bytes]:
    ...


comb: Combiner = good_cb  # OK
comb = bad_cb  # Static Typecheck Error! # type: ignore
# Argument 2 has incompatible type because of different name and kind in the callback

print("Unions and intersections of protocols")

from typing import Union, Optional, Protocol


class Exitable(Protocol):
    def exit(self) -> int:
        ...


class Quittable(Protocol):
    def quit(self) -> Optional[int]:
        ...


def finish(task: Union[Exitable, Quittable]) -> int:
    ...


class DefaultJob:
    ...

    def quit(self) -> int:
        return 0


finish(DefaultJob())  # OK

# ---------------

from typing import Iterable, Hashable


# # class HashableFloats(Iterable[float], Hashable, Protocol):
# FIXME: TypeError: multiple bases have instance lay-out conflict
# class HashableFloats(Iterable, Hashable, Protocol):
#     pass
# def cached_func(args: HashableFloats) -> float: ...
# cached_func((1, 2, 3))  # OK, tuple is both hashable and iterable


print("Type[] and class objects vs protocols")
from typing import Type


class Proto(Protocol):
    @abstractmethod
    def meth(self) -> int:
        ...


class Concrete:
    def meth(self) -> int:
        return 42


def fun(cls: Type[Proto]) -> int:
    return cls().meth()  # OK


fun(Concrete)  # OK

# FIXME: Should Throw: Can't instantiate protocol with abstract methods -
# try:
#     fun(Proto)  # Error # type: ignore
#     print("-[ ] FIXME: Should Throw: Can't instantiate protocol with abstract methods")
# except Exception:
#     print("Expected: Can't instantiate protocol with abstract methods")

# ---------------

from typing import Any, Protocol


class ProtoA(Protocol):
    def meth(self, x: int) -> int:
        ...


class ProtoB(Protocol):
    def meth(self, obj: Any, x: int) -> int:
        ...


class C:
    def meth(self, x: int) -> int:
        ...


a: ProtoA = C  # Type check error, signatures don't match! # type: ignore
b: ProtoB = C  # OK # type: ignore


print("NewType() and type aliases")

from typing import NewType, Protocol, Iterator


class Id(Protocol):
    code: int
    secrets: Iterator[bytes]


UserId = NewType("UserId", Id)  # Error, can't provide distinct type # type: ignore

# -------------------------

from typing import TypeVar, Reversible, Iterable, Sized

# FIXME: cpy_diff : User Defined Generic Classes unsupported
# TypeError: 'type' object isn't subscriptable

# T = TypeVar("T")
# class SizedIterable_3(Iterable[T], Sized, Protocol):
#     pass
# CompatReversible = Union[Reversible[T], SizedIterable_3[T]]

print("@runtime_checkable decorator and narrowing types by isinstance()")


from typing import runtime_checkable, Protocol

# FIXME: cpy_diff : NotImplementedError: @runtime_checkable decorator unsupported
# @runtime_checkable
# class SupportsClose(Protocol):
#     def close(self): ...


# assert isinstance(open(__file__), SupportsClose)


class Foo(Protocol):
    @property
    def c(self) -> int:
        return 42  # Default value can be provided for property...


print("typing.Protocols")
# https://docs.python.org/3/library/typing.html#protocols

from typing import (
    SupportsInt,
    SupportsBytes,
    SupportsFloat,
    SupportsComplex,
    SupportsRound,
    SupportsAbs,
    SupportsIndex,
)
# TODO: what are sensible tests for these protocols?

print("-----")
