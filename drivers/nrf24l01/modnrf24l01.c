
#include <string.h>
#include <stdio.h>
#include "py/runtime.h"
#include "py/obj.h"
#include "py/objstr.h"
#include "py/stream.h"
#include "py/mphal.h"

// nRF24L01+ registers
#define CONFIG      0x00
#define EN_RXADDR   0x02
#define SETUP_AW    0x03
#define SETUP_RETR  0x04
#define RF_CH       0x05
#define RF_SETUP    0x06
#define STATUS      0x07
#define RX_ADDR_P0  0x0a
#define TX_ADDR     0x10
#define RX_PW_P0    0x11
#define FIFO_STATUS 0x17
#define DYNPD	    0x1c

// CONFIG register
#define EN_CRC     0x08 // enable CRC
#define CRCO       0x04 // CRC encoding scheme; 0=1 byte, 1=2 bytes
#define PWR_UP     0x02 // 1=power up, 0=power down
#define PRIM_RX    0x01 // RX/TX control; 0=PTX, 1=PRX

// RF_SETUP register
#define POWER_0    0x00 // -18 dBm
#define POWER_1    0x02 // -12 dBm
#define POWER_2    0x04 // -6 dBm
#define POWER_3    0x06 // 0 dBm
#define SPEED_1M   0x00
#define SPEED_2M   0x08
#define SPEED_250K 0x20

// STATUS register
#define RX_DR      0x40 // RX data ready; write 1 to clear
#define TX_DS      0x20 // TX data sent; write 1 to clear
#define MAX_RT     0x10 // max retransmits reached; write 1 to clear

// FIFO_STATUS register
#define RX_EMPTY   0x01 // 1 if RX FIFO is empty

// constants for instructions
#define R_RX_PL_WID 0x60 // read RX payload width
#define R_RX_PAYLOAD    0x61 // read RX payload
#define W_TX_PAYLOAD    0xa0 // write TX payload
#define FLUSH_TX    0xe1 // flush TX FIFO
#define FLUSH_RX    0xe2 // flush RX FIFO
#define NOP         0xff // use to read STATUS register

extern mp_obj_type_t pyb_spi_type;

typedef struct {
    mp_obj_base_t base;
    mp_obj_t spi;
    mp_obj_t ce;
    mp_obj_t cs;
    mp_int_t channel;
    mp_int_t payload_size;
    mp_int_t speed;
    mp_int_t power;
    mp_int_t crc;
    mp_int_t timeout;
    mp_int_t delay;
    mp_obj_t pipe0_read_addr;
    mp_obj_t barray1;
    mp_obj_t trxbuf;
} nrf24l01_obj_t;

STATIC const mp_obj_type_t nrf42l01_type;

//
// helpers
//
STATIC void _pin_set(mp_obj_t pin, uint8_t v) {
    mp_obj_t meth[2];
    if (v)
        mp_load_method(pin, MP_QSTR_high, meth);
    else
        mp_load_method(pin, MP_QSTR_low, meth);
    mp_call_method_n_kw(0, 0, meth);
}

STATIC void _spi_send_obj_nocs(mp_obj_t spi, mp_obj_t obj) {
    mp_obj_t meth[3];
    mp_load_method(spi, MP_QSTR_send, meth);
    meth[2] = obj;
    mp_call_method_n_kw(1, 0, meth);
}

STATIC void _spi_send_obj(mp_obj_t spi, mp_obj_t cs, mp_obj_t obj) {
    _pin_set(cs, 0);   // CS set low
    _spi_send_obj_nocs(spi, obj);
    _pin_set(cs, 1);   // CS high
}

STATIC void _spi_send_byte(mp_obj_t spi, mp_obj_t cs, uint8_t v) {
    _spi_send_obj(spi, cs, MP_OBJ_NEW_SMALL_INT(v));
}

STATIC mp_obj_t _spi_recv_nocs(mp_obj_t spi, mp_obj_t buf) {
    mp_obj_t meth[3];
    mp_load_method(spi, MP_QSTR_recv, meth);
    meth[2] = buf;
    return mp_call_method_n_kw(1, 0, meth);
}

