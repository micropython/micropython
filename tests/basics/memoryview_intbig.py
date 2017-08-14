# test memoryview accessing maximum values for signed/unsigned elements
try:
    from array import array
    memoryview
except:
    print("SKIP")
    raise SystemExit

print(list(memoryview(array('i', [0x7f000000, -0x80000000]))))
print(list(memoryview(array('I', [0x7f000000, 0x80000000, 0x81000000, 0xffffffff]))))
