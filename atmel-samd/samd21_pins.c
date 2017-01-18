#include "shared-bindings/microcontroller/Pin.h"

#include "samd21_pins.h"

#define SERCOM(p_sercom, p_pad) \
{ \
  .sercom = p_sercom, \
  .pad = p_pad \
}

#define NO_SERCOM SERCOM(0, 0)

#define TCC(p_tcc, p_channel, p_wave_output) \
{ \
  .tcc = p_tcc, \
  .is_tc = false, \
  .channel = p_channel, \
  .wave_output = p_wave_output \
}

#define TC(p_tc, p_channel, p_wave_output) \
{ \
  .tc = p_tc, \
  .is_tc = true, \
  .channel = p_channel, \
  .wave_output = p_wave_output \
}

#define NO_TIMER TCC(0, 0, 0)

#define TOUCH(y_line) \
    .has_touch = true, \
    .touch_y_line = y_line,

#define NO_TOUCH \
    .has_touch = false,

#define ADC_INPUT(input) \
    .has_adc = true, \
    .adc_input = input,

#define NO_ADC \
    .has_adc = false,

// This macro is used to simplify pin definition in boards/<board>/pins.c
#define PIN(p_name, p_adc, p_touch, p_primary_timer,  p_secondary_timer, \
            p_primary_sercom, p_secondary_sercom) \
const mcu_pin_obj_t pin_## p_name = { \
    { &mcu_pin_type }, \
    .name = MP_QSTR_ ## p_name, \
    .pin = (PIN_## p_name), \
    p_adc \
    p_touch \
    .primary_timer = p_primary_timer, \
    .secondary_timer =  p_secondary_timer, \
    .sercom = {p_primary_sercom, p_secondary_sercom}, \
}

#define NO_ADC_INPUT (0)

void reset_pin(uint8_t pin) {
    if (pin >= PORT_BITS) {
        return;
    }
    struct system_pinmux_config config;
    system_pinmux_get_config_defaults(&config);
    config.powersave = true;
    system_pinmux_pin_set_config(pin, &config);
}

// Pins in datasheet order.
#ifdef PIN_PA00
PIN(PA00, NO_ADC, NO_TOUCH,
    TCC(TCC2, 0, 0),
    NO_TIMER,
    NO_SERCOM,
    SERCOM(SERCOM1, 0));
#endif
#ifdef PIN_PA01
PIN(PA01, NO_ADC, NO_TOUCH,
    TCC(TCC2, 1, 1),
    NO_TIMER,
    NO_SERCOM,
    SERCOM(SERCOM1, 1));
#endif
#ifdef PIN_PA02
PIN(PA02, ADC_INPUT(ADC_POSITIVE_INPUT_PIN0), TOUCH(0),
    NO_TIMER,
    NO_TIMER,
    NO_SERCOM,
    NO_SERCOM);
#endif
#ifdef PIN_PA03
PIN(PA03, ADC_INPUT(ADC_POSITIVE_INPUT_PIN1), TOUCH(1),
    NO_TIMER,
    NO_TIMER,
    NO_SERCOM,
    NO_SERCOM);
#endif
#ifdef PIN_PB04
PIN(PB04, ADC_INPUT(ADC_POSITIVE_INPUT_PIN12), TOUCH(10),
    NO_TIMER,
    NO_TIMER,
    NO_SERCOM,
    NO_SERCOM);
#endif
#ifdef PIN_PB05
PIN(PB05, ADC_INPUT(ADC_POSITIVE_INPUT_PIN13), TOUCH(11),
    NO_TIMER,
    NO_TIMER,
    NO_SERCOM,
    NO_SERCOM);
#endif
#ifdef PIN_PB06
PIN(PB06, ADC_INPUT(ADC_POSITIVE_INPUT_PIN14), TOUCH(12),
    NO_TIMER,
    NO_TIMER,
    NO_SERCOM,
    NO_SERCOM);
#endif
#ifdef PIN_PB07
PIN(PB07, TOUCH(ADC_POSITIVE_INPUT_PIN15), TOUCH(13),
    NO_TIMER,
    NO_TIMER,
    NO_SERCOM,
    NO_SERCOM);
#endif
#ifdef PIN_PB08
PIN(PB08, ADC_INPUT(ADC_POSITIVE_INPUT_PIN2), TOUCH(14),
    TC(TC4, 0, 0),
    NO_TIMER,
    NO_SERCOM,
    SERCOM(SERCOM4, 0));
