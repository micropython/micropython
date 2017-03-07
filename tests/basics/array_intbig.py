# test array('q') and array('Q')

try:
    from array import array
except ImportError:
    import sys
    print("SKIP")
    sys.exit()

print(array('q'))
print(array('Q'))

print(array('q', [0]))
print(array('Q', [0]))

print(array('q', [-2**63, -1, 0, 1, 2, 2**63-1]))
print(array('Q', [0, 1, 2, 2**64-1]))

print(bytes(array('q', [-1])))
print(bytes(array('Q', [2**64-1])))
