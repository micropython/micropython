/*
 * Micro Python CC3100 Driver
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Kimball Johnson
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

#include <string.h>
#include <stdarg.h>
#include <errno.h>

#include "py/nlr.h"
#include "py/objtuple.h"
#include "py/objlist.h"
#include "py/stream.h"
#include "py/runtime.h"
#include "py/mphal.h"

#include "netutils.h"
#include "modnetwork.h"
#include "pin.h"
#include "genhdr/pins.h"
#include "spi.h"
#include "pybioctl.h"

#include "extint.h"

#include "modnwcc3100.h"
#include "simplelink.h"
#include "socket.h"



// *** Begin simplelink interface functions
STATIC volatile irq_handler_t cc3100_IrqHandler = 0;

STATIC SPI_HandleTypeDef *SPI_HANDLE = NULL;
STATIC const pin_obj_t *PIN_CS = NULL;
STATIC const pin_obj_t *PIN_EN = NULL;
STATIC const pin_obj_t *PIN_IRQ = NULL;

// External CC3100
Fd_t spi_Open(char* pIfName, unsigned long flags)
{
    mp_uint_t spi_clock, br_prescale;
    spi_clock = HAL_RCC_GetPCLK1Freq();
    br_prescale = spi_clock / 2000000; //2MHz
    if (br_prescale <= 2) { SPI_HANDLE->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2; }
    else if (br_prescale <= 4) { SPI_HANDLE->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4; }
    else if (br_prescale <= 8) { SPI_HANDLE->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; }
    else if (br_prescale <= 16) { SPI_HANDLE->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16; }
    else if (br_prescale <= 32) { SPI_HANDLE->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32; }
    else if (br_prescale <= 64) { SPI_HANDLE->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64; }
    else if (br_prescale <= 128) { SPI_HANDLE->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128; }
    else { SPI_HANDLE->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256; }

    SPI_HANDLE->Init.Mode = SPI_MODE_MASTER;
    SPI_HANDLE->Init.CLKPolarity = SPI_POLARITY_LOW;
    SPI_HANDLE->Init.CLKPhase = SPI_PHASE_1EDGE;
    SPI_HANDLE->Init.Direction = SPI_DIRECTION_2LINES;
    SPI_HANDLE->Init.DataSize = SPI_DATASIZE_8BIT;
    SPI_HANDLE->Init.NSS = SPI_NSS_SOFT;
    SPI_HANDLE->Init.FirstBit = SPI_FIRSTBIT_MSB;
    SPI_HANDLE->Init.TIMode = SPI_TIMODE_DISABLED;
    SPI_HANDLE->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
    SPI_HANDLE->Init.CRCPolynomial = 0;
    
    // init the SPI bus
    spi_init(SPI_HANDLE, false);
    
    // CS Pin
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Pin = PIN_CS->pin_mask;;
    HAL_GPIO_Init(PIN_CS->gpio, &GPIO_InitStructure);
    GPIO_set_pin(PIN_CS->gpio, PIN_CS->pin_mask);
    
    HAL_Delay(50);
    return 1;
}

int spi_Close(Fd_t Fd)
{
    spi_deinit(SPI_HANDLE);
    return 0;
}

int spi_Read(Fd_t Fd, unsigned char* pBuff, int Len)
{
    int retval;
    memset(pBuff, 0xFF, Len);
    extint_disable(PIN_IRQ->pin);
    GPIO_clear_pin(PIN_CS->gpio, PIN_CS->pin_mask);
    retval = HAL_SPI_Receive(SPI_HANDLE, pBuff, Len, 20);
    GPIO_set_pin(PIN_CS->gpio, PIN_CS->pin_mask);
    extint_enable(PIN_IRQ->pin);
    if(retval < 0)
      return(0);
      
    return Len;
}

int spi_Write(Fd_t Fd, unsigned char* pBuff, int Len){
    extint_disable(PIN_IRQ->pin);
    GPIO_clear_pin(PIN_CS->gpio, PIN_CS->pin_mask);
    HAL_SPI_Transmit(SPI_HANDLE, pBuff, Len, 20);
    GPIO_set_pin(PIN_CS->gpio, PIN_CS->pin_mask);
    extint_enable(PIN_IRQ->pin);
    return Len;
}

void NwpPowerOnPreamble(void){

// IRQ pin
    GPIO_InitTypeDef init;
    init.Pin = PIN_IRQ->pin_mask;
    init.Mode = GPIO_MODE_INPUT;
    init.Pull = GPIO_PULLDOWN;
    init.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(PIN_IRQ->gpio, &init);

//nHib pin
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Pin = PIN_EN->pin_mask;;
    HAL_GPIO_Init(PIN_EN->gpio, &GPIO_InitStructure);
    GPIO_clear_pin(PIN_EN->gpio, PIN_EN->pin_mask);
}
void NwpPowerOn(void){
    GPIO_set_pin(PIN_EN->gpio, PIN_EN->pin_mask);
}
void NwpPowerOff(void){
    GPIO_clear_pin(PIN_EN->gpio, PIN_EN->pin_mask);
}

STATIC mp_obj_t cc3100_callback(mp_obj_t line) {
    if (cc3100_IrqHandler != 0) {
        (cc3100_IrqHandler)(line);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(cc3100_callback_obj, cc3100_callback);

int NwpRegisterInterruptHandler(SL_P_EVENT_HANDLER InterruptHdl, void* pValue){
    extint_disable(PIN_IRQ->pin);
    cc3100_IrqHandler = InterruptHdl;
    extint_register((mp_obj_t)PIN_IRQ,
                    GPIO_MODE_IT_RISING,
                    GPIO_PULLDOWN,
                    (mp_obj_t)&cc3100_callback_obj,
                    true);
    extint_enable(PIN_IRQ->pin);
    return 0;
}

void NwpMaskInterrupt(){
    extint_disable(PIN_IRQ->pin);
}

void NwpUnMaskInterrupt(){
    extint_enable(PIN_IRQ->pin);
}

// *** END simplelink interface functions



#define MAKE_SOCKADDR(addr, ip, port) \
    SlSockAddr_t addr; \
    addr.sa_family = SL_AF_INET; \
    addr.sa_data[0] = port >> 8; \
    addr.sa_data[1] = port; \
    addr.sa_data[2] = ip[0]; \
    addr.sa_data[3] = ip[1]; \
    addr.sa_data[4] = ip[2]; \
    addr.sa_data[5] = ip[3];

#define UNPACK_SOCKADDR(addr, ip, port) \
    port = (addr.sa_data[0] << 8) | addr.sa_data[1]; \
    ip[0] = addr.sa_data[2]; \
    ip[1] = addr.sa_data[3]; \
    ip[2] = addr.sa_data[4]; \
    ip[3] = addr.sa_data[5];


#define MAX_RX_PACKET       16000
#define MAX_TX_PACKET       1460


STATIC volatile uint32_t fd_closed_state = 0;
STATIC volatile bool wlan_connected = false;
STATIC volatile bool ip_obtained = false;

STATIC int cc3100_get_fd_closed_state(int fd) {
    return fd_closed_state & (1 << fd);
}

STATIC void cc3100_set_fd_closed_state(int fd) {
    fd_closed_state |= 1 << fd;
}

STATIC void cc3100_reset_fd_closed_state(int fd) {
    fd_closed_state &= ~(1 << fd);
}


// Socket functions

// gethostbyname
STATIC int cc3100_gethostbyname(mp_obj_t nic, const char *name, mp_uint_t len, uint8_t *out_ip) {
    uint32_t ip;

    int rc = sl_NetAppDnsGetHostByName((signed char *)name, (uint16_t)len, &ip, SL_AF_INET);
    if (rc != 0) {
        return rc;
    }

    if (ip == 0) {
        // unknown host
        return ENOENT;
    }

    out_ip[0] = ip >> 24;
    out_ip[1] = ip >> 16;
    out_ip[2] = ip >> 8;
    out_ip[3] = ip;

    return 0;
}



// Additional interface functions
// method connect(ssid, key=None, *, security=WPA2, bssid=None)
STATIC mp_obj_t cc3100_connect(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_ssid, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_key, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_security, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = SL_SEC_TYPE_WPA_WPA2} },
        { MP_QSTR_bssid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get ssid
    mp_uint_t ssid_len;
    const char *ssid = mp_obj_str_get_data(args[0].u_obj, &ssid_len);

    // get key and sec
    mp_uint_t key_len = 0;
    const char *key = NULL;
    mp_uint_t sec = SL_SEC_TYPE_OPEN;
    if (args[1].u_obj != mp_const_none) {
        key = mp_obj_str_get_data(args[1].u_obj, &key_len);
        sec = args[2].u_int;
    }

    // get bssid
    const char *bssid = NULL;
    if (args[3].u_obj != mp_const_none) {
        bssid = mp_obj_str_get_str(args[3].u_obj);
    }

    SlSecParams_t sec_params;
    sec_params.Type = sec;
    sec_params.Key = (int8_t*)key;
    sec_params.KeyLen = key_len;

    // connect to AP
    printf("Connect to AP\n");
    if (sl_WlanConnect((int8_t*)ssid, ssid_len, (uint8_t*)bssid, &sec_params, NULL)!= 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
          "could not connect to ssid=%s, sec=%d, key=%s\n", ssid, sec, key));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(cc3100_connect_obj, 1, cc3100_connect);

STATIC mp_obj_t cc3100_disconnect(mp_obj_t self_in) {
    sl_WlanDisconnect();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(cc3100_disconnect_obj, cc3100_disconnect);

STATIC mp_obj_t cc3100_is_connected(mp_obj_t self_in) {

    if (wlan_connected && ip_obtained) {
        return mp_const_true;
    }
    return mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(cc3100_is_connected_obj, cc3100_is_connected);

STATIC mp_obj_t cc3100_update(mp_obj_t self_in) {

    _SlNonOsMainLoopTask();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(cc3100_update_obj, cc3100_update);

STATIC mp_obj_t cc3100_sleep(mp_obj_t self_in) {

    sl_Stop(100);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(cc3100_sleep_obj, cc3100_sleep);

/******************************************************************************/
// Micro Python bindings; CC3100 class

