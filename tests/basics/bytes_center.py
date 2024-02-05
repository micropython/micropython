try:
    bytes.center
except:
    print("SKIP")
    raise SystemExit

print(b"foo".center(0))
print(b"foo".center(1))
print(b"foo".center(3))
print(b"foo".center(4))
print(b"foo".center(5))
print(b"foo".center(6))
print(b"foo".center(20))
