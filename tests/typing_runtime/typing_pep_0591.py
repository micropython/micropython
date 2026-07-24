# Python 3.8
# PEP 0591 - Final qualifier for types
# https://peps.python.org/pep-0591/

try:
    from typing import TYPE_CHECKING
except Exception:
    print("SKIP")
    raise SystemExit

import unittest

from typing import List, Sequence
from typing import Final
from typing import NamedTuple

# FIXME: typing.final not available in MicroPython; use a no-op fallback.
try:
    from typing import final
except ImportError:

    def final(cls):
        return cls


class TestFinalDecorator(unittest.TestCase):
    # The final decorator can be applied to a class.
    def test_final_decorator_on_class(self):
        @final
        class Base_1: ...

        self.assertTrue(Base_1 is not None)

    # In CPython, inheriting from a @final-decorated class is rejected by the
    # type checker but not enforced at runtime; on MicroPython runtime it is
    # also not enforced. Test that subclassing runs.
    def test_inherit_from_final_class_runtime(self):
        @final
        class Base_1: ...

        try:

            class Derived_1(Base_1):  # Error: Cannot inherit from final class "Base"
                ...
        except Exception:
            # print("- [ ] FIXME: Cannot inherit from final class 'Base'")
            self.fail("subclassing a @final class should not raise at runtime")

    # The @final decorator on methods does not enforce override prevention at runtime.
    def test_final_decorator_on_method(self):
        class Base_2:
            @final
            def foo(self) -> None: ...

        try:

            class Derived_2(Base_2):
                def foo(self) -> None:  # Error: Cannot override final attribute "foo"
                    # (previously declared in base class "Base")
                    ...
        except Exception:
            # print("Expected: Cannot override final attribute 'foo'")
            self.fail("overriding a @final method should not raise at runtime")


class TestFinalAnnotation(unittest.TestCase):
    # The Final annotation should accept type parameterization at module scope.
    def test_final_annotation_with_value(self):
        ID_1: Final[float] = 1
        ID_2: Final = 2

        self.assertEqual(ID_1, 1)
        self.assertEqual(ID_2, 2)


class TestFinalSemantics(unittest.TestCase):
    # Final is a typing-only marker; reassignment is not prevented at runtime.
    def test_final_module_reassignment_runtime(self):
        RATE: Final = 3000
        try:
            RATE = 300  # Error: can't assign to final attribute
        except Exception:
            # print("Expected: can't assign to final attribute 'RATE'")
            self.fail("Reassignment of Final value should not raise at runtime")

    # Class-level Final attributes are not enforced at runtime either.
    def test_final_class_attribute_assignment_runtime(self):
        class Base:
            DEFAULT_ID: Final = 0

        try:
            Base.DEFAULT_ID = 1  # Error: can't override a final attribute
        except Exception:
            # print("Expected: can't override a final attribute 'DEFAULT_ID'")
            self.fail("Reassignment of class-level Final attribute should not raise at runtime")


class TestFinalWithContainers(unittest.TestCase):
    # FIXME: document cpydiff - Final[ContainerType] is a typing-only construct
    # and is not validated at runtime.
    def test_final_with_list_container_runtime(self):
        try:
            x: List[Final[int]] = []  # Error!
            # print("- [ ] FIXME: document cpydiff : Final cannot be used with container types")
        except Exception:
            # print("Expected: Final cannot be used with container types")
            self.fail("Final[int] inside List should not raise at runtime")

    # Final[List[int]] as parameter annotation should not error at runtime.
    def test_final_list_param_runtime(self):
        try:

            def fun(x: Final[List[int]]) -> None:  # Error!
                ...
        except Exception:
            # print("Expected: Final cannot be used for parameters or return types")
            self.fail("Final[List[int]] as parameter annotation should not raise at runtime")

    # Mutable containers are not made immutable by Final at runtime.
    def test_final_mutable_container_remains_mutable(self):
        x_2: Final = ["a", "b"]
        x_2.append("c")  # OK
        self.assertEqual(x_2, ["a", "b", "c"])

        y: Final[Sequence[str]] = ["a", "b"]
        z: Final = ("a", "b")  # Also works
        self.assertEqual(list(y), ["a", "b"])
        self.assertEqual(z, ("a", "b"))


class TestFinalWithNamedTuple(unittest.TestCase):
    # NamedTuple factory call with Final-typed field names. Runtime support for
    # typing.NamedTuple factory differs on MicroPython.
    @unittest.expectedFailure
    def test_namedtuple_with_final_field_names(self):
        X: Final = "x"
        Y: Final = "y"
        N = NamedTuple("N", [(X, int), (Y, int)])
        self.assertTrue(N is not None)


if __name__ == "__main__":
    unittest.main()
