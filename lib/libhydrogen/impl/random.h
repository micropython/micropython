static TLS struct {
    _hydro_attr_aligned_(16) uint8_t state[gimli_BLOCKBYTES];
    uint64_t counter;
    uint8_t  initialized;
    uint8_t  available;
} hydro_random_context;

#if defined(AVR) && !defined(__unix__)
#include <Arduino.h>

static bool
hydro_random_rbit(uint16_t x)
{
    uint8_t x8;

    x8 = ((uint8_t)(x >> 8)) ^ (uint8_t) x;
    x8 = (x8 >> 4) ^ (x8 & 0xf);
    x8 = (x8 >> 2) ^ (x8 & 0x3);
    x8 = (x8 >> 1) ^ x8;

    return (bool) (x8 & 1);
}

static int
hydro_random_init(void)
{
    const char       ctx[hydro_hash_CONTEXTBYTES] = { 'h', 'y', 'd', 'r', 'o', 'P', 'R', 'G' };
    hydro_hash_state st;
    uint16_t         ebits = 0;
    uint16_t         tc;
    bool             a, b;

    cli();
    MCUSR = 0;
    WDTCSR |= _BV(WDCE) | _BV(WDE);
    WDTCSR = _BV(WDIE);
    sei();

    hydro_hash_init(&st, ctx, NULL);

    while (ebits < 256) {
        delay(1);
        tc = TCNT1;
        hydro_hash_update(&st, (const uint8_t *) &tc, sizeof tc);
        a = hydro_random_rbit(tc);
        delay(1);
        tc = TCNT1;
        b  = hydro_random_rbit(tc);
        hydro_hash_update(&st, (const uint8_t *) &tc, sizeof tc);
        if (a == b) {
            continue;
        }
        hydro_hash_update(&st, (const uint8_t *) &b, sizeof b);
        ebits++;
    }

    cli();
    MCUSR = 0;
    WDTCSR |= _BV(WDCE) | _BV(WDE);
    WDTCSR = 0;
    sei();

    hydro_hash_final(&st, hydro_random_context.state, sizeof hydro_random_context.state);
    hydro_random_context.counter = ~LOAD64_LE(hydro_random_context.state);

    return 0;
}

ISR(WDT_vect) {}

#elif (defined(ESP32) || defined(ESP8266)) && !defined(__unix__)

// Important: RF *must* be activated on ESP board
// https://techtutorialsx.com/2017/12/22/esp32-arduino-random-number-generation/

#include <esp_system.h>

static int
hydro_random_init(void)
{
    const char       ctx[hydro_hash_CONTEXTBYTES] = { 'h', 'y', 'd', 'r', 'o', 'P', 'R', 'G' };
    hydro_hash_state st;
    uint16_t         ebits = 0;

    hydro_hash_init(&st, ctx, NULL);

    while (ebits < 256) {
        uint32_t r = esp_random();

        delay(10);
        hydro_hash_update(&st, (const uint32_t *) &r, sizeof r);
        ebits += 32;
    }

    hydro_hash_final(&st, hydro_random_context.state, sizeof hydro_random_context.state);
    hydro_random_context.counter = ~LOAD64_LE(hydro_random_context.state);

    return 0;
}

#elif defined(PARTICLE) && defined(PLATFORM_ID) && PLATFORM_ID > 2 && !defined(__unix__)

// Note: All particle platforms except for the Spark Core have a HW RNG.  Only allow building on
// supported platforms for now. PLATFORM_ID definitions:
// https://github.com/particle-iot/device-os/blob/mesh-develop/hal/shared/platforms.h

#include "Particle.h"

static int
hydro_random_init(void)
{
    const char       ctx[hydro_hash_CONTEXTBYTES] = { 'h', 'y', 'd', 'r', 'o', 'P', 'R', 'G' };
    hydro_hash_state st;
    uint16_t         ebits = 0;

    hydro_hash_init(&st, ctx, NULL);

    while (ebits < 256) {
        uint32_t r = HAL_RNG_GetRandomNumber();
        hydro_hash_update(&st, (const uint32_t *) &r, sizeof r);
        ebits += 32;
    }

    hydro_hash_final(&st, hydro_random_context.state, sizeof hydro_random_context.state);
    hydro_random_context.counter = ~LOAD64_LE(hydro_random_context.state);

    return 0;
}

#elif (defined(NRF52832_XXAA) || defined(NRF52832_XXAB)) && !defined(__unix__)

// Important: The SoftDevice *must* be activated to enable reading from the RNG
// http://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.nrf52832.ps.v1.1%2Frng.html

#include <nrf_soc.h>

