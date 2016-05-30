#include "gfx.h"

#if (GFX_USE_GINPUT && GINPUT_NEED_TOGGLE) 

#include "src/ginput/ginput_driver_toggle.h"

GINPUT_TOGGLE_DECLARE_STRUCTURE();

void ginput_lld_toggle_init(const GToggleConfig *ptc) {
	//palSetGroupMode(((IOBus *)ptc->id)->portid, ptc->mask, 0, ptc->mode);
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.Pin = ptc->mask;
  GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
  GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(ptc->id, &GPIO_InitStructure);
}

unsigned ginput_lld_toggle_getbits(const GToggleConfig *ptc) {
	//return palReadBus((IOBus *)ptc->id);
}

#endif /* GFX_USE_GINPUT && GINPUT_NEED_TOGGLE */