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

// This macro is used to simplify pin definition in boards/<board>/pins.c
#define PIN(p_name, p_has_adc, p_adc_input, p_primary_timer, \
            p_secondary_timer, p_primary_sercom, p_secondary_sercom) \
const mcu_pin_obj_t pin_## p_name = { \
    { &mcu_pin_type }, \
    .name = MP_QSTR_ ## p_name, \
    .pin = (PIN_## p_name), \
    .has_adc = p_has_adc, \
    .adc_input = p_adc_input, \
    .primary_timer = p_primary_timer, \
    .secondary_timer =  p_secondary_timer, \
    .sercom = {p_primary_sercom, p_secondary_sercom}, \
}

#define NO_ADC_INPUT (0)

// Pins in datasheet order.
#ifdef PIN_PA00
PIN(PA00, false, NO_ADC_INPUT, \
    TCC(TCC2, 0, 0),
    NO_TIMER,
    SERCOM(SERCOM1, 0),
    NO_SERCOM);
#endif
#ifdef PIN_PA01
PIN(PA01, false, NO_ADC_INPUT,
    TCC(TCC2, 1, 1),
    NO_TIMER,
    SERCOM(SERCOM1, 1),
    NO_SERCOM);
#endif
#ifdef PIN_PA02
PIN(PA02, true, ADC_POSITIVE_INPUT_PIN0,
    NO_TIMER,
    NO_TIMER,
    NO_SERCOM,
    NO_SERCOM);
#endif
#ifdef PIN_PA03
PIN(PA03, true, ADC_POSITIVE_INPUT_PIN1,
    NO_TIMER,
    NO_TIMER,
    NO_SERCOM,
    NO_SERCOM);
#endif
#ifdef PIN_PB04
PIN(PB04, true, ADC_POSITIVE_INPUT_PIN12,
    NO_TIMER,
    NO_TIMER,
    NO_SERCOM,
    NO_SERCOM);
#endif
#ifdef PIN_PB05
PIN(PB05, true, ADC_POSITIVE_INPUT_PIN13,
    NO_TIMER,
    NO_TIMER,
    NO_SERCOM,
    NO_SERCOM);
#endif
#ifdef PIN_PB06
PIN(PB06, true, ADC_POSITIVE_INPUT_PIN14,
    NO_TIMER,
    NO_TIMER,
    NO_SERCOM,
    NO_SERCOM);
#endif
#ifdef PIN_PB07
PIN(PB07, true, ADC_POSITIVE_INPUT_PIN15,
    NO_TIMER,
    NO_TIMER,
    NO_SERCOM,
    NO_SERCOM);
#endif
#ifdef PIN_PB08
PIN(PB08, true, ADC_POSITIVE_INPUT_PIN2,
    TC(TC4, 0, 0),
    NO_TIMER,
    NO_SERCOM,
    SERCOM(SERCOM4, 0));
#endif
#ifdef PIN_PB09
PIN(PB09, true, ADC_POSITIVE_INPUT_PIN3,
    TC(TC4, 1, 1),
    NO_TIMER,
    NO_SERCOM,
    SERCOM(SERCOM4, 1));
#endif
#ifdef PIN_PA04
PIN(PA04, true, ADC_POSITIVE_INPUT_PIN4,
    TCC(TCC0, 0, 0),
    NO_TIMER,
    NO_SERCOM,
    SERCOM(SERCOM0, 0));
#endif
#ifdef PIN_PA05
PIN(PA05, true, ADC_POSITIVE_INPUT_PIN5,
    TCC(TCC0, 1, 1),
    NO_TIMER,
    NO_SERCOM,
    SERCOM(SERCOM0, 1));
#endif
#ifdef PIN_PA06
PIN(PA06, true, ADC_POSITIVE_INPUT_PIN6,
    TCC(TCC1, 0, 0),
    NO_TIMER,
    SERCOM(SERCOM0, 2),
    NO_SERCOM);
#endif
#ifdef PIN_PA07
PIN(PA07, true, ADC_POSITIVE_INPUT_PIN7,
    TCC(TCC1, 1, 1),
    NO_TIMER,
    SERCOM(SERCOM0, 3),
    NO_SERCOM);
#endif
#ifdef PIN_PA08
PIN(PA08, true, ADC_POSITIVE_INPUT_PIN17,
    TCC(TCC0, 0, 0),
    TCC(TCC1, 2, 2),
    SERCOM(SERCOM0, 0),
    SERCOM(SERCOM2, 0));
#endif
#ifdef PIN_PA09
PIN(PA09, true, ADC_POSITIVE_INPUT_PIN17,
    TCC(TCC0, 1, 1),
    TCC(TCC1, 3, 3),
    SERCOM(SERCOM0, 1),
    SERCOM(SERCOM2, 1));
#endif
#ifdef PIN_PA10
PIN(PA10, true, ADC_POSITIVE_INPUT_PIN18,
    TCC(TCC1, 0, 0),
    TCC(TCC0, 2, 2),
    SERCOM(SERCOM0, 2),
    SERCOM(SERCOM2, 2));
#endif
#ifdef PIN_PA11
PIN(PA11, true, ADC_POSITIVE_INPUT_PIN19,
    TCC(TCC1, 1, 1),
    TCC(TCC0, 3, 3),
    SERCOM(SERCOM0, 3),
    SERCOM(SERCOM2, 3));
#endif
#ifdef PIN_PB10
PIN(PB10, false, NO_ADC_INPUT,
    TC(TC5, 0, 0),
    TCC(TCC0, 0, 4),
    NO_SERCOM,
    SERCOM(SERCOM4, 2));
