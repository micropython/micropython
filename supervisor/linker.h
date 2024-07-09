// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// These macros are used to place code and data into different linking sections.

#pragma once

#if defined(IMXRT1XXX) || defined(FOMU) || defined(STM32H7) || defined(RASPBERRYPI)
#define PLACE_IN_DTCM_DATA(name) name __attribute__((section(".dtcm_data." #name)))
#define PLACE_IN_DTCM_BSS(name) name __attribute__((section(".dtcm_bss." #name)))
// Don't inline ITCM functions because that may pull them out of ITCM into other sections.
#define PLACE_IN_ITCM(name) __attribute__((section(".itcm." #name), noinline, aligned(4))) name
#else
#define PLACE_IN_DTCM_DATA(name) name
#define PLACE_IN_DTCM_BSS(name) name
#define PLACE_IN_ITCM(name) name
#endif