STATIC const uint8_t _spi_reg_write_obj(nrf24l01_obj_t *nrf, uint8_t reg, mp_obj_t obj) {
    mp_buffer_info_t bufinfo;
    mp_obj_t meth[4];

    _pin_set(nrf->cs, 0);  // CS low

    // spi.send_recv(ba, ba), ba is a 1-size bytearray, value=0x20|reg
    mp_get_buffer_raise(nrf->barray1, &bufinfo, MP_BUFFER_WRITE);
    ((uint8_t*)bufinfo.buf)[0] = 0x20 | reg;

    mp_load_method(nrf->spi, MP_QSTR_send_recv, meth);
    meth[2] = nrf->barray1;
    meth[3] = nrf->barray1;
    mp_obj_t ret = mp_call_method_n_kw(2, 0, meth);

    // check ret[0] for status
    mp_get_buffer_raise(ret, &bufinfo, MP_BUFFER_READ);
    uint8_t status = ((uint8_t*)bufinfo.buf)[0];

    // spi.send(val)
    mp_load_method(nrf->spi, MP_QSTR_send, meth);
    meth[2] = obj;
    mp_call_method_n_kw(1, 0, meth);

    _pin_set(nrf->cs, 1);  // CS high
    return status;
}

STATIC const uint8_t _spi_reg_write_byte(nrf24l01_obj_t *nrf, uint8_t reg, uint8_t val) {
    return _spi_reg_write_obj(nrf, reg, MP_OBJ_NEW_SMALL_INT(val));
}

STATIC const uint8_t _spi_reg_read_byte(nrf24l01_obj_t *nrf, uint8_t reg) {
    mp_buffer_info_t bufinfo;
    mp_obj_t meth[4];

    _pin_set(nrf->cs, 0);  // CS set low
    
    // spi.send_recv(ba, ba)
    mp_get_buffer_raise(nrf->barray1, &bufinfo, MP_BUFFER_WRITE);
    ((uint8_t*)bufinfo.buf)[0] = reg;

    mp_load_method(nrf->spi, MP_QSTR_send_recv, meth);
    meth[2] = nrf->barray1;
    meth[3] = nrf->barray1;
    mp_call_method_n_kw(2, 0, meth);

    // spi.recv(buf)
    mp_load_method(nrf->spi, MP_QSTR_recv, meth);
    meth[2] = nrf->barray1;
    mp_obj_t ret = mp_call_method_n_kw(1, 0, meth);

    // get ret[0]
    mp_get_buffer_raise(ret, &bufinfo, MP_BUFFER_READ);
    uint8_t val = ((uint8_t*)bufinfo.buf)[0];

    _pin_set(nrf->cs, 1);  // CS set high
    return val;
}


//
// methods
//
STATIC void nrf24l01_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    nrf24l01_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_printf(print, "<NRF24L01 channel=%d, payload_size=%d, crc=%d, timeout=%d, delay=%d", \
            self->channel, self->payload_size, self->crc, self->timeout, self->delay);

    mp_printf(print, ", speed=");
    switch(self->speed) {
        case SPEED_250K:
            mp_printf(print, "250K");
            break;
        case SPEED_1M:
            mp_printf(print, "1M");
            break;
        case SPEED_2M:
            mp_printf(print, "2M");
            break;
    }

    mp_printf(print, ", power=");
    switch(self->power) {
        case POWER_0:
            mp_printf(print, "POWER_0");
            break;
        case POWER_1:
            mp_printf(print, "POWER_1");
            break;
        case POWER_2:
            mp_printf(print, "POWER_2");
            break;
        case POWER_3:
            mp_printf(print, "POWER_3");
            break;
    }

    mp_printf(print, ">");
}

