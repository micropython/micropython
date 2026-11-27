# Test str.capitalize() and str.swapcase()
try:
    str.capitalize
except:
    print("SKIP")
    raise SystemExit

# capitalize
print("".capitalize())
print("hello".capitalize())
print("HELLO".capitalize())
print("hello world".capitalize())
print("hELLO".capitalize())
print("123abc".capitalize())
print("a".capitalize())
print("A".capitalize())

# swapcase
print("".swapcase())
print("Hello World".swapcase())
print("hELLO".swapcase())
print("123abc".swapcase())
print("ABC".swapcase())
