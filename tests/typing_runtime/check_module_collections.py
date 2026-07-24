# collections module runtime checks.

try:
    import collections
except ImportError:
    print("SKIP")
    raise SystemExit

import sys
import unittest

try:
    from collections import MutableMapping as collections_py  # type: ignore
except ImportError:
    collections_py = None


class TestCollectionsRuntime(unittest.TestCase):
    def test_collections_C_module_symbols_exist(self):
        # collections should expose expected module-level symbols.
        expected = ("OrderedDict", "deque", "namedtuple")
        for name in expected:
            with self.subTest(name=name):
                self.assertTrue(hasattr(collections, name), "missing: {}".format(name))

    @unittest.skipIf(collections_py is None, "MutableMapping not implemented in this runtime")
    def test_collections_py_module_MutableMapping(self):
        # MutableMapping is implemented in a .py module that needs to be explicitly included in a firmware.
        expected = ("MutableMapping",)
        for name in expected:
            with self.subTest(name=name):
                self.assertTrue(hasattr(collections, name), "missing: {}".format(name))

    # namedtuple factory path should create tuple-like classes and instances.
    def test_namedtuple_factory_path(self):
        point_t = collections.namedtuple("Point", ("x", "y"))
        point = point_t(1, 2)

        self.assertEqual(point[0], 1)
        self.assertEqual(point[1], 2)

    # deque and OrderedDict basic operations should work.
    def test_deque_and_ordereddict_basic_ops(self):
        dq = collections.deque((), 8)
        dq.append(1)
        dq.append(2)
        self.assertEqual(dq.popleft(), 1)

        od = collections.OrderedDict()
        od["a"] = 1
        od["b"] = 2
        self.assertEqual(list(od.keys()), ["a", "b"])

    @unittest.expectedFailure
    # TODO: cpydiff namedtuple rename/defaults keyword args are not supported on MicroPython.
    # TypeError: function doesn't take keyword arguments
    def test_namedtuple_keyword_arguments_runtime_difference(self):
        nt = collections.namedtuple("NT", ["a", "a"], rename=True)
        self.assertEqual(nt._fields, ("a", "_1"))

        nt2 = collections.namedtuple("NT2", "a b c", defaults=(1, 2))
        self.assertEqual(nt2(1), nt2(a=1, b=1, c=2))


if __name__ == "__main__":
    unittest.main()
