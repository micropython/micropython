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

#include <std.h>
#include <stdint.h>
#include <string.h>

#include "py/mpstate.h"
#include MICROPY_HAL_H
#include "hw_types.h"
#include "pybsleep.h"

/* Storage memory for Cortex M4 registers. To make implementation independent of
   CPU, register specific constructs must be made part of platform services.
*/

typedef struct {
    uint32_t     msp;
    uint32_t     psp;
    uint32_t     psr;
    uint32_t     primask;
    uint32_t     faultmask;
    uint32_t     basepri;
    uint32_t     control;
}arm_cm4_core_regs;

//static arm_cm4_core_regs vault_arm_registers;


#define BACK_UP_ARM_REGISTERS() {                                  \
        __asm(" push {r0-r12, LR}                           \n"    \
          " ldr  r1, pxVaultRegistersSave                   \n"    \
          " mrs  r0, msp                                    \n"    \
          " str  r0, [r1]                                   \n"    \
          " mrs  r0, psp                                    \n"    \
          " str  r0, [r1, #4]                               \n"    \
          " mrs  r0, primask                                \n"    \
          " str  r0, [r1, #12]                              \n"    \
          " mrs  r0, faultmask                              \n"    \
          " str  r0, [r1, #16]                              \n"    \
          " mrs  r0, basepri                                \n"    \
          " str  r0, [r1, #20]                              \n"    \
          " mrs  r0, control                                \n"    \
          " str  r0, [r1, #24]                              \n"    \
          "pxVaultRegistersSave: .word vault_arm_registers  \n");  \
}

#define RESTORE_ARM_REGISTERS() {                                  \
    __asm(" ldr  r1, pxVaultRegistersLoad                   \n"    \
          " ldr  r0, [r1, #24]                              \n"    \
          " msr  control, r0                                \n"    \
          " ldr  r0, [r1]                                   \n"    \
          " msr  msp, r0                                    \n"    \
          " ldr  r0, [r1,#4]                                \n"    \
          " msr  psp, r0                                    \n"    \
          " ldr  r0, [r1, #12]                              \n"    \
          " msr  primask, r0                                \n"    \
          " ldr  r0, [r1, #16]                              \n"    \
          " msr  faultmask, r0                              \n"    \
          " ldr  r0, [r1, #20]                              \n"    \
          " msr  basepri, r0                                \n"    \
          " pop  {r0-r12, LR}                               \n"    \
          "pxVaultRegistersLoad: .word vault_arm_registers  \n");  \
}

#if 0
/* Called directly by boot ROM after waking from S3 state */
void resume_from_S3(void)
{
        /* Jump from ROM context hence introduce the sync barriers */
        INTRODUCE_SYNC_BARRIER(); /* Data and instruction sync barriers */

        RESTORE_ARM_REGISTERS(); /* Core registers and code is in assembly */

        INTRODUCE_SYNC_BARRIER(); /* Data and instruction sync barriers */

        pform->pm_ops->restore_soc_data();
        make_modules_to_M0_no_irq(pform->used_list_len); /* Wake up all  */
        pform->pm_ops->handle_S3_wakeup();   /* Should be last statement */
        return;
}

static void enter_into_S3(void)
{
    pform->pm_ops->back_up_soc_data();

    INTRODUCE_SYNC_BARRIER(); /* Data and instruction sync barriers */

    BACK_UP_ARM_REGISTERS(); /* Core registers and code is in assembly */

    cc_enter_S3(resume_from_S3, vault_arm_registers.psp/*save_restore[1]*/);

    /* Introducing delays to facilitate CPU to fade away ........ */
    asm(" NOP"); asm(" NOP"); asm(" NOP"); asm(" NOP"); asm(" NOP");
    asm(" NOP"); asm(" NOP"); asm(" NOP"); asm(" NOP"); asm(" NOP");
    asm(" NOP"); asm(" NOP"); asm(" NOP"); asm(" NOP"); asm(" NOP");
}

