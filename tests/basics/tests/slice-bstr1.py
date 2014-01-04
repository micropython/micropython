print(b"123"[0:1])

print(b"123"[0:2])

print(b"123"[:1])

print(b"123"[1:])

# Idiom for copying sequence
print(b"123"[:])

print(b"123"[:-1])

# Weird cases
print(b"123"[0:0])
print(b"123"[1:0])
print(b"123"[1:1])
print(b"123"[-1:-1])
print(b"123"[-3:])
print(b"123"[-3:3])
print(b"123"[0:])
print(b"123"[:0])
print(b"123"[:-3])
print(b"123"[:-4])
# No IndexError!
print(b""[1:1])
print(b""[-1:-1])
