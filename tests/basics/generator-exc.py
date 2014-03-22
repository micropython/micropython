# Test proper handling of exceptions within generator across yield
def gen():
    try:
        yield 1
        raise ValueError
    except ValueError:
        print("Caught")
    yield 2

for i in gen():
    print(i)