typedef struct _cc3100_obj_t {
    mp_obj_base_t base;
} cc3100_obj_t;

STATIC const cc3100_obj_t cc3100_obj = {{(mp_obj_type_t*)&mod_network_nic_type_cc3100}};
#define LOG_ERR(str) printf("Error: %s\n",str)
#define LOG_INFO(str) printf("Info: %s\n",str)
#define LOG_COND_CONT(condition)       ((void)0)
#define LOG_COND_RET(condition, rc)    ((void)0)


STATIC mp_obj_t cc3100_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {

    wlan_connected = false;
    ip_obtained    = false;

    uint8_t val = 1;
    uint8_t configOpt = 0;
    uint8_t power = 0;
    int32_t retVal = -1;
    int32_t mode = -1;

    // Either defaults, or SPI Obj, IRQ Pin, nHIB Pin 
    mp_arg_check_num(n_args, n_kw, 0, 4, false); 
    
    //If no args given, we setup from coded defaults
    if (n_args == 0) {
#ifdef MICROPY_HW_CC3100_SPI
      SPI_HANDLE = &MICROPY_HW_CC3100_SPI;
      PIN_CS = &MICROPY_HW_CC3100_CS;
      PIN_EN = &MICROPY_HW_CC3100_HIB;
      PIN_IRQ = &MICROPY_HW_CC3100_IRQ;
#else
      nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "No Default CC3100 definition"));
