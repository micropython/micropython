"""
categories: Modules,typing
description: ``TypedDict`` does not behave according to spec.
cause: Micropython does not implement all typing features
workaround: None
"""
# Specification: https://typing.readthedocs.io/en/latest/spec/typeddict.html

from typing import TypedDict


class Movie(TypedDict):
    title: str
    year: int


movie = Movie(title="Life of Brian", year=1979)
print("Type is: ", type(movie))

try:
    if isinstance(movie, Movie):  # type: ignore
        pass
    print("TypedDict class not allowed for instance or class checks")

except TypeError as e:
    print("Handled according to spec")
