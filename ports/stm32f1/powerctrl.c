/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2018 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/mperrno.h"
#include "py/mphal.h"
#include "powerctrl.h"
#include "rtc.h"
#include "genhdr/pllfreqtable.h"

// Location in RAM of bootloader state (just after the top of the stack)
extern uint32_t _estack[];
#define BL_STATE ((uint32_t *)&_estack)

NORETURN void powerctrl_mcu_reset(void) {
	BL_STATE[1] = 1; // invalidate bootloader address
	NVIC_SystemReset();
}

NORETURN void powerctrl_enter_bootloader(uint32_t r0, uint32_t bl_addr) {
	BL_STATE[0] = r0;
	BL_STATE[1] = bl_addr;
	NVIC_SystemReset();
}

static __attribute__((naked)) void branch_to_bootloader(uint32_t r0, uint32_t bl_addr) {
	__asm volatile(
		"ldr r2, [r1, #0]\n" // get address of stack pointer
		"msr msp, r2\n"		 // get stack pointer
		"ldr r2, [r1, #4]\n" // get address of destination
		"bx r2\n"			 // branch to bootloader
	);
}

void powerctrl_check_enter_bootloader(void) {
	uint32_t bl_addr = BL_STATE[1];
	BL_STATE[1] = 1; // invalidate bootloader address
	if ((bl_addr & 0xfff) == 0 && (RCC->CSR & RCC_CSR_SFTRSTF)) {
		// Reset by NVIC_SystemReset with bootloader data set -> branch to bootloader
		RCC->CSR = RCC_CSR_RMVF;
		uint32_t r0 = BL_STATE[0];
		branch_to_bootloader(r0, bl_addr);
	}
}

// Assumes that PLL is used as the SYSCLK source
int powerctrl_rcc_clock_config_pll(RCC_ClkInitTypeDef *rcc_init) {
	uint32_t flash_latency;
#if defined(MICROPY_HW_FLASH_LATENCY)
	flash_latency = MICROPY_HW_FLASH_LATENCY;
#else
	flash_latency = FLASH_ACR_LATENCY_2;
#endif

	rcc_init->SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	if (HAL_RCC_ClockConfig(rcc_init, flash_latency) != HAL_OK) {
		return -MP_EIO;
	}

	return 0;
}

int powerctrl_set_sysclk(uint32_t sysclk, uint32_t ahb, uint32_t apb1, uint32_t apb2) {
	if (
		sysclk == HAL_RCC_GetSysClockFreq() &&
		ahb == HAL_RCC_GetHCLKFreq()        &&
		apb1 == HAL_RCC_GetPCLK1Freq()      &&
		apb2 == HAL_RCC_GetPCLK2Freq()
	) {
		return 0;
	}
	mp_hal_delay_ms(5);

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_OscInitStruct.OscillatorType = MICROPY_HW_RCC_OSCILLATOR_TYPE;
	RCC_OscInitStruct.HSEState = MICROPY_HW_RCC_HSE_STATE;
	RCC_OscInitStruct.HSIState = MICROPY_HW_RCC_HSI_STATE;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.HSEPredivValue = MICROPY_HW_HSE_PREDIV;

	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = MICROPY_HW_RCC_PLL_SRC;
	RCC_OscInitStruct.PLL.PLLMUL = MICROPY_HW_CLK_PLLMUL;

	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		return -MP_EIO;
	}

	// Configure clock
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = MICROPY_HW_SYSCLK_SRC;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		return -MP_EIO;
	}
	// TODO: RTC CLOCK
	return 0;
}

void powerctrl_enter_stop_mode(void) {
	// Disable IRQs so that the IRQ that wakes the device from stop mode is not
	// executed until after the clocks are reconfigured
	uint32_t irq_state = disable_irq();
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

	// reconfigure the system clock after waking up
	// enable clock
	__HAL_RCC_HSE_CONFIG(MICROPY_HW_RCC_HSE_STATE);
#if MICROPY_HW_CLK_USE_HSI
	__HAL_RCC_HSI_ENABLE();
#endif
	while (!__HAL_RCC_GET_FLAG(MICROPY_HW_RCC_FLAG_HSxRDY));

	// enable PLL
	__HAL_RCC_PLL_ENABLE();
	while (!__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY));

	// select PLL as system clock source
	MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_SYSCLKSOURCE_PLLCLK);
	while (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_CFGR_SWS_PLL);

	// Enable IRQs now that all clocks are reconfigured
	enable_irq(irq_state);
}

void powerctrl_enter_standby_mode(void) {
	rtc_init_finalise();

	// We need to clear the PWR wake-up-flag before entering standby, since
	// the flag may have been set by a previous wake-up event.  Furthermore,
	// we need to disable the wake-up sources while clearing this flag, so
	// that if a source is active it does actually wake the device.
	// See section 5.3.7 of RM0090.

	// Note: we only support RTC ALRA, ALRB, WUT and TS.
	// TODO support TAMP and WKUP (PA0 external pin).

	// save RTC interrupts and disable it, clear RTC wake-up flags
	uint32_t save_irq_bits = RTC->CRH & RTC_CRH_ALRIE;
	RTC->CRH &= ~RTC_CRH_ALRIE;
	RTC->CRL &= ~RTC_CRL_ALRF;

	// clear global wake-up flag
	PWR->CR |= PWR_CR_CWUF;

	// enable previously-enabled RTC interrupts
	RTC->CRH |= save_irq_bits;

	// enter standby mode
	HAL_PWR_EnterSTANDBYMode();
	// we never return; MCU is reset on exit from standby
}
