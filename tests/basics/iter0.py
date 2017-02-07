# builtin type that is not iterable
try:
    for i in 1:
        pass
except TypeError:
    print('TypeError')

# builtin type that is iterable, calling __next__ explicitly
print(iter(range(4)).__next__())