static void apply_io_park(u8 pin_num,
                          enum io_park_state park_value)
{
        u32 pin_strength, pin_type;

        if(DONT_CARE != park_value) {
                /* Change the pin mode to GPIO to be safe */
                //MAP_PinModeSet(pin_num, PIN_MODE_0);

                /* First apply PullUp/PullDn (or no pull) according
                to the default levels specified in the user supplied
                parking table */
                MAP_PinConfigGet(pin_num, &pin_strength, &pin_type);

                if(NO_PULL_HIZ != park_value) {
                        MAP_PinConfigSet(pin_num, pin_strength, park_value);
                } else {
                        MAP_PinConfigSet(pin_num, pin_strength, PIN_TYPE_STD);
                }

                /* One by one HiZ all the IOs,
                  by writing the register that drives IOEN_N control
                  pin of the IOs. This register and the signal path is
                  always-on and hence not get lost during True-LPDS */
                MAP_PinDirModeSet(pin_num, PIN_DIR_MODE_IN);

                /* Once all the digital IOs has been made HiZ,
                  the desired default PAD levels would be held by
                  the weak-pulls. Input buffers would be alive
                  (such as auto-SPI or wake-GPIOs) and would not
                  have Iddq issue since pulls are present. */
        }
        return;
}

i32 cc_io_park_safe(struct soc_io_park *io_park_choice,
                     u8 num_pins)
{
        i32 loopcnt;

        if(NULL == io_park_choice) {
                return -1;
        }

        /* Park the IOs safely as specified by the application */
        for(loopcnt = 0; loopcnt < num_pins; loopcnt++) {
                switch(io_park_choice[loopcnt].pin_num) {
                        /* Shared SPI pins for SFLASH */
                        case PIN_11:
                        case PIN_12:
                        case PIN_13:
                        case PIN_14:
#ifdef DEBUG_MODE
                        /* JTAG pins */
                        case PIN_16:
                        case PIN_17:
                        case PIN_19:
                        case PIN_20:
#endif
                                /* Do not park these pins as they may
                                   have external dependencies */
                                break;
                        default:
                                /* Apply the specified IO parking scheme */
                                apply_io_park(io_park_choice[loopcnt].pin_num,
                                        io_park_choice[loopcnt].park_val);

                }

        }

        /* parking the SFLASH IOs */
        HWREG(0x4402E0E8) &= ~(0x3 << 8);
        HWREG(0x4402E0E8) |= (0x2 << 8);
        HWREG(0x4402E0EC) &= ~(0x3 << 8);
        HWREG(0x4402E0EC) |= (0x2 << 8);
        HWREG(0x4402E0F0) &= ~(0x3 << 8);
        HWREG(0x4402E0F0) |= (0x2 << 8);
        HWREG(0x4402E0F4) &= ~(0x3 << 8);
        HWREG(0x4402E0F4) |= (0x1 << 8);

        return 0;
}



