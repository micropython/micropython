# String operations which don't require allocation
import micropython

micropython.heap_lock()

b"" + b""
b"" + b"1"
b"2" + b""

"" + ""
"" + "1"
"2" + ""

micropython.heap_unlock()
