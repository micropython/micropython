#include <stdio.h>
#include <string.h>
#include "py/runtime.h"
#include "py/objstr.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "lib/netutils/netutils.h"
#include "drivers/wiznet5k/ethernet/wizchip_conf.h"
#include "drivers/wiznet5k/ethernet/socket.h"
#include "spi.h"

#include "lwip/init.h"
#include "lwip/timers.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/dns.h"
#include "lwip/tcp_impl.h"
#include "lwip/igmp.h"
#include "lwip/dhcp.h"
#include "netif/etharp.h"

/*******************************************************************************/
// Wiznet Ethernet driver

typedef struct _wiznet5k_obj_t {
    mp_obj_base_t base;
    void (*poll_callback)(void*); // must be second entry for modnetwork.c
    mp_uint_t cris_state;
    SPI_HandleTypeDef *spi;
    const pin_obj_t *cs;
    const pin_obj_t *rst;
    uint8_t eth_frame[1514];
    struct netif netif;
    struct dhcp dhcp_struct;
} wiznet5k_obj_t;

STATIC void wiznet_lwip_init(wiznet5k_obj_t *self);
STATIC void wiznet_lwip_poll(void *self_in);

const mp_obj_type_t mod_network_nic_type_wiznet5k;

STATIC wiznet5k_obj_t wiznet5k_obj;

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
    HAL_StatusTypeDef status = HAL_SPI_Receive(wiznet5k_obj.spi, buf, len, 5000);
    (void)status;
}

STATIC void wiz_spi_write(const uint8_t *buf, uint32_t len) {
    HAL_StatusTypeDef status = HAL_SPI_Transmit(wiznet5k_obj.spi, (uint8_t*)buf, len, 5000);
    (void)status;
}

// WIZNET5K(spi, pin_cs, pin_rst)
STATIC mp_obj_t wiznet5k_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 3, 3, false);

    // init the wiznet5k object
    wiznet5k_obj.base.type = &mod_network_nic_type_wiznet5k;
    wiznet5k_obj.poll_callback = wiznet_lwip_poll;
    wiznet5k_obj.cris_state = 0;
    wiznet5k_obj.spi = spi_get_handle(args[0]);
    wiznet5k_obj.cs = pin_find(args[1]);
    wiznet5k_obj.rst = pin_find(args[2]);

    /*!< SPI configuration */
    wiznet5k_obj.spi->Init.Mode = SPI_MODE_MASTER;
    wiznet5k_obj.spi->Init.Direction = SPI_DIRECTION_2LINES;
    wiznet5k_obj.spi->Init.DataSize = SPI_DATASIZE_8BIT;
    wiznet5k_obj.spi->Init.CLKPolarity = SPI_POLARITY_LOW; // clock is low when idle
    wiznet5k_obj.spi->Init.CLKPhase = SPI_PHASE_1EDGE; // data latched on first edge, which is rising edge for low-idle
    wiznet5k_obj.spi->Init.NSS = SPI_NSS_SOFT;
    wiznet5k_obj.spi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2; // clock freq = f_PCLK / this_prescale_value; Wiz820i can do up to 80MHz
    wiznet5k_obj.spi->Init.FirstBit = SPI_FIRSTBIT_MSB;
    wiznet5k_obj.spi->Init.TIMode = SPI_TIMODE_DISABLED;
    wiznet5k_obj.spi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
    wiznet5k_obj.spi->Init.CRCPolynomial = 7; // unused
    spi_init(wiznet5k_obj.spi, false);

    mp_hal_pin_output(wiznet5k_obj.cs);
    mp_hal_pin_output(wiznet5k_obj.rst);

    mp_hal_pin_low(wiznet5k_obj.rst);
    mp_hal_delay_ms(1); // datasheet says 2us
    mp_hal_pin_high(wiznet5k_obj.rst);
    mp_hal_delay_ms(150); // datasheet says 150ms

    reg_wizchip_cris_cbfunc(wiz_cris_enter, wiz_cris_exit);
    reg_wizchip_cs_cbfunc(wiz_cs_select, wiz_cs_deselect);
    reg_wizchip_spi_cbfunc(wiz_spi_read, wiz_spi_write);

    //uint8_t sn_size[16] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}; // 2k buffer for each socket
    uint8_t sn_size[16] = {16, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0}; // need 16k for fast MACRAW
    ctlwizchip(CW_INIT_WIZCHIP, sn_size);

    // set some sensible default values; they are configurable using ifconfig method
    wiz_NetInfo netinfo = {
        .mac = {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef},
        .ip = {192, 168, 0, 18},
        .sn = {255, 255, 255, 0},
        .gw = {192, 168, 0, 1},
        .dns = {8, 8, 8, 8}, // Google public DNS
        .dhcp = NETINFO_STATIC,
    };
    ctlnetwork(CN_SET_NETINFO, (void*)&netinfo);

    // seems we need a small delay after init
    mp_hal_delay_ms(250);

    // Hook the Wiznet into lwIP
    wiznet_lwip_init(&wiznet5k_obj);

    // return wiznet5k object
    return &wiznet5k_obj;
}

