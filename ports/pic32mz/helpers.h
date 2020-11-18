#ifndef MICROPY_INCLUDED_PIC32BIT_HELPERS_H
#define MICROPY_INCLUDED_PIC32BIT_HELPERS_H

#define BIT_INV(x, b) *x ^= (1 << b)
#define BIT_CLR(x, b) *x = *x & ~(1 << b)
#define BIT_SET(x, b) *x |= (1 << b)

#define BIT_MASK(x, mask, val) *x = (*x & ~mask) | (val & mask)

#endif // MICROPY_INCLUDED_PIC32BIT_HELPERS_H
