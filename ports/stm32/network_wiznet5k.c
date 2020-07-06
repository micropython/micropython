/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2018 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <string.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "spi.h"
#include "modnetwork.h"

#if MICROPY_PY_WIZNET5K && MICROPY_PY_LWIP

#include "lib/netutils/netutils.h"
#include "drivers/wiznet5k/ethernet/socket.h"
#include "lwip/err.h"
#include "lwip/dns.h"
#include "lwip/dhcp.h"
#include "netif/etharp.h"

#define TRACE_ETH_TX (0x0002)
#define TRACE_ETH_RX (0x0004)

/*******************************************************************************/
// Wiznet5k Ethernet driver in MACRAW mode

typedef struct _wiznet5k_obj_t {
    mp_obj_base_t base;
    mp_uint_t cris_state;
    const spi_t *spi;
    mp_hal_pin_obj_t cs;
    mp_hal_pin_obj_t rst;
    uint8_t eth_frame[1514];
    uint32_t trace_flags;
    struct netif netif;
    struct dhcp dhcp_struct;
} wiznet5k_obj_t;

// Global object holding the Wiznet5k state
STATIC wiznet5k_obj_t wiznet5k_obj;

STATIC void wiznet5k_lwip_init(wiznet5k_obj_t *self);

STATIC void wiz_cris_enter(void) {
    wiznet5k_obj.cris_state = MICROPY_BEGIN_ATOMIC_SECTION();
}

STATIC void wiz_cris_exit(void) {
    MICROPY_END_ATOMIC_SECTION(wiznet5k_obj.cris_state);
}

STATIC void wiz_cs_select(void) {
    mp_hal_pin_low(wiznet5k_obj.cs);
}

STATIC void wiz_cs_deselect(void) {
    mp_hal_pin_high(wiznet5k_obj.cs);
}

STATIC void wiz_spi_read(uint8_t *buf, uint32_t len) {
    HAL_StatusTypeDef status = HAL_SPI_Receive(wiznet5k_obj.spi->spi, buf, len, 5000);
    (void)status;
}

STATIC void wiz_spi_write(const uint8_t *buf, uint32_t len) {
    HAL_StatusTypeDef status = HAL_SPI_Transmit(wiznet5k_obj.spi->spi, (uint8_t *)buf, len, 5000);
    (void)status;
}

STATIC void wiznet5k_init(void) {
    // SPI configuration
    SPI_InitTypeDef *init = &wiznet5k_obj.spi->spi->Init;
    init->Mode = SPI_MODE_MASTER;
    init->Direction = SPI_DIRECTION_2LINES;
    init->DataSize = SPI_DATASIZE_8BIT;
    init->CLKPolarity = SPI_POLARITY_LOW; // clock is low when idle
    init->CLKPhase = SPI_PHASE_1EDGE; // data latched on first edge, which is rising edge for low-idle
    init->NSS = SPI_NSS_SOFT;
    init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2; // clock freq = f_PCLK / this_prescale_value; Wiz820i can do up to 80MHz
    init->FirstBit = SPI_FIRSTBIT_MSB;
    init->TIMode = SPI_TIMODE_DISABLED;
    init->CRCCalculation = SPI_CRCCALCULATION_DISABLED;
    init->CRCPolynomial = 7; // unused
    spi_init(wiznet5k_obj.spi, false);

    mp_hal_pin_output(wiznet5k_obj.cs);
    mp_hal_pin_output(wiznet5k_obj.rst);

    // Reset the chip
    mp_hal_pin_low(wiznet5k_obj.rst);
    mp_hal_delay_ms(1); // datasheet says 2us
    mp_hal_pin_high(wiznet5k_obj.rst);
    mp_hal_delay_ms(150); // datasheet says 150ms

    // Set physical interface callbacks
    reg_wizchip_cris_cbfunc(wiz_cris_enter, wiz_cris_exit);
    reg_wizchip_cs_cbfunc(wiz_cs_select, wiz_cs_deselect);
    reg_wizchip_spi_cbfunc(wiz_spi_read, wiz_spi_write);

    // Configure 16k buffers for fast MACRAW
    uint8_t sn_size[16] = {16, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0};
    ctlwizchip(CW_INIT_WIZCHIP, sn_size);

    // Seems we need a small delay after init
    mp_hal_delay_ms(250);

    // If the device doesn't have a MAC address then set one
    uint8_t mac[6];
    getSHAR(mac);
    if ((mac[0] | mac[1] | mac[2] | mac[3] | mac[4] | mac[5]) == 0) {
        mp_hal_get_mac(MP_HAL_MAC_ETH0, mac);
        setSHAR(mac);
    }

    // Hook the Wiznet into lwIP
    wiznet5k_lwip_init(&wiznet5k_obj);
}