#endif
    } else {
    //Else we use the given args
       //TODO should verify the argument types
       SPI_HANDLE = spi_get_handle(args[0]);
       PIN_CS = pin_find(args[1]);
       PIN_EN = pin_find(args[2]);
       PIN_IRQ = pin_find(args[3]);
    }


    mode = sl_Start(NULL,NULL,NULL);

    if(ROLE_STA != mode)
    {   // Configure the device into station mode
        if(ROLE_AP == mode)
        {
            LOG_INFO("mode: ROLE_AP");
            /* If the device is in AP mode, we need to wait for this event before doing anything */
            while(ip_obtained == false)
            {
                _SlNonOsMainLoopTask();
            }
        }

        // Select station mode, and restart to activate it
        retVal = sl_WlanSetMode(ROLE_STA);
        LOG_COND_RET(retVal>=0, -1);
        retVal = sl_Stop(100);
        LOG_COND_RET(retVal>=0, -1);
        mode = sl_Start(0, 0, 0);
        LOG_COND_RET(mode>=0, -1);
        if(ROLE_STA != mode)
        {
            LOG_ERR("not in station mode");
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Failed to init cc3100 module"));
        }
    }

    /* Set connection policy to Auto + SmartConfig (Device's default connection policy) */
    LOG_INFO("Set connection policy");
    retVal = sl_WlanPolicySet(SL_POLICY_CONNECTION, SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);
    LOG_COND_RET(retVal>=0, -1);

    /* Remove all profiles */
    LOG_INFO("Remove all profiles");
    retVal = sl_WlanProfileDel(0xFF);
    LOG_COND_RET(retVal>=0, -1);

    /*
     * Device in station-mode. Disconnect previous connection if any
     * The function returns 0 if 'Disconnected done', negative number if already disconnected
     * Wait for 'disconnection' event if 0 is returned, Ignore other return-codes
     */
    LOG_INFO("Disconnect from AP");
    retVal = sl_WlanDisconnect();
    if(0 == retVal)
    {  // wait for disconnection
        while(wlan_connected == true)
        {
            _SlNonOsMainLoopTask();
        }
    }

    /* Enable DHCP client*/
    LOG_INFO("Enable DHCP");
    retVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,(uint8_t *)&val);
    LOG_COND_RET(retVal>=0, -1);

    /* Disable scan */
    LOG_INFO("Disable Scan");
    configOpt = SL_SCAN_POLICY(0);
    retVal = sl_WlanPolicySet(SL_POLICY_SCAN, configOpt, NULL, 0);
    LOG_COND_RET(retVal>=0, -1);

    /* Set Tx power level for station mode
       Number between 0-15, as dB offset from max power - 0 will set maximum power */
    power = 0;
    retVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *)&power);
    LOG_COND_RET(retVal>=0, -1);

    /* Set PM policy to normal */
    retVal = sl_WlanPolicySet(SL_POLICY_PM, SL_NORMAL_POLICY, NULL, 0);
    LOG_COND_RET(retVal>=0, -1);

    /* Unregister mDNS services */
    retVal = sl_NetAppMDNSUnRegisterService(0, 0);
    LOG_COND_RET(retVal>=0, -1);

    retVal = sl_Stop(100);
    LOG_COND_RET(retVal>=0, -1);

    /* Initializing the CC3100 device */
    retVal = sl_Start(0, 0, 0);
    if((retVal < 0) || (ROLE_STA != retVal))
    {
        LOG_ERR("");
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Failed to init cc31k module"));
    }
    // register with network module
    mod_network_register_nic((mp_obj_t)&cc3100_obj);
    return (mp_obj_t)&cc3100_obj;

}

