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
#include "dma.h"
#include "irq.h"
#include "spi.h"
#include "pybioctl.h"
#include "netcfg.h"

#include "extint.h"

#include "modnwcc3100.h"
#include "simplelink.h"
#include "socket.h"

#define LOG_ERR(str) printf("Error: %s\n",str)
#define LOG_INFO(str) printf("Info: %s\n",str)
#define LOG_COND_CONT(condition)       ((void)0)
#define LOG_COND_RET(condition, rc)    ((void)0)

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
    br_prescale = spi_clock / 20000000; //20MHz
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

HAL_StatusTypeDef spi_wait_dma_finished(SPI_HandleTypeDef *spi, uint32_t timeout) {
    // Note: we can't use WFI to idle in this loop because the DMA completion
    // interrupt may occur before the WFI.  Hence we miss it and have to wait
    // until the next sys-tick (up to 1ms).
    uint32_t start = HAL_GetTick();
    while (HAL_SPI_GetState(spi) != HAL_SPI_STATE_READY) {
        if (HAL_GetTick() - start >= timeout) {
            return HAL_TIMEOUT;
        }
    }
    return HAL_OK;
}

int spi_TransmitReceive(unsigned char* txBuff, unsigned char* rxBuff, int Len)
{
    HAL_StatusTypeDef status;
    extint_disable(PIN_IRQ->pin);
    GPIO_clear_pin(PIN_CS->gpio, PIN_CS->pin_mask);
    if (Len == 1 || query_irq() == IRQ_STATE_DISABLED) {
        status = HAL_SPI_TransmitReceive(SPI_HANDLE, txBuff, rxBuff, Len, 0x1000);
    } else {
        DMA_HandleTypeDef tx_dma, rx_dma;
        dma_init(&tx_dma, &dma_SPI_1_TX, SPI_HANDLE);
        SPI_HANDLE->hdmatx = &tx_dma;
        dma_init(&rx_dma, &dma_SPI_1_RX, SPI_HANDLE);
        SPI_HANDLE->hdmarx = &rx_dma;

        status = HAL_SPI_TransmitReceive_DMA(SPI_HANDLE, txBuff, rxBuff, Len);
        if (status == HAL_OK) {
            status = spi_wait_dma_finished(SPI_HANDLE, 0x1000);
        }
        dma_deinit(&dma_SPI_1_TX);
        dma_deinit(&dma_SPI_1_RX);
    }
    GPIO_set_pin(PIN_CS->gpio, PIN_CS->pin_mask);
    extint_enable(PIN_IRQ->pin);
    return status;
}

int spi_Read(Fd_t Fd, unsigned char* pBuff, int Len)
{
    HAL_StatusTypeDef status;
    unsigned char* dummy;
    dummy = malloc(sizeof(unsigned char)*Len);
    status = spi_TransmitReceive(dummy, pBuff, Len);
    free(dummy);
    if (status != HAL_OK)
        return(0);

    return Len;
}

int spi_Write(Fd_t Fd, unsigned char* pBuff, int Len)
{
    HAL_StatusTypeDef status;
    unsigned char* dummy;
    dummy = malloc(sizeof(unsigned char)*Len);
    status = spi_TransmitReceive(pBuff, dummy, Len);
    free(dummy);
    if (status != HAL_OK)
        return(0);

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

_u32 NwpSystemTicks(void)
{
	return HAL_GetTick();
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
// method connect(ssid, key=None, *, security=WPA2, bssid=None, timeout=90)
STATIC mp_obj_t cc3100_connect(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_ssid, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_key, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_security, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_SMALL_INT(SL_SEC_TYPE_WPA_WPA2)} },
        { MP_QSTR_bssid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_SMALL_INT(90) } },
        { MP_QSTR_eapmethod, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_SMALL_INT(SL_ENT_EAP_METHOD_TTLS_MSCHAPv2)} },
        { MP_QSTR_username,  MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_anonname,  MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
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

        if (!MP_OBJ_IS_INT(args[2].u_obj))
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "invalid 'security' parameter\n"));
        sec = mp_obj_get_int(args[2].u_obj);
    }

    // get bssid
    const char *bssid = NULL;
    if (args[3].u_obj != mp_const_none) {
        bssid = mp_obj_str_get_str(args[3].u_obj);
    }

    mp_int_t timeout = -1;
    if (MP_OBJ_IS_INT(args[4].u_obj)) {
      timeout = mp_obj_get_int(args[4].u_obj) * 1000;
    }

    SlSecParams_t sec_params;
    sec_params.Type = sec;
    sec_params.Key = (int8_t*)key;
    sec_params.KeyLen = key_len;

    SlSecParamsExt_t sec_ext_params, *use_ext = NULL;
    if (sec == SL_SEC_TYPE_WPA_ENT) {
        mp_uint_t len;

        memset(&sec_ext_params, 0, sizeof(sec_ext_params));
        if (!MP_OBJ_IS_INT(args[5].u_obj))
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "invalid 'eapmethod' parameter\n"));
        sec_ext_params.EapMethod = mp_obj_get_int(args[5].u_obj);
        if (args[6].u_obj != mp_const_none) {
            sec_ext_params.User = mp_obj_str_get_data(args[6].u_obj, &len);
            sec_ext_params.UserLen = len;
        }
        if (args[7].u_obj != mp_const_none) {
            sec_ext_params.AnonUser = mp_obj_str_get_data(args[7].u_obj, &len);
            sec_ext_params.AnonUserLen = len;
        }
        use_ext = &sec_ext_params;
    }

    // connect to AP
    printf("Connect to AP\n");
    if (sl_WlanConnect((int8_t*)ssid, ssid_len, (uint8_t*)bssid, &sec_params, use_ext)!= 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
          "could not connect to ssid=%s, sec=%d, key=%s\n", ssid, sec, key));
    }

    if (timeout >= 0) {
      // Wait until connected or timeout, calling simplelink loop
      uint32_t start = HAL_GetTick();
      while (!(ip_obtained && wlan_connected) && ((HAL_GetTick() - start) < timeout) ){
        _SlNonOsMainLoopTask();
      }
      if (!wlan_connected || !ip_obtained) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
          "timed out connecting to ssid=%s, sec=%d, key=%s\n", ssid, sec, key));
      }
      sl_WlanRxStatStart();
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(cc3100_connect_obj, 1, cc3100_connect);

