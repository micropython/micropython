from array import array

# construct from something with unknown length (requires generators)
print(array('i', (i for i in range(10))))
