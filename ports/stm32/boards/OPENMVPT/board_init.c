#include "py/mphal.h"
#include "storage.h"
#include "sdram.h"

#define OMV_BOOTLOADER_MAGIC_ADDR   (0x2001FFFCU)
#define OMV_BOOTLOADER_MAGIC_VALUE  (0xB00710ADU)

void board_early_init(void) {
    // Hold the FLIR Lepton in reset until its master clock is running: the part
    // wants MCLK present when reset is released.
    mp_hal_pin_config(pyb_pin_LEPTON_RSTN, MP_HAL_PIN_MODE_OUTPUT, MP_HAL_PIN_PULL_NONE, 0);
    mp_hal_pin_config_speed(pyb_pin_LEPTON_RSTN, MP_HAL_PIN_SPEED_LOW);
    mp_hal_pin_write(pyb_pin_LEPTON_RSTN, 0);

    // Release powerdown.
    mp_hal_pin_config(pyb_pin_LEPTON_PWDN, MP_HAL_PIN_MODE_OUTPUT, MP_HAL_PIN_PULL_NONE, 0);
    mp_hal_pin_config_speed(pyb_pin_LEPTON_PWDN, MP_HAL_PIN_SPEED_LOW);
    mp_hal_pin_write(pyb_pin_LEPTON_PWDN, 1);

    // Enable FLIR Lepton MCLK.
    mp_hal_pin_config(pyb_pin_LEPTON_MCLK, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, 4);
    mp_hal_pin_config_speed(pyb_pin_LEPTON_MCLK, MP_HAL_PIN_SPEED_HIGH);

    // Hardcode the clock frequency as SystemClock_Config() hasn't been called yet.
    int tclk = 120000000 * 2; // HAL_RCC_GetPCLK2Freq() * 2
    int period = (tclk / 24000000) - 1;

    TIM_HandleTypeDef mclk_tim_handle;
    mclk_tim_handle.Instance = TIM15;
    mclk_tim_handle.Init.Prescaler = 0;
    mclk_tim_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    mclk_tim_handle.Init.Period = period;
    mclk_tim_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    mclk_tim_handle.Init.RepetitionCounter = 0;
    mclk_tim_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    TIM_OC_InitTypeDef mclk_tim_oc_handle;
    mclk_tim_oc_handle.Pulse = (period + 1) / 2;
    mclk_tim_oc_handle.OCMode = TIM_OCMODE_PWM1;
    mclk_tim_oc_handle.OCPolarity = TIM_OCPOLARITY_HIGH;
    mclk_tim_oc_handle.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    mclk_tim_oc_handle.OCFastMode = TIM_OCFAST_DISABLE;
    mclk_tim_oc_handle.OCIdleState = TIM_OCIDLESTATE_RESET;
    mclk_tim_oc_handle.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    __HAL_RCC_TIM15_CLK_ENABLE();
    __HAL_RCC_TIM15_CLK_SLEEP_ENABLE();
    __HAL_RCC_TIM15_FORCE_RESET();
    __HAL_RCC_TIM15_RELEASE_RESET();

    HAL_TIM_PWM_Init(&mclk_tim_handle);
    HAL_TIM_PWM_ConfigChannel(&mclk_tim_handle, &mclk_tim_oc_handle, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&mclk_tim_handle, TIM_CHANNEL_2);

    // Release the Lepton from reset after well over the 5000 MCLK periods it
    // requires.  SystemClock_Config() has not run yet, so this is a spin rather
    // than mp_hal_delay_ms(): at the 64MHz HSI boot clock it is well past 210us.
    for (volatile uint32_t i = 0; i < 50000; i++) {
    }
    mp_hal_pin_write(pyb_pin_LEPTON_RSTN, 1);
}

void board_low_power(int mode) {
    switch (mode) {
        case 0:     // Leave stop mode.
            sdram_leave_low_power();
            break;
        case 1:     // Enter stop mode.
            sdram_enter_low_power();
            break;
        case 2:     // Enter standby mode.
            sdram_enter_power_down();
            break;
    }
    // Enable QSPI deepsleep for modes 1 and 2
    mp_spiflash_deepsleep(&spi_bdev.spiflash, (mode != 0));
}

void board_enter_bootloader(void) {
    *((uint32_t *)OMV_BOOTLOADER_MAGIC_ADDR) = OMV_BOOTLOADER_MAGIC_VALUE;
    NVIC_SystemReset();
}
