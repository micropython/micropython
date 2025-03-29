# Check that micropython.repl_autocomplete works as expected.
import micropython

try:
    micropython.repl_autocomplete
except AttributeError:
    print("SKIP")
    raise SystemExit

print(micropython.repl_autocomplete("micropytho"))

print(micropython.repl_autocomplete("impo"))

import math
print(micropython.repl_autocomplete("m"))