STATIC mp_obj_t nrf24l01_close(mp_obj_t self_in) {
    //nrf24l01_obj_t *self = MP_OBJ_TO_PTR(self_in);
    printf("TODO: NRF24L01.close()\n");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(nrf24l01_close_obj, nrf24l01_close);

STATIC mp_obj_t nrf24l01___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return nrf24l01_close(args[0]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(nrf24l01___exit___obj, 4, 4, nrf24l01___exit__);

STATIC mp_obj_t nrf24l01_set_power_speed(mp_obj_t self_in, mp_obj_t power, mp_obj_t speed) {
    mp_int_t pwr, sp;
    if (mp_obj_get_int_maybe(power, &pwr) && (pwr==POWER_0||pwr==POWER_1||pwr==POWER_2||pwr==POWER_3)) {
        if (mp_obj_get_int_maybe(speed, &sp) && (sp==SPEED_250K||sp==SPEED_1M||sp==SPEED_2M)) {
            nrf24l01_obj_t *self = (nrf24l01_obj_t*)self_in;
            if (pwr!=self->power || sp!=self->speed) {
                uint8_t setup = _spi_reg_read_byte(self, RF_SETUP) & 0b11010001;
                _spi_reg_write_byte(self, RF_SETUP, setup|pwr|sp);
                self->power = pwr;
                self->speed = sp;
            }
            return mp_const_none;
        }
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "speed must be one of SPEED_250K, SPEED_1M, SPEED_2M"));
    }
    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "power must be one of POWER_0, POWER_1, POWER_2, POWER_3"));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(nrf24l01_set_power_speed_obj, nrf24l01_set_power_speed);

STATIC mp_obj_t nrf24l01_set_crc(mp_obj_t self_in, mp_obj_t crc) {
    mp_int_t c;
    if (mp_obj_get_int_maybe(crc, &c) && (c==0||c==8||c==16)) {
        nrf24l01_obj_t *self = (nrf24l01_obj_t*) self_in;
        if (c != self->crc) {
            uint8_t config = _spi_reg_read_byte(self, CONFIG) & ~(CRCO | EN_CRC);
            switch(c) {
                case 0:
                    break;
                case 8:
                    config |= EN_CRC;
                    break;
                case 16:
                    config |= EN_CRC | CRCO;
                    break;
            }
            _spi_reg_write_byte(self, CONFIG, config);
            self->crc = c;
        }
        return mp_const_none;
    }
    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "crc must be one of 0, 8, 16"));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(nrf24l01_set_crc_obj, nrf24l01_set_crc);

STATIC mp_obj_t nrf24l01_set_channel(mp_obj_t self_in, mp_obj_t channel) {
    mp_int_t ch;
    if (mp_obj_get_int_maybe(channel, &ch) && ch>=0 && ch<=125) {
        nrf24l01_obj_t *self = (nrf24l01_obj_t*)self_in;
        if (ch != self->channel) {
            _spi_reg_write_byte(self, RF_CH, ch);
            self->channel = ch;
        }
        return mp_const_none;
    }
    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "channel must be an integer between 0 and 125"));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(nrf24l01_set_channel_obj, nrf24l01_set_channel);

STATIC mp_obj_t nrf24l01_set_payload_size(mp_obj_t self_in, mp_obj_t size) {
    mp_int_t sz;
    if (mp_obj_get_int_maybe(size, &sz) && sz>=2 && sz<=32) {
        nrf24l01_obj_t *self = (nrf24l01_obj_t*)self_in;
        if (sz != self->payload_size) {
            self->payload_size = sz;
            if (self->trxbuf) {
                m_del_obj(mp_type_bytearray, self->trxbuf);
            }
            void *m = m_malloc0(sz);
            self->trxbuf = mp_obj_new_bytearray(sz, m);
            m_del(char, m, sz);
        }
        return mp_const_none;
    }
    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "size must be an integer between 2 and 32"));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(nrf24l01_set_payload_size_obj, nrf24l01_set_payload_size);

STATIC mp_obj_t nrf24l01_set_timeout(mp_obj_t self_in, mp_obj_t tm) {
    mp_int_t t;
    if (mp_obj_get_int_maybe(tm, &t)) {
        ((nrf24l01_obj_t*)self_in)->timeout = t<0? -1 : t;
        return mp_const_none;
    }
    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "timeout must be an interger"));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(nrf24l01_set_timeout_obj, nrf24l01_set_timeout);

