# test some extreme cases of allocating exceptions and tracebacks

try:
    import micropython
except ImportError:
    print("SKIP")
    raise SystemExit

# Check for stackless build, which can't call functions without
# allocating a frame on the heap.
try:

    def stackless():
        pass

    micropython.heap_lock()
    stackless()
    micropython.heap_unlock()
except RuntimeError:
    print("SKIP")
    raise SystemExit

# some ports need to allocate heap for the emergency exception
try:
    micropython.alloc_emergency_exception_buf(256)
except AttributeError:
    pass


def main():
    # create an exception with many args while heap is locked
    # should revert to empty tuple for args
    micropython.heap_lock()
    e = Exception(
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
    )
    micropython.heap_unlock()
    print(repr(e))

    # create an exception with a long formatted error message while heap is locked
    # should use emergency exception buffer and truncate the message
    def f():
        pass

    micropython.heap_lock()
    try:
        f(
            abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz=1
        )
    except Exception as er:
        e = er
    micropython.heap_unlock()
    print(repr(e)[:10])

    # create an exception with a long formatted error message while heap is low
    # should use the heap and truncate the message
    lst = []
    while 1:
        try:
            lst = [lst]
        except MemoryError:
            break
    try:
        f(
            abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz=1
        )
    except Exception as er:
        e = er
    while lst:
        lst[0], lst = None, lst[0]  # unlink lists to free up heap
    print(repr(e)[:10])

    # raise a deep exception with the heap locked
    # should use emergency exception and be unable to resize traceback array
    def g():
        g()

    micropython.heap_lock()
    try:
        g()
    except Exception as er:
        e = er
    micropython.heap_unlock()
    print(repr(e)[:13])

    # create an exception on the heap with some traceback on the heap, but then
    # raise it with the heap locked so it can't allocate any more traceback
    exc = Exception("my exception")
    try:
        raise exc
    except:
        pass

    def h(e):
        raise e

    micropython.heap_lock()
    try:
        h(exc)
    except Exception as er:
        e = er
    micropython.heap_unlock()
    print(repr(e))


main()
