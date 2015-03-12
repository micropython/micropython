print(','.join(()))
print(','.join(('a',)))
print(','.join(('a', 'b')))

print(','.join([]))
print(','.join(['a']))
print(','.join(['a', 'b']))

print(''.join(''))
print(''.join('abc'))
print(','.join('abc'))
print(','.join('abc' for i in range(5)))

print(b','.join([b'abc', b'123']))

try:
    print(b','.join(['abc', b'123']))
except TypeError:
    print("TypeError")

try:
    print(','.join([b'abc', b'123']))
except TypeError:
    print("TypeError")

# joined by the compiler
print("a" "b")
