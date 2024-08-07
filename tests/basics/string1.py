# basic strings

# literals
print('abc')
print(r'abc')
print(u'abc')
print(repr('\a\b\t\n\v\f\r'))

# construction
print(str())
print(str('abc'))

# inplace addition
x = 'abc'
print(x)
x += 'def'
print(x)

# binary ops
print('123' + "456")
print('123' * 5)
try:
    '123' * '1'
except TypeError:
    print('TypeError')
try:
    '123' + 1
except TypeError:
    print('TypeError')

# subscription
print('abc'[1])
print('abc'[-1])
try:
    'abc'[100]
except IndexError:
    print('IndexError')
try:
    'abc'[-4]
except IndexError:
    print('IndexError')

# iter
print(list('str'))

# comparison
print('123' + '789' == '123789')
print('a' + 'b' != 'a' + 'b ')
print('1' + '2' > '2')
print('1' + '2' < '2')

# printing quote char in string
print(repr('\'\"'))
