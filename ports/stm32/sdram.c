/*
 * This file is part of the OpenMV project.
 * Copyright (c) 2013/2014 Ibrahim Abdelkader <i.abdalkader@gmail.com>
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * SDRAM Driver.
 *
 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "py/runtime.h"
#include "py/mphal.h"
#include "boardctrl.h"
#include "pin.h"
#include "pin_static_af.h"
#include "mpu.h"
#include "systick.h"
#include "sdram.h"

#define SDRAM_TIMEOUT                            ((uint32_t)0xFFFF)
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

#if defined(MICROPY_HW_FMC_SDCKE0) && defined(MICROPY_HW_FMC_SDNE0)
#define FMC_SDRAM_BANK FMC_SDRAM_BANK1
#define FMC_SDRAM_CMD_TARGET_BANK FMC_SDRAM_CMD_TARGET_BANK1
#if MICROPY_HW_FMC_SWAP_BANKS
#define SDRAM_START_ADDRESS 0x60000000
#else
#define SDRAM_START_ADDRESS 0xC0000000
#endif
#elif defined(MICROPY_HW_FMC_SDCKE1) && defined(MICROPY_HW_FMC_SDNE1)
#define FMC_SDRAM_BANK FMC_SDRAM_BANK2
#define FMC_SDRAM_CMD_TARGET_BANK FMC_SDRAM_CMD_TARGET_BANK2
#if MICROPY_HW_FMC_SWAP_BANKS
#define SDRAM_START_ADDRESS 0x70000000
#else
#define SDRAM_START_ADDRESS 0xD0000000
#endif
#endif

// Provides the MPU_REGION_SIZE_X value when passed the size of region in bytes
// "m" must be a power of 2 between 32 and 4G (2**5 and 2**32) and this formula
// computes the log2 of "m", minus 1
#define MPU_REGION_SIZE(m) (((m) - 1) / (((m) - 1) % 255 + 1) / 255 % 255 * 8 + 7 - 86 / (((m) - 1) % 255 + 12) - 1)

#define SDRAM_MPU_REGION_SIZE (MPU_REGION_SIZE(MICROPY_HW_SDRAM_SIZE))

#ifdef FMC_SDRAM_BANK

static void sdram_init_seq(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_CommandTypeDef *command);

bool sdram_init(void) {
    SDRAM_HandleTypeDef hsdram;
    FMC_SDRAM_TimingTypeDef SDRAM_Timing;
    FMC_SDRAM_CommandTypeDef command;

    __HAL_RCC_FMC_CLK_ENABLE();

    #if MICROPY_HW_FMC_SWAP_BANKS
    HAL_SetFMCMemorySwappingConfig(FMC_SWAPBMAP_SDRAM_SRAM);
    #endif

    #if defined(MICROPY_HW_FMC_SDCKE0)
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_SDCKE0, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_SDCKE0);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_SDNE0, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_SDNE0);

    #elif defined(MICROPY_HW_FMC_SDCKE1)
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_SDCKE1, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_SDCKE1);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_SDNE1, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_SDNE1);
    #endif

    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_SDCLK, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_SDCLK);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_SDNCAS, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_SDNCAS);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_SDNRAS, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_SDNRAS);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_SDNWE, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_SDNWE);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_BA0, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_BA0);
    #ifdef MICROPY_HW_FMC_BA1
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_BA1, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_BA1);
    #endif
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_NBL0, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_NBL0);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_NBL1, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_NBL1);
    #ifdef MICROPY_HW_FMC_NBL2
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_NBL2, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_NBL2);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_NBL3, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_NBL3);
    #endif
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_A0, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_A0);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_A1, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_A1);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_A2, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_A2);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_A3, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_A3);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_A4, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_A4);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_A5, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_A5);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_A6, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_A6);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_A7, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_A7);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_A8, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_A8);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_A9, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_A9);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_A10, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_A10);
    #ifdef MICROPY_HW_FMC_A11
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_A11, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_A11);
    #endif
    #ifdef MICROPY_HW_FMC_A12
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_A12, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_A12);
    #endif
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D0, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D0);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D1, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D1);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D2, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D2);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D3, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D3);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D4, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D4);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D5, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D5);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D6, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D6);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D7, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D7);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D8, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D8);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D9, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D9);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D10, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D10);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D11, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D11);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D12, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D12);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D13, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D13);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D14, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D14);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D15, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D15);
    #ifdef MICROPY_HW_FMC_D16
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D16, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D16);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D17, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D17);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D18, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D18);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D19, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D19);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D20, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D20);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D21, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D21);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D22, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D22);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D23, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D23);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D24, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D24);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D25, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D25);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D26, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D26);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D27, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D27);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D28, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D28);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D29, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D29);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D30, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D30);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_FMC_D31, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FMC_D31);
    #endif

    /* SDRAM device configuration */
    hsdram.Instance = FMC_SDRAM_DEVICE;
    /* Timing configuration for 90 Mhz of SD clock frequency (180Mhz/2) */
    /* TMRD: 2 Clock cycles */
    SDRAM_Timing.LoadToActiveDelay = MICROPY_HW_SDRAM_TIMING_TMRD;
    /* TXSR: min=70ns (6x11.90ns) */
    SDRAM_Timing.ExitSelfRefreshDelay = MICROPY_HW_SDRAM_TIMING_TXSR;
    /* TRAS */
    SDRAM_Timing.SelfRefreshTime = MICROPY_HW_SDRAM_TIMING_TRAS;
    /* TRC */
    SDRAM_Timing.RowCycleDelay = MICROPY_HW_SDRAM_TIMING_TRC;
    /* TWR */
    SDRAM_Timing.WriteRecoveryTime = MICROPY_HW_SDRAM_TIMING_TWR;
    /* TRP */
    SDRAM_Timing.RPDelay = MICROPY_HW_SDRAM_TIMING_TRP;
    /* TRCD */
    SDRAM_Timing.RCDDelay = MICROPY_HW_SDRAM_TIMING_TRCD;

    #define _FMC_INIT(x, n) x##_##n
    #define FMC_INIT(x, n) _FMC_INIT(x,  n)

    hsdram.Init.SDBank = FMC_SDRAM_BANK;
    hsdram.Init.ColumnBitsNumber = FMC_INIT(FMC_SDRAM_COLUMN_BITS_NUM, MICROPY_HW_SDRAM_COLUMN_BITS_NUM);
    hsdram.Init.RowBitsNumber = FMC_INIT(FMC_SDRAM_ROW_BITS_NUM, MICROPY_HW_SDRAM_ROW_BITS_NUM);
    hsdram.Init.MemoryDataWidth = FMC_INIT(FMC_SDRAM_MEM_BUS_WIDTH, MICROPY_HW_SDRAM_MEM_BUS_WIDTH);
    hsdram.Init.InternalBankNumber = FMC_INIT(FMC_SDRAM_INTERN_BANKS_NUM, MICROPY_HW_SDRAM_INTERN_BANKS_NUM);
    hsdram.Init.CASLatency = FMC_INIT(FMC_SDRAM_CAS_LATENCY, MICROPY_HW_SDRAM_CAS_LATENCY);
    hsdram.Init.SDClockPeriod = FMC_INIT(FMC_SDRAM_CLOCK_PERIOD, MICROPY_HW_SDRAM_CLOCK_PERIOD);
    hsdram.Init.ReadPipeDelay = FMC_INIT(FMC_SDRAM_RPIPE_DELAY, MICROPY_HW_SDRAM_RPIPE_DELAY);
    hsdram.Init.ReadBurst = (MICROPY_HW_SDRAM_RBURST) ? FMC_SDRAM_RBURST_ENABLE : FMC_SDRAM_RBURST_DISABLE;
    hsdram.Init.WriteProtection = (MICROPY_HW_SDRAM_WRITE_PROTECTION) ? FMC_SDRAM_WRITE_PROTECTION_ENABLE : FMC_SDRAM_WRITE_PROTECTION_DISABLE;

    /* Initialize the SDRAM controller */
    if (HAL_SDRAM_Init(&hsdram, &SDRAM_Timing) != HAL_OK) {
        return false;
    }

    sdram_init_seq(&hsdram, &command);
    return true;
}

