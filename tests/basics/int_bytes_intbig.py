print((2**64).to_bytes(9, "little"))
print((2**64).to_bytes(9, "big"))

b = bytes(range(20))

il = int.from_bytes(b, "little")
ib = int.from_bytes(b, "big")
print(il)
print(ib)
print(il.to_bytes(20, "little"))
print(ib.to_bytes(20, "big"))

# check that extra zero bytes don't change the internal int value
print(int.from_bytes(b + bytes(10), "little") == int.from_bytes(b, "little"))
