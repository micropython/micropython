# to test arbitrary precision integers
# requires: !int-big !int-64

try:
    print(int.from_bytes(b'\1' * 9))
except OverflowError:
    print("OverflowError")