STATIC mp_obj_t cc3100_disconnect(mp_obj_t self_in) {
    sl_WlanRxStatStop(); 
    sl_WlanDisconnect();
    while ((wlan_connected)){
      _SlNonOsMainLoopTask();
    }
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

STATIC mp_obj_t cc3100_ifconfig(mp_obj_t self_in) {
    // These are separate calls on the cc3100
    // mac
    _i32 retVal;
    unsigned char dummy;
    unsigned char mac[6];
    unsigned char len = sizeof(mac);

    mp_obj_t tuple[7];
    VSTR_FIXED(mac_vstr, 18);
    retVal = sl_NetCfgGet(SL_MAC_ADDRESS_GET, &dummy, &len, mac);
    if (retVal == 0) {
      vstr_printf(&mac_vstr, "%02x:%02x:%02x:%02x:%02x:%02x", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
      tuple[5] = mp_obj_new_str(mac_vstr.buf, mac_vstr.len, false);
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "Error %d getting MAC address", retVal));
    }
    //return mp_const_none;

    unsigned char iplen = sizeof(SlNetCfgIpV4Args_t);
    unsigned char dhcpIsOn = 0;
    SlNetCfgIpV4Args_t ipV4 = {0};
    VSTR_FIXED(ip_vstr, 15);
    VSTR_FIXED(mask_vstr, 15);
    VSTR_FIXED(gw_vstr, 15);
    VSTR_FIXED(dns_vstr, 15);
    retVal = sl_NetCfgGet(SL_IPV4_STA_P2P_CL_GET_INFO,&dhcpIsOn,&iplen,(unsigned char *)&ipV4);
    if(retVal == 0) {
      vstr_printf(&ip_vstr, "%03d.%03d.%03d.%03d", SL_IPV4_BYTE(ipV4.ipV4,3), SL_IPV4_BYTE(ipV4.ipV4,2), SL_IPV4_BYTE(ipV4.ipV4,1), SL_IPV4_BYTE(ipV4.ipV4,0));
      vstr_printf(&mask_vstr, "%03d.%03d.%03d.%03d", SL_IPV4_BYTE(ipV4.ipV4Mask,3), SL_IPV4_BYTE(ipV4.ipV4Mask,2), SL_IPV4_BYTE(ipV4.ipV4Mask,1), SL_IPV4_BYTE(ipV4.ipV4Mask,0));
      vstr_printf(&gw_vstr, "%03d.%03d.%03d.%03d", SL_IPV4_BYTE(ipV4.ipV4Gateway,3), SL_IPV4_BYTE(ipV4.ipV4Gateway,2), SL_IPV4_BYTE(ipV4.ipV4Gateway,1), SL_IPV4_BYTE(ipV4.ipV4Gateway,0));
      vstr_printf(&dns_vstr, "%03d.%03d.%03d.%03d", SL_IPV4_BYTE(ipV4.ipV4DnsServer,3), SL_IPV4_BYTE(ipV4.ipV4DnsServer,2), SL_IPV4_BYTE(ipV4.ipV4DnsServer,1), SL_IPV4_BYTE(ipV4.ipV4DnsServer,0));
      tuple[0] = mp_obj_new_str(ip_vstr.buf, ip_vstr.len, false);
      tuple[1] = mp_obj_new_str(mask_vstr.buf, mask_vstr.len, false);
      tuple[2] = mp_obj_new_str(gw_vstr.buf, gw_vstr.len, false);
      tuple[3] = mp_obj_new_str(dns_vstr.buf, dns_vstr.len, false);
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "Error %d getting IP address", retVal));
    }
    return mp_obj_new_tuple(MP_ARRAY_SIZE(tuple), tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(cc3100_ifconfig_obj, cc3100_ifconfig);

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

STATIC mp_obj_t cc3100_list_aps(mp_obj_t self_in) {
  /* Enable scan */ //TODO: check if already enabled
  #define SL_SCAN_ENABLE 1 
  int retVal;
  uint8_t configOpt = SL_SCAN_POLICY(1);
  uint8_t configVal = 60;
  LOG_INFO("Enable Scan");
  retVal = sl_WlanPolicySet(SL_POLICY_SCAN, configOpt, &configVal, sizeof(configVal));
  LOG_COND_RET(retVal >= 0, -1);
  mp_hal_delay_ms(1000); // Wait 1 second to ensure scan starts
  
  int runningIdx, numOfEntries, idx;
  Sl_WlanNetworkEntry_t netentry = {0};
  Sl_WlanNetworkEntry_t netEntries[20];
  memset(netEntries, 0, sizeof(netEntries));
  
  numOfEntries = 20;
  runningIdx = 0;
  idx = 0;
  retVal = sl_WlanGetNetworkList(runningIdx,numOfEntries,&netEntries[runningIdx]);
  
  /*
   * Because of a bug user should either read the maximum entries or read
   * entries one by one from the end and check for duplicates. Once a duplicate
   * is found process should be stopped.
   */
  /* get scan results - one by one */
  runningIdx = 20;
  numOfEntries = 1;
  memset(netEntries, 0, sizeof(netEntries));
  
  do
  { 
    runningIdx--;
    retVal = sl_WlanGetNetworkList(runningIdx, numOfEntries, &netentry);
    if(retVal < numOfEntries)
      nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Error getting AP list"));

    if(idx > 0)
    {
        if(0 == memcmp(netentry.bssid,
                  netEntries[idx - 1].bssid, SL_BSSID_LENGTH))
        {
            /* Duplicate entry */
            break;
        }
    }

    memcpy(&netEntries[idx], &netentry, sizeof(Sl_WlanNetworkEntry_t));
    idx++;

} while (runningIdx > 0);
  mp_obj_t returnVal = mp_obj_new_list(0, NULL);
  for(int i = 0; i < idx; i++) {
    mp_obj_t entry = mp_obj_new_dict(4);
    mp_obj_dict_store(entry,
                      mp_obj_new_str("ssid", strlen("ssid"), false),
                      mp_obj_new_str(netEntries[i].ssid, netEntries[i].ssid_len, false));
    mp_obj_dict_store(entry,
                      mp_obj_new_str("bssid", strlen("bssid"), false),
                      mp_obj_new_bytearray(SL_BSSID_LENGTH,netEntries[i].bssid));
    mp_obj_dict_store(entry,
                      mp_obj_new_str("rssi", strlen("rssi"), false),
                      MP_OBJ_NEW_SMALL_INT(netEntries[i].rssi));
    mp_obj_dict_store(entry,
                      mp_obj_new_str("security", strlen("security"), false),
                      MP_OBJ_NEW_SMALL_INT(netEntries[i].sec_type));
    mp_obj_list_append(returnVal, entry);
  }
  return returnVal;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(cc3100_list_aps_obj, cc3100_list_aps);

STATIC mp_obj_t cc3100_get_rssi(mp_obj_t self_in) {
  SlGetRxStatResponse_t rxStatResp;
  _i16 avgRssi, retVal;
  retVal = sl_WlanRxStatGet(&rxStatResp, 0);
  if (retVal == 0) {
    avgRssi = rxStatResp.AvarageMgMntRssi;
    return MP_OBJ_NEW_SMALL_INT(avgRssi);
  }
  nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Error getting RSSI"));
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(cc3100_get_rssi_obj, cc3100_get_rssi);

STATIC mp_obj_t cc3100_settime(mp_obj_t self_in, mp_obj_t tuple) {
  mp_uint_t len;
  mp_obj_t *elem;
  _i32 retVal = -1;
  SlDateTime_t dateTime= {0};

  mp_obj_get_array(tuple, &len, &elem);

  // localtime generates a tuple of len 8. CPython uses 9, so we accept both, also we only need 6, so thats fine too.
  if (len < 6 || len > 9) {
      nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "CC3100.settime needs a tuple of length 6-9 (%d given)", len));
  }

  dateTime.sl_tm_day = mp_obj_get_int(elem[2]);
  dateTime.sl_tm_mon = mp_obj_get_int(elem[1]);
  dateTime.sl_tm_year = mp_obj_get_int(elem[0]);
  dateTime.sl_tm_hour = mp_obj_get_int(elem[3]);
  dateTime.sl_tm_min = mp_obj_get_int(elem[4]);
  dateTime.sl_tm_sec = mp_obj_get_int(elem[5]);

  retVal = sl_DevSet(SL_DEVICE_GENERAL_CONFIGURATION,SL_DEVICE_GENERAL_CONFIGURATION_DATE_TIME,
        sizeof(SlDateTime_t),(_u8 *)(&dateTime));
  if (retVal != 0)
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "Error setting time"));

  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(cc3100_settime_obj, cc3100_settime);

