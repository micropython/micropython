# test compile builtin

try:
    compile
except NameError:
    print("SKIP")
    import sys
    sys.exit()

c = compile("print(x)", "file", "exec")

try:
    exec(c)
except NameError:
    print("NameError")

x = 1
exec(c)

exec(c, {"x":2})
exec(c, {}, {"x":3})
