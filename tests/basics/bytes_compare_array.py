try:
    import array
except ImportError:
    import sys
    print("SKIP")
    sys.exit()

print(array.array('b', [1, 2]) in b'\x01\x02\x03')
# CPython gives False here
#print(b"\x01\x02\x03" == array.array("B", [1, 2, 3]))
