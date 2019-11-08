/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Glenn Ruben Bakke
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

#include <arm_cmse.h>
#include <nrf.h>

// Secure flash 32K.
#define SECURE_32K_FLASH_PAGE_START    (0)
#define SECURE_32K_FLASH_PAGE_END      (0)

// Non-secure flash 992K.
#define NONSECURE_32K_FLASH_PAGE_START (1)
#define NONSECURE_32K_FLASH_PAGE_END   (31)

// Secure RAM 64K.
#define SECURE_8K_RAM_BLOCK_START      (0)
#define SECURE_8K_RAM_BLOCK_END        (7)

// Non-secure RAM 128K + 64K BSD lib.
#define NONSECURE_8K_RAM_BLOCK_START   (8)
#define NONSECURE_8K_RAM_BLOCK_END     (31)

#define PERIPHERAL_ID_GET(base_addr) (((uint32_t)(base_addr) >> 12) & 0xFF)

#if !defined(__ARM_FEATURE_CMSE)
    #pragma warning "CMSE not enabled"
#endif

static void configure_flash(void) {
    for (uint8_t i = SECURE_32K_FLASH_PAGE_START; i <= SECURE_32K_FLASH_PAGE_END; i++) {
        uint32_t perm = 0;
        perm |= (SPU_FLASHREGION_PERM_EXECUTE_Enable << SPU_FLASHREGION_PERM_EXECUTE_Pos);
        perm |= (SPU_FLASHREGION_PERM_WRITE_Enable   << SPU_FLASHREGION_PERM_WRITE_Pos);
        perm |= (SPU_FLASHREGION_PERM_READ_Enable    << SPU_FLASHREGION_PERM_READ_Pos);
        perm |= (SPU_FLASHREGION_PERM_LOCK_Locked    << SPU_FLASHREGION_PERM_LOCK_Pos);
        perm |= (SPU_FLASHREGION_PERM_SECATTR_Secure << SPU_FLASHREGION_PERM_SECATTR_Pos);
        NRF_SPU_S->FLASHREGION[i].PERM = perm;
    }

    for (uint8_t i = NONSECURE_32K_FLASH_PAGE_START; i <= NONSECURE_32K_FLASH_PAGE_END; i++) {
        uint32_t perm = 0;
        perm |= (SPU_FLASHREGION_PERM_EXECUTE_Enable     << SPU_FLASHREGION_PERM_EXECUTE_Pos);
        perm |= (SPU_FLASHREGION_PERM_WRITE_Enable       << SPU_FLASHREGION_PERM_WRITE_Pos);
        perm |= (SPU_FLASHREGION_PERM_READ_Enable        << SPU_FLASHREGION_PERM_READ_Pos);
        perm |= (SPU_FLASHREGION_PERM_LOCK_Locked        << SPU_FLASHREGION_PERM_LOCK_Pos);
        perm |= (SPU_FLASHREGION_PERM_SECATTR_Non_Secure << SPU_FLASHREGION_PERM_SECATTR_Pos);
        NRF_SPU_S->FLASHREGION[i].PERM = perm;
    }
}

