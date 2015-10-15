def func1():
    try:
        return "it worked"
    finally:
        print("finally 1")

print(func1())


def func2():
    try:
        return "it worked"
    finally:
        print("finally 2")

def func3():
    try:
        s = func2()
        return s + ", did this work?"
    finally:
        print("finally 3")

print(func3())

# for loop within try-finally
def f():
    try:
        for i in [1, 2]:
            return i
    finally:
        print('finally')
print(f())

# multiple for loops within try-finally
def f():
    try:
        for i in [1, 2]:
            for j in [3, 4]:
                return (i, j)
    finally:
        print('finally')
print(f())

# multiple for loops and nested try-finally's
def f():
    try:
        for i in [1, 2]:
            for j in [3, 4]:
                try:
                    for k in [5, 6]:
                        for l in [7, 8]:
                            return (i, j, k, l)
                finally:
                    print('finally 2')
    finally:
        print('finally 1')
print(f())

# multiple for loops that are optimised, and nested try-finally's
def f():
    try:
        for i in range(1, 3):
            for j in range(3, 5):
                try:
                    for k in range(5, 7):
                        for l in range(7, 9):
                            return (i, j, k, l)
                finally:
                    print('finally 2')
    finally:
        print('finally 1')
print(f())