#endif
#ifdef PIN_PB09
PIN(PB09, ADC_INPUT(ADC_POSITIVE_INPUT_PIN3), TOUCH(15),
    TC(TC4, 1, 1),
    NO_TIMER,
    NO_SERCOM,
    SERCOM(SERCOM4, 1));
#endif
#ifdef PIN_PA04
PIN(PA04, ADC_INPUT(ADC_POSITIVE_INPUT_PIN4), TOUCH(2),
    TCC(TCC0, 0, 0),
    NO_TIMER,
    NO_SERCOM,
    SERCOM(SERCOM0, 0));
#endif
#ifdef PIN_PA05
PIN(PA05, ADC_INPUT(ADC_POSITIVE_INPUT_PIN5), TOUCH(3),
    TCC(TCC0, 1, 1),
    NO_TIMER,
    NO_SERCOM,
    SERCOM(SERCOM0, 1));
#endif
#ifdef PIN_PA06
PIN(PA06, ADC_INPUT(ADC_POSITIVE_INPUT_PIN6), TOUCH(4),
    TCC(TCC1, 0, 0),
    NO_TIMER,
    SERCOM(SERCOM0, 2),
    NO_SERCOM);
#endif
#ifdef PIN_PA07
PIN(PA07, ADC_INPUT(ADC_POSITIVE_INPUT_PIN7), TOUCH(5),
    TCC(TCC1, 1, 1),
    NO_TIMER,
    SERCOM(SERCOM0, 3),
    NO_SERCOM);
#endif
#ifdef PIN_PA08
PIN(PA08, ADC_INPUT(ADC_POSITIVE_INPUT_PIN16), NO_TOUCH,
    TCC(TCC0, 0, 0),
    TCC(TCC1, 2, 2),
    SERCOM(SERCOM0, 0),
    SERCOM(SERCOM2, 0));
#endif
#ifdef PIN_PA09
PIN(PA09, ADC_INPUT(ADC_POSITIVE_INPUT_PIN17), NO_TOUCH,
    TCC(TCC0, 1, 1),
    TCC(TCC1, 3, 3),
    SERCOM(SERCOM0, 1),
    SERCOM(SERCOM2, 1));
#endif
#ifdef PIN_PA10
PIN(PA10, ADC_INPUT(ADC_POSITIVE_INPUT_PIN18), NO_TOUCH,
    TCC(TCC1, 0, 0),
    TCC(TCC0, 2, 2),
    SERCOM(SERCOM0, 2),
    SERCOM(SERCOM2, 2));
#endif
#ifdef PIN_PA11
PIN(PA11, ADC_INPUT(ADC_POSITIVE_INPUT_PIN19), NO_TOUCH,
    TCC(TCC1, 1, 1),
    TCC(TCC0, 3, 3),
    SERCOM(SERCOM0, 3),
    SERCOM(SERCOM2, 3));
#endif
#ifdef PIN_PB10
PIN(PB10, NO_ADC, NO_TOUCH,
    TC(TC5, 0, 0),
    TCC(TCC0, 0, 4),
    NO_SERCOM,
    SERCOM(SERCOM4, 2));
#endif
#ifdef PIN_PB11
PIN(PB11, NO_ADC, NO_TOUCH,
    TC(TC5, 1, 1),
    TCC(TCC0, 1, 5),
    NO_SERCOM,
    SERCOM(SERCOM4, 3));
#endif
#ifdef PIN_PB12
PIN(PB12, NO_ADC, NO_TOUCH,
    TC(TC4, 0, 0),
    TCC(TCC0, 2, 6),
    SERCOM(SERCOM4, 0),
    NO_SERCOM);
#endif
#ifdef PIN_PB13
PIN(PB13, NO_ADC, NO_TOUCH,
    TC(TC4, 1, 1),
    TCC(TCC0, 3, 7),
    SERCOM(SERCOM4, 1),
    NO_SERCOM);
#endif
#ifdef PIN_PB14
PIN(PB14, NO_ADC, NO_TOUCH,
    TC(TC5, 0, 0),
    NO_TIMER,
    SERCOM(SERCOM4, 2),
    NO_SERCOM);
#endif

// Second page.
#ifdef PIN_PB15
PIN(PB15, NO_ADC, NO_TOUCH,
    TC(TC5, 1, 1),
    NO_TIMER,
    SERCOM(SERCOM4, 3),
    NO_SERCOM);
#endif
#ifdef PIN_PA12
PIN(PA12, NO_ADC, NO_TOUCH,
    TCC(TCC2, 0, 0),
    TCC(TCC0, 2, 6),
    SERCOM(SERCOM2, 0),
    SERCOM(SERCOM4, 0));