#define INSTR_READ_STATUS       0x05
#define INSTR_DEEP_POWER_DOWN   0xB9
#define STATUS_BUSY             0x01
//****************************************************************************
//
//! Put SPI flash into Deep Power Down mode
//!
//! Note:SPI flash is a shared resource between MCU and Network processing
//!      units. This routine should only be exercised after all the network
//!      processing has been stopped. To stop network processing use sl_stop API
//! \param None
//!
//! \return Status, 0:Pass, -1:Fail
//
//****************************************************************************
void SpiFlashDeepPowerDown(void) {
    uint32_t status;

    // Enable clock for SSPI module
    MAP_PRCMPeripheralClkEnable(PRCM_SSPI, PRCM_RUN_MODE_CLK);
    // Reset SSPI at PRCM level and wait for reset to complete
    MAP_PRCMPeripheralReset(PRCM_SSPI);
    while(!MAP_PRCMPeripheralStatusGet(PRCM_SSPI);

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
        MAP_SPIDataPut(SSPI_BASE, INSTR_READ_STATUS);
        MAP_SPIDataGet(SSPI_BASE, &status);

        // Write a dummy byte then read back the actual status.
        MAP_SPIDataPut(SSPI_BASE, 0xFF);
        MAP_SPIDataGet(SSPI_BASE, &status);
    } while ((status & 0xFF) == STATUS_BUSY);

    // Disable chip select for the spi flash.
    MAP_SPICSDisable(SSPI_BASE);
    // Start another CS enable sequence for Power down command.
    MAP_SPICSEnable(SSPI_BASE);
    // Send Deep Power Down command to spi flash
    MAP_SPIDataPut(SSPI_BASE, INSTR_DEEP_POWER_DOWN);
    // Disable chip select for the spi flash.
    MAP_SPICSDisable(SSPI_BASE);
}


#define DBG_PRINT               Report
#define NUM_NVIC_PEND_REG       6
#define ERR_TIMER_TO_WAKE       (-2)
#define MAX_GPIO_WAKESOURCE     6

struct {
        u32 vector_table;   // Vector Table Offset
        u32 aux_ctrl;       // Auxiliary control register
        u32 int_ctrl_state; // Interrupt Control and State
        u32 app_int;        // Application Interrupt Reset control
        u32 sys_ctrl;       // System control
        u32 config_ctrl;    // Configuration control
        u32 sys_pri_1;      // System Handler Priority 1
        u32 sys_pri_2;      // System Handler Priority 2
        u32 sys_pri_3;      // System Handler Priority 3
        u32 sys_hcrs;       // System Handler control and state register
        u32 systick_ctrl;   // SysTick Control Status
        u32 systick_reload; // SysTick Reload
        u32 systick_calib;  // SysTick Calibration
        u32 int_en[6];      // Interrupt set enable
        u32 int_priority[49]; // Interrupt priority
} nvic_reg_store;

u8 gpio_wake_src[] = {2, 4, 13, 17, 11, 24};
u8 gpio_lpds_inttype[] = {1, 1, 2, 0xFF, 3, 0xFF, 0};
u8 gpio_hib_inttype[] = {2, 2, 0, 0xFF, 3, 0xFF, 1};

u32 nvic_int_mask[] = {NVIC_PEND0_MASK, NVIC_PEND1_MASK, NVIC_PEND2_MASK,
                                NVIC_PEND3_MASK, NVIC_PEND4_MASK, NVIC_PEND5_MASK};

volatile i32 debug = 0;


/* Network (Host IRQ) based wakeup from S3(LPDS) */
static i32 setup_S3_wakeup_from_nw()
{
#define IS_NWPIC_INTR_SET() (HWREG(NVIC_EN5) & (1 << ((INT_NWPIC - 16) & 31)))

        /* Check if the NWP->APPs interrupt is enabled */
        if(IS_NWPIC_INTR_SET()) {
                /* Set LPDS Wakeup source as NWP request */
                MAP_PRCMLPDSWakeupSourceEnable(PRCM_LPDS_HOST_IRQ);
                return 0;
        } else {
                return -1;
        }
}



/* GPIO based wakeup from S3(LPDS) */
static i32 check_n_setup_S3_wakeup_from_gpio()
{
        i32 retval, indx;
        u8 gpio_num[MAX_GPIO_WAKESOURCE];
        u8 int_type[MAX_GPIO_WAKESOURCE];

        /* Check for any special purpose GPIO usage */
        retval = cc_gpio_get_spl_purpose(&gpio_num[0],
                                &int_type[0],
                                MAX_GPIO_WAKESOURCE);

        if(retval > 0) {
                for(indx = 0; indx < sizeof(gpio_wake_src); indx++) {
                        if(gpio_wake_src[indx] == gpio_num[0]) {
                                /* Setup the GPIO to be the wake source */
                                MAP_PRCMLPDSWakeUpGPIOSelect(
                                      indx, gpio_lpds_inttype[int_type[0]]);
                                MAP_PRCMLPDSWakeupSourceEnable(PRCM_LPDS_GPIO);
                                /* Save the GPIO number wake from LPDS */
                                cc_pm_ctrl.spl_gpio_wakefrom_lpds = gpio_num[0];
                                break;
                        }
                }
        } else {
                return -1;
        }

        return 0;
}

