import micropython

# Does the full test from heapalloc_exc_compressed.py but while the heap is
# locked (this can only work when the emergency exception buf is enabled).

# Some ports need to allocate heap for the emgergency exception buffer.
try:
    micropython.alloc_emergency_exception_buf(256)
except AttributeError:
    pass

a = set()


def test():
    micropython.heap_lock()

    try:
        name()
    except NameError as e:
        print(type(e).__name__, e)

    try:
        a.pop()
    except KeyError as e:
        print(type(e).__name__, e)

    try:
        name()
    except NameError as e:
        print(e.args[0])

    try:
        a.pop()
    except KeyError as e:
        print(e.args[0])

    micropython.heap_unlock()


test()
