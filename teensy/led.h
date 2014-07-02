typedef enum {
    PYB_LED_BUILTIN = 1,
} pyb_led_t;

void led_init(void);
void led_state(pyb_led_t led, int state);
void led_toggle(pyb_led_t led);

extern const mp_obj_type_t pyb_led_type;
