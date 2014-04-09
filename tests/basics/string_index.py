print("hello world".index("ll"))
print("hello world".index("ll", None))
print("hello world".index("ll", 1))
print("hello world".index("ll", 1, None))
print("hello world".index("ll", None, None))
print("hello world".index("ll", 1, -1))

try:
    print("hello world".index("ll", 1, 1))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

try:
    print("hello world".index("ll", 1, 2))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

try:
    print("hello world".index("ll", 1, 3))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

print("hello world".index("ll", 1, 4))
print("hello world".index("ll", 1, 5))
print("hello world".index("ll", -100))
print("0000".index('0'))
print("0000".index('0', 0))
print("0000".index('0', 1))
print("0000".index('0', 2))
print("0000".index('0', 3))

try:
    print("0000".index('0', 4))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

try:
    print("0000".index('0', 5))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

try:
    print("0000".index('-1', 3))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

try:
    print("0000".index('1', 3))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

try:
    print("0000".index('1', 4))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

try:
    print("0000".index('1', 5))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")
