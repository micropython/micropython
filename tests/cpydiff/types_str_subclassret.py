"""
categories: Types,str
description: str() does not preserve str subclass type from ``__str__`` or ``__repr__`` return value
cause: Implementation discards str subclass type returned by ``__str__`` or ``__repr__`` and always returns a plain str instance from str().
workaround: Do not rely on str() preserving str subclass types
"""


class MyStr(str):
    pass


class Foo:
    def __str__(self):
        return MyStr("abc")


result = str(Foo())
print(type(result))