#endif
#ifdef PIN_PA13
PIN(PA13, NO_ADC, NO_TOUCH,
    TCC(TCC2, 1, 1),
    TCC(TCC0, 3, 7),
    SERCOM(SERCOM2, 1),
    SERCOM(SERCOM4, 1));
#endif
#ifdef PIN_PA14
PIN(PA14, NO_ADC, NO_TOUCH,
    TC(TC3, 0, 0),
    TCC(TCC0, 0, 4),
    SERCOM(SERCOM2, 2),
    #ifdef SERCOM4
    SERCOM(SERCOM4, 2)
    #else
    NO_SERCOM
    #endif
    );
#endif
#ifdef PIN_PA15
PIN(PA15, NO_ADC, NO_TOUCH,
    TC(TC3, 1, 1),
    TCC(TCC0, 1, 5),
    SERCOM(SERCOM2, 3),
    #ifdef SERCOM4
    SERCOM(SERCOM4, 3)
    #else
    NO_SERCOM
    #endif
    );
#endif
#ifdef PIN_PA16
PIN(PA16, NO_ADC, NO_TOUCH,
    TCC(TCC2, 0, 0),
    TCC(TCC0, 2, 6),
    SERCOM(SERCOM1, 0),
    SERCOM(SERCOM3, 0));
#endif
#ifdef PIN_PA17
PIN(PA17, NO_ADC, NO_TOUCH,
  TCC(TCC2, 1, 1),
  TCC(TCC0, 3, 7),
  SERCOM(SERCOM1, 1),
  SERCOM(SERCOM3, 1));
#endif
#ifdef PIN_PA18
PIN(PA18, NO_ADC, NO_TOUCH,
  TC(TC3, 0, 0),
  TCC(TCC0, 2, 2),
  SERCOM(SERCOM1, 2),
  SERCOM(SERCOM3, 2));
#endif
#ifdef PIN_PA19
PIN(PA19, NO_ADC, NO_TOUCH,
  TC(TC3, 1, 1),
  TCC(TCC0, 3, 3),
  SERCOM(SERCOM1, 3),
  SERCOM(SERCOM3, 3));
#endif
#ifdef PIN_PB16
PIN(PB16, NO_ADC, NO_TOUCH,
    #ifdef _SAMD21_TC6_INSTANCE_
    TC(TC6, 0, 0),
    #else
    NO_TIMER,
    #endif
    TCC(TCC0, 0, 4),
    SERCOM(SERCOM5, 0),
    NO_SERCOM);
#endif
#ifdef PIN_PB17
PIN(PB17, NO_ADC, NO_TOUCH,
    #ifdef _SAMD21_TC6_INSTANCE_
    TC(TC6, 0, 0),
    #else
    NO_TIMER,
    #endif
    TCC(TCC0, 1, 5),
    SERCOM(SERCOM5, 1),
    NO_SERCOM);
#endif
#ifdef PIN_PA20
PIN(PA20, NO_ADC, NO_TOUCH,
    #ifdef _SAMD21_TC7_INSTANCE_
    TC(TC7, 0, 0),
    #else
    NO_TIMER,
    #endif
    TCC(TCC0, 2, 6),
    SERCOM(SERCOM5, 2),
    SERCOM(SERCOM3, 2));
#endif
#ifdef PIN_PA21
PIN(PA21, NO_ADC, NO_TOUCH,
    #ifdef _SAMD21_TC7_INSTANCE_
    TC(TC7, 1, 1),
    #else
    NO_TIMER,
    #endif
    TCC(TCC0, 3, 7),
    SERCOM(SERCOM5, 3),
    SERCOM(SERCOM3, 3));
#endif
#ifdef PIN_PA22
PIN(PA22, NO_ADC, NO_TOUCH,
    TC(TC4, 0, 0),
    TCC(TCC0, 0, 4),
    SERCOM(SERCOM3, 0),
    #ifdef SERCOM5
    SERCOM(SERCOM5, 0)
    #else
    NO_SERCOM
    #endif
    );
#endif
#ifdef PIN_PA23
PIN(PA23, NO_ADC, NO_TOUCH,
    TC(TC4, 1, 1),
    TCC(TCC0, 1, 5),
    SERCOM(SERCOM3, 1),
    #ifdef SERCOM5
    SERCOM(SERCOM5, 1)
    #else
    NO_SERCOM
    #endif
    );
