def test_int(x, byteorder, signed):
    if signed:
        x = -x

    size = 12
    if x <= 1073741823 and x >= -1073741823:  # 2**30 - 1
        size = 4
    elif x <= 4611686018427387903 and x >= -4611686018427387903:  # 2**62 - 1
        size = 8
    
    print("x:{}, x:0x{}, size:{}".format(x, x, size), end="")
    b = x.to_bytes(size, byteorder=byteorder, signed=signed)
    print(", to_bytes:{}".format(b), end="")
    y = int.from_bytes(b, byteorder=byteorder, signed=signed)
    print(", y = x.from_bytes:{}, {}".format(y, x == y))
    print()
    assert x == y


def do_test_int(byteorder, signed):
    print("========== byteorder:{}, signed:{} ==========".format(byteorder, signed))
    test_int(0, byteorder, signed)
    test_int(2**55 - 2, byteorder, signed)
    test_int(2**55, byteorder, signed)
    test_int(2**55 + 2, byteorder, signed)
    test_int(2**62 - 2, byteorder, signed)
    test_int(2**62, byteorder, signed)
    test_int(2**62 + 2, byteorder, signed)
    test_int(2**70 - 2, byteorder, signed)
    test_int(2**70, byteorder, signed)
    test_int(2**70 + 2, byteorder, signed)


do_test_int(byteorder="big", signed=False)
do_test_int(byteorder="little", signed=False)
do_test_int(byteorder="big", signed=True)
do_test_int(byteorder="little", signed=True)

print("Ok.")
