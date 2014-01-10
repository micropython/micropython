# basic for loop

def f():
    for x in range(2):
        for y in range(2):
            for z in range(2):
                print(x, y, z)

f()

# range with negative step
for i in range(3, -1, -1):
    print(i)

a = -1
# range with non-constant step - we optimize constant steps, so this
# will be executed differently
for i in range(3, -1, a):
    print(i)
