# test returning from within a for loop

def f():
    for i in [1, 2, 3]:
        return i

print(f())
