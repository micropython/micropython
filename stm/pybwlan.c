#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stm32f4xx.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_tim.h>
#include <stm32f4xx_pwr.h>
#include <stm32f4xx_rtc.h>
#include <stm32f4xx_usart.h>
#include <stm_misc.h>
#include "std.h"

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "systick.h"

#include "nlr.h"
#include "misc.h"
#include "lexer.h"
#include "parse.h"
#include "obj.h"
#include "runtime.h"

#include "cc3k/ccspi.h"
#include "cc3k/hci.h"
#include "cc3k/socket.h"
#include "cc3k/netapp.h"
#include "cc3k/wlan.h"
#include "cc3k/nvmem.h"

mp_obj_t pyb_wlan_connect(uint n_args, const mp_obj_t *args) {
  const char *ap;
  const char *key;
    if (n_args == 2) {
        ap = qstr_str(mp_obj_get_qstr(args[0]));
        key = qstr_str(mp_obj_get_qstr(args[1]));
    } else {
        ap = "your-ssid";
        key = "your-password";
    }
    // might want to set wlan_ioctl_set_connection_policy
    int ret = wlan_connect(WLAN_SEC_WPA2, ap, strlen(ap), NULL, (byte*)key, strlen(key));
    return mp_obj_new_int(ret);
}

mp_obj_t pyb_wlan_disconnect(void) {
    int ret = wlan_disconnect();
    return mp_obj_new_int(ret);
}

mp_obj_t decode_addr(unsigned char *ip, int n_bytes) {
    char data[64] = "";
    if (n_bytes == 4) {
        snprintf(data, 64, "%u.%u.%u.%u", ip[3], ip[2], ip[1], ip[0]);
    } else if (n_bytes == 6) {
        snprintf(data, 64, "%02x:%02x:%02x:%02x:%02x:%02x", ip[5], ip[4], ip[3], ip[2], ip[1], ip[0]);
    } else if (n_bytes == 32) {
        snprintf(data, 64, "%s", ip);
    }
    return mp_obj_new_str(qstr_from_strn(data, strlen(data)));
}

void decode_addr_and_store(mp_obj_t object, qstr q_attr, unsigned char *ip, int n_bytes) {
    rt_store_attr(object, q_attr, decode_addr(ip, n_bytes));
}

static mp_obj_t net_address_type = MP_OBJ_NULL;

mp_obj_t pyb_wlan_get_ip(void) {
    tNetappIpconfigRetArgs ipconfig;
    netapp_ipconfig(&ipconfig);

    // If byte 1 is 0 we don't have a valid address
    if (ipconfig.aucIP[3] == 0) {
        return mp_const_none;
    }

    // if it doesn't already exist, make a new empty class for NetAddress objects
    if (net_address_type == MP_OBJ_NULL) {
        net_address_type = mp_obj_new_type(QSTR_FROM_STR_STATIC("NetAddress"), mp_const_empty_tuple, mp_obj_new_dict(0));
    }

    // make a new NetAddress object
    mp_obj_t net_addr = rt_call_function_0(net_address_type);

    // fill the NetAddress object with data
    decode_addr_and_store(net_addr, QSTR_FROM_STR_STATIC("ip"), &ipconfig.aucIP[0], 4);
    decode_addr_and_store(net_addr, QSTR_FROM_STR_STATIC("subnet"), &ipconfig.aucSubnetMask[0], 4);
    decode_addr_and_store(net_addr, QSTR_FROM_STR_STATIC("gateway"), &ipconfig.aucDefaultGateway[0], 4);
    decode_addr_and_store(net_addr, QSTR_FROM_STR_STATIC("dhcp"), &ipconfig.aucDHCPServer[0], 4);
    decode_addr_and_store(net_addr, QSTR_FROM_STR_STATIC("dns"), &ipconfig.aucDNSServer[0], 4);
    decode_addr_and_store(net_addr, QSTR_FROM_STR_STATIC("mac"), &ipconfig.uaMacAddr[0], 6);
    decode_addr_and_store(net_addr, QSTR_FROM_STR_STATIC("ssid"), &ipconfig.uaSSID[0], 32);

    return net_addr;
}

uint32_t last_ip = 0; // XXX such a hack!
mp_obj_t pyb_wlan_get_host(mp_obj_t host_name) {
    const char *host = qstr_str(mp_obj_get_qstr(host_name));
    uint32_t ip;
    if (gethostbyname(host, strlen(host), &ip) < 0) {
        printf("gethostbyname failed\n");
        return mp_const_none;
    }
    if (ip == 0) {
        // unknown host
        return mp_const_none;
    }
    last_ip = ip;
    byte ip_data[4];
    ip_data[0] = ((ip >> 0) & 0xff);
    ip_data[1] = ((ip >> 8) & 0xff);
    ip_data[2] = ((ip >> 16) & 0xff);
    ip_data[3] = ((ip >> 24) & 0xff);
    return decode_addr(ip_data, 4);
}

