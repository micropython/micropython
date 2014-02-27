mp_obj_t pyb_gpio(uint n_args, mp_obj_t *args);
mp_obj_t pyb_gpio_input(mp_obj_t arg_pin, mp_obj_t arg_mode);
mp_obj_t pyb_gpio_output(mp_obj_t arg_pin, mp_obj_t arg_mode);
void gpio_init(mp_obj_t mod);