STATIC mp_obj_t cc3100_setcountry(mp_obj_t self_in, mp_obj_t cc) {
  _i32 retVal = -1;
  mp_uint_t country_len;
  const unsigned char *country = (const unsigned char *)mp_obj_str_get_data(cc, &country_len);

  if (!country || country_len != 2)
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "Invalid country code given"));

  retVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, WLAN_GENERAL_PARAM_OPT_COUNTRY_CODE, 2, country);
  if (retVal != 0)
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "Error %d setting country code", retVal));

  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(cc3100_setcountry_obj, cc3100_setcountry);

STATIC mp_obj_t cc3100_file_open(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
  static const mp_arg_t allowed_args[] = {
      { MP_QSTR_filename, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
      { MP_QSTR_mode,  MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = FS_MODE_OPEN_READ} },
      { MP_QSTR_size,  MP_ARG_INT, {0} },
  };

  // parse args
  mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
  mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
  
  _i32 retVal;
  _i32 pFileHandle = 0;
  _i32 mode = args[1].u_int;
  _i32 size = args[2].u_int;
  
  if (mode == MP_SL_FS_CREATE && size <= 0) {
      // Create with no size
      nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "Create mode needs a valid size"));
  }
  // TODO: check max size
  
  _u32 cc_fs_mode;
  switch(mode){
    case MP_SL_FS_READ:
      cc_fs_mode = FS_MODE_OPEN_READ;
      break;
    case MP_SL_FS_WRITE:
      cc_fs_mode = FS_MODE_OPEN_WRITE;
      break;
    case MP_SL_FS_CREATE:
      cc_fs_mode = FS_MODE_OPEN_CREATE(size, 0); // No special flags
      break;
    default: 
      nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "Invalid Mode"));
      break;
  }
  
  retVal = sl_FsOpen(mp_obj_str_get_str(args[0].u_obj),cc_fs_mode,NULL,&pFileHandle);
  if (!retVal == 0)
  {
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "Error opening file: %d", retVal));
  }
  return mp_obj_new_int_from_uint(pFileHandle);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(cc3100_file_open_obj, 1, cc3100_file_open);


