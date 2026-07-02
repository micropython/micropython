# Python 3.8
# PEP 544 - Protocols: Structural subtyping (static duck typing)
# https://peps.python.org/topic/typing/
# https://peps.python.org/pep-0544/
#
# Currently xfails tests using `Generic[T]` due to MicroPython runtime limitations

try:
    from typing import TYPE_CHECKING
    from abc import abstractmethod
except Exception:
    print("SKIP")
    raise SystemExit

import unittest

from typing import Protocol, Iterable
from typing import List
from typing import Tuple
from typing import Sized
from typing import TypeVar
from typing import Optional
from typing import Union
from typing import Hashable
from typing import Type
from typing import Any
from typing import NewType, Iterator
from typing import Reversible


from typing import (
    SupportsInt,
    SupportsBytes,
    SupportsFloat,
    SupportsComplex,
    SupportsRound,
    SupportsAbs,
    SupportsIndex,
)


def xfail_on_error(func):
    """Report test as skipped ("xfail: ...") if it raises, or as ok if it
    passes. Use instead of @unittest.expectedFailure when an unexpected pass
    should NOT be reported as a failure (xpass)."""

    def wrapper(self):
        try:
            func(self)
        except Exception as e:
            raise unittest.SkipTest("xfail: {}".format(e))

    return wrapper


class TestPep544DefiningAProtocol(unittest.TestCase):
    # A simple Protocol with a close() method should declare and execute.
    def test_supports_close_protocol(self):
        class SupportsClose(Protocol):
            def close(self) -> None: ...

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


class TestPep544ProtocolMembers(unittest.TestCase):
    # Protocol with default and abstract methods declares without error.
    def test_protocol_with_default_and_abstract(self):
        class Example(Protocol):
            def first(self) -> int:  # This is a protocol member
                return 42

            @abstractmethod
            def second(self) -> int:  # Method without a default implementation
                raise NotImplementedError

    # Protocol with class-level attribute and method definitions.
    def test_protocol_with_attributes(self):
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
        self.assertEqual(var.name, "value")


class TestPep544ExplicitlyDeclaringImplementation(unittest.TestCase):
    # Subclassing a Protocol explicitly and providing partial implementations.
    def test_explicit_protocol_subclass(self):
        class PColor(Protocol):
            @abstractmethod
            def draw(self) -> str: ...

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

    # Protocol that requires a tuple attribute and an abstract method.
    def test_rgb_protocol(self):
        class RGB(Protocol):
            rgb: Tuple[int, int, int]

            @abstractmethod
            def intensity(self) -> int:
                return 0

        class Point(RGB):
            def __init__(self, red: int, green: int, blue: str) -> None:
                self.rgb = red, green, blue  # Error, 'blue' must be 'int' # type: ignore


class TestPep544MergingAndExtending(unittest.TestCase):
    # FIXME: TypeError: multiple bases have instance lay-out conflict - CRASH
    # (now succeeds in this MicroPython typing variant).
    def test_sized_and_closable_with_protocol(self):
        try:

            class SizedAndClosable_1(Sized, Protocol):
                def close(self) -> None: ...
        except Exception:
            assert False, "Sized Protocols unsupported"

    # FIXME: TypeError: multiple bases have instance lay-out conflict - CRASH
    # (now succeeds in this MicroPython typing variant).
    def test_sized_and_closable_inheriting_protocols(self):
        try:

            class SupportsClose_2(Protocol):
                def close(self) -> None: ...

            class SizedAndClosable_2(Sized, SupportsClose_2, Protocol):
                pass
        except Exception:
            assert False, "inheriting Protocolsunsupported"


class TestPep544GenericProtocols(unittest.TestCase):
    # FIXME: MicroPython does not support User Defined Generic Classes.
    # TypeError: 'type' object isn't subscriptable.
    def test_generic_protocol(self):
        try:
            T = TypeVar("T")

            class IterableP(Protocol[T]):
                @abstractmethod
                def __iter__(self) -> Iterator[T]: ...
        except Exception:
            assert False, "User Defined Generic Classes unsupported"


class TestPep544RecursiveProtocols(unittest.TestCase):
    # Recursive Protocol references itself by forward reference.
    def test_recursive_traversable(self):
        T = TypeVar("T")

        class Traversable(Protocol):
            def leaves(self) -> Iterable["Traversable"]: ...

        class SimpleTree:
            def leaves(self) -> List["SimpleTree"]: ...

        root: Traversable = SimpleTree()  # OK
        self.assertTrue(isinstance(root, SimpleTree))

        # FIXME: cpydiff : MicroPython does not support User Defined Generic Classes
        # TypeError: 'type' object isn't subscriptable
        # class Tree(Generic[T]):
        #     def leaves(self) -> List["Tree[T]"]: ...
        #
        # def walk(graph: Traversable) -> None:
        #     ...
        # tree: Tree[float] = Tree()
        # tree: Tree = Tree()
        # walk(tree)  # OK, 'Tree[float]' is a subtype of 'Traversable'


class TestPep544SelfTypesInProtocols(unittest.TestCase):
    # Protocol using a bound TypeVar for self-typed methods.
    def test_copyable_self_type(self):
        C = TypeVar("C", bound="Copyable")  # type: ignore

        class Copyable(Protocol):
            def copy(self: C) -> C: ...

        class One:
            def copy(self) -> "One": ...

        T = TypeVar("T", bound="Other")

        class Other:
            def copy(self: T) -> T: ...

        c: Copyable
        c = One()  # OK # type: ignore
        c = Other()  # Also OK # type: ignore


