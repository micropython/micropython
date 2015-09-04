/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Daniel Campora
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

#include <stdint.h>
#include <string.h>

#include "py/mpstate.h"
#include MICROPY_HAL_H
#include "py/runtime.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "inc/hw_common_reg.h"
#include "inc/hw_memmap.h"
#include "cc3200_asm.h"
#include "rom_map.h"
#include "interrupt.h"
#include "systick.h"
#include "prcm.h"
#include "spi.h"
#include "pin.h"
#include "pybsleep.h"
#include "pybpin.h"
#include "simplelink.h"
#include "modnetwork.h"
#include "modwlan.h"
#include "osi.h"
#include "debug.h"
#include "mpexception.h"
#include "mpcallback.h"
#include "mperror.h"
#include "sleeprestore.h"
#include "serverstask.h"
#include "antenna.h"
#include "cryptohash.h"

/******************************************************************************
 DECLARE PRIVATE CONSTANTS
 ******************************************************************************/
#define SPIFLASH_INSTR_READ_STATUS              (0x05)
#define SPIFLASH_INSTR_DEEP_POWER_DOWN          (0xB9)
#define SPIFLASH_STATUS_BUSY                    (0x01)

#define LPDS_UP_TIME                            (425)         // 13 msec
#define LPDS_DOWN_TIME                          (98)          // 3 msec
#define USER_OFFSET                             (131)         // 4 smec
#define WAKEUP_TIME_LPDS                        (LPDS_UP_TIME + LPDS_DOWN_TIME + USER_OFFSET)   // 20 msec
#define WAKEUP_TIME_HIB                         (32768)       // 1 s

#define FORCED_TIMER_INTERRUPT_MS               (1)
#define FAILED_SLEEP_DELAY_MS                   (FORCED_TIMER_INTERRUPT_MS * 3)

/******************************************************************************
 DECLARE PRIVATE TYPES
 ******************************************************************************/
// storage memory for Cortex M4 registers
typedef struct {
    uint32_t     msp;
    uint32_t     psp;
    uint32_t     psr;
    uint32_t     primask;
    uint32_t     faultmask;
    uint32_t     basepri;
    uint32_t     control;
} arm_cm4_core_regs_t;

// storage memory for the NVIC registers
typedef struct {
    uint32_t vector_table;      // Vector Table Offset
    uint32_t aux_ctrl;          // Auxiliary control register
    uint32_t int_ctrl_state;    // Interrupt Control and State
    uint32_t app_int;           // Application Interrupt Reset control
    uint32_t sys_ctrl;          // System control
    uint32_t config_ctrl;       // Configuration control
    uint32_t sys_pri_1;         // System Handler Priority 1
    uint32_t sys_pri_2;         // System Handler Priority 2
    uint32_t sys_pri_3;         // System Handler Priority 3
    uint32_t sys_hcrs;          // System Handler control and state register
    uint32_t systick_ctrl;      // SysTick Control Status
    uint32_t systick_reload;    // SysTick Reload
    uint32_t systick_calib;     // SysTick Calibration
    uint32_t int_en[6];         // Interrupt set enable
    uint32_t int_priority[49];  // Interrupt priority
} nvic_reg_store_t;

typedef struct {
    mp_obj_base_t base;
    mp_obj_t      obj;
    WakeUpCB_t    wakeup;
} pybsleep_obj_t;

typedef struct {
    mp_obj_t    wlan_lpds_wake_cb;
    mp_obj_t    timer_lpds_wake_cb;
    mp_obj_t    gpio_lpds_wake_cb;
    uint        timer_wake_pwrmode;
} pybsleep_data_t;

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
STATIC const mp_obj_type_t pybsleep_type;
STATIC nvic_reg_store_t    *nvic_reg_store;
STATIC pybsleep_data_t   pybsleep_data = {NULL, NULL, NULL, 0};
volatile arm_cm4_core_regs_t vault_arm_registers;
STATIC pybsleep_reset_cause_t pybsleep_reset_cause = PYB_SLP_PWRON_RESET;
STATIC pybsleep_wake_reason_t pybsleep_wake_reason = PYB_SLP_WAKED_PWRON;

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
STATIC pybsleep_obj_t *pybsleep_find (mp_obj_t obj);
STATIC void pybsleep_flash_powerdown (void);
STATIC NORETURN void pybsleep_suspend_enter (void);
void pybsleep_suspend_exit (void);
STATIC void pybsleep_obj_wakeup (void);
STATIC void PRCMInterruptHandler (void);
STATIC void pybsleep_iopark (bool hibernate);
STATIC bool setup_timer_lpds_wake (void);
STATIC bool setup_timer_hibernate_wake (void);

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
__attribute__ ((section (".boot")))
void pybsleep_pre_init (void) {
    // allocate memory for nvic registers vault
    ASSERT ((nvic_reg_store = mem_Malloc(sizeof(nvic_reg_store_t))) != NULL);
}

