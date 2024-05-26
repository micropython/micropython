// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"

#include "mpconfigboard.h"
#include "csr.h"

// ICE40 LED Driver hard macro.
// See http://www.latticesemi.com/-/media/LatticeSemi/Documents/ApplicationNotes/IK/ICE40LEDDriverUsageGuide.ashx?document_id=50668
enum led_registers {
    LEDDCR0 = 8,
    LEDDBR = 9,
    LEDDONR = 10,
    LEDDOFR = 11,
    LEDDBCRR = 5,
    LEDDBCFR = 6,
    LEDDPWRR = 1,
    LEDDPWRG = 2,
    LEDDPWRB = 3,
};

#define BREATHE_ENABLE (1 << 7)
#define BREATHE_EDGE_ON (0 << 6)
#define BREATHE_EDGE_BOTH (1 << 6)
#define BREATHE_MODE_MODULATE (1 << 5)
#define BREATHE_RATE(x) ((x & 7) << 0)

// Write a value into the LEDDA_IP register.
static void ledda_write(uint8_t value, uint8_t addr) {
    rgb_addr_write(addr);
    rgb_dat_write(value);
}

void board_init(void) {
    uint8_t onrate = 15;
    uint8_t offrate = 1;

    ledda_write(BREATHE_ENABLE | BREATHE_MODE_MODULATE | BREATHE_RATE(onrate), LEDDBCRR);
    ledda_write(BREATHE_ENABLE | BREATHE_MODE_MODULATE | BREATHE_RATE(offrate), LEDDBCFR);

    ledda_write(123, LEDDPWRR); // Red
    ledda_write(3, LEDDPWRG);   // Green
    ledda_write(98, LEDDPWRB);  // Blue
}
