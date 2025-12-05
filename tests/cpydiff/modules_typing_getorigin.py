"""
categories: Modules,typing
description: ``get_origin()`` function not fully implemented.
cause: Micropython does not implement all typing features from Python 3.8+
workaround: None
"""
# https://docs.python.org/3/library/typing.html#typing.get_origin

from typing import Dict, get_origin

assert get_origin(Dict[str, int]) is dict, "origin Dict cannot be detected"

assert get_origin(str) is None, "origin str should be None"
