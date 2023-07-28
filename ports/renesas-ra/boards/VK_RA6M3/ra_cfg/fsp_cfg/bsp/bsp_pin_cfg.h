/* generated configuration header file - do not edit */
#ifndef BSP_PIN_CFG_H_
#define BSP_PIN_CFG_H_
#include "r_ioport.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

#define ET_nRST (BSP_IO_PORT_04_PIN_00)
#define LED_G (BSP_IO_PORT_06_PIN_01)
#define LED_R (BSP_IO_PORT_06_PIN_03)
#define LED_B (BSP_IO_PORT_06_PIN_05)
#define LED_Y (BSP_IO_PORT_06_PIN_08)
#define CMR_PWDN (BSP_IO_PORT_06_PIN_11)
#define CMR_RST (BSP_IO_PORT_06_PIN_12)
#define LCD_BLON (BSP_IO_PORT_08_PIN_01)
#define LCD_DISP (BSP_IO_PORT_08_PIN_06)
extern const ioport_cfg_t g_bsp_pin_cfg; /* VK_RA6M3.pincfg */

void BSP_PinConfigSecurityInit();

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif /* BSP_PIN_CFG_H_ */
