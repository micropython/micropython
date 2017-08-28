# MicroPython doesn't support tuple argument

try:
    "foobar".startswith(("foo", "sth"))
except TypeError:
    print("TypeError")
