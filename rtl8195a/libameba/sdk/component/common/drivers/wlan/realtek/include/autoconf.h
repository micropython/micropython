#ifndef WLANCONFIG_H
#define WLANCONFIG_H

/*
 * Include user defined options first. Anything not defined in these files
 * will be set to standard values. Override anything you dont like!
 */
 #if defined(CONFIG_PLATFORM_8195A) || defined(CONFIG_PLATFORM_8711B)
#include "platform_opts.h"
#endif

#if defined(CONFIG_PLATFORM_8195A) || defined(CONFIG_PLATFORM_8711B)
#define CONFIG_PLATFORM_AMEBA_X
#endif

#if !defined(CONFIG_PLATFORM_AMEBA_X)
#define PLATFORM_FREERTOS 	1
#define CONFIG_GSPI_HCI
#else
#define CONFIG_LX_HCI
#endif

#ifndef CONFIG_INIC_EN
#define CONFIG_INIC_EN 0 //For iNIC project
#if CONFIG_INIC_EN
#define CONFIG_LWIP_LAYER    0
#endif
#endif

#define CONFIG_LITTLE_ENDIAN
#define CONFIG_80211N_HT 
//#define CONFIG_RECV_REORDERING_CTRL
#define RTW_NOTCH_FILTER 0
#define CONFIG_EMBEDDED_FWIMG 1
#define CONFIG_PHY_SETTING_WITH_ODM
#if !defined(CONFIG_PLATFORM_AMEBA_X)
#define CONFIG_ODM_REFRESH_RAMASK
#define HAL_MAC_ENABLE 1
#define HAL_BB_ENABLE 1
#define HAL_RF_ENABLE 1
#endif
#if defined(CONFIG_PLATFORM_AMEBA_X)
/* Patch when dynamic mechanism is not ready */
//#define CONFIG_DM_PATCH
#endif

//#define CONFIG_DEBUG
//#define CONFIG_DEBUG_RTL871X
#if defined(CONFIG_PLATFORM_AMEBA_X)
	#define CONFIG_MEM_MONITOR	MEM_MONITOR_SIMPLE
	#define WLAN_INTF_DBG		0 
	#define CONFIG_DEBUG_DYNAMIC
	//#define DBG_TX 1
	//#define DBG_XMIT_BUF 1
	//#define DBG_XMIT_BUF_EXT 1
	#define DBG_TX_DROP_FRAME
#else
	#define CONFIG_MEM_MONITOR	MEM_MONITOR_LEAK
	//#define CONFIG_TRACE_SKB
	//#define WLAN_INTF_DBG
#endif // CONFIG_PLATFORM_AMEBA_X

//#define CONFIG_DONT_CARE_TP
//#define CONFIG_MEMORY_ACCESS_ALIGNED
#define CONFIG_POWER_SAVING
#ifdef CONFIG_POWER_SAVING
	#define CONFIG_IPS
	#define CONFIG_LPS
	//#define CONFIG_LPS_LCLK
	#define CONFIG_LPS_32K
	#define TDMA_POWER_SAVING
	#define CONFIG_WAIT_PS_ACK
#endif

#if defined(CONFIG_PLATFORM_AMEBA_X)
	#if !defined(CONFIG_PLATFORM_8711B)
		#define CONFIG_USE_TCM_HEAP 1					/* USE TCM HEAP */
	#endif
	#define CONFIG_RECV_TASKLET_THREAD
	#define CONFIG_XMIT_TASKLET_THREAD
#else
	#define CONFIG_XMIT_THREAD_MODE
#endif // CONFIG_PLATFORM_AMEBA_X
//#define CONFIG_RECV_THREAD_MODE				/* Wlan IRQ Polling  Mode*/
//#define CONFIG_ISR_THREAD_MODE_POLLING		/* Wlan IRQ Polling  Mode*/

//1 Chris
#ifndef CONFIG_SDIO_HCI
#define CONFIG_ISR_THREAD_MODE_INTERRUPT	/* Wlan IRQ Interrupt  Mode*/
#endif