#endif
#ifdef PIN_PA24
PIN(PA24, NO_ADC, NO_TOUCH,
    TC(TC5, 0, 0),
    TCC(TCC0, 2, 2),
    SERCOM(SERCOM3, 2),
    #ifdef SERCOM5
    SERCOM(SERCOM5, 2)
    #else
    NO_SERCOM
    #endif
    );
#endif
#ifdef PIN_PA25
PIN(PA25, NO_ADC, NO_TOUCH,
    TC(TC5, 1, 1),
    TCC(TCC1, 3, 3),
    SERCOM(SERCOM3, 3),
    #ifdef SERCOM5
    SERCOM(SERCOM5, 3)
    #else
    NO_SERCOM
    #endif
    );
#endif
#ifdef PIN_PB22
PIN(PB22, NO_ADC, NO_TOUCH,
    #ifdef _SAMD21_TC7_INSTANCE_
    TC(TC7, 1, 1),
    #else
    NO_TIMER,
    #endif
    NO_TIMER,
    NO_SERCOM,
    SERCOM(SERCOM5, 2));
#endif
#ifdef PIN_PB23
PIN(PB23, NO_ADC, NO_TOUCH,
    #ifdef _SAMD21_TC7_INSTANCE_
    TC(TC7, 1, 1),
    #else
    NO_TIMER,
    #endif
    NO_TIMER,
    NO_SERCOM,
    SERCOM(SERCOM5, 3));
#endif
#ifdef PIN_PA27
PIN(PA27, NO_ADC, NO_TOUCH,
    NO_TIMER,
    NO_TIMER,
    NO_SERCOM,
    NO_SERCOM);
#endif
#ifdef PIN_PA28
PIN(PA28, NO_ADC, NO_TOUCH,
    NO_TIMER,
    NO_TIMER,
    NO_SERCOM,
    NO_SERCOM);
#endif
#ifdef PIN_PA30
PIN(PA30, NO_ADC, NO_TOUCH,
    TCC(TCC1, 0, 0),
    NO_TIMER,
    NO_SERCOM,
    SERCOM(SERCOM1, 2));
#endif
#ifdef PIN_PA31
PIN(PA31, NO_ADC, NO_TOUCH,
    TCC(TCC1, 1, 1),
    NO_TIMER,
    NO_SERCOM,
    SERCOM(SERCOM1, 3));
#endif
#ifdef PIN_PB30
PIN(PB30, NO_ADC, NO_TOUCH,
    TCC(TCC0, 0, 0),
    TCC(TCC1, 2, 2),
    NO_SERCOM,
    SERCOM(SERCOM5, 0));
#endif
#ifdef PIN_PB31
PIN(PB31, NO_ADC, NO_TOUCH,
    TCC(TCC0, 1, 1),
    TCC(TCC1, 3, 3),
    NO_SERCOM,
    SERCOM(SERCOM5, 1));
#endif
#ifdef PIN_PB00
PIN(PB00, ADC_INPUT(ADC_POSITIVE_INPUT_PIN8), TOUCH(6),
    #ifdef _SAMD21_TC7_INSTANCE_
    TC(TC7, 0, 0),
    #else
    NO_TIMER,
    #endif
    NO_TIMER,
    NO_SERCOM,
    SERCOM(SERCOM5, 2));
#endif
#ifdef PIN_PB01
PIN(PB01, ADC_INPUT(ADC_POSITIVE_INPUT_PIN9), TOUCH(7),
    #ifdef _SAMD21_TC7_INSTANCE_
    TC(TC7, 1, 1),
    #else
    NO_TIMER,
    #endif
    NO_TIMER,
    NO_SERCOM,
    SERCOM(SERCOM5, 3));
#endif
#ifdef PIN_PB02
PIN(PB02, ADC_INPUT(ADC_POSITIVE_INPUT_PIN10), TOUCH(8),
    #ifdef _SAMD21_TC6_INSTANCE_
    TC(TC6, 0, 0),
    #else
    NO_TIMER,
    #endif
    NO_TIMER,
    NO_SERCOM,
    SERCOM(SERCOM5, 0));
#endif
#ifdef PIN_PB03
PIN(PB03, ADC_INPUT(ADC_POSITIVE_INPUT_PIN11), TOUCH(9),
    #ifdef _SAMD21_TC6_INSTANCE_
    TC(TC6, 1, 1),
    #else
    NO_TIMER,
    #endif
    NO_TIMER,
    NO_SERCOM,
    SERCOM(SERCOM5, 1));
#endif