void *sdram_start(void) {
    return (void *)SDRAM_START_ADDRESS;
}

void *sdram_end(void) {
    return (void *)(SDRAM_START_ADDRESS + MICROPY_HW_SDRAM_SIZE);
}

static void sdram_init_seq(SDRAM_HandleTypeDef
    *hsdram, FMC_SDRAM_CommandTypeDef *command) {
    /* Program the SDRAM external device */
    __IO uint32_t tmpmrd = 0;

    /* Step 3:  Configure a clock configuration enable command */
    command->CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
    command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK;
    command->AutoRefreshNumber = 1;
    command->ModeRegisterDefinition = 0;

    /* Send the command */
    HAL_SDRAM_SendCommand(hsdram, command, 0x1000);

    /* Step 4: Insert 100 ms delay */
    HAL_Delay(100);

    /* Step 5: Configure a PALL (precharge all) command */
    command->CommandMode = FMC_SDRAM_CMD_PALL;
    command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK;
    command->AutoRefreshNumber = 1;
    command->ModeRegisterDefinition = 0;

    /* Send the command */
    HAL_SDRAM_SendCommand(hsdram, command, 0x1000);

    /* Step 6 : Configure a Auto-Refresh command */
    command->CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
    command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK;
    command->AutoRefreshNumber = MICROPY_HW_SDRAM_AUTOREFRESH_NUM;
    command->ModeRegisterDefinition = 0;

    /* Send the command */
    HAL_SDRAM_SendCommand(hsdram, command, 0x1000);

    /* Step 7: Program the external memory mode register */
    tmpmrd = (uint32_t)FMC_INIT(SDRAM_MODEREG_BURST_LENGTH, MICROPY_HW_SDRAM_BURST_LENGTH) |
        SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL |
        FMC_INIT(SDRAM_MODEREG_CAS_LATENCY, MICROPY_HW_SDRAM_CAS_LATENCY) |
        SDRAM_MODEREG_OPERATING_MODE_STANDARD |
        SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

    command->CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
    command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK;
    command->AutoRefreshNumber = 1;
    command->ModeRegisterDefinition = tmpmrd;

    /* Send the command */
    HAL_SDRAM_SendCommand(hsdram, command, 0x1000);

    /* Step 8: Set the refresh rate counter
       RefreshRate = 64 ms / 8192 cyc = 7.8125 us/cyc

       RefreshCycles = 7.8125 us * 90 MHz = 703
       According to the formula on p.1665 of the reference manual,
       we also need to subtract 20 from the value, so the target
       refresh rate is 703 - 20 = 683.
     */
    #define REFRESH_COUNT (MICROPY_HW_SDRAM_REFRESH_RATE * 90000 / 8192 - 20)
    HAL_SDRAM_ProgramRefreshRate(hsdram, REFRESH_COUNT);

    #if defined(STM32F7) || defined(STM32H7)
    /* Enable MPU for the SDRAM Memory Region to allow non-aligned
       accesses (hard-fault otherwise)
       Initially disable all access for the entire SDRAM memory space,
       then enable access/caching for the size used
    */
    uint32_t irq_state = mpu_config_start();
    mpu_config_region(MPU_REGION_SDRAM1, SDRAM_START_ADDRESS, MPU_CONFIG_NOACCESS(0x00, MPU_REGION_SIZE_512MB));
    mpu_config_region(MPU_REGION_SDRAM2, SDRAM_START_ADDRESS, MPU_CONFIG_SDRAM(SDRAM_MPU_REGION_SIZE));
    mpu_config_end(irq_state);
    #endif
}

