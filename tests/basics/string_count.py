print("".count(""))
print("".count("a"))
print("a".count(""))
print("a".count("a"))
print("a".count("b"))
print("b".count("a"))

print("aaa".count(""))
print("aaa".count("a"))
print("aaa".count("aa"))
print("aaa".count("aaa"))
print("aaa".count("aaaa"))

print("aaaa".count(""))
print("aaaa".count("a"))
print("aaaa".count("aa"))
print("aaaa".count("aaa"))
print("aaaa".count("aaaa"))
print("aaaa".count("aaaaa"))

print("aaa".count("", 1))
print("aaa".count("", 2))
print("aaa".count("", 3))

print("aaa".count("", 1, 2))

print("asdfasdfaaa".count("asdf", -100))
print("asdfasdfaaa".count("asdf", -8))
print("asdf".count('s', True))
print("asdf".count('a', True))
print("asdf".count('a', False))
print("asdf".count('a', 1 == 2))
print("hello world".count('l'))
print("hello world".count('l', 5))
print("hello world".count('l', 3))
print("hello world".count('z', 3, 6))
print("aaaa".count('a'))
print("aaaa".count('a', 0, 3))
print("aaaa".count('a', 0, 4))
print("aaaa".count('a', 0, 5))
print("aaaa".count('a', 1, 5))
print("aaaa".count('a', -1, 5))
print("abbabba".count("abba"))

def t():
    return True

print("0000".count('0', t()))

try:
    'abc'.count(1)
except TypeError:
    print('TypeError')
