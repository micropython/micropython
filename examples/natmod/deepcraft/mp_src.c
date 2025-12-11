#include "py/dynruntime.h"
#include "model.h"
#include <string.h>

typedef struct _dc_obj_t {
    mp_obj_base_t base;
    bool model_state; 
    uint8_t model_in_dim;
    uint8_t model_out_dim;
} dc_obj_t;

mp_obj_t get_model_input_dim(mp_obj_t self_in){
    dc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(self->model_in_dim);
}

mp_obj_t get_model_output_dim(mp_obj_t self_in){
    dc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(self->model_out_dim);
}

mp_obj_t init(mp_obj_t self_in){
    dc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->model_state = true;
    self->model_in_dim = IMAI_DATA_IN_COUNT;
    self->model_out_dim = IMAI_DATA_OUT_COUNT;
    IMAI_init();
    return mp_const_none;
}

mp_obj_t enqueue(mp_obj_t self_in, const mp_obj_t data_in_obj){
    dc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    
    // Check if model is initialized
    if(!self->model_state){
        mp_raise_ValueError("Model should be initialized first.");
    }

    float data_in[self->model_in_dim];
    mp_obj_t *data_in_items;
    size_t len;
    mp_obj_get_array(data_in_obj, &len, &data_in_items);

    if (len != self->model_in_dim) {
        mp_raise_ValueError("data_in must be a list of floats with size matching to input dimensions to model. Check using get_model_input_dim().");
    }

    for (int i = 0; i < self->model_in_dim; i++) {
        data_in[i] = mp_obj_get_float(data_in_items[i]);
    }
    int result = IMAI_enqueue(data_in);
    return MP_OBJ_NEW_SMALL_INT(result);
}

mp_obj_t dequeue(mp_obj_t self_in, mp_obj_t data_out_obj) {
    dc_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_buffer_info_t buf_info;
    mp_get_buffer(data_out_obj, &buf_info, MP_BUFFER_WRITE);
    float *data_out = (float *)buf_info.buf;
    size_t len = buf_info.len / sizeof(float);

    if (len != self->model_out_dim) {
        mp_raise_ValueError("data_out must be a list of floats with size matching to output dimensions of model. Check using get_model_output_dim().");
    }
    
    int result = IMAI_dequeue(data_out);
    if (result == 0) {
        return MP_OBJ_NEW_SMALL_INT(result);
    } else if (result == -1) {
        return MP_OBJ_NEW_SMALL_INT(result);
    } else if (result == -2) {
        mp_raise_ValueError(MP_ERROR_TEXT("Internal memory allocation error"));
    }
    return MP_OBJ_NEW_SMALL_INT(result);
}


static const mp_obj_fun_builtin_fixed_t init_obj = {
    .base = { &mp_type_fun_builtin_1 },
    .fun._1 = (mp_fun_1_t)init,
};


static const mp_obj_fun_builtin_fixed_t enqueue_obj = {
    .base = { &mp_type_fun_builtin_2 },
    .fun._2 = (mp_fun_2_t)enqueue,
};

static const mp_obj_fun_builtin_fixed_t dequeue_obj = {
    .base = { &mp_type_fun_builtin_2 },
    .fun._2 = (mp_fun_2_t)dequeue,
};

static const mp_obj_fun_builtin_fixed_t get_model_input_dim_obj = {
    .base = { &mp_type_fun_builtin_1 },
    .fun._1 = (mp_fun_1_t)get_model_input_dim,
};

static const mp_obj_fun_builtin_fixed_t get_model_output_dim_obj = {
    .base = { &mp_type_fun_builtin_1 },
    .fun._1 = (mp_fun_1_t)get_model_output_dim,
};
