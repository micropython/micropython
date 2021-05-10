# test memoryview accessing maximum values for signed/unsigned elements
try:
    memoryview
except:
    print("SKIP")
    raise SystemExit
try:
    from uarray import array
except ImportError:
    try:
        from array import array
    except ImportError:
        print("SKIP")
        raise SystemExit

print(list(memoryview(array('i', [0x7f000000, -0x80000000]))))
print(list(memoryview(array('I', [0x7f000000, 0x80000000, 0x81000000, 0xffffffff]))))
