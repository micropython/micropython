#ifndef MICROPY_INCLUDED_ESP8266_ETSHAL_H
#define MICROPY_INCLUDED_ESP8266_ETSHAL_H

#include <os_type.h>

// see http://esp8266-re.foogod.com/wiki/Random_Number_Generator
#define WDEV_HWRNG ((volatile uint32_t*)0x3ff20e44)

void ets_delay_us(uint16_t us);
void ets_intr_lock(void);
void ets_intr_unlock(void);
void ets_isr_mask(uint32_t mask);
void ets_isr_unmask(uint32_t mask);
void ets_isr_attach(int irq_no, void (*handler)(void *), void *arg);
void ets_install_putc1();
void uart_div_modify(uint8_t uart, uint32_t divisor);
void ets_set_idle_cb(void (*handler)(void *), void *arg);

void ets_timer_arm_new(os_timer_t *tim, uint32_t millis, bool repeat, bool is_milli_timer);
void ets_timer_setfn(os_timer_t *tim, ETSTimerFunc callback, void *cb_data);
void ets_timer_disarm(os_timer_t *tim);

extern void ets_wdt_disable(void);
extern void wdt_feed(void);

// Opaque structure
#ifndef MD5_CTX
typedef char MD5_CTX[88];
#endif

void MD5Init(MD5_CTX *context);
void MD5Update(MD5_CTX *context, const void *data, unsigned int len);
void MD5Final(unsigned char digest[16], MD5_CTX *context);

// These prototypes are for recent SDKs with "malloc tracking"
void *pvPortMalloc(size_t sz, const char *fname, unsigned line);
void *pvPortZalloc(size_t sz, const char *fname, unsigned line);
void *pvPortRealloc(void *p, unsigned sz, const char *fname, unsigned line);
void vPortFree(void *p, const char *fname, unsigned line);

uint32_t SPIRead(uint32_t offset, void *buf, uint32_t len);
uint32_t SPIWrite(uint32_t offset, const void *buf, uint32_t len);
uint32_t SPIEraseSector(int sector);

#endif // MICROPY_INCLUDED_ESP8266_ETSHAL_H
