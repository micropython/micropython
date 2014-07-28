#include "gpio_clk.h"
#include "stm32f4xx_hal.h"

void GPIO_CLK_ENABLE(void)
{
	__GPIOA_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();
	__GPIOD_CLK_ENABLE();
}
