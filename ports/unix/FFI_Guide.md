# MicroPython FFI (Foreign Function Interface) Guide

## Overview

The FFI module allows MicroPython to call C functions from shared libraries directly, enabling high-performance operations without requiring MicroPython recompilation. This is particularly useful for:

- Calling system libraries (libc, libm, etc.)
- Interfacing with hardware through C libraries
- Accelerating performance-critical code
- Reusing existing C libraries

## Prerequisites

```bash
# FFI requires libffi development headers
sudo apt install libffi-dev
```

## Basic Concepts

### Type Codes

FFI uses single-character type codes based on Python's `struct` module:

| Code | C Type | Description |
|------|--------|-------------|
| `v` | void | Return type only |
| `b` | int8_t | Signed 8-bit integer |
| `B` | uint8_t | Unsigned 8-bit integer |
| `h` | int16_t | Signed 16-bit integer |
| `H` | uint16_t | Unsigned 16-bit integer |
| `i` | int32_t | Signed 32-bit integer |
| `I` | uint32_t | Unsigned 32-bit integer |
| `l` | long | Signed long integer |
| `L` | unsigned long | Unsigned long integer |
| `q` | int64_t | Signed 64-bit integer |
| `Q` | uint64_t | Unsigned 64-bit integer |
| `f` | float | Single-precision float |
| `d` | double | Double-precision float |
| `p` | void* | Pointer (writable memory) |
| `P` | const void* | Pointer (read-only memory) |
| `s` | char* | String (as arg: pointer, as return: allocates string) |
| `O` | mp_obj_t | MicroPython object (passed as-is) |
| `C` | function pointer | Callback function |

## Loading Libraries

### Opening a Shared Library

```python
import ffi

# Load system library
libc = ffi.open("libc.so.6")

# Load custom library
mylib = ffi.open("./libmylib.so")

# Load current process (access main executable symbols)
main = ffi.open(None)
```

### Closing Libraries

```python
libc.close()  # Free library resources
```

## Calling Functions

### Basic Function Declaration

```python
# Syntax: lib.func(return_type, function_name, argument_types)
time = libc.func("i", "time", "p")

# Call the function
current_time = time(None)
print("UNIX timestamp:", current_time)
```

### Functions with Multiple Arguments

```python
# int open(const char *pathname, int flags)
open_func = libc.func("i", "open", "si")

# Call with arguments
fd = open_func("/tmp/test.txt", 0)  # O_RDONLY = 0
```

### Functions with Floating Point

```python
libm = ffi.open("libm.so.6")

# double sqrt(double x)
sqrt = libm.func("d", "sqrt", "d")
result = sqrt(16.0)
print("sqrt(16.0) =", result)  # 4.0

# double pow(double base, double exp)
pow_func = libm.func("d", "pow", "dd")
result = pow_func(2.0, 8.0)
print("2^8 =", result)  # 256.0
```

## Working with Variables

### Accessing Global Variables

```python
# Get errno variable
errno = libc.var("i", "errno")

# Read value
print("errno value:", errno.get())

# Set value
errno.set(0)
```

## Pointers and Memory

### Passing Buffers

```python
# Create a buffer
buf = bytearray(1024)

# Pass to C function
# ssize_t read(int fd, void *buf, size_t count)
read_func = libc.func("l", "read", "ipl")
bytes_read = read_func(fd, buf, len(buf))
```

### Creating Buffers from Pointers

```python
# Get a pointer from C
ptr = some_c_func_returning_pointer()

# Create Python bytearray view
arr = ffi.as_bytearray(ptr, size)
print(arr)
```

## Callbacks (Python → C)

### Creating Callbacks

```python
import ffi

libc = ffi.open("libc.so.6")

# Define comparison function for qsort
def compare(pa, pb):
    import uctypes
    a = uctypes.bytearray_at(pa, 1)[0]
    b = uctypes.bytearray_at(pb, 1)[0]
    return a - b

# Create FFI callback
# Syntax: ffi.callback(return_type, python_func, arg_types)
cmp_callback = ffi.callback("i", compare, "PP")

# Use with qsort
# void qsort(void *base, size_t nmemb, size_t size,
#            int (*compar)(const void *, const void *))
qsort = libc.func("v", "qsort", "piiC")

data = bytearray(b"dcba")
qsort(data, len(data), 1, cmp_callback)
print(data)  # bytearray(b'abcd')
```

### Thread-Safe Callbacks

For callbacks that may be called from different threads:

```python
callback = ffi.callback("i", my_func, "ii", lock=True)
```

## Advanced Examples

### Example 1: Fast String Processing with C

```python
import ffi

libc = ffi.open("libc.so.6")

# strlen: size_t strlen(const char *s)
strlen = libc.func("l", "strlen", "s")

# memcpy: void *memcpy(void *dest, const void *src, size_t n)
memcpy = libc.func("p", "memcpy", "ppl")

# Test strlen
s = "Hello, World!"
length = strlen(s)
print(f"Length of '{s}' = {length}")

# Test memcpy
src = bytearray(b"Source")
dst = bytearray(6)
memcpy(dst, src, 6)
print("Copied:", dst)
```

### Example 2: System Calls

```python
import ffi

libc = ffi.open("libc.so.6")

# pid_t getpid(void)
getpid = libc.func("i", "getpid", "")
print("Process ID:", getpid())

# int gethostname(char *name, size_t len)
gethostname = libc.func("i", "gethostname", "pl")
hostname = bytearray(256)
gethostname(hostname, len(hostname))
print("Hostname:", hostname.decode().rstrip('\x00'))
```

### Example 3: File Operations

