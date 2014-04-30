#include <stdint.h>

#include "stm32f4xx_hal.h"

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "pin.h"
#include "genhdr/pins.h"
#include "led.h"
#include "extint.h"
#include "spi.h"
#include "ccspi.h"
#include "ccdebug.h"
#include "pybcc3k.h"

// IRQ on PA14, input, pulled up, active low
// EN on PC7, output, active high
// CS on PC6, output, active low
// SPI2 on PB15=MOSI, PB14=MISO, PB13=SCK
// SCK for CC3000: max 16MHz, low when idle, data sampled on falling edge

#define PIN_CS pin_B12 // Y5
#define PIN_EN pin_B9 // Y4
#define PIN_IRQ pin_B8 // Y3
#define SPI_HANDLE SPIHandle2

// TODO this could be really wrong wrt calibration
void pyb_delay_us(uint32_t usec) {
    volatile uint32_t count = 0;
    const uint32_t utime = (160 * usec / 5);
    do {
        if (++count > utime) {
            return;
        }
    } while (1);
}

void pyb_cc3000_set_en(int val) {
    DEBUG_printf("pyb_cc3000_set_en val=%d\n", val);
    if (val) {
        PIN_EN.gpio->BSRRL = PIN_EN.pin_mask; // set pin high
    } else {
        PIN_EN.gpio->BSRRH = PIN_EN.pin_mask; // set pin low
    }
}

void pyb_cc3000_set_cs(int val) {
    DEBUG_printf("pyb_cc3000_set_cs val=%d\n", val);
    if (val) {
        PIN_CS.gpio->BSRRL = PIN_CS.pin_mask; // set pin high
    } else {
        PIN_CS.gpio->BSRRH = PIN_CS.pin_mask; // set pin low
    }
}

int pyb_cc3000_get_irq(void) {
    if ((PIN_IRQ.gpio->IDR & PIN_IRQ.pin_mask) == 0) {
        return 0;
    } else {
        return 1;
    }
}

uint32_t exti14_enabled = 0; // TODO hack; do it properly!
uint32_t exti14_missed = 0; // TODO hack; do it properly!

void pyb_cc3000_enable_irq(void) {
    DEBUG_printf("pyb_cc3000_enable_irq: en=%lu miss=%lu\n", exti14_enabled, exti14_missed);
    if (exti14_missed) {
        /* doesn't look like this is needed
        DEBUG_printf("pyb_cc3000_enable_irq: handling missed IRQ\n");
        // TODO hack if we have a pending IRQ
        extern void SpiIntGPIOHandler(void);
        SpiIntGPIOHandler();
        */
        exti14_missed = 0;
    }
    exti14_enabled = 1;
}

void pyb_cc3000_disable_irq(void) {
    DEBUG_printf("pyb_cc3000_disable_irq: en=%lu miss=%lu\n", exti14_enabled, exti14_missed);
    exti14_enabled = 0;
}

void pyb_cc3000_pause_spi(void) {
    DEBUG_printf("pyb_cc3000_pause_spi\n");
    exti14_enabled = 0;
}

void pyb_cc3000_resume_spi(void) {
    DEBUG_printf("pyb_cc3000_resume_spi\n");
    exti14_enabled = 1;
}

STATIC mp_obj_t irq_callback(mp_obj_t line) {
    led_toggle(2);
    //extern void SpiIntGPIOHandler(void);
    //extern uint32_t exti14_enabled;
    //extern uint32_t exti14_missed;
    //printf("-> EXTI14 en=%lu miss=%lu\n", exti14_enabled, exti14_missed);
    if (exti14_enabled) {
        exti14_missed = 0;
        SpiIntGPIOHandler(); // CC3000 interrupt
    } else {
        exti14_missed = 1;
    }
    //printf("<- EXTI14 done\n");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(irq_callback_obj, irq_callback);

void pyb_cc3000_spi_init(void) {
    DEBUG_printf("pyb_cc3000_spi_init\n");

    /*
  inf.baudRate = 100000; // FIXME - just slow for debug
  inf.spiMode = SPIF_SPI_MODE_1;  // Mode 1   CPOL= 0  CPHA= 1
  */

    /*!< SPI configuration */
    SPI_HANDLE.Init.Mode = SPI_MODE_MASTER;
    SPI_HANDLE.Init.Direction = SPI_DIRECTION_2LINES;
    SPI_HANDLE.Init.DataSize = SPI_DATASIZE_8BIT; // should be correct
    SPI_HANDLE.Init.CLKPolarity = SPI_POLARITY_LOW; // clock is low when idle
    SPI_HANDLE.Init.CLKPhase = SPI_PHASE_2EDGE; // data latched on second edge, which is falling edge for low-idle
    SPI_HANDLE.Init.NSS = SPI_NSS_SOFT; // software control
    SPI_HANDLE.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; // clock freq = f_PCLK / this_prescale_value
    SPI_HANDLE.Init.FirstBit = SPI_FIRSTBIT_MSB; // should be correct
    SPI_HANDLE.Init.TIMode = SPI_TIMODE_DISABLED;
    SPI_HANDLE.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
    SPI_HANDLE.Init.CRCPolynomial = 7; // unused

    spi_init(&SPI_HANDLE);

    // configure wlan CS and EN pins
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Alternate = 0;

    GPIO_InitStructure.Pin = PIN_CS.pin_mask;
    HAL_GPIO_Init(PIN_CS.gpio, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = PIN_EN.pin_mask;
    HAL_GPIO_Init(PIN_EN.gpio, &GPIO_InitStructure);

    pyb_cc3000_set_cs(1); // de-assert CS
    pyb_cc3000_set_en(0); // disable wlan

    // configure EXTI for PIN_IRQ
    extint_register((mp_obj_t)&PIN_IRQ, GPIO_MODE_IT_FALLING, GPIO_PULLUP, (mp_obj_t)&irq_callback_obj, true, NULL);

    // wait a little (ensure that WLAN takes effect)
    HAL_Delay(500); // force a 500ms delay! FIXME
}

uint8_t pyb_cc3000_spi_send(uint8_t val) {
    uint8_t data[1] = {val};
    HAL_SPI_TransmitReceive(&SPI_HANDLE, data, data, 1, 1000);
    return data[0];
}
