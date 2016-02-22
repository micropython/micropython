#ifndef __ATCMD_WIFI_H__
#define __ATCMD_WIFI_H__
#include "main.h"
#ifndef WLAN0_NAME
  #define WLAN0_NAME		"wlan0"
#endif
#ifndef WLAN1_NAME
  #define WLAN1_NAME		"wlan1"
#endif
/* Give default value if not defined */
#ifndef NET_IF_NUM
#ifdef CONFIG_CONCURRENT_MODE
#define NET_IF_NUM 2
#else
#define NET_IF_NUM 1
#endif
#endif

/*Static IP ADDRESS*/
#ifndef IP_ADDR0
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   1
#define IP_ADDR3   80
#endif

/*NETMASK*/
#ifndef NETMASK_ADDR0
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0
#endif

/*Gateway Address*/
#ifndef GW_ADDR0
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   1
#define GW_ADDR3   1
#endif

/*Static IP ADDRESS*/
#ifndef AP_IP_ADDR0
#define AP_IP_ADDR0   192
#define AP_IP_ADDR1   168
#define AP_IP_ADDR2   43
#define AP_IP_ADDR3   1
#endif
   
/*NETMASK*/
#ifndef AP_NETMASK_ADDR0
#define AP_NETMASK_ADDR0   255
#define AP_NETMASK_ADDR1   255
#define AP_NETMASK_ADDR2   255
#define AP_NETMASK_ADDR3   0
#endif

/*Gateway Address*/
#ifndef AP_GW_ADDR0
#define AP_GW_ADDR0   192
#define AP_GW_ADDR1   168
#define AP_GW_ADDR2   43
#define AP_GW_ADDR3   1  
#endif

#endif
