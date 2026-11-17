# Test weakref.finalize() functionality that doesn't require gc.collect().

try:
    import weakref
except ImportError:
    print("SKIP")
    raise SystemExit

# Cannot reference non-heap objects.
for value in (None, False, True, Ellipsis, 0, "", ()):
    try:
        weakref.finalize(value, lambda: None)
    except TypeError:
        print(value, "TypeError")


# Convert (obj, func, args, kwargs) so CPython and MicroPython have a chance to match.
def convert_4_tuple(values):
    if values is None:
        return None
    return (type(values[0]).__name__, type(values[1]), values[2], values[3])


class A:
    def __str__(self):
        return "<A object>"


print("test alive, peek, detach")
a = A()
f = weakref.finalize(a, lambda: None, 1, 2, kwarg=3)
print("alive", f.alive)
print("peek", convert_4_tuple(f.peek()))
print("detach", convert_4_tuple(f.detach()))
print("alive", f.alive)
print("peek", convert_4_tuple(f.peek()))
print("detach", convert_4_tuple(f.detach()))
print("call", f())
a = None

print("test alive, peek, call")
a = A()
f = weakref.finalize(a, lambda *args, **kwargs: (args, kwargs), 1, 2, kwarg=3)
print("alive", f.alive)
print("peek", convert_4_tuple(f.peek()))
print("call", f())
print("alive", f.alive)
print("peek", convert_4_tuple(f.peek()))
print("call", f())
print("detach", convert_4_tuple(f.detach()))

print("test call which raises exception")
a = A()
f = weakref.finalize(a, lambda: 1 / 0)
try:
    f()
except ZeroDivisionError as er:
    print("call ZeroDivisionError")