STATIC const mp_map_elem_t cc3100_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_connect),         (mp_obj_t)&cc3100_connect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disconnect),      (mp_obj_t)&cc3100_disconnect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_is_connected),    (mp_obj_t)&cc3100_is_connected_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_update),          (mp_obj_t)&cc3100_update_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sleep),           (mp_obj_t)&cc3100_sleep_obj },


    // class constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_WEP), MP_OBJ_NEW_SMALL_INT(SL_SEC_TYPE_WEP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WPA), MP_OBJ_NEW_SMALL_INT(SL_SEC_TYPE_WPA) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WPA2), MP_OBJ_NEW_SMALL_INT(SL_SEC_TYPE_WPA_ENT) },
};

STATIC MP_DEFINE_CONST_DICT(cc3100_locals_dict, cc3100_locals_dict_table);

STATIC int cc3100_socket_socket(mod_network_socket_obj_t *socket, int *_errno) {
    if (socket->u_param.domain != MOD_NETWORK_AF_INET) {
        *_errno = EAFNOSUPPORT;
        return -1;
    }

    mp_uint_t type;
    switch (socket->u_param.type) {
        case MOD_NETWORK_SOCK_STREAM: type = SL_SOCK_STREAM; break;
        case MOD_NETWORK_SOCK_DGRAM: type = SL_SOCK_DGRAM; break;
        case MOD_NETWORK_SOCK_RAW: type = SL_SOCK_RAW; break;
        default: *_errno = EINVAL; return -1;
    }

    // open socket
    int fd = sl_Socket(SL_AF_INET, type, 0);
    if (fd < 0) {
        *_errno = fd;
        return -1;
    }

    // clear socket state
    cc3100_reset_fd_closed_state(fd);

    // store state of this socket
    socket->u_state = fd;

    return 0;
}