#if defined(CONFIG_ISR_THREAD_MODE_POLLING) && defined(CONFIG_ISR_THREAD_MODE_INTERRUPT)
#error "CONFIG_ISR_THREAD_MODE_POLLING and CONFIG_ISR_THREAD_MODE_INTERRUPT are mutually exclusive. "
#endif

#if defined(CONFIG_PLATFORM_AMEBA_X)
/* CRC DMEM optimized mode consume 1k less SRM memory consumption */
#define CRC_IMPLEMENTATION_MODE CRC_IMPLEMENTATION_DMEM_OPTIMIZED
#endif

/* AES DMEM optimized mode comsume 10k less memory compare to 
     IMEM optimized mode AES_IMPLEMENTATION_IMEM_OPTIMIZED */
#define AES_IMPLEMENTATION_MODE AES_IMPLEMENTATION_DMEM_OPTIMIZED

#define USE_SKB_AS_XMITBUF	1
#if defined(CONFIG_PLATFORM_AMEBA_X)
#define USE_XMIT_EXTBUFF	1
#else
#define USE_XMIT_EXTBUFF	0
#endif
#define USE_MUTEX_FOR_SPINLOCK	1
#define SUPPORT_5G_CHANNEL	0
#define SUPPORT_ONLY_HT_1T	1
#define SUPPORT_FAKE_EFUSE	0

#define CONFIG_AUTO_RECONNECT 1
#define ENABLE_HWPDN_PIN
#define SUPPORT_SCAN_BUF	1
#if !defined(CONFIG_PLATFORM_AMEBA_X)
#define BE_I_CUT			1
#endif

/* For WPA2 */
#define CONFIG_INCLUDE_WPA_PSK
#ifdef CONFIG_INCLUDE_WPA_PSK
#define CONFIG_MULTIPLE_WPA_STA
//#define CONFIG_WPA2_PREAUTH
#define PSK_SUPPORT_TKIP	1
#endif

/* For promiscuous mode */
#define CONFIG_PROMISC

#define PROMISC_DENY_PAIRWISE	0

/* For Simple Link */
#ifndef CONFIG_INCLUDE_SIMPLE_CONFIG
//#define CONFIG_INCLUDE_SIMPLE_CONFIG 1
#endif

// for probe request with custom vendor specific IE
#define CONFIG_CUSTOM_IE

#if !defined(CONFIG_PLATFORM_AMEBA_X)
/* For multicast */
#define CONFIG_MULTICAST
#endif

/* For STA+AP Concurrent MODE */
#if !defined(CONFIG_PLATFORM_8711B)
#define CONFIG_CONCURRENT_MODE
#endif
#ifdef CONFIG_CONCURRENT_MODE
#if defined(CONFIG_PLATFORM_8195A)
#define CONFIG_RUNTIME_PORT_SWITCH
#endif
#define NET_IF_NUM 2
#else
#define NET_IF_NUM 1
#endif


/* For WPS and P2P */
#ifndef CONFIG_WPS
#define CONFIG_WPS
#if defined(CONFIG_WPS) 
#define CONFIG_ENABLE_WPS       1
#endif
#if 0//def CONFIG_WPS
#define CONFIG_WPS_AP
#define CONFIG_P2P_NEW
#if (!defined(SUPPORT_SCAN_BUF)||!defined(CONFIG_WPS_AP)) && defined(CONFIG_P2P_NEW)
#error "If CONFIG_P2P_NEW, need to SUPPORT_SCAN_BUF"
#endif
#endif
#endif

#if !defined(CONFIG_PLATFORM_AMEBA_X)
#define CONFIG_NEW_SIGNAL_STAT_PROCESS
#endif

/* For AP_MODE */
#define CONFIG_AP_MODE
#if defined(CONFIG_PLATFORM_AMEBA_X)
#define AP_STA_NUM	3	//2014/10/27 modify to 3
#define USE_DEDICATED_BCN_TX	0
#if USE_DEDICATED_BCN_TX
#error "WLAN driver for Ameba should not enable USE_DEDICATED_BCN_TX"
#endif
#else
extern unsigned int g_ap_sta_num;
#define AP_STA_NUM g_ap_sta_num
#endif
#ifdef CONFIG_AP_MODE
	#define CONFIG_NATIVEAP_MLME