class TestPep544CallbackProtocols(unittest.TestCase):
    # Protocol with __call__ signature accepts compatible callables.
    def test_combiner_callback_protocol(self):
        class Combiner(Protocol):
            def __call__(self, *vals: bytes, maxlen: Optional[int] = None) -> List[bytes]: ...

        def good_cb(*vals: bytes, maxlen: Optional[int] = None) -> List[bytes]: ...

        def bad_cb(*vals: bytes, maxitems: Optional[int]) -> List[bytes]: ...

        comb: Combiner = good_cb  # OK
        comb = bad_cb  # Static Typecheck Error! # type: ignore
        # Argument 2 has incompatible type because of different name and kind in the callback


class TestPep544UnionsAndIntersections(unittest.TestCase):
    # Union of two protocols accepts a class satisfying one of them.
    def test_finish_union_of_protocols(self):
        class Exitable(Protocol):
            def exit(self) -> int: ...

        class Quittable(Protocol):
            def quit(self) -> Optional[int]: ...

        def finish(task: Union[Exitable, Quittable]) -> int: ...

        class DefaultJob:
            ...

            def quit(self) -> int:
                return 0

        finish(DefaultJob())  # OK

    # FIXME: TypeError: multiple bases have instance lay-out conflict
    # (now succeeds in this MicroPython typing variant).
    def test_hashable_floats_intersection(self):
        # # class HashableFloats(Iterable[float], Hashable, Protocol):
        # FIXME: TypeError: multiple bases have instance lay-out conflict
        try:

            class HashableFloats(Iterable, Hashable, Protocol):
                pass
        except TypeError:
            assert False, "multiple bases have instance lay-out conflict"

        def cached_func(args: HashableFloats) -> float: ...

        cached_func((1, 2, 3))  # OK, tuple is both hashable and iterable


class TestPep544TypeAndClassObjectsVsProtocols(unittest.TestCase):
    # Type[Protocol] accepts concrete subclass references.
    def test_type_of_protocol_with_concrete(self):
        class Proto(Protocol):
            @abstractmethod
            def meth(self) -> int: ...

        class Concrete:
            def meth(self) -> int:
                return 42

        def fun(cls: Type[Proto]) -> int:
            return cls().meth()  # OK

        fun(Concrete)  # OK

    # FIXME: Should Throw: Can't instantiate protocol with abstract methods.
    @unittest.expectedFailure
    def test_instantiate_abstract_protocol_raises(self):
        class Proto(Protocol):
            @abstractmethod
            def meth(self) -> int: ...

        def fun(cls: Type[Proto]) -> int:
            return cls().meth()

        # try:
        fun(Proto)  # Error # type: ignore
        # print("- [ ] FIXME: Should Throw: Can't instantiate protocol with abstract methods")
        # except Exception:
        #     print("Expected: Can't instantiate protocol with abstract methods")
        raise AssertionError("Should not reach here")

    # Class object vs protocol assignments parse at runtime.
    def test_class_object_vs_protocol_assignment(self):
        class ProtoA(Protocol):
            def meth(self, x: int) -> int: ...

        class ProtoB(Protocol):
            def meth(self, obj: Any, x: int) -> int: ...

        class C:
            def meth(self, x: int) -> int: ...

        a: ProtoA = C  # Type check error, signatures don't match! # type: ignore
        b: ProtoB = C  # OK # type: ignore


class TestPep544NewTypeAndAliases(unittest.TestCase):
    # NewType built on top of a Protocol parses at runtime.
    def test_newtype_over_protocol(self):
        class Id(Protocol):
            code: int
            secrets: Iterator[bytes]

        UserId = NewType("UserId", Id)  # type: ignore

    def test_sized_iterable_generic_protocol(self):
        T = TypeVar("T", covariant=True)

        class SizedIterable_3(Iterable[T], Sized, Protocol):
            pass

        CompatReversible = Union[Reversible[T], SizedIterable_3[T]]


class TestPep544RuntimeCheckable(unittest.TestCase):
    # FIXME: cpy_diff : NotImplementedError: @runtime_checkable decorator unsupported.
    @unittest.expectedFailure
    def test_runtime_checkable_supports_close(self):
        from typing import runtime_checkable

        @runtime_checkable
        class SupportsCloseRC(Protocol):
            def close(self): ...

        assert isinstance(open(__file__), SupportsCloseRC)

    # Protocol with property default value parses at runtime.
    def test_protocol_with_property_default(self):
        class Foo(Protocol):
            @property
            def c(self) -> int:
                return 42  # Default value can be provided for property...


class TestPep544TypingProtocols(unittest.TestCase):
    # https://docs.python.org/3/library/typing.html#protocols
    # The standard Supports* protocols should be importable.
    def test_supports_protocols_importable(self):
        # TODO: what are sensible tests for these protocols?
        for p in (
            SupportsInt,
            SupportsBytes,
            SupportsFloat,
            SupportsComplex,
            SupportsRound,
            SupportsAbs,
            SupportsIndex,
        ):
            self.assertTrue(p is not None)


if __name__ == "__main__":
    unittest.main()