STATIC void wiznet5k_deinit(void) {
    for (struct netif *netif = netif_list; netif != NULL; netif = netif->next) {
        if (netif == &wiznet5k_obj.netif) {
            netif_remove(netif);
            netif->flags = 0;
            break;
        }
    }
}

STATIC void wiznet5k_get_mac_address(wiznet5k_obj_t *self, uint8_t mac[6]) {
    (void)self;
    getSHAR(mac);
}

STATIC void wiznet5k_send_ethernet(wiznet5k_obj_t *self, size_t len, const uint8_t *buf) {
    uint8_t ip[4] = {1, 1, 1, 1}; // dummy
    int ret = WIZCHIP_EXPORT(sendto)(0, (byte *)buf, len, ip, 11); // dummy port
    if (ret != len) {
        printf("wiznet5k_send_ethernet: fatal error %d\n", ret);
        netif_set_link_down(&self->netif);
        netif_set_down(&self->netif);
    }
}

// Stores the frame in self->eth_frame and returns number of bytes in the frame, 0 for no frame
STATIC uint16_t wiznet5k_recv_ethernet(wiznet5k_obj_t *self) {
    uint16_t len = getSn_RX_RSR(0);
    if (len == 0) {
        return 0;
    }

    byte ip[4];
    uint16_t port;
    int ret = WIZCHIP_EXPORT(recvfrom)(0, self->eth_frame, 1514, ip, &port);
    if (ret <= 0) {
        printf("wiznet5k_poll: fatal error len=%u ret=%d\n", len, ret);
        netif_set_link_down(&self->netif);
        netif_set_down(&self->netif);
        return 0;
    }

    return ret;
}

/*******************************************************************************/
// Wiznet5k lwIP interface

STATIC err_t wiznet5k_netif_output(struct netif *netif, struct pbuf *p) {
    wiznet5k_obj_t *self = netif->state;
    pbuf_copy_partial(p, self->eth_frame, p->tot_len, 0);
    if (self->trace_flags & TRACE_ETH_TX) {
        netutils_ethernet_trace(MP_PYTHON_PRINTER, p->tot_len, self->eth_frame, NETUTILS_TRACE_IS_TX | NETUTILS_TRACE_NEWLINE);
    }
    wiznet5k_send_ethernet(self, p->tot_len, self->eth_frame);
    return ERR_OK;
}

STATIC err_t wiznet5k_netif_init(struct netif *netif) {
    netif->linkoutput = wiznet5k_netif_output;
    netif->output = etharp_output;
    netif->mtu = 1500;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP;
    wiznet5k_get_mac_address(netif->state, netif->hwaddr);
    netif->hwaddr_len = sizeof(netif->hwaddr);
    int ret = WIZCHIP_EXPORT(socket)(0, Sn_MR_MACRAW, 0, 0);
    if (ret != 0) {
        printf("WIZNET fatal error in netifinit: %d\n", ret);
        return ERR_IF;
    }

    // Enable MAC filtering so we only get frames destined for us, to reduce load on lwIP
    setSn_MR(0, getSn_MR(0) | Sn_MR_MFEN);

    return ERR_OK;
}

STATIC void wiznet5k_lwip_init(wiznet5k_obj_t *self) {
    ip_addr_t ipconfig[4];
    ipconfig[0].addr = 0;
    ipconfig[1].addr = 0;
    ipconfig[2].addr = 0;
    ipconfig[3].addr = 0;
    netif_add(&self->netif, &ipconfig[0], &ipconfig[1], &ipconfig[2], self, wiznet5k_netif_init, ethernet_input);
    self->netif.name[0] = 'e';
    self->netif.name[1] = '0';
    netif_set_default(&self->netif);
    dns_setserver(0, &ipconfig[3]);
    dhcp_set_struct(&self->netif, &self->dhcp_struct);
    // Setting NETIF_FLAG_UP then clearing it is a workaround for dhcp_start and the
    // LWIP_DHCP_CHECK_LINK_UP option, so that the DHCP client schedules itself to
    // automatically start when the interface later goes up.
    self->netif.flags |= NETIF_FLAG_UP;
    dhcp_start(&self->netif);
    self->netif.flags &= ~NETIF_FLAG_UP;
}

