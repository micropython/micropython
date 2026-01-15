# Test the marshal module in combination with native/viper functions.

try:
    import marshal

    (lambda: 0).__code__
except (AttributeError, ImportError):
    print("SKIP")
    raise SystemExit

import unittest


def f_native():
    @micropython.native
    def g():
        pass

    return g


def f_viper():
    @micropython.viper
    def g():
        pass

    return g


class Test(unittest.TestCase):
    def test_native_function(self):
        # Can't marshal a function with native code.
        code = f_native.__code__
        with self.assertRaises(ValueError):
            marshal.dumps(code)

    def test_viper_function(self):
        # Can't marshal a function with viper code.
        code = f_viper.__code__
        with self.assertRaises(ValueError):
            marshal.dumps(code)


if __name__ == "__main__":
    unittest.main()
