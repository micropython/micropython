# Python 3.7
# PEP 560 - Type Hinting Generics In Standard Collections

try:
    from typing import TYPE_CHECKING
except Exception:
    print("SKIP")
    raise SystemExit

import unittest


class TestPep560ClassGetItem(unittest.TestCase):
    # Specification: __class_getitem__ basic behavior on instances.
    def test_instance_getitem_on_user_class(self):
        class MyList:
            def __getitem__(self, index):
                return index + 1

            def __class_getitem__(cls, item):
                return f"{cls.__name__}[{item.__name__}]"

        class MyOtherList(MyList):
            pass

        self.assertEqual(MyList()[0], 1)

    # FIXME: Difference or Crash - __class_getitem__ not supported on user classes.
    @unittest.expectedFailure
    def test_class_getitem_on_user_class(self):
        class MyList:
            def __getitem__(self, index):
                return index + 1

            def __class_getitem__(cls, item):
                return f"{cls.__name__}[{item.__name__}]"

        class MyOtherList(MyList):
            pass

        # tests/extmod/typing_pep_0560.py", line 29, in <module>
        # TypeError: 'type' object isn't subscriptable
        self.assertEqual(MyList[int], "MyList[int]")
        self.assertEqual(MyOtherList()[0], 1)
        self.assertEqual(MyOtherList[int], "MyOtherList[int]")


class TestPep560MroEntries(unittest.TestCase):
    # __mro_entries__ allows GenericAlias-style bases. Subscription on user classes
    # is not supported in this runtime, so this case is expected to fail.
    @unittest.expectedFailure
    def test_generic_alias_mro_entries(self):
        class GenericAlias:
            def __init__(self, origin, item):
                self.origin = origin
                self.item = item

            def __mro_entries__(self, bases):
                return (self.origin,)

        class NewList:
            def __class_getitem__(cls, item):
                return GenericAlias(cls, item)

        # FIXME: Difference or Crash - __class_getitem__ not supported
        # TypeError: 'type' object isn't subscriptable
        class Tokens(NewList[int]): ...

        # Not sure these make sense to test
        # self.assertEqual(Tokens.__bases__, (NewList,))
        # self.assertEqual(Tokens.__orig_bases__, (NewList[int],))


if __name__ == "__main__":
    unittest.main()