STATIC mp_obj_t cc3100_file_close(mp_obj_t self_in, mp_obj_t fh) {
  _i32 retVal;
  _i32 pFileHandle = mp_obj_get_int(fh);
  retVal = sl_FsClose(pFileHandle,0,0,0);
  if (!retVal == 0)
  {
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "Error closing file: %d", retVal));
  }
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(cc3100_file_close_obj, cc3100_file_close);


STATIC mp_obj_t cc3100_file_write(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
  static const mp_arg_t allowed_args[] = {
      { MP_QSTR_fh, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
      { MP_QSTR_offset,  MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
      { MP_QSTR_data,  MP_ARG_REQUIRED | MP_ARG_OBJ, {0} },
  };

  // parse args
  mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
  mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);


  mp_buffer_info_t bufinfo;
  mp_get_buffer_raise(args[2].u_obj, &bufinfo, MP_BUFFER_READ);
  _i32 retVal;
  _i32 fileHandle = args[0].u_int;
  _i32 offset = args[1].u_int;
  retVal = sl_FsWrite(fileHandle, offset, (_u8 *)bufinfo.buf, bufinfo.len);
  if (retVal <= 0)
  {
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "Error writing to file: %d", retVal));
  }  
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(cc3100_file_write_obj, 1,cc3100_file_write);


STATIC mp_obj_t cc3100_file_read(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
  static const mp_arg_t allowed_args[] = {
      { MP_QSTR_fh,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
      { MP_QSTR_offset, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
      { MP_QSTR_len,    MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 1024} },
  };
  unsigned char buf[1024];

  // parse args
  mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
  mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

  _i32 fileHandle = args[0].u_int;
  _u32 offset = args[1].u_int;
  _u32 len = args[2].u_int;
  _i32 retVal;

  if (len > sizeof(buf))
    len = sizeof(buf);

  retVal = sl_FsRead(fileHandle, offset, buf, len);
  if (retVal <= 0)
  {
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "Error reading from file: %d", retVal));
  }
  return mp_obj_new_bytearray(retVal, buf);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(cc3100_file_read_obj, 1,cc3100_file_read);


