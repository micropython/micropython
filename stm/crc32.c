#include "std.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "stm32f4xx_crc.h"

#if MICROPY_HW_HASH
// Reverse bits
static inline uint32_t reverse_bits(uint32_t x)
{
    asm volatile (
            "rbit  %[r], %[x]\n"
            : [r] "=r" (x)
            : [x] "r"  (x));
    return x;
}

machine_uint_t qstr_compute_hash(const byte *buf, uint len) {
    uint32_t *p = (uint32_t*) buf;
    uint32_t crc, crc_reg=~0UL;

    CRC_ResetDR();

    while(len >= 4) {
        crc_reg = CRC_CalcCRC(reverse_bits(*p++));
        len -= 4;
    }
    crc = reverse_bits(crc_reg);
    if(len) {
        CRC_CalcCRC(crc_reg);
        switch(len) {
            case 1:
                crc_reg = CRC_CalcCRC(reverse_bits((*p & 0xFF) ^ crc) >> 24);
                crc = ( crc >> 8 ) ^ reverse_bits(crc_reg);
                break;
            case 2:
                crc_reg = CRC_CalcCRC(reverse_bits((*p & 0xFFFF) ^ crc) >> 16);
                crc = ( crc >> 16 ) ^ reverse_bits(crc_reg);
                break;
            case 3:
                crc_reg = CRC_CalcCRC(reverse_bits((*p & 0xFFFFFF) ^ crc) >> 8);
                crc = ( crc >> 24 ) ^ reverse_bits(crc_reg);
                break;
        }
    }
    return (~crc)&0xffff;
}
#endif