STATIC mp_obj_t nrf24l01_set_delay(mp_obj_t self_in, mp_obj_t tm) {
    mp_int_t t;
    if (mp_obj_get_int_maybe(tm, &t) && t>=0) {
        ((nrf24l01_obj_t*)self_in)->delay = t;
        return mp_const_none;
    }
    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "delay must be an interger >=0"));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(nrf24l01_set_delay_obj, nrf24l01_set_delay);

STATIC mp_obj_t nrf24l01_flush_rx(mp_obj_t self_in) {
    _spi_send_byte(((nrf24l01_obj_t*)self_in)->spi, ((nrf24l01_obj_t*)self_in)->cs, FLUSH_RX);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(nrf24l01_flush_rx_obj, nrf24l01_flush_rx);

STATIC mp_obj_t nrf24l01_flush_tx(mp_obj_t self_in) {
    _spi_send_byte(((nrf24l01_obj_t*)self_in)->spi, ((nrf24l01_obj_t*)self_in)->cs, FLUSH_TX);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(nrf24l01_flush_tx_obj, nrf24l01_flush_tx);

STATIC mp_obj_t nrf24l01_open_tx_pipe(mp_obj_t self_in, mp_obj_t addr) {
    if (MP_OBJ_IS_STR_OR_BYTES(addr)) {
        GET_STR_LEN(addr, slen);
        if (slen == 5) {
            nrf24l01_obj_t *self = (nrf24l01_obj_t*) self_in;
            _spi_reg_write_obj(self, RX_ADDR_P0, addr);
            _spi_reg_write_obj(self, TX_ADDR, addr);
            _spi_reg_write_byte(self, RX_PW_P0, self->payload_size);
            return mp_const_none;
        }
    }
    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "addr must be a string or bytes with length 5"));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(nrf24l01_open_tx_pipe_obj, nrf24l01_open_tx_pipe);

STATIC mp_obj_t nrf24l01_open_rx_pipe(mp_obj_t self_in, mp_obj_t pipeid, mp_obj_t addr) {
    mp_int_t pid;
    if (mp_obj_get_int_maybe(pipeid, &pid) && pid>=0 && pid<=5) {
        if (MP_OBJ_IS_STR_OR_BYTES(addr)) {
            GET_STR_LEN(addr, slen);
            if (slen == 5) {
                nrf24l01_obj_t *self = (nrf24l01_obj_t*) self_in;
                if (pid==0) {
                    self->pipe0_read_addr = addr;
                }
                if (pid<2) {
                    // 5 bytes addr for pipe # 0,1
                    _spi_reg_write_obj(self, RX_ADDR_P0 + pid, addr);
                } else {
                    // use addr[0] for pipe #2,3,4,5
                    GET_STR_DATA_LEN(addr, data, len);
                    _spi_reg_write_byte(self, RX_ADDR_P0 + pid, data[0]);
                }
                return mp_const_none;
            }
        }
    }
    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "pipeid must be an integer between 0 and 5"));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(nrf24l01_open_rx_pipe_obj, nrf24l01_open_rx_pipe);

