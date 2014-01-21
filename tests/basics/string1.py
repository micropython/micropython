# basic strings

x = 'abc'
print(x)

x += 'def'
print(x)

print('123' + "456")

print('123' * 5)

print('abc'[1])
print('abc'[-1])
try:
    'abc'[100]
except IndexError:
    print('caught')
try:
    'abc'[-4]
except IndexError:
    print('caught2')

# iter
print(list('str'))

print('123' + '789' == '123789')
print('a' + 'b' != 'a' + 'b ')

# Not implemented so far
# print('1' + '2' > '2')
# print('1' + '2' < '2')
