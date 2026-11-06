# test construction of bytearray from different objects
# requires: bytearray

print(bytearray(b'123'))
print(bytearray('1234', 'utf-8'))
print(bytearray('12345', 'utf-8', 'strict'))
print(bytearray((1, 2)))
print(bytearray([1, 2]))

try:
    print(bytearray('1234'))
except TypeError:
    print("TypeError")

# int subclass as the length
class MyInt(int):
    pass

print(bytearray(MyInt(0)))
print(bytearray(MyInt(2)))
