// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "include/sam.h"

#include "reset.h"
#include "supervisor/filesystem.h"

void NVIC_SystemReset(void) NORETURN;

void reset(void) {
    filesystem_flush();
    NVIC_SystemReset();
}


extern uint32_t _srelocate;
bool bootloader_available(void) {
    return &_bootloader_dbl_tap >= &_srelocate;
}
