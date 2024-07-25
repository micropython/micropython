try:
    import binascii
except ImportError:
    print("SKIP")
    raise SystemExit

for x in (
    b"0001020304050607",
    b"08090a0b0c0d0e0f",
    b"7f80ff",
    b"313233344142434461626364",
):
    print(binascii.unhexlify(x))

# CIRCUITPY-CHANGE
# Unicode strings can be decoded
print(binascii.unhexlify("313233344142434461626364"))

try:
    a = binascii.unhexlify(b"0")  # odd buffer length
except ValueError:
    print("ValueError")

try:
    a = binascii.unhexlify(b"gg")  # digit not hex
except ValueError:
    print("ValueError")
