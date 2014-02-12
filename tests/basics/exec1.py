print(exec("def foo(): return 42"))
print(foo())

d = {}
exec("def bar(): return 84", d)
print(d["bar"]())
