/** esp_iomux.h
 *
 * GPIO functions.
 *
 * Part of esp-open-rtos
 * Copyright (C) 2015 Superhouse Automation Pty Ltd
 * BSD Licensed as described in the file LICENSE
 */
#ifndef _ESP_GPIO_H
#define _ESP_GPIO_H
#include <stdbool.h>
#include "esp/gpio_regs.h"
#include "esp/iomux.h"
#include "esp/interrupts.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef enum {
    GPIO_INPUT,
    GPIO_OUTPUT,         /* "Standard" push-pull output */
    GPIO_OUT_OPEN_DRAIN, /* Open drain output */
} gpio_direction_t;

/* Enable GPIO on the specified pin, and set it to input or output mode
 */
void gpio_enable(const uint8_t gpio_num, const gpio_direction_t direction);

/* Enable/disable internal pullup resistor for a particular GPIO
 *
 * Note: According to Espressif, pullup resistor values are between 30K and
 * 100K ohms (see http://bbs.espressif.com/viewtopic.php?t=1079#p4097)
 * However, measured values suggest that the actual value is likely to be close
 * to 47K in reality.
 *
 * NOTE: The enabled_during_sleep setting is currently untested (please send
 * feedback if you give it a try)
 */
void gpio_set_pullup(uint8_t gpio_num, bool enabled, bool enabled_during_sleep);

/* Disable GPIO on the specified pin, and set it Hi-Z.
 *
 * If later muxing this pin to a different function, make sure to set
 * IOMUX_PIN_OUTPUT_ENABLE if necessary to enable the output buffer.
 */
static inline void gpio_disable(const uint8_t gpio_num)
{
    GPIO.ENABLE_OUT_CLEAR = BIT(gpio_num);
    *gpio_iomux_reg(gpio_num) &= ~IOMUX_PIN_OUTPUT_ENABLE;
}

/* Set whether the specified pin continues to drive its output when the ESP8266
 * goes into sleep mode.  Note that this setting is reset to off whenever
 * gpio_enable is called, so this must be called after calling that function.
 *
 * NOTE: This functionality is currently untested (please send feedback if you
 * give it a try)
 */
static inline void gpio_set_output_on_sleep(const uint8_t gpio_num, bool enabled)
{
    if (enabled) {
        IOMUX.PIN[gpio_to_iomux(gpio_num)] |= IOMUX_PIN_OUTPUT_ENABLE_SLEEP;
    } else {
        IOMUX.PIN[gpio_to_iomux(gpio_num)] &= ~IOMUX_PIN_OUTPUT_ENABLE_SLEEP;
    }
}

/* Set output of a pin high or low.
 *
 * Only works if pin has been set to GPIO_OUTPUT or GPIO_OUT_OPEN_DRAIN via
 * gpio_enable()
 *
 * If the mode is GPIO_OUT_OPEN_DRAIN, setting it low (false) will pull the pin
 * down to ground, but setting it high (true) will allow it to float.  Note
 * that even in GPIO_OUT_OPEN_DRAIN mode, the input gates are still physically
 * connected to the pin, and can be damaged if the voltage is not in either the
 * "low" or "high" range.  Make sure there is some sort of pull-up resistor on
 * the line to avoid floating logic lines!
 */
static inline void gpio_write(const uint8_t gpio_num, const bool set)
{
    if (set)
        GPIO.OUT_SET = BIT(gpio_num) & GPIO_OUT_PIN_MASK;
    else
        GPIO.OUT_CLEAR = BIT(gpio_num) & GPIO_OUT_PIN_MASK;
}

/* Toggle output of a pin
 *
 * Only works if pin has been set to GPIO_OUTPUT or GPIO_OUT_OPEN_DRAIN via
 * gpio_enable()
 *
 * See notes in gpio_write() about GPIO_OUT_OPEN_DRAIN mode.
 */
static inline void gpio_toggle(const uint8_t gpio_num)
{
    /* Why implement like this instead of GPIO_OUT_REG ^= xxx?
       Concurrency. If an interrupt or higher priority task writes to
       GPIO_OUT between reading and writing, only the gpio_num pin can
       get an invalid value. Prevents one task from clobbering another
       task's pins, without needing to disable/enable interrupts.
    */
    if(GPIO.OUT & BIT(gpio_num))
        GPIO.OUT_CLEAR = BIT(gpio_num) & GPIO_OUT_PIN_MASK;
    else
        GPIO.OUT_SET = BIT(gpio_num) & GPIO_OUT_PIN_MASK;
}

/* Read input value of a GPIO pin.
 *
 * If pin is set GPIO_INPUT, this reads the level on the pin.
 * If pin is set GPIO_OUTPUT, this reads the level at which the pin is
 * currently being driven (i.e. the last value written).
 * If pin is set GPIO_OUT_OPEN_DRAIN, when the pin is written low, this will
 * return low (false), when the pin is written high, this will behave like
 * GPIO_INPUT.
 */
static inline bool gpio_read(const uint8_t gpio_num)
{
    return GPIO.IN & BIT(gpio_num);
}

extern void gpio_interrupt_handler(void);

/* Set the interrupt type for a given pin
 *
 * If int_type is not GPIO_INTTYPE_NONE, the gpio_interrupt_handler will be
 * attached and unmasked.
 */
static inline void gpio_set_interrupt(const uint8_t gpio_num, const gpio_inttype_t int_type)
{
    GPIO.CONF[gpio_num] = SET_FIELD(GPIO.CONF[gpio_num], GPIO_CONF_INTTYPE, int_type);
    if(int_type != GPIO_INTTYPE_NONE) {
        _xt_isr_attach(INUM_GPIO, gpio_interrupt_handler);
        _xt_isr_unmask(1<<INUM_GPIO);
    }
}

/* Return the interrupt type set for a pin */
static inline gpio_inttype_t gpio_get_interrupt(const uint8_t gpio_num)
{
    return (gpio_inttype_t)FIELD2VAL(GPIO_CONF_INTTYPE, GPIO.CONF[gpio_num]);
}

#ifdef	__cplusplus
}
#endif

#endif