STATIC mp_obj_t nrf24l01_start_listening(mp_obj_t self_in) {
    nrf24l01_obj_t *self = (nrf24l01_obj_t*) self_in;

    uint8_t b = _spi_reg_read_byte(self, CONFIG) | PWR_UP | PRIM_RX;
    _spi_reg_write_byte(self, CONFIG, b);
    _spi_reg_write_byte(self, STATUS, RX_DR | TX_DS | MAX_RT);

    if (self->pipe0_read_addr) {
        _spi_reg_write_obj(self, RX_ADDR_P0, self->pipe0_read_addr);
    }

    nrf24l01_flush_rx(self_in);
    nrf24l01_flush_tx(self_in);

    _pin_set(self->ce, 1);  // CE high
    mp_hal_delay_us(130);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(nrf24l01_start_listening_obj, nrf24l01_start_listening);

STATIC mp_obj_t nrf24l01_stop_listening(mp_obj_t self_in) {
    nrf24l01_obj_t *self = (nrf24l01_obj_t*) self_in;
    _pin_set(self->ce, 0);  // CE low
    nrf24l01_flush_rx(self_in);
    nrf24l01_flush_tx(self_in);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(nrf24l01_stop_listening_obj, nrf24l01_stop_listening);

STATIC bool _any(mp_obj_t self_in) {
    // if any data available to recv
    uint8_t v = _spi_reg_read_byte((nrf24l01_obj_t*)self_in, FIFO_STATUS) & RX_EMPTY;
    return v? false : true;
}
STATIC mp_obj_t nrf24l01_any(mp_obj_t self_in) {
    return _any(self_in)? mp_const_true : mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(nrf24l01_any_obj, nrf24l01_any);


//
// stream protocols
//
STATIC void _send_nonblocking(nrf24l01_obj_t *o, mp_obj_t buf) {
    // power up
    uint8_t b = (_spi_reg_read_byte(o, CONFIG) | PWR_UP) & ~PRIM_RX;
    _spi_reg_write_byte(o, CONFIG, b);
    mp_hal_delay_us(150);

    // send the data
    _pin_set(o->cs, 0);

    _spi_send_obj_nocs(o->spi, MP_OBJ_NEW_SMALL_INT(W_TX_PAYLOAD));
    _spi_send_obj_nocs(o->spi, buf);

    _pin_set(o->cs, 1);

    // enable the chip (>10us) so it can send the data
    _pin_set(o->ce, 1);
    mp_hal_delay_us(15);
    _pin_set(o->cs, 0);
}

// check tx status, returns 0=in progress, 1=success, 2=fail
STATIC uint8_t _send_status(nrf24l01_obj_t *o) {
    if (! (_spi_reg_read_byte(o, STATUS) & (TX_DS|MAX_RT)) ) {
        // tx not finished
        return 0;
    }

    // either finished or failed: get and clear status flags, power down
    uint8_t status = _spi_reg_write_byte(o, STATUS, RX_DR|TX_DS|MAX_RT);
    uint8_t b = _spi_reg_read_byte(o, CONFIG) & ~PWR_UP;
    _spi_reg_write_byte(o, CONFIG, b);

    return (status & TX_DS)? 1 : 2;
}

STATIC mp_uint_t stream_p_read(mp_obj_t o_in, void *buf, mp_uint_t size, int *errcode) {
    nrf24l01_obj_t *o = (nrf24l01_obj_t*)o_in;
    if (size < o->payload_size) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "read buf size at least equals to payload_size"));
    }

    bool has_data = false;
    mp_uint_t start = mp_hal_ticks_ms();
    for( ; !has_data && (o->timeout<0 || mp_hal_ticks_ms()-start<o->timeout); ) {
        has_data = _any(o_in);
    }
    if ( !has_data ) {
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    } 

    mp_buffer_info_t bufinfo;
    mp_uint_t max=size/(o->payload_size-1), ttl=0;
    byte *ptr = buf;

    mp_get_buffer_raise(o->trxbuf, &bufinfo, MP_BUFFER_READ);

    for (mp_uint_t cnt=0; has_data && cnt<max; cnt++) {
        // read data
        _pin_set(o->cs, 0);
        _spi_send_obj_nocs(o->spi, MP_OBJ_NEW_SMALL_INT(R_RX_PAYLOAD));
        _spi_recv_nocs(o->spi, o->trxbuf);
        _pin_set(o->cs, 1);

        // clear RX ready flag
        _spi_reg_write_byte(o, STATUS, RX_DR);

        // copy from trxbuf to buf
        uint8_t len = ((uint8_t*)bufinfo.buf)[0];
        if (len==0) break;

        if (len < bufinfo.len) {
            memcpy(ptr, ((byte*)bufinfo.buf)+1, len);
            ptr += len;
            ttl += len;
        }

        // check for more
        has_data = _any(o_in);
    }

    return ttl;
}

