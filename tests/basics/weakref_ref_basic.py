# Test weakref.ref() functionality that doesn't require gc.collect().

try:
    import weakref
except ImportError:
    print("SKIP")
    raise SystemExit

# Cannot reference non-heap objects.
for value in (None, False, True, Ellipsis, 0, "", ()):
    try:
        weakref.ref(value)
    except TypeError:
        print(value, "TypeError")
