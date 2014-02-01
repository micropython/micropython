def func1():
    try:
        return "it worked"
    finally:
        print("finally 1")

print(func1())
