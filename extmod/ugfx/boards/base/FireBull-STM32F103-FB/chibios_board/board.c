#include "ch.h"
#include "hal.h"

/**
* @brief   PAL setup.
* @details Digital I/O ports static configuration as defined in @p board.h.
*          This variable is used by the HAL when initializing the PAL driver.
*/
#if HAL_USE_PAL || defined(__DOXYGEN__)
const PALConfig pal_default_config = {
	{VAL_GPIOAODR, VAL_GPIOACRL, VAL_GPIOACRH},
	{VAL_GPIOBODR, VAL_GPIOBCRL, VAL_GPIOBCRH},
	{VAL_GPIOCODR, VAL_GPIOCCRL, VAL_GPIOCCRH},
	{VAL_GPIODODR, VAL_GPIODCRL, VAL_GPIODCRH},
	{VAL_GPIOEODR, VAL_GPIOECRL, VAL_GPIOECRH},
};
#endif

#if HAL_USE_MMC_SPI
/* Board-related functions related to the MMC_SPI driver.*/
bool_t mmc_lld_is_card_inserted(MMCDriver *mmcp) {
	(void)mmcp;

	return !palReadPad(GPIOA, GPIOA_MMCCP);
}

bool_t mmc_lld_is_write_protected(MMCDriver *mmcp) {
	(void)mmcp;

	return 0;
}
#endif

/*
 * Early initialization code.
 * This initialization must be performed just after stack setup and before
 * any other initialization.
 */
void __early_init(void) {
	stm32_clock_init();
}

void boardInit(void) {
	palClearPad(GPIOD, GPIOD_LED1);
	palClearPad(GPIOD, GPIOD_LED2);
	palClearPad(GPIOD, GPIOD_LED3);
	palClearPad(GPIOD, GPIOD_LED4);
}