STATIC mp_obj_t wiznet5k_regs(mp_obj_t self_in) {
    //wiznet5k_obj_t *self = self_in;
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
    (void)self_in;
    return mp_obj_new_bool(wizphy_getphylink() == PHY_LINK_ON);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wiznet5k_isconnected_obj, wiznet5k_isconnected);

STATIC mp_obj_t wiznet5k_active(size_t n_args, const mp_obj_t *args) {
    wiznet5k_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        return mp_obj_new_bool(self->netif.flags & NETIF_FLAG_UP);
    } else {
        if (mp_obj_is_true(args[1])) {
            netif_set_up(&self->netif);
            netif_set_link_up(&self->netif);
        } else {
            netif_set_link_down(&self->netif);
            netif_set_down(&self->netif);
        }
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wiznet5k_active_obj, 1, 2, wiznet5k_active);

STATIC mp_obj_t wiznet5k_ifconfig(size_t n_args, const mp_obj_t *args) {
    wiznet5k_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        // get
        ip_addr_t dns = dns_getserver(0);
        mp_obj_t tuple[4] = {
            netutils_format_ipv4_addr((uint8_t*)&self->netif.ip_addr, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t*)&self->netif.netmask, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t*)&self->netif.gw, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t*)&dns, NETUTILS_BIG),
        };
        return mp_obj_new_tuple(4, tuple);
    } else if (args[1] == MP_OBJ_NEW_QSTR(MP_QSTR_dhcp)) {
        dhcp_set_struct(&self->netif, &self->dhcp_struct);
        dhcp_start(&self->netif);

        // wait for dhcp to get IP address
        uint32_t start = mp_hal_ticks_ms();
        while (self->netif.dhcp->state != DHCP_BOUND && mp_hal_ticks_ms() - start < 8000) {
            mp_hal_delay_ms(100);
        }
        if (self->netif.dhcp->state != DHCP_BOUND) {
            mp_raise_msg(&mp_type_OSError, "timeout waiting for DHCP to get IP address");
        }
        return mp_const_none;
    } else {
        // set
        mp_obj_t *items;
        mp_obj_get_array_fixed_n(args[1], 4, &items);
        netutils_parse_ipv4_addr(items[0], (uint8_t*)&self->netif.ip_addr, NETUTILS_BIG);
        netutils_parse_ipv4_addr(items[1], (uint8_t*)&self->netif.netmask, NETUTILS_BIG);
        netutils_parse_ipv4_addr(items[2], (uint8_t*)&self->netif.gw, NETUTILS_BIG);
        ip_addr_t dns;
        netutils_parse_ipv4_addr(items[3], (uint8_t*)&dns, NETUTILS_BIG);
        dns_setserver(0, &dns);
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wiznet5k_ifconfig_obj, 1, 2, wiznet5k_ifconfig);

STATIC void get_mac_address(wiznet5k_obj_t *self, uint8_t mac[6]) {
    (void)self;
    getSHAR(mac);
}

STATIC void send_ethernet(wiznet5k_obj_t *self, size_t len, const uint8_t *buf) {
    uint8_t ip[4] = {1, 1, 1, 1}; // dummy
    int ret = WIZCHIP_EXPORT(sendto)(0, (byte*)buf, len, ip, 11); // dummy port
    if (ret != len) {
        printf("WIZCHIP fatal error in send_ethernet: %d\n", ret);
        netif_set_link_down(&self->netif);
        netif_set_down(&self->netif);
    }
}

