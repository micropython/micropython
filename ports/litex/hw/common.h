// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#pragma once
#include <stdint.h>
static inline void csr_writeb(uint8_t value, uint32_t addr) {
    *((volatile uint8_t *)addr) = value;
}

static inline uint8_t csr_readb(uint32_t addr) {
    return *(volatile uint8_t *)addr;
}

static inline void csr_writew(uint16_t value, uint32_t addr) {
    *((volatile uint16_t *)addr) = value;
}

static inline uint16_t csr_readw(uint32_t addr) {
    return *(volatile uint16_t *)addr;
}

static inline void csr_writel(uint32_t value, uint32_t addr) {
    *((volatile uint32_t *)addr) = value;
}

static inline uint32_t csr_readl(uint32_t addr) {
    return *(volatile uint32_t *)addr;
}
