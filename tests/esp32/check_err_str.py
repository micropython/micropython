try:
    from esp32 import Partition as p
    import micropython
except ImportError:
    print("SKIP")
    raise SystemExit

# try some vanilla OSError to get std error code
try:
    open("this filedoesnotexist", "r")
    print("FAILED TO RAISE")
except OSError as e:
    print(e)

# try to make nvs partition bootable, which ain't gonna work
part = p.find(type=p.TYPE_DATA)[0]
fun = p.set_boot
try:
    fun(part)
    print("FAILED TO RAISE")
except OSError as e:
    print(e)

# same but with out of memory condition by locking the heap
exc = "FAILED TO RAISE"
micropython.heap_lock()
try:
    fun(part)
except OSError as e:
    exc = e
micropython.heap_unlock()
print("exc:", exc)  # exc empty due to no memory

# same again but having an emergency buffer
micropython.alloc_emergency_exception_buf(256)
exc = "FAILED TO RAISE"
micropython.heap_lock()
try:
    fun(part)
except Exception as e:
    exc = e
micropython.heap_unlock()
print(exc)
