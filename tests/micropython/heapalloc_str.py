# String operations which don't require allocation
import micropython

micropython.heap_lock()

# Concatenating empty string returns original string
b"" + b""
b"" + b"1"
b"2" + b""

"" + ""
"" + "1"
"2" + ""

# If no replacements done, returns original string
"foo".replace(",", "_")

micropython.heap_unlock()
