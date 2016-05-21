try:
    str.center
except:
    import sys
    print("SKIP")
    sys.exit()

print("foo".center(0))
print("foo".center(1))
print("foo".center(3))
print("foo".center(4))
print("foo".center(5))
print("foo".center(6))
print("foo".center(20))
