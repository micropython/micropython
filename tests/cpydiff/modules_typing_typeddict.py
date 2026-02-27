"""
categories: Modules,typing
description: ``TypedDict`` class not allowed for instance or class checks.
cause: Micropython does not implement all typing features
workaround: None
"""

from typing import TypeVar, TypedDict


class Movie(TypedDict):
    name: str
    year: int


movie: Movie = {"name": "Blade Runner", "year": 1982}

try:
    if isinstance(movie, Movie):  # type: ignore
        pass
    print("TypedDict class not allowed for instance or class checks")

except TypeError as e:
    print("Handled according to spec")
