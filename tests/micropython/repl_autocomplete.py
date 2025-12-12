# Check that micropython.repl_autocomplete works as expected.
import micropython

try:
    micropython.repl_autocomplete
except AttributeError:
    print("SKIP")
    raise SystemExit

# built-in import (complete includes space on end)
print(micropython.repl_autocomplete("impo"))

# local variable
x = '123'
print(micropython.repl_autocomplete("x.isdi"))

# multiple choices
y = "s"
print(micropython.repl_autocomplete("y.en"))
