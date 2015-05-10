class MyGen:

    def __init__(self):
        self.v = 0

    def __iter__(self):
        return self

    def __next__(self):
        self.v += 1
        if self.v > 5:
            raise StopIteration
        return self.v

def gen():
    yield from MyGen()

def gen2():
    yield from gen()

print(list(gen()))
print(list(gen2()))


class Incrementer:

    def __iter__(self):
        return self

    def __next__(self):
        return self.send(None)

    def send(self, val):
        if val is None:
            return "Incrementer initialized"
        return val + 1

def gen3():
    yield from Incrementer()

g = gen3()
print(next(g))
print(g.send(5))
print(g.send(100))


#
# Test proper handling of StopIteration vs other exceptions
#
class MyIter:
    def __iter__(self):
        return self
    def __next__(self):
        raise StopIteration(42)

def gen4():
    global ret
    ret = yield from MyIter()
    1//0

ret = None
try:
    print(list(gen4()))
except ZeroDivisionError:
    print("ZeroDivisionError")
print(ret)
