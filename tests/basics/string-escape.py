a = "a\1b"
print(len(a))
print(ord(a[1]))
print(len("a\123b"))
a = "a\12345b"
print(len(a))
print(ord(a[1]))

a = "a\xffb"
print(len(a))
print(ord(a[1]))
