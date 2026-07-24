# Tests for the MicroPython __future__ module runtime flags.

try:
    import __future__ as future_mod
except ImportError:
    print("SKIP")
    raise SystemExit


import unittest


class TestFutureModule(unittest.TestCase):
    # __future__ exports known feature-switch names as module attributes.
    def test_expected_feature_flags_exist(self):
        expected_names = (
            "nested_scopes",
            "generators",
            "division",
            "absolute_import",
            "with_statement",
            "print_function",
            "unicode_literals",
            "annotations",
        )

        for name in expected_names:
            with self.subTest(name=name):
                self.assertTrue(hasattr(future_mod, name), "missing: {}".format(name))

    # In this runtime module, each known flag is a bool set to True.
    def test_feature_flags_are_true_booleans(self):
        expected_names = (
            "nested_scopes",
            "generators",
            "division",
            "absolute_import",
            "with_statement",
            "print_function",
            "unicode_literals",
            "annotations",
        )

        for name in expected_names:
            with self.subTest(name=name):
                value = getattr(future_mod, name)
                self.assertTrue(type(value) is bool)
                self.assertTrue(value)

    # Guard against accidental symbol leakage in the module namespace.
    def test_random_symbols_are_not_present(self):
        self.assertFalse(hasattr(future_mod, "FooBas"))
        self.assertFalse(hasattr(future_mod, "SNAFU"))

    # __future__.annotations and typing.cast should not enforce runtime conversion.
    def test_future_annotations_with_cast_runtime_behavior(self):
        ns = {}
        code = (
            "from __future__ import annotations\n"
            "from typing import cast\n"
            "x = 1\n"
            "y = cast(str, x)\n"
            "ok = False\n"
            "try:\n"
            "    y.upper()\n"
            "except AttributeError:\n"
            "    ok = True\n"
        )
        exec(code, ns, ns)
        self.assertTrue(ns["ok"])


if __name__ == "__main__":
    unittest.main()
