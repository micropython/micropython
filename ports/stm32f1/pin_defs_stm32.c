#include "py/obj.h"
#include "pin.h"

// Returns the pin mode. This value returned by this macro should be one of:
// MP_HAL_PIN_MODE_IN, MP_HAL_PIN_MODE_OUT, MP_HAL_PIN_MODE_ANALOG,
// MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_MODE_ALT_OD, or MP_HAL_PIN_MODE_OUT_OD.
uint32_t pin_get_mode(const pin_obj_t *pin) {
    GPIO_TypeDef *gpio = pin->gpio;

	// get register CRL/CRH (pin0~pin7 use CRL, pin8~pin15 use CRH)
    register uint32_t *pReg = (uint32_t *)((uint32_t)(&gpio->CRL) + (pin->pin / 8 * 4));

	uint32_t cfg = (*pReg >> ( (pin->pin%8) * 4)) & 0x0f; // 4bit config value

	if (cfg == 0) {                  // CNF=0 and MODE=0, Analog Input
		return 3;                    // MP_HAL_PIN_MODE_ANALOG
	} else if ( (cfg & 0x03) == 0) { // MODE=0, means Input (floating/pull-up/pull-down)
		return 0;                    // MP_HAL_PIN_MODE_IN
	}
	cfg >>= 2;                       // the other modes can be computed with high 2bit

	// CNF[1:0]  Macro[3:0] Value  Macro Name
	// 00b       (1=0001b)         MP_HAL_PIN_MODE_OUT
	// 10b       (2=0010b)         MP_HAL_PIN_MODE_ALT
	// 01b       (5=0101b)         MP_HAL_PIN_MODE_OUT_OD
	// 11b       (6=0110b)         MP_HAL_PIN_MODE_ALT_OD
	// 
	// CNF[0] is equal to Macro[2],  mark as (cfg & 1) << 2
	// CNF[1] is equal to Macro[1],  mark as (cfg & 2)
	// CNF[1] is equal to ~Macro[0], mark as (~cfg & 2)) >> 1
	uint32_t mode= (uint32_t) (
        ((cfg & 1) << 2) |
        (cfg & 2)        |
        ((~cfg & 2) >> 1)
    );
    return mode;
}

// Returns the pin pullup/pulldown. The value returned by this macro should
// be one of 0, GPIO_PULLUP, or GPIO_PULLDOWN.
uint32_t pin_get_pull(const pin_obj_t *pin) {
    GPIO_TypeDef *gpio = pin->gpio;
    register uint32_t *pReg = (uint32_t *)((uint32_t)(&gpio->CRL) + (pin->pin / 8 * 4));
	uint32_t cfg = (*pReg >> ( (pin->pin%8) * 4)) & 0x0f; // 4bit config value

    if ( (cfg & 0x03) || ((cfg&0x0b) == 0) ) {
        // out,alt, analog, input floating
        return 0;
    } else {
        // input with pull
        // 1 - pull-up
        // 0 - pull-down
        return 2 - ( (gpio->ODR >> pin->pin) & 0x01 );
    }
}

// Returns the af (alternate function) index currently set for a pin.
uint32_t pin_get_af(const pin_obj_t *pin) {
	return 0; // 对于STM32F1 系列 af 值 没啥用(或者考虑将AFIO->MAPR, AFIO->MAPR2 的 mask 及 索引位置 返回)
}