static void configure_ram(void) {
    for (uint8_t i = SECURE_8K_RAM_BLOCK_START; i <= SECURE_8K_RAM_BLOCK_END; i++) {
        uint32_t perm = 0;
        perm |= (SPU_RAMREGION_PERM_EXECUTE_Enable << SPU_RAMREGION_PERM_EXECUTE_Pos);
        perm |= (SPU_RAMREGION_PERM_WRITE_Enable << SPU_RAMREGION_PERM_WRITE_Pos);
        perm |= (SPU_RAMREGION_PERM_READ_Enable << SPU_RAMREGION_PERM_READ_Pos);
        perm |= (SPU_RAMREGION_PERM_LOCK_Locked << SPU_RAMREGION_PERM_LOCK_Pos);
        perm |= (SPU_RAMREGION_PERM_SECATTR_Secure << SPU_RAMREGION_PERM_SECATTR_Pos);
        NRF_SPU_S->RAMREGION[i].PERM = perm;
    }

    for (uint8_t i = NONSECURE_8K_RAM_BLOCK_START; i <= NONSECURE_8K_RAM_BLOCK_END; i++) {
        uint32_t perm = 0;
        perm |= (SPU_RAMREGION_PERM_EXECUTE_Enable << SPU_RAMREGION_PERM_EXECUTE_Pos);
        perm |= (SPU_RAMREGION_PERM_WRITE_Enable << SPU_RAMREGION_PERM_WRITE_Pos);
        perm |= (SPU_RAMREGION_PERM_READ_Enable << SPU_RAMREGION_PERM_READ_Pos);
        perm |= (SPU_RAMREGION_PERM_LOCK_Locked << SPU_RAMREGION_PERM_LOCK_Pos);
        perm |= (SPU_RAMREGION_PERM_SECATTR_Non_Secure << SPU_RAMREGION_PERM_SECATTR_Pos);
        NRF_SPU_S->RAMREGION[i].PERM = perm;
    }
}

static void peripheral_setup(uint8_t peripheral_id)
{
    NVIC_DisableIRQ(peripheral_id);
    uint32_t perm = 0;
    perm |= (SPU_PERIPHID_PERM_PRESENT_IsPresent << SPU_PERIPHID_PERM_PRESENT_Pos);
    perm |= (SPU_PERIPHID_PERM_SECATTR_NonSecure << SPU_PERIPHID_PERM_SECATTR_Pos);
    perm |= (SPU_PERIPHID_PERM_LOCK_Locked << SPU_PERIPHID_PERM_LOCK_Pos);
    NRF_SPU_S->PERIPHID[peripheral_id].PERM = perm;

    NVIC_SetTargetState(peripheral_id);
}

static void configure_peripherals(void)
{
    NRF_SPU_S->GPIOPORT[0].PERM = 0;
    peripheral_setup(PERIPHERAL_ID_GET(NRF_REGULATORS_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_CLOCK_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_UARTE0_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_UARTE1_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_UARTE2_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_UARTE3_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_SAADC_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_TIMER0_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_TIMER1_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_TIMER2_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_RTC0_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_RTC1_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_DPPIC_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_WDT_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_EGU1_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_EGU2_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_EGU3_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_EGU4_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_EGU5_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_PWM0_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_PWM1_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_PWM2_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_PWM3_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_PDM_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_I2S_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_IPC_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_FPU_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_GPIOTE1_NS));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_NVMC_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_VMC_S));
    peripheral_setup(PERIPHERAL_ID_GET(NRF_P0_NS));
}

typedef void __attribute__((cmse_nonsecure_call)) nsfunc(void);

static void jump_to_non_secure(void)
{
    TZ_SAU_Disable();
    SAU->CTRL |= SAU_CTRL_ALLNS_Msk;

    // Set NS vector table.
    uint32_t * vtor_ns = (uint32_t *)0x8000;
    SCB_NS->VTOR = (uint32_t)vtor_ns;

    // Allow for FPU to be used by NS.
    SCB->NSACR |= (1UL << SCB_NSACR_CP10_Pos) | (1UL << SCB_NSACR_CP11_Pos);

    // Set stack pointers.
    __TZ_set_MSP_NS(vtor_ns[0]);
    __TZ_set_PSP_NS(0);

    uint32_t control_ns = __TZ_get_CONTROL_NS();
    control_ns &= ~(CONTROL_SPSEL_Msk | CONTROL_nPRIV_Msk);
    __TZ_set_CONTROL_NS(control_ns);

    // Cast NS Reset_Handler to a non-secure function.
    nsfunc *fp = (nsfunc *)vtor_ns[1];
    fp =  (nsfunc *)((intptr_t)(fp) & ~1);

    if (cmse_is_nsfptr(fp)) {
        __DSB();
        __ISB();

        // Jump to Non-Secure function.
        fp();
    }
}

int main(void) {
    configure_flash();
    configure_ram();
    configure_peripherals();

    jump_to_non_secure();

    while (1) {
	;
    }

    return 0;
}

void _start(void) {main();}

