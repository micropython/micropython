// SPDX-License-Identifier: The MIT License (MIT)
// Copyright 2022 IBM Corp.
#ifndef __IO_H
#define __IO_H

static inline uint8_t readb(unsigned long addr) {
    uint8_t val;
    __asm__ volatile ("sync; lbzcix %0,0,%1" : "=r" (val) : "r" (addr) : "memory");
    return val;
}

static inline uint16_t readw(unsigned long addr) {
    uint16_t val;
    __asm__ volatile ("sync; lhzcix %0,0,%1" : "=r" (val) : "r" (addr) : "memory");
    return val;
}

static inline uint32_t readl(unsigned long addr) {
    uint32_t val;
    __asm__ volatile ("sync; lwzcix %0,0,%1" : "=r" (val) : "r" (addr) : "memory");
    return val;
}

static inline uint64_t readq(unsigned long addr) {
    uint64_t val;
    __asm__ volatile ("sync; ldcix %0,0,%1" : "=r" (val) : "r" (addr) : "memory");
    return val;
}

static inline void writeb(uint8_t val, unsigned long addr) {
    __asm__ volatile ("sync; stbcix %0,0,%1" : : "r" (val), "r" (addr) : "memory");
}

static inline void writew(uint16_t val, unsigned long addr) {
    __asm__ volatile ("sync; sthcix %0,0,%1" : : "r" (val), "r" (addr) : "memory");
}

static inline void writel(uint32_t val, unsigned long addr) {
    __asm__ volatile ("sync; stwcix %0,0,%1" : : "r" (val), "r" (addr) : "memory");
}

static inline void writeq(uint64_t val, unsigned long addr) {
    __asm__ volatile ("sync; stdcix %0,0,%1" : : "r" (val), "r" (addr) : "memory");
}

#endif /* __IO_H */
