# Test a UTF-8 encoded literal
s = "asdf©qwer"
for i in range(len(s)):
    print("s[%d]: %s   %X" % (i, s[i], ord(s[i])))

# Test all three forms of Unicode escape, and
# all blocks of UTF-8 byte patterns
s = "a\xa9\xff\u0123\u0800\uffee\U0001f44c"
for i in range(-len(s), len(s)):
    print("s[%d]: %s   %X" % (i, s[i], ord(s[i])))
    print("s[:%d]: %d chars, '%s'" % (i, len(s[:i]), s[:i]))
    for j in range(i, len(s)):
        print("s[%d:%d]: %d chars, '%s'" % (i, j, len(s[i:j]), s[i:j]))
    print("s[%d:]: %d chars, '%s'" % (i, len(s[i:]), s[i:]))

# Test UTF-8 encode and decode
enc = s.encode()
print(enc, enc.decode() == s)

# printing of unicode chars using repr
# NOTE: for some characters (eg \u10ff) we differ to CPython
print(repr("a\uffff"))
print(repr("a\U0001ffff"))

# test invalid escape code
try:
    eval('"\\U00110000"')
except SyntaxError:
    print("SyntaxError")

# test unicode string given to int
try:
    int("\u0200")
except ValueError:
    print("ValueError")

# test invalid UTF-8 string
try:
    str(b"ab\xa1", "utf8")
except UnicodeError:
    print("UnicodeError")
try:
    str(b"ab\xf8", "utf8")
except UnicodeError:
    print("UnicodeError")
try:
    str(bytearray(b"ab\xc0a"), "utf8")
except UnicodeError:
    print("UnicodeError")
try:
    str(b"\xf0\xe0\xed\xe8", "utf8")
except UnicodeError:
    print("UnicodeError")
