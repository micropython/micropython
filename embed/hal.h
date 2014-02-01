typedef struct {
  const char *const_name;
  machine_int_t const_val;
} hal_int_const;

void hal_target_init_Gpio_class(mp_obj_t gpio_class_type);

