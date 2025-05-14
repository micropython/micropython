# test memoryview accessing maximum values for signed/unsigned elements
try:
    memoryview
except:
    print("SKIP")
    raise SystemExit
try:
    from array import array
except ImportError:
    print("SKIP")
    raise SystemExit

print(list(memoryview(b'\x7f\x80\x81\xff')))
print(list(memoryview(array('b', [0x7f, -0x80]))))
print(list(memoryview(array('B', [0x7f, 0x80, 0x81, 0xff]))))
print(list(memoryview(array('h', [0x7f00, -0x8000]))))
print(list(memoryview(array('H', [0x7f00, 0x8000, 0x8100, 0xffff]))))
