# Python 3.0+
# module abc
# https://peps.python.org/pep-3119/
# https://docs.python.org/3/library/abc.html#module-abc

from math import e

try:
    import abc
except ImportError:
    print("SKIP")
    raise SystemExit

import unittest


class TestPep3119AbcBase(unittest.TestCase):
    # ABC base class should be importable and subclassable.
    def test_import_abc_and_subclass(self):
        try:
            from abc import ABC

            class MyABC(ABC):
                pass
        except Exception:
            # print("- [ ] FIXME: from abc import ABC")
            self.fail("from abc import ABC failed")

    # do not test : class MyABC(metaclass=ABCMeta): ...


class TestPep3119AbstractMethod(unittest.TestCase):
    # abstractmethod should be importable from abc.
    def test_import_abstractmethod(self):
        try:
            from abc import ABC, abstractmethod
        except Exception:
            # print("- [ ] FIXME: from abc import abstractmethod")
            self.fail("from abc import abstractmethod failed")

    # Defining a class using abstractmethod / classmethod / staticmethod / property.
    def test_define_abstract_class_with_decorators(self):
        from abc import ABC, abstractmethod

        class C1(ABC):
            @abstractmethod
            def my_abstract_method(self, arg1): ...

            @classmethod
            @abstractmethod
            def my_abstract_classmethod(cls, arg2): ...

            @staticmethod
            @abstractmethod
            def my_abstract_staticmethod(arg3): ...

            @property
            @abstractmethod
            def my_abstract_property(self): ...

            @my_abstract_property.setter
            @abstractmethod
            def my_abstract_property(self, val): ...

            @abstractmethod
            def _get_x(self): ...

            @abstractmethod
            def _set_x(self, val): ...

            x = property(_get_x, _set_x)

        # do not test deprecated abstractclassmethod, abstractstaticmethod, abstractproperty
        self.assertTrue(C1 is not None)


class TestPep3119AbcGetCacheToken(unittest.TestCase):
    # abc.get_cache_token is not present in the MicroPython abc module.
    def test_abc_get_cache_token(self):
        from abc import get_cache_token

        token = get_cache_token()
        self.assertTrue(token is not None or token is None)


if __name__ == "__main__":
    unittest.main()
