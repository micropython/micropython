// machine_spi_port.c - machine.SPI (hardware) for GD32F4xx

#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"

#include "gd32f4xx.h"
#include "pin_defs.h"

typedef struct _machine_spi_obj_t {
    mp_obj_base_t base;
    uint32_t spi;
    uint32_t rcu;
    uint8_t  idx;
    uint32_t baudrate;
    uint8_t  polarity;
    uint8_t  phase;
    uint8_t  bits;
    uint8_t  firstbit;
    int16_t  sck_pin;
    int16_t  mosi_pin;
    int16_t  miso_pin;
} machine_spi_obj_t;

static const struct {
    uint32_t spi;
    uint32_t rcu;
} spi_hw[] = {
    { SPI0, RCU_SPI0 },
    { SPI1, RCU_SPI1 },
    { SPI2, RCU_SPI2 },
    #ifdef SPI3
    { SPI3, RCU_SPI3 },
    #endif
    #ifdef SPI4
    { SPI4, RCU_SPI4 },
    #endif
    #ifdef SPI5
    { SPI5, RCU_SPI5 },
    #endif
};

#define NUM_SPI MP_ARRAY_SIZE(spi_hw)

static machine_spi_obj_t machine_spi_obj[NUM_SPI];

static uint32_t spi_calc_psc(uint32_t pclk, uint32_t baudrate) {
    uint32_t div = 2;
    uint32_t psc = 0;
    while (div < 256) {
        if (baudrate && (pclk / div <= baudrate)) {
            return psc;
        }
        div <<= 1;
        psc++;
    }
    return 7;
}

static void spi_cfg_pin(int16_t enc_pin, uint32_t af, uint32_t pupd) {
    if (enc_pin < 0) {
        return;
    }
    uint8_t port = PIN_PORT((uint8_t)enc_pin);
    uint8_t num = PIN_NUM((uint8_t)enc_pin);
    uint32_t base = pin_port_base(port);
    uint32_t mask = pin_hal_mask(num);

    rcu_periph_clock_enable(pin_port_rcu(port));
    gpio_af_set(base, af, mask);
    gpio_mode_set(base, GPIO_MODE_AF, pupd, mask);
    gpio_output_options_set(base, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, mask);
}

static void spi_apply_config(machine_spi_obj_t *self) {
    spi_parameter_struct cfg;
    spi_struct_para_init(&cfg);
    cfg.device_mode = SPI_MASTER;
    cfg.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    cfg.frame_size = (self->bits == 16) ? SPI_FRAMESIZE_16BIT : SPI_FRAMESIZE_8BIT;
    cfg.nss = SPI_NSS_SOFT;
    cfg.endian = (self->firstbit == MICROPY_PY_MACHINE_SPI_LSB) ? SPI_ENDIAN_LSB : SPI_ENDIAN_MSB;
    if (self->polarity) {
        cfg.clock_polarity_phase = self->phase ? SPI_CK_PL_HIGH_PH_2EDGE : SPI_CK_PL_HIGH_PH_1EDGE;
    } else {
        cfg.clock_polarity_phase = self->phase ? SPI_CK_PL_LOW_PH_2EDGE : SPI_CK_PL_LOW_PH_1EDGE;
    }
    cfg.prescale = (uint32_t)spi_calc_psc(rcu_clock_freq_get(CK_APB2), self->baudrate);

    spi_i2s_deinit(self->spi);
    spi_init(self->spi, &cfg);
    spi_nss_internal_high(self->spi);
    spi_enable(self->spi);
}

