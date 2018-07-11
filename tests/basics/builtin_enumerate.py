print(list(enumerate([])))
print(list(enumerate([1, 2, 3])))
print(list(enumerate([1, 2, 3], 5)))
print(list(enumerate([1, 2, 3], -5)))
print(list(enumerate(range(100))))

# specifying args with keywords
print(list(enumerate([1, 2, 3], start=1)))
print(list(enumerate(iterable=[1, 2, 3])))
print(list(enumerate(iterable=[1, 2, 3], start=1)))

# check handling of extra positional args (exercises some logic in mp_arg_parse_all)
# don't print anything because it doesn't error with MICROPY_CPYTHON_COMPAT disabled
try:
    enumerate([], 1, 2)
except TypeError:
    pass
