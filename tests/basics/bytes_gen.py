# construct a bytes object from a generator
def gen():
    for i in range(4):
        yield i
print(bytes(gen()))
