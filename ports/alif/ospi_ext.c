/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 OpenMV LLC.
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

#include ALIF_CMSIS_H
#include "ospi_ext.h"
#include "ospi_xip_user.h"

static void ospi_xip_disable(ospi_flash_cfg_t *ospi_cfg) {
    ospi_cfg->aes_regs->aes_control &= ~AES_CONTROL_XIP_EN;
}

static void ospi_xip_enable(ospi_flash_cfg_t *ospi_cfg) {
    ospi_cfg->aes_regs->aes_control |= AES_CONTROL_XIP_EN;
    #if OSPI_XIP_ENABLE_AES_DECRYPTION
    ospi_cfg->aes_regs->aes_control |= (AES_CONTROL_LD_KEY | AES_CONTROL_DECRYPT_EN);
    #endif
}

// Standard SPI transfer.
void ospi_spi_transfer(ospi_flash_cfg_t *ospi_cfg, size_t len, const uint8_t *buf_out, uint8_t *buf_in) {
    ospi_writel(ospi_cfg, ser, 0);
    spi_disable(ospi_cfg);

    uint32_t ctrlr0 = CTRLR0_IS_MST
        | (SINGLE << CTRLR0_SPI_FRF_OFFSET)
        | (SPI_TMOD_TR << CTRLR0_TMOD_OFFSET)
        | (CTRLR0_DFS_8bit << CTRLR0_DFS_OFFSET)
    ;

    uint32_t spi_ctrlr0 = TRANS_TYPE_STANDARD;

    ospi_writel(ospi_cfg, ctrlr0, ctrlr0);
    ospi_writel(ospi_cfg, ctrlr1, len - 1);
    ospi_writel(ospi_cfg, spi_ctrlr0, spi_ctrlr0);
    spi_enable(ospi_cfg);

    // Buffer output data in SPI FIFO.
    for (int i = 0; i < len; ++i) {
        ospi_writel(ospi_cfg, data_reg, buf_out[i]);
    }

    // Enable the SPI peripheral.
    ospi_writel(ospi_cfg, ser, ospi_cfg->ser);

    // Read in data.
    for (int i = 0; i < len; ++i) {
        unsigned int timeout = 100000;
        while (ospi_readl(ospi_cfg, rxflr) == 0) {
            if (--timeout == 0) {
                return;
            }
        }
        buf_in[i] = ospi_readl(ospi_cfg, data_reg);
    }
}

void ospi_setup_read_ext(ospi_flash_cfg_t *ospi_cfg, bool rxds, uint32_t inst_len, uint32_t addr_len, uint32_t data_len, uint32_t read_len, uint32_t wait_cycles) {
    ospi_writel(ospi_cfg, ser, 0);
    spi_disable(ospi_cfg);

    uint32_t val = CTRLR0_IS_MST
        | (OCTAL << CTRLR0_SPI_FRF_OFFSET)
        | (TMOD_RO << CTRLR0_TMOD_OFFSET)
        | (data_len << CTRLR0_DFS_OFFSET);

    ospi_writel(ospi_cfg, ctrlr0, val);
    ospi_writel(ospi_cfg, ctrlr1, read_len - 1);

    if (ospi_cfg->ddr_en) {
        val = TRANS_TYPE_FRF_DEFINED
            | (rxds << CTRLR0_SPI_RXDS_EN_OFFSET)
            | (1 << CTRLR0_SPI_DDR_EN_OFFSET)
            | (inst_len << CTRLR0_INST_L_OFFSET)
            | (addr_len << CTRLR0_ADDR_L_OFFSET)
            | (wait_cycles << CTRLR0_WAIT_CYCLES_OFFSET);
        if (inst_len == OSPI_INST_L_16bit) {
            val |= 1 << CTRLR0_INST_DDR_EN_OFFSET;
        }
    } else {
        val = TRANS_TYPE_FRF_DEFINED
            | (rxds << CTRLR0_SPI_RXDS_EN_OFFSET)
            | (inst_len << CTRLR0_INST_L_OFFSET)
            | (addr_len << CTRLR0_ADDR_L_OFFSET)
            | (wait_cycles << CTRLR0_WAIT_CYCLES_OFFSET);
    }

    ospi_writel(ospi_cfg, spi_ctrlr0, val);
    ospi_cfg->rx_req = read_len;
    spi_enable(ospi_cfg);
}

int ospi_recv_blocking_8bit_data(ospi_flash_cfg_t *ospi_cfg, uint32_t command, uint8_t *buffer) {
    ospi_writel(ospi_cfg, data_reg, command);
    ospi_writel(ospi_cfg, ser, ospi_cfg->ser);

    ospi_cfg->rx_cnt = 0;

    while (ospi_cfg->rx_cnt < ospi_cfg->rx_req) {
        unsigned int timeout = 100000;
        while (ospi_readl(ospi_cfg, rxflr) == 0) {
            if (--timeout == 0) {
                return -OSPI_ETIMEDOUT;
            }
        }
        *buffer++ = ospi_readl(ospi_cfg, data_reg);
        ospi_cfg->rx_cnt++;
    }

    return 0;
}

