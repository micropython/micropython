# Test the marshal module, MicroPython-specific functionality.

try:
    import marshal
except ImportError:
    print("SKIP")
    raise SystemExit

import unittest


class Test(unittest.TestCase):
    def test_function_with_children(self):
        # Can't marshal a function with children (in this case the module has a child function f).
        code = compile("def f(): pass", "", "exec")
        with self.assertRaises(ValueError):
            marshal.dumps(code)


if __name__ == "__main__":
    unittest.main()
