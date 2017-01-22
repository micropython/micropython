# test MicroPython-specific features of array.array
import array

# arrays of objects
a = array.array('O')
a.append(1)
print(a[0])

# arrays of pointers
a = array.array('P')
a.append(1)
print(a[0])
