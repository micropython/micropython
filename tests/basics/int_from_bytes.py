# int_from_bytes.py


def calc_size(x):
    size = 0
    if x < 0:
        x = -x
        size += 1
    while True:
        x >>= 8
        size += 1
        if not x:
            break
    return size


def test_int(x, order, signed):
    size = 4 if x < 2**30 and x >= -(2**30) else 8
    # size = calc_size(x)
    print(f"x:{x}, x:0x{x:X}, size:{size}", end="")
    b = x.to_bytes(size, byteorder=order, signed=signed)
    print(f", b:{b}")
    y = int.from_bytes(b, byteorder=order, signed=signed)
    print(x, y, x == y, b)
    print()
    assert x == y


def do_test_int(n, order, signed):
    print(f"========== n:{n}, order:{order}, signed:{signed} ==========")
    test_int(0, order, signed)
    test_int(2, order, signed)
    test_int(2**7 - 2, order, signed)
    test_int(2**7, order, signed)
    test_int(2**7 + 2, order, signed)
    test_int(2**15 - 2, order, signed)
    test_int(2**15, order, signed)
    test_int(2**15 + 2, order, signed)
    test_int(2**23 - 2, order, signed)
    test_int(2**23, order, signed)
    test_int(2**23 + 2, order, signed)
    test_int(2**31 - 2, order, signed)
    test_int(2**31, order, signed)
    test_int(2**31 + 2, order, signed)
    test_int(2**39 - 2, order, signed)
    test_int(2**39, order, signed)
    test_int(2**39 + 2, order, signed)
    test_int(2**47 - 2, order, signed)
    test_int(2**47, order, signed)
    test_int(2**47 + 2, order, signed)
    test_int(2**55 - 2, order, signed)
    test_int(2**55, order, signed)
    test_int(2**55 + 2, order, signed)
    test_int(2**63 - 2, order, signed)
    test_int(2**63, order, signed)
    test_int(2**63 + 2, order, signed)

    # test_int(0xFF-2, order, signed)
    # test_int(0xFF, order, signed)
    # test_int(0xFF+2, order, signed)
    # test_int(0xFFFF-2, order, signed)
    # test_int(0xFFFF, order, signed)
    # test_int(0xFFFF+2, order, signed)
    # test_int(0xFFFFFF-2, order, signed)
    # test_int(0xFFFFFF, order, signed)
    # test_int(0xFFFFFF+2, order, signed)
    # test_int(0x3FFFFFFF, order, signed)
    # test_int(0x7FFFffff, order, signed)
    # test_int(0xFFFFffff, order, signed)
    # test_int(0xffFFFFffff, order, signed)
    # test_int(0xffffFFFFffff, order, signed)
    # test_int(0xFFffffFFFFffff, order, signed)
    # test_int(0xFFFFffffFFFFffff, order, signed)

    if signed:
        print(f"---------- n:{n}, order:{order}, signed:{signed} ----------")
        test_int(-0, order, signed)
        test_int(-2, order, signed)
        test_int(-(2**7 - 2), order, signed)
        test_int(-(2**7), order, signed)
        test_int(-(2**7 + 2), order, signed)
        test_int(-(2**15 - 2), order, signed)
        test_int(-(2**15), order, signed)
        test_int(-(2**15 + 2), order, signed)
        test_int(-(2**23 - 2), order, signed)
        test_int(-(2**23), order, signed)
        test_int(-(2**23 + 2), order, signed)
        test_int(-(2**31 - 2), order, signed)
        test_int(-(2**31), order, signed)
        test_int(-(2**31 + 2), order, signed)
        test_int(-(2**39 - 2), order, signed)
        test_int(-(2**39), order, signed)
        test_int(-(2**39 + 2), order, signed)
        test_int(-(2**47 - 2), order, signed)
        test_int(-(2**47), order, signed)
        test_int(-(2**47 + 2), order, signed)
        test_int(-(2**55 - 2), order, signed)
        test_int(-(2**55), order, signed)
        test_int(-(2**55 + 2), order, signed)
        test_int(-(2**63 - 2), order, signed)
        test_int(-(2**63), order, signed)
        test_int(-(2**63 + 2), order, signed)

        # test_int(-0xFF, order, signed)
        # test_int(-0xFF - 1, order, signed)
        # test_int(-0xFF - 2, order, signed)
        # test_int(-0xFFFF - 0, order, signed)
        # test_int(-0xFFFF - 1, order, signed)
        # test_int(-0xFFFF - 2, order, signed)
        # test_int(-0xFFFFFF - 0, order, signed)
        # test_int(-0xFFFFFF + 1, order, signed)
        # test_int(-0xFFFFFF + 2, order, signed)
        # test_int(-0x3FFFFFFF - 1, order, signed)
        # test_int(-0x7FFFffff-2, order, signed)
        # test_int(-0xFFFFffff-2, order, signed)
        # test_int(-0xffFFFFffff-2, order, signed)
        # test_int(-0xffffFFFFffff-2, order, signed)
        # test_int(-0xFFffffFFFFffff-2, order, signed)
        # test_int(-0xFFFFffffFFFFffff-2, order, signed)


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


