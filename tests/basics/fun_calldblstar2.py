# test passing a string object as the key for a keyword argument

try:
    exec
except NameError:
    print("SKIP")
    raise SystemExit

# they key in this dict is a string object and is not interned
args = {'thisisaverylongargumentname': 123}

# when this string is executed it will intern the keyword argument
exec("def foo(*,thisisaverylongargumentname=1):\n print(thisisaverylongargumentname)")

# test default arg
foo()

# the string from the dict should match the interned keyword argument
foo(**args)
