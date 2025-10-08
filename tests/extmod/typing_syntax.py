# This doesn't quite test everything but just serves to verify that basic syntax works,
# which for MicroPython means everything typing-related should be ignored.

try:
    import typing
except ImportError:
    print("SKIP")
    raise SystemExit

from typing import List, Tuple, Iterable, NewType, TypeVar, Union, Generic

# Available with MICROPY_PY_TYPING_EXTRA_MODULES.
try:
    import typing_extensions
except ImportError:
    from typing import Any

    typing_extensions = None

# Available with MICROPY_PY_TYPING_EXTRA_MODULES and MICROPY_MODULE_BUILTIN_SUBPACKAGES.
try:
    import collections.abc

    collections.abc.Sequence
except ImportError:
    pass

import sys

# If this is available verify it works, and try the other modules as well.
if typing_extensions is not None:
    from typing_extensions import Any
    import __future__
    from abc import abstractmethod

    getattr(__future__, "annotations")


if "micropython" in sys.implementation.name:
    # Verify assignment is not possible.
    try:
        typing.a = None
        raise Exception()
    except AttributeError:
        pass
    try:
        typing[0] = None
        raise Exception()
    except TypeError:
        pass
    try:
        List.a = None
        raise Exception()
    except AttributeError:
        pass


MyAlias = str
Vector: typing.List[float]
UserId = NewType("UserId", int)
T = TypeVar("T", int, float, complex)

hintedGlobal: Any = None


def func_with_hints(c: int, b: MyAlias, a: Union[int, None], lst: List[float] = [0.0]) -> Any:
    pass
