def gen():
    yield from gen()

try:
    print(list(gen()))
except RuntimeError:
    print("RuntimeError")
