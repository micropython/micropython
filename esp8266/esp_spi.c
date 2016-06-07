/*
 * ESP hardware SPI master driver
 *
 * Part of esp-open-rtos
 * Copyright (c) Ruslan V. Uss, 2016
 * BSD Licensed as described in the file LICENSE
 */
#include "esp/spi.h"

#include "esp/iomux.h"
#include "esp/gpio.h"
#include <string.h>

#define _SPI0_SCK_GPIO  6
#define _SPI0_MISO_GPIO 7
#define _SPI0_MOSI_GPIO 8
#define _SPI0_HD_GPIO   9
#define _SPI0_WP_GPIO   10
#define _SPI0_CS0_GPIO  11

#define _SPI1_MISO_GPIO 12
#define _SPI1_MOSI_GPIO 13
#define _SPI1_SCK_GPIO  14
#define _SPI1_CS0_GPIO  15

#define _SPI0_FUNC 1
#define _SPI1_FUNC 2

#define _SPI_BUF_SIZE 64

static bool _minimal_pins[2] = {false, false};

inline static void _set_pin_function(uint8_t pin, uint32_t function)
{
    iomux_set_function(gpio_to_iomux(pin), function);
}

bool spi_init(uint8_t bus, spi_mode_t mode, uint32_t freq_divider, bool msb, spi_endianness_t endianness, bool minimal_pins)
{
    switch (bus)
    {
        case 0:
            _set_pin_function(_SPI0_MISO_GPIO, _SPI0_FUNC);
            _set_pin_function(_SPI0_MOSI_GPIO, _SPI0_FUNC);
            _set_pin_function(_SPI0_SCK_GPIO, _SPI0_FUNC);
            if (!minimal_pins)
            {
                _set_pin_function(_SPI0_HD_GPIO, _SPI0_FUNC);
                _set_pin_function(_SPI0_WP_GPIO, _SPI0_FUNC);
                _set_pin_function(_SPI0_CS0_GPIO, _SPI0_FUNC);
            }
            break;
        case 1:
            _set_pin_function(_SPI1_MISO_GPIO, _SPI1_FUNC);
            _set_pin_function(_SPI1_MOSI_GPIO, _SPI1_FUNC);
            _set_pin_function(_SPI1_SCK_GPIO, _SPI1_FUNC);
            if (!minimal_pins)
                _set_pin_function(_SPI1_CS0_GPIO, _SPI1_FUNC);
            break;
        default:
            return false;
    }

    _minimal_pins[bus] = minimal_pins;
    SPI(bus).USER0 = SPI_USER0_MOSI | SPI_USER0_CLOCK_IN_EDGE | SPI_USER0_DUPLEX |
        (minimal_pins ? 0 : (SPI_USER0_CS_HOLD | SPI_USER0_CS_SETUP));

    spi_set_frequency_div(bus, freq_divider);
    spi_set_mode(bus, mode);
    spi_set_msb(bus, msb);
    spi_set_endianness(bus, endianness);

    return true;
}

void spi_get_settings(uint8_t bus, spi_settings_t *s)
{
    s->mode = spi_get_mode(bus);
    s->freq_divider = spi_get_frequency_div(bus);
    s->msb = spi_get_msb(bus);
    s->endianness = spi_get_endianness(bus);
    s->minimal_pins = _minimal_pins[bus];
}

void spi_set_mode(uint8_t bus, spi_mode_t mode)
{
    bool cpha = (uint8_t)mode & 1;
    bool cpol = (uint8_t)mode & 2;
    if (cpol)
        cpha = !cpha;  // CPHA must be inverted when CPOL = 1, I have no idea why

    // CPHA
    if (cpha)
        SPI(bus).USER0 |= SPI_USER0_CLOCK_OUT_EDGE;
    else
        SPI(bus).USER0 &= ~SPI_USER0_CLOCK_OUT_EDGE;

    // CPOL - see http://bbs.espressif.com/viewtopic.php?t=342#p5384
    if (cpol)
        SPI(bus).PIN |= SPI_PIN_IDLE_EDGE;
    else
        SPI(bus).PIN &= ~SPI_PIN_IDLE_EDGE;
}

spi_mode_t spi_get_mode(uint8_t bus)
{
    uint8_t cpha = SPI(bus).USER0 & SPI_USER0_CLOCK_OUT_EDGE ? 1 : 0;
    uint8_t cpol = SPI(bus).PIN & SPI_PIN_IDLE_EDGE ? 2 : 0;

    return (spi_mode_t)(cpol | (cpol ? 1 - cpha : cpha)); // see spi_set_mode
}

void spi_set_msb(uint8_t bus, bool msb)
{
    if (msb)
        SPI(bus).CTRL0 &= ~(SPI_CTRL0_WR_BIT_ORDER | SPI_CTRL0_RD_BIT_ORDER);
    else
        SPI(bus).CTRL0 |= (SPI_CTRL0_WR_BIT_ORDER | SPI_CTRL0_RD_BIT_ORDER);
}

void spi_set_endianness(uint8_t bus, spi_endianness_t endianness)
{
    if (endianness == SPI_BIG_ENDIAN)
        SPI(bus).USER0 |= (SPI_USER0_WR_BYTE_ORDER | SPI_USER0_RD_BYTE_ORDER);
    else
        SPI(bus).USER0 &= ~(SPI_USER0_WR_BYTE_ORDER | SPI_USER0_RD_BYTE_ORDER);
}

