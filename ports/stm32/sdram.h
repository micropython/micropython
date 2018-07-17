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
bool sdram_init();
void * sdram_start();
void * sdram_end();
bool sdram_test(bool fast);
#endif // __SDRAM_H__
