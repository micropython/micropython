#include <stdio.h>
#include <string.h>

#include "stm32f4xx_hal.h"

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "pin.h"
#include "genhdr/pins.h"
#include "spi.h"

SPI_HandleTypeDef SPIHandle1 = {.Instance = NULL};
SPI_HandleTypeDef SPIHandle2 = {.Instance = NULL};
#if MICROPY_HW_ENABLE_SPI3
SPI_HandleTypeDef SPIHandle3 = {.Instance = NULL};
#endif

void spi_init0(void) {
    // reset the SPI handles
    memset(&SPIHandle1, 0, sizeof(SPI_HandleTypeDef));
    SPIHandle1.Instance = SPI1;
    memset(&SPIHandle2, 0, sizeof(SPI_HandleTypeDef));
    SPIHandle2.Instance = SPI2;
#if MICROPY_HW_ENABLE_SPI3
    memset(&SPIHandle3, 0, sizeof(SPI_HandleTypeDef));
    SPIHandle3.Instance = SPI3;
#endif
}

// TODO allow to take a list of pins to use
void spi_init(SPI_HandleTypeDef *spi) {
    // auto-detect the GPIO pins to use
    const pin_obj_t *pins[4];
    uint32_t af_type;
    if (spi->Instance == SPI1) {
        // X-skin: X5=PA4=SPI1_NSS, X6=PA5=SPI1_SCK, X7=PA6=SPI1_MISO, X8=PA7=SPI1_MOSI
        pins[0] = &pin_A4;
        pins[1] = &pin_A5;
        pins[2] = &pin_A6;
        pins[3] = &pin_A7;
        af_type = GPIO_AF5_SPI1;
    } else if (spi->Instance == SPI2) {
        // Y-skin: Y5=PB12=SPI2_NSS, Y6=PB13=SPI2_SCK, Y7=PB14=SPI2_MISO, Y8=PB15=SPI2_MOSI
        pins[0] = &pin_B12;
        pins[1] = &pin_B13;
        pins[2] = &pin_B14;
        pins[3] = &pin_B15;
        af_type = GPIO_AF5_SPI2;
#if MICROPY_HW_ENABLE_SPI3
    } else if (spi->Instance == SPI3) {
        pins[0] = &pin_A4;
        pins[1] = &pin_B3;
        pins[2] = &pin_B4;
        pins[3] = &pin_B5;
        af_type = GPIO_AF6_SPI3;
#endif
    } else {
        // SPI does not exist for this board
        printf("HardwareError: invalid SPI\n");
        return;
    }

    // init the GPIO lines
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    GPIO_InitStructure.Pull = GPIO_PULLUP; // ST examples use PULLUP
    for (uint i = 0; i < 4; i++) {
        GPIO_InitStructure.Pin = pins[i]->pin_mask;
        GPIO_InitStructure.Alternate = af_type;
        HAL_GPIO_Init(pins[i]->gpio, &GPIO_InitStructure);
    }

    // enable the SPI clock
    if (spi->Instance == SPI1) {
        __SPI1_CLK_ENABLE();
    } else if (spi->Instance == SPI2) {
        __SPI2_CLK_ENABLE();
#if MICROPY_HW_ENABLE_SPI3
    } else {
        __SPI3_CLK_ENABLE();
#endif
    }

    // init the I2C device
    if (HAL_SPI_Init(spi) != HAL_OK) {
        // init error
        // TODO should raise an exception, but this function is not necessarily going to be
        // called via Python, so may not be properly wrapped in an NLR handler
        printf("HardwareError: HAL_SPI_Init failed\n");
        return;
    }
}

void spi_deinit(SPI_HandleTypeDef *spi) {
    HAL_SPI_DeInit(spi);
    if (spi->Instance == SPI1) {
        __SPI1_CLK_DISABLE();
    } else if (spi->Instance == SPI2) {
        __SPI2_CLK_DISABLE();
#if MICROPY_HW_ENABLE_SPI3
    } else {
        __SPI3_CLK_DISABLE();
#endif
    }
}

/******************************************************************************/
/* Micro Python bindings                                                      */

#define PYB_SPI_NUM (2)

