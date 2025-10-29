try:
    from typing import TYPE_CHECKING
except Exception:
    print("SKIP")
    raise SystemExit

print("# Python 3.7")
print("### PEP 560 - Type Hinting Generics In Standard Collections")

print("Specification")


print("__class_getitem__")


class MyList:
    def __getitem__(self, index):
        return index + 1

    def __class_getitem__(cls, item):
        return f"{cls.__name__}[{item.__name__}]"


class MyOtherList(MyList):
    pass


assert MyList()[0] == 1

# FIXME: Difference or Crash - __class_getitem__ not supported
# tests/extmod/typing_pep_0560.py", line 29, in <module>
# TypeError: 'type' object isn't subscriptable
# assert MyList[int] == "MyList[int]"
# assert MyOtherList()[0] == 1
# assert MyOtherList[int] == "MyOtherList[int]"


# -------------------------


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
# class Tokens(NewList[int]): ...


# Not sure these make sense to test
# assert Tokens.__bases__ == (NewList,)
# assert Tokens.__orig_bases__ == (NewList[int],)

print("-----")
