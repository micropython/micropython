try:
    from math import *
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


instances = [T1(), T2(), T3()]

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
