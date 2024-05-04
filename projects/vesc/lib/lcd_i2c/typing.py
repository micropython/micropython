#!/usr/bin/env python3
# -*- coding: UTF-8 -*-

"""
Fake classes of typing module.

https://github.com/micropython/micropython-lib/issues/190

https://github.com/micropython/micropython-lib/blob/3d779b8ceab5b65b9f70accbcbb15ab3509eceb7/typing/typing.py
"""


class _Subscriptable():
    def __getitem__(self, item):
        return None


_subscriptable = _Subscriptable()


class Any:
    pass


class NoReturn:
    pass


class ClassVar:
    pass


Union = _subscriptable


Optional = _subscriptable


class Generic:
    pass


class NamedTuple:
    pass


class Hashable:
    pass


class Awaitable:
    pass


class Coroutine:
    pass


class AsyncIterable:
    pass


class AsyncIterator:
    pass


class Iterable:
    pass


class Iterator:
    pass


class Reversible:
    pass


class Sized:
    pass


class Container:
    pass


class Collection:
    pass


# class Callable:
#     pass
Callable = _subscriptable


class AbstractSet:
    pass


class MutableSet:
    pass


class Mapping:
    pass


class MutableMapping:
    pass


class Sequence:
    pass


class MutableSequence:
    pass


class ByteString:
    pass


Tuple = _subscriptable


List = _subscriptable


class Deque:
    pass


class Set:
    pass


class dict_keys:
    pass


class FrozenSet:
    pass


class MappingView:
    pass


class KeysView:
    pass


class ItemsView:
    pass


class ValuesView:
    pass


class ContextManager:
    pass


class AsyncContextManager:
    pass


Dict = _subscriptable


class DefaultDict:
    pass


class Counter:
    pass


class ChainMap:
    pass


class Generator:
    pass


class AsyncGenerator:
    pass


class Type:
    pass


def cast(typ, val):
    return val


def _overload_dummy(*args, **kwds):
    """Helper for @overload to raise when called."""
    raise NotImplementedError(
        "You should not call an overloaded function. "
        "A series of @overload-decorated functions "
        "outside a stub module should always be followed "
        "by an implementation that is not @overload-ed."
    )


def overload():
    return _overload_dummy
