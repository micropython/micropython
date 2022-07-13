#ifndef MICROPY_INCLUDED_ESP8266_ETSHAL_H
#define MICROPY_INCLUDED_ESP8266_ETSHAL_H

#include <os_type.h>

// see http://esp8266-re.foogod.com/wiki/Random_Number_Generator
#define WDEV_HWRNG ((volatile uint32_t *)0x3ff20e44)

void ets_isr_mask(uint32_t mask);
void ets_isr_unmask(uint32_t mask);

// Opaque structure
#ifndef MD5_CTX
typedef char MD5_CTX[88];
#endif

void MD5Init(MD5_CTX *context);
void MD5Update(MD5_CTX *context, const void *data, unsigned int len);
void MD5Final(unsigned char digest[16], MD5_CTX *context);

uint32_t SPIRead(uint32_t offset, void *buf, uint32_t len);
uint32_t SPIWrite(uint32_t offset, const void *buf, uint32_t len);
uint32_t SPIEraseSector(int sector);

#endif // MICROPY_INCLUDED_ESP8266_ETSHAL_H
