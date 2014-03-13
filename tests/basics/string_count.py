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

def t():
    return True

print("0000".count('0', t()))