/* Timer based wakeup from S3 (LPDS) */
static i32 check_n_setup_S3_wakeup_from_timer()
{
        u64 scc_match, scc_curr, scc_remaining;

        /* Check if there is an alarm set */
        if(cc_rtc_has_alarm()) {
                /* Get the time remaining for the RTC timer to expire */
                scc_match = MAP_PRCMSlowClkCtrMatchGet();
                scc_curr = MAP_PRCMSlowClkCtrGet();

                if(scc_match > scc_curr) {
                        /* Get the time remaining in terms of slow clocks */
                        scc_remaining = (scc_match - scc_curr);
                        if(scc_remaining > WAKEUP_TIME_LPDS) {
                                /* Subtract the time it takes for wakeup
                                   from S3 (LPDS) */
                                scc_remaining -= WAKEUP_TIME_LPDS;
                                scc_remaining = (scc_remaining > 0xFFFFFFFF)?
                                        0xFFFFFFFF: scc_remaining;
                                /* Setup the LPDS wake time */
                                MAP_PRCMLPDSIntervalSet(
                                        (u32)scc_remaining);
                                /* Enable the wake source to be timer */
                                MAP_PRCMLPDSWakeupSourceEnable(
                                        PRCM_LPDS_TIMER);
                        } else {
                                /* Cannot enter LPDS */
                                return ERR_TIMER_TO_WAKE;
                        }
                } else {
                       return ERR_TIMER_TO_WAKE;
                }
        } else {
                /* Disable timer as the wake source  */
                MAP_PRCMLPDSWakeupSourceDisable(PRCM_LPDS_TIMER);
                return -1;
        }

        return 0;
}

/* Setup the HIBernate wakr source as apecified GPIO */
static void setup_hib_gpio_wake(u32 gpio_num,
                           u32 gpio_wake_type)
{
        MAP_PRCMHibernateWakeUpGPIOSelect(gpio_num, gpio_wake_type);
        MAP_PRCMHibernateWakeupSourceEnable(gpio_num);

        return;
}

/* GPIO based wakeup from S4 (HIB) */
static i32 check_n_setup_S4_wakeup_from_gpio()
{
        i32 retval, indx;
        u8 gpio_num[MAX_GPIO_WAKESOURCE];
        u8 int_type[MAX_GPIO_WAKESOURCE];

        /* Check for any special purpose GPIO usage */
        retval = cc_gpio_get_spl_purpose(&gpio_num[0],
                                        &int_type[0],
                                        MAX_GPIO_WAKESOURCE);

        if(retval > 0) {
                for(indx = 0; indx < retval; indx++) {
                        switch(gpio_num[indx]) {
                                case 2:
                                        setup_hib_gpio_wake(PRCM_HIB_GPIO2,
                                                gpio_hib_inttype[int_type[indx]]);
                                        break;
                                case 4:
                                        setup_hib_gpio_wake(PRCM_HIB_GPIO4,
                                                gpio_hib_inttype[int_type[indx]]);
                                        break;
                                case 13:
                                        setup_hib_gpio_wake(PRCM_HIB_GPIO13,
                                                gpio_hib_inttype[int_type[indx]]);
                                        break;
                                case 17:
                                        setup_hib_gpio_wake(PRCM_HIB_GPIO17,
                                                gpio_hib_inttype[int_type[indx]]);

                                        break;
                                case 11:
                                        setup_hib_gpio_wake(PRCM_HIB_GPIO11,
                                                gpio_hib_inttype[int_type[indx]]);
                                        break;
                                case 24:
                                        setup_hib_gpio_wake(PRCM_HIB_GPIO24,
                                                gpio_hib_inttype[int_type[indx]]);
                                        break;
                                default:
                                        break;
                        }
                }
        } else {
            return -1;
        }

        return 0;
}

