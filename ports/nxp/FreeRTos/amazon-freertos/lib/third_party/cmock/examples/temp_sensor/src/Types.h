#ifndef _MYTYPES_H_
#define _MYTYPES_H_

#include "AT91SAM7X256.h"
#include <math.h>

#ifndef __monitor
#define __monitor
#endif

// Peripheral Helper Definitions
#define USART0_CLOCK_ENABLE (AT91C_ID_US0)
#define USART0_TX_PIN       (AT91C_PA1_TXD0)
#define TIMER0_CLOCK_ENABLE (((uint32)0x1) << AT91C_ID_TC0)
#define PIOA_CLOCK_ENABLE   (((uint32)0x1) << AT91C_ID_PIOA)
#define PIOB_CLOCK_ENABLE   (((uint32)0x1) << AT91C_ID_PIOB)
#define TIOA0_PIN_MASK      (((uint32)0x1) << 23) // Timer/Counter Output Pin

// Application Type Definitions
typedef unsigned int uint32;
typedef int int32;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned char uint8;
typedef char int8;
typedef char bool;

// Application Special Value Definitions
#ifndef TRUE
#define TRUE      (1)
#endif
#ifndef FALSE
#define FALSE     (0)
#endif
#ifndef NULL
#define NULL      (0)
#endif // NULL
#define DONT_CARE (0)

#ifndef INFINITY
#define INFINITY (1.0 / 0.0)
#endif

#ifndef NAN
#define NAN (0.0 / 0.0)
#endif

// MIN/MAX Definitions for Standard Types
#ifndef INT8_MAX
#define INT8_MAX 127
#endif

#ifndef INT8_MIN
#define INT8_MIN (-128)
#endif

#ifndef UINT8_MAX
#define UINT8_MAX 0xFFU
#endif

#ifndef UINT8_MIN
#define UINT8_MIN 0x00U
#endif

#ifndef INT16_MAX
#define INT16_MAX 32767
#endif

#ifndef INT16_MIN
#define INT16_MIN (-32768)
#endif

#ifndef UINT16_MAX
#define UINT16_MAX 0xFFFFU
#endif

#ifndef UINT16_MIN
#define UINT16_MIN 0x0000U
#endif

#ifndef INT32_MAX
#define INT32_MAX 0x7FFFFFFF
#endif

#ifndef INT32_MIN
#define INT32_MIN (-INT32_MAX - 1)
#endif

#ifndef UINT32_MAX
#define UINT32_MAX 0xFFFFFFFFU
#endif

#ifndef UINT32_MIN
#define UINT32_MIN 0x00000000U
#endif

typedef struct _EXAMPLE_STRUCT_T
{
    int x;
    int y;
} EXAMPLE_STRUCT_T;

#endif // _MYTYPES_H_
