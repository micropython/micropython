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
