# Additional typing spec coverage for runtime behavior and known unsupported parts.
# Spec index: https://typing.python.org/en/latest/spec/

try:
    import typing
except ImportError:
    print("SKIP")
    raise SystemExit

import sys
import unittest


class TestTypingUnsupportedRuntime(unittest.TestCase):
    # TODO: cpydiff: NewType has class-like runtime behavior in MicroPython vs callable object in CPython.
    @unittest.expectedFailure
    def test_newtype_class_semantics_runtime_difference(self):
        UserId = typing.NewType("UserId", int)
        value = UserId(7)
        self.assertFalse(type(UserId) is type)
        # CPython : type(UserId) -> <class 'typing.NewType'>
        # MicroPython : type(UserId) -> <class 'type'>


class TestPython312Syntax(unittest.TestCase):
    # Python 3.12 type statement not supported in MicroPython
    @unittest.expectedFailure
    def test_type_statement(self):
        code = "type Point = tuple[float, float]\n"
        ns = {}
        exec(code, ns, ns)

    # Python 3.12 generic function syntax not supported in MicroPython
    @unittest.expectedFailure
    def test_type_parameter_syntax(self):
        code = (
            "from collections.abc import Sequence\n"
            "def first[T](l: Sequence[T]) -> T:\n"
            "    return l[0]\n"
        )
        ns = {}
        exec(code, ns, ns)
        self.assertEqual(ns["first"]([1, 2, 3]), 1)


if __name__ == "__main__":
    unittest.main()