void wiznet5k_poll(void) {
    wiznet5k_obj_t *self = &wiznet5k_obj;
    if (!(self->netif.flags & NETIF_FLAG_LINK_UP)) {
        return;
    }
    uint16_t len;
    while ((len = wiznet5k_recv_ethernet(self)) > 0) {
        if (self->trace_flags & TRACE_ETH_RX) {
            netutils_ethernet_trace(MP_PYTHON_PRINTER, len, self->eth_frame, NETUTILS_TRACE_NEWLINE);
        }
        struct pbuf *p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
        if (p != NULL) {
            pbuf_take(p, self->eth_frame, len);
            if (self->netif.input(p, &self->netif) != ERR_OK) {
                pbuf_free(p);
            }
        }
    }
}

/*******************************************************************************/
// MicroPython bindings

// WIZNET5K([spi, pin_cs, pin_rst])
STATIC mp_obj_t wiznet5k_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 3, 3, false);

    const spi_t *spi = spi_from_mp_obj(args[0]);
    mp_hal_pin_obj_t cs = pin_find(args[1]);
    mp_hal_pin_obj_t rst = pin_find(args[2]);

    // Access the existing object, if it has been constructed with the same hardware interface
    if (wiznet5k_obj.base.type == &mod_network_nic_type_wiznet5k) {
        if (!(wiznet5k_obj.spi == spi && wiznet5k_obj.cs == cs && wiznet5k_obj.rst == rst
              && wiznet5k_obj.netif.flags != 0)) {
            wiznet5k_deinit();
        }
    }

    // Init the wiznet5k object
    wiznet5k_obj.base.type = &mod_network_nic_type_wiznet5k;
    wiznet5k_obj.cris_state = 0;
    wiznet5k_obj.spi = spi;
    wiznet5k_obj.cs = cs;
    wiznet5k_obj.rst = rst;
    wiznet5k_obj.trace_flags = 0;

    // Return wiznet5k object
    return MP_OBJ_FROM_PTR(&wiznet5k_obj);
}

STATIC mp_obj_t wiznet5k_regs(mp_obj_t self_in) {
    (void)self_in;
    printf("Wiz CREG:");
    for (int i = 0; i < 0x50; ++i) {
        if (i % 16 == 0) {
            printf("\n  %04x:", i);
        }
        #if MICROPY_PY_WIZNET5K == 5200
        uint32_t reg = i;
        #else
        uint32_t reg = _W5500_IO_BASE_ | i << 8;
        #endif
        printf(" %02x", WIZCHIP_READ(reg));
    }
    for (int sn = 0; sn < 4; ++sn) {
        printf("\nWiz SREG[%d]:", sn);
        for (int i = 0; i < 0x30; ++i) {
            if (i % 16 == 0) {
                printf("\n  %04x:", i);
            }
            #if MICROPY_PY_WIZNET5K == 5200
            uint32_t reg = WIZCHIP_SREG_ADDR(sn, i);
            #else
            uint32_t reg = _W5500_IO_BASE_ | i << 8 | WIZCHIP_SREG_BLOCK(sn) << 3;
            #endif
            printf(" %02x", WIZCHIP_READ(reg));
        }
    }
    printf("\n");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wiznet5k_regs_obj, wiznet5k_regs);

STATIC mp_obj_t wiznet5k_isconnected(mp_obj_t self_in) {
    wiznet5k_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(
        wizphy_getphylink() == PHY_LINK_ON
        && (self->netif.flags & NETIF_FLAG_UP)
        && self->netif.ip_addr.addr != 0
        );
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wiznet5k_isconnected_obj, wiznet5k_isconnected);

STATIC mp_obj_t wiznet5k_active(size_t n_args, const mp_obj_t *args) {
    wiznet5k_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        return mp_obj_new_bool(self->netif.flags & NETIF_FLAG_UP);
    } else {
        if (mp_obj_is_true(args[1])) {
            if (!(self->netif.flags & NETIF_FLAG_UP)) {
                wiznet5k_init();
                netif_set_link_up(&self->netif);
                netif_set_up(&self->netif);
            }
        } else {
            if (self->netif.flags & NETIF_FLAG_UP) {
                netif_set_down(&self->netif);
                netif_set_link_down(&self->netif);
                wiznet5k_deinit();
            }
        }
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wiznet5k_active_obj, 1, 2, wiznet5k_active);

