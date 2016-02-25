#ifndef __LINUX_USB_ULINKER_H
#define __LINUX_USB_ULINKER_H

//#include "linux/autoconf.h"

//#ifndef CONFIG_RTL_ULINKER_CUSTOMIZATION
#if 1//ModifiedByJD    
#define ULINKER_ETHER_VID		0x0BDA
#define ULINKER_ETHER_PID		0x8195
#define ULINKER_MANUFACTURER    "Realtek Semicoonductor Corp."

#define ULINKER_WINTOOLS_GUID               "1CACC490-055C-4035-A026-1DAB0BDA8196"
#define ULINKER_WINTOOLS_DISPLAY_NAME       "Realtek RTL8196EU Universal Linker"
#define ULINKER_WINTOOLS_CONTACT            "nicfae@realtek.com.tw"
#define ULINKER_WINTOOLS_DISPLAY_VERSION    "v1.0.0.0"
#define ULINKER_WINTOOLS_HELP_LINK          "http://www.realtek.com.tw"
#define ULINKER_WINTOOLS_PUBLISHER          ULINKER_MANUFACTURER
#define ULINKER_WINTOOLS_TARGET_DIR         ULINKER_WINTOOLS_DISPLAY_NAME
#else
#define ULINKER_ETHER_VID		CONFIG_RTL_ULINKER_VID
#define ULINKER_ETHER_PID		CONFIG_RTL_ULINKER_PID
#define ULINKER_STORAGE_VID		CONFIG_RTL_ULINKER_VID_S
#define ULINKER_STORAGE_PID		CONFIG_RTL_ULINKER_PID_S
#define ULINKER_MANUFACTURER    CONFIG_RTL_ULINKER_MANUFACTURE

#define ULINKER_WINTOOLS_GUID               CONFIG_RTL_ULINKER_WINTOOLS_GUID
#define ULINKER_WINTOOLS_DISPLAY_NAME       CONFIG_RTL_ULINKER_WINTOOLS_DISPLAY_NAME
#define ULINKER_WINTOOLS_CONTACT            CONFIG_RTL_ULINKER_WINTOOLS_CONTACT
#define ULINKER_WINTOOLS_DISPLAY_VERSION    CONFIG_RTL_ULINKER_WINTOOLS_DISPLAY_VERSION
#define ULINKER_WINTOOLS_HELP_LINK          CONFIG_RTL_ULINKER_WINTOOLS_HELP_LINK
#define ULINKER_WINTOOLS_PUBLISHER          ULINKER_MANUFACTURER
#define ULINKER_WINTOOLS_TARGET_DIR         ULINKER_WINTOOLS_DISPLAY_NAME
#endif

//------------------------------------------------
// if you don't have a specific PID for storage, don't change following define of storage mode.
//
// begin: don't change 
#ifndef ULINKER_STORAGE_VID
#define ULINKER_STORAGE_VID		0x0BDA
#define ULINKER_STORAGE_PID		0x8197
#endif

#define ULINKER_STORAGE_VID_STR			"USB Ether "
#define ULINKER_STORAGE_PID_DISK_STR	"Driver DISC"
#define ULINKER_STORAGE_PID_CDROM_STR	"Driver CDROM"

#define ULINKER_WINTOOLS_DRIVER_PATH        "Driver"
// end: don't change 
//------------------------------------------------


//----------------------------------------------------------------------
#if defined(CONFIG_RTL_ULINKER)

#define ULINKER_DEVINIT
#define ULINKER_DEVINITDATA
#define ULINKER_DEVINITCONST
#define ULINKER_DEVEXIT
#define ULINKER_DEVEXITDATA
#define ULINKER_DEVEXITCONST

#else

#define ULINKER_DEVINIT		__devinit
#define ULINKER_DEVINITDATA	__devinitdata
#define ULINKER_DEVINITCONST	__devinitconst
#define ULINKER_DEVEXIT		__devexit
#define ULINKER_DEVEXITDATA	__devexitdata
#define ULINKER_DEVEXITCONST	__devexitconst

#endif

#endif	/* __LINUX_USB_ULINKER_H */
