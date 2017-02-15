/**
 ******************************************************************************
 *This file contains general configurations for ameba platform
 ******************************************************************************
*/

#ifndef __PLATFORM_OPTS_H__
#define __PLATFORM_OPTS_H__

/*For MP mode setting*/
#define SUPPORT_MP_MODE		1

/**
 * For AT cmd Log service configurations
 */
#define SUPPORT_LOG_SERVICE	1
#if SUPPORT_LOG_SERVICE
#define LOG_SERVICE_BUFLEN     100 //can't larger than UART_LOG_CMD_BUFLEN(127)
#define CONFIG_LOG_HISTORY	0
#if CONFIG_LOG_HISTORY
#define LOG_HISTORY_LEN    5
#endif
#define SUPPORT_INTERACTIVE_MODE		0//on/off wifi_interactive_mode
#endif

/**
 * For interactive mode configurations, depends on log service
 */
#if SUPPORT_INTERACTIVE_MODE
#define CONFIG_INTERACTIVE_MODE     1
#define CONFIG_INTERACTIVE_EXT      0
#else
#define CONFIG_INTERACTIVE_MODE     0
#define CONFIG_INTERACTIVE_EXT      0
#endif

/**
 * For FreeRTOS tickless configurations
 */
#define FREERTOS_PMU_TICKLESS_PLL_RESERVED 0 // In sleep mode, 0: close PLL clock, 1: reserve PLL clock

/******************************************************************************/


/**
 * For Wlan configurations
 */
#define CONFIG_WLAN	1
#if CONFIG_WLAN
#define CONFIG_LWIP_LAYER	1
#define CONFIG_INIT_NET         1 //init lwip layer when start up

//on/off relative commands in log service
#define CONFIG_SSL_CLIENT       0
#define CONFIG_WEBSERVER        0
#define CONFIG_OTA_UPDATE       1
#define CONFIG_BSD_TCP          0
#define CONFIG_ENABLE_P2P       0//on/off p2p cmd in log_service or interactive mode
#define CONFIG_AIRKISS          0//on or off tencent airkiss
#define CONFIG_UART_SOCKET		0
#define CONFIG_JD_SMART			0//on or off for jdsmart
#define SUPPORT_UART_YMODEM     0//support uart ymodem upgrade or not
#define CONFIG_GOOGLE_NEST      0//on or off the at command control for google nest

/* For WPS and P2P */
#define CONFIG_WPS
#if defined(CONFIG_WPS) 
#define CONFIG_ENABLE_WPS       1
#endif

/* For Simple Link */
#define CONFIG_INCLUDE_SIMPLE_CONFIG	1

/*For fast reconnection*/
#define CONFIG_EXAMPLE_WLAN_FAST_CONNECT 1

#endif //end of #if CONFIG_WLAN
/*******************************************************************************/

/**
 * For iNIC configurations
 */
#define CONFIG_INIC_EN 0//enable iNIC mode
#if CONFIG_INIC_EN
#undef CONFIG_LWIP_LAYER
#define CONFIG_LWIP_LAYER	0
#define CONFIG_INIC_SDIO_HCI	1 //for SDIO or USB iNIC
#define CONFIG_INIC_USB_HCI	0
#endif

/* For Wigadget example */
#define CONFIG_EXAMPLE_WIGADGET              0

/*For google nest example*/
#define CONFIG_EXAMPLE_GOOGLE_NEST       0

/* For mDNS example */
#define CONFIG_EXAMPLE_MDNS              0

/* For multicast example */
#define CONFIG_EXAMPLE_MCAST             0

/* For XML example */
#define CONFIG_EXAMPLE_XML               0

/* For socket select example */
#define CONFIG_EXAMPLE_SOCKET_SELECT     0

/* For ssl download example */
#define CONFIG_EXAMPLE_SSL_DOWNLOAD      0

/* For uvc example */
#ifdef CONFIG_UVC
#define CONFIG_EXAMPLE_UVC               1
#else
#define CONFIG_EXAMPLE_UVC               0
#endif

#if CONFIG_EXAMPLE_UVC
#define CONFIG_ENABLE_WPS 0
#endif

/*For uart update example*/
#define CONFIG_UART_UPDATE				0

/*For wifi manager example */
#define CONFIG_EXAMPLE_WIFI_MANAGER      0

/* For uart adapter example */
/* Please also configure LWIP_UART_ADAPTER to 1 
in lwip_opt.h for support uart adapter*/
#define CONFIG_EXAMPLE_UART_ADAPTER             0
#if CONFIG_EXAMPLE_UART_ADAPTER
#undef CONFIG_EXAMPLE_WLAN_FAST_CONNECT
#define CONFIG_EXAMPLE_WLAN_FAST_CONNECT	1
#undef CONFIG_EXAMPLE_MDNS
#define CONFIG_EXAMPLE_MDNS	1
#undef FREERTOS_PMU_TICKLESS_PLL_RESERVED 
#define FREERTOS_PMU_TICKLESS_PLL_RESERVED	1
#endif

#if CONFIG_JD_SMART
#if (CONFIG_ENABLE_WPS == 1)
#define CONFIG_ENABLE_WPS	0
#endif
#if (CONFIG_INCLUDE_SIMPLE_CONFIG == 1)
#define CONFIG_INCLUDE_SIMPLE_CONFIG	0
#endif
#if (CONFIG_EXAMPLE_WLAN_FAST_CONNECT == 1)
#define CONFIG_EXAMPLE_WLAN_FAST_CONNECT	0
#endif
#endif

#endif

/*******************************************************************************/

/**
 * For Ameba on Arduion IDE configurations
 */

#ifdef ARDUINO_SDK

#undef  SUPPORT_MP_MODE
#define SUPPORT_MP_MODE 0

#undef  CONFIG_OTA_UPDATE
#define CONFIG_OTA_UPDATE 0

#undef  CONFIG_INCLUDE_SIMPLE_CONFIG
#define CONFIG_INCLUDE_SIMPLE_CONFIG 0

#endif