mp_obj_t pyb_wlan_http_get(mp_obj_t host_name, mp_obj_t host_path) {
    if (host_name == mp_const_none) {
        last_ip = (192 << 24) | (168 << 16) | (0 << 8) | (3);
    } else {
        if (pyb_wlan_get_host(host_name) == mp_const_none) {
            nlr_jump(mp_obj_new_exception_msg(QSTR_FROM_STR_STATIC("WlanError"), "unknown host"));
        }
    }
    int sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sd < 0) {
        nlr_jump(mp_obj_new_exception_msg_varg(QSTR_FROM_STR_STATIC("WlanError"), "socket failed: %d", sd));
    }
    //printf("socket seemed to work\n");
    //sys_tick_delay_ms(200);
    sockaddr_in remote;
    memset(&remote, 0, sizeof(sockaddr_in));
    remote.sin_family = AF_INET;
    remote.sin_port = htons(80);
    remote.sin_addr.s_addr = htonl(last_ip);
    int ret = connect(sd, (sockaddr*)&remote, sizeof(sockaddr));
    if (ret != 0) {
        nlr_jump(mp_obj_new_exception_msg_varg(QSTR_FROM_STR_STATIC("WlanError"), "connect failed: %d", ret));
    }
    //printf("connect seemed to work\n");
    //sys_tick_delay_ms(200);

    vstr_t *vstr = vstr_new();
    vstr_printf(vstr, "GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: PYBv2\r\n\r\n", qstr_str(mp_obj_get_qstr(host_path)), qstr_str(mp_obj_get_qstr(host_name)));
    const char *query = vstr_str(vstr);

    // send query
    {
        int sent = 0;
        while (sent < strlen(query)) {
            /*
            extern void SpiIntGPIOHandler(void);
            SpiIntGPIOHandler();
            */
            //printf("sending %d bytes\n", strlen(query + sent));
            ret = send(sd, query + sent, strlen(query + sent), 0);
            //printf("sent %d bytes\n", ret);
            if (ret < 0) {
                nlr_jump(mp_obj_new_exception_msg(QSTR_FROM_STR_STATIC("WlanError"), "send failed"));
            }
            sent += ret;
            //sys_tick_delay_ms(200);
        }
    }

    //printf("send seemed to work!\n");
    //sys_tick_delay_ms(5000);

    // receive reply
    mp_obj_t mp_ret = mp_const_none;
    {
        //printf("doing receive\n");
        char buf[64];
        vstr_reset(vstr);

        for (;;) {
            // do a select() call on this socket
            timeval timeout;
            fd_set fd_read;

            memset(&fd_read, 0, sizeof(fd_read));
            FD_SET(sd, &fd_read);

            timeout.tv_sec = 0;
            timeout.tv_usec = 500000; // 500 millisec

            int s = select(sd+1, &fd_read, NULL, NULL, &timeout);
            if (s == 0) {
                // no data available
                break;
            }

            // read data
            ret = recv(sd, buf, 64, 0);
            if (ret < 0) {
                nlr_jump(mp_obj_new_exception_msg_varg(QSTR_FROM_STR_STATIC("WlanError"), "recv failed %d", ret));
            }
            vstr_add_strn(vstr, buf, ret);
        }

        mp_ret = mp_obj_new_str(qstr_from_strn_take(vstr->buf, vstr->alloc, vstr->len));
    }

    closesocket(sd);

    return mp_ret;
}

mp_obj_t pyb_wlan_serve(void) {
    printf("serve socket\n");
    int sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    printf("serve socket got %d\n", sd);
    sys_tick_delay_ms(500);
    if (sd < 0) {
        printf("socket fail\n");
        nlr_jump(mp_obj_new_exception_msg_varg(QSTR_FROM_STR_STATIC("WlanError"), "socket failed: %d", sd));
    }

    /*
    if (setsockopt(sd, SOL_SOCKET, SOCKOPT_ACCEPT_NONBLOCK, SOCK_ON, sizeof(SOCK_ON)) < 0) {
        printf("couldn't set socket as non-blocking\n");
        return mp_const_none;
    }
    */

    sockaddr_in remote;
    memset(&remote, 0, sizeof(sockaddr_in));
    remote.sin_family = AF_INET;
    remote.sin_port = htons(8080);
    remote.sin_addr.s_addr = htonl(0);
    printf("serve bind\n");
    int ret = bind(sd, (sockaddr*)&remote, sizeof(sockaddr));
    printf("serve bind got %d\n", ret);
    sys_tick_delay_ms(100);
    if (ret != 0) {
        printf("bind fail\n");
        nlr_jump(mp_obj_new_exception_msg_varg(QSTR_FROM_STR_STATIC("WlanError"), "bind failed: %d", ret));
    }
    printf("bind seemed to work\n");

    // listen
    ret = listen(sd, 0);
    printf("listen = %d\n", ret);
    sys_tick_delay_ms(100);

    // accept connections
    int fd = -1;
    for (;;) {
        sockaddr accept_addr;
        socklen_t accept_len;
        fd = accept(sd, &accept_addr, &accept_len);
        printf("accept = %d\n", fd);
        sys_tick_delay_ms(500);
        if (fd >= 0) {
            break;
        }
    }

    // receive some data
    {
        printf("receiving on sd=%d fd=%d\n", sd, fd);
        char buf[64];
        ret = recv(fd, buf, 64, 0);
        printf("recv = %d\n", ret);
        if (ret > 0) {
            printf("****%.*s****\n", ret, buf);
        }
        sys_tick_delay_ms(100);
    }

    // send some data
    ret = send(fd, "test data!", 10, 0);
    printf("send = %d\n", ret);
    sys_tick_delay_ms(100);

    closesocket(fd);
    closesocket(sd);

    return mp_const_none;
}

