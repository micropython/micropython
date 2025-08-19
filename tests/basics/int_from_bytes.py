# int_from_bytes.py


def test_int(x, byteorder, signed):
    size = 12
    if x < 2**30 and x >= -(2**30):
        size = 4
    elif x < 2**62 and x >= -(2**62):
        size = 8
    
    print(f"x:{x}, x:0x{x:X}, size:{size}", end="")
    b = x.to_bytes(size, byteorder=byteorder, signed=signed)
    print(f", to_bytes:{b}", end="")
    y = int.from_bytes(b, byteorder=byteorder, signed=signed)
    print(f", y = x.from_bytes:{y}, {x == y}")
    print()
    assert x == y


def do_test_int(n, byteorder, signed):
    print(f"========== test_int() n:{n}, byteorder:{byteorder}, signed:{signed}, positive ==========")
    test_int(0, byteorder, signed)
    test_int(2, byteorder, signed)
    test_int(2**7 - 2, byteorder, signed)
    test_int(2**7, byteorder, signed)
    test_int(2**7 + 2, byteorder, signed)
    test_int(2**15 - 2, byteorder, signed)
    test_int(2**15, byteorder, signed)
    test_int(2**15 + 2, byteorder, signed)
    test_int(2**23 - 2, byteorder, signed)
    test_int(2**23, byteorder, signed)
    test_int(2**23 + 2, byteorder, signed)
    test_int(2**31 - 2, byteorder, signed)
    test_int(2**31, byteorder, signed)
    test_int(2**31 + 2, byteorder, signed)
    test_int(2**39 - 2, byteorder, signed)
    test_int(2**39, byteorder, signed)
    test_int(2**39 + 2, byteorder, signed)
    test_int(2**47 - 2, byteorder, signed)
    test_int(2**47, byteorder, signed)
    test_int(2**47 + 2, byteorder, signed)
    test_int(2**55 - 2, byteorder, signed)
    test_int(2**55, byteorder, signed)
    test_int(2**55 + 2, byteorder, signed)
    test_int(2**62 - 2, byteorder, signed)
    test_int(2**62, byteorder, signed)
    test_int(2**62 + 2, byteorder, signed)

    if signed:
        print(f"---------- test_int() n:{n}, byteorder:{byteorder}, signed:{signed}, negative ----------")
        test_int(-0, byteorder, signed)
        test_int(-2, byteorder, signed)
        test_int(-(2**7 - 2), byteorder, signed)
        test_int(-(2**7), byteorder, signed)
        test_int(-(2**7 + 2), byteorder, signed)
        test_int(-(2**15 - 2), byteorder, signed)
        test_int(-(2**15), byteorder, signed)
        test_int(-(2**15 + 2), byteorder, signed)
        test_int(-(2**23 - 2), byteorder, signed)
        test_int(-(2**23), byteorder, signed)
        test_int(-(2**23 + 2), byteorder, signed)
        test_int(-(2**31 - 2), byteorder, signed)
        test_int(-(2**31), byteorder, signed)
        test_int(-(2**31 + 2), byteorder, signed)
        test_int(-(2**39 - 2), byteorder, signed)
        test_int(-(2**39), byteorder, signed)
        test_int(-(2**39 + 2), byteorder, signed)
        test_int(-(2**47 - 2), byteorder, signed)
        test_int(-(2**47), byteorder, signed)
        test_int(-(2**47 + 2), byteorder, signed)
        test_int(-(2**55 - 2), byteorder, signed)
        test_int(-(2**55), byteorder, signed)
        test_int(-(2**55 + 2), byteorder, signed)
        test_int(-(2**62 - 2), byteorder, signed)
        test_int(-(2**62), byteorder, signed)
        test_int(-(2**62 + 2), byteorder, signed)


def reverse(b):
    r = bytearray(len(b))
    for i in range(len(b)):
        r[i] = b[len(b) - 1 - i]
    return r


def signed_from_unsigned(uint, size):
    if uint == 0:
        return 0
    if uint & (1 << (size * 8 - 1)):
        mask = (2 ** (size * 8)) - 1
        sint = -((uint ^ mask) + 1)
        return sint
    else:
        return uint


def test_bytes(b, uint, byteorder, signed):
    size = len(b)
    assert b == uint.to_bytes(size, byteorder='little', signed=False)
    sint = signed_from_unsigned(uint, size)
    print(f"bytes:{b}, size:{size}, byteorder:{byteorder}, signed:{signed}, ", end="")
    if byteorder == "big":
        b = reverse(b)
        print(f"reverse:{b}, ", end="")
    i = int.from_bytes(b, byteorder=byteorder, signed=signed)
    if signed:
        print(f"sint:{sint}, from_bytes:{i}, {i == sint}, ", end="")
    else:
        print(f"uint:{uint}, from_bytes:{i}, {i == uint}, ", end="")
    to_b = i.to_bytes(size, byteorder=byteorder, signed=signed)
    print(f"to_bytes:{to_b}")
    print()
    if signed:
        assert i == sint
    else:
        assert i == uint


def do_test_bytes(n, byteorder, signed):
    print(f"********** test_bytes() n:{n}, byteorder:{byteorder}, signed:{signed} **********")
    # bytes in little byteorder
    test_bytes(b"\x00", 0, byteorder, signed)
    test_bytes(b"\x02", 2, byteorder, signed)
    test_bytes(b"\xFE", 254, byteorder, signed)
    test_bytes(b"\x00\x02", 2 << 8, byteorder, signed)
    test_bytes(b"\x00\xFE", 254 << 8, byteorder, signed)
    test_bytes(b"\x00\x00\x02", 2 << 16, byteorder, signed)
    test_bytes(b"\x00\x00\xFE", 254 << 16, byteorder, signed)
    test_bytes(b"\x00\x00\x00\x02", 2 << 24, byteorder, signed)
    test_bytes(b"\x00\x00\x00\xFE", 254 << 24, byteorder, signed)
    test_bytes(b"\x00\x00\x00\x00\x02", 2 << 32, byteorder, signed)
    test_bytes(b"\x00\x00\x00\x00\xFE", 254 << 32, byteorder, signed)
    test_bytes(b"\x00\x00\x00\x00\x00\x02", 2 << 40, byteorder, signed)
    test_bytes(b"\x00\x00\x00\x00\x00\xFE", 254 << 40, byteorder, signed)
    test_bytes(b"\x00\x00\x00\x00\x00\x00\x02", 2 << 48, byteorder, signed)
    test_bytes(b"\x00\x00\x00\x00\x00\x00\xFE", 254 << 48, byteorder, signed)
    # test_bytes(b"\x00\x00\x00\x00\x00\x00\x00\x02", 2 << 56, byteorder, signed)
    # test_bytes(b"\x00\x00\x00\x00\x00\x00\x00\xFE", 254 << 56, byteorder, signed)


do_test_bytes(n=1, byteorder="little", signed=False)
do_test_bytes(n=2, byteorder="big", signed=False)
do_test_bytes(n=3, byteorder="little", signed=True)
do_test_bytes(n=4, byteorder="big", signed=True)

do_test_int(n=5, byteorder="little", signed=False)
do_test_int(n=6, byteorder="big", signed=False)
do_test_int(n=7, byteorder="little", signed=True)
do_test_int(n=8, byteorder="big", signed=True)

print("Ok.")
