/* generated configuration header file - do not edit */
#ifndef BSP_PIN_CFG_H_
#define BSP_PIN_CFG_H_
#include "r_ioport.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

#define A0 (BSP_IO_PORT_00_PIN_00)
#define A1 (BSP_IO_PORT_00_PIN_01)
#define A2 (BSP_IO_PORT_00_PIN_02)
#define A3 (BSP_IO_PORT_00_PIN_03)
#define LED_B (BSP_IO_PORT_00_PIN_06)
#define LED_G (BSP_IO_PORT_00_PIN_07)
#define LED_R (BSP_IO_PORT_00_PIN_08)
#define BTN_2 (BSP_IO_PORT_00_PIN_09)
#define BTN_1 (BSP_IO_PORT_00_PIN_10)
#define A4 (BSP_IO_PORT_00_PIN_14)
#define A5 (BSP_IO_PORT_00_PIN_15)
#define D3 (BSP_IO_PORT_01_PIN_11)
#define D4 (BSP_IO_PORT_01_PIN_12)
#define D5 (BSP_IO_PORT_01_PIN_13)
#define D6 (BSP_IO_PORT_01_PIN_14)
#define D9 (BSP_IO_PORT_01_PIN_15)
#define D12 (BSP_IO_PORT_02_PIN_02)
#define D11 (BSP_IO_PORT_02_PIN_03)
#define D13 (BSP_IO_PORT_02_PIN_04)
#define D8 (BSP_IO_PORT_02_PIN_07)
#define D10 (BSP_IO_PORT_03_PIN_01)
#define D2 (BSP_IO_PORT_04_PIN_09)
#define CAN_STBY (BSP_IO_PORT_05_PIN_13)
#define D7 (BSP_IO_PORT_06_PIN_08)
#define D1 (BSP_IO_PORT_06_PIN_13)
#define D0 (BSP_IO_PORT_06_PIN_14)
extern const ioport_cfg_t g_bsp_pin_cfg; /* VK-RA6M5_v2.0(R7FA6M5BH3CFC).pincfg */

void BSP_PinConfigSecurityInit();

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif /* BSP_PIN_CFG_H_ */
