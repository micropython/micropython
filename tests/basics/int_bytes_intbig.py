import skip_if
skip_if.no_bigint()

print((2**64).to_bytes(9, "little"))
print((-2**64).to_bytes(9, "little", signed=True))
print((2**64).to_bytes(9, "big"))
print((-2**64).to_bytes(9, "big", signed=True))

b = bytes(range(20))

il = int.from_bytes(b, "little")
ib = int.from_bytes(b, "big")
print(il)
print(ib)
print(il.to_bytes(20, "little"))
print(ib.to_bytes(20, "big"))

# check that extra zero bytes don't change the internal int value
print(int.from_bytes(b + bytes(10), "little") == int.from_bytes(b, "little"))

# too small buffer should raise an error
try:
    (2**64).to_bytes(8, "little")
except OverflowError:
    print("OverflowError")

# negative numbers should raise an error if signed=False
try:
    (-2**64).to_bytes(9, "little")
except OverflowError:
    print("OverflowError")
try:
    (-2**64).to_bytes(9, "little", signed=False)
except OverflowError:
    print("OverflowError")
