if not hasattr(bytes, "fromhex"):
    print("SKIP")
    raise SystemExit

for x in (
    b"\x00\x01\x02\x03\x04\x05\x06\x07",
    b"\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f",
    b"\x7f\x80\xff",
    b"1234ABCDabcd",
):
    print(x.hex())
    print(bytearray(x).hex())
    print(memoryview(x).hex())
    print(x.hex(":"))
    print(bytearray(x).hex(":"))
    print(memoryview(x).hex(":"))

for x in (
    "0001020304050607",
    "08090a0b0c0d0e0f",
    "7f80ff",
    "313233344142434461626364",
):
    print(bytes.fromhex(x))
