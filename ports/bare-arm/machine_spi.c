/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 MicroPython contributors
 */

// SPI implementation for bare-arm port (STM32F4)

#include "py/runtime.h"
#include "py/mphal.h"

#define SPI1_BASE 0x40013000
#define SPI2_BASE 0x40003800

#define RCC_APB1ENR (*(volatile uint32_t *)0x40023840)
#define RCC_APB2ENR (*(volatile uint32_t *)0x40023844)

typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SR;
    volatile uint32_t DR;
} SPI_TypeDef;

#define SPI1 ((SPI_TypeDef *)SPI1_BASE)
#define SPI2 ((SPI_TypeDef *)SPI2_BASE)

typedef struct _machine_spi_obj_t {
    mp_obj_base_t base;
    SPI_TypeDef *spi;
    uint8_t spi_id;
} machine_spi_obj_t;

static mp_obj_t machine_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    machine_spi_obj_t *self = mp_obj_malloc(machine_spi_obj_t, type);
    self->spi_id = mp_obj_get_int(args[0]);
    self->spi = (self->spi_id == 1) ? SPI1 : SPI2;

    // Enable SPI clock
    if (self->spi_id == 1) {
        RCC_APB2ENR |= (1 << 12);
    } else {
        RCC_APB1ENR |= (1 << 14);
    }

    // Configure SPI: Master, CPOL=0, CPHA=0, 8-bit, MSB first
    self->spi->CR1 = (1 << 6) | (1 << 2) | (7 << 3);  // SPE | MSTR | BR=/256
    self->spi->CR2 = 0;

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_spi_write(mp_obj_t self_in, mp_obj_t buf_in) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    for (size_t i = 0; i < bufinfo.len; i++) {
        while (!(self->spi->SR & (1 << 1)));  // Wait TXE
        self->spi->DR = ((uint8_t *)bufinfo.buf)[i];
        while (!(self->spi->SR & (1 << 0)));  // Wait RXNE
        (void)self->spi->DR;  // Read to clear
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_spi_write_obj, machine_spi_write);

static const mp_rom_map_elem_t machine_spi_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&machine_spi_write_obj) },
};
static MP_DEFINE_CONST_DICT(machine_spi_locals_dict, machine_spi_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_spi_type,
    MP_QSTR_SPI,
    MP_TYPE_FLAG_NONE,
    make_new, machine_spi_make_new,
    locals_dict, &machine_spi_locals_dict
);