STATIC mp_obj_t cc3100_file_del(mp_obj_t *self, mp_obj_t *nameobj) {
  _i32 retVal;
  unsigned char *name = (unsigned char *)mp_obj_str_get_str(nameobj);

  retVal = sl_FsDel(name, 0);
  if (!retVal == 0)
  {
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "Error deleting file: %d", retVal));
  }
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(cc3100_file_del_obj, cc3100_file_del);


STATIC mp_obj_t cc3100_fw_open(mp_obj_t self_in) {
  _i32 retVal;
  _i32 pFileHandle = 0;
  _u32 token;
  
  retVal = sl_FsOpen("/sys/servicepack.ucf", FS_MODE_OPEN_CREATE(131072,_FS_FILE_OPEN_FLAG_SECURE|_FS_FILE_OPEN_FLAG_COMMIT|_FS_FILE_PUBLIC_WRITE), &token, &pFileHandle);
  
  if (!retVal == 0)
  {
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "Error opening file: %d", retVal));
  }
  return mp_obj_new_int_from_uint(pFileHandle);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(cc3100_fw_open_obj, cc3100_fw_open);

STATIC mp_obj_t cc3100_fw_close(mp_obj_t self_in, mp_obj_t fh, mp_obj_t sig) {
  _i32 retVal;
  _i32 pFileHandle = mp_obj_get_int(fh);
  
  mp_buffer_info_t sigbufinfo;
  mp_get_buffer_raise(sig, &sigbufinfo, MP_BUFFER_READ);
  
  retVal = sl_FsClose(pFileHandle,0,(_u8 *)sigbufinfo.buf, sigbufinfo.len);
  if (!retVal == 0)
  {
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "Error closing file: %d", retVal));
  }
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(cc3100_fw_close_obj, cc3100_fw_close);

STATIC mp_obj_t cc3100_get_mac(mp_obj_t self_in) {
  _i32 retVal;
  unsigned char dummy;
  unsigned char mac[6];
  unsigned char len = sizeof(mac);

  retVal = sl_NetCfgGet(SL_MAC_ADDRESS_GET, &dummy, &len, mac);
  if (retVal == 0) {
    return mp_obj_new_bytearray(sizeof(mac), mac);
  }
  nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "Error %d getting MAC address", retVal));
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(cc3100_get_mac_obj, cc3100_get_mac);

STATIC mp_obj_t cc3100_version(mp_obj_t self_in) {
    int32_t retVal = -1;
    uint8_t pConfigOpt;
    uint8_t pConfigLen;
    SlVersionFull ver;

    pConfigLen = sizeof(ver);
    pConfigOpt = SL_DEVICE_GENERAL_VERSION;
    retVal = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &pConfigOpt, &pConfigLen, (_u8 *)(&ver));
 
    if (retVal >= 0) {
        STATIC const qstr version_fields[] = {
            MP_QSTR_chip, MP_QSTR_mac, MP_QSTR_phy, MP_QSTR_nwp, MP_QSTR_rom, MP_QSTR_host
        };
        mp_obj_t tuple[6];
        tuple[0] = mp_obj_new_int_from_uint(ver.ChipFwAndPhyVersion.ChipId);
        
        vstr_t vstr;
        vstr_init(&vstr, 50);
        vstr_printf(&vstr, "31.%lu.%lu.%lu.%lu", ver.ChipFwAndPhyVersion.FwVersion[0],ver.ChipFwAndPhyVersion.FwVersion[1],ver.ChipFwAndPhyVersion.FwVersion[2],ver.ChipFwAndPhyVersion.FwVersion[3]);
        tuple[1] = mp_obj_new_str(vstr.buf, vstr.len, false);
        vstr_init(&vstr, 50);
        vstr_printf(&vstr, "%d.%d.%d.%d", ver.ChipFwAndPhyVersion.PhyVersion[0],ver.ChipFwAndPhyVersion.PhyVersion[1],ver.ChipFwAndPhyVersion.PhyVersion[2],ver.ChipFwAndPhyVersion.PhyVersion[3]);
        tuple[2] = mp_obj_new_str(vstr.buf, vstr.len, false);
        vstr_init(&vstr, 50);
        vstr_printf(&vstr, "%lu.%lu.%lu.%lu", ver.NwpVersion[0],ver.NwpVersion[1],ver.NwpVersion[2],ver.NwpVersion[3]);
        tuple[3] = mp_obj_new_str(vstr.buf, vstr.len, false);
        tuple[4] = mp_obj_new_int_from_uint(ver.RomVersion);
        vstr_init(&vstr, 50);
        vstr_printf(&vstr, "%lu.%lu.%lu.%lu", SL_MAJOR_VERSION_NUM,SL_MINOR_VERSION_NUM,SL_VERSION_NUM,SL_SUB_VERSION_NUM);
        tuple[5] = mp_obj_new_str(vstr.buf, vstr.len, false);

        return mp_obj_new_attrtuple(version_fields, 6, tuple);
    } 
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(cc3100_version_obj, cc3100_version);

