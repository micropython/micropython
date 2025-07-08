# Test function.__code__ attributes not available with MICROPY_PY_BUILTINS_CODE <= MICROPY_PY_BUILTINS_CODE_BASIC

try:
    (lambda: 0).__code__.co_code
except AttributeError:
    print("SKIP")
    raise SystemExit

def f(x, y):
    a = x + y
    b = x - y
    return a * b

code = f.__code__

print(type(code.co_code)) # both bytes (but mpy and cpy have different instruction sets)
print(code.co_consts) # (not necessarily the same set, but in this function they are)
print(code.co_filename.rsplit('/')[-1]) # same terminal filename but might be different paths on other ports
print(type(code.co_firstlineno)) # both ints (but mpy points to first line inside, cpy points to declaration)
print(code.co_name)
print(iter(code.co_names) is not None) # both iterable (but mpy returns dict with names as keys, cpy only the names; and not necessarily the same set)

co_lines = code.co_lines()

l = list(co_lines)
first_start = l[0][0]
last_end = l[-1][1]
print(first_start) # co_lines should start at the start of the bytecode
print(len(code.co_code) - last_end) # and end at the end of the bytecode

prev_end = 0
for start, end, line_no in l:
    if end != prev_end:
        print("non-contiguous")
        break # the offset ranges should be contiguous
    prev_end = end
else:
    print("contiguous")


