/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Damien P. George
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

#include <string.h>

#include "py/obj.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "qspi.h"
#include "pin_static_af.h"

#if defined(MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2)

QSPI_HandleTypeDef QSPIHandle;

#define QSPI_OK             0
#define QSPI_ERROR         -1
#define QSPI_NOT_SUPPORTED -2

/* Status Register */
#define SR_WIP                      ((uint8_t)0x01)    /*!< Write in progress */
#define SR_WREN                     ((uint8_t)0x02)    /*!< Write enable latch */
#define SR_BLOCKPR                  ((uint8_t)0x5C)    /*!< Block protected against program and erase operations */
#define SR_PRBOTTOM                 ((uint8_t)0x20)    /*!< Protected memory area defined by BLOCKPR starts from top or bottom */
#define SR_SRWREN                   ((uint8_t)0x80)    /*!< Status register write enable/disable */

#define DUMMY_CYCLES_READ_QUAD      10
#define VCR_NB_DUMMY                ((uint8_t)0xF0)    /*!< Number of dummy clock cycles */

/* Reset Operations */
#define RESET_ENABLE_CMD                     0x66
#define RESET_MEMORY_CMD                     0x99

/* Identification Operations */
#define READ_ID_CMD                          0x9E
#define READ_ID_CMD2                         0x9F
#define MULTIPLE_IO_READ_ID_CMD              0xAF
#define READ_SERIAL_FLASH_DISCO_PARAM_CMD    0x5A

/* Read Operations */
#define READ_CMD                             0x03
#define FAST_READ_CMD                        0x0B
#define DUAL_OUT_FAST_READ_CMD               0x3B
#define DUAL_INOUT_FAST_READ_CMD             0xBB
#define QUAD_OUT_FAST_READ_CMD               0x6B
#define QUAD_INOUT_FAST_READ_CMD             0xEB

/* Write Operations */
#define WRITE_ENABLE_CMD                     0x06
#define WRITE_DISABLE_CMD                    0x04

/* Register Operations */
#define READ_STATUS_REG_CMD                  0x05
#define WRITE_STATUS_REG_CMD                 0x01

#define READ_LOCK_REG_CMD                    0xE8
#define WRITE_LOCK_REG_CMD                   0xE5

#define READ_FLAG_STATUS_REG_CMD             0x70
#define CLEAR_FLAG_STATUS_REG_CMD            0x50

#define READ_NONVOL_CFG_REG_CMD              0xB5
#define WRITE_NONVOL_CFG_REG_CMD             0xB1

#define READ_VOL_CFG_REG_CMD                 0x85
#define WRITE_VOL_CFG_REG_CMD                0x81

/**
  * @brief  This function send a Write Enable and wait it is effective.
  * @param  hqspi: QSPI handle
  * @retval None
  */