typedef struct _pyb_spi_obj_t {
    mp_obj_base_t base;
    SPI_HandleTypeDef *spi;
} pyb_spi_obj_t;

STATIC const pyb_spi_obj_t pyb_spi_obj[PYB_SPI_NUM] = {{{&pyb_spi_type}, &SPIHandle1}, {{&pyb_spi_type}, &SPIHandle2}};

STATIC void pyb_spi_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_spi_obj_t *self = self_in;

    uint spi_num;
    if (self->spi->Instance == SPI1) { spi_num = 1; }
    else if (self->spi->Instance == SPI2) { spi_num = 2; }
    else { spi_num = 3; }

    if (self->spi->State == HAL_SPI_STATE_RESET) {
        print(env, "SPI(%u)", spi_num);
    } else {
        if (self->spi->Init.Mode == SPI_MODE_MASTER) {
            // compute baudrate
            uint spi_clock;
            if (self->spi->Instance == SPI1) {
                // SPI1 is on APB2
                spi_clock = HAL_RCC_GetPCLK2Freq();
            } else {
                // SPI2 and SPI3 are on APB1
                spi_clock = HAL_RCC_GetPCLK1Freq();
            }
            uint baudrate = spi_clock >> ((self->spi->Init.BaudRatePrescaler >> 3) + 1);
            print(env, "SPI(%u, SPI.MASTER, clock=%u, baudrate=%u)", spi_num, spi_clock, baudrate);
        } else {
            print(env, "SPI(%u, SPI.SLAVE)", spi_num);
        }
    }
}

STATIC const mp_arg_parse_t pyb_spi_init_accepted_args[] = {
    { MP_QSTR_mode,     MP_ARG_PARSE_REQUIRED | MP_ARG_PARSE_INT, {.u_int = 0} },
    { MP_QSTR_baudrate, MP_ARG_PARSE_INT,  {.u_int = 328125} },
    { MP_QSTR_clkpol,   MP_ARG_PARSE_KW_ONLY | MP_ARG_PARSE_INT,  {.u_int = SPI_POLARITY_LOW} },
    { MP_QSTR_clkphase, MP_ARG_PARSE_KW_ONLY | MP_ARG_PARSE_INT,  {.u_int = SPI_PHASE_1EDGE} },
    { MP_QSTR_dir,      MP_ARG_PARSE_KW_ONLY | MP_ARG_PARSE_INT,  {.u_int = SPI_DIRECTION_2LINES} },
    { MP_QSTR_size,     MP_ARG_PARSE_KW_ONLY | MP_ARG_PARSE_INT,  {.u_int = 8} },
    { MP_QSTR_nss,      MP_ARG_PARSE_KW_ONLY | MP_ARG_PARSE_INT,  {.u_int = SPI_NSS_SOFT} },
    { MP_QSTR_firstbit, MP_ARG_PARSE_KW_ONLY | MP_ARG_PARSE_INT,  {.u_int = SPI_FIRSTBIT_MSB} },
    { MP_QSTR_ti,       MP_ARG_PARSE_KW_ONLY | MP_ARG_PARSE_BOOL, {.u_bool = false} },
    { MP_QSTR_crcpoly,  MP_ARG_PARSE_KW_ONLY | MP_ARG_PARSE_OBJ,  {.u_obj = mp_const_none} },
};

#define PYB_SPI_INIT_NUM_ARGS (sizeof(pyb_spi_init_accepted_args) / sizeof(pyb_spi_init_accepted_args[0]))