STATIC void cc3100_socket_close(mod_network_socket_obj_t *socket) {
    sl_Close(socket->u_state);
    cc3100_set_fd_closed_state(socket->u_state);
}

STATIC int cc3100_socket_bind(mod_network_socket_obj_t *socket, byte *ip, mp_uint_t port, int *_errno) {
    MAKE_SOCKADDR(addr, ip, port)
    int ret = sl_Bind(socket->u_state, &addr, sizeof(addr));
    if (ret != 0) {
        *_errno = ret;
        return -1;
    }
    return 0;
}

STATIC int cc3100_socket_listen(mod_network_socket_obj_t *socket, mp_int_t backlog, int *_errno) {
    int ret = sl_Listen(socket->u_state, backlog);
    if (ret != 0) {
        *_errno = ret;
        return -1;
    }
    return 0;
}

STATIC int cc3100_socket_accept(mod_network_socket_obj_t *socket, mod_network_socket_obj_t *socket2, byte *ip, mp_uint_t *port, int *_errno) {
    // accept incoming connection
    int fd;
    SlSockAddr_t addr;
    SlSocklen_t addr_len = sizeof(addr);
    if ((fd = sl_Accept(socket->u_state, &addr, &addr_len)) < 0) {
        if (fd == SL_EAGAIN) {
            *_errno = EAGAIN;
        } else {
            *_errno = -fd;
        }
        return -1;
    }

    // clear socket state
    cc3100_reset_fd_closed_state(fd);

    // store state in new socket object
    socket2->u_state = fd;

    // TODO need to check this on cc3100
    // return ip and port
    // it seems CC3000 returns little endian for accept??
    //UNPACK_SOCKADDR(addr, ip, *port);
    *port = (addr.sa_data[1] << 8) | addr.sa_data[0];
    ip[3] = addr.sa_data[2];
    ip[2] = addr.sa_data[3];
    ip[1] = addr.sa_data[4];
    ip[0] = addr.sa_data[5];

    return 0;
}

STATIC int cc3100_socket_connect(mod_network_socket_obj_t *socket, byte *ip, mp_uint_t port, int *_errno) {
    if (cc3100_get_fd_closed_state(socket->u_state))
    {
      cc3100_socket_socket(socket,_errno);// Socket has been closed, we need to recreate it
    }

    MAKE_SOCKADDR(addr, ip, port)
    int ret = sl_Connect(socket->u_state, &addr, sizeof(addr));
    if (ret != 0) {
        *_errno = -1; // TODO find correct errno
        return -1;
    }
    return 0;
}

STATIC mp_uint_t cc3100_socket_send(mod_network_socket_obj_t *socket, const byte *buf, mp_uint_t len, int *_errno) {
    if (cc3100_get_fd_closed_state(socket->u_state)) {
        sl_Close(socket->u_state);
        *_errno = EPIPE;
        return -1;
    }

    // CC3100 does not handle fragmentation, and will overflow,
    // split the packet into smaller ones and send them out.
    mp_int_t bytes = 0;
    while (bytes < len) {
        int n = MIN((len - bytes), MAX_TX_PACKET);
        n = sl_Send(socket->u_state, (uint8_t*)buf + bytes, n, 0);
        if (n <= 0) {
            *_errno = -1; //TODO find correct error
            return -1;
        }
        bytes += n;
    }

    return bytes;
}