STATIC mp_uint_t nrf24l01_write(mp_obj_t o_in, const void *buf, mp_uint_t size, int *errcode) {
    nrf24l01_obj_t *o = MP_OBJ_TO_PTR(o_in);

    // send in packets
    byte* ptr = (byte*) buf;
    mp_uint_t ttl=0;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(o->trxbuf, &bufinfo, MP_BUFFER_WRITE);

    while (size > 0) {
        if (size < o->payload_size) {
            memset(bufinfo.buf, 0, o->payload_size);    // reset to zero
            ((byte*)bufinfo.buf)[0] = size;             // save len to first byte
            memcpy(((byte*)bufinfo.buf)+1, ptr, size); // user data
            ttl += size;                                // add to total size
            size = 0;                                   // all done
        } else {
            ((byte*)bufinfo.buf)[0] = o->payload_size-1; // len=payload_size-1
            memcpy(((byte*)bufinfo.buf)+1, ptr, o->payload_size-1);
            ptr += o->payload_size-1;
            ttl += o->payload_size-1;
            size -= o->payload_size-1;
        }

        _send_nonblocking(o, o->trxbuf);

        if (o->delay > 0)
            mp_hal_delay_ms(o->delay);
    }

    // wait unil success, timeout or fail
    mp_uint_t start = mp_hal_ticks_ms();
    uint8_t st = 0;
    for (; st==0 && (o->timeout<0 || mp_hal_ticks_ms()-start<=o->timeout); ) {
        st = _send_status(o);
    }

    mp_uint_t ret;
    switch (st) {
        case 0:  // in progress
            *errcode = MP_ETIMEDOUT;
            ret = MP_STREAM_ERROR;
            break;
        case 1:  // success
            ret = ttl;
            break;
        case 2:  // fail
            *errcode = MP_EIO;
            ret = MP_STREAM_ERROR;
            break;
    }

    return ret;
}

STATIC mp_uint_t nrf24l01_ioctl(mp_obj_t o_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    mp_uint_t res = 0;
    //nrf24l01_obj_t *o = MP_OBJ_TO_PTR(o_in);
    switch (request) {
        case MP_STREAM_FLUSH:
            nrf24l01_flush_rx(o_in);
            nrf24l01_flush_tx(o_in);
            break;
        default:
            printf("NRF24L01.ioctl(%d) not supported\n", request);
            *errcode = MP_EOPNOTSUPP;  // not supported
            res = MP_STREAM_ERROR;
    }
    return res;
}