/* Timer based wakeup from S4 (HIB) */
static i32 check_n_setup_S4_wakeup_from_timer()
{
        u64 scc_match, scc_curr, scc_remaining;

        /* Check if there is an alarm set */
        if(cc_rtc_has_alarm()) {
                /* Get the time remaining for the RTC timer to expire */
                scc_match = MAP_PRCMSlowClkCtrMatchGet();
                scc_curr = MAP_PRCMSlowClkCtrGet();

                if(scc_match > scc_curr) {
                        /* Get the time remaining in terms of slow clocks */
                        scc_remaining = (scc_match - scc_curr);
                        if(scc_remaining > WAKEUP_TIME_HIB) {
                                /* Subtract the time it takes for wakeup
                                   from S4 (HIB) */
                                scc_remaining -= WAKEUP_TIME_HIB;
                                /* Setup the HIB wake time */
                                MAP_PRCMHibernateIntervalSet(scc_remaining);
                                /* Enable the wake source to be RTC */
                                MAP_PRCMHibernateWakeupSourceEnable(
                                        PRCM_HIB_SLOW_CLK_CTR);
                        } else {
                                /* Cannot enter HIB */
                                return ERR_TIMER_TO_WAKE;
                        }
                } else {
                        return -1;
                }
        } else {
                /* Disable Timer as wake source */
                MAP_PRCMHibernateWakeupSourceDisable(PRCM_HIB_SLOW_CLK_CTR);
                return -1;
        }

        return 0;
}

/* Sets up wake-up sources for indicated power mode */
i32 cc_set_up_wkup_srcs(enum soc_pm target)
{
        i32 nw_ret = -1, gpio_ret = -1, timer_ret = -1;
        switch(target) {
                case e_pm_S0:
                case e_pm_S1:
                case e_pm_S2:
                        /* These handle the cases of run, sleep, deepsleep.
                           Wake source is configured outside this scope in
                           individual peripherals */
                        break;
                case e_pm_S3:
                        /* Low power deep sleep condition */
                        /* Network (Host IRQ) based wakeup is always enabled */
                        nw_ret = setup_S3_wakeup_from_nw();
                        /* Check and enable GPIO based wakeup */
                        gpio_ret = check_n_setup_S3_wakeup_from_gpio();
                        /* Check and enable LRT based wakeup */
                        timer_ret = check_n_setup_S3_wakeup_from_timer();
                        break;
                case e_pm_S4:
                        /* Hibernate condition */
                        /* Check and enable GPIO based wakeup */
                        gpio_ret = check_n_setup_S4_wakeup_from_gpio();
                        /* Check and enable LRT based wakeup */
                        timer_ret = check_n_setup_S4_wakeup_from_timer();
                        break;
                default:
                        break;
        }

        if(ERR_TIMER_TO_WAKE == timer_ret) {
                return -1;
        }
        if((nw_ret < 0) && (gpio_ret < 0) && (timer_ret < 0)) {
                return -1;
        }
        else if((gpio_ret < 0) && (timer_ret < 0)) {
                /* Setup the LPDS wake time */
                MAP_PRCMLPDSIntervalSet(LPDS_WDOG_TIME);
                /* Enable the wake source to be timer */
                MAP_PRCMLPDSWakeupSourceEnable(
                        PRCM_LPDS_TIMER);
        }
        return 0;
}

