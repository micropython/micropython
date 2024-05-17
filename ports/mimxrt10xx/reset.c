// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "reset.h"
#include "supervisor/filesystem.h"
#include "supervisor/linker.h"

#include "fsl_common.h"

void PLACE_IN_ITCM(reset)(void) {
    filesystem_flush();
    NVIC_SystemReset();
}

bool bootloader_available(void) {
    return SNVS->LPGPR[0] >= 0;
}