//
// constructor (SPI, cs=Pin, ce=Pin, channel=0, payload_size=16)
//
STATIC mp_obj_t nrf24l01_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 5, true);

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_cs,           MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_ce,           MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_channel,      MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_payload_size, MP_ARG_INT, {.u_int = 16} },
        { MP_QSTR_timeout,      MP_ARG_INT, {.u_int = 500} },
    };
    mp_arg_val_t pargs[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args-1, args+1, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, pargs);

    if (!MP_OBJ_IS_TYPE(args[0], &pyb_spi_type)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "argument 1 must be an SPI object"));
    }
    if (!MP_OBJ_IS_TYPE(pargs[0].u_obj, &pin_type)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "argument cs must be a Pin object"));
    }
    if (!MP_OBJ_IS_TYPE(pargs[1].u_obj, &pin_type)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "argument ce must be a Pin object"));
    }

    nrf24l01_obj_t *o = m_new_obj(nrf24l01_obj_t);

    o->base.type = &nrf42l01_type;
    o->spi = args[0];
    o->cs = pargs[0].u_obj;
    o->ce = pargs[1].u_obj;
    o->channel = o->crc = o->power = o->speed = o->payload_size = -1;
    o->delay = 50;
    o->trxbuf = NULL;

    // pre-alloc bytearray(1) for spi recv(1)
    const mp_obj_t bargs[1] = { MP_OBJ_NEW_SMALL_INT(1) };
    o->barray1 = mp_type_bytearray.make_new(&mp_type_bytearray, 1, 0, bargs);

    nrf24l01_set_payload_size(o, MP_OBJ_NEW_SMALL_INT(pargs[3].u_int));
    nrf24l01_set_timeout(o, MP_OBJ_NEW_SMALL_INT(pargs[4].u_int));
    
    mp_obj_t meth[11];

    // spi.init(MASTER, baudrate=4000000, polarity=0, phase=0, firstbit=MSB)
    mp_load_method(o->spi, MP_QSTR_init, meth);
    meth[2] = MP_OBJ_NEW_SMALL_INT(SPI_MODE_MASTER);
    meth[3] = MP_OBJ_NEW_QSTR(MP_QSTR_baudrate);
    meth[4] = mp_obj_new_int_from_ull(4000000);
    meth[5] = MP_OBJ_NEW_QSTR(MP_QSTR_polarity);
    meth[6] = MP_OBJ_NEW_SMALL_INT(0);
    meth[7] = MP_OBJ_NEW_QSTR(MP_QSTR_phase);
    meth[8] = MP_OBJ_NEW_SMALL_INT(0);
    meth[9] = MP_OBJ_NEW_QSTR(MP_QSTR_firstbit);
    meth[10] = MP_OBJ_NEW_SMALL_INT(SPI_FIRSTBIT_MSB);
    mp_call_method_n_kw(1, 4, meth);

    // cs.init(OUTPUT_PP, PULL_NONE)
    mp_load_method(o->cs, MP_QSTR_init, meth);
    meth[2] = MP_OBJ_NEW_SMALL_INT(GPIO_MODE_OUTPUT_PP);
    meth[3] = MP_OBJ_NEW_SMALL_INT(GPIO_NOPULL);
    mp_call_method_n_kw(2, 0, meth);

    // ce.init(OUTPUT_PP, PULL_NONE)
    mp_load_method(o->ce, MP_QSTR_init, meth);
    meth[2] = MP_OBJ_NEW_SMALL_INT(GPIO_MODE_OUTPUT_PP);
    meth[3] = MP_OBJ_NEW_SMALL_INT(GPIO_NOPULL);
    mp_call_method_n_kw(2, 0, meth);

    // reset everything
    _pin_set(o->ce, 0);     // CE low
    _pin_set(o->cs, 0);     // CS low
    o->pipe0_read_addr = NULL;
    mp_hal_delay_ms(5);

    // set addr width to 5 bytes and check for device present
    _spi_reg_write_byte(o, SETUP_AW, 0b11);
    if (_spi_reg_read_byte(o, SETUP_AW) != 0b11) {
        mp_raise_msg(&mp_type_OSError, "nRF24L01+ Hardware not responding");
    }

    // disable dynamic payloads
    _spi_reg_write_byte(o, DYNPD, 0);

    // auto retransmit delay: 1750us
    // auto retransmit count: 8
    _spi_reg_write_byte(o, SETUP_RETR, (6 << 4) | 8);

    // set rf power and speed
    nrf24l01_set_power_speed(o, MP_OBJ_NEW_SMALL_INT(POWER_3), MP_OBJ_NEW_SMALL_INT(SPEED_250K));

    // set channel
    nrf24l01_set_channel(o, MP_OBJ_NEW_SMALL_INT(pargs[2].u_int));

    // init CRC 16
    nrf24l01_set_crc(o, MP_OBJ_NEW_SMALL_INT(16));

    // clear status flags
    _spi_reg_write_byte(o, STATUS, RX_DR | TX_DS | MAX_RT);

    // flush buffers
    nrf24l01_flush_rx(o);
    nrf24l01_flush_tx(o);

    return MP_OBJ_FROM_PTR(o);
}

