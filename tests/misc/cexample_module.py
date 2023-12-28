# test custom builtin module

try:
    import cexample
except ImportError:
    print("SKIP")
    raise SystemExit

print(cexample)
print(cexample.__name__)

d = dir(cexample)
d.index("add_ints")
d.index("Timer")

print(cexample.add_ints(1, 3))
