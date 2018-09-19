#include "sigmadelta.h"
#include "esp_mphal.h"

void ICACHE_FLASH_ATTR
sigmadelta_start(void)
{
    SET_PERI_REG_MASK(GPIO_SIGMA_DELTA, (1<<SIGMA_DELTA_ENABLE));
}

void ICACHE_FLASH_ATTR
sigmadelta_stop(void)
{
    CLEAR_PERI_REG_MASK(GPIO_SIGMA_DELTA, (1<<SIGMA_DELTA_ENABLE));
}

void ICACHE_FLASH_ATTR
sigmadelta_setup(uint8_t prescaler, uint8_t target)
{
    uint32_t reg = READ_PERI_REG(GPIO_SIGMA_DELTA);

    //
    // NOTE:
    // In techref, target is noted as signed byte, though
    // no interpretation is given.
    //
    
    reg &= ~0xFFFF;
    reg |= (prescaler << SIGMA_DELTA_PRESCALAR) | (target << SIGMA_DELTA_TARGET);

    WRITE_PERI_REG(GPIO_SIGMA_DELTA, reg);
}

void ICACHE_FLASH_ATTR
sigmadelta_attach(uint8_t pin_id)
{
    // make the chosen pin an output pin
    mp_hal_pin_output(pin_id);

    if (pin_id < 16) {
        // set SIGMA_AS_PIN_SOURCE bit
        SET_PERI_REG_MASK(GPIO_PIN_ADDR(pin_id), GPIO_PIN_SOURCE_MASK);
    }
}

void ICACHE_FLASH_ATTR
sigmadelta_detach(uint8_t pin_id)
{
    if (pin_id < 16) {
        CLEAR_PERI_REG_MASK(GPIO_PIN_ADDR(pin_id), GPIO_PIN_SOURCE_MASK);
    }
}
