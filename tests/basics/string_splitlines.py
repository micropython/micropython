# test string.splitlines() method

try:
    str.splitlines
except:
    import sys
    print("SKIP")
    sys.exit()

# test \n as newline
print("foo\nbar".splitlines())
print("foo\nbar\n".splitlines())
print("foo and\nbar\n".splitlines())
print("foo\nbar\n\n".splitlines())
print("foo\n\nbar\n\n".splitlines())
print("\nfoo\nbar\n".splitlines())

# test \r as newline
print("foo\rbar\r".splitlines())
print("\rfoo and\r\rbar\r".splitlines())

# test \r\n as newline
print("foo\r\nbar\r\n".splitlines())
print("\r\nfoo and\r\n\r\nbar\r\n".splitlines())

# test keepends arg
print("foo\nbar".splitlines(True))
print("foo\nbar\n".splitlines(True))
print("foo\nbar\n\n".splitlines(True))
print("foo\rbar".splitlines(keepends=True))
print("foo\rbar\r\r".splitlines(keepends=True))
print("foo\r\nbar".splitlines(keepends=True))
print("foo\r\nbar\r\n\r\n".splitlines(keepends=True))

# test splitting bytes objects
print(b"foo\nbar".splitlines())
print(b"foo\nbar\n".splitlines())
print(b"foo\r\nbar\r\n\r\n".splitlines(True))
