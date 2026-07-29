# probe module availability
# test import only

import collections
import sys
import unittest


try:
    from typing import TYPE_CHECKING
except Exception:
    print("SKIP")
    raise SystemExit


class TestModuleIncluded(unittest.TestCase):
    def test_typing(self):
        import typing

        self.assertIsNotNone(typing)

    def test_typing_extensions(self):
        try:
            import typing_extensions

            self.assertIsNotNone(typing_extensions)
        except ImportError:
            self.skipTest("module not available")

    def test_future(self):
        try:
            import __future__

            self.assertIsNotNone(__future__)
        except ImportError:
            self.skipTest("module not available")

    def test_abc(self):
        try:
            import abc

            self.assertIsNotNone(abc)
        except ImportError:
            self.skipTest("module not available")

    def test_collections(self):
        try:
            import collections

            self.assertIsNotNone(collections)
        except ImportError:
            self.skipTest("module not available")

    def test_collections_abc(self):
        try:
            import collections.abc

            self.assertIsNotNone(collections.abc)
        except ImportError:
            self.skipTest("module not available")


if __name__ == "__main__":
    unittest.main()
