# This Python code will be merged with the C code in main.c

# ruff: noqa: F821 - this file is evaluated with C-defined names in scope

import array


def isclose(a, b):
    return abs(a - b) < 1e-3


def test():
    tests = [
        isclose(add(0.1, 0.2), 0.3),
        isclose(add_f(0.1, 0.2), 0.3),
    ]

    ar = array.array("f", [1, 2, 3.5])
    productf(ar)
    tests.append(isclose(ar[0], 7))

    if "add_d" in globals():
        tests.append(isclose(add_d(0.1, 0.2), 0.3))

    print(tests)

    if not all(tests):
        raise SystemExit(1)


test()
