import array

a = array.array('B', [1, 2, 3])
print(a, len(a))
i = array.array('I', [1, 2, 3])
print(i, len(i))
print(a[0])
print(i[-1])

# Empty arrays
print(len(array.array('h')))
print(array.array('i'))

