# MicroPython Advanced C Module Examples

This directory contains advanced examples demonstrating real-world C integration patterns with MicroPython.

## Overview

These examples go beyond the basic `cexample` module to show:
- Advanced buffer protocol usage
- Event-driven programming with callbacks
- Interrupt-safe programming patterns
- Zero-copy data transfer
- Production-ready error handling

## Examples Included

### 1. Buffer Protocol Example (`buffer_example.c`)

Demonstrates advanced buffer manipulation and zero-copy operations.

**Modules:**
- `RingBuffer` - Circular buffer implementation with buffer protocol support
- `crc32()` - Efficient CRC32 calculation on buffers
- `xor()` - XOR two buffers
- `find()` - Pattern matching in buffers

**Usage:**
```python
from buffer_example import RingBuffer, crc32, xor, find

# Create ring buffer
rb = RingBuffer(256)
rb.write(b"Hello, World!")
data = rb.read(5)  # b"Hello"
available = rb.available()  # 8

# Calculate CRC32
checksum = crc32(b"test data")

# XOR two buffers
result = xor(b"\\x01\\x02\\x03", b"\\xff\\xfe\\xfd")

# Find pattern
index = find(b"Hello World", b"World")  # Returns 6
```

**Key Concepts:**
- Implementing buffer protocol for custom types
- Using `mp_buffer_info_t` for efficient buffer access
- Zero-copy data access with `memoryview`
- Working with `bytes`, `bytearray`, and custom buffers

---

### 2. Callback and Event Handling Example (`callback_example.c`)

Shows how to implement event-driven patterns with Python callbacks.

**Modules:**
- `EventManager` - Multiple callback registration and event emission
- `CallbackTimer` - Timer with callback support
- `EventQueue` - Interrupt-safe event queueing

**Usage:**
```python
from callback_example import EventManager, CallbackTimer, EventQueue

# Event manager with multiple callbacks
em = EventManager()

def handler1(data):
    print("Handler 1:", data)

def handler2(data):
    print("Handler 2:", data)

id1 = em.on(handler1)
id2 = em.on(handler2)

em.emit("test event")  # Calls both handlers
em.off(id1)  # Remove first handler

# Timer with callback
def tick():
    print("Tick!")

timer = CallbackTimer(1000)  # 1 second interval
timer.callback(tick)
timer.start()

# In main loop:
while True:
    timer.tick()  # Check and call callback if interval elapsed
    time.sleep_ms(10)

# Interrupt-safe event queue
eq = EventQueue()

def process_event(event_id):
    print(f"Event: {event_id}")

eq.set_callback(process_event)

# In ISR (simulated):
# eq.push_isr(event_id)

# In main loop:
eq.process()  # Process all pending events
```

**Key Concepts:**
- Storing Python callbacks in C structures
- Calling Python functions from C
- Validating callables with `mp_obj_is_callable()`
- Interrupt-safe programming with atomic sections
- Event queueing for deferred processing

---

## Building

### Using Make

```bash
cd micropython/ports/unix
make USER_C_MODULES=../../examples/usercmodule/advanced_examples
```

### Using CMake

```bash
cd micropython/ports/rp2
make USER_C_MODULES=../../examples/usercmodule/advanced_examples/micropython.cmake
```

## Testing

After building, test the modules:

```python
# Test buffer_example
import buffer_example

rb = buffer_example.RingBuffer(10)
rb.write(b"12345")
print(rb.read(3))  # Should print b"123"

data = b"Hello, World!"
crc = buffer_example.crc32(data)
print(f"CRC32: 0x{crc:08x}")

# Test callback_example
import callback_example

em = callback_example.EventManager()
count = [0]

def handler(msg):
    count[0] += 1
    print(f"Event {count[0]}: {msg}")

em.on(handler)
em.emit("test 1")
em.emit("test 2")
print(f"Total events: {em.count()}")
```

## Learning Path

1. **Start with buffer_example.c**
   - Understand `mp_buffer_info_t`
   - Learn buffer protocol implementation
   - See efficient memory operations

2. **Move to callback_example.c**
   - Learn callback storage and invocation
   - Understand event-driven patterns
   - See interrupt-safe programming

3. **Review the comprehensive C API guide**
   - See `docs/develop/c_api_guide.md`
   - Learn all MicroPython C API patterns
   - Study best practices and optimization

## Common Patterns

### Safe Callback Invocation

```c
static void call_callback_safe(mp_obj_t callback, mp_obj_t arg) {
    if (callback != mp_const_none) {
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            mp_call_function_1(callback, arg);
            nlr_pop();
        } else {
            // Exception occurred in callback - handle or log
            mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(nlr.ret_val));
        }
    }
}
```

### Interrupt-Safe Data Access

```c
static uint32_t read_shared_data(volatile uint32_t *data) {
    mp_uint_t state = MICROPY_BEGIN_ATOMIC_SECTION();
    uint32_t value = *data;
    MICROPY_END_ATOMIC_SECTION(state);
    return value;
}
```

### Buffer Protocol Implementation

```c
static mp_int_t my_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo,
                                mp_uint_t flags) {
    my_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (flags == MP_BUFFER_READ) {
        bufinfo->buf = self->data;
        bufinfo->len = self->size;
        bufinfo->typecode = 'B';
        return 0;
    }

    return 1;  // Unsupported operation
}
```

## Performance Tips

1. **Use buffer protocol for large data**
   - Avoids copying data between Python and C
   - Direct memory access
   - Works with `memoryview` for zero-copy slicing

2. **Minimize Python object creation**
   - Reuse objects when possible
   - Use small ints for common values
   - Pre-allocate buffers

3. **Keep ISRs minimal**
   - Only update volatile variables
   - Queue events for processing in main context
   - Defer callback invocation

4. **Use atomic sections correctly**
   - Keep critical sections short
   - Only protect shared data access
   - Don't call Python code in atomic sections

## Debugging

Enable debug output:

```c
#include "py/mpprint.h"

#if MICROPY_DEBUG_VERBOSE
    mp_printf(&mp_plat_print, "Debug: value=%d\\n", value);
#endif
```

Check memory usage:

```python
import gc
gc.collect()
print(gc.mem_free())
```

## Further Resources

- **Main documentation:** `docs/develop/c_api_guide.md` - Comprehensive C API reference
- **Basic examples:** `examples/usercmodule/cexample/` - Simple starting point
- **Native modules:** `examples/natmod/` - Dynamically loadable .mpy modules
- **Embedding:** `examples/embedding/` - Embedding MicroPython in C applications

## Contributing

When adding new examples:
1. Follow MicroPython coding style
2. Add comprehensive comments
3. Include usage examples in docstrings
4. Test on multiple platforms
5. Update this README

## License

MIT License - same as MicroPython