def test_bytes(b, uint, order, signed):
    size = len(b)
    sint = signed_from_unsigned(uint, size)
    print(f"bytes:{b}, size:{size}, order:{order}, signed:{signed}, ", end="")
    if order == "little":
        b = reverse(b)
        print(f"reverse:{b}, ", end="")
    i = int.from_bytes(b, byteorder=order, signed=signed)
    if signed:
        print(f"sint:{sint}, from_bytes:{i}, {i == sint}, ", end="")
    else:
        print(f"uint:{uint}, from_bytes:{i}, {i == uint}, ", end="")
    to_b = i.to_bytes(size, byteorder=order, signed=signed)
    print(f"to_bytes:{to_b}")
    print()
    if signed:
        assert i == sint
    else:
        assert i == uint


def do_test_bytes(n, order, signed):
    print(f"********** n:{n}, order:{order}, signed:{signed} **********")
    test_bytes(b"\x00", 0, order, signed)
    test_bytes(b"\x02", 2, order, signed)
    test_bytes(b"\xfe", 254, order, signed)
    test_bytes(b"\x02\x00", 2 << 8, order, signed)
    test_bytes(b"\xfe\x00", 254 << 8, order, signed)
    test_bytes(b"\x02\x00\x00", 2 << 16, order, signed)
    test_bytes(b"\xfe\x00\x00", 254 << 16, order, signed)
    test_bytes(b"\x02\x00\x00\x00", 2 << 24, order, signed)
    test_bytes(b"\xfe\x00\x00\x00", 254 << 24, order, signed)
    test_bytes(b"\x02\x00\x00\x00\x00", 2 << 32, order, signed)
    test_bytes(b"\xfe\x00\x00\x00\x00", 254 << 32, order, signed)
    test_bytes(b"\x02\x00\x00\x00\x00\x00", 2 << 40, order, signed)
    test_bytes(b"\xfe\x00\x00\x00\x00\x00", 254 << 40, order, signed)
    test_bytes(b"\x02\x00\x00\x00\x00\x00\x00", 2 << 48, order, signed)
    test_bytes(b"\xfe\x00\x00\x00\x00\x00\x00", 254 << 48, order, signed)
    test_bytes(b"\x02\x00\x00\x00\x00\x00\x00\x00", 2 << 56, order, signed)
    test_bytes(b"\xfe\x00\x00\x00\x00\x00\x00\x00", 254 << 56, order, signed)


do_test_bytes(n=1, order="little", signed=False)
do_test_bytes(n=2, order="big", signed=False)
do_test_bytes(n=3, order="little", signed=True)
do_test_bytes(n=4, order="big", signed=True)

do_test_int(n=5, order="little", signed=False)
do_test_int(n=6, order="big", signed=False)
do_test_int(n=7, order="little", signed=True)
do_test_int(n=8, order="big", signed=True)

print("Ok.")