#endif
#ifdef PIN_PB11
PIN(PB11, false, NO_ADC_INPUT,
    TC(TC5, 1, 1),
    TCC(TCC0, 1, 5),
    NO_SERCOM,
    SERCOM(SERCOM4, 3));
#endif
#ifdef PIN_PB12
PIN(PB12, false, NO_ADC_INPUT,
    TC(TC4, 0, 0),
    TCC(TCC0, 2, 6),
    SERCOM(SERCOM4, 0),
    NO_SERCOM);
#endif
#ifdef PIN_PB13
PIN(PB13, false, NO_ADC_INPUT,
    TC(TC4, 1, 1),
    TCC(TCC0, 3, 7),
    SERCOM(SERCOM4, 1),
    NO_SERCOM);
#endif
#ifdef PIN_PB14
PIN(PB14, false, NO_ADC_INPUT,
    TC(TC5, 0, 0),
    NO_TIMER,
    SERCOM(SERCOM4, 2),
    NO_SERCOM);
#endif

// Second page.
#ifdef PIN_PB15
PIN(PB15, false, NO_ADC_INPUT,
    TC(TC5, 1, 1),
    NO_TIMER,
    SERCOM(SERCOM4, 3),
    NO_SERCOM);
#endif
#ifdef PIN_PA12
PIN(PA12, false, NO_ADC_INPUT,
    TCC(TCC2, 0, 0),
    TCC(TCC0, 2, 6),
    SERCOM(SERCOM2, 0),
    SERCOM(SERCOM4, 0));
#endif
#ifdef PIN_PA13
PIN(PA13, false, NO_ADC_INPUT,
    TCC(TCC2, 1, 1),
    TCC(TCC0, 3, 7),
    SERCOM(SERCOM2, 1),
    SERCOM(SERCOM4, 1));
#endif
#ifdef PIN_PA14
PIN(PA14, false, NO_ADC_INPUT,
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
PIN(PA15, false, NO_ADC_INPUT,
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
PIN(PA16, false, NO_ADC_INPUT,
    TCC(TCC2, 0, 0),
    TCC(TCC0, 2, 6),
    SERCOM(SERCOM1, 0),
    SERCOM(SERCOM3, 0));
#endif
#ifdef PIN_PA17
PIN(PA17, false, NO_ADC_INPUT,
  TCC(TCC2, 1, 1),
  TCC(TCC0, 3, 7),
  SERCOM(SERCOM1, 1),
  SERCOM(SERCOM3, 1));
#endif
#ifdef PIN_PA18
PIN(PA18, false, NO_ADC_INPUT,
  TC(TC3, 0, 0),
  TCC(TCC0, 2, 2),
  SERCOM(SERCOM1, 2),
  SERCOM(SERCOM3, 2));
#endif
#ifdef PIN_PA19
PIN(PA19, false, NO_ADC_INPUT,
  TC(TC3, 1, 1),
  TCC(TCC0, 3, 3),
  SERCOM(SERCOM1, 3),
  SERCOM(SERCOM3, 3));
#endif
#ifdef PIN_PB16
PIN(PB16, false, NO_ADC_INPUT,
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
PIN(PB17, false, NO_ADC_INPUT,
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
PIN(PA20, false, NO_ADC_INPUT,
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
PIN(PA21, false, NO_ADC_INPUT,
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
PIN(PA22, false, NO_ADC_INPUT,
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
PIN(PA23, false, NO_ADC_INPUT,
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
PIN(PA24, false, NO_ADC_INPUT,
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
PIN(PA25, false, NO_ADC_INPUT,
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
PIN(PB22, false, NO_ADC_INPUT,
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
PIN(PB23, false, NO_ADC_INPUT,
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
PIN(PA27, false, NO_ADC_INPUT,
    NO_TIMER,
    NO_TIMER,
    NO_SERCOM,
    NO_SERCOM);
#endif
#ifdef PIN_PA28
PIN(PA28, false, NO_ADC_INPUT,
    NO_TIMER,
    NO_TIMER,
    NO_SERCOM,
    NO_SERCOM);
#endif
#ifdef PIN_PA30
PIN(PA30, false, NO_ADC_INPUT,
    TCC(TCC1, 0, 0),
    NO_TIMER,
    NO_SERCOM,
    SERCOM(SERCOM1, 2));
#endif
#ifdef PIN_PA31
PIN(PA31, false, NO_ADC_INPUT,
    TCC(TCC1, 1, 1),
    NO_TIMER,
    NO_SERCOM,
    SERCOM(SERCOM1, 3));
#endif
#ifdef PIN_PB30
PIN(PB30, false, NO_ADC_INPUT,
    TCC(TCC0, 0, 0),
    TCC(TCC1, 2, 2),
    NO_SERCOM,
    SERCOM(SERCOM5, 0));
#endif
#ifdef PIN_PB31
PIN(PB31, false, NO_ADC_INPUT,
    TCC(TCC0, 1, 1),
    TCC(TCC1, 3, 3),
    NO_SERCOM,
    SERCOM(SERCOM5, 1));
#endif
#ifdef PIN_PB00
PIN(PB00, true, ADC_POSITIVE_INPUT_PIN8,
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
PIN(PB01, true, ADC_POSITIVE_INPUT_PIN9,
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
PIN(PB02, true, ADC_POSITIVE_INPUT_PIN10,
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
PIN(PB03, true, ADC_POSITIVE_INPUT_PIN11,
    #ifdef _SAMD21_TC6_INSTANCE_
    TC(TC6, 1, 1),
    #else
    NO_TIMER,
    #endif
    NO_TIMER,
    NO_SERCOM,
    SERCOM(SERCOM5, 1));
#endif