#if defined(CONFIG_PLATFORM_AMEBA_X)
	#define CONFIG_INTERRUPT_BASED_TXBCN
#endif
	#ifdef CONFIG_INTERRUPT_BASED_TXBCN
		//#define CONFIG_INTERRUPT_BASED_TXBCN_EARLY_INT
		#define CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR
	#endif
//	#define CONFIG_GK_REKEY
#if !defined(CONFIG_PLATFORM_AMEBA_X)
	#define USE_DEDICATED_BCN_TX	1
#endif
#if CONFIG_INIC_EN
	#define REPORT_STA_EVENT
#endif
#else
#if !defined(CONFIG_PLATFORM_AMEBA_X)
	#define USE_DEDICATED_BCN_TX	0
#endif
#endif

#if defined(CONFIG_AP_MODE) && defined(CONFIG_GK_REKEY) && !defined(CONFIG_MULTIPLE_WPA_STA)
#error "If CONFIG_GK_REKEY when CONFIG_AP_MODE, need to CONFIG_MULTIPLE_WPA_STA"
#endif

#if !defined(CONFIG_PLATFORM_AMEBA_X)
#if !defined(CONFIG_AP_MODE) && defined(CONFIG_CONCURRENT_MODE)
#error "If CONFIG_CONCURRENT_MODEE, need to CONFIG_AP_MODE"
#endif
#endif

/* For efuse or flash config */
#if defined(CONFIG_PLATFORM_AMEBA_X)
	#define CONFIG_RW_PHYSICAL_EFUSE			0	// Mask efuse user blocks
	#define CONFIG_HIDE_PROTECT_EFUSE			1
	#define CONFIG_ADAPTOR_INFO_CACHING_FLASH	1
	#define CHECK_FLASH_VALID_MASK				1
	/* For K-free */
	#if !defined(CONFIG_PLATFORM_8711B)
		#define CONFIG_RF_GAIN_OFFSET
	#endif
#endif	// CONFIG_PLATFORM_AMEBA_X

/* For MP_MODE */
//#define CONFIG_MP_INCLUDED
#ifdef CONFIG_MP_INCLUDED
	#define MP_DRIVER		1
	#define CONFIG_MP_IWPRIV_SUPPORT
//	#define HAL_EFUSE_MEMORY
	#if defined(CONFIG_PLATFORM_AMEBA_X)
		#define MP_REG_TEST
	#endif
#else
	#define MP_DRIVER		0
	#if defined(CONFIG_PLATFORM_AMEBA_X)
		//Control wifi mcu function
		#define CONFIG_LITTLE_WIFI_MCU_FUNCTION_THREAD
		#define CONFIG_ODM_REFRESH_RAMASK
	#endif
#endif // #ifdef CONFIG_MP_INCLUDED

#if defined(CONFIG_PLATFORM_AMEBA_X)
#if defined(CONFIG_PLATFORM_8195A)
#define CONFIG_RTL8195A
#endif
#if defined(CONFIG_PLATFORM_8711B)
#define CONFIG_RTL8711B
#endif
#else
#define CONFIG_RTL8188E
#endif
#define RTL8192C_SUPPORT 0
#define RTL8192CE_SUPPORT 0
#define RTL8192CU_SUPPORT 0
#define RTL8192D_SUPPORT 0
#define RTL8192DE_SUPPORT 0
#define RTL8192DU_SUPPORT 0
#define RTL8723A_SUPPORT 0
#define RTL8723AU_SUPPORT 0
#define RTL8723AS_SUPPORT 0
#define RTL8192E_SUPPORT 0
#define RTL8812A_SUPPORT 0
#define RTL8821A_SUPPORT 0
#define RTL8723B_SUPPORT 0
#if defined(CONFIG_PLATFORM_AMEBA_X)
#define RTL8195A_SUPPORT 1
#define RTL8188E_SUPPORT 0
#else
#define RTL8188E_SUPPORT 1
#define RTL8195A_SUPPORT 0
#endif
#define TEST_CHIP_SUPPORT 0

