extern const mp_obj_type_t pyb_pin_type;
extern const mp_obj_type_t pyb_adc_type;
extern const mp_obj_type_t pyb_rtc_type;
extern const mp_obj_type_t pyb_i2c_type;

typedef struct _pyb_pin_obj_t {
    mp_obj_base_t base;
    uint16_t pin_id;
    uint16_t phys_port;
    uint32_t periph;
    uint16_t func;
} pyb_pin_obj_t;

uint mp_obj_get_pin(mp_obj_t pin_in);
pyb_pin_obj_t *mp_obj_get_pin_obj(mp_obj_t pin_in);
int pin_get(uint pin);
void pin_set(uint pin, int value);