STATIC const mp_rom_map_elem_t nrf24l01_locals_dict_table[] = {
    // class constants
    { MP_ROM_QSTR(MP_QSTR_SPEED_250K), MP_OBJ_NEW_SMALL_INT(SPEED_250K) },
    { MP_ROM_QSTR(MP_QSTR_SPEED_1M), MP_OBJ_NEW_SMALL_INT(SPEED_1M) },
    { MP_ROM_QSTR(MP_QSTR_SPEED_2M), MP_OBJ_NEW_SMALL_INT(SPEED_2M) },
    { MP_ROM_QSTR(MP_QSTR_POWER_0), MP_OBJ_NEW_SMALL_INT(POWER_0) },
    { MP_ROM_QSTR(MP_QSTR_POWER_1), MP_OBJ_NEW_SMALL_INT(POWER_1) },
    { MP_ROM_QSTR(MP_QSTR_POWER_2), MP_OBJ_NEW_SMALL_INT(POWER_2) },
    { MP_ROM_QSTR(MP_QSTR_POWER_3), MP_OBJ_NEW_SMALL_INT(POWER_3) },
    
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_set_power_speed), (mp_obj_t)&nrf24l01_set_power_speed_obj },
    { MP_ROM_QSTR(MP_QSTR_set_channel), (mp_obj_t)&nrf24l01_set_channel_obj },
    { MP_ROM_QSTR(MP_QSTR_set_payload_size), (mp_obj_t)&nrf24l01_set_payload_size_obj },
    { MP_ROM_QSTR(MP_QSTR_set_crc), (mp_obj_t)&nrf24l01_set_crc_obj },
    { MP_ROM_QSTR(MP_QSTR_set_timeout), (mp_obj_t)&nrf24l01_set_timeout_obj },
    { MP_ROM_QSTR(MP_QSTR_set_delay), (mp_obj_t)&nrf24l01_set_delay_obj },
    { MP_ROM_QSTR(MP_QSTR_flush_rx), (mp_obj_t)&nrf24l01_flush_rx_obj },
    { MP_ROM_QSTR(MP_QSTR_flush_tx), (mp_obj_t)&nrf24l01_flush_tx_obj },
    { MP_ROM_QSTR(MP_QSTR_open_tx_pipe), (mp_obj_t)&nrf24l01_open_tx_pipe_obj },
    { MP_ROM_QSTR(MP_QSTR_open_rx_pipe), (mp_obj_t)&nrf24l01_open_rx_pipe_obj },
    { MP_ROM_QSTR(MP_QSTR_start_listening), (mp_obj_t)&nrf24l01_start_listening_obj },
    { MP_ROM_QSTR(MP_QSTR_stop_listening), (mp_obj_t)&nrf24l01_stop_listening_obj },
    { MP_ROM_QSTR(MP_QSTR_any), (mp_obj_t)&nrf24l01_any_obj },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&nrf24l01_close_obj) },

    // stream ops
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    //{ MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&mp_stream_flush_obj) },
    //{ MP_ROM_QSTR(MP_QSTR_seek), MP_ROM_PTR(&mp_stream_seek_obj) },

    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&nrf24l01___exit___obj) },
};

STATIC MP_DEFINE_CONST_DICT(nrf24l01_locals_dict, nrf24l01_locals_dict_table);

STATIC const mp_stream_p_t nrf24l01_stream_p = {
    .read = stream_p_read,
    .write = nrf24l01_write,
    .ioctl = nrf24l01_ioctl,
    .is_text = 0,
};

STATIC const mp_obj_type_t nrf42l01_type = {
    { &mp_type_type },
    .name = MP_QSTR_NRF24L01,
    .make_new = nrf24l01_make_new,
    .print = nrf24l01_print,
    .protocol = &nrf24l01_stream_p,
    .locals_dict = (mp_obj_dict_t*)&nrf24l01_locals_dict,
};

STATIC const mp_rom_map_elem_t mp_module_nrf24l01_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_nrf24l01) },
    { MP_ROM_QSTR(MP_QSTR_NRF24L01), MP_ROM_PTR(&nrf42l01_type) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_nrf24l01_globals, mp_module_nrf24l01_globals_table);

const mp_obj_module_t mp_module_nrf24l01 = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_nrf24l01_globals,
};