void spi_set_frequency_div(uint8_t bus, uint32_t divider)
{
    uint32_t predivider = (divider & 0xffff) - 1;
    uint32_t count = (divider >> 16) - 1;
    if (count || predivider)
    {
        IOMUX.CONF &= ~(bus == 0 ? IOMUX_CONF_SPI0_CLOCK_EQU_SYS_CLOCK : IOMUX_CONF_SPI1_CLOCK_EQU_SYS_CLOCK);
        SPI(bus).CLOCK = VAL2FIELD_M(SPI_CLOCK_DIV_PRE, predivider) |
                         VAL2FIELD_M(SPI_CLOCK_COUNT_NUM, count) |
                         VAL2FIELD_M(SPI_CLOCK_COUNT_HIGH, count / 2) |
                         VAL2FIELD_M(SPI_CLOCK_COUNT_LOW, count);
    }
    else
    {
        IOMUX.CONF |= bus == 0 ? IOMUX_CONF_SPI0_CLOCK_EQU_SYS_CLOCK : IOMUX_CONF_SPI1_CLOCK_EQU_SYS_CLOCK;
        SPI(bus).CLOCK = SPI_CLOCK_EQU_SYS_CLOCK;
    }
}

inline static void _set_size(uint8_t bus, uint8_t bytes)
{
    uint32_t bits = ((uint32_t)bytes << 3) - 1;
    SPI(bus).USER1 = SET_FIELD(SPI(bus).USER1, SPI_USER1_MISO_BITLEN, bits);
    SPI(bus).USER1 = SET_FIELD(SPI(bus).USER1, SPI_USER1_MOSI_BITLEN, bits);
}

inline static void _wait(uint8_t bus)
{
    while (SPI(bus).CMD & SPI_CMD_USR)
        ;
}

inline static void _start(uint8_t bus)
{
    SPI(bus).CMD |= SPI_CMD_USR;
}

inline static uint32_t _swap_bytes(uint32_t value)
{
    return (value << 24) | ((value << 8) & 0x00ff0000) | ((value >> 8) & 0x0000ff00) | (value >> 24);
}

inline static uint32_t _swap_words(uint32_t value)
{
    return (value << 16) | (value >> 16);
}

static void _spi_buf_prepare(uint8_t bus, size_t len, spi_endianness_t e, spi_word_size_t word_size)
{
    if (e == SPI_LITTLE_ENDIAN || word_size == SPI_32BIT) return;

    size_t count = word_size == SPI_16BIT ? (len + 1) / 2 : (len + 3) / 4;
    uint32_t *data = (uint32_t *)&SPI(bus).W0;
    for (size_t i = 0; i < count; i ++)
    {
        data[i] = word_size == SPI_16BIT
            ? _swap_words(data[i])
            : _swap_bytes(data[i]);
    }
}

static void _spi_buf_transfer(uint8_t bus, const void *out_data, void *in_data,
    size_t len, spi_endianness_t e, spi_word_size_t word_size)
{
    _wait(bus);
    size_t bytes = len * (uint8_t)word_size;
    _set_size(bus, bytes);
    memcpy((void *)&SPI(bus).W0, out_data, bytes);
    _spi_buf_prepare(bus, len, e, word_size);
    _start(bus);
    _wait(bus);
    if (in_data)
    {
        _spi_buf_prepare(bus, len, e, word_size);
        memcpy(in_data, (void *)&SPI(bus).W0, bytes);
    }
}

uint8_t spi_transfer_8(uint8_t bus, uint8_t data)
{
    uint8_t res;
    _spi_buf_transfer(bus, &data, &res, 1, spi_get_endianness(bus), SPI_8BIT);
    return res;
}

uint16_t spi_transfer_16(uint8_t bus, uint16_t data)
{
    uint16_t res;
    _spi_buf_transfer(bus, &data, &res, 1, spi_get_endianness(bus), SPI_16BIT);
    return res;
}

uint32_t spi_transfer_32(uint8_t bus, uint32_t data)
{
    uint32_t res;
    _spi_buf_transfer(bus, &data, &res, 1, spi_get_endianness(bus), SPI_32BIT);
    return res;
}

size_t spi_transfer(uint8_t bus, const void *out_data, void *in_data, size_t len, spi_word_size_t word_size)
{
    if (!out_data || !len) return 0;

    spi_endianness_t e = spi_get_endianness(bus);
    uint8_t buf_size = _SPI_BUF_SIZE / (uint8_t)word_size;

    size_t blocks = len / buf_size;
    for (size_t i = 0; i < blocks; i++)
    {
        size_t offset = i * _SPI_BUF_SIZE;
        _spi_buf_transfer(bus, (const uint8_t *)out_data + offset,
            in_data ? (uint8_t *)in_data + offset : NULL, buf_size, e, word_size);
    }

    uint8_t tail = len % buf_size;
    if (tail)
    {
        _spi_buf_transfer(bus, (const uint8_t *)out_data + blocks * _SPI_BUF_SIZE,
            in_data ? (uint8_t *)in_data + blocks * _SPI_BUF_SIZE : NULL, tail, e, word_size);
    }

    return len;
}