STATIC mp_obj_t wiznet5k_ifconfig(size_t n_args, const mp_obj_t *args) {
    wiznet5k_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    return mod_network_nic_ifconfig(&self->netif, n_args - 1, args + 1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wiznet5k_ifconfig_obj, 1, 2, wiznet5k_ifconfig);

STATIC mp_obj_t wiznet5k_status(size_t n_args, const mp_obj_t *args) {
    wiznet5k_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    (void)self;

    if (n_args == 1) {
        // No arguments: return link status
        if (self->netif.flags && wizphy_getphylink() == PHY_LINK_ON) {
            if ((self->netif.flags & NETIF_FLAG_UP) && self->netif.ip_addr.addr != 0) {
                return MP_OBJ_NEW_SMALL_INT(2);
            } else {
                return MP_OBJ_NEW_SMALL_INT(1);
            }
        } else {
            return MP_OBJ_NEW_SMALL_INT(0);
        }
    }

    mp_raise_ValueError(MP_ERROR_TEXT("unknown config param"));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wiznet5k_status_obj, 1, 2, wiznet5k_status);

STATIC mp_obj_t wiznet5k_config(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    wiznet5k_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (kwargs->used == 0) {
        // Get config value
        if (n_args != 2) {
            mp_raise_TypeError(MP_ERROR_TEXT("must query one param"));
        }

        switch (mp_obj_str_get_qstr(args[1])) {
            case MP_QSTR_mac: {
                uint8_t buf[6];
                wiznet5k_get_mac_address(self, buf);
                return mp_obj_new_bytes(buf, 6);
            }
            default:
                mp_raise_ValueError(MP_ERROR_TEXT("unknown config param"));
        }
    } else {
        // Set config value(s)
        if (n_args != 1) {
            mp_raise_TypeError(MP_ERROR_TEXT("can't specify pos and kw args"));
        }

        for (size_t i = 0; i < kwargs->alloc; ++i) {
            if (MP_MAP_SLOT_IS_FILLED(kwargs, i)) {
                mp_map_elem_t *e = &kwargs->table[i];
                switch (mp_obj_str_get_qstr(e->key)) {
                    case MP_QSTR_mac: {
                        mp_buffer_info_t buf;
                        mp_get_buffer_raise(e->value, &buf, MP_BUFFER_READ);
                        if (buf.len != 6) {
                            mp_raise_ValueError(NULL);
                        }
                        setSHAR(buf.buf);
                        memcpy(self->netif.hwaddr, buf.buf, 6);
                        break;
                    }
                    case MP_QSTR_trace: {
                        self->trace_flags = mp_obj_get_int(e->value);
                        break;
                    }
                    default:
                        mp_raise_ValueError(MP_ERROR_TEXT("unknown config param"));
                }
            }
        }

        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(wiznet5k_config_obj, 1, wiznet5k_config);

STATIC mp_obj_t send_ethernet_wrapper(mp_obj_t self_in, mp_obj_t buf_in) {
    wiznet5k_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t buf;
    mp_get_buffer_raise(buf_in, &buf, MP_BUFFER_READ);
    wiznet5k_send_ethernet(self, buf.len, buf.buf);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(send_ethernet_obj, send_ethernet_wrapper);

STATIC const mp_rom_map_elem_t wiznet5k_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_regs), MP_ROM_PTR(&wiznet5k_regs_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&wiznet5k_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&wiznet5k_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&wiznet5k_ifconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&wiznet5k_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&wiznet5k_config_obj) },

    { MP_ROM_QSTR(MP_QSTR_send_ethernet), MP_ROM_PTR(&send_ethernet_obj) },
};
STATIC MP_DEFINE_CONST_DICT(wiznet5k_locals_dict, wiznet5k_locals_dict_table);

const mp_obj_type_t mod_network_nic_type_wiznet5k = {
    { &mp_type_type },
    .name = MP_QSTR_WIZNET5K,
    .make_new = wiznet5k_make_new,
    .locals_dict = (mp_obj_dict_t *)&wiznet5k_locals_dict,
};

#endif // MICROPY_PY_WIZNET5K && MICROPY_PY_LWIP