static int
hydro_random_init(void)
{
    const char       ctx[hydro_hash_CONTEXTBYTES] = { 'h', 'y', 'd', 'r', 'o', 'P', 'R', 'G' };
    hydro_hash_state st;
    const uint8_t    total_bytes     = 32;
    uint8_t          remaining_bytes = total_bytes;
    uint8_t          available_bytes;
    uint8_t          rand_buffer[32];

    hydro_hash_init(&st, ctx, NULL);

    for (;;) {
        if (sd_rand_application_bytes_available_get(&available_bytes) != NRF_SUCCESS) {
            return -1;
        }
        if (available_bytes > 0) {
            if (available_bytes > remaining_bytes) {
                available_bytes = remaining_bytes;
            }
            if (sd_rand_application_vector_get(rand_buffer, available_bytes) != NRF_SUCCESS) {
                return -1;
            }
            hydro_hash_update(&st, rand_buffer, total_bytes);
            remaining_bytes -= available_bytes;
        }
        if (remaining_bytes <= 0) {
            break;
        }
        delay(10);
    }
    hydro_hash_final(&st, hydro_random_context.state, sizeof hydro_random_context.state);
    hydro_random_context.counter = ~LOAD64_LE(hydro_random_context.state);

    return 0;
}

#elif defined(_WIN32)

#include <windows.h>
#define RtlGenRandom SystemFunction036
#if defined(__cplusplus)
extern "C"
#endif
    BOOLEAN NTAPI
    RtlGenRandom(PVOID RandomBuffer, ULONG RandomBufferLength);
#pragma comment(lib, "advapi32.lib")

static int
hydro_random_init(void)
{
    if (!RtlGenRandom((PVOID) hydro_random_context.state,
                      (ULONG) sizeof hydro_random_context.state)) {
        return -1;
    }
    hydro_random_context.counter = ~LOAD64_LE(hydro_random_context.state);

    return 0;
}

#elif defined(__wasi__)

#include <unistd.h>

static int
hydro_random_init(void)
{
    if (getentropy(hydro_random_context.state, sizeof hydro_random_context.state) != 0) {
        return -1;
    }
    hydro_random_context.counter = ~LOAD64_LE(hydro_random_context.state);

    return 0;
}

#elif defined(__unix__)

#include <errno.h>
#include <fcntl.h>
#ifdef __linux__
#include <poll.h>
#endif
#include <sys/types.h>
#include <unistd.h>

#ifdef __linux__
static int
hydro_random_block_on_dev_random(void)
{
    struct pollfd pfd;
    int           fd;
    int           pret;

    fd = open("/dev/random", O_RDONLY);
    if (fd == -1) {
        return 0;
    }
    pfd.fd      = fd;
    pfd.events  = POLLIN;
    pfd.revents = 0;
    do {
        pret = poll(&pfd, 1, -1);
    } while (pret < 0 && (errno == EINTR || errno == EAGAIN));
    if (pret != 1) {
        (void) close(fd);
        errno = EIO;
        return -1;
    }
    return close(fd);
}
#endif

static ssize_t
hydro_random_safe_read(const int fd, void *const buf_, size_t len)
{
    unsigned char *buf = (unsigned char *) buf_;
    ssize_t        readnb;

    do {
        while ((readnb = read(fd, buf, len)) < (ssize_t) 0 && (errno == EINTR || errno == EAGAIN)) {
        }
        if (readnb < (ssize_t) 0) {
            return readnb;
        }
        if (readnb == (ssize_t) 0) {
            break;
        }
        len -= (size_t) readnb;
        buf += readnb;
    } while (len > (ssize_t) 0);

    return (ssize_t)(buf - (unsigned char *) buf_);
}

static int
hydro_random_init(void)
{
    uint8_t tmp[gimli_BLOCKBYTES + 8];
    int     fd;
    int     ret = -1;

#ifdef __linux__
    if (hydro_random_block_on_dev_random() != 0) {
        return -1;
    }
#endif
    do {
        fd = open("/dev/urandom", O_RDONLY);
        if (fd == -1 && errno != EINTR) {
            return -1;
        }
    } while (fd == -1);
    if (hydro_random_safe_read(fd, tmp, sizeof tmp) == (ssize_t) sizeof tmp) {
        memcpy(hydro_random_context.state, tmp, gimli_BLOCKBYTES);
        memcpy(&hydro_random_context.counter, tmp + gimli_BLOCKBYTES, 8);
        hydro_memzero(tmp, sizeof tmp);
        ret = 0;
    }
    ret |= close(fd);

    return ret;
}

#elif defined(TARGET_LIKE_MBED)

#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>

#if defined(MBEDTLS_ENTROPY_C)

