print(b"".count(b""))
print(b"".count(b"a"))
print(b"a".count(b""))
print(b"a".count(b"a"))
print(b"a".count(b"b"))
print(b"b".count(b"a"))

print(b"aaa".count(b""))
print(b"aaa".count(b"a"))
print(b"aaa".count(b"aa"))
print(b"aaa".count(b"aaa"))
print(b"aaa".count(b"aaaa"))

print(b"aaaa".count(b""))
print(b"aaaa".count(b"a"))
print(b"aaaa".count(b"aa"))
print(b"aaaa".count(b"aaa"))
print(b"aaaa".count(b"aaaa"))
print(b"aaaa".count(b"aaaaa"))

print(b"aaa".count(b"", 1))
print(b"aaa".count(b"", 2))
print(b"aaa".count(b"", 3))

print(b"aaa".count(b"", 1, 2))

print(b"asdfasdfaaa".count(b"asdf", -100))
print(b"asdfasdfaaa".count(b"asdf", -8))
print(b"asdf".count(b's', True))
print(b"asdf".count(b'a', True))
print(b"asdf".count(b'a', False))
print(b"asdf".count(b'a', 1 == 2))
print(b"hello world".count(b'l'))
print(b"hello world".count(b'l', 5))
print(b"hello world".count(b'l', 3))
print(b"hello world".count(b'z', 3, 6))
print(b"aaaa".count(b'a'))
print(b"aaaa".count(b'a', 0, 3))
print(b"aaaa".count(b'a', 0, 4))
print(b"aaaa".count(b'a', 0, 5))
print(b"aaaa".count(b'a', 1, 5))
print(b"aaaa".count(b'a', -1, 5))
print(b"abbabba".count(b"abba"))

def t():
    return True

print(b"0000".count(b'0', t()))
