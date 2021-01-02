for byteorder in ["little", "big"]:
    print((255).to_bytes(1, byteorder, signed=False))

    #signed shall be False by default
    print((255).to_bytes(1, byteorder))

    # test min/max signed value
    for i in [1,2,4,8]:
        v = 2**(i*8 - 1) - 1
        print(v)
        vbytes = v.to_bytes(i, byteorder)
        print(vbytes)
        print(int.from_bytes(vbytes, byteorder))
        v = - (2**(i*8 - 1))
        print(v)
        vbytes = v.to_bytes(i, byteorder, signed=True)
        print(vbytes)
        print(int.from_bytes(vbytes, byteorder, signed=True))

    # negative number should raise an error if signed=False
    try:
        (-1).to_bytes(1, byteorder, signed=False)
        print("fail")
    except OverflowError:
        print("OverflowError")

    # one byte shall be enough for -1
    (-1).to_bytes(1, byteorder, signed=True)

    # overflow condition only when signed=True
    for i in [1,2,4,8]:
        v = (2**(i*8 - 1))
        vbytes = v.to_bytes(i, byteorder, signed=False)
        print(vbytes)
        print(int.from_bytes(vbytes, byteorder, signed=False))
        print(v,"shall overflow")
        try:
            print(v.to_bytes(i, byteorder, signed=True))
            print("fail")
        except OverflowError:
            print("OverflowError")
        v = - (2**(i*8 - 1)) - 1
        vbytes = (-v).to_bytes(i+1, byteorder, signed=False)
        print(vbytes)
        print(int.from_bytes(vbytes, byteorder, signed=False))
        print(v,"shall overflow")
        try:
            print(v.to_bytes(i, byteorder, signed=True))
            print("fail")
        except OverflowError:
            print("OverflowError")