int ospi_recv_blocking_16bit_data(ospi_flash_cfg_t *ospi_cfg, uint32_t command, uint16_t *buffer) {
    ospi_writel(ospi_cfg, data_reg, command);
    ospi_writel(ospi_cfg, ser, ospi_cfg->ser);

    ospi_cfg->rx_cnt = 0;

    while (ospi_cfg->rx_cnt < ospi_cfg->rx_req) {
        unsigned int timeout = 100000;
        while (ospi_readl(ospi_cfg, rxflr) == 0) {
            if (--timeout == 0) {
                return -OSPI_ETIMEDOUT;
            }
        }
        *buffer++ = ospi_readl(ospi_cfg, data_reg);
        ospi_cfg->rx_cnt++;
    }

    return 0;
}

int ospi_recv_blocking_32bit_data(ospi_flash_cfg_t *ospi_cfg, uint32_t command, uint32_t *buffer) {
    ospi_writel(ospi_cfg, data_reg, command);
    ospi_writel(ospi_cfg, ser, ospi_cfg->ser);

    ospi_cfg->rx_cnt = 0;

    while (ospi_cfg->rx_cnt < ospi_cfg->rx_req) {
        unsigned int timeout = 100000;
        while (ospi_readl(ospi_cfg, rxflr) == 0) {
            if (--timeout == 0) {
                return -OSPI_ETIMEDOUT;
            }
        }
        *buffer++ = __ROR(ospi_readl(ospi_cfg, data_reg), 16);
        ospi_cfg->rx_cnt++;
    }

    return 0;
}

void ospi_setup_write_ext(ospi_flash_cfg_t *ospi_cfg, bool rxds, uint32_t inst_len, uint32_t addr_len, uint32_t data_len) {
    ospi_writel(ospi_cfg, ser, 0);
    spi_disable(ospi_cfg);

    uint32_t val = CTRLR0_IS_MST
        | (OCTAL << CTRLR0_SPI_FRF_OFFSET)
        | (TMOD_TO << CTRLR0_TMOD_OFFSET)
        | (data_len << CTRLR0_DFS_OFFSET);

    ospi_writel(ospi_cfg, ctrlr0, val);
    ospi_writel(ospi_cfg, ctrlr1, 0);

    if (ospi_cfg->ddr_en) {
        val = TRANS_TYPE_FRF_DEFINED
            | (rxds << CTRLR0_SPI_RXDS_EN_OFFSET)
            | (1 << CTRLR0_SPI_DDR_EN_OFFSET)
            | (inst_len << CTRLR0_INST_L_OFFSET)
            | (addr_len << CTRLR0_ADDR_L_OFFSET)
            | (0 << CTRLR0_WAIT_CYCLES_OFFSET);
        if (inst_len == OSPI_INST_L_16bit) {
            val |= 1 << CTRLR0_INST_DDR_EN_OFFSET;
        }
    } else {
        val = TRANS_TYPE_FRF_DEFINED
            | (rxds << CTRLR0_SPI_RXDS_EN_OFFSET)
            | (inst_len << CTRLR0_INST_L_OFFSET)
            | (addr_len << CTRLR0_ADDR_L_OFFSET)
            | (0 << CTRLR0_WAIT_CYCLES_OFFSET);
    }

    ospi_writel(ospi_cfg, spi_ctrlr0, val);
    spi_enable(ospi_cfg);
}

static inline uint32_t ospi_xip_ctrlr0(uint32_t data_len) {
    return CTRLR0_IS_MST
           | (OCTAL << CTRLR0_SPI_FRF_OFFSET)
           | (0 << CTRLR0_SCPOL_OFFSET)
           | (0 << CTRLR0_SCPH_OFFSET)
           | (0 << CTRLR0_SSTE_OFFSET)
           | (TMOD_RO << CTRLR0_TMOD_OFFSET)
           | (data_len << CTRLR0_DFS_OFFSET);
}