/* LPDS wake SW interrupt handler */
void wake_interrupt_handler()
{
        i32 wake_source;

        /* Identify the wakeup source */
        wake_source = MAP_PRCMLPDSWakeupCauseGet();

        switch(wake_source) {
        case PRCM_LPDS_HOST_IRQ:
                        break;
        case PRCM_LPDS_GPIO:
                /* Invoke the callback with the last GPIO num
                   used to enter LPDS (S3) */
                gpio_wake_interrupt_handler(
                        &cc_pm_ctrl.spl_gpio_wakefrom_lpds);
                        break;
        case PRCM_LPDS_TIMER:
                        break;
        }

        return;
}

/* Process events that have woken up system from S3 (LPDS) */
i32 cc_handle_S3_wakeup()
{
        /* Trigger the SW interrupt */
        MAP_IntPendSet(INT_PRCM);
        return 0;
}

/* Are there interrupts pending in system?   TRUE -> yes else no */
bool cc_are_irqs_pending(void)
{
        i32 indx = 0;
        u32 *base_reg_addr;

        /* Check if there are any interrupts pending */
        base_reg_addr = (u32 *)NVIC_PEND0;
        for(indx = 0; indx < NUM_NVIC_PEND_REG; indx++) {
                if(base_reg_addr[indx] & nvic_int_mask[indx]) {
                        return true;
                }
        }

        return false;
}

/* Must push system to low power state of S4 (Hibernate) */
i32 cc_enter_S4(void)
{
        /* Invoke the driverlib API to enter HIBernate */
        MAP_PRCMHibernateEnter();

        return 0;
}

/* Must push system to low power state of S3 (LPDS) */
i32 cc_enter_S3(void(*resume_fn)(void), u32 stack_ptr)
{
        MAP_PRCMLPDSRestoreInfoSet(stack_ptr, (u32)resume_fn);

        /* Enter LPDS */
        MAP_PRCMLPDSEnter();
        return 0;
}

/* Must push system to low power state of S2 (Deepsleep) */
i32 cc_enter_S2(void)
{
        /* Enter deepsleep */
        //MAP_PRCMDeepSleepEnter();

        return 0;
}
volatile i32 sleep_count = 0;
/* Must push system to low power state of S1 */
i32  cc_enter_S1(void)
{
        //MAP_PRCMSleepEnter();
        return 0;
}

/* Save the NVIC registers */
void back_up_nvic_regs()
{
        i32 indx = 0;
        u32 *base_reg_addr;
        /* Save the NVIC control registers */
        nvic_reg_store.vector_table = HWREG(NVIC_VTABLE);
        nvic_reg_store.aux_ctrl = HWREG(NVIC_ACTLR);
        nvic_reg_store.int_ctrl_state = HWREG(NVIC_INT_CTRL);
        nvic_reg_store.app_int = HWREG(NVIC_APINT);
        nvic_reg_store.sys_ctrl = HWREG(NVIC_SYS_CTRL);
        nvic_reg_store.config_ctrl = HWREG(NVIC_CFG_CTRL);
        nvic_reg_store.sys_pri_1 = HWREG(NVIC_SYS_PRI1);
        nvic_reg_store.sys_pri_2 = HWREG(NVIC_SYS_PRI2);
        nvic_reg_store.sys_pri_3 = HWREG(NVIC_SYS_PRI3);
        nvic_reg_store.sys_hcrs = HWREG(NVIC_SYS_HND_CTRL);

        /* Systick registers */
        nvic_reg_store.systick_ctrl = HWREG(NVIC_ST_CTRL);
        nvic_reg_store.systick_reload = HWREG(NVIC_ST_RELOAD);
        nvic_reg_store.systick_calib = HWREG(NVIC_ST_CAL);

        /* Save the interrupt enable registers */
        base_reg_addr = (u32 *)NVIC_EN0;
        for(indx = 0; indx < (sizeof(nvic_reg_store.int_en) / 4); indx++) {
                nvic_reg_store.int_en[indx] = base_reg_addr[indx];
        }

        /* Save the interrupt priority registers */
        base_reg_addr = (u32 *)NVIC_PRI0;
        for(indx = 0; indx < (sizeof(nvic_reg_store.int_priority) / 4); indx++) {
                nvic_reg_store.int_priority[indx] = base_reg_addr[indx];
        }

        return;
}

