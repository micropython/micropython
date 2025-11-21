/**
 * Advanced Buffer Protocol Example
 *
 * Demonstrates:
 * - Reading and writing buffers efficiently
 * - Implementing buffer protocol for custom types
 * - Zero-copy data transfer
 * - Working with different buffer types (bytes, bytearray, memoryview)
 */

#include "py/runtime.h"
#include "py/binary.h"

//=============================================================================
// Example 1: Ring Buffer with Buffer Protocol
//=============================================================================

typedef struct _ringbuffer_obj_t {
    mp_obj_base_t base;
    uint8_t *buffer;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t count;
} ringbuffer_obj_t;

static mp_obj_t ringbuffer_make_new(const mp_obj_type_t *type, size_t n_args,
                                      size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    ringbuffer_obj_t *self = mp_obj_malloc(ringbuffer_obj_t, type);

    self->capacity = mp_obj_get_int(args[0]);
    self->buffer = m_new(uint8_t, self->capacity);
    self->head = 0;
    self->tail = 0;
    self->count = 0;

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t ringbuffer_write(mp_obj_t self_in, mp_obj_t data_in) {
    ringbuffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data_in, &bufinfo, MP_BUFFER_READ);

    uint8_t *data = (uint8_t *)bufinfo.buf;
    size_t written = 0;

    for (size_t i = 0; i < bufinfo.len; i++) {
        if (self->count >= self->capacity) {
            break;  // Buffer full
        }

        self->buffer[self->head] = data[i];
        self->head = (self->head + 1) % self->capacity;
        self->count++;
        written++;
    }

    return mp_obj_new_int(written);
}
static MP_DEFINE_CONST_FUN_OBJ_2(ringbuffer_write_obj, ringbuffer_write);

static mp_obj_t ringbuffer_read(size_t n_args, const mp_obj_t *args) {
    ringbuffer_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    size_t to_read = (n_args > 1) ? mp_obj_get_int(args[1]) : self->count;

    // Limit to available data
    if (to_read > self->count) {
        to_read = self->count;
    }

    // Create bytes object with read data
    vstr_t vstr;
    vstr_init_len(&vstr, to_read);

    for (size_t i = 0; i < to_read; i++) {
        vstr.buf[i] = self->buffer[self->tail];
        self->tail = (self->tail + 1) % self->capacity;
        self->count--;
    }

    return mp_obj_new_bytes_from_vstr(&vstr);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ringbuffer_read_obj, 1, 2, ringbuffer_read);

static mp_obj_t ringbuffer_available(mp_obj_t self_in) {
    ringbuffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(self->count);
}
static MP_DEFINE_CONST_FUN_OBJ_1(ringbuffer_available_obj, ringbuffer_available);

// Implement buffer protocol for zero-copy access
static mp_int_t ringbuffer_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo,
                                        mp_uint_t flags) {
    ringbuffer_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Only support read-only contiguous access
    if (flags != MP_BUFFER_READ) {
        return 1;  // Failure
    }

    bufinfo->buf = self->buffer;
    bufinfo->len = self->capacity;
    bufinfo->typecode = 'B';  // Unsigned bytes

    return 0;  // Success
}

static const mp_rom_map_elem_t ringbuffer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&ringbuffer_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&ringbuffer_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_available), MP_ROM_PTR(&ringbuffer_available_obj) },
};
static MP_DEFINE_CONST_DICT(ringbuffer_locals_dict, ringbuffer_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    ringbuffer_type,
    MP_QSTR_RingBuffer,
    MP_TYPE_FLAG_NONE,
    make_new, ringbuffer_make_new,
    buffer, ringbuffer_get_buffer,
    locals_dict, &ringbuffer_locals_dict
);

//=============================================================================
// Example 2: Efficient Buffer Processing Functions
//=============================================================================

// CRC32 calculation on buffer
static mp_obj_t buffer_crc32(mp_obj_t buf_in) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    uint32_t crc = 0xFFFFFFFF;
    uint8_t *data = (uint8_t *)bufinfo.buf;

    for (size_t i = 0; i < bufinfo.len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
        }
    }

    return mp_obj_new_int_from_uint(~crc);
}
static MP_DEFINE_CONST_FUN_OBJ_1(buffer_crc32_obj, buffer_crc32);

// XOR two buffers
static mp_obj_t buffer_xor(mp_obj_t buf1_in, mp_obj_t buf2_in) {
    mp_buffer_info_t buf1, buf2;
    mp_get_buffer_raise(buf1_in, &buf1, MP_BUFFER_READ);
    mp_get_buffer_raise(buf2_in, &buf2, MP_BUFFER_READ);

    if (buf1.len != buf2.len) {
        mp_raise_ValueError(MP_ERROR_TEXT("buffers must be same length"));
    }

    // Create result buffer
    vstr_t vstr;
    vstr_init_len(&vstr, buf1.len);

    uint8_t *data1 = (uint8_t *)buf1.buf;
    uint8_t *data2 = (uint8_t *)buf2.buf;

    for (size_t i = 0; i < buf1.len; i++) {
        vstr.buf[i] = data1[i] ^ data2[i];
    }

    return mp_obj_new_bytes_from_vstr(&vstr);
}
static MP_DEFINE_CONST_FUN_OBJ_2(buffer_xor_obj, buffer_xor);

// Find pattern in buffer (returns index or -1)
static mp_obj_t buffer_find(mp_obj_t haystack_in, mp_obj_t needle_in) {
    mp_buffer_info_t haystack, needle;
    mp_get_buffer_raise(haystack_in, &haystack, MP_BUFFER_READ);
    mp_get_buffer_raise(needle_in, &needle, MP_BUFFER_READ);

    if (needle.len == 0 || needle.len > haystack.len) {
        return mp_obj_new_int(-1);
    }

    uint8_t *h = (uint8_t *)haystack.buf;
    uint8_t *n = (uint8_t *)needle.buf;

    for (size_t i = 0; i <= haystack.len - needle.len; i++) {
        if (memcmp(h + i, n, needle.len) == 0) {
            return mp_obj_new_int(i);
        }
    }

    return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_2(buffer_find_obj, buffer_find);

//=============================================================================
// Module Definition
//=============================================================================

static const mp_rom_map_elem_t buffer_example_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_buffer_example) },
    { MP_ROM_QSTR(MP_QSTR_RingBuffer), MP_ROM_PTR(&ringbuffer_type) },
    { MP_ROM_QSTR(MP_QSTR_crc32), MP_ROM_PTR(&buffer_crc32_obj) },
    { MP_ROM_QSTR(MP_QSTR_xor), MP_ROM_PTR(&buffer_xor_obj) },
    { MP_ROM_QSTR(MP_QSTR_find), MP_ROM_PTR(&buffer_find_obj) },
};
static MP_DEFINE_CONST_DICT(buffer_example_module_globals, buffer_example_module_globals_table);

const mp_obj_module_t buffer_example_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&buffer_example_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_buffer_example, buffer_example_user_cmodule);
