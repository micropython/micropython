# MicroPython doesn't support tuple argument

try:
    "foobar".endswith(("bar", "sth"))
except TypeError:
    print("TypeError")
