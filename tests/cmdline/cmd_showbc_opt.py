# cmdline: -v -v
# test printing of bytecode when it's optimised away


def f0():
    return 0
    print(1)


def f1(x):
    if x:
        return
        print(1)
    print(2)


def f2(x):
    raise Exception
    print(1)


def f3(x):
    while x:
        break
        print(1)
    print(2)


def f4(x):
    while x:
        continue
        print(1)
    print(2)
