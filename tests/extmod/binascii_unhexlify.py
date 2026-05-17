try:
    from binascii import unhexlify
except ImportError:
    print("SKIP")
    raise SystemExit

for x in (
    b"0001020304050607",
    b"08090a0b0c0d0e0f",
    b"7f80ff",
    b"313233344142434461626364",
):
    print(unhexlify(x))

try:
    a = unhexlify(b"0")  # odd buffer length
except ValueError:
    print("ValueError")

try:
    a = unhexlify(b"gg")  # digit not hex
except ValueError:
    print("ValueError")
