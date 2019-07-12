#include "py/obj.h"
#include "pin.h"

// Returns the pin mode. This value returned by this macro should be one of:
// GPIO_MODE_INPUT, GPIO_MODE_OUTPUT_PP, GPIO_MODE_OUTPUT_OD,
// GPIO_MODE_AF_PP, GPIO_MODE_AF_OD, or GPIO_MODE_ANALOG.
uint32_t pin_get_mode(const pin_obj_t *pin) {
    GPIO_TypeDef *gpio = pin->gpio;
	register uint32_t *pReg = (uint32_t *)((uint32_t)(&gpio->CRL) + (pin->pin > 7 ? 4 : 0));
	uint32_t cfg = (*pReg >> ( (pin->pin%8) * 4)) & 0x0f; // 4bit config value
	if (cfg == 0) { // 模拟输入
		return 3; // MP_HAL_PIN_MODE_ANALOG
	} else if ( (cfg & 0x03) == 0) { // 后2bit是0， 是输入(浮空/上拉/下拉)
		return 0; // MP_HAL_PIN_MODE_INPUT
	}
	cfg >>= 2; // 输出用高2bit即可判断

	// 配置值    宏值       宏名
	// 00b   => (1=0001b) MP_HAL_PIN_MODE_INPUT 
	// 10b   => (2=0010b) MP_HAL_PIN_MODE_ALT
	// 01b   => (5=0101b) MP_HAL_PIN_MODE_OPEN_DRAIN
	// 11b   => (6=0110b) MP_HAL_PIN_MODE_ALT_OPEN_DRAIN
	// 
	// 配置值 bit0 与 宏值 的 bit2 一致, 记为 (cfg & 1) << 2
	// 配置值 bit1 与 宏值 的 bit1 一致, 记为 cfg & 2
	// 配置值 bit1 与 宏值 的 bit0 取 ~ 一致, 记为 ~( (cfg & 2) >> 1 ) 或 (~(cfg & 2)) >> 1
	return (uint32_t) ( ((cfg & 1) << 2) | (cfg & 2) | (~(cfg & 2) >> 1) );
}

// Returns the pin pullup/pulldown. The value returned by this macro should
// be one of GPIO_NOPULL(except STM32F1), GPIO_PULLUP, or GPIO_PULLDOWN.
uint32_t pin_get_pull(const pin_obj_t *pin) {
	return (pin->gpio->ODR >> pin->pin) & 0x01;
}

// Returns the af (alternate function) index currently set for a pin.
uint32_t pin_get_af(const pin_obj_t *pin) {
	return 0; // 对于STM32F1 系列 af 值 没啥用(或者考虑将AFIO->MAPR, AFIO->MAPR2 的 mask 及 索引位置 返回)
}