```python
import ffi

libc = ffi.open("libc.so.6")

# Declare functions
open_func = libc.func("i", "open", "si")
close_func = libc.func("i", "close", "i")
read_func = libc.func("l", "read", "ipl")
write_func = libc.func("l", "write", "ipl")

# Open file
O_RDONLY = 0
O_WRONLY = 1
O_CREAT = 64
O_TRUNC = 512

fd = open_func("/tmp/test.txt", O_WRONLY | O_CREAT | O_TRUNC)
if fd < 0:
    print("Failed to open file")
else:
    # Write data
    data = b"Hello from FFI!\n"
    bytes_written = write_func(fd, data, len(data))
    print(f"Wrote {bytes_written} bytes")

    close_func(fd)
```

### Example 4: Math Library Functions

```python
import ffi

libm = ffi.open("libm.so.6")

# Declare math functions
sin = libm.func("d", "sin", "d")
cos = libm.func("d", "cos", "d")
tan = libm.func("d", "tan", "d")
log = libm.func("d", "log", "d")
exp = libm.func("d", "exp", "d")

import math as pymath

# Compare with Python math
angle = 0.785398  # π/4
print("sin(π/4):")
print("  FFI:", sin(angle))
print("  Python:", pymath.sin(angle))

print("cos(π/4):")
print("  FFI:", cos(angle))
print("  Python:", pymath.cos(angle))
```

### Example 5: Custom C Library

First, create a C library:

```c
// mylib.c
#include <stdint.h>

// Fast array sum
int64_t array_sum(int32_t *arr, size_t len) {
    int64_t sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum += arr[i];
    }
    return sum;
}

// Fast array multiply
void array_multiply(int32_t *arr, size_t len, int32_t factor) {
    for (size_t i = 0; i < len; i++) {
        arr[i] *= factor;
    }
}
```

Compile it:
```bash
gcc -shared -fPIC -o libmylib.so mylib.c
```

Use from MicroPython:
```python
import ffi
import array

mylib = ffi.open("./libmylib.so")

# Declare functions
array_sum = mylib.func("q", "array_sum", "pl")
array_multiply = mylib.func("v", "array_multiply", "pli")

# Create array
data = array.array('i', [1, 2, 3, 4, 5])

# Call C function
total = array_sum(data, len(data))
print("Sum:", total)  # 15

# Multiply all elements by 2
array_multiply(data, len(data), 2)
print("After multiply:", list(data))  # [2, 4, 6, 8, 10]
```

### Example 6: Using GPIO via sysfs (Fast Alternative)

```python
import ffi

libc = ffi.open("libc.so.6")

# File operations
open_func = libc.func("i", "open", "si")
close_func = libc.func("i", "close", "i")
write_func = libc.func("l", "write", "ipl")
read_func = libc.func("l", "read", "ipl")

O_RDONLY = 0
O_WRONLY = 1

class FastGPIO:
    def __init__(self, pin):
        self.pin = pin
        self.value_fd = None

        # Export pin
        fd = open_func("/sys/class/gpio/export", O_WRONLY)
        pin_str = str(pin).encode()
        write_func(fd, pin_str, len(pin_str))
        close_func(fd)

        # Open value file
        value_path = f"/sys/class/gpio/gpio{pin}/value"
        self.value_fd = open_func(value_path, O_RDWR)

    def set(self, value):
        val = b"1" if value else b"0"
        write_func(self.value_fd, val, 1)

    def get(self):
        buf = bytearray(1)
        read_func(self.value_fd, buf, 1)
        return buf[0] == ord('1')

    def close(self):
        if self.value_fd:
            close_func(self.value_fd)

# Usage
gpio = FastGPIO(23)
gpio.set(1)  # Set high
print("GPIO value:", gpio.get())
gpio.close()
```

## Performance Tips

1. **Reuse function objects**: Declaring a function is expensive; do it once
2. **Use appropriate types**: Match C types exactly to avoid conversions
3. **Batch operations**: Call C functions with buffers rather than individual values
4. **Use callbacks wisely**: Python → C callbacks have overhead; minimize calls

## Common Pitfalls

### 1. Memory Management

C functions don't manage Python memory:

```python
# WRONG: buf will be garbage collected
def read_file_wrong(fd):
    buf = bytearray(1024)
    return read_func(fd, buf, len(buf))

# CORRECT: Keep reference
buf = bytearray(1024)
bytes_read = read_func(fd, buf, len(buf))
result = bytes(buf[:bytes_read])
```

### 2. String Encoding

```python
# Strings must be bytes or bytearray
path = "/tmp/file.txt"
fd = open_func(path, 0)  # WRONG

fd = open_func(path.encode(), 0)  # CORRECT
```

### 3. Type Mismatches

```python
# size_t on 64-bit is 'L', not 'i'
read_func = libc.func("l", "read", "ipL")  # CORRECT for 64-bit
```

## Debugging

### Enable FFI Debug Mode

```python
# Check function signature
print(my_func)  # Shows <ffifunc 0x...>

# Verify library loading
print(libc)  # Shows <ffimod 0x...>
```

### Common Errors

- `OSError: [Errno 2]`: Library not found
- `ValueError: error in ffi_prep_cif`: Incorrect type signature
- `TypeError: don't know how to pass object`: Unsupported argument type

## Best Practices

1. **Wrap C libraries in Python classes** for better API
2. **Handle errors** from C functions (check return values)
3. **Document type signatures** in comments
4. **Test with small examples** before complex code
5. **Use `uctypes` module** for complex structures

## See Also

- `/home/user/micropython/examples/unix/ffi_example.py` - Basic example
- `/home/user/micropython/tests/ports/unix/ffi_*.py` - Test suite
- [libffi documentation](https://sourceware.org/libffi/)
- [Python ctypes documentation](https://docs.python.org/3/library/ctypes.html) (similar concepts)