STATIC mp_uint_t cc3100_socket_recv(mod_network_socket_obj_t *socket, byte *buf, mp_uint_t len, int *_errno) {
    // check the socket is open
    if (cc3100_get_fd_closed_state(socket->u_state)) {
        // socket is closed, but CC3100 may have some data remaining in buffer, so check
        SlFdSet_t rfds;
        SL_FD_ZERO(&rfds);
        SL_FD_SET(socket->u_state, &rfds);
        SlTimeval_t tv;
        tv.tv_sec = 0;
        tv.tv_usec = 1;
        int nfds = sl_Select(socket->u_state + 1, &rfds, NULL, NULL, &tv);
        if (nfds == -1 || !SL_FD_ISSET(socket->u_state, &rfds)) {
            // no data waiting, so close socket and return 0 data
            sl_Close(socket->u_state);
            return 0;
        }
    }

    // cap length at MAX_RX_PACKET
    len = MIN(len, MAX_RX_PACKET);

    // do the recv
    int ret = sl_Recv(socket->u_state, buf, len, 0);
    if (ret < 0) {
        *_errno = -1; //TODO find correct error
        return -1;
    }

    return ret;
}

STATIC mp_uint_t cc3100_socket_sendto(mod_network_socket_obj_t *socket, const byte *buf, mp_uint_t len, byte *ip, mp_uint_t port, int *_errno) {
    MAKE_SOCKADDR(addr, ip, port)
    int ret = sl_SendTo(socket->u_state, (byte*)buf, len, 0, (SlSockAddr_t*)&addr, sizeof(addr));
    if (ret < 0) {
        *_errno = -1; //TODO find correct error
        return -1;
    }
    return ret;
}

STATIC mp_uint_t cc3100_socket_recvfrom(mod_network_socket_obj_t *socket, byte *buf, mp_uint_t len, byte *ip, mp_uint_t *port, int *_errno) {
    SlSockAddr_t addr;
    SlSocklen_t addr_len = sizeof(addr);
    mp_int_t ret = sl_RecvFrom(socket->u_state, buf, len, 0, &addr, &addr_len);
    if (ret < 0) {
        *_errno = -1; //TODO find correct error
        return -1;
    }
    UNPACK_SOCKADDR(addr, ip, *port);
    return ret;
}

STATIC int cc3100_socket_setsockopt(mod_network_socket_obj_t *socket, mp_uint_t level, mp_uint_t opt, const void *optval, mp_uint_t optlen, int *_errno) {
    int ret = sl_SetSockOpt(socket->u_state, level, opt, optval, optlen);
    if (ret < 0) {
        *_errno = -1; //TODO find correct error
        return -1;
    }
    return 0;
}

STATIC int cc3100_socket_settimeout(mod_network_socket_obj_t *socket, mp_uint_t timeout_ms, int *_errno) {
    int ret;
    if (timeout_ms == 0 || timeout_ms == -1) {
        SlSockNonblocking_t optval;
        SlSocklen_t optlen = sizeof(optval);
        if (timeout_ms == 0) {
            // set non-blocking mode
            optval.NonblockingEnabled = 1;
        } else {
            // set blocking mode
            optval.NonblockingEnabled = 0;
        }
        ret = sl_SetSockOpt(socket->u_state, SL_SOL_SOCKET, SL_SO_NONBLOCKING , &optval, optlen);

    } else {
        // set timeout
        SlTimeval_t timeout;
        timeout.tv_sec = timeout_ms / 1000;
        timeout.tv_usec = timeout_ms % 1000;
        SlSocklen_t optlen = sizeof(timeout);
        ret = sl_SetSockOpt(socket->u_state, SL_SOL_SOCKET, SL_SO_RCVTIMEO , &timeout, optlen);
    }

    if (ret != 0) {
        *_errno = -1; //TODO find correct error
        return -1;
    }

    return 0;
}

