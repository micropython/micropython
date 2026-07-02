# module abc
# runtime parity checks .

import sys
import unittest


try:
    import abc
except Exception:
    print("SKIP")
    raise SystemExit


class TestAbcRuntime(unittest.TestCase):
    # Basic abc helpers should be callable.

    @unittest.expectedFailure
    def test_abc_helper_functions(self):
        token = abc.get_cache_token()

        class C(abc.ABC):
            @abc.abstractmethod
            def f(self): ...

        cls = abc.update_abstractmethods(C)
        self.assertTrue(type(token) is int)
        self.assertTrue(cls is C)

    # Abstract class pattern with concrete implementations should execute.
    def test_abstract_class_usage(self):
        class Shape(abc.ABC):
            @abc.abstractmethod
            def get_area(self): ...

        class Square(Shape):
            def __init__(self, side):
                self.side = side

            def get_area(self):
                return self.side * self.side

        sq = Square(5)
        self.assertEqual(sq.get_area(), 25)

    @unittest.expectedFailure
    # TODO: cpydiff: metaclass=ABCMeta behavior differs between CPython and MicroPython.
    def test_abcmeta_metaclass_bot_supported(self):
        code = "class MyABC(metaclass=ABCMeta):\n    pass\n"

        ns = {"ABCMeta": abc.ABCMeta}
        exec(code, ns, ns)
        self.assertTrue("MyABC" in ns)


if __name__ == "__main__":
    unittest.main()
