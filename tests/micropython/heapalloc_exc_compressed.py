try:
    import micropython

    micropython.heap_lock
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

# Tests both code paths for built-in exception raising.
# mp_obj_new_exception_msg_varg (exception requires decompression at raise-time to format)
# mp_obj_new_exception_msg (decompression can be deferred)

# NameError uses mp_obj_new_exception_msg_varg for NameError("name '%q' isn't defined")
# `raise 0` uses mp_obj_new_exception_msg for TypeError("exceptions must derive from BaseException")

# Tests that deferred decompression works both via print(e) and accessing the message directly via e.args.

# First test the regular case (can use heap for allocating the decompression buffer).
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

# Then test that it still works when the heap is locked (i.e. in ISR context).
micropython.heap_lock()

try:
    name()
except NameError as e:
    print(type(e).__name__)

try:
    raise 0
except TypeError as e:
    print(type(e).__name__)

micropython.heap_unlock()
