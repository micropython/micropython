class BadIter:
    def __iter__(self):
        return object()


try:
    iter(BadIter())
except TypeError:
    print("TypeError")


class BadListIter:
    def __iter__(self):
        return []


try:
    iter(BadListIter())
except TypeError:
    print("TypeError")


class FakeNext:
    def __getattr__(self, name):
        if name == "__next__":
            return lambda: 42
        raise AttributeError


class BadDynamicIter:
    def __iter__(self):
        return FakeNext()


try:
    iter(BadDynamicIter())
except TypeError:
    print("TypeError")


class Iterator:
    def __next__(self):
        return 42


class GoodIter:
    def __iter__(self):
        return Iterator()


print(next(iter(GoodIter())))