/******************************************************************************/
// Micro Python bindings; CC3100 class

typedef struct _cc3100_obj_t {
    mp_obj_base_t base;
} cc3100_obj_t;

STATIC const cc3100_obj_t cc3100_obj = {{(mp_obj_type_t*)&mod_network_nic_type_cc3100}};


STATIC mp_obj_t cc3100_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {

    wlan_connected = false;
    ip_obtained    = false;

    uint8_t val = 1;
    uint8_t power = 0;
    int32_t retVal = -1;
    int32_t mode = -1;
    static const unsigned char defaultcountry[2] = "EU";

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

    if (sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, WLAN_GENERAL_PARAM_OPT_COUNTRY_CODE, 2, defaultcountry)) {
        LOG_ERR("failed to set country code!");
    }

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

    /* Set connection policy to nothing magic  */
    LOG_INFO("Set connection policy");
    retVal = sl_WlanPolicySet(SL_POLICY_CONNECTION, SL_CONNECTION_POLICY(0, 0, 0, 0, 0), NULL, 0);
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
    { MP_OBJ_NEW_QSTR(MP_QSTR_ifconfig),        (mp_obj_t)&cc3100_ifconfig_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_update),          (mp_obj_t)&cc3100_update_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sleep),           (mp_obj_t)&cc3100_sleep_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_list_aps),        (mp_obj_t)&cc3100_list_aps_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_rssi),        (mp_obj_t)&cc3100_get_rssi_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_settime),         (mp_obj_t)&cc3100_settime_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_setcountry),      (mp_obj_t)&cc3100_setcountry_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_file_open),       (mp_obj_t)&cc3100_file_open_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_file_close),      (mp_obj_t)&cc3100_file_close_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_file_write),      (mp_obj_t)&cc3100_file_write_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_file_read),       (mp_obj_t)&cc3100_file_read_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_file_del),        (mp_obj_t)&cc3100_file_del_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_fw_open),         (mp_obj_t)&cc3100_fw_open_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_fw_close),        (mp_obj_t)&cc3100_fw_close_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_mac),         (mp_obj_t)&cc3100_get_mac_obj}, 
    { MP_OBJ_NEW_QSTR(MP_QSTR_version),         (mp_obj_t)&cc3100_version_obj}, 

    // class constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_SEC_OPEN), MP_OBJ_NEW_SMALL_INT(SL_SEC_TYPE_OPEN) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WEP), MP_OBJ_NEW_SMALL_INT(SL_SEC_TYPE_WEP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WPA), MP_OBJ_NEW_SMALL_INT(SL_SEC_TYPE_WPA) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WPA2), MP_OBJ_NEW_SMALL_INT(SL_SEC_TYPE_WPA_WPA2) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WPA_ENT), MP_OBJ_NEW_SMALL_INT(SL_SEC_TYPE_WPA_ENT) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SEC_WEP), MP_OBJ_NEW_SMALL_INT(SL_SEC_TYPE_WEP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SEC_WPA), MP_OBJ_NEW_SMALL_INT(SL_SEC_TYPE_WPA) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SEC_WPA2), MP_OBJ_NEW_SMALL_INT(SL_SEC_TYPE_WPA_WPA2) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SEC_WPA_ENT), MP_OBJ_NEW_SMALL_INT(SL_SEC_TYPE_WPA_ENT) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_EAP_METHOD_TLS),                      MP_OBJ_NEW_SMALL_INT(SL_ENT_EAP_METHOD_TLS) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_EAP_METHOD_TTLS_TLS),                 MP_OBJ_NEW_SMALL_INT(SL_ENT_EAP_METHOD_TTLS_TLS) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_EAP_METHOD_TTLS_MSCHAPv2),            MP_OBJ_NEW_SMALL_INT(SL_ENT_EAP_METHOD_TTLS_MSCHAPv2) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_EAP_METHOD_TTLS_PSK),                 MP_OBJ_NEW_SMALL_INT(SL_ENT_EAP_METHOD_TTLS_PSK) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_EAP_METHOD_PEAP0_TLS),                MP_OBJ_NEW_SMALL_INT(SL_ENT_EAP_METHOD_PEAP0_TLS) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_EAP_METHOD_PEAP0_MSCHAPv2),           MP_OBJ_NEW_SMALL_INT(SL_ENT_EAP_METHOD_PEAP0_MSCHAPv2) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_EAP_METHOD_PEAP0_PSK),                MP_OBJ_NEW_SMALL_INT(SL_ENT_EAP_METHOD_PEAP0_PSK) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_EAP_METHOD_PEAP1_TLS),                MP_OBJ_NEW_SMALL_INT(SL_ENT_EAP_METHOD_PEAP1_TLS) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_EAP_METHOD_PEAP1_MSCHAPv2),           MP_OBJ_NEW_SMALL_INT(SL_ENT_EAP_METHOD_PEAP1_MSCHAPv2) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_EAP_METHOD_PEAP1_PSK),                MP_OBJ_NEW_SMALL_INT(SL_ENT_EAP_METHOD_PEAP1_PSK) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_EAP_METHOD_FAST_AUTH_PROVISIONING),   MP_OBJ_NEW_SMALL_INT(SL_ENT_EAP_METHOD_FAST_AUTH_PROVISIONING) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_EAP_METHOD_FAST_UNAUTH_PROVISIONING), MP_OBJ_NEW_SMALL_INT(SL_ENT_EAP_METHOD_FAST_UNAUTH_PROVISIONING) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_EAP_METHOD_FAST_NO_PROVISIONING),     MP_OBJ_NEW_SMALL_INT(SL_ENT_EAP_METHOD_FAST_NO_PROVISIONING) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SOL_SOCKET), MP_OBJ_NEW_SMALL_INT(SL_SOL_SOCKET)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_RCVBUF), MP_OBJ_NEW_SMALL_INT(SL_SO_RCVBUF)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_KEEPALIVE) , MP_OBJ_NEW_SMALL_INT(SL_SO_KEEPALIVE )},
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_RCVTIME0), MP_OBJ_NEW_SMALL_INT(SL_SO_RCVTIMEO)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_NONBLOCKING), MP_OBJ_NEW_SMALL_INT(SL_SO_NONBLOCKING)},

    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_SECMETHOD), MP_OBJ_NEW_SMALL_INT(SL_SO_SECMETHOD )},
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_SEC_METHOD_SSLV3), MP_OBJ_NEW_SMALL_INT(SL_SO_SEC_METHOD_SSLV3 )},
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_SEC_METHOD_TLSV1), MP_OBJ_NEW_SMALL_INT(SL_SO_SEC_METHOD_TLSV1 )},
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_SEC_METHOD_TLSV1_1), MP_OBJ_NEW_SMALL_INT(SL_SO_SEC_METHOD_TLSV1_1 )},
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_SEC_METHOD_TLSV1_2), MP_OBJ_NEW_SMALL_INT(SL_SO_SEC_METHOD_TLSV1_2 )},
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_SEC_METHOD_SSLv3_TLSV1_2), MP_OBJ_NEW_SMALL_INT(SL_SO_SEC_METHOD_SSLv3_TLSV1_2 )},
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_SEC_METHOD_DLSV1), MP_OBJ_NEW_SMALL_INT(SL_SO_SEC_METHOD_DLSV1 )},


    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_SECURE_MASK), MP_OBJ_NEW_SMALL_INT(SL_SO_SECURE_MASK)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_SEC_MASK_SSL_RSA_WITH_RC4_128_SHA), MP_OBJ_NEW_SMALL_INT(SL_SEC_MASK_SSL_RSA_WITH_RC4_128_SHA)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_SEC_MASK_SSL_RSA_WITH_RC4_128_MD5), MP_OBJ_NEW_SMALL_INT(SL_SEC_MASK_SSL_RSA_WITH_RC4_128_MD5)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_SEC_MASK_TLS_RSA_WITH_AES_256_CBC_SHA), MP_OBJ_NEW_SMALL_INT(SL_SEC_MASK_TLS_RSA_WITH_AES_256_CBC_SHA)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_SEC_MASK_TLS_DHE_RSA_WITH_AES_256_CBC_SHA), MP_OBJ_NEW_SMALL_INT(SL_SEC_MASK_TLS_DHE_RSA_WITH_AES_256_CBC_SHA)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_SEC_MASK_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA), MP_OBJ_NEW_SMALL_INT(SL_SEC_MASK_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_SEC_MASK_TLS_ECDHE_RSA_WITH_RC4_128_SHA), MP_OBJ_NEW_SMALL_INT(SL_SEC_MASK_TLS_ECDHE_RSA_WITH_RC4_128_SHA)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_SEC_MASK_TLS_RSA_WITH_AES_128_CBC_SHA256), MP_OBJ_NEW_SMALL_INT(SL_SEC_MASK_TLS_RSA_WITH_AES_128_CBC_SHA256)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_SEC_MASK_TLS_RSA_WITH_AES_256_CBC_SHA256), MP_OBJ_NEW_SMALL_INT(SL_SEC_MASK_TLS_RSA_WITH_AES_256_CBC_SHA256)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_SEC_MASK_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256), MP_OBJ_NEW_SMALL_INT(SL_SEC_MASK_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_SEC_MASK_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256), MP_OBJ_NEW_SMALL_INT(SL_SEC_MASK_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256)},

    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_CHANGE_CHANNEL), MP_OBJ_NEW_SMALL_INT(SL_SO_CHANGE_CHANNEL)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_SECURE_FILES_PRIVATE_KEY_FILE_NAME), MP_OBJ_NEW_SMALL_INT(SL_SO_SECURE_FILES_PRIVATE_KEY_FILE_NAME)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_SECURE_FILES_CERTIFICATE_FILE_NAME), MP_OBJ_NEW_SMALL_INT(SL_SO_SECURE_FILES_CERTIFICATE_FILE_NAME )},
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_SECURE_FILES_CA_FILE_NAME), MP_OBJ_NEW_SMALL_INT(SL_SO_SECURE_FILES_CA_FILE_NAME)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_SECURE_FILES_DH_KEY_FILE_NAME), MP_OBJ_NEW_SMALL_INT(SL_SO_SECURE_FILES_DH_KEY_FILE_NAME)},

    { MP_OBJ_NEW_QSTR(MP_QSTR_FILE_MODE_READ), MP_OBJ_NEW_SMALL_INT(MP_SL_FS_READ)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_FILE_MODE_WRITE), MP_OBJ_NEW_SMALL_INT(MP_SL_FS_WRITE)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_FILE_MODE_CREATE), MP_OBJ_NEW_SMALL_INT(MP_SL_FS_CREATE)},

};