/* Reestore the NVIC registers */
void restore_nvic_regs()
{
        i32 indx = 0;
        u32 *base_reg_addr;

        /* Restore the NVIC control registers */
        HWREG(NVIC_VTABLE) = nvic_reg_store.vector_table;
        HWREG(NVIC_ACTLR) = nvic_reg_store.aux_ctrl;
        HWREG(NVIC_APINT) = nvic_reg_store.app_int;
        HWREG(NVIC_SYS_CTRL) = nvic_reg_store.sys_ctrl;
        HWREG(NVIC_CFG_CTRL) = nvic_reg_store.config_ctrl;
        HWREG(NVIC_SYS_PRI1) = nvic_reg_store.sys_pri_1;
        HWREG(NVIC_SYS_PRI2) = nvic_reg_store.sys_pri_2;
        HWREG(NVIC_SYS_PRI3) = nvic_reg_store.sys_pri_3;
        HWREG(NVIC_SYS_HND_CTRL) = nvic_reg_store.sys_hcrs;

        /* Systick registers */
        HWREG(NVIC_ST_CTRL) = nvic_reg_store.systick_ctrl;
        HWREG(NVIC_ST_RELOAD) = nvic_reg_store.systick_reload;
        HWREG(NVIC_ST_CAL) = nvic_reg_store.systick_calib;

        /* Restore the interrupt priority registers */
        base_reg_addr = (u32 *)NVIC_PRI0;
        for(indx = 0; indx < (sizeof(nvic_reg_store.int_priority) / 4); indx++) {
                base_reg_addr[indx] = nvic_reg_store.int_priority[indx];
        }

        /* Restore the interrupt enable registers */
        base_reg_addr = (u32 *)NVIC_EN0;
        for(indx = 0; indx < (sizeof(nvic_reg_store.int_en) / 4); indx++) {
                base_reg_addr[indx] = nvic_reg_store.int_en[indx];
        }

        INTRODUCE_SYNC_BARRIER(); /* Data and instruction sync barriers */

        return;
}

/* S3 (LPDS): Back-up system regs & data */
void cc_back_up_soc_data(void) {
        /* Enable the RAM retention */
        MAP_PRCMSRAMRetentionEnable(PRCM_SRAM_COL_1 | PRCM_SRAM_COL_2 | PRCM_SRAM_COL_3 | PRCM_SRAM_COL_4, PRCM_SRAM_LPDS_RET);
        /* Store the NVIC registers */
        back_up_nvic_regs();

        // Park all IO pins

        // Park antenna selection pins
        HWREG(0x4402E108) = 0x00000E61;
        HWREG(0x4402E10C) = 0x00000E61;

        INTRODUCE_SYNC_BARRIER(); /* Data and instruction sync barriers */

        BACK_UP_ARM_REGISTERS(); /* Core registers and code is in assembly */

        return;
}

/* S3 (LPDS): Restore system regs & data */
void cc_restore_soc_data(void)
{
        uint32_t reg;
        /* Check if any of the registers/data need to be restored */
        /* Take I2C semaphore */
        reg = HWREG(COMMON_REG_BASE + COMMON_REG_O_I2C_Properties_Register);
        reg = (reg & ~0x3) | 0x1;
        HWREG(COMMON_REG_BASE + COMMON_REG_O_I2C_Properties_Register) = reg;

        /* Take GPIO semaphore */
        reg = HWREG(COMMON_REG_BASE + COMMON_REG_O_GPIO_properties_register);
        reg = (reg & ~0x3FF) | 0x155;
        HWREG(COMMON_REG_BASE + COMMON_REG_O_GPIO_properties_register) = reg;

        /* Restore the NVIC registers */
        restore_nvic_regs();

        /* ungates the clk for the shared SPI*/
        MAP_PRCMPeripheralClkEnable(PRCM_SSPI, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);
        MAP_PRCMIntEnable (PRCM_INT_SLOW_CLK_CTR);

        return;
}


