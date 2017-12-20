# Test that we can raise and catch (preallocated) exception
# without memory allocation.
import micropython

e = ValueError("error")

def func():
    micropython.heap_lock()
    try:
        # This works as is because traceback is not allocated
        # if not possible (heap is locked, no memory). If heap
        # is not locked, this would allocate a traceback entry.
        # To avoid that, traceback should be warmed up (by raising
        # it once after creation) and then cleared before each
        # raise with:
        # e.__traceback__ = None
        raise e
    except Exception as e2:
        print(e2)
    micropython.heap_unlock()

func()
print("ok")
