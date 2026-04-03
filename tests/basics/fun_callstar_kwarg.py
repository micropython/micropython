# A CPython syntax feature not supported by MicroPython. For more information
# see `this issue <https://github.com/micropython/micropython/issues/11439>`_.
# and tests/cpydiff/core_function_star.py

try:
    exec("f(y=1, *(3,))")
except SyntaxError as e:
    print("SyntaxError")
