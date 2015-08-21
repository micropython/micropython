print("".replace("a", "b"))
print("aaa".replace("b", "c"))
print("aaa".replace("a", "b", 0))
print("aaa".replace("a", "b", -5))
print("asdfasdf".replace("a", "b"))
print("aabbaabbaabbaa".replace("aa", "cc", 3))
print("a".replace("aa", "bb"))
print("testingtesting".replace("ing", ""))
print("testINGtesting".replace("ing", "ING!"))

print("".replace("", "1"))
print("A".replace("", "1"))
print("AB".replace("", "1"))
print("AB".replace("", "12"))

try:
    'abc'.replace(1, 2)
except TypeError:
    print('TypeError')

try:
    'abc'.replace('1', 2)
except TypeError:
    print('TypeError')
