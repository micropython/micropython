# builtin len

print(len(()))
print(len((1,)))
print(len((1, 2)))

print(len([]))
x = [1, 2, 3]
print(len(x))

f = len
print(f({}))
print(f({1:2, 3:4}))
