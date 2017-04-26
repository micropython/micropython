# test bytes + other
try:
    import array
except ImportError:
    import sys
    print("SKIP")
    sys.exit()

print(b"123" + array.array('i', [1]))
