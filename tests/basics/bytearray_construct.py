# test construction of bytearray from different objects

print(bytearray(b'123'))
print(bytearray('1234', 'utf-8'))
print(bytearray('12345', 'utf-8', 'strict'))
print(bytearray((1, 2)))
print(bytearray([1, 2]))

try:
    print(bytearray('1234'))
except TypeError:
    print("TypeError")