STATIC mp_obj_t send_ethernet_wrapper(mp_obj_t self_in, mp_obj_t buf_in) {
    wiznet5k_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t buf;
    mp_get_buffer_raise(buf_in, &buf, MP_BUFFER_READ);
    send_ethernet(self, buf.len, buf.buf);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(send_ethernet_obj, send_ethernet_wrapper);

/*******************************************************************************/
// lwip stuff

STATIC err_t my_netif_output(struct netif *netif, struct pbuf *p) {
    wiznet5k_obj_t *self = netif->state;
    pbuf_copy_partial(p, self->eth_frame, p->tot_len, 0);
    //printf("LWIP ETHERNET TX len=%u:", p->tot_len); xxd(p->tot_len, self->eth_frame);
    send_ethernet(netif->state, p->tot_len, self->eth_frame);
    return ERR_OK;
}

STATIC err_t my_netif_init(struct netif *netif) {
    netif->linkoutput = my_netif_output;
    netif->output = etharp_output;
    netif->mtu = 1500; // TODO work out what to use here
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP;
    uint8_t mac[6];
    get_mac_address(netif->state, mac);
    //printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    SMEMCPY(netif->hwaddr, mac, sizeof(netif->hwaddr));
    netif->hwaddr_len = sizeof(netif->hwaddr);
    int ret = WIZCHIP_EXPORT(socket)(0, Sn_MR_MACRAW, 0, 0);
    if (ret != 0) {
        printf("WIZNET fatal error in netifinit: %d\n", ret);
        return ERR_IF;
    }
    return ERR_OK;
}

STATIC void wiznet_lwip_init(wiznet5k_obj_t *self) {
    for (struct netif *netif = netif_list; netif != NULL; netif = netif->next) {
        if (netif == &self->netif) {
            netif_remove(netif);
            break;
        }
    }
    ip_addr_t ipconfig[4];
    ipconfig[0].addr = 0;
    ipconfig[1].addr = 0;
    ipconfig[2].addr = 0;
    ipconfig[3].addr = 0;
    netif_add(&self->netif, &ipconfig[0], &ipconfig[1], &ipconfig[2], self, my_netif_init, ethernet_input);
    self->netif.name[0] = 'l';
    self->netif.name[1] = '0';
    netif_set_default(&self->netif);
    dns_setserver(0, &ipconfig[3]);
}

STATIC void wiznet_lwip_poll(void *self_in) {
    wiznet5k_obj_t *self = self_in;
    uint16_t len = getSn_RX_RSR(0);
    while (len > 0) {
        byte ip[4];
        uint16_t port;
        int ret = WIZCHIP_EXPORT(recvfrom)(0, self->eth_frame, 1514, ip, &port);
        if (ret <= 0) {
            printf("WIZNET fata error in poll: len=%u ret=%d\n", len, ret);
            netif_set_link_down(&self->netif);
            netif_set_down(&self->netif);
            return;
        }
        len = ret;
        //printf("LWIP ETHERNET RX len=%u:", len); xxd(len, self->eth_frame);
        struct pbuf *p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
        if (p != NULL) {
            pbuf_take(p, self->eth_frame, len);
            if (self->netif.input(p, &self->netif) != ERR_OK) {
                pbuf_free(p);
            }
        }
        len = getSn_RX_RSR(0);
    }
}

/*******************************************************************************/
// class bindings

STATIC const mp_rom_map_elem_t wiznet5k_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_regs), MP_ROM_PTR(&wiznet5k_regs_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&wiznet5k_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&wiznet5k_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&wiznet5k_ifconfig_obj) },

    { MP_ROM_QSTR(MP_QSTR_send_ethernet), MP_ROM_PTR(&send_ethernet_obj) },
};
STATIC MP_DEFINE_CONST_DICT(wiznet5k_locals_dict, wiznet5k_locals_dict_table);

const mp_obj_type_t mod_network_nic_type_wiznet5k = {
    { &mp_type_type },
    .name = MP_QSTR_WIZNET5K,
    .make_new = wiznet5k_make_new,
    .locals_dict = (mp_obj_dict_t*)&wiznet5k_locals_dict,
};

