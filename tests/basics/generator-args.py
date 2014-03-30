# Handling of "complicated" arg forms to generators
# https://github.com/micropython/micropython/issues/397
def gen(v=5):
    for i in range(v):
        yield i

print(list(gen()))
# Still not supported, ditto for *args and **kwargs
#print(list(gen(v=10)))
