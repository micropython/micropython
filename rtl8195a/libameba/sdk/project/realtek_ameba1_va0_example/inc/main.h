#ifndef MAIN_H
#define MAIN_H

#include <autoconf.h>

#ifndef CONFIG_WLAN
#define CONFIG_WLAN	1
#endif

/* Header file declaration*/
void wlan_network();

/* Interactive Mode */
#define SERIAL_DEBUG_RX 1

/* WLAN and Netork */
#define STA_MODE_SSID	"ap"    /* Set SSID here */
#define AP_MODE_SSID		"wlan_ap_ssid"    /* Set SSID here */
#define AP_DEFAULT_CH	6
#define WLAN0_NAME		"wlan0"
#define WLAN1_NAME      	"wlan1"
#define WPA_PASSPHRASE	"1234567890" /* Max 32 cahracters */
#define WEP40_KEY		{0x12, 0x34, 0x56, 0x78, 0x90}

/*Static IP ADDRESS*/
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   1
#define IP_ADDR3   80

/*NETMASK*/
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

/*Gateway Address*/
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   1
#define GW_ADDR3   1

/*******************************************/

/*Static IP ADDRESS*/
#define AP_IP_ADDR0   192
#define AP_IP_ADDR1   168
#define AP_IP_ADDR2   43
#define AP_IP_ADDR3   1
   
/*NETMASK*/
#define AP_NETMASK_ADDR0   255
#define AP_NETMASK_ADDR1   255
#define AP_NETMASK_ADDR2   255
#define AP_NETMASK_ADDR3   0

/*Gateway Address*/
#define AP_GW_ADDR0   192
#define AP_GW_ADDR1   168
#define AP_GW_ADDR2   43
#define AP_GW_ADDR3   1  

#endif
