def gen():
    try:
        yield 1
    except ValueError as e:
        print("got ValueError from upstream!", repr(e.args))
    yield "str1"
    raise TypeError

def gen2():
    print((yield from gen()))

g = gen2()
print(next(g))
print(g.throw(ValueError))
try:
    print(next(g))
except TypeError:
    print("got TypeError from downstream!")

# thrown value is caught and then generator returns normally
def gen():
    try:
        yield 123
    except ValueError:
        print('ValueError')
    # return normally after catching thrown exception
def gen2():
    yield from gen()
    yield 789
g = gen2()
print(next(g))
print(g.throw(ValueError))
