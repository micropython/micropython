# test subclassing a native exception


class MyExc(Exception):
    pass


e = MyExc(100, "Some error")
print(e)
print(repr(e))
print(e.args)

try:
    raise MyExc("Some error", 1)
except MyExc as e:
    print("Caught exception:", repr(e))

try:
    raise MyExc("Some error2", 2)
except Exception as e:
    print("Caught exception:", repr(e))

try:
    raise MyExc("Some error2")
except:
    print("Caught user exception")


class MyStopIteration(StopIteration):
    pass


print(MyStopIteration().value)
print(MyStopIteration(1).value)


class Iter:
    def __iter__(self):
        return self

    def __next__(self):
        # This exception will stop the "yield from", with a value of 3
        raise MyStopIteration(3)


def gen():
    print((yield from Iter()))
    return 4


try:
    next(gen())
except StopIteration as er:
    print(er.args)


class MyOSError(OSError):
    pass


print(MyOSError().errno)
print(MyOSError(1, "msg").errno)
