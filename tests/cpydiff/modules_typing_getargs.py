"""
categories: Modules,typing
description: ``get_args()`` function not fully implemented.
cause: Micropython does not implement all typing features
workaround: None
"""

from typing import get_args

# partial implementation of get_args
x = get_args(int)
assert x == (), f"expected () but got {x}"
