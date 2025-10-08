"""
categories: Modules,typing
description: ``runtime_checkable()`` is not implemented.
cause: Micropython does not implement all typing features from Python 3.8+
workaround: None
"""

from typing import runtime_checkable, Protocol


@runtime_checkable
class SwallowLaden(Protocol):
    def __iter__(self):
        ...


assert isinstance([1, 2, 3], SwallowLaden)
