# Protocol runtime parity checks from the notebook scenarios.

try:
    from typing import Callable, Protocol, Self
except ImportError:
    print("SKIP")
    raise SystemExit

import unittest


class TestTypingProtocolRuntime(unittest.TestCase):
    # Protocol-typed argument should accept objects with a compatible method.
    def test_protocol_adder_usage(self):
        class Adder(Protocol):
            def add(self, x, y): ...

        class IntAdder:
            def add(self, x, y):
                return x + y

        def use_adder(adder: Adder):
            return adder.add(2, 3)

        self.assertEqual(use_adder(IntAdder()), 5)

    # Protocol callback shape using Self should execute without runtime typing checks.
    def test_protocol_self_callback_usage(self):
        class BaseClass:
            def register(self, callback: Callable[[Self], None]) -> None:
                callback(self)

        called = {"ok": False}

        def cb(x):
            called["ok"] = x is not None

        base = BaseClass()
        base.register(cb)
        self.assertTrue(called["ok"])


if __name__ == "__main__":
    unittest.main()