void ospi_xip_enter_ext(ospi_flash_cfg_t *ospi_cfg, uint32_t inst_len, uint32_t data_len, uint16_t incr_command, uint16_t wrap_command, uint16_t read_dummy_cycles) {
    spi_disable(ospi_cfg);

    ospi_writel(ospi_cfg, ctrlr0, ospi_xip_ctrlr0(data_len));

    uint32_t val = (OCTAL << XIP_CTRL_FRF_OFFSET)
        | (0x2 << XIP_CTRL_TRANS_TYPE_OFFSET)
        | (ADDR_L32bit << XIP_CTRL_ADDR_L_OFFSET)
        | (inst_len << XIP_CTRL_INST_L_OFFSET)
        | (0x0 << XIP_CTRL_MD_BITS_EN_OFFSET)
        | (read_dummy_cycles << XIP_CTRL_WAIT_CYCLES_OFFSET)
        | (0x1 << XIP_CTRL_DFC_HC_OFFSET)
        | (ospi_cfg->ddr_en << XIP_CTRL_DDR_EN_OFFSET)
        | (0x1 << XIP_CTRL_RXDS_EN_OFFSET)
        | (0x1 << XIP_CTRL_INST_EN_OFFSET)
        | (0x1 << XIP_CTRL_CONT_XFER_EN_OFFSET)
        | (0x0 << XIP_CTRL_HYPERBUS_EN_OFFSET)
        | (0x1 << XIP_CTRL_RXDS_SIG_EN)
        | (0x0 << XIP_CTRL_XIP_MBL_OFFSET)
        | (0x0 << XIP_PREFETCH_EN_OFFSET)
        | (0x0 << XIP_CTRL_RXDS_VL_EN_OFFSET);

    if (inst_len == OSPI_INST_L_16bit) {
        val |= 1 << XIP_CTRL_INST_DDR_EN_OFFSET;
    }

    ospi_writel(ospi_cfg, xip_ctrl, val);

    ospi_writel(ospi_cfg, rx_sample_dly, OSPI_XIP_RX_SAMPLE_DELAY);
    ospi_writel(ospi_cfg, txd_drive_edge, OSPI_XIP_DDR_DRIVE_EDGE);
    ospi_cfg->aes_regs->aes_rxds_delay = OSPI_XIP_RXDS_DELAY;

    ospi_writel(ospi_cfg, xip_mode_bits, 0x0);
    ospi_writel(ospi_cfg, xip_incr_inst, incr_command);
    ospi_writel(ospi_cfg, xip_wrap_inst, wrap_command);
    ospi_writel(ospi_cfg, xip_ser, ospi_cfg->ser);
    ospi_writel(ospi_cfg, xip_cnt_time_out, 100);

    spi_enable(ospi_cfg);
    ospi_xip_enable(ospi_cfg);
}

void ospi_xip_exit_ext(ospi_flash_cfg_t *ospi_cfg, uint32_t inst_len, uint16_t incr_command, uint16_t wrap_command) {
    spi_disable(ospi_cfg);

    uint32_t val = CTRLR0_IS_MST
        | (OCTAL << CTRLR0_SPI_FRF_OFFSET)
        | (0 << CTRLR0_SCPOL_OFFSET)
        | (0 << CTRLR0_SCPH_OFFSET)
        | (0 << CTRLR0_SSTE_OFFSET)
        | (TMOD_RO << CTRLR0_TMOD_OFFSET)
        | (CTRLR0_DFS_32bit << CTRLR0_DFS_OFFSET);

    ospi_writel(ospi_cfg, ctrlr0, val);

    val = TRANS_TYPE_FRF_DEFINED
        | ((ospi_cfg->ddr_en) << CTRLR0_SPI_DDR_EN_OFFSET)
        | (2 << CTRLR0_XIP_MBL_OFFSET)
        | (1 << CTRLR0_XIP_DFS_HC_OFFSET)
        | (1 << CTRLR0_XIP_INST_EN_OFFSET)
        | (inst_len << CTRLR0_INST_L_OFFSET)
        | (ospi_cfg->addrlen) << (CTRLR0_ADDR_L_OFFSET)
        | (ospi_cfg->wait_cycles << CTRLR0_WAIT_CYCLES_OFFSET);

    ospi_writel(ospi_cfg, spi_ctrlr0, val);

    ospi_writel(ospi_cfg, xip_mode_bits, 0x1);
    ospi_writel(ospi_cfg, xip_incr_inst, incr_command);
    ospi_writel(ospi_cfg, xip_wrap_inst, wrap_command);
    ospi_writel(ospi_cfg, xip_ser, ospi_cfg->ser);
    ospi_writel(ospi_cfg, ser, ospi_cfg->ser);
    ospi_writel(ospi_cfg, xip_cnt_time_out, 100);

    spi_enable(ospi_cfg);

    ospi_xip_enable(ospi_cfg);
    ospi_xip_disable(ospi_cfg);
}

void ospi_xip_restore_ext(ospi_flash_cfg_t *ospi_cfg, uint32_t data_len) {
    spi_disable(ospi_cfg);
    ospi_writel(ospi_cfg, ctrlr0, ospi_xip_ctrlr0(data_len));
    spi_enable(ospi_cfg);
}
