@micropython.native
def f_large_pos():
    return 0x1234_5678_9abc_def0


@micropython.native
def f_large_neg():
    return -0x7fff_ffff_ffff_ffff


@micropython.native
def f_shift16():
    return 1 << 16


@micropython.native
def f_shift32():
    return 1 << 32


@micropython.native
def f_shift48():
    return 1 << 48


@micropython.native
def f_mixed():
    a = 0x0000_ffff_0000_0000
    b = 0xffff_0000_0000_0000
    return a | b


@micropython.native
def f_many_locals():
    a = 1
    b = 2
    c = 3
    d = 4
    e = 5
    f = 6
    g = 7
    h = 8
    i = 9
    j = 10
    return a + b + c + d + e + f + g + h + i + j


print(hex(f_large_pos()))
print(hex(f_large_neg()))
print(hex(f_shift16()))
print(hex(f_shift32()))
print(hex(f_shift48()))
print(hex(f_mixed()))
print(f_many_locals())
