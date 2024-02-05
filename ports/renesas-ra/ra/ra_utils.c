/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Renesas Electronics Corporation
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

#include "hal_data.h"
#include "ra_config.h"
#include "ra_utils.h"

static R_SYSTEM_Type *system_reg = (R_SYSTEM_Type *)0x4001E000;
static R_MSTP_Type *mstp_reg = R_MSTP;

void ra_mstpcra_stop(uint32_t mod_mask) {
    system_reg->PRCR = 0xa502;
    system_reg->MSTPCRA |= mod_mask;
    system_reg->PRCR = 0xa500;
}

void ra_mstpcra_start(uint32_t mod_mask) {
    system_reg->PRCR = 0xa502;
    system_reg->MSTPCRA &= ~mod_mask;
    system_reg->PRCR = 0xa500;
}

void ra_mstpcrb_stop(uint32_t mod_mask) {
    system_reg->PRCR = 0xa502;
    mstp_reg->MSTPCRB |= mod_mask;
    system_reg->PRCR = 0xa500;
}

void ra_mstpcrb_start(uint32_t mod_mask) {
    system_reg->PRCR = 0xa502;
    mstp_reg->MSTPCRB &= ~mod_mask;
    system_reg->PRCR = 0xa500;
}

void ra_mstpcrc_stop(uint32_t mod_mask) {
    system_reg->PRCR = 0xa502;
    mstp_reg->MSTPCRC |= mod_mask;
    system_reg->PRCR = 0xa500;
}

void ra_mstpcrc_start(uint32_t mod_mask) {
    system_reg->PRCR = 0xa502;
    mstp_reg->MSTPCRC &= ~mod_mask;
    system_reg->PRCR = 0xa500;
}

void ra_mstpcrd_stop(uint32_t mod_mask) {
    system_reg->PRCR = 0xa502;
    mstp_reg->MSTPCRD |= mod_mask;
    system_reg->PRCR = 0xa500;
}

void ra_mstpcrd_start(uint32_t mod_mask) {
    system_reg->PRCR = 0xa502;
    mstp_reg->MSTPCRD &= ~mod_mask;
    system_reg->PRCR = 0xa500;
}

void ra_mstpcre_stop(uint32_t mod_mask) {
    system_reg->PRCR = 0xa502;
    mstp_reg->MSTPCRE |= mod_mask;
    system_reg->PRCR = 0xa500;
}

void ra_mstpcre_start(uint32_t mod_mask) {
    system_reg->PRCR = 0xa502;
    mstp_reg->MSTPCRE &= ~mod_mask;
    system_reg->PRCR = 0xa500;
}

__WEAK void ctsu_write_isr(void) {
    // dummy
}

__WEAK void ctsu_read_isr(void) {
    // dummy
}

__WEAK void ctsu_end_isr(void) {
    // dummy
}
