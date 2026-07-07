
def test_int(x, byteorder, signed):
    size = 12
    if x <= 2**30 and x >= -(2**30):
        size = 4
    elif x <= 2**62 and x >= -(2**62):
        size = 8
    
    print("x:{}, x:0x{}, size:{}".format(x, x, size), end="")
    b = x.to_bytes(size, byteorder=byteorder, signed=signed)
    print(", to_bytes:{}".format(b), end="")
    y = int.from_bytes(b, byteorder=byteorder, signed=signed)
    print(", y = x.from_bytes:{}, {}".format(y, x == y))
    print()
    assert x == y


def do_test_int(n, byteorder, signed):
    print("========== test_int() n:{}, byteorder:{}, signed:{}, positive ==========".format(n, byteorder, signed))
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
    test_int(2**30 - 2, byteorder, signed)
    test_int(2**30, byteorder, signed)
    test_int(2**30 + 2, byteorder, signed)
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
        print("---------- test_int() n:{}, byteorder:{}, signed:{}, negative ----------".format(n, byteorder, signed))
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
        test_int(-(2**30 - 2), byteorder, signed)
        test_int(-(2**30), byteorder, signed)
        test_int(-(2**30 + 2), byteorder, signed)
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
    r = []
    for x in b:
        r.insert(0, x)
    return bytes(r)


def signed_from_unsigned(uint, size):
    if uint == 0:
        return 0
    b = size * 8 - 1
    b = 1 << b
    if uint & b:
        mask = (2 ** (size * 8)) - 1
        sint = -((uint ^ mask) + 1)
        return sint
    else:
        return uint


def test_bytes(b, uint, byteorder, signed):
#     if uint > 2**30:
#         print("SKIP")
#         raise SystemExit
    
    size = len(b)
    assert b == uint.to_bytes(size, byteorder='little', signed=False)
    sint = signed_from_unsigned(uint, size)
    print("bytes:{}, size:{}, byteorder:{}, signed:{}, ".format(b, size, byteorder, signed), end="")
    if byteorder == "big":
        b = reverse(b)
        print("reverse:{}, ".format(b), end="")
    i = int.from_bytes(b, byteorder=byteorder, signed=signed)
    if signed:
        print("sint:{}, from_bytes:{}, {}, ".format(sint, i, i == sint), end="")
    else:
        print("uint:{}, from_bytes:{}, {}, ".format(uint, i, i == uint), end="")
    to_b = i.to_bytes(size, byteorder=byteorder, signed=signed)
    print("to_bytes:{}".format(to_b))
    print()
    if signed:
        assert i == sint
    else:
        assert i == uint


def do_test_bytes(n, byteorder, signed):
    print("********** test_bytes() n:{}, byteorder:{}, signed:{} **********".format(n, byteorder, signed))
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
    test_bytes(b"\x00\x00\x00\x00\x00\x00\x00\x02", 2 << 56, byteorder, signed)
    test_bytes(b"\x00\x00\x00\x00\x00\x00\x00\xFE", 254 << 56, byteorder, signed)


do_test_bytes(n=1, byteorder="little", signed=False)
do_test_bytes(n=2, byteorder="big", signed=False)
do_test_bytes(n=3, byteorder="little", signed=True)
do_test_bytes(n=4, byteorder="big", signed=True)

do_test_int(n=5, byteorder="little", signed=False)
do_test_int(n=6, byteorder="big", signed=False)
do_test_int(n=7, byteorder="little", signed=True)
do_test_int(n=8, byteorder="big", signed=True)

print("Ok.")
