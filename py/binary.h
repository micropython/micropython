// Use special typecode to differentiate repr() of bytearray vs array.array('B')
// (underlyingly they're same).
#define BYTEARRAY_TYPECODE 0

int mp_binary_get_size(char typecode);
mp_obj_t mp_binary_get_val(char typecode, void *p, int index);
void mp_binary_set_val(char typecode, void *p, int index, mp_obj_t val_in);
