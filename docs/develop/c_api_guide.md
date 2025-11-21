# MicroPython C API Comprehensive Integration Guide

**Author:** MicroPython Community
**Date:** 2025-11-21
**Version:** 1.0

## Table of Contents

1. [Introduction](#introduction)
2. [Core API Concepts](#core-api-concepts)
3. [Object System](#object-system)
4. [Type System](#type-system)
5. [Memory Management](#memory-management)
6. [Buffer Protocol](#buffer-protocol)
7. [Exception Handling](#exception-handling)
8. [Iterators and Generators](#iterators-and-generators)
9. [String Operations](#string-operations)
10. [Container Types](#container-types)
11. [Callbacks and Events](#callbacks-and-events)
12. [Hardware Integration](#hardware-integration)
13. [Thread Safety](#thread-safety)
14. [Performance Optimization](#performance-optimization)
15. [Debugging Techniques](#debugging-techniques)

---

## Introduction

This guide provides comprehensive coverage of the MicroPython C API with practical examples. It goes beyond the basic `cexample` module to cover advanced integration patterns, best practices, and real-world use cases.

### When to Use C Extensions

Use C extensions when you need to:
- Access hardware peripherals directly
- Integrate with existing C/C++ libraries
- Achieve performance-critical operations
- Implement low-level protocols
- Interface with platform-specific APIs

### Prerequisites

- Understanding of C programming
- Familiarity with Python and MicroPython
- Knowledge of the target hardware platform
- Understanding of memory management

---

## Core API Concepts

### The mp_obj_t Type

All Python objects in MicroPython are represented by the opaque `mp_obj_t` type:

```c
#include "py/runtime.h"
#include "py/obj.h"

// mp_obj_t is the fundamental type for all Python objects
// It can represent:
// - Small integers (encoded directly)
// - QSTR strings (encoded directly)
// - Pointers to heap-allocated objects

// Check if an object is a small int
bool is_small_int = mp_obj_is_small_int(obj);

// Convert small int to C int
if (mp_obj_is_small_int(obj)) {
    mp_int_t value = MP_OBJ_SMALL_INT_VALUE(obj);
}

// Create a small int from C int
mp_obj_t obj = MP_OBJ_NEW_SMALL_INT(42);
```

### Type Checking

Always validate types before operations:

```c
#include "py/runtime.h"

static mp_obj_t my_function(mp_obj_t arg) {
    // Method 1: Check specific type
    if (!mp_obj_is_int(arg)) {
        mp_raise_TypeError(MP_ERROR_TEXT("expected int"));
    }

    // Method 2: Check type compatibility
    if (!mp_obj_is_type(arg, &mp_type_list)) {
        mp_raise_TypeError(MP_ERROR_TEXT("expected list"));
    }

    // Method 3: Try conversion with error handling
    mp_int_t value = mp_obj_get_int(arg);  // Raises TypeError if not convertible

    return mp_const_none;
}
```

### Converting Between C and Python Types

```c
#include "py/runtime.h"

// Integer conversions
mp_int_t c_int = mp_obj_get_int(py_obj);           // Python int -> C int
mp_obj_t py_obj = mp_obj_new_int(c_int);           // C int -> Python int
mp_obj_t py_obj = mp_obj_new_int_from_uint(c_uint); // C uint -> Python int

// Float conversions (if enabled)
#if MICROPY_PY_BUILTINS_FLOAT
mp_float_t c_float = mp_obj_get_float(py_obj);     // Python float -> C float
mp_obj_t py_obj = mp_obj_new_float(c_float);       // C float -> Python float
#endif

// Boolean conversions
bool c_bool = mp_obj_is_true(py_obj);              // Python -> C bool
mp_obj_t py_bool = mp_obj_new_bool(c_bool);        // C bool -> Python bool

// String conversions
const char *str = mp_obj_str_get_str(py_obj);      // Python str -> C string (no copy)
size_t len;
const char *str = mp_obj_str_get_data(py_obj, &len); // Get string and length
mp_obj_t py_str = mp_obj_new_str("hello", 5);      // C string -> Python str
mp_obj_t py_str = mp_obj_new_str_from_cstr("hello"); // Null-terminated C string

// Bytes conversions
const byte *data = (const byte *)mp_obj_str_get_data(py_bytes, &len);
mp_obj_t py_bytes = mp_obj_new_bytes(data, len);   // C bytes -> Python bytes

// None, True, False
mp_obj_t none = mp_const_none;
mp_obj_t true_obj = mp_const_true;
mp_obj_t false_obj = mp_const_false;
```

---

## Object System

### Creating Custom Types

```c
#include "py/runtime.h"
#include "py/obj.h"

// 1. Define the object structure
typedef struct _my_device_obj_t {
    mp_obj_base_t base;      // MUST be first member
    uint32_t device_id;       // Custom fields
    uint8_t *buffer;
    size_t buffer_size;
    bool is_initialized;
} my_device_obj_t;

// 2. Implement methods
static mp_obj_t my_device_init(mp_obj_t self_in) {
    my_device_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Initialize hardware
    self->is_initialized = true;

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(my_device_init_obj, my_device_init);

static mp_obj_t my_device_read(mp_obj_t self_in) {
    my_device_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (!self->is_initialized) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("device not initialized"));
    }

    // Read from device
    return mp_obj_new_bytes(self->buffer, self->buffer_size);
}
static MP_DEFINE_CONST_FUN_OBJ_1(my_device_read_obj, my_device_read);

// 3. Create the type's methods dictionary
static const mp_rom_map_elem_t my_device_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&my_device_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&my_device_read_obj) },
};
static MP_DEFINE_CONST_DICT(my_device_locals_dict, my_device_locals_dict_table);

// 4. Implement constructor
static mp_obj_t my_device_make_new(const mp_obj_type_t *type, size_t n_args,
                                     size_t n_kw, const mp_obj_t *args) {
    // Check arguments
    mp_arg_check_num(n_args, n_kw, 1, 2, false);

    // Allocate object (automatically sets type)
    my_device_obj_t *self = mp_obj_malloc(my_device_obj_t, type);

    // Initialize fields
    self->device_id = mp_obj_get_int(args[0]);
    self->buffer_size = (n_args > 1) ? mp_obj_get_int(args[1]) : 256;
    self->buffer = m_new(uint8_t, self->buffer_size);
    self->is_initialized = false;

    return MP_OBJ_FROM_PTR(self);
}

// 5. Define the type
MP_DEFINE_CONST_OBJ_TYPE(
    my_device_type,
    MP_QSTR_Device,
    MP_TYPE_FLAG_NONE,
    make_new, my_device_make_new,
    locals_dict, &my_device_locals_dict
);
```

### Implementing Properties (Read/Write Attributes)

```c
// Property-style attribute access
static void my_device_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    my_device_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Handle 'status' property
    if (attr == MP_QSTR_status) {
        if (dest[0] == MP_OBJ_NULL) {
            // READ: dest[0] = MP_OBJ_NULL means load attribute
            dest[0] = mp_obj_new_int(self->is_initialized ? 1 : 0);
            return;
        } else if (dest[1] != MP_OBJ_NULL) {
            // WRITE: dest[0] = MP_OBJ_SENTINEL, dest[1] = value
            self->is_initialized = mp_obj_is_true(dest[1]);
            dest[0] = MP_OBJ_NULL; // Indicate success
            return;
        }
    }

    // Attribute not found, look in locals dict
    dest[1] = MP_OBJ_SENTINEL;
}

// Add to type definition:
MP_DEFINE_CONST_OBJ_TYPE(
    my_device_type,
    MP_QSTR_Device,
    MP_TYPE_FLAG_NONE,
    make_new, my_device_make_new,
    attr, my_device_attr,  // Add this
    locals_dict, &my_device_locals_dict
);
```

---

## Type System

### Implementing __repr__ and __str__

```c
static void my_device_print(const mp_print_t *print, mp_obj_t self_in,
                             mp_print_kind_t kind) {
    my_device_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (kind == PRINT_STR) {
        // __str__: user-friendly representation
        mp_printf(print, "Device(%u, %s)",
                  self->device_id,
                  self->is_initialized ? "ready" : "not ready");
    } else {
        // __repr__: developer representation
        mp_printf(print, "<Device id=%u buffer=%u bytes>",
                  self->device_id, self->buffer_size);
    }
}

// Add to type:
MP_DEFINE_CONST_OBJ_TYPE(
    my_device_type,
    MP_QSTR_Device,
    MP_TYPE_FLAG_NONE,
    make_new, my_device_make_new,
    print, my_device_print,  // Add this
    locals_dict, &my_device_locals_dict
);
```

### Implementing Binary Operations

```c
static mp_obj_t my_vec_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    if (!mp_obj_is_type(rhs_in, &my_vec_type)) {
        return MP_OBJ_NULL; // Operation not supported
    }

    my_vec_obj_t *lhs = MP_OBJ_TO_PTR(lhs_in);
    my_vec_obj_t *rhs = MP_OBJ_TO_PTR(rhs_in);

    switch (op) {
        case MP_BINARY_OP_ADD: {
            // Implement vector addition
            my_vec_obj_t *result = mp_obj_malloc(my_vec_obj_t, &my_vec_type);
            result->x = lhs->x + rhs->x;
            result->y = lhs->y + rhs->y;
            return MP_OBJ_FROM_PTR(result);
        }

        case MP_BINARY_OP_MULTIPLY: {
            // Implement dot product
            float dot = lhs->x * rhs->x + lhs->y * rhs->y;
            return mp_obj_new_float(dot);
        }

        case MP_BINARY_OP_EQUAL:
            return mp_obj_new_bool(lhs->x == rhs->x && lhs->y == rhs->y);

        default:
            return MP_OBJ_NULL; // Operation not supported
    }
}

// Add to type:
MP_DEFINE_CONST_OBJ_TYPE(
    my_vec_type,
    MP_QSTR_Vector,
    MP_TYPE_FLAG_NONE,
    make_new, my_vec_make_new,
    binary_op, my_vec_binary_op,  // Add this
    locals_dict, &my_vec_locals_dict
);
```

---

## Memory Management

### Allocation Best Practices

```c
#include "py/gc.h"
#include "py/runtime.h"

// GOOD: Use MicroPython allocators
uint8_t *buffer = m_new(uint8_t, 1024);          // Allocates 1024 bytes
uint8_t *buffer_zeroed = m_new0(uint8_t, 1024);  // Allocates and zeros
m_del(uint8_t, buffer, 1024);                     // Free memory

// GOOD: Reallocate
buffer = m_renew(uint8_t, buffer, old_size, new_size);

// BAD: Never use malloc/free directly in MicroPython code
// uint8_t *bad = malloc(1024);  // DON'T DO THIS

// GC-allocated objects (automatically managed)
my_device_obj_t *obj = mp_obj_malloc(my_device_obj_t, &my_device_type);
// No need to free - garbage collector handles it

// For temporary allocations in functions
static mp_obj_t process_data(mp_obj_t data_in) {
    // Stack-allocated for small, temporary data
    uint8_t temp[64];

    // Heap-allocated for larger data
    size_t size = 4096;
    uint8_t *large = m_new(uint8_t, size);

    // ... process data ...

    // Must free before returning
    m_del(uint8_t, large, size);

    return result;
}
```

### Error Handling with Cleanup

```c
static mp_obj_t safe_operation(mp_obj_t arg) {
    uint8_t *buffer = NULL;
    int fd = -1;
    mp_obj_t result = mp_const_none;

    // Allocate resources
    buffer = m_new(uint8_t, 1024);

    // Use nlr (non-local return) for exception-safe cleanup
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        // Protected code that might raise exceptions
        fd = open_device();
        read_device(fd, buffer, 1024);
        result = process_buffer(buffer, 1024);

        nlr_pop();
    } else {
        // Exception occurred - nlr.ret_val contains the exception
        // Cleanup will happen below, then re-raise
    }

    // Cleanup (always executed)
    if (fd >= 0) {
        close_device(fd);
    }
    if (buffer != NULL) {
        m_del(uint8_t, buffer, 1024);
    }

    // Re-raise exception if one occurred
    if (nlr.ret_val != NULL) {
        nlr_jump(nlr.ret_val);
    }

    return result;
}
```

---

## Buffer Protocol

### Reading Buffers

```c
#include "py/runtime.h"

static mp_obj_t process_buffer(mp_obj_t buf_in) {
    mp_buffer_info_t bufinfo;

    // Get buffer with read-only access
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    // bufinfo.buf is void* pointing to data
    // bufinfo.len is the length in bytes
    // bufinfo.typecode is the type ('B' for bytes, 'i' for ints, etc.)

    uint8_t *data = (uint8_t *)bufinfo.buf;
    size_t len = bufinfo.len;

    // Process data
    uint32_t sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum += data[i];
    }

    return mp_obj_new_int(sum);
}
```

### Writing to Buffers

```c
static mp_obj_t fill_buffer(mp_obj_t buf_in, mp_obj_t value_in) {
    mp_buffer_info_t bufinfo;

    // Get buffer with write access
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_WRITE);

    uint8_t value = mp_obj_get_int(value_in);
    uint8_t *data = (uint8_t *)bufinfo.buf;

    // Fill buffer
    memset(data, value, bufinfo.len);

    return mp_const_none;
}
```

### Implementing Buffer Protocol for Custom Types

```c
// Add to your type
static mp_int_t my_device_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo,
                                       mp_uint_t flags) {
    my_device_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (flags == MP_BUFFER_READ) {
        bufinfo->buf = self->buffer;
        bufinfo->len = self->buffer_size;
        bufinfo->typecode = 'B';  // Unsigned byte
        return 0;  // Success
    }

    // Write access not supported
    return 1;  // Failure
}

static MP_DEFINE_CONST_OBJ_TYPE(
    my_device_type,
    MP_QSTR_Device,
    MP_TYPE_FLAG_NONE,
    make_new, my_device_make_new,
    buffer, my_device_get_buffer,  // Add this
    locals_dict, &my_device_locals_dict
);
```

---

## Exception Handling

### Raising Exceptions

```c
#include "py/runtime.h"

// Standard exceptions
mp_raise_ValueError(MP_ERROR_TEXT("invalid value"));
mp_raise_TypeError(MP_ERROR_TEXT("wrong type"));
mp_raise_OSError(MP_EIO);  // I/O error
mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("operation failed"));

// With formatted message
mp_raise_msg_varg(&mp_type_ValueError,
                  MP_ERROR_TEXT("expected %d, got %d"),
                  expected, actual);

// Custom exception types
static MP_DEFINE_EXCEPTION(DeviceError, Exception)

// Raise custom exception
mp_obj_t exc = mp_obj_new_exception_msg(&DeviceError_type,
                                         MP_ERROR_TEXT("device timeout"));
nlr_raise(exc);
```

### Catching Exceptions

```c
static mp_obj_t call_with_exception_handling(mp_obj_t func, mp_obj_t arg) {
    nlr_buf_t nlr;

    if (nlr_push(&nlr) == 0) {
        // Call function that might raise
        mp_obj_t result = mp_call_function_1(func, arg);
        nlr_pop();
        return result;
    } else {
        // Exception occurred
        mp_obj_t exc = MP_OBJ_FROM_PTR(nlr.ret_val);

        // Check exception type
        if (mp_obj_is_subclass_fast(MP_OBJ_FROM_PTR(mp_obj_get_type(exc)),
                                     MP_OBJ_FROM_PTR(&mp_type_OSError))) {
            // Handle OSError
            mp_printf(&mp_plat_print, "Caught OSError\\n");
            return mp_const_none;
        }

        // Re-raise if not handled
        nlr_jump(nlr.ret_val);
    }
}
```

---

## Iterators and Generators

### Implementing an Iterator

```c
typedef struct _my_range_obj_t {
    mp_obj_base_t base;
    mp_int_t start;
    mp_int_t stop;
    mp_int_t step;
    mp_int_t current;
} my_range_obj_t;

static mp_obj_t my_range_iternext(mp_obj_t self_in) {
    my_range_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if ((self->step > 0 && self->current >= self->stop) ||
        (self->step < 0 && self->current <= self->stop)) {
        // End of iteration
        return MP_OBJ_STOP_ITERATION;
    }

    mp_int_t value = self->current;
    self->current += self->step;

    return mp_obj_new_int(value);
}

static mp_obj_t my_range_make_new(const mp_obj_type_t *type, size_t n_args,
                                    size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 3, false);

    my_range_obj_t *self = mp_obj_malloc(my_range_obj_t, type);

    if (n_args == 1) {
        self->start = 0;
        self->stop = mp_obj_get_int(args[0]);
        self->step = 1;
    } else if (n_args == 2) {
        self->start = mp_obj_get_int(args[0]);
        self->stop = mp_obj_get_int(args[1]);
        self->step = 1;
    } else {
        self->start = mp_obj_get_int(args[0]);
        self->stop = mp_obj_get_int(args[1]);
        self->step = mp_obj_get_int(args[2]);
    }

    self->current = self->start;

    return MP_OBJ_FROM_PTR(self);
}

static MP_DEFINE_CONST_OBJ_TYPE(
    my_range_type,
    MP_QSTR_MyRange,
    MP_TYPE_FLAG_ITER_IS_ITERNEXT,
    make_new, my_range_make_new,
    iter, my_range_iternext
);
```

---

## String Operations

### Working with Strings

```c
// Create strings
mp_obj_t str1 = mp_obj_new_str("hello", 5);
mp_obj_t str2 = mp_obj_new_str_from_cstr("world");  // Null-terminated

// Get string data
size_t len;
const char *data = mp_obj_str_get_data(str1, &len);

// String comparison
if (mp_obj_str_equal(str1, str2)) {
    // Strings are equal
}

// Format strings
mp_obj_t formatted = mp_obj_str_format(2, args, kwargs);

// String concatenation
mp_obj_t parts[2] = {str1, str2};
mp_obj_t result = mp_obj_str_join(MP_OBJ_NEW_QSTR(MP_QSTR_),
                                   2, parts);
```

---

## Container Types

### Working with Lists

```c
// Create list
mp_obj_t list = mp_obj_new_list(0, NULL);  // Empty list
mp_obj_t items[3] = {
    mp_obj_new_int(1),
    mp_obj_new_int(2),
    mp_obj_new_int(3)
};
mp_obj_t list_with_items = mp_obj_new_list(3, items);

// Append to list
mp_obj_list_append(list, mp_obj_new_int(42));

// Get list length
size_t len = mp_obj_list_len(list);

// Access list items
mp_obj_t item = mp_obj_list_get(list, 0);  // Get first item
mp_obj_list_set(list, 0, mp_obj_new_int(100));  // Set first item

// Iterate over list
mp_obj_t *items;
size_t len;
mp_obj_list_get(list, &len, &items);
for (size_t i = 0; i < len; i++) {
    // Process items[i]
}
```

### Working with Dictionaries

```c
// Create dict
mp_obj_t dict = mp_obj_new_dict(0);

// Store key-value pairs
mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_name),
                  mp_obj_new_str_from_cstr("Device"));
mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_id),
                  mp_obj_new_int(42));

// Get value
mp_obj_t value = mp_obj_dict_get(dict, MP_OBJ_NEW_QSTR(MP_QSTR_name));

// Delete key
mp_obj_dict_delete(dict, MP_OBJ_NEW_QSTR(MP_QSTR_id));
```

---

## Callbacks and Events

### Implementing Callbacks

```c
typedef struct _my_sensor_obj_t {
    mp_obj_base_t base;
    mp_obj_t callback;  // Store Python callback
    uint32_t threshold;
} my_sensor_obj_t;

static mp_obj_t my_sensor_set_callback(mp_obj_t self_in, mp_obj_t callback) {
    my_sensor_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (!mp_obj_is_callable(callback) && callback != mp_const_none) {
        mp_raise_TypeError(MP_ERROR_TEXT("callback must be callable"));
    }

    self->callback = callback;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(my_sensor_set_callback_obj, my_sensor_set_callback);

// Trigger callback from C code (e.g., interrupt handler)
static void trigger_callback(my_sensor_obj_t *self, uint32_t value) {
    if (self->callback != mp_const_none && value > self->threshold) {
        // Call Python callback with value
        mp_call_function_1(self->callback, mp_obj_new_int(value));
    }
}
```

---

## Hardware Integration

### GPIO Example

```c
#include "py/runtime.h"
#include "py/mphal.h"

// Hardware register definitions (example for ARM Cortex-M)
#define GPIO_BASE 0x40020000
#define GPIO_MODER   (*(volatile uint32_t *)(GPIO_BASE + 0x00))
#define GPIO_ODR     (*(volatile uint32_t *)(GPIO_BASE + 0x14))
#define GPIO_IDR     (*(volatile uint32_t *)(GPIO_BASE + 0x10))

typedef struct _machine_pin_obj_t {
    mp_obj_base_t base;
    uint8_t pin;
    uint8_t mode;  // 0=INPUT, 1=OUTPUT
} machine_pin_obj_t;

static mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 1) {
        // GET: Read pin value
        uint32_t value = (GPIO_IDR >> self->pin) & 1;
        return mp_obj_new_int(value);
    } else {
        // SET: Write pin value
        uint32_t value = mp_obj_get_int(args[1]);
        if (value) {
            GPIO_ODR |= (1 << self->pin);  // Set high
        } else {
            GPIO_ODR &= ~(1 << self->pin);  // Set low
        }
        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_value_obj, 1, 2, machine_pin_value);
```

### SPI Example

```c
typedef struct _machine_spi_obj_t {
    mp_obj_base_t base;
    void *spi_base;  // Hardware SPI peripheral base address
    uint32_t baudrate;
} machine_spi_obj_t;

static mp_obj_t machine_spi_write(mp_obj_t self_in, mp_obj_t buf_in) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    uint8_t *data = (uint8_t *)bufinfo.buf;

    for (size_t i = 0; i < bufinfo.len; i++) {
        // Wait for TX buffer empty
        while (!(SPI_SR(self->spi_base) & SPI_SR_TXE));

        // Send byte
        SPI_DR(self->spi_base) = data[i];

        // Wait for transmission complete
        while (!(SPI_SR(self->spi_base) & SPI_SR_RXNE));

        // Read and discard received byte
        (void)SPI_DR(self->spi_base);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_spi_write_obj, machine_spi_write);
```

---

## Thread Safety

### Interrupt-Safe Operations

```c
#include "py/mphal.h"

typedef struct _shared_data_t {
    volatile uint32_t counter;
    volatile bool flag;
} shared_data_t;

static shared_data_t g_shared;

// Access from Python (non-interrupt context)
static mp_obj_t increment_counter(void) {
    // Disable interrupts during critical section
    mp_uint_t state = MICROPY_BEGIN_ATOMIC_SECTION();

    g_shared.counter++;

    MICROPY_END_ATOMIC_SECTION(state);

    return mp_obj_new_int(g_shared.counter);
}

// Access from interrupt handler
void EXTI0_IRQHandler(void) {
    // Interrupt context - keep minimal
    g_shared.counter++;
    g_shared.flag = true;

    // Clear interrupt flag
    EXTI->PR = (1 << 0);
}
```

---

## Performance Optimization

### Inline Assembly

```c
#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)

// Example: Fast GPIO toggle using inline assembly
static inline void fast_gpio_toggle(uint32_t pin) {
    __asm__ volatile (
        "ldr r1, =0x40020014\\n"  // GPIO_ODR address
        "ldr r2, [r1]\\n"          // Read current value
        "eor r2, r2, %0\\n"        // XOR with pin mask
        "str r2, [r1]\\n"          // Write back
        :
        : "r" (1 << pin)
        : "r1", "r2"
    );
}

#endif
```

### Using @micropython.native

```c
// In your module, you can hint that functions should be compiled natively
// This is done in Python, but you can provide helper C functions that are
// optimized for native compilation

static mp_obj_t fast_sum(mp_obj_t list_in) {
    // This function is designed to be fast when compiled with @micropython.native
    mp_obj_t *items;
    size_t len;
    mp_obj_list_get(list_in, &len, &items);

    mp_int_t sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum += mp_obj_get_int(items[i]);
    }

    return mp_obj_new_int(sum);
}
```

---

## Debugging Techniques

### Debug Printing

```c
#include "py/mpprint.h"

// Print to console
mp_printf(&mp_plat_print, "Debug: value = %d\\n", value);

// Conditional debugging
#if MICROPY_DEBUG_VERBOSE
    mp_printf(&mp_plat_print, "Verbose: entering function\\n");
#endif

// Print object representation
mp_obj_print_helper(&mp_plat_print, obj, PRINT_REPR);
```

### Assertions and Checks

```c
#include <assert.h>

static mp_obj_t my_function(mp_obj_t arg) {
    // Runtime assertions (enabled in debug builds)
    assert(mp_obj_is_int(arg));

    // Always-on checks for production
    if (!mp_obj_is_type(arg, &expected_type)) {
        mp_raise_TypeError(MP_ERROR_TEXT("unexpected type"));
    }

    return mp_const_none;
}
```

### Memory Debugging

```c
// Check GC stats
#include "py/gc.h"

static mp_obj_t get_mem_info(void) {
    gc_info_t info;
    gc_info(&info);

    mp_printf(&mp_plat_print, "Total: %u bytes\\n", info.total);
    mp_printf(&mp_plat_print, "Used: %u bytes\\n", info.used);
    mp_printf(&mp_plat_print, "Free: %u bytes\\n", info.free);

    return mp_const_none;
}
```

---

## Complete Example: I2C Sensor Driver

```c
#include "py/runtime.h"
#include "py/mphal.h"

// I2C Sensor driver complete example
#define SENSOR_I2C_ADDR 0x48
#define SENSOR_REG_TEMP 0x00
#define SENSOR_REG_CONFIG 0x01

typedef struct _sensor_obj_t {
    mp_obj_base_t base;
    uint8_t i2c_addr;
    void *i2c_base;
    mp_obj_t callback;
    float threshold;
} sensor_obj_t;

// I2C communication helper
static int i2c_read_reg(sensor_obj_t *self, uint8_t reg, uint8_t *data, size_t len) {
    // Simplified I2C read - actual implementation depends on hardware
    // 1. Send START
    // 2. Send device address + WRITE
    // 3. Send register address
    // 4. Send RESTART
    // 5. Send device address + READ
    // 6. Read data
    // 7. Send STOP

    return 0;  // Success
}

static int i2c_write_reg(sensor_obj_t *self, uint8_t reg, const uint8_t *data, size_t len) {
    // Simplified I2C write
    return 0;  // Success
}

// Sensor methods
static mp_obj_t sensor_read_temp(mp_obj_t self_in) {
    sensor_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint8_t data[2];

    if (i2c_read_reg(self, SENSOR_REG_TEMP, data, 2) < 0) {
        mp_raise_OSError(MP_EIO);
    }

    // Convert raw data to temperature (example formula)
    int16_t raw = (data[0] << 8) | data[1];
    float temp = raw / 256.0f;

    // Trigger callback if threshold exceeded
    if (self->callback != mp_const_none && temp > self->threshold) {
        mp_call_function_1(self->callback, mp_obj_new_float(temp));
    }

    return mp_obj_new_float(temp);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sensor_read_temp_obj, sensor_read_temp);

static mp_obj_t sensor_set_threshold(mp_obj_t self_in, mp_obj_t thresh_in) {
    sensor_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->threshold = mp_obj_get_float(thresh_in);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sensor_set_threshold_obj, sensor_set_threshold);

static mp_obj_t sensor_on_alert(mp_obj_t self_in, mp_obj_t callback) {
    sensor_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (callback != mp_const_none && !mp_obj_is_callable(callback)) {
        mp_raise_TypeError(MP_ERROR_TEXT("callback must be callable"));
    }

    self->callback = callback;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sensor_on_alert_obj, sensor_on_alert);

// Type definition
static const mp_rom_map_elem_t sensor_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read_temp), MP_ROM_PTR(&sensor_read_temp_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_threshold), MP_ROM_PTR(&sensor_set_threshold_obj) },
    { MP_ROM_QSTR(MP_QSTR_on_alert), MP_ROM_PTR(&sensor_on_alert_obj) },
};
static MP_DEFINE_CONST_DICT(sensor_locals_dict, sensor_locals_dict_table);

static mp_obj_t sensor_make_new(const mp_obj_type_t *type, size_t n_args,
                                  size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);

    sensor_obj_t *self = mp_obj_malloc(sensor_obj_t, type);

    self->i2c_addr = (n_args > 0) ? mp_obj_get_int(args[0]) : SENSOR_I2C_ADDR;
    self->i2c_base = (void *)I2C1_BASE;  // Platform-specific
    self->callback = mp_const_none;
    self->threshold = 100.0f;

    // Initialize sensor
    uint8_t config = 0x60;  // Example config
    i2c_write_reg(self, SENSOR_REG_CONFIG, &config, 1);

    return MP_OBJ_FROM_PTR(self);
}

static MP_DEFINE_CONST_OBJ_TYPE(
    sensor_type,
    MP_QSTR_TempSensor,
    MP_TYPE_FLAG_NONE,
    make_new, sensor_make_new,
    locals_dict, &sensor_locals_dict
);

// Module definition
static const mp_rom_map_elem_t sensor_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_tempsensor) },
    { MP_ROM_QSTR(MP_QSTR_TempSensor), MP_ROM_PTR(&sensor_type) },
};
static MP_DEFINE_CONST_DICT(sensor_module_globals, sensor_module_globals_table);

const mp_obj_module_t sensor_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&sensor_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_tempsensor, sensor_user_cmodule);
```

---

## Best Practices Summary

1. **Always validate inputs** before use
2. **Use MicroPython allocators** (m_new, m_del) instead of malloc/free
3. **Clean up resources** in error paths using nlr
4. **Check return values** from MicroPython API calls
5. **Use const correctness** (MP_ROM_QSTR, MP_ROM_PTR)
6. **Document your code** with comments explaining Python behavior
7. **Test edge cases** (None, empty containers, negative numbers)
8. **Keep interrupt handlers minimal** - defer work to main context
9. **Use buffer protocol** for efficient data transfer
10. **Follow MicroPython coding style** (match existing code)

---

## Additional Resources

- MicroPython source code: https://github.com/micropython/micropython
- Official documentation: https://docs.micropython.org
- Forum: https://forum.micropython.org
- Example modules in `examples/usercmodule/`
- Port-specific examples in `ports/*/modules/`

---

**End of Guide**
