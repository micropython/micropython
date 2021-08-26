# test f-string debug feature {x=}


def f():
    return 4


def g(_):
    return 5


def h():
    return 6


x, y = 1, 2
print(f"{x=}")
print(f"{x=:08x}")
print(f"a {x=} b {y} c")
print(f"a {x=:08x} b {y} c")

print(f'a {f() + g("foo") + h()=} b')
print(f'a {f() + g("foo") + h()=:08x} b')
