// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#pragma once

extern uint32_t SystemCoreClock;

void clocks_init(void);
uint32_t setarmclock(uint32_t frequency);
