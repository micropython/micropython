#define MICROPY_HW_BOARD_NAME       "Sparkfun SAMD51 Thing Plus"
#define MICROPY_HW_MCU_NAME         "SAMD51J20A"

#define MICROPY_HW_XOSC32K          (1)

// There seems to be an inconsistency in the SAMD51 Thing bootloader in that
// the bootloader magic address is at the end of a 192k RAM area, instead of
// 256k. Since the SAMD51x20A has 256k RAM, the loader symbol is at that address
// and so there is a fix here using the previous definition.
#define DBL_TAP_ADDR_ALT    ((volatile uint32_t *)(HSRAM_ADDR + HSRAM_SIZE - 0x10000 - 4))
