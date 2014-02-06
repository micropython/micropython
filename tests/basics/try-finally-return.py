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