STATIC mp_obj_t pyb_spi_init_helper(const pyb_spi_obj_t *self, uint n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // parse keyword args
    mp_arg_parse_val_t vals[PYB_SPI_INIT_NUM_ARGS];
    mp_arg_parse_all(n_args, args, kw_args, PYB_SPI_INIT_NUM_ARGS, pyb_spi_init_accepted_args, vals);

    // set the SPI configuration values
    SPI_InitTypeDef *init = &self->spi->Init;
    init->Mode = vals[0].u_int;

    // compute the baudrate prescaler from the requested baudrate
    // select a prescaler that yields at most the requested baudrate
    uint spi_clock;
    if (self->spi->Instance == SPI1) {
        // SPI1 is on APB2
        spi_clock = HAL_RCC_GetPCLK2Freq();
    } else {
        // SPI2 and SPI3 are on APB1
        spi_clock = HAL_RCC_GetPCLK1Freq();
    }
    uint br_prescale = spi_clock / vals[1].u_int;
    if (br_prescale <= 2) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2; }
    else if (br_prescale <= 4) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4; }
    else if (br_prescale <= 8) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; }
    else if (br_prescale <= 16) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16; }
    else if (br_prescale <= 32) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32; }
    else if (br_prescale <= 64) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64; }
    else if (br_prescale <= 128) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128; }
    else { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4; }

    init->CLKPolarity = vals[2].u_int;
    init->CLKPhase = vals[3].u_int;
    init->Direction = vals[4].u_int;
    init->DataSize = (vals[5].u_int == 16) ? SPI_DATASIZE_16BIT : SPI_DATASIZE_8BIT;
    init->NSS = vals[6].u_int;
    init->FirstBit = vals[7].u_int;
    init->TIMode = vals[8].u_bool ? SPI_TIMODE_ENABLED : SPI_TIMODE_DISABLED;
    if (vals[9].u_obj == mp_const_none) {
        init->CRCCalculation = SPI_CRCCALCULATION_DISABLED;
        init->CRCPolynomial = 0;
    } else {
        init->CRCCalculation = SPI_CRCCALCULATION_ENABLED;
        init->CRCPolynomial = mp_obj_get_int(vals[9].u_obj);
    }

    // init the SPI bus
    spi_init(self->spi);

    return mp_const_none;
}

STATIC mp_obj_t pyb_spi_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // get SPI number
    machine_int_t spi_id = mp_obj_get_int(args[0]) - 1;

    // check SPI number
    if (!(0 <= spi_id && spi_id < PYB_SPI_NUM)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "SPI bus %d does not exist", spi_id + 1));
    }

    // get SPI object
    const pyb_spi_obj_t *spi_obj = &pyb_spi_obj[spi_id];

    if (n_args > 1 || n_kw > 0) {
        // start the peripheral
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        pyb_spi_init_helper(spi_obj, n_args - 1, args + 1, &kw_args);
    }

    return (mp_obj_t)spi_obj;
}

STATIC mp_obj_t pyb_spi_init(uint n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return pyb_spi_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_spi_init_obj, 1, pyb_spi_init);

STATIC mp_obj_t pyb_spi_deinit(mp_obj_t self_in) {
    pyb_spi_obj_t *self = self_in;
    spi_deinit(self->spi);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_spi_deinit_obj, pyb_spi_deinit);

STATIC mp_obj_t pyb_spi_send(mp_obj_t self_in, mp_obj_t data_in) {
    // TODO assumes transmission size is 8-bits wide
    // TODO accept timeout as keyword argument

    pyb_spi_obj_t *self = self_in;

    uint8_t data[1];
    mp_buffer_info_t bufinfo;
    if (MP_OBJ_IS_INT(data_in)) {
        data[0] = mp_obj_get_int(data_in);
        bufinfo.buf = data;
        bufinfo.len = 1;
        bufinfo.typecode = 'B';
    } else {
        mp_get_buffer_raise(data_in, &bufinfo, MP_BUFFER_READ);
    }

    HAL_StatusTypeDef status = HAL_SPI_Transmit(self->spi, bufinfo.buf, bufinfo.len, 1000);

    if (status != HAL_OK) {
        // TODO really need a HardwareError object, or something
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_Exception, "HAL_SPI_Transmit failed with code %d", status));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_spi_send_obj, pyb_spi_send);

STATIC mp_obj_t pyb_spi_recv(mp_obj_t self_in, mp_obj_t n_in) {
    // TODO assumes transmission size is 8-bits wide
    // TODO accept timeout as keyword argument

    pyb_spi_obj_t *self = self_in;
    machine_uint_t n = mp_obj_get_int(n_in);

    byte *data;
    mp_obj_t o = mp_obj_str_builder_start(&mp_type_bytes, n, &data);
    HAL_StatusTypeDef status = HAL_SPI_Receive(self->spi, data, n, 1000);

    if (status != HAL_OK) {
        // TODO really need a HardwareError object, or something
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_Exception, "HAL_SPI_Receive failed with code %d", status));
    }

    return mp_obj_str_builder_end(o);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_spi_recv_obj, pyb_spi_recv);

