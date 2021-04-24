try:
    from math import *
    from cmath import polar
except ImportError:
    print("SKIP")
    raise SystemExit


class T1:
    pass


class T2:
    def __float__(self):
        return 1.23


class T3:
    def __float__(self):
        return 1.23

    def __complex__(self):
        return 1.23j


class T4:
    def __float__(self):
        return 1

    def __complex__(self):
        return 1


class T5:
    def __float__(self):
        return []

    def __complex__(self):
        return []


instances = [T1(), T2(), T3(), T4(), T5()]

for i in instances:
    print(i.__class__.__name__)

    try:
        print("{:.5g}".format(sin(i)))
    except TypeError:
        print("Type Error")

    try:
        print(1.0 + i)
    except TypeError:
        print("Type Error")

    try:
        print(1.0 + float(i))
    except TypeError:
        print("Type Error")

    try:
        print(1.0j + i)
    except TypeError:
        print("Type Error")

    try:
        print(1.0j + float(i))
    except TypeError:
        print("Type Error")

    try:
        print(1.0j + complex(i))
    except TypeError:
        print("Type Error")

    try:
        r, theta = polar(i)
        print("{:.5g}, {:.5g}".format(r, theta))
    except TypeError:
        print("Type Error")
