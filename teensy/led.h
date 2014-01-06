typedef enum {
    PYB_LED_BUILTIN = 0,
} pyb_led_t;

void led_init(void);
void led_state(pyb_led_t led, int state);
void led_toggle(pyb_led_t led);

mp_obj_t pyb_Led(mp_obj_t led_id);
