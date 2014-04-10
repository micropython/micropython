# anything above 0xa0 is printed as Unicode by CPython3.3
for c in range(0xa1):
    print("0x%02x: %s" % (c, repr(chr(c))))
