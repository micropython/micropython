try:
    import micropython

    micropython.heap_lock
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

# Does the full test from heapalloc_exc_compressed.py but while the heap is
# locked (this can only work when the emergency exception buf is enabled).

# Some ports need to allocate heap for the emgergency exception buffer.
try:
    micropython.alloc_emergency_exception_buf(256)
except AttributeError:
    pass


def test():
    micropython.heap_lock()

    try:
        name()
    except NameError as e:
        print(type(e).__name__, e)

    try:
        raise 0
    except TypeError as e:
        print(type(e).__name__, e)

    try:
        name()
    except NameError as e:
        print(e.args[0])

    try:
        raise 0
    except TypeError as e:
        print(e.args[0])

    micropython.heap_unlock()


test()
