# test custom builtin module

try:
    import cexample
except ImportError:
    print("SKIP")
    raise SystemExit

print(cexample)

d = dir(cexample)
d.index("add_ints")

print(cexample.add_ints(1, 3))