STATIC MP_DEFINE_CONST_DICT(cc3100_locals_dict, cc3100_locals_dict_table);

STATIC int cc3100_socket_socket(mod_network_socket_obj_t *socket, int *_errno) {
    if (socket->u_param.domain != MOD_NETWORK_AF_INET) {
        *_errno = EAFNOSUPPORT;
        return -1;
    }

    mp_uint_t type;
    mp_uint_t proto = 0;
    switch (socket->u_param.type) {
        case MOD_NETWORK_SOCK_STREAM: type = SL_SOCK_STREAM; break;
        case MOD_NETWORK_SOCK_DGRAM: type = SL_SOCK_DGRAM; break;
        case MOD_NETWORK_SOCK_RAW: type = SL_SOCK_RAW; break;
        default: *_errno = EINVAL; return -1;
    }

    if (socket->u_param.proto == MOD_NETWORK_SEC_SOCKET)
    {
        // SSL Socket
        if (socket->u_param.type != MOD_NETWORK_SOCK_STREAM ){
          *_errno = EINVAL; return -1; // Only support TCP SSL
        }
        // To start we will setup ssl sockets ignoring certificates
        proto = SL_SEC_SOCKET;
    }

    // open socket
    int fd = sl_Socket(SL_AF_INET, type, proto);
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
    if (!cc3100_get_fd_closed_state(socket->u_state)) {
      sl_Close(socket->u_state);
      cc3100_set_fd_closed_state(socket->u_state);
    }
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
    if (ret != 0 && ret != SL_ESECSNOVERIFY) {
        *_errno = ret;
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

  int ret;
  // Todo : Review and Clean this up
  if (opt == SL_SO_SECMETHOD) {
    SlSockSecureMethod method;
    method.secureMethod = (unsigned int) *(unsigned int *)optval;
    ret = sl_SetSockOpt(socket->u_state, level, opt, (_u8 *) &method, sizeof(method));
  } else {
    ret = sl_SetSockOpt(socket->u_state, level, opt, optval, optlen);
  }

  if (ret < 0) {
    *_errno = ret;
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
         ip1 = pEventData->ip & 0xff;
         ip2 = (pEventData->ip >> 8) & 0xff;
         ip3 = (pEventData->ip >> 16) & 0xff;
         ip4 = (pEventData->ip >> 24) & 0xff;
         printf("IP:  %d.%d.%d.%d\n",ip4,ip3,ip2,ip1);
         ip1 = pEventData->gateway & 0xff;
         ip2 = (pEventData->gateway >> 8) & 0xff;
         ip3 = (pEventData->gateway >> 16) & 0xff;
         ip4 = (pEventData->gateway >> 24) & 0xff;
         printf("GW:  %d.%d.%d.%d\n",ip4,ip3,ip2,ip1);
         ip1 = pEventData->dns & 0xff;
         ip2 = (pEventData->dns >> 8) & 0xff;
         ip3 = (pEventData->dns >> 16) & 0xff;
         ip4 = (pEventData->dns >> 24) & 0xff;
         printf("DNS: %d.%d.%d.%d\n",ip4,ip3,ip2,ip1);

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