void sdram_enter_low_power(void) {
    // Enter self-refresh mode.
    // In self-refresh mode the SDRAM retains data without external clocking.
    FMC_SDRAM_DEVICE->SDCMR |= (FMC_SDRAM_CMD_SELFREFRESH_MODE |     // Command Mode
        FMC_SDRAM_CMD_TARGET_BANK |                                  // Command Target
        (0 << 5U) |                                                  // Auto Refresh Number -1
        (0 << 9U));                                                  // Mode Register Definition
}

void sdram_leave_low_power(void) {
    // Exit self-refresh mode.
    // Self-refresh mode is exited when the device is accessed or the mode bits are
    // set to Normal mode, so technically it's not necessary to call this functions.
    FMC_SDRAM_DEVICE->SDCMR |= (FMC_SDRAM_CMD_NORMAL_MODE |          // Command Mode
        FMC_SDRAM_CMD_TARGET_BANK |                                  // Command Target
        (0 << 5U) |                                                  // Auto Refresh Number - 1
        (0 << 9U));                                                  // Mode Register Definition
}

void sdram_enter_power_down(void) {
    // Enter power-down mode.
    FMC_SDRAM_DEVICE->SDCMR |= (FMC_SDRAM_CMD_POWERDOWN_MODE |       // Command Mode
        FMC_SDRAM_CMD_TARGET_BANK |                                  // Command Target
        (0 << 5U) |                                                  // Auto Refresh Number -1
        (0 << 9U));                                                  // Mode Register Definition
}

