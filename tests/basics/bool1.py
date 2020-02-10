# tests for bool objects

# basic logic
print(not False)
print(not True)
print(False and True)
print(False or True)

# unary operators
print(+True)
print(-True)

# comparison with itself
print(False == False)
print(False == True)
print(True == False)
print(True == True)
print(False != False)
print(False != True)
print(True != False)
print(True != True)

# comparison with integers
print(False == 0)
print(0 == False)
print(True == 1)
print(1 == True)
print(True == 2)
print(2 == True)
print(False != 0)
print(0 != False)
print(True != 1)
print(1 != True)
print(True != 2)
print(2 != True)

# unsupported unary op
try:
    len(False)
except TypeError:
    print('TypeError')
