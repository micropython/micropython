typedef struct _mp_obj_str_t {
    mp_obj_base_t base;
    // XXX here we assume the hash size is 16 bits (it is at the moment; see qstr.c)
    machine_uint_t hash : 16;
    // len == number of bytes used in data, alloc = len + 1 because (at the moment) we also append a null byte
    machine_uint_t len : 16;
    const byte *data;
} mp_obj_str_t;

#define MP_DEFINE_STR_OBJ(obj_name, str) mp_obj_str_t obj_name = {{&mp_type_str}, 0, sizeof(str) - 1, (const byte*)str};

mp_obj_t mp_obj_str_format(uint n_args, const mp_obj_t *args);
