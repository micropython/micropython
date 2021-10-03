/*
 * This file is part of the OpenMV project.
 * Copyright (c) 2013/2014 Ibrahim Abdelkader <i.abdalkader@gmail.com>
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * SDRAM Driver.
 *
 */
#ifndef __SDRAM_H__
#define __SDRAM_H__
bool sdram_init(void);
void *sdram_start(void);
void *sdram_end(void);
void sdram_enter_low_power(void);
void sdram_leave_low_power(void);
bool sdram_test(bool exhaustive);
#endif // __SDRAM_H__