void prcm_interrupt_handler(void *intr_param)
{
        int status;

        /* Read the interrupt status, also clears the status */
        status = MAP_PRCMIntStatus();

        if((PRCM_INT_SLOW_CLK_CTR == status) || (sw_simulate_rtc)) {
                sw_simulate_rtc = 0;
                /* Invoke the RTC interrupt handler */
                cc_rtc_isr();
        } else if(0 == status) {
                /* Invoke the wake from LPDS interrupt handler */
                wake_interrupt_handler();
        } else {
        }
}

/* LPDS wake SW interrupt handler */
void wake_interrupt_handler()
{
        i32 wake_source;

        /* Identify the wakeup source */
        wake_source = MAP_PRCMLPDSWakeupCauseGet();

        switch(wake_source) {
        case PRCM_LPDS_HOST_IRQ:
                        break;
        case PRCM_LPDS_GPIO:
                /* Invoke the callback with the last GPIO num
                   used to enter LPDS (S3) */
                gpio_wake_interrupt_handler(
                        &cc_pm_ctrl.spl_gpio_wakefrom_lpds);
                        break;
        case PRCM_LPDS_TIMER:
                        break;
        }

        return;
}

/* Invoked in interrupt context */
void cc_rtc_isr(void) {
        struct u64_time alarm, value;
        u32 status;

        /* Read the interrupt status, also clears the status */
        status = MAP_PRCMIntStatus();

        // call the python RTC callback interrupt handler
}
#endif


typedef struct {
    mp_obj_t     obj;
    WakeUpCB_t   wakeup;
}pybsleep_obj_t;


STATIC pybsleep_obj_t * pybsleep_find (mp_obj_t obj) {
    for (mp_uint_t i = 0; i < MP_STATE_PORT(pybsleep_obj_list).len; i++) {
        // search for the object and then remove it
        pybsleep_obj_t *sleep_obj = ((pybsleep_obj_t *)MP_STATE_PORT(pybsleep_obj_list).items[i]);
        if (sleep_obj->obj == obj) {
            return sleep_obj;
        }
    }
    return NULL;
}

void pyblsleep_init0 (void) {
    mp_obj_list_init(&MP_STATE_PORT(pybsleep_obj_list), 0);
}

void pybsleep_add (mp_obj_t obj, WakeUpCB_t wakeup) {
    pybsleep_obj_t * sleep_obj = m_new_obj(pybsleep_obj_t);
    sleep_obj->obj = obj;
    sleep_obj->wakeup = wakeup;
    // only add objects once
    if (!pybsleep_find(sleep_obj)) {
        mp_obj_list_append(&MP_STATE_PORT(pybsleep_obj_list), sleep_obj);
    }
}

void pybsleep_remove (mp_obj_t obj) {
    pybsleep_obj_t *sleep_obj;
    if ((sleep_obj = pybsleep_find(obj))) {
        mp_obj_list_remove(&MP_STATE_PORT(pybsleep_obj_list), sleep_obj);
    }
}

void pybsleep_wakeup (void) {
    for (mp_uint_t i = 0; i < MP_STATE_PORT(pybsleep_obj_list).len; i++) {
        pybsleep_obj_t *sleep_obj = ((pybsleep_obj_t *)MP_STATE_PORT(pybsleep_obj_list).items[i]);
        sleep_obj->wakeup(sleep_obj->obj);
    }
}

