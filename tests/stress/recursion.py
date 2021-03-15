def foo():
    foo()


try:
    foo()
except RuntimeError:
    print("RuntimeError")