static uint8_t QSPI_WriteEnable(QSPI_HandleTypeDef *hqspi)
{
    QSPI_CommandTypeDef     s_command;
    QSPI_AutoPollingTypeDef s_config;

    /* Enable write operations */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = WRITE_ENABLE_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_NONE;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }
    
    /* Configure automatic polling mode to wait for write enabling */  
    s_config.Match           = SR_WREN;
    s_config.Mask            = SR_WREN;
    s_config.MatchMode       = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval        = 0x10;
    s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

    s_command.Instruction    = READ_STATUS_REG_CMD;
    s_command.DataMode       = QSPI_DATA_1_LINE;

    if (HAL_QSPI_AutoPolling(hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

  return QSPI_OK;
}


/**
  * @brief  This function read the SR of the memory and wait the EOP.
  * @param  hqspi: QSPI handle
  * @param  Timeout
  * @retval None
  */
static uint8_t QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *hqspi, uint32_t Timeout)
{
    QSPI_CommandTypeDef     s_command;
    QSPI_AutoPollingTypeDef s_config;

    /* Configure automatic polling mode to wait for memory ready */  
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = READ_STATUS_REG_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_1_LINE;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    s_config.Match           = 0;
    s_config.Mask            = SR_WIP;
    s_config.MatchMode       = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval        = 0x10;
    s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

    if (HAL_QSPI_AutoPolling(hqspi, &s_command, &s_config, Timeout) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

/**
  * @brief  This function reset the QSPI memory.
  * @param  hqspi: QSPI handle
  * @retval None
  */
static uint8_t QSPI_ResetMemory(QSPI_HandleTypeDef *hqspi)
{
    QSPI_CommandTypeDef s_command;

    /* Initialize the reset enable command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = RESET_ENABLE_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_NONE;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Send the command */
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    /* Send the reset memory command */
    s_command.Instruction = RESET_MEMORY_CMD;
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    /* Configure automatic polling mode to wait the memory is ready */  
    if (QSPI_AutoPollingMemReady(hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

/**
  * @brief  This function configure the dummy cycles on memory side.
  * @param  hqspi: QSPI handle
  * @retval None
  */
static uint8_t QSPI_DummyCyclesCfg(QSPI_HandleTypeDef *hqspi)
{
    QSPI_CommandTypeDef s_command;
    uint8_t reg;

    /* Initialize the read volatile configuration register command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = READ_VOL_CFG_REG_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_1_LINE;
    s_command.DummyCycles       = 0;
    s_command.NbData            = 1;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Configure the command */
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    /* Reception of the data */
    if (HAL_QSPI_Receive(hqspi, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    /* Enable write operations */
    if (QSPI_WriteEnable(hqspi) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    /* Update volatile configuration register (with new dummy cycles) */  
    s_command.Instruction = WRITE_VOL_CFG_REG_CMD;
    MODIFY_REG(reg, VCR_NB_DUMMY, (DUMMY_CYCLES_READ_QUAD << POSITION_VAL(VCR_NB_DUMMY)));
        
    /* Configure the write volatile configuration register command */
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    /* Transmission of the data */
    if (HAL_QSPI_Transmit(hqspi, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }
    
    return QSPI_OK;
}

int qspi_init(void) {
    // Configure pins
    mp_hal_pin_config_alt_static(MICROPY_HW_QSPIFLASH_CS, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, STATIC_AF_QUADSPI_BK1_NCS);
    mp_hal_pin_config_alt_static(MICROPY_HW_QSPIFLASH_SCK, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, STATIC_AF_QUADSPI_CLK);
    mp_hal_pin_config_alt_static(MICROPY_HW_QSPIFLASH_IO0, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, STATIC_AF_QUADSPI_BK1_IO0);
    mp_hal_pin_config_alt_static(MICROPY_HW_QSPIFLASH_IO1, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, STATIC_AF_QUADSPI_BK1_IO1);
    mp_hal_pin_config_alt_static(MICROPY_HW_QSPIFLASH_IO2, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, STATIC_AF_QUADSPI_BK1_IO2);
    mp_hal_pin_config_alt_static(MICROPY_HW_QSPIFLASH_IO3, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, STATIC_AF_QUADSPI_BK1_IO3);

    // Bring up the QSPI peripheral

    __HAL_RCC_QSPI_CLK_ENABLE();
    __QSPI_FORCE_RESET();
    __QSPI_RELEASE_RESET();

      QSPIHandle.Instance = QUADSPI;

    /* Call the DeInit function to reset the driver */
    if (HAL_QSPI_DeInit(&QSPIHandle) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    // QUADSPI->CR =
    //     2 << QUADSPI_CR_PRESCALER_Pos // F_CLK = F_AHB/3 (72MHz when CPU is 216MHz)
    //     | 3 << QUADSPI_CR_FTHRES_Pos // 4 bytes must be available to read/write
    //     #if defined(QUADSPI_CR_FSEL_Pos)
    //     | 0 << QUADSPI_CR_FSEL_Pos // FLASH 1 selected
    //     #endif
    //     #if defined(QUADSPI_CR_DFM_Pos)
    //     | 0 << QUADSPI_CR_DFM_Pos // dual-flash mode disabled
    //     #endif
    //     | 0 << QUADSPI_CR_SSHIFT_Pos // no sample shift
    //     | 1 << QUADSPI_CR_TCEN_Pos // timeout counter enabled
    //     | 1 << QUADSPI_CR_EN_Pos // enable the peripheral
    //     ;

    // QUADSPI->DCR =
    //     (MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2 - 3 - 1) << QUADSPI_DCR_FSIZE_Pos
    //     | 1 << QUADSPI_DCR_CSHT_Pos // nCS stays high for 2 cycles
    //     | 0 << QUADSPI_DCR_CKMODE_Pos // CLK idles at low state
    //     ;

    #define FLASH_SIZE                  0x2000000 /* 256 MBits => 32MBytes */
    // #define SECTOR_SIZE                 0x10000   /* 256 sectors of 64KBytes */

    /* QSPI initialization */
    QSPIHandle.Init.ClockPrescaler     = 1; /* QSPI freq = 216 MHz/(1+1) = 108 Mhz */
    QSPIHandle.Init.FifoThreshold      = 4;
    QSPIHandle.Init.SampleShifting     = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
    QSPIHandle.Init.FlashSize          = POSITION_VAL(FLASH_SIZE) - 1;
    QSPIHandle.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_2_CYCLE;
    QSPIHandle.Init.ClockMode          = QSPI_CLOCK_MODE_0;
    QSPIHandle.Init.FlashID            = QSPI_FLASH_ID_1;
    QSPIHandle.Init.DualFlash          = QSPI_DUALFLASH_DISABLE;

    if (HAL_QSPI_Init(&QSPIHandle) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    /* QSPI memory reset */
    if (QSPI_ResetMemory(&QSPIHandle) != QSPI_OK)
    {
        return QSPI_NOT_SUPPORTED;
    }
    
    /* Configuration of the dummy cycles on QSPI memory side */
    if (QSPI_DummyCyclesCfg(&QSPIHandle) != QSPI_OK)
    {
        return QSPI_NOT_SUPPORTED;
    }

    return QSPI_OK;
}

int qspi_memory_map(void) {
    // Enable memory-mapped mode

    // QUADSPI->ABR = 0; // disable continuous read mode
    // QUADSPI->LPTR = 100; // to tune
    // QUADSPI->CCR =
    //     0 << QUADSPI_CCR_DDRM_Pos // DDR mode disabled
    //     | 0 << QUADSPI_CCR_SIOO_Pos // send instruction every transaction
    //     | 3 << QUADSPI_CCR_FMODE_Pos // memory-mapped mode
    //     | 3 << QUADSPI_CCR_DMODE_Pos // data on 4 lines
    //     | 4 << QUADSPI_CCR_DCYC_Pos // 4 dummy cycles
    //     | 0 << QUADSPI_CCR_ABSIZE_Pos // 8-bit alternate byte
    //     | 3 << QUADSPI_CCR_ABMODE_Pos // alternate byte on 4 lines
    //     | 2 << QUADSPI_CCR_ADSIZE_Pos // 24-bit address size
    //     | 3 << QUADSPI_CCR_ADMODE_Pos // address on 4 lines
    //     | 1 << QUADSPI_CCR_IMODE_Pos // instruction on 1 line
    //     | 0xeb << QUADSPI_CCR_INSTRUCTION_Pos // quad read opcode
    //     ;

    QSPI_CommandTypeDef      s_command;
    QSPI_MemoryMappedTypeDef s_mem_mapped_cfg;

    /* Configure the command for the read instruction */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = QUAD_INOUT_FAST_READ_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
    s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_4_LINES;
    s_command.DummyCycles       = DUMMY_CYCLES_READ_QUAD;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
    
    /* Configure the memory mapped mode */
    s_mem_mapped_cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_ENABLE;
    s_mem_mapped_cfg.TimeOutPeriod     = 1;
    
    if (HAL_QSPI_MemoryMapped(&QSPIHandle, &s_command, &s_mem_mapped_cfg) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;

}

STATIC int qspi_ioctl(void *self_in, uint32_t cmd) {
    (void)self_in;
    switch (cmd) {
        case MP_QSPI_IOCTL_INIT:
            qspi_init();
            break;
        case MP_QSPI_IOCTL_BUS_RELEASE:
            // Switch to memory-map mode when bus is idle
            qspi_memory_map();
            break;
    }
    return 0; // success
}

STATIC void qspi_write_cmd_data(void *self_in, uint8_t cmd, size_t len, uint32_t data) {
    (void)self_in;

    QUADSPI->FCR = QUADSPI_FCR_CTCF; // clear TC flag

    if (len == 0) {
        QUADSPI->CCR =
            0 << QUADSPI_CCR_DDRM_Pos // DDR mode disabled
            | 0 << QUADSPI_CCR_SIOO_Pos // send instruction every transaction
            | 0 << QUADSPI_CCR_FMODE_Pos // indirect write mode
            | 0 << QUADSPI_CCR_DMODE_Pos // no data
            | 0 << QUADSPI_CCR_DCYC_Pos // 0 dummy cycles
            | 0 << QUADSPI_CCR_ABMODE_Pos // no alternate byte
            | 0 << QUADSPI_CCR_ADMODE_Pos // no address
            | 1 << QUADSPI_CCR_IMODE_Pos // instruction on 1 line
            | cmd << QUADSPI_CCR_INSTRUCTION_Pos // write opcode
            ;
    } else {
        QUADSPI->DLR = len - 1;

        QUADSPI->CCR =
            0 << QUADSPI_CCR_DDRM_Pos // DDR mode disabled
            | 0 << QUADSPI_CCR_SIOO_Pos // send instruction every transaction
            | 0 << QUADSPI_CCR_FMODE_Pos // indirect write mode
            | 1 << QUADSPI_CCR_DMODE_Pos // data on 1 line
            | 0 << QUADSPI_CCR_DCYC_Pos // 0 dummy cycles
            | 0 << QUADSPI_CCR_ABMODE_Pos // no alternate byte
            | 0 << QUADSPI_CCR_ADMODE_Pos // no address
            | 1 << QUADSPI_CCR_IMODE_Pos // instruction on 1 line
            | cmd << QUADSPI_CCR_INSTRUCTION_Pos // write opcode
            ;

        // This assumes len==2
        *(uint16_t*)&QUADSPI->DR = data;
    }

    // Wait for write to finish
    while (!(QUADSPI->SR & QUADSPI_SR_TCF)) {
    }

    QUADSPI->FCR = QUADSPI_FCR_CTCF; // clear TC flag
}

STATIC void qspi_write_cmd_addr_data(void *self_in, uint8_t cmd, uint32_t addr, size_t len, const uint8_t *src) {
    (void)self_in;

    QUADSPI->FCR = QUADSPI_FCR_CTCF; // clear TC flag

    if (len == 0) {
        QUADSPI->CCR =
            0 << QUADSPI_CCR_DDRM_Pos // DDR mode disabled
            | 0 << QUADSPI_CCR_SIOO_Pos // send instruction every transaction
            | 0 << QUADSPI_CCR_FMODE_Pos // indirect write mode
            | 0 << QUADSPI_CCR_DMODE_Pos // no data
            | 0 << QUADSPI_CCR_DCYC_Pos // 0 dummy cycles
            | 0 << QUADSPI_CCR_ABMODE_Pos // no alternate byte
            | 2 << QUADSPI_CCR_ADSIZE_Pos // 24-bit address size
            | 1 << QUADSPI_CCR_ADMODE_Pos // address on 1 line
            | 1 << QUADSPI_CCR_IMODE_Pos // instruction on 1 line
            | cmd << QUADSPI_CCR_INSTRUCTION_Pos // write opcode
            ;

        QUADSPI->AR = addr;
    } else {
        QUADSPI->DLR = len - 1;

        QUADSPI->CCR =
            0 << QUADSPI_CCR_DDRM_Pos // DDR mode disabled
            | 0 << QUADSPI_CCR_SIOO_Pos // send instruction every transaction
            | 0 << QUADSPI_CCR_FMODE_Pos // indirect write mode
            | 1 << QUADSPI_CCR_DMODE_Pos // data on 1 line
            | 0 << QUADSPI_CCR_DCYC_Pos // 0 dummy cycles
            | 0 << QUADSPI_CCR_ABMODE_Pos // no alternate byte
            | 2 << QUADSPI_CCR_ADSIZE_Pos // 24-bit address size
            | 1 << QUADSPI_CCR_ADMODE_Pos // address on 1 line
            | 1 << QUADSPI_CCR_IMODE_Pos // instruction on 1 line
            | cmd << QUADSPI_CCR_INSTRUCTION_Pos // write opcode
            ;

        QUADSPI->AR = addr;

        // Write out the data 1 byte at a time
        while (len) {
            while (!(QUADSPI->SR & QUADSPI_SR_FTF)) {
            }
            *(volatile uint8_t*)&QUADSPI->DR = *src++;
            --len;
        }
    }

    // Wait for write to finish
    while (!(QUADSPI->SR & QUADSPI_SR_TCF)) {
    }

    QUADSPI->FCR = QUADSPI_FCR_CTCF; // clear TC flag
}

STATIC uint32_t qspi_read_cmd(void *self_in, uint8_t cmd, size_t len) {
    (void)self_in;

    QUADSPI->FCR = QUADSPI_FCR_CTCF; // clear TC flag

    QUADSPI->DLR = len - 1; // number of bytes to read

    QUADSPI->CCR =
        0 << QUADSPI_CCR_DDRM_Pos // DDR mode disabled
        | 0 << QUADSPI_CCR_SIOO_Pos // send instruction every transaction
        | 1 << QUADSPI_CCR_FMODE_Pos // indirect read mode
        | 1 << QUADSPI_CCR_DMODE_Pos // data on 1 line
        | 0 << QUADSPI_CCR_DCYC_Pos // 0 dummy cycles
        | 0 << QUADSPI_CCR_ABMODE_Pos // no alternate byte
        | 0 << QUADSPI_CCR_ADMODE_Pos // no address
        | 1 << QUADSPI_CCR_IMODE_Pos // instruction on 1 line
        | cmd << QUADSPI_CCR_INSTRUCTION_Pos // read opcode
        ;

    // Wait for read to finish
    while (!(QUADSPI->SR & QUADSPI_SR_TCF)) {
    }

    QUADSPI->FCR = QUADSPI_FCR_CTCF; // clear TC flag

    // Read result
    return QUADSPI->DR;
}

STATIC void qspi_read_cmd_qaddr_qdata(void *self_in, uint8_t cmd, uint32_t addr, size_t len, uint8_t *dest) {
    (void)self_in;
    QUADSPI->FCR = QUADSPI_FCR_CTCF; // clear TC flag

    QUADSPI->DLR = len - 1; // number of bytes to read

    QUADSPI->CCR =
        0 << QUADSPI_CCR_DDRM_Pos // DDR mode disabled
        | 0 << QUADSPI_CCR_SIOO_Pos // send instruction every transaction
        | 1 << QUADSPI_CCR_FMODE_Pos // indirect read mode
        | 3 << QUADSPI_CCR_DMODE_Pos // data on 4 lines
        | 4 << QUADSPI_CCR_DCYC_Pos // 4 dummy cycles
        | 0 << QUADSPI_CCR_ABSIZE_Pos // 8-bit alternate byte
        | 3 << QUADSPI_CCR_ABMODE_Pos // alternate byte on 4 lines
        | 2 << QUADSPI_CCR_ADSIZE_Pos // 24-bit address size
        | 3 << QUADSPI_CCR_ADMODE_Pos // address on 4 lines
        | 1 << QUADSPI_CCR_IMODE_Pos // instruction on 1 line
        | cmd << QUADSPI_CCR_INSTRUCTION_Pos // quad read opcode
        ;

    QUADSPI->ABR = 0; // alternate byte: disable continuous read mode
    QUADSPI->AR = addr; // addres to read from

    // Read in the data 4 bytes at a time if dest is aligned
    if (((uintptr_t)dest & 3) == 0) {
        while (len >= 4) {
            while (!(QUADSPI->SR & QUADSPI_SR_FTF)) {
            }
            *(uint32_t*)dest = QUADSPI->DR;
            dest += 4;
            len -= 4;
        }
    }

    // Read in remaining data 1 byte at a time
    while (len) {
        while (!((QUADSPI->SR >> QUADSPI_SR_FLEVEL_Pos) & 0x3f)) {
        }
        *dest++ = *(volatile uint8_t*)&QUADSPI->DR;
        --len;
    }

    QUADSPI->FCR = QUADSPI_FCR_CTCF; // clear TC flag
}

const mp_qspi_proto_t qspi_proto = {
    .ioctl = qspi_ioctl,
    .write_cmd_data = qspi_write_cmd_data,
    .write_cmd_addr_data = qspi_write_cmd_addr_data,
    .read_cmd = qspi_read_cmd,
    .read_cmd_qaddr_qdata = qspi_read_cmd_qaddr_qdata,
};

#endif // defined(MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2)
