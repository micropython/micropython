# With MICROPY_CPYTHON_COMPAT, the "return" statement can only appear in a
# function.
# Otherwise (in minimal builds), it ends execution of a module/class.

try:
    exec
except NameError:
    print("SKIP")
    raise SystemExit

try:
    exec('return; print("this should not be executed.")')
    # if we get here then MICROPY_CPYTHON_COMPAT is disabled and test
    # should be skipped.
    print("SKIP")
    raise SystemExit
except SyntaxError:
    print('SyntaxError')
