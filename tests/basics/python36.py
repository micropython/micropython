# tests for things that only Python 3.6 supports

try:
    eval
except NameError:
    print("SKIP")
    raise SystemExit

# underscores in numeric literals
print(eval('100_000'))
print(eval('0b1010_0101'))
print(eval('0xff_ff'))

# underscore supported by int constructor
print(int('1_2_3'))
print(int('0o1_2_3', 8))
