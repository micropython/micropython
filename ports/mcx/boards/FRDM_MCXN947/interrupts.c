/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 NXP
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

#include "board.h"

void MCX_BoardGenericGPIOISR(uint8_t id);
void MCX_BoardGenericUARTISR(uint8_t id);
void MCX_BoardGenericUSBISR(uint8_t id);

void GPIO00_IRQHandler(void) {
    MCX_BoardGenericGPIOISR(0);
}

void GPIO10_IRQHandler(void) {
    MCX_BoardGenericGPIOISR(1);
}

void GPIO20_IRQHandler(void) {
    MCX_BoardGenericGPIOISR(2);
}

void GPIO30_IRQHandler(void) {
    MCX_BoardGenericGPIOISR(3);
}

void GPIO40_IRQHandler(void) {
    MCX_BoardGenericGPIOISR(4);
}

void GPIO50_IRQHandler(void) {
    MCX_BoardGenericGPIOISR(5);
}

void LP_FLEXCOMM0_IRQHandler(void) {
    MCX_BoardGenericUARTISR(0);
}

void LP_FLEXCOMM1_IRQHandler(void) {
    MCX_BoardGenericUARTISR(1);
}

void LP_FLEXCOMM2_IRQHandler(void) {
    MCX_BoardGenericUARTISR(2);
}

void LP_FLEXCOMM3_IRQHandler(void) {
    MCX_BoardGenericUARTISR(3);
}

void LP_FLEXCOMM4_IRQHandler(void) {
    MCX_BoardGenericUARTISR(4);
}

void LP_FLEXCOMM5_IRQHandler(void) {
    MCX_BoardGenericUARTISR(5);
}

void LP_FLEXCOMM6_IRQHandler(void) {
    MCX_BoardGenericUARTISR(6);
}

void LP_FLEXCOMM7_IRQHandler(void) {
    MCX_BoardGenericUARTISR(7);
}

void LP_FLEXCOMM8_IRQHandler(void) {
    MCX_BoardGenericUARTISR(8);
}

void LP_FLEXCOMM9_IRQHandler(void) {
    MCX_BoardGenericUARTISR(9);
}

void USB0_FS_IRQHandler(void) {
    MCX_BoardGenericUSBISR(0);
}

void USB1_HS_IRQHandler(void) {
    MCX_BoardGenericUSBISR(1);
}
