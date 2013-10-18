typedef enum {
    PYB_LED_R1 = 0,
    PYB_LED_R2 = 1,
    PYB_LED_G1 = 2,
    PYB_LED_G2 = 3,
} pyb_led_t;

void led_init();
void led_state(pyb_led_t led, int state);
