"""
categories: Core,Generator
description: Context manager __exit__() not called in a generator which does not run to completion
cause: Unknown
workaround: Unknown
"""


class foo(object):
    def __enter__(self):
        print("Enter")

    def __exit__(self, *args):
        print("Exit")


def bar(x):
    with foo():
        while True:
            x += 1
            yield x


def func():
    g = bar(0)
    for _ in range(3):
        print(next(g))


func()
