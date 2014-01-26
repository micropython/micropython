typedef struct {
    mp_obj_base_t base;
    PinName pin;
    gpio_t gpio;
} hal_gpio_obj_t;

void hal_init_Gpio_class(mp_obj_t module_obj);

