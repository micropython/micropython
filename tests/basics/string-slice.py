print("123"[0:1])

print("123"[0:2])

print("123"[:1])

print("123"[1:])

# Idiom for copying sequence
print("123"[:])

print("123"[:-1])

# Weird cases
print("123"[0:0])
print("123"[1:0])
print("123"[1:1])
print("123"[-1:-1])
print("123"[-3:])
print("123"[-3:3])
print("123"[0:])
print("123"[:0])
print("123"[:-3])
print("123"[:-4])
# Range check testing, don't segfault, please ;-)
print("123"[:1000000])
print("123"[1000000:])
print("123"[:-1000000])
print("123"[-1000000:])
# No IndexError!
print(""[1:1])
print(""[-1:-1])

# bytes
print(b"123"[0:2])