// ---- protocol callbacks ----
static void mp_machine_spi_init(mp_obj_base_t *s, size_t n_args,
                                const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_spi_obj_t *self = (machine_spi_obj_t *)s;
    enum {
        ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit,
        ARG_sck, ARG_mosi, ARG_miso, ARG_nss,
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = 1000000} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = MICROPY_PY_MACHINE_SPI_MSB} },
        { MP_QSTR_sck,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_mosi,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_miso,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_nss,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->baudrate = args[ARG_baudrate].u_int;
    self->polarity = (uint8_t)args[ARG_polarity].u_int;
    self->phase = (uint8_t)args[ARG_phase].u_int;
    self->bits = (uint8_t)args[ARG_bits].u_int;
    self->firstbit = (uint8_t)args[ARG_firstbit].u_int;

    rcu_periph_clock_enable(self->rcu);

    // pins may be supplied as machine.Pin objects; accept int encoding too
    if (args[ARG_sck].u_obj != mp_const_none) {
        self->sck_pin = (int16_t)mp_obj_get_int(args[ARG_sck].u_obj);
    }
    if (args[ARG_mosi].u_obj != mp_const_none) {
        self->mosi_pin = (int16_t)mp_obj_get_int(args[ARG_mosi].u_obj);
    }
    if (args[ARG_miso].u_obj != mp_const_none) {
        self->miso_pin = (int16_t)mp_obj_get_int(args[ARG_miso].u_obj);
    }

    spi_cfg_pin(self->sck_pin, GPIO_AF_5, GPIO_PUPD_NONE);
    spi_cfg_pin(self->mosi_pin, GPIO_AF_5, GPIO_PUPD_NONE);
    spi_cfg_pin(self->miso_pin, GPIO_AF_5, GPIO_PUPD_PULLUP);

    spi_apply_config(self);
}

static void mp_machine_spi_deinit(mp_obj_base_t *s) {
    machine_spi_obj_t *self = (machine_spi_obj_t *)s;
    spi_disable(self->spi);
    spi_i2s_deinit(self->spi);
    rcu_periph_clock_disable(self->rcu);
}

static void mp_machine_spi_transfer(mp_obj_base_t *s, size_t len,
                                    const uint8_t *src, uint8_t *dest) {
    machine_spi_obj_t *self = (machine_spi_obj_t *)s;
    bool wide = (self->bits == 16);
    const uint16_t *src16 = (const uint16_t *)src;
    uint16_t *dest16 = (uint16_t *)dest;

    for (size_t i = 0; i < len; i++) {
        uint16_t tx;
        if (src == NULL) {
            tx = 0xffff;
        } else if (wide) {
            tx = src16[i];
        } else {
            tx = src[i];
        }

        while (spi_i2s_flag_get(self->spi, SPI_FLAG_TBE) == RESET) {
            ;
        }
        spi_i2s_data_transmit(self->spi, tx);

        while (spi_i2s_flag_get(self->spi, SPI_FLAG_RBNE) == RESET) {
            ;
        }
        uint16_t rx = (uint16_t)spi_i2s_data_receive(self->spi);

        if (dest != NULL) {
            if (wide) {
                dest16[i] = rx;
            } else {
                dest[i] = (uint8_t)rx;
            }
        }
    }
}

static const mp_machine_spi_p_t machine_spi_p = {
    .init = mp_machine_spi_init,
    .deinit = mp_machine_spi_deinit,
    .transfer = mp_machine_spi_transfer,
};

static mp_obj_t machine_spi_make_new(const mp_obj_type_t *type, size_t n_args,
                                     size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);
    mp_int_t spi_id = mp_obj_get_int(all_args[0]);
    if (spi_id < 0 || (mp_uint_t)spi_id >= NUM_SPI) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid SPI bus"));
    }

    machine_spi_obj_t *self = &machine_spi_obj[spi_id];
    self->base.type = &machine_spi_type;
    self->spi = spi_hw[spi_id].spi;
    self->rcu = spi_hw[spi_id].rcu;
    self->idx = (uint8_t)spi_id;
    self->sck_pin = -1;
    self->mosi_pin = -1;
    self->miso_pin = -1;

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    mp_machine_spi_init(&self->base, n_args - 1, all_args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

MP_DEFINE_CONST_OBJ_TYPE(
    machine_spi_type,
    MP_QSTR_SPI,
    MP_TYPE_FLAG_NONE,
    make_new, machine_spi_make_new,
    protocol, &machine_spi_p,
    locals_dict, &mp_machine_spi_locals_dict
    );
