# Test the marshal module, basic functionality.

try:
    import marshal

    (lambda: 0).__code__
except (AttributeError, ImportError):
    print("SKIP")
    raise SystemExit

ftype = type(lambda: 0)

# Test basic dumps and loads.
print(ftype(marshal.loads(marshal.dumps((lambda: a).__code__)), {"a": 4})())

# Test dumps of a result from compile().
ftype(marshal.loads(marshal.dumps(compile("print(a)", "", "exec"))), {"print": print, "a": 5})()

# Test marshalling a function with arguments.
print(ftype(marshal.loads(marshal.dumps((lambda x, y: x + y).__code__)), {})(1, 2))

# Test marshalling a function with default arguments.
print(ftype(marshal.loads(marshal.dumps((lambda x=0: x).__code__)), {})("arg"))

# Test marshalling a function containing constant objects (a tuple).
print(ftype(marshal.loads(marshal.dumps((lambda: (None, ...)).__code__)), {})())

# Test instantiating multiple code's with different globals dicts.
code = marshal.loads(marshal.dumps((lambda: a).__code__))
f1 = ftype(code, {"a": 1})
f2 = ftype(code, {"a": 2})
print(f1(), f2())

# Test unmarshallable object.
try:
    marshal.dumps(type)
except ValueError:
    print("ValueError")
