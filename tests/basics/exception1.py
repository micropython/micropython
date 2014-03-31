print(repr(IndexError()))
print(str(IndexError()))

print(repr(IndexError("foo")))
print(str(IndexError("foo")))

a = IndexError(1, "test", [100, 200])
print(repr(a))
print(str(a))
print(a.args)

s = StopIteration()
print(s.value)
s = StopIteration(1, 2, 3)
print(s.value)
