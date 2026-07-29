# Python 3.5
# PEP 484
# https://peps.python.org/topic/typing/
# https://peps.python.org/pep-0484/
#
# Currently excludes tests using `Generic[T]` due to MicroPython runtime limitations

try:
    from typing import TYPE_CHECKING
except ImportError:
    print("SKIP")
    raise SystemExit

import unittest

from typing import List
from typing import Callable, TypeVar, Union
from typing import Generic, TypeVar
from typing import NewType
from typing import NoReturn
from typing import overload
from typing import cast


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


class TestPep484TypeDefinitionSyntax(unittest.TestCase):
    # Function annotations are accepted at definition and call time.
    def test_function_annotation_runtime(self):
        def greeting(name: str) -> str:
            return "Hello " + name

        self.assertEqual(greeting("world"), "Hello world")

    # Variable annotations without assignment are runtime no-ops.
    def test_list_int_annotation_only(self):
        numberlist: List[int]


class TestPep484TypeAliases(unittest.TestCase):
    # Simple type aliases reduce to their target type at runtime.
    def test_simple_alias_runtime_path(self):
        Url = str

        def retry(url: Url, retry_count: int) -> None:
            return None

        self.assertIsNone(retry("http://example", 3))


class TestPep484Callable(unittest.TestCase):
    # Callable annotations should not affect runtime callable invocation.
    def test_callable_annotation_runtime(self):
        def feeder(get_next_item: Callable[[], str]) -> str:
            return get_next_item()

        def get_const():
            return "x"

        self.assertEqual(feeder(get_const), "x")


class TestPep484TypeVar(unittest.TestCase):
    # Constrained TypeVar usage should not break runtime function execution.
    def test_constrained_typevar_runtime(self):
        AnyStr = TypeVar("AnyStr", str, bytes)

        def concat(x: AnyStr, y: AnyStr) -> AnyStr:
            return x + y

        self.assertEqual(concat("a", "b"), "ab")


class TestPep484GenericUserClass(unittest.TestCase):
    # TODO: Crash - inheriting from typing.Generic[T] unsupported at runtime
    # (somehow does work in MicroPython .py typing variant).
    @xfail_on_error
    def test_generic_t_base_class(self):
        T = TypeVar("T")

        class LoggedVar(Generic[T]):
            def __init__(self, value: T, name: str) -> None:
                self.name = name
                self.value = value

            def set(self, new: T) -> None:
                self.value = new

            def get(self) -> T:
                return self.value

        lv = LoggedVar(1, "x")
        lv.set(2)
        self.assertEqual(lv.get(), 2)


class TestPep484UnionOptional(unittest.TestCase):
    # Union/Optional annotations should accept str and None at runtime.
    def test_union_str_none_runtime(self):
        def handle_employee(e: Union[str, None]) -> Union[str, None]:
            return e

        self.assertEqual(handle_employee("John"), "John")
        self.assertIsNone(handle_employee(None))


class TestPep484AnyAnnotation(unittest.TestCase):
    # Any annotation is permissive at runtime.
    def test_any_dict_annotation_runtime(self):
        def use_map(m: dict) -> list:
            return list(m.keys())

        self.assertEqual(use_map({"a": 1}), ["a"])


class TestPep484NewType(unittest.TestCase):
    # NewType returns identity-like behavior at runtime.
    def test_newtype_user_id_runtime(self):
        UserId = NewType("UserId", int)
        v = UserId(5)
        self.assertEqual(v, 5)
        self.assertTrue(isinstance(v, int))


class TestPep484TypeCheckingGuard(unittest.TestCase):
    # The TYPE_CHECKING constant is False at runtime, so guarded blocks do not run.
    def test_type_checking_runtime_false(self):
        if TYPE_CHECKING:
            # This block is for type checkers only
            self.fail("typing.TYPE_CHECKING is True at runtime. ERROR")
        else:
            self.assertFalse(TYPE_CHECKING)


class TestPep484ForwardReference(unittest.TestCase):
    # Quoted forward references in annotations are not evaluated at runtime.
    def test_forward_reference_in_method_annotation(self):
        class Tree:
            def __init__(self, left: "Tree" = None, right: "Tree" = None):  # type: ignore
                self.left = left
                self.right = right

        tr = Tree()
        self.assertIsNone(tr.left)
        self.assertIsNone(tr.right)


class TestPep484NoReturn(unittest.TestCase):
    # NoReturn annotation is documentation-only; runtime raising still works.
    def test_noreturn_raises_runtime_error(self):
        def stop() -> NoReturn:
            raise RuntimeError("stop")

        with self.assertRaises(RuntimeError):
            stop()


class TestPep484Overload(unittest.TestCase):
    # @overload declarations are typing-only; the concrete implementation runs.
    def test_overload_runtime_dispatch_to_impl(self):
        @overload
        def func(x: int) -> int: ...

        @overload
        def func(x: str) -> str: ...

        def func(x):
            return x

        self.assertEqual(func(1), 1)


class TestPep484Cast(unittest.TestCase):
    # cast is an identity function at runtime, ie does not change the value.
    def test_cast_runtime_identity(self):
        foo = cast(str, 123)
        self.assertEqual(foo, 123)
        self.assertEqual(cast(str, 456), 456)


if __name__ == "__main__":
    unittest.main()
