# test builtin hash function, on generators

def gen():
    yield

print(type(hash(gen)))
print(type(hash(gen())))
