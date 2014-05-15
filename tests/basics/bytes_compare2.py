import array

print(b"1" == 1)
print(b"123" == bytearray(b"123"))
print(b"123" == "123")
# CPyhon gives False here
#print(b"\x01\x02\x03" == array.array("B", [1, 2, 3]))