//*****************************************************************************
//
//! CC3000_UsynchCallback
//!
//! @param  lEventType Event type
//! @param  data
//! @param  length
//!
//! @return none
//!
//! @brief  The function handles asynchronous events that come from CC3000
//!         device and operates a led for indicate
//
//*****************************************************************************
void CC3000_UsynchCallback(long lEventType, char * data, unsigned char length)
{
  if (lEventType == HCI_EVNT_WLAN_ASYNC_SIMPLE_CONFIG_DONE)
  {
    //ulSmartConfigFinished = 1;
    //ucStopSmartConfig     = 1;
    printf("WLAN: simple config done\n");
  }

  if (lEventType == HCI_EVNT_WLAN_UNSOL_CONNECT)
  {
    //ulCC3000Connected = 1;
    printf("WLAN unsol connect\n");
  }

  if (lEventType == HCI_EVNT_WLAN_UNSOL_DISCONNECT)
  {
    //ulCC3000Connected = 0;
    //ulCC3000DHCP      = 0;
    //ulCC3000DHCP_configured = 0;
    printf("WLAN unsol disconnect\n");
  }
  
  if (lEventType == HCI_EVNT_WLAN_UNSOL_DHCP)
  {
    //ulCC3000DHCP = 1;
    printf("WLAN unsol DHCP\n");
  }

  if (lEventType == HCI_EVENT_CC3000_CAN_SHUT_DOWN)
  {
    //OkToDoShutDown = 1;
    printf("WLAN can shut down\n");
  }

  if (lEventType == HCI_EVNT_WLAN_ASYNC_PING_REPORT)
  {
    printf("WLAN async ping report\n");
    //PRINT_F("CC3000: Ping report\n\r");
    //pingReportnum++;
    //memcpy(&pingReport, data, length);
  }

  if (lEventType == HCI_EVNT_BSD_TCP_CLOSE_WAIT) {
      printf("WLAN bsd tcp close wait\n");
      /*
    uint8_t socketnum;
    socketnum = data[0];
    //PRINT_F("TCP Close wait #"); printDec(socketnum);
    if (socketnum < MAX_SOCKETS)
      closed_sockets[socketnum] = true;
      */
  }
}

void pyb_wlan_init(void) {
    SpiInit();
    wlan_init(CC3000_UsynchCallback, sendWLFWPatch, sendDriverPatch, sendBootLoaderPatch, ReadWlanInterruptPin, WlanInterruptEnable, WlanInterruptDisable, WriteWlanPin);

    mp_obj_t m = mp_obj_new_module(QSTR_FROM_STR_STATIC("wlan"));
    rt_store_attr(m, QSTR_FROM_STR_STATIC("connect"), rt_make_function_var(0, pyb_wlan_connect));
    rt_store_attr(m, QSTR_FROM_STR_STATIC("disconnect"), rt_make_function_n(0, pyb_wlan_disconnect));
    rt_store_attr(m, QSTR_FROM_STR_STATIC("ip"), rt_make_function_n(0, pyb_wlan_get_ip));
    rt_store_attr(m, QSTR_FROM_STR_STATIC("get_host"), rt_make_function_n(1, pyb_wlan_get_host));
    rt_store_attr(m, QSTR_FROM_STR_STATIC("http_get"), rt_make_function_n(2, pyb_wlan_http_get));
    rt_store_attr(m, QSTR_FROM_STR_STATIC("serve"), rt_make_function_n(0, pyb_wlan_serve));
    rt_store_name(QSTR_FROM_STR_STATIC("wlan"), m);
}

void pyb_wlan_start(void) {
    wlan_start(0);

    // TODO: check return value !=0

    wlan_ioctl_set_connection_policy(0, 0, 0); // don't auto-connect
    wlan_ioctl_del_profile(255); // delete stored eeprom data

    // Mask out all non-required events from CC3000
    wlan_set_event_mask(HCI_EVNT_WLAN_UNSOL_INIT        |
                        //HCI_EVNT_WLAN_ASYNC_PING_REPORT |// we want ping reports
                        //HCI_EVNT_BSD_TCP_CLOSE_WAIT |
                        //HCI_EVNT_WLAN_TX_COMPLETE |
                        HCI_EVNT_WLAN_KEEPALIVE);

    /*
    byte ver[2];
    int ret = nvmem_read_sp_version(ver);
    printf("nvmem_read_sp_version=%d; %02x %02x\n", ret, ver[0], ver[1]);
    */
}
