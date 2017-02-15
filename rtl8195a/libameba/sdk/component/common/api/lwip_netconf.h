/**
  ******************************************************************************
  * @file    netconf.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    07-October-2011 
  * @brief   This file contains all the functions prototypes for the netconf.c 
  *          file.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NETCONF_H
#define __NETCONF_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "tcpip.h"
/* Includes ------------------------------------------------------------------*/
#include <platform/platform_stdlib.h>
/* Private typedef -----------------------------------------------------------*/
typedef enum 
{ 
	DHCP_START=0,
	DHCP_WAIT_ADDRESS,
	DHCP_ADDRESS_ASSIGNED,
	DHCP_RELEASE_IP,
	DHCP_STOP,
	DHCP_TIMEOUT
} DHCP_State_TypeDef;

/* Extern functions ------------------------------------------------------------*/
void wifi_rx_beacon_hdl( char* buf, int buf_len, int flags, void* userdata);


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void LwIP_Init(void);
uint8_t LwIP_DHCP(uint8_t idx, uint8_t dhcp_state);
unsigned char* LwIP_GetMAC(struct netif *pnetif);
unsigned char* LwIP_GetIP(struct netif *pnetif);
unsigned char* LwIP_GetGW(struct netif *pnetif);
uint8_t* LwIP_GetMASK(struct netif *pnetif);
uint8_t* LwIP_GetBC(struct netif *pnetif);
#if LWIP_DNS
void LwIP_GetDNS(struct ip_addr* dns);
void LwIP_SetDNS(struct ip_addr* dns);
#endif
void LwIP_UseStaticIP(struct netif *pnetif);
#if LWIP_AUTOIP
void LwIP_AUTOIP(struct netif *pnetif);
#endif
#if LWIP_IPV6
void LwIP_AUTOIP_IPv6(struct netif *pnetif);
#endif
#ifdef __cplusplus
}
#endif

#endif /* __NETCONF_H */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