#if __GNUC__ >= 11
// Prevent array bounds warnings when accessing SDRAM_START_ADDRESS as a memory pointer.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif

bool __attribute__((optimize("Os"))) sdram_test(bool exhaustive) {
    uint8_t const pattern = 0xaa;
    uint8_t const antipattern = 0x55;
    volatile uint8_t *const mem_base = (uint8_t *)sdram_start();

    #if MICROPY_HW_SDRAM_TEST_FAIL_ON_ERROR
    char error_buffer[1024];
    #endif

    #if (__DCACHE_PRESENT == 1)
    bool i_cache_disabled = false;
    bool d_cache_disabled = false;

    // Disable caches for testing.
    if (SCB->CCR & (uint32_t)SCB_CCR_IC_Msk) {
        SCB_DisableICache();
        i_cache_disabled = true;
    }

    if (SCB->CCR & (uint32_t)SCB_CCR_DC_Msk) {
        SCB_DisableDCache();
        d_cache_disabled = true;
    }
    #endif

    // Test data bus
    for (uint32_t i = 0; i < MICROPY_HW_SDRAM_MEM_BUS_WIDTH; i++) {
        *((volatile uint32_t *)mem_base) = (1 << i);
        __DSB();
        if (*((volatile uint32_t *)mem_base) != (1 << i)) {
            #if MICROPY_HW_SDRAM_TEST_FAIL_ON_ERROR
            snprintf(error_buffer, sizeof(error_buffer),
                "Data bus test failed at 0x%p expected 0x%x found 0x%lx",
                &mem_base[0], (1 << i), ((volatile uint32_t *)mem_base)[0]);
            MICROPY_BOARD_FATAL_ERROR(error_buffer);
            #endif
            return false;
        }
    }

    // Test address bus
    for (uint32_t i = 1; i < MICROPY_HW_SDRAM_SIZE; i <<= 1) {
        mem_base[i] = pattern;
        __DSB();
        if (mem_base[i] != pattern) {
            #if MICROPY_HW_SDRAM_TEST_FAIL_ON_ERROR
            snprintf(error_buffer, sizeof(error_buffer),
                "Address bus test failed at 0x%p expected 0x%x found 0x%x",
                &mem_base[i], pattern, mem_base[i]);
            MICROPY_BOARD_FATAL_ERROR(error_buffer);
            #endif
            return false;
        }
    }

    // Check for aliasing (overlapping addresses)
    mem_base[0] = antipattern;
    __DSB();
    for (uint32_t i = 1; i < MICROPY_HW_SDRAM_SIZE; i <<= 1) {
        if (mem_base[i] != pattern) {
            #if MICROPY_HW_SDRAM_TEST_FAIL_ON_ERROR
            snprintf(error_buffer, sizeof(error_buffer),
                "Address bus overlap at 0x%p expected 0x%x found 0x%x",
                &mem_base[i], pattern, mem_base[i]);
            MICROPY_BOARD_FATAL_ERROR(error_buffer);
            #endif
            return false;
        }
    }

    // Test all RAM cells
    if (exhaustive) {
        // Write all memory first then compare, so even if the cache
        // is enabled, it's not just writing and reading from cache.
        // Note: This test should also detect refresh rate issues.
        for (uint32_t i = 0; i < MICROPY_HW_SDRAM_SIZE; i++) {
            mem_base[i] = ((i % 2) ? pattern : antipattern);
        }

        for (uint32_t i = 0; i < MICROPY_HW_SDRAM_SIZE; i++) {
            if (mem_base[i] != ((i % 2) ? pattern : antipattern)) {
                #if MICROPY_HW_SDRAM_TEST_FAIL_ON_ERROR
                snprintf(error_buffer, sizeof(error_buffer),
                    "Address bus slow test failed at 0x%p expected 0x%x found 0x%x",
                    &mem_base[i], ((i % 2) ? pattern : antipattern), mem_base[i]);
                MICROPY_BOARD_FATAL_ERROR(error_buffer);
                #endif
                return false;
            }
        }
    }

    #if (__DCACHE_PRESENT == 1)
    // Re-enable caches if they were enabled before the test started.
    if (i_cache_disabled) {
        SCB_EnableICache();
    }

    if (d_cache_disabled) {
        SCB_EnableDCache();
    }
    #endif

    return true;
}

#if __GNUC__ >= 11
#pragma GCC diagnostic pop
#endif

#endif // FMC_SDRAM_BANK
