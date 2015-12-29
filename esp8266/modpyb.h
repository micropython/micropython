extern const mp_obj_type_t pyb_pin_type;
extern const mp_obj_type_t pyb_adc_type;
extern const mp_obj_type_t pyb_rtc_type;

uint mp_obj_get_pin(mp_obj_t pin_in);
int pin_get(uint pin);
void pin_set(uint pin, int value);
