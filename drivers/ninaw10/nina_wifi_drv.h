/*
 * This file is part of the OpenMV project, https://openmv.io.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
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
 *
 * NINA-W10 WiFi driver.
 */
#ifndef MICROPY_INCLUDED_DRIVERS_NINAW10_NINA_WIFI_DRV_H
#define MICROPY_INCLUDED_DRIVERS_NINAW10_NINA_WIFI_DRV_H

#define NINA_FW_VER_LEN         (6)
#define NINA_IPV4_ADDR_LEN      (4)
#define NINA_MAC_ADDR_LEN       (6)
#define NINA_MAX_SSID_LEN       (32)
#define NINA_MAX_WEP_LEN        (13)
#define NINA_MAX_WPA_LEN        (63)
#define NINA_MAX_NETWORK_LIST   (10)
#define NINA_MAX_SOCKET         (10)

#define NINA_FW_VER_MAJOR       (1)
#define NINA_FW_VER_MINOR       (4)
#define NINA_FW_VER_PATCH       (8)

#define NINA_FW_VER_MAJOR_OFFS  (0)
#define NINA_FW_VER_MINOR_OFFS  (2)
#define NINA_FW_VER_PATCH_OFFS  (4)

typedef enum {
    NINA_SEC_INVALID = 0,
    NINA_SEC_OPEN,
    NINA_SEC_WPA_PSK,
    NINA_SEC_WEP
} nina_security_t;

typedef enum {
    NINA_SOCKET_TYPE_TCP = 0,
    NINA_SOCKET_TYPE_UDP,
    NINA_SOCKET_TYPE_TLS,
    NINA_SOCKET_TYPE_UDP_MULTICAST,
    NINA_SOCKET_TYPE_TLS_BEARSSL
} nina_socket_type_t;

typedef struct {
    uint8_t ip_addr[NINA_IPV4_ADDR_LEN];
    uint8_t subnet_addr[NINA_IPV4_ADDR_LEN];
    uint8_t gateway_addr[NINA_IPV4_ADDR_LEN];
    uint8_t dns_addr[NINA_IPV4_ADDR_LEN];
} nina_ifconfig_t;

typedef struct {
    int32_t rssi;
    uint8_t security;
    uint8_t channel;
    uint8_t bssid[NINA_MAC_ADDR_LEN];
    char ssid[NINA_MAX_SSID_LEN];
} nina_scan_result_t;

typedef struct {
    int32_t rssi;
    uint8_t security;
    char ssid[NINA_MAX_SSID_LEN];
    uint8_t bssid[NINA_MAC_ADDR_LEN];
} nina_netinfo_t;

typedef int (*nina_scan_callback_t)(nina_scan_result_t *, void *);

int nina_init(void);
int nina_deinit(void);
int nina_connect(const char *ssid, uint8_t security, const char *key, uint16_t channel);
int nina_start_ap(const char *ssid, uint8_t security, const char *key, uint16_t channel);
int nina_disconnect(void);
int nina_isconnected(void);
int nina_connected_sta(uint32_t *sta_ip);
int nina_wait_for_sta(uint32_t *sta_ip, uint32_t timeout);
int nina_ifconfig(nina_ifconfig_t *ifconfig, bool set);
int nina_netinfo(nina_netinfo_t *netinfo);
int nina_scan(nina_scan_callback_t scan_callback, void *arg, uint32_t timeout);
int nina_get_rssi(void);
int nina_fw_version(uint8_t *fw_ver);
int nina_set_hostname(const char *name);
int nina_gethostbyname(const char *name, uint8_t *out_ip);
int nina_ioctl(uint32_t cmd, size_t len, uint8_t *buf, uint32_t iface);
int nina_socket_socket(uint8_t type);
int nina_socket_close(int fd);
int nina_socket_bind(int fd, uint8_t *ip, uint16_t port, int type);
int nina_socket_listen(int fd, uint32_t backlog);
int nina_socket_avail(int fd, int type, uint16_t *data);
int nina_socket_accept(int fd, uint8_t *ip, uint16_t *port, int *fd_out, int32_t timeout);
int nina_socket_connect(int fd, uint8_t *ip, uint16_t port, int32_t timeout);
int nina_socket_send(int fd, const uint8_t *buf, uint32_t len, int32_t timeout);
int nina_socket_recv(int fd, uint8_t *buf, uint32_t len, int32_t timeout);
int nina_socket_sendto(int fd, const uint8_t *buf, uint32_t len, uint8_t *ip, uint16_t port, int32_t timeout);
int nina_socket_recvfrom(int fd, uint8_t *buf, uint32_t len, uint8_t *ip, uint16_t *port, int32_t timeout);
int nina_socket_setsockopt(int fd, uint32_t level, uint32_t opt, const void *optval, uint32_t optlen);

#endif // MICROPY_INCLUDED_DRIVERS_NINAW10_NINA_WIFI_DRV_H