STATIC int cc3100_socket_ioctl(mod_network_socket_obj_t *socket, mp_uint_t request, mp_uint_t arg, int *_errno) {
    mp_uint_t ret;
    if (request == MP_IOCTL_POLL) {
        mp_uint_t flags = arg;
        ret = 0;
        int fd = socket->u_state;

        // init fds
        SlFdSet_t  rfds, wfds, xfds;
        SL_FD_ZERO(&rfds);
        SL_FD_ZERO(&wfds);
        SL_FD_ZERO(&xfds);

        // set fds if needed
        if (flags & MP_IOCTL_POLL_RD) {
            SL_FD_SET(fd, &rfds);

            // A socked that just closed is available for reading.  A call to
            // recv() returns 0 which is consistent with BSD.
            if (cc3100_get_fd_closed_state(fd)) {
                ret |= MP_IOCTL_POLL_RD;
            }
        }
        if (flags & MP_IOCTL_POLL_WR) {
            SL_FD_SET(fd, &wfds);
        }
        if (flags & MP_IOCTL_POLL_HUP) {
            SL_FD_SET(fd, &xfds);
        }

        // call cc3100 select with minimum timeout
        SlTimeval_t tv;
        tv.tv_sec = 0;
        tv.tv_usec = 1;
        int nfds = sl_Select(fd + 1, &rfds, &wfds, &xfds, &tv);

        // check for error
        if (nfds == -1) {
            *_errno = -1;
            return -1;
        }

        // check return of select
        if (SL_FD_ISSET(fd, &rfds)) {
            ret |= MP_IOCTL_POLL_RD;
        }
        if (SL_FD_ISSET(fd, &wfds)) {
            ret |= MP_IOCTL_POLL_WR;
        }
        if (SL_FD_ISSET(fd, &xfds)) {
            ret |= MP_IOCTL_POLL_HUP;
        }
    } else {
        *_errno = EINVAL;
        ret = -1;
    }
    return ret;
}


const mod_network_nic_type_t mod_network_nic_type_cc3100 = {
    .base = {
        { &mp_type_type },
        .name = MP_QSTR_CC3100,
        .make_new = cc3100_make_new,
        .locals_dict = (mp_obj_t)&cc3100_locals_dict,
    },
    .gethostbyname = cc3100_gethostbyname,
    .socket = cc3100_socket_socket,
    .close = cc3100_socket_close,
    .bind = cc3100_socket_bind,
    .listen = cc3100_socket_listen,
    .accept = cc3100_socket_accept,
    .connect = cc3100_socket_connect,
    .send = cc3100_socket_send,
    .recv = cc3100_socket_recv,
    .sendto = cc3100_socket_sendto,
    .recvfrom = cc3100_socket_recvfrom,
    .setsockopt = cc3100_socket_setsockopt,
    .settimeout = cc3100_socket_settimeout,
    .ioctl = cc3100_socket_ioctl,
};

// --------------------------------------------------------------------------------------
// ASYNCHRONOUS EVENT HANDLERS
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
//    This function handles WLAN events
//
//    pWlanEvent is the event passed to the handler
//
// --------------------------------------------------------------------------------------
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    switch(pWlanEvent->Event)
    {
    case SL_WLAN_CONNECT_EVENT:
    {
        LOG_INFO("[WLAN EVENT] connect");
        wlan_connected = true;
        /*
         * Information about the connected AP (like name, MAC etc) will be
         * available in 'slWlanConnectAsyncResponse_t' - Applications
         * can use it if required
         *
         * slWlanConnectAsyncResponse_t *pEventData = NULL;
         * pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
         *
         */
    }
    break;

    case SL_WLAN_DISCONNECT_EVENT:
    {
        //LOG_INFO("[WLAN EVENT] disconnect");
        // link down
        wlan_connected = false;
        ip_obtained = false;
        // Get the reason code
        slWlanConnectAsyncResponse_t*  pEventData = NULL;
        pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

        if(SL_USER_INITIATED_DISCONNECTION == pEventData->reason_code)
        {           // user initiated a disconnect request
                    //LOG_INFO("Device disconnected from the AP on application's request");
        }
        else
        {
            LOG_INFO("Device disconnected from the AP on an ERROR..!!");
        }
    }
    break;

    case SL_WLAN_STA_CONNECTED_EVENT:
        LOG_INFO("[WLAN EVENT] SL_WLAN_STA_CONNECTED_EVENT");
        break;
    case SL_WLAN_STA_DISCONNECTED_EVENT:
        LOG_INFO("[WLAN EVENT] SL_WLAN_STA_DISCONNECTED_EVENT");
        break;
    case SL_WLAN_CONNECTION_FAILED_EVENT:
        LOG_INFO("[WLAN EVENT] SL_WLAN_CONNECTION_FAILED_EVENT");
        break;
    default:
        LOG_INFO("[WLAN EVENT] Unexpected event");
        printf("pWlanEvent->Event: %d\n", (int)pWlanEvent->Event);
        break;
    }
}

