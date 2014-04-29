#include <string.h>

#include "stm32f4xx_hal.h"

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "rng.h"

#if MICROPY_HW_ENABLE_RNG

/// \moduleref pyb

STATIC RNG_HandleTypeDef RNGHandle = {.Instance = NULL};

void rng_init0(void) {
    // reset the RNG handle
    memset(&RNGHandle, 0, sizeof(RNG_HandleTypeDef));
    RNGHandle.Instance = RNG;
}

void rng_init(void) {
    __RNG_CLK_ENABLE();
    HAL_RNG_Init(&RNGHandle);
}

uint32_t rng_get(void) {
    if (RNGHandle.State == HAL_RNG_STATE_RESET) {
        rng_init();
    }
    return HAL_RNG_GetRandomNumber(&RNGHandle);
}

/// \function rng()
/// Return a 30-bit hardware generated random number.
STATIC mp_obj_t pyb_rng_get(void) {
    if (RNGHandle.State == HAL_RNG_STATE_RESET) {
        rng_init();
    }
    return mp_obj_new_int(HAL_RNG_GetRandomNumber(&RNGHandle) >> 2);
}

MP_DEFINE_CONST_FUN_OBJ_0(pyb_rng_get_obj, pyb_rng_get);

#endif // MICROPY_HW_ENABLE_RNG