STATIC mp_obj_t pyb_spi_send_recv(mp_obj_t self_in, mp_obj_t data_in) {
    // TODO assumes transmission size is 8-bits wide
    // TODO accept timeout as keyword argument

    pyb_spi_obj_t *self = self_in;

    uint8_t data_send[1];
    mp_buffer_info_t bufinfo;
    if (MP_OBJ_IS_INT(data_in)) {
        data_send[0] = mp_obj_get_int(data_in);
        bufinfo.buf = data_send;
        bufinfo.len = 1;
        bufinfo.typecode = 'B';
    } else {
        mp_get_buffer_raise(data_in, &bufinfo, MP_BUFFER_READ);
    }

    byte *data_recv;
    mp_obj_t o = mp_obj_str_builder_start(&mp_type_bytes, bufinfo.len, &data_recv);
    HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(self->spi, bufinfo.buf, data_recv, bufinfo.len, 1000);

    if (status != HAL_OK) {
        // TODO really need a HardwareError object, or something
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_Exception, "HAL_SPI_TransmitReceive failed with code %d", status));
    }

    return mp_obj_str_builder_end(o);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_spi_send_recv_obj, pyb_spi_send_recv);

STATIC const mp_map_elem_t pyb_spi_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_init), (mp_obj_t)&pyb_spi_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit), (mp_obj_t)&pyb_spi_deinit_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send), (mp_obj_t)&pyb_spi_send_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recv), (mp_obj_t)&pyb_spi_recv_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send_recv), (mp_obj_t)&pyb_spi_send_recv_obj },

    // class constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_MASTER),        MP_OBJ_NEW_SMALL_INT(SPI_MODE_MASTER) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SLAVE),        MP_OBJ_NEW_SMALL_INT(SPI_MODE_SLAVE) },
    /* TODO
    { MP_OBJ_NEW_QSTR(MP_QSTR_DIRECTION_2LINES             ((uint32_t)0x00000000)
    { MP_OBJ_NEW_QSTR(MP_QSTR_DIRECTION_2LINES_RXONLY      SPI_CR1_RXONLY
    { MP_OBJ_NEW_QSTR(MP_QSTR_DIRECTION_1LINE              SPI_CR1_BIDIMODE
    { MP_OBJ_NEW_QSTR(MP_QSTR_POLARITY_LOW                ((uint32_t)0x00000000)
    { MP_OBJ_NEW_QSTR(MP_QSTR_POLARITY_HIGH               SPI_CR1_CPOL
    { MP_OBJ_NEW_QSTR(MP_QSTR_PHASE_1EDGE                 ((uint32_t)0x00000000)
    { MP_OBJ_NEW_QSTR(MP_QSTR_PHASE_2EDGE                 SPI_CR1_CPHA
    { MP_OBJ_NEW_QSTR(MP_QSTR_NSS_SOFT                    SPI_CR1_SSM
    { MP_OBJ_NEW_QSTR(MP_QSTR_NSS_HARD_INPUT              ((uint32_t)0x00000000)
    { MP_OBJ_NEW_QSTR(MP_QSTR_NSS_HARD_OUTPUT             ((uint32_t)0x00040000)
    { MP_OBJ_NEW_QSTR(MP_QSTR_FIRSTBIT_MSB                ((uint32_t)0x00000000)
    { MP_OBJ_NEW_QSTR(MP_QSTR_FIRSTBIT_LSB                SPI_CR1_LSBFIRST
    */
};

STATIC MP_DEFINE_CONST_DICT(pyb_spi_locals_dict, pyb_spi_locals_dict_table);

const mp_obj_type_t pyb_spi_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPI,
    .print = pyb_spi_print,
    .make_new = pyb_spi_make_new,
    .locals_dict = (mp_obj_t)&pyb_spi_locals_dict,
};