void pybsleep_init0 (void) {
    // initialize the sleep objects list
    mp_obj_list_init(&MP_STATE_PORT(pybsleep_obj_list), 0);

    // register and enable the PRCM interrupt
    osi_InterruptRegister(INT_PRCM, (P_OSI_INTR_ENTRY)PRCMInterruptHandler, INT_PRIORITY_LVL_1);

    // disable all LPDS and hibernate wake up sources (WLAN is disabed/enabled before entering LDPS mode)
    MAP_PRCMLPDSWakeupSourceDisable(PRCM_LPDS_GPIO);
    MAP_PRCMLPDSWakeupSourceDisable(PRCM_LPDS_TIMER);
    MAP_PRCMHibernateWakeupSourceDisable(PRCM_HIB_SLOW_CLK_CTR | PRCM_HIB_GPIO2  | PRCM_HIB_GPIO4  | PRCM_HIB_GPIO13 |
                                         PRCM_HIB_GPIO17       | PRCM_HIB_GPIO11 | PRCM_HIB_GPIO24 | PRCM_HIB_GPIO26);

    // store the reset casue (if it's soft reset, leave it as it is)
    if (pybsleep_reset_cause != PYB_SLP_SOFT_RESET) {
        switch (MAP_PRCMSysResetCauseGet()) {
        case PRCM_POWER_ON:
            pybsleep_reset_cause = PYB_SLP_PWRON_RESET;
            break;
        case PRCM_CORE_RESET:
        case PRCM_MCU_RESET:
        case PRCM_SOC_RESET:
            pybsleep_reset_cause = PYB_SLP_HARD_RESET;
            break;
        case PRCM_WDT_RESET:
            pybsleep_reset_cause = PYB_SLP_WDT_RESET;
            break;
        case PRCM_HIB_EXIT:
            if (PRCMGetSpecialBit(PRCM_WDT_RESET_BIT)) {
                pybsleep_reset_cause = PYB_SLP_WDT_RESET;
            }
            else {
                pybsleep_reset_cause = PYB_SLP_HIB_RESET;
                // set the correct wake reason
                switch (MAP_PRCMHibernateWakeupCauseGet()) {
                case PRCM_HIB_WAKEUP_CAUSE_SLOW_CLOCK:
                    pybsleep_wake_reason = PYB_SLP_WAKED_BY_RTC;
                    break;
                case PRCM_HIB_WAKEUP_CAUSE_GPIO:
                    pybsleep_wake_reason = PYB_SLP_WAKED_BY_GPIO;
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
        }
    }
}

void pybsleep_signal_soft_reset (void) {
    pybsleep_reset_cause = PYB_SLP_SOFT_RESET;
}

void pybsleep_add (const mp_obj_t obj, WakeUpCB_t wakeup) {
    pybsleep_obj_t * sleep_obj = m_new_obj(pybsleep_obj_t);
    sleep_obj->base.type = &pybsleep_type;
    sleep_obj->obj = obj;
    sleep_obj->wakeup = wakeup;
    // remove it in case it was already registered
    pybsleep_remove (obj);
    mp_obj_list_append(&MP_STATE_PORT(pybsleep_obj_list), sleep_obj);
}

void pybsleep_remove (const mp_obj_t obj) {
    pybsleep_obj_t *sleep_obj;
    if ((sleep_obj = pybsleep_find(obj))) {
        mp_obj_list_remove(&MP_STATE_PORT(pybsleep_obj_list), sleep_obj);
    }
}

void pybsleep_set_wlan_lpds_callback (mp_obj_t cb_obj) {
    pybsleep_data.wlan_lpds_wake_cb = cb_obj;
}

void pybsleep_set_gpio_lpds_callback (mp_obj_t cb_obj) {
    pybsleep_data.gpio_lpds_wake_cb = cb_obj;
}

void pybsleep_set_timer_lpds_callback (mp_obj_t cb_obj) {
    pybsleep_data.timer_lpds_wake_cb = cb_obj;
}

void pybsleep_configure_timer_wakeup (uint pwrmode) {
    pybsleep_data.timer_wake_pwrmode = pwrmode;
}

pybsleep_reset_cause_t pybsleep_get_reset_cause (void) {
    return pybsleep_reset_cause;
}

/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/
STATIC pybsleep_obj_t *pybsleep_find (mp_obj_t obj) {
    for (mp_uint_t i = 0; i < MP_STATE_PORT(pybsleep_obj_list).len; i++) {
        // search for the object and then remove it
        pybsleep_obj_t *sleep_obj = ((pybsleep_obj_t *)(MP_STATE_PORT(pybsleep_obj_list).items[i]));
        if (sleep_obj->obj == obj) {
            return sleep_obj;
        }
    }
    return NULL;
}

STATIC void pybsleep_flash_powerdown (void) {
    uint32_t status;

    // Enable clock for SSPI module
    MAP_PRCMPeripheralClkEnable(PRCM_SSPI, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);
    // Reset SSPI at PRCM level and wait for reset to complete
    MAP_PRCMPeripheralReset(PRCM_SSPI);
    while(!MAP_PRCMPeripheralStatusGet(PRCM_SSPI));

    // Reset SSPI at module level
    MAP_SPIReset(SSPI_BASE);
    // Configure SSPI module
    MAP_SPIConfigSetExpClk (SSPI_BASE, PRCMPeripheralClockGet(PRCM_SSPI),
                            20000000, SPI_MODE_MASTER,SPI_SUB_MODE_0,
                            (SPI_SW_CTRL_CS   |
                             SPI_4PIN_MODE    |
                             SPI_TURBO_OFF    |
                             SPI_CS_ACTIVELOW |
                             SPI_WL_8));

    // Enable SSPI module
    MAP_SPIEnable(SSPI_BASE);
    // Enable chip select for the spi flash.
    MAP_SPICSEnable(SSPI_BASE);
    // Wait for the spi flash
    do {
        // Send the status register read instruction and read back a dummy byte.
        MAP_SPIDataPut(SSPI_BASE, SPIFLASH_INSTR_READ_STATUS);
        MAP_SPIDataGet(SSPI_BASE, &status);

        // Write a dummy byte then read back the actual status.
        MAP_SPIDataPut(SSPI_BASE, 0xFF);
        MAP_SPIDataGet(SSPI_BASE, &status);
    } while ((status & 0xFF) == SPIFLASH_STATUS_BUSY);

    // Disable chip select for the spi flash.
    MAP_SPICSDisable(SSPI_BASE);
    // Start another CS enable sequence for Power down command.
    MAP_SPICSEnable(SSPI_BASE);
    // Send Deep Power Down command to spi flash
    MAP_SPIDataPut(SSPI_BASE, SPIFLASH_INSTR_DEEP_POWER_DOWN);
    // Disable chip select for the spi flash.
    MAP_SPICSDisable(SSPI_BASE);
}

STATIC NORETURN void pybsleep_suspend_enter (void) {
    // enable full RAM retention
    MAP_PRCMSRAMRetentionEnable(PRCM_SRAM_COL_1 | PRCM_SRAM_COL_2 | PRCM_SRAM_COL_3 | PRCM_SRAM_COL_4, PRCM_SRAM_LPDS_RET);

    // save the NVIC control registers
    nvic_reg_store->vector_table = HWREG(NVIC_VTABLE);
    nvic_reg_store->aux_ctrl = HWREG(NVIC_ACTLR);
    nvic_reg_store->int_ctrl_state = HWREG(NVIC_INT_CTRL);
    nvic_reg_store->app_int = HWREG(NVIC_APINT);
    nvic_reg_store->sys_ctrl = HWREG(NVIC_SYS_CTRL);
    nvic_reg_store->config_ctrl = HWREG(NVIC_CFG_CTRL);
    nvic_reg_store->sys_pri_1 = HWREG(NVIC_SYS_PRI1);
    nvic_reg_store->sys_pri_2 = HWREG(NVIC_SYS_PRI2);
    nvic_reg_store->sys_pri_3 = HWREG(NVIC_SYS_PRI3);
    nvic_reg_store->sys_hcrs = HWREG(NVIC_SYS_HND_CTRL);

    // save the systick registers
    nvic_reg_store->systick_ctrl = HWREG(NVIC_ST_CTRL);
    nvic_reg_store->systick_reload = HWREG(NVIC_ST_RELOAD);
    nvic_reg_store->systick_calib = HWREG(NVIC_ST_CAL);

    // save the interrupt enable registers
    uint32_t *base_reg_addr = (uint32_t *)NVIC_EN0;
    for(int32_t i = 0; i < (sizeof(nvic_reg_store->int_en) / 4); i++) {
        nvic_reg_store->int_en[i] = base_reg_addr[i];
    }

    // save the interrupt priority registers
    base_reg_addr = (uint32_t *)NVIC_PRI0;
    for(int32_t i = 0; i < (sizeof(nvic_reg_store->int_priority) / 4); i++) {
        nvic_reg_store->int_priority[i] = base_reg_addr[i];
    }

    // switch off the heartbeat led (this makes sure it will blink as soon as we wake up)
    mperror_heartbeat_switch_off();

    // park the gpio pins
    pybsleep_iopark(false);

    // store the cpu registers
    sleep_store();

    // save the restore info and enter LPDS
    MAP_PRCMLPDSRestoreInfoSet(vault_arm_registers.psp, (uint32_t)sleep_restore);
    MAP_PRCMLPDSEnter();

    // let the cpu fade away...
    for ( ; ; );
}

void pybsleep_suspend_exit (void) {
    // take the I2C semaphore
    uint32_t reg = HWREG(COMMON_REG_BASE + COMMON_REG_O_I2C_Properties_Register);
    reg = (reg & ~0x3) | 0x1;
    HWREG(COMMON_REG_BASE + COMMON_REG_O_I2C_Properties_Register) = reg;

    // take the GPIO semaphore
    reg = HWREG(COMMON_REG_BASE + COMMON_REG_O_GPIO_properties_register);
    reg = (reg & ~0x3FF) | 0x155;
    HWREG(COMMON_REG_BASE + COMMON_REG_O_GPIO_properties_register) = reg;

    // restore de NVIC control registers
    HWREG(NVIC_VTABLE) = nvic_reg_store->vector_table;
    HWREG(NVIC_ACTLR) = nvic_reg_store->aux_ctrl;
    HWREG(NVIC_INT_CTRL) = nvic_reg_store->int_ctrl_state;
    HWREG(NVIC_APINT) = nvic_reg_store->app_int;
    HWREG(NVIC_SYS_CTRL) = nvic_reg_store->sys_ctrl;
    HWREG(NVIC_CFG_CTRL) = nvic_reg_store->config_ctrl;
    HWREG(NVIC_SYS_PRI1) = nvic_reg_store->sys_pri_1;
    HWREG(NVIC_SYS_PRI2) = nvic_reg_store->sys_pri_2;
    HWREG(NVIC_SYS_PRI3) = nvic_reg_store->sys_pri_3;
    HWREG(NVIC_SYS_HND_CTRL) = nvic_reg_store->sys_hcrs;

    // restore the systick register
    HWREG(NVIC_ST_CTRL) = nvic_reg_store->systick_ctrl;
    HWREG(NVIC_ST_RELOAD) = nvic_reg_store->systick_reload;
    HWREG(NVIC_ST_CAL) = nvic_reg_store->systick_calib;

    // restore the interrupt priority registers
    uint32_t *base_reg_addr = (uint32_t *)NVIC_PRI0;
    for (uint32_t i = 0; i < (sizeof(nvic_reg_store->int_priority) / 4); i++) {
        base_reg_addr[i] = nvic_reg_store->int_priority[i];
    }

    // restore the interrupt enable registers
    base_reg_addr = (uint32_t *)NVIC_EN0;
    for(uint32_t i = 0; i < (sizeof(nvic_reg_store->int_en) / 4); i++) {
        base_reg_addr[i] = nvic_reg_store->int_en[i];
    }

    HAL_INTRODUCE_SYNC_BARRIER();

    // ungate the clock to the shared spi bus
    MAP_PRCMPeripheralClkEnable(PRCM_SSPI, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);

#if MICROPY_HW_ANTENNA_DIVERSITY
    // re-configure the antenna selection pins
    antenna_init0();
#endif

    // reinitialize simplelink's interface
    sl_IfOpen (NULL, 0);

    // restore the configuration of all active peripherals
    pybsleep_obj_wakeup();

    // reconfigure all the previously enabled interrupts
    mpcallback_wake_all();

    // we need to init the crypto hash engine again
    CRYPTOHASH_Init();

    // trigger a sw interrupt
    MAP_IntPendSet(INT_PRCM);

    // force an exception to go back to the point where suspend mode was entered
    nlr_raise(mp_obj_new_exception(&mp_type_SystemExit));
}

STATIC void PRCMInterruptHandler (void) {
    // reading the interrupt status automatically clears the interrupt
    if (PRCM_INT_SLOW_CLK_CTR == MAP_PRCMIntStatus()) {
        // this interrupt is triggered during active mode
        mpcallback_handler(pybsleep_data.timer_lpds_wake_cb);
    }
    else {
        // interrupt has been triggered while waking up from LPDS
        switch (MAP_PRCMLPDSWakeupCauseGet()) {
        case PRCM_LPDS_HOST_IRQ:
            mpcallback_handler(pybsleep_data.wlan_lpds_wake_cb);
            pybsleep_wake_reason = PYB_SLP_WAKED_BY_WLAN;
            break;
        case PRCM_LPDS_GPIO:
            mpcallback_handler(pybsleep_data.gpio_lpds_wake_cb);
            pybsleep_wake_reason = PYB_SLP_WAKED_BY_GPIO;
            break;
        case PRCM_LPDS_TIMER:
            // disable the timer as wake-up source
            pybsleep_data.timer_wake_pwrmode &= ~PYB_PWR_MODE_LPDS;
            MAP_PRCMLPDSWakeupSourceDisable(PRCM_LPDS_TIMER);
            mpcallback_handler(pybsleep_data.timer_lpds_wake_cb);
            pybsleep_wake_reason = PYB_SLP_WAKED_BY_RTC;
            break;
        default:
            break;
        }
    }
}

STATIC void pybsleep_obj_wakeup (void) {
    for (mp_uint_t i = 0; i < MP_STATE_PORT(pybsleep_obj_list).len; i++) {
        pybsleep_obj_t *sleep_obj = ((pybsleep_obj_t *)MP_STATE_PORT(pybsleep_obj_list).items[i]);
        sleep_obj->wakeup(sleep_obj->obj);
    }
}

STATIC void pybsleep_iopark (bool hibernate) {
    mp_map_t *named_map = mp_obj_dict_get_map((mp_obj_t)&pin_board_pins_locals_dict);
    for (uint i = 0; i < named_map->used; i++) {
        pin_obj_t * pin = (pin_obj_t *)named_map->table[i].value;
        switch (pin->pin_num) {
#ifdef DEBUG
        // skip the JTAG pins
        case PIN_16:
        case PIN_17:
        case PIN_19:
        case PIN_20:
            break;
#endif
        default:
            // enable a weak pull-down if the pin is unused
            if (!pin->used) {
                MAP_PinConfigSet(pin->pin_num, pin->strength, PIN_TYPE_STD_PD);
            }
            if (hibernate) {
                // make it an input
                MAP_PinDirModeSet(pin->pin_num, PIN_DIR_MODE_IN);
            }
            break;
        }
    }

    // park the sflash pins
    HWREG(0x4402E0E8) &= ~(0x3 << 8);
    HWREG(0x4402E0E8) |= (0x2 << 8);
    HWREG(0x4402E0EC) &= ~(0x3 << 8);
    HWREG(0x4402E0EC) |= (0x2 << 8);
    HWREG(0x4402E0F0) &= ~(0x3 << 8);
    HWREG(0x4402E0F0) |= (0x2 << 8);
    HWREG(0x4402E0F4) &= ~(0x3 << 8);
    HWREG(0x4402E0F4) |= (0x1 << 8);

    // if the board has antenna diversity, only park the antenna
    // selection pins when going into hibernation
#if MICROPY_HW_ANTENNA_DIVERSITY
    if (hibernate) {
#endif
        // park the antenna selection pins
        // (tri-stated with pull down enabled)
        HWREG(0x4402E108) = 0x00000E61;
        HWREG(0x4402E10C) = 0x00000E61;
#if MICROPY_HW_ANTENNA_DIVERSITY
    } else {
        // park the antenna selection pins
        // (tri-stated without changing the pull up/down resistors)
        HWREG(0x4402E108) &= ~0x000000FF;
        HWREG(0x4402E108) |=  0x00000C61;
        HWREG(0x4402E10C) &= ~0x000000FF;
        HWREG(0x4402E10C) |=  0x00000C61;
    }
#endif
}

STATIC bool setup_timer_lpds_wake (void) {
    uint64_t t_match, t_curr, t_remaining;

    // get the time remaining for the RTC timer to expire
    t_match = MAP_PRCMSlowClkCtrMatchGet();
    t_curr  = MAP_PRCMSlowClkCtrGet();

    if (t_match > t_curr) {
        // get the time remaining in terms of slow clocks
        t_remaining = (t_match - t_curr);
        if (t_remaining > WAKEUP_TIME_LPDS) {
            // subtract the time it takes for wakeup from lpds
            t_remaining -= WAKEUP_TIME_LPDS;
            t_remaining = (t_remaining > 0xFFFFFFFF) ? 0xFFFFFFFF: t_remaining;
            // setup the LPDS wake time
            MAP_PRCMLPDSIntervalSet((uint32_t)t_remaining);
            // enable the wake source
            MAP_PRCMLPDSWakeupSourceEnable(PRCM_LPDS_TIMER);
            return true;
        }
    }
    else {
        // setup a timer interrupt immediately
        MAP_PRCMRTCMatchSet(0, FORCED_TIMER_INTERRUPT_MS);
    }

    // disable the timer as wake source
    MAP_PRCMLPDSWakeupSourceDisable(PRCM_LPDS_TIMER);

    // LPDS wake by timer was not possible, force
    // an interrupt in active mode instead
    MAP_PRCMIntEnable(PRCM_INT_SLOW_CLK_CTR);

    return false;
}

STATIC bool setup_timer_hibernate_wake (void) {
    uint64_t t_match, t_curr, t_remaining;

    // get the time remaining for the RTC timer to expire
    t_match = MAP_PRCMSlowClkCtrMatchGet();
    t_curr  = MAP_PRCMSlowClkCtrGet();

    if (t_match > t_curr) {
        // get the time remaining in terms of slow clocks
        t_remaining = (t_match - t_curr);
        if (t_remaining > WAKEUP_TIME_HIB) {
            // subtract the time it takes for wakeup from hibernate
            t_remaining -= WAKEUP_TIME_HIB;
            // setup the LPDS wake time
            MAP_PRCMHibernateIntervalSet((uint32_t)t_remaining);
            // enable the wake source
            MAP_PRCMHibernateWakeupSourceEnable(PRCM_HIB_SLOW_CLK_CTR);
            return true;
        }
    }
    else {
        // setup a timer interrupt immediately
        MAP_PRCMRTCMatchSet(0, FORCED_TIMER_INTERRUPT_MS);
    }

    // disable the timer as wake source
    MAP_PRCMLPDSWakeupSourceDisable(PRCM_HIB_SLOW_CLK_CTR);

    // hibernate wake by timer was not possible, force
    // an interrupt in active mode instead
    MAP_PRCMIntEnable(PRCM_INT_SLOW_CLK_CTR);

    return false;
}

/******************************************************************************/
// Micro Python bindings; Sleep class

/// \function idle()
/// Gates the processor clock until an interrupt is triggered
STATIC mp_obj_t pyb_sleep_idle (mp_obj_t self_in) {
    __WFI();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_sleep_idle_obj, pyb_sleep_idle);

/// \function suspend(wlan)
/// Enters suspended mode. Wake up sources should have been enable prior to
/// calling this method.
STATIC mp_obj_t pyb_sleep_suspend (mp_obj_t self_in) {
    nlr_buf_t nlr;

    // check if we should enable timer wake-up
    if (pybsleep_data.timer_wake_pwrmode & PYB_PWR_MODE_LPDS) {
        if (!setup_timer_lpds_wake()) {
            // lpds entering is not possible, wait for the forced interrupt and return
            pybsleep_data.timer_wake_pwrmode &= ~PYB_PWR_MODE_LPDS;
            HAL_Delay (FAILED_SLEEP_DELAY_MS);
            return mp_const_none;
        }
    }

    // do we need network wake-up?
    if (pybsleep_data.wlan_lpds_wake_cb) {
        MAP_PRCMLPDSWakeupSourceEnable (PRCM_LPDS_HOST_IRQ);
        server_sleep_sockets();
    }
    else {
        MAP_PRCMLPDSWakeupSourceDisable (PRCM_LPDS_HOST_IRQ);
    }

    // entering and exiting suspended mode must be an atomic operation
    // therefore interrupts need to be disabled
    uint primsk = disable_irq();
    if (nlr_push(&nlr) == 0) {
        pybsleep_suspend_enter();
        nlr_pop();
    }

    // an exception is always raised when exiting suspend mode
    enable_irq(primsk);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_sleep_suspend_obj, pyb_sleep_suspend);

/// \function hibernate()
/// Enters hibernate mode. Wake up sources should have been enable prior to
/// calling this method.
STATIC mp_obj_t pyb_sleep_hibernate (mp_obj_t self_in) {
    // check if we should enable timer wake-up
    if (pybsleep_data.timer_wake_pwrmode & PYB_PWR_MODE_HIBERNATE) {
        if (!setup_timer_hibernate_wake()) {
            // hibernating is not possible, wait for the forced interrupt and return
            pybsleep_data.timer_wake_pwrmode &= ~PYB_PWR_MODE_HIBERNATE;
            HAL_Delay (FAILED_SLEEP_DELAY_MS);
            return mp_const_none;
        }
    }
    wlan_stop(SL_STOP_TIMEOUT);
    pybsleep_flash_powerdown();
    // must be done just before entering hibernate mode
    pybsleep_iopark(true);
    MAP_PRCMHibernateEnter();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_sleep_hibernate_obj, pyb_sleep_hibernate);

/// \function reset_cause()
/// Returns the last reset casue
STATIC mp_obj_t pyb_sleep_reset_cause (mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(pybsleep_reset_cause);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_sleep_reset_cause_obj, pyb_sleep_reset_cause);

/// \function wake_reason()
/// Returns the wake up reson from ldps or hibernate
STATIC mp_obj_t pyb_sleep_wake_reason (mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(pybsleep_wake_reason);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_sleep_wake_reason_obj, pyb_sleep_wake_reason);

STATIC const mp_map_elem_t pybsleep_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_idle),                    (mp_obj_t)&pyb_sleep_idle_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_suspend),                 (mp_obj_t)&pyb_sleep_suspend_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_hibernate),               (mp_obj_t)&pyb_sleep_hibernate_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_reset_cause),             (mp_obj_t)&pyb_sleep_reset_cause_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_wake_reason),             (mp_obj_t)&pyb_sleep_wake_reason_obj },

    // class constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_ACTIVE),                  MP_OBJ_NEW_SMALL_INT(PYB_PWR_MODE_ACTIVE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SUSPENDED),               MP_OBJ_NEW_SMALL_INT(PYB_PWR_MODE_LPDS) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_HIBERNATING),             MP_OBJ_NEW_SMALL_INT(PYB_PWR_MODE_HIBERNATE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_POWER_ON),                MP_OBJ_NEW_SMALL_INT(PYB_SLP_PWRON_RESET) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_HARD_RESET),              MP_OBJ_NEW_SMALL_INT(PYB_SLP_HARD_RESET) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WDT_RESET),               MP_OBJ_NEW_SMALL_INT(PYB_SLP_WDT_RESET) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_HIB_RESET),               MP_OBJ_NEW_SMALL_INT(PYB_SLP_HIB_RESET) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SOFT_RESET),              MP_OBJ_NEW_SMALL_INT(PYB_SLP_SOFT_RESET) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WLAN_WAKE),               MP_OBJ_NEW_SMALL_INT(PYB_SLP_WAKED_BY_WLAN) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PIN_WAKE),                MP_OBJ_NEW_SMALL_INT(PYB_SLP_WAKED_BY_GPIO) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RTC_WAKE),                MP_OBJ_NEW_SMALL_INT(PYB_SLP_WAKED_BY_RTC) },
};

STATIC MP_DEFINE_CONST_DICT(pybsleep_locals_dict, pybsleep_locals_dict_table);

STATIC const mp_obj_type_t pybsleep_type = {
    { &mp_type_type },
    .name = MP_QSTR_Sleep,
    .locals_dict = (mp_obj_t)&pybsleep_locals_dict,
};

const mp_obj_base_t pyb_sleep_obj = {&pybsleep_type};
