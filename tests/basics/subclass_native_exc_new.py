# test subclassing exceptions and providing __new__


class Dummy(BaseException):
    pass


class GoodException(BaseException):
    def __new__(cls, *args, **kwargs):
        print("GoodException __new__")
        return Dummy(*args, **kwargs)


class BadException(BaseException):
    def __new__(cls, *args, **kwargs):
        print("BadException __new__")
        return 1


try:
    raise GoodException("good message")
except BaseException as good:
    print(type(good), good.args[0])

try:
    raise BadException("bad message")
except Exception as bad:
    # Should be TypeError 'exceptions must derive from BaseException'
    print(type(bad), bad.args[0])

try:

    def gen():
        yield

    gen().throw(BadException)
except Exception as genbad:
    # Should be TypeError 'exceptions must derive from BaseException'
    print(type(genbad), genbad.args[0])