// --------------------------------------------------------------------------------------
//    This function handles events for IP address acquisition via DHCP
//
//    pNetAppEvent is the event passed to the handler
//
// --------------------------------------------------------------------------------------
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
    switch(pNetAppEvent->Event)
    {
    case SL_NETAPP_IPV4_IPACQUIRED_EVENT:
        LOG_INFO("[NETAPP EVENT] IP acquired");
        ip_obtained = true;
        /*
         * Information about the connection (like IP, gateway address etc)
         * will be available in 'SlIpV4AcquiredAsync_t'
         * Applications can use it if required
         *
         * SlIpV4AcquiredAsync_t *pEventData = NULL;
         * pEventData = &pNetAppEvent->EventData.ipAcquiredV4;
         *
         */
         SlIpV4AcquiredAsync_t *pEventData = NULL;
         pEventData = &pNetAppEvent->EventData.ipAcquiredV4;
         uint8_t ip1,ip2,ip3,ip4;
         ip1 = pEventData->ip & 0xfff0;
         ip2 = (pEventData->ip & 0xff0f) >> 8;
         ip3 = (pEventData->ip & 0xf0ff) >> 16;
         ip4 = (pEventData->ip & 0x0fff) >> 24;
         printf("IP: %d.%d.%d.%d\n",ip1,ip2,ip3,ip4);
         
        break;
    case SL_NETAPP_IP_LEASED_EVENT:
        LOG_INFO("[NETAPP EVENT] SL_NETAPP_IP_LEASED");
        break;
    case SL_NETAPP_IP_RELEASED_EVENT:
        LOG_INFO("[NETAPP EVENT] IP released");
        // mark socket for closure

        break;
    default:
        LOG_INFO("[NETAPP EVENT] Unexpected event");
        printf("pWlanEvent->Event: %d\n", (int)pNetAppEvent->Event);
        break;
    }
}

// --------------------------------------------------------------------------------------
//    This function handles callback for the HTTP server events
//
//    pServerEvent - Contains the relevant event information
//    pServerResponse - Should be filled by the user with the
//    relevant response information
//
// --------------------------------------------------------------------------------------
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
                                  SlHttpServerResponse_t *pHttpResponse)
{
    /* Unused in this application */
    LOG_INFO("[HTTP EVENT] Unexpected event");
}

// --------------------------------------------------------------------------------------
//    This function handles general error events indication
//
//    pDevEvent is the event passed to the handler
//
// --------------------------------------------------------------------------------------
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
    /*
     * Most of the general errors are not FATAL are are to be handled
     * appropriately by the application
     */

    switch(pDevEvent->Event)
    {
        case SL_DEVICE_FATAL_ERROR_EVENT:
            LOG_INFO("[GENERAL EVENT] Fatal error: reset device");
            break;
        default:
            LOG_INFO("[GENERAL EVENT]");
            printf("%d\n", (int)pDevEvent->Event);
            break;
    }
}

// --------------------------------------------------------------------------------------
//    This function handles socket events indication
//
//    pSock is the event passed to the handler
//
// --------------------------------------------------------------------------------------
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
    switch(pSock->Event)
    {
        case SL_SOCKET_TX_FAILED_EVENT:
            LOG_INFO("[SOCK EVENT] SL_NETAPP_SOCKET_TX_FAILED");
            /*
             * TX Failed
             *
             * Information about the socket descriptor and status will be
             * available in 'SlSockEventData_t' - Applications can use it if
             * required
             *
             * SlSockEventData_t *pEventData = NULL;
             * pEventData = & pSock->EventData;
             */
            switch(pSock->socketAsyncEvent.SockTxFailData.status)
            {
                case SL_ECLOSE:
                    LOG_INFO("[SOCK EVENT] Close socket operation failed to transmit all queued packets");
                    break;
                default:
                    printf("[SOCK EVENT] Unexpected status %d", pSock->socketAsyncEvent.SockTxFailData.status);
                    break;
            }
            break;

        /*case SL_SOCKET_ASYNC_EVENT:
            LOG_INFO("[SOCK EVENT] SL_SOCKET_ASYNC_EVENT");
            break;*/

        default:
            printf("[SOCK EVENT] Unexpected event %d\n", (int)pSock->Event);
            break;
    }
}

