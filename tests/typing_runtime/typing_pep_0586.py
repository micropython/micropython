# Python 3.8
# PEP 586 - Literal Types
# https://peps.python.org/pep-0586


try:
    from __future__ import annotations  # type: ignore
except Exception:
    print("SKIP")
    raise SystemExit

import unittest

from typing import Literal, Optional, TypeAlias
from typing import Tuple, List
from typing import overload, IO, Any, Union


class TestPep586LegalParameters(unittest.TestCase):
    # Legal Literal parameters at type-check time also accept at runtime.
    def test_literal_parameter_forms_runtime(self):
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

    def test_literal_alias_grouping(self):
        ReadOnlyMode = Literal["r", "r+"]
        WriteAndTruncateMode = Literal["w", "w+", "wt", "w+t"]
        WriteNoTruncateMode = Literal["r+", "r+t"]
        AppendMode = Literal["a", "a+", "at", "a+t"]

        AllModes = Literal[ReadOnlyMode, WriteAndTruncateMode, WriteNoTruncateMode, AppendMode]

    def test_literal_nested_subscription(self):
        try:
            Literal[Literal[Literal[1, 2, 3], "foo"], 5, None]
            Optional[Literal[1, 2, 3, "foo", 5]]
        except TypeError:
            #  TypeError: 'type' object isn't subscriptable for nested Literal use
            assert False, "nested Literal subscription should be supported"


class TestPep586ParametersAtRuntime(unittest.TestCase):
    # Functions annotated with Literal still execute normally at runtime.
    def test_literal_annotated_function_runtime(self):
        def my_function(x: Literal[1, 2]) -> int:
            return x * 3

        self.assertEqual(my_function(1), 3)

    # Variable annotated with Literal[...] of a function value is runtime-permissive.
    def test_literal_function_value_assignment(self):
        def my_function(x: Literal[1, 2]) -> int:
            return x * 3

        x: Literal[1, 2, 3] = 3
        y: Literal[my_function] = my_function  # type: ignore
        self.assertEqual(x, 3)
        self.assertTrue(y is my_function)


class TestPep586NonLiteralsInLiteralContexts(unittest.TestCase):
    # Literal["foo"] is a subtype of str and accepted by str-typed callee.
    def test_literal_str_passthrough_runtime(self):
        def expects_str(x: str) -> None:
            return None

        var: Literal["foo"] = "foo"
        # Legal: Literal["foo"] is a subtype of str
        expects_str(var)

    # Passing a regular str to a Literal-typed function still works at runtime.
    def test_runner_with_arbitrary_string(self):
        def expects_literal(x: Literal["foo"]) -> None:
            return None

        def runner(my_str: str) -> None:
            expects_literal(my_str)  # type: ignore

        runner("foo")  # type: ignore


class TestPep586IntelligentIndexing(unittest.TestCase):
    # Tuple indexing with Literal-typed indices works at runtime.
    def test_tuple_indexed_by_literal_runtime(self):
        a: Literal[0] = 0
        b: Literal[5] = 5

        some_tuple: Tuple[int, str, List[bool]] = (3, "abc", [True, False])

        # FIXME: NameError: name 'reveal_type' isn't defined
        # reveal_type(some_tuple[a])  # Revealed type is 'int'

        with self.assertRaises(IndexError):
            some_tuple[b]  # Error: 5 is not a valid index into the tuple # type: ignore

    # getattr with Literal-typed key on a custom class.
    def test_getattr_literal_keys_on_class(self):
        class Test:
            def __init__(self, param: int) -> None:
                self.myfield = param

            def mymethod(self, val: int) -> str: ...

        a: Literal["myfield"] = "myfield"
        b: Literal["mymethod"] = "mymethod"
        c: Literal["blah"] = "blah"

        t = Test(24)
        # reveal_type(getattr(t, a))  # Revealed type is 'int'
        # reveal_type(getattr(t, b))  # Revealed type is 'Callable[[int], str]'

        with self.assertRaises(AttributeError):
            getattr(t, c)


# FIXME: TypeError: 'type' object isn't subscriptable
# _PathType = Union[str, bytes, int]
_PathType: TypeAlias = str


class TestPep586OverloadsInteraction(unittest.TestCase):
    # @overload declarations with Literal modes parse and stack at runtime.
    def test_overload_with_literal_modes(self):
        @overload
        def open(
            path: _PathType,
            mode: Literal["r", "w", "a", "x", "r+", "w+", "a+", "x+"],
        ) -> IO[str]: ...

        @overload
        def open(
            path: _PathType,
            mode: Literal["rb", "wb", "ab", "xb", "r+b", "w+b", "a+b", "x+b"],
        ) -> IO[bytes]: ...

        # Fallback overload for when the user isn't using literal types
        @overload
        def open(path: _PathType, mode: str) -> IO[Any]:
            pass


class TestPep586GenericsInteraction(unittest.TestCase):
    # FIXME: User Defined Generic Classes unsupported in MicroPython runtime.
    @unittest.expectedFailure
    def test_generic_matrix_class_with_literal(self):
        from typing import Generic, TypeVar

        A = TypeVar("A", bound=int)
        B = TypeVar("B", bound=int)
        C = TypeVar("C", bound=int)

        # requires from __futures__ import annotations
        # A simplified definition for Matrix[row, column]
        # TypeError: 'type' object isn't subscriptable
        class Matrix(Generic[A, B]):
            def __add__(self, other: Matrix[A, B]) -> Matrix[A, B]: ...

            def __matmul__(self, other: Matrix[B, C]) -> Matrix[A, C]: ...

            def transpose(self) -> Matrix[B, A]: ...

        foo: Matrix[Literal[2], Literal[3]] = Matrix()
        bar: Matrix[Literal[3], Literal[7]] = Matrix()

        baz = foo @ bar
        # reveal_type(baz)  # Revealed type is 'Matrix[Literal[2], Literal[7]]'


class TestPep586EnumsAndExhaustiveness(unittest.TestCase):
    # FIXME: enum module not standard in MicroPython.
    @unittest.expectedFailure
    def test_status_enum_exhaustiveness(self):
        from enum import Enum

        class Status(Enum):
            SUCCESS = 0
            INVALID_DATA = 1
            FATAL_ERROR = 2

        def parse_status(s: Union[str, Status]) -> None:
            if s is Status.SUCCESS:
                return None
            elif s is Status.INVALID_DATA:
                return None
            elif s is Status.FATAL_ERROR:
                return None
            else:
                # 's' must be of type 'str' since all other options are exhausted
                return None

        parse_status(Status.SUCCESS)


if __name__ == "__main__":
    unittest.main()