static int
hydro_random_init(void)
{
    mbedtls_entropy_context entropy;
    uint16_t                pos = 0;

    mbedtls_entropy_init(&entropy);

    // Pull data directly out of the entropy pool for the state, as it's small enough.
    if (mbedtls_entropy_func(&entropy, (uint8_t *) &hydro_random_context.counter,
                             sizeof hydro_random_context.counter) != 0) {
        return -1;
    }
    // mbedtls_entropy_func can't provide more than MBEDTLS_ENTROPY_BLOCK_SIZE in one go.
    // This constant depends of mbedTLS configuration (whether the PRNG is backed by SHA256/SHA512
    // at this time) Therefore, if necessary, we get entropy multiple times.

    do {
        const uint8_t dataLeftToConsume = gimli_BLOCKBYTES - pos;
        const uint8_t currentChunkSize  = (dataLeftToConsume > MBEDTLS_ENTROPY_BLOCK_SIZE)
                                             ? MBEDTLS_ENTROPY_BLOCK_SIZE
                                             : dataLeftToConsume;

        // Forces mbedTLS to fetch fresh entropy, then get some to feed libhydrogen.
        if (mbedtls_entropy_gather(&entropy) != 0 ||
            mbedtls_entropy_func(&entropy, &hydro_random_context.state[pos], currentChunkSize) !=
                0) {
            return -1;
        }
        pos += MBEDTLS_ENTROPY_BLOCK_SIZE;
    } while (pos < gimli_BLOCKBYTES);

    mbedtls_entropy_free(&entropy);

    return 0;
}

#else
#error Need an entropy source
#endif

#else
#error Unsupported platform
#endif

static void
hydro_random_check_initialized(void)
{
    if (hydro_random_context.initialized == 0) {
        if (hydro_random_init() != 0) {
            abort();
        }
        gimli_core_u8(hydro_random_context.state, 0);
        hydro_random_ratchet();
        hydro_random_context.initialized = 1;
    }
}

void
hydro_random_ratchet(void)
{
    mem_zero(hydro_random_context.state, gimli_RATE);
    STORE64_LE(hydro_random_context.state, hydro_random_context.counter);
    hydro_random_context.counter++;
    gimli_core_u8(hydro_random_context.state, 0);
    hydro_random_context.available = gimli_RATE;
}

uint32_t
hydro_random_u32(void)
{
    uint32_t v;

    hydro_random_check_initialized();
    if (hydro_random_context.available < 4) {
        hydro_random_ratchet();
    }
    memcpy(&v, &hydro_random_context.state[gimli_RATE - hydro_random_context.available], 4);
    hydro_random_context.available -= 4;

    return v;
}

uint32_t
hydro_random_uniform(const uint32_t upper_bound)
{
    uint32_t min;
    uint32_t r;

    if (upper_bound < 2U) {
        return 0;
    }
    min = (1U + ~upper_bound) % upper_bound; /* = 2**32 mod upper_bound */
    do {
        r = hydro_random_u32();
    } while (r < min);
    /* r is now clamped to a set whose size mod upper_bound == 0
     * the worst case (2**31+1) requires 2 attempts on average */

    return r % upper_bound;
}

void
hydro_random_buf(void *out, size_t out_len)
{
    uint8_t *p = (uint8_t *) out;
    size_t   i;
    size_t   leftover;

    hydro_random_check_initialized();
    for (i = 0; i < out_len / gimli_RATE; i++) {
        gimli_core_u8(hydro_random_context.state, 0);
        memcpy(p + i * gimli_RATE, hydro_random_context.state, gimli_RATE);
    }
    leftover = out_len % gimli_RATE;
    if (leftover != 0) {
        gimli_core_u8(hydro_random_context.state, 0);
        mem_cpy(p + i * gimli_RATE, hydro_random_context.state, leftover);
    }
    hydro_random_ratchet();
}

void
hydro_random_buf_deterministic(void *out, size_t out_len,
                               const uint8_t seed[hydro_random_SEEDBYTES])
{
    static const uint8_t             prefix[] = { 7, 'd', 'r', 'b', 'g', '2', '5', '6' };
    _hydro_attr_aligned_(16) uint8_t state[gimli_BLOCKBYTES];
    uint8_t *                        p = (uint8_t *) out;
    size_t                           i;
    size_t                           leftover;

    mem_zero(state, gimli_BLOCKBYTES);
    COMPILER_ASSERT(sizeof prefix + 8 <= gimli_RATE);
    memcpy(state, prefix, sizeof prefix);
    STORE64_LE(state + sizeof prefix, (uint64_t) out_len);
    gimli_core_u8(state, 1);
    COMPILER_ASSERT(hydro_random_SEEDBYTES == gimli_RATE * 2);
    mem_xor(state, seed, gimli_RATE);
    gimli_core_u8(state, 2);
    mem_xor(state, seed + gimli_RATE, gimli_RATE);
    gimli_core_u8(state, 2);
    for (i = 0; i < out_len / gimli_RATE; i++) {
        gimli_core_u8(state, 0);
        memcpy(p + i * gimli_RATE, state, gimli_RATE);
    }
    leftover = out_len % gimli_RATE;
    if (leftover != 0) {
        gimli_core_u8(state, 0);
        mem_cpy(p + i * gimli_RATE, state, leftover);
    }
    hydro_random_ratchet();
}

void
hydro_random_reseed(void)
{
    hydro_random_context.initialized = 0;
    hydro_random_check_initialized();
}
