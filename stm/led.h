typedef enum {
    PYB_LED_R1 = 1,
    PYB_LED_R2 = 2,
    PYB_LED_G1 = 3,
    PYB_LED_G2 = 4,
    //STM32F4DISC
    PYB_LED_R = 1,
    PYB_LED_G = 2,
    PYB_LED_B = 3,
    PYB_LED_O = 4,
} pyb_led_t;

void led_init(void);
void led_state(pyb_led_t led, int state);
void led_toggle(pyb_led_t led);

mp_obj_t pyb_Led(mp_obj_t led_id);
