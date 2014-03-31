#include "stm32f4xx_hal.h"

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "rng.h"

STATIC RNG_HandleTypeDef RngHandle;

void rng_init(void) {
    __RNG_CLK_ENABLE();
    RngHandle.Instance = RNG;
    HAL_RNG_Init(&RngHandle);
}

uint32_t rng_get(void) {
    return HAL_RNG_GetRandomNumber(&RngHandle);
}

STATIC mp_obj_t pyb_rng_get(void) {
    return mp_obj_new_int(HAL_RNG_GetRandomNumber(&RngHandle) >> 2);
}

MP_DEFINE_CONST_FUN_OBJ_0(pyb_rng_get_obj, pyb_rng_get);
