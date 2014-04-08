print("hello world".rindex("ll"))
print("hello world".rindex("ll", None))
print("hello world".rindex("ll", 1))
print("hello world".rindex("ll", 1, None))
print("hello world".rindex("ll", None, None))
print("hello world".rindex("ll", 1, -1))

try:
    print("hello world".rindex("ll", 1, 1))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

try:
    print("hello world".rindex("ll", 1, 2))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

try:
    print("hello world".rindex("ll", 1, 3))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

print("hello world".rindex("ll", 1, 4))
print("hello world".rindex("ll", 1, 5))
print("hello world".rindex("ll", -100))
print("0000".rindex('0'))
print("0000".rindex('0', 0))
print("0000".rindex('0', 1))
print("0000".rindex('0', 2))
print("0000".rindex('0', 3))

try:
    print("0000".rindex('0', 4))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

try:
    print("0000".rindex('0', 5))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

try:
    print("0000".rindex('-1', 3))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

try:
    print("0000".rindex('1', 3))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

try:
    print("0000".rindex('1', 4))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

try:
    print("0000".rindex('1', 5))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")
