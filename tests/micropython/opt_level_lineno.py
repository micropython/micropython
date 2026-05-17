# test micropython.opt_level() and line numbers

try:
    import micropython
except ImportError:
    print("SKIP")
    raise SystemExit

# check that level 3 doesn't store line numbers
# the expected output is that any line is printed as "line 1"
micropython.opt_level(3)

# force bytecode emitter, because native emitter doesn't store line numbers
exec("""
@micropython.bytecode
def f():
    try:
        xyz
    except NameError as er:
        import sys
        sys.print_exception(er)
f()
""")
