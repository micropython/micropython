/*
 *
 * SRAM Driver.
 *
 */
#ifndef __SRAM_H__
#define __SRAM_H__

bool sram_init(void);
void *sram_start(void);
void *sram_end(void);

bool sram_test(bool fast);

#endif // __SDRAM_H__