typedef enum {
    // PYBv3
    PYB_LED_R1 = 1,
    PYB_LED_R2 = 2,
    PYB_LED_G1 = 3,
    PYB_LED_G2 = 4,
    // PYBv4
    PYB_LED_RED = 1,
    PYB_LED_GREEN = 2,
    PYB_LED_YELLOW = 3,
    PYB_LED_BLUE = 4,
    //STM32F4DISC
    PYB_LED_R = 1,
    PYB_LED_G = 2,
    PYB_LED_B = 3,
    PYB_LED_O = 4,
} pyb_led_t;

void led_init(void);
void led_state(pyb_led_t led, int state);
void led_toggle(pyb_led_t led);
void led_debug(int value, int delay);

extern const mp_obj_type_t pyb_led_type;
