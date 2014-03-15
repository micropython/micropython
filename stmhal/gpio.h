mp_obj_t pyb_gpio(uint n_args, mp_obj_t *args);
mp_obj_t pyb_gpio_output(uint n_args, mp_obj_t *args);
mp_obj_t pyb_gpio_input(uint n_args, mp_obj_t *args);

MP_DECLARE_CONST_FUN_OBJ(pyb_gpio_obj);
MP_DECLARE_CONST_FUN_OBJ(pyb_gpio_input_obj);
MP_DECLARE_CONST_FUN_OBJ(pyb_gpio_output_obj);
