# Test str.ljust(), str.rjust(), and str.zfill()
try:
    str.ljust
except:
    print("SKIP")
    raise SystemExit

# ljust
print("foo".ljust(0))
print("foo".ljust(1))
print("foo".ljust(3))
print("foo".ljust(5))
print("foo".ljust(10))
print("foo".ljust(5, "*"))
print(repr("foo".ljust(10)))

# rjust
print("foo".rjust(0))
print("foo".rjust(1))
print("foo".rjust(3))
print("foo".rjust(5))
print("foo".rjust(10))
print("foo".rjust(5, "*"))
print(repr("foo".rjust(10)))

# zfill
print("42".zfill(5))
print("-42".zfill(5))
print("+42".zfill(5))
print("42".zfill(1))
print("".zfill(5))
print("-".zfill(5))
print("abc".zfill(6))
