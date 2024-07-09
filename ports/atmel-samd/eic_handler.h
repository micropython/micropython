// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT
#pragma once

#define EIC_HANDLER_NO_INTERRUPT 0x0
#define EIC_HANDLER_PULSEIN 0x1
#define EIC_HANDLER_INCREMENTAL_ENCODER 0x2
#define EIC_HANDLER_PS2 0x3
#define EIC_HANDLER_COUNTER 0x04
#define EIC_HANDLER_ALARM 0x05
#define EIC_HANDLER_MAX3421E 0x06

void set_eic_handler(uint8_t channel, uint8_t eic_handler);
void shared_eic_handler(uint8_t channel);
