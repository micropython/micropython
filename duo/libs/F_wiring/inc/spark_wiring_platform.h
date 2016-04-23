#ifndef SPARK_WIRING_PLATFORM_H
#define	SPARK_WIRING_PLATFORM_H

#include "inet_hal.h"

/**
 *  This header file maps platform ID to compile-time switches for the Wiring API.
 */

// This is my code (mdma), but on second thoughts I feel this should be driven bottom up for
// components of the platform. (I.e. platform  defines comes from the HAL)


#if PLATFORM_ID==0 || PLATFORM_ID==2     // core / core hd
#define Wiring_WiFi 1
#define Wiring_IPv6 0
#define Wiring_Serial2 1
#endif

#if PLATFORM_ID==1      // unused
#error Unkonwn platform ID
#endif

#if PLATFORM_ID==3      // gcc
#define Wiring_WiFi 1
#define Wiring_IPv6 0
#define Wiring_SPI1 1
#endif

#if PLATFORM_ID==4      // photon dev
#define Wiring_WiFi 1
#define Wiring_IPv6 1
#define Wiring_SPI1 1
#endif

#if PLATFORM_ID==5
#define Wiring_WiFi 1
#define Wiring_IPv6 1
#define Wiring_SPI1 1
#define Wiring_Serial2 1
#endif

#if PLATFORM_ID==6      // photon
#define Wiring_WiFi 1
#define Wiring_IPv6 1
#define Wiring_SPI1 1
#define Wiring_Serial2 1
#endif

#if PLATFORM_ID==7
#define Wiring_WiFi 1
#define Wiring_IPv6 1
#define Wiring_SPI1 1
#define Wiring_Serial2 1
#endif

#if PLATFORM_ID==8      // P1 / bm14
#define Wiring_WiFi 1
#define Wiring_IPv6 1
#define Wiring_SPI1 1
#define Wiring_Serial2 1
#endif

#if PLATFORM_ID==9      // ethernet
#define Wiring_IPv6 1
#endif

#if PLATFORM_ID==10      // electron
#define Wiring_Cellular 1
#define Wiring_Serial2 1
#define Wiring_Serial3 0 // Used internally by modem
#define Wiring_Serial4 1
#define Wiring_Serial5 1
#define Wiring_SPI1 1
#define Wiring_SPI2 1

#define Wiring_Wire1 1
#define Wiring_Wire3 1 /* System PMIC and Fuel Guage I2C3 */
#define Wiring_SetupButtonUX 1
#endif

#if PLATFORM_ID==88      // duo
#define Wiring_WiFi 1
#define Wiring_IPv6 1
#define Wiring_SPI1 1
#define Wiring_Serial2 1
#define Wiring_Serial6 1
#endif

#ifndef Wiring_SPI1
#define Wiring_SPI1 0
#endif

#ifndef Wiring_SPI2
#define Wiring_SPI2 0
#endif

#ifndef Wiring_Wire1
#define Wiring_Wire1 0
#endif

#ifndef Wiring_Wire3
#define Wiring_Wire3 0
#endif

#ifndef Wiring_WiFi
#define Wiring_WiFi 0
#endif

#ifndef Wiring_Cellular
#define Wiring_Cellular 0
#endif

#ifndef Wiring_Serial2
#define Wiring_Serial2 0
#endif

#ifndef Wiring_Serial3
#define Wiring_Serial3 0
#endif

#ifndef Wiring_Serial4
#define Wiring_Serial4 0
#endif

#ifndef Wiring_Serial5
#define Wiring_Serial5 0
#endif

#ifndef Wiring_SetupButtonUX
#define Wiring_SetupButtonUX 0
#endif

#endif	/* SPARK_WIRING_PLATFORM_H */

