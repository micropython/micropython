# Test the marshal module, with functions that have children.

try:
    import marshal

    (lambda: 0).__code__
except (AttributeError, ImportError):
    print("SKIP")
    raise SystemExit


def f_with_child():
    def child():
        return a

    return child


def f_with_child_defargs():
    def child(a="default"):
        return a

    return child


def f_with_child_closure():
    a = "closure 1"

    def child():
        return a

    a = "closure 2"
    return child


def f_with_child_closure_defargs():
    a = "closure defargs 1"

    def child(b="defargs default"):
        return (a, b)

    a = "closure defargs 1"
    return child


def f_with_list_comprehension(a):
    return [i + a for i in range(4)]


ftype = type(lambda: 0)

# Test function with a child.
f = ftype(marshal.loads(marshal.dumps(f_with_child.__code__)), {"a": "global"})
print(f()())

# Test function with a child that has default arguments.
f = ftype(marshal.loads(marshal.dumps(f_with_child_defargs.__code__)), {})
print(f()())
print(f()("non-default"))

# Test function with a child that is a closure.
f = ftype(marshal.loads(marshal.dumps(f_with_child_closure.__code__)), {})
print(f()())

# Test function with a child that is a closure and has default arguments.
f = ftype(marshal.loads(marshal.dumps(f_with_child_closure_defargs.__code__)), {})
print(f()())
print(f()("defargs non-default"))

# Test function with a list comprehension (which will be an anonymous child).
f = ftype(marshal.loads(marshal.dumps(f_with_list_comprehension.__code__)), {})
print(f(10))

# Test child within a module (the outer scope).
code = compile("def child(a): return a", "", "exec")
f = marshal.loads(marshal.dumps(code))
ctx = {}
exec(f, ctx)
print(ctx["child"]("arg"))
