# typing_extensions runtime parity checks from notebook scenarios.

import sys
import unittest

try:
    import typing
    import typing_extensions
except ImportError:
    print("SKIP")
    raise SystemExit


class TestTypingExtensionsRuntime(unittest.TestCase):
    # TYPE_CHECKING should be present and boolean.
    def test_type_checking_bool(self):
        if typing_extensions is None:
            return

        self.assertTrue(hasattr(typing_extensions, "TYPE_CHECKING"))
        self.assertTrue(type(typing_extensions.TYPE_CHECKING) is bool)

    # Self annotation path should execute and return self.
    def test_self_annotation_runtime_path(self):
        if typing_extensions is None:
            return

        if not hasattr(typing_extensions, "Self"):
            return

        class Foo:
            def return_self(self) -> typing_extensions.Self:
                return self

        foo = Foo()
        self.assertTrue(foo.return_self() is foo)

    # Generator annotation path should execute and produce expected first value.
    def test_generator_annotation_runtime_path(self):
        if typing_extensions is None:
            return

        if not hasattr(typing_extensions, "Generator"):
            return

        def echo_round() -> typing_extensions.Generator[int, float, str]:
            sent = yield 0
            while sent >= 0:
                sent = yield round(sent)
            return "Done"

        g = echo_round()
        self.assertEqual(next(g), 0)

    # reveal_type should be callable if provided.
    def test_reveal_type_runtime_path(self):
        if typing_extensions is None:
            return

        if not hasattr(typing_extensions, "reveal_type"):
            return

        value = 42
        revealed = typing_extensions.reveal_type(value)
        self.assertEqual(revealed, value)

    # TypeVarTuple and Unpack are optional in this runtime setup.
    def test_typevar_tuple_symbols_optional(self):
        Ts = typing_extensions.TypeVarTuple("Ts")
        self.assertTrue(Ts is not None)

        self.assertTrue(typing_extensions.Unpack is not None)

    # TypeVar should be importable from typing_extensions in this runtime setup.
    def test_typevar_runtime_path(self):
        T = typing_extensions.TypeVar("T")
        self.assertTrue(T is not None)

    # reveal_type may be absent on MicroPython and is tracked explicitly.
    def test_reveal_type_runtime_difference(self):
        self.assertTrue(hasattr(typing_extensions, "reveal_type"))


if __name__ == "__main__":
    unittest.main()