#define RTL8188E_FOR_TEST_CHIP 0
#define RTL8188E_FPGA_TRUE_PHY_VERIFICATION 0

// for Debug message
#define DBG 0
#if defined(CONFIG_PLATFORM_AMEBA_X)
#if(DBG == 0)
	#define ROM_E_RTW_MSG 1
	/* For DM debug*/
	// BB
	#define DBG_RX_INFO 1
	#define DBG_TX_RATE 1			// DebugComponents: bit9
	#define DBG_DM_RA 1				// DebugComponents: bit9
	#define DBG_DM_DIG 1			// DebugComponents: bit0
	// RF
	#define DBG_PWR_TRACKING 1		// DebugComponents: bit24
	#define DBG_RF_IQK 1			// DebugComponents: bit26
	// Common
	#define DBG_PWR_INDEX 1			// DebugComponents: bit30
#endif
#endif

/* For DM support */
#define RATE_ADAPTIVE_SUPPORT 1
#define CONFIG_RTW_ADAPTIVITY_EN 0
#define RTW_ADAPTIVITY_MODE_NORMAL 0
#define RTW_ADAPTIVITY_MODE_CARRIER_SENSE 1
#define CONFIG_RTW_ADAPTIVITY_MODE RTW_ADAPTIVITY_MODE_CARRIER_SENSE
#if defined(CONFIG_PLATFORM_AMEBA_X)
	#define CONFIG_POWER_TRAINING_WIL 0 // in RA
#else
	#define POWER_BY_RATE_SUPPORT 0
#endif

#if defined(CONFIG_PLATFORM_AMEBA_X)
#define RTL8195A_FOR_TEST_CHIP 0

//#define CONFIG_WIFI_TEST 1
//#define CONFIG_MAC_LOOPBACK_DRIVER 1
//#define CONFIG_WLAN_HAL_TEST 1
//#define SKB_PRE_ALLOCATE_TX 1
#define SKB_PRE_ALLOCATE_RX	0
#define TX_CHECK_DSEC_ALWAYS 1
#define CONFIG_DBG_DISABLE_RDU_INTERRUPT
//#define CONFIG_WLAN_HAL_RX_TASK
#if (SKB_PRE_ALLOCATE_RX == 1)
	#define EXCHANGE_LXBUS_RX_SKB 0
#endif

//Enable mac loopback for test mode (Ameba)
//#define ENABLE_MAC_LB_FOR_TEST_MODE // for test mode
#ifdef ENABLE_MAC_LB_FOR_TEST_MODE
	#define CONFIG_SUDO_PHY_SETTING
	#define INT_HANDLE_IN_ISR 1
	#define CONFIG_LWIP_LAYER 0
	#define CONFIG_WLAN_HAL_TEST
	#define CONFIG_WLAN_HAL_RX_TASK
	#define CONFIG_MAC_LOOPBACK_DRIVER_RTL8195A 1
	//#define CONFIG_TWO_MAC_TEST_MODE 
	#define DISABLE_BB_RF 1
#else
	//#define CONFIG_TWO_MAC_DRIVER //for mornal driver; two mac
	#ifdef CONFIG_TWO_MAC_DRIVER
		#define CONFIG_SUDO_PHY_SETTING
		#define DISABLE_BB_RF 1
	#else
		#define HAL_MAC_ENABLE 1
		#define HAL_BB_ENABLE 1
		#define HAL_RF_ENABLE 1
		#define DISABLE_BB_RF 0
	#endif
	//#define INT_HANDLE_IN_ISR 1
#endif
#endif // CONFIG_PLATFORM_AMEBA_X

#ifndef CONFIG_LWIP_LAYER
#define CONFIG_LWIP_LAYER 1
#endif
#define CONFIG_MAC_ADDRESS 0
//fast reconnection
//#define CONFIG_FAST_RECONNECTION 1
#endif //WLANCONFIG_H
