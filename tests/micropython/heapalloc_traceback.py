# test that we can generate a traceback without allocating

import micropython
import sys

# preallocate exception instance with some room for a traceback
global_exc = StopIteration()
try:
    raise global_exc
except:
    pass

def test():
    global global_exc
    global_exc.__traceback__ = None
    try:
        raise global_exc
    except StopIteration as e:
        sys.print_exception(e)

# call test() with heap allocation disabled
micropython.heap_lock()
test()
micropython.heap_unlock()
