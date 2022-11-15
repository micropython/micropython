/* generated pin source file - do not edit */
#include "bsp_api.h"
#include "r_ioport_api.h"
const ioport_pin_cfg_t g_bsp_pin_cfg_data[] = {
    {
        .pin = BSP_IO_PORT_00_PIN_00,
        .pin_cfg = ((uint32_t)IOPORT_CFG_ANALOG_ENABLE),
    },
    {
        .pin = BSP_IO_PORT_01_PIN_00,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_SPI),
    },
    {
        .pin = BSP_IO_PORT_01_PIN_01,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_SPI),
    },
    {
        .pin = BSP_IO_PORT_01_PIN_02,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_SPI),
    },
    {
        .pin = BSP_IO_PORT_01_PIN_03,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_SPI),
    },
    {
        .pin = BSP_IO_PORT_01_PIN_05,
        .pin_cfg = ((uint32_t)IOPORT_CFG_IRQ_ENABLE | (uint32_t)IOPORT_CFG_PORT_DIRECTION_INPUT),
    },
    {
        .pin = BSP_IO_PORT_01_PIN_06,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PORT_DIRECTION_OUTPUT | (uint32_t)IOPORT_CFG_PORT_OUTPUT_LOW),
    },
    {
        .pin = BSP_IO_PORT_01_PIN_08,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_DEBUG),
    },
    {
        .pin = BSP_IO_PORT_01_PIN_09,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_DEBUG),
    },
    {
        .pin = BSP_IO_PORT_01_PIN_10,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_DEBUG),
    },
    {
        .pin = BSP_IO_PORT_01_PIN_15,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_CTSU),
    },
    {
        .pin = BSP_IO_PORT_02_PIN_05,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_CTSU),
    },
    {
        .pin = BSP_IO_PORT_03_PIN_00,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_DEBUG),
    },
    {
        .pin = BSP_IO_PORT_03_PIN_01,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_SCI0_2_4_6_8),
    },
    {
        .pin = BSP_IO_PORT_03_PIN_02,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_SCI0_2_4_6_8),
    },
    {
        .pin = BSP_IO_PORT_04_PIN_00,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PORT_DIRECTION_OUTPUT | (uint32_t)IOPORT_CFG_PORT_OUTPUT_LOW),
    },
    {
        .pin = BSP_IO_PORT_04_PIN_01,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_SCI1_3_5_7_9),
    },
    {
        .pin = BSP_IO_PORT_04_PIN_02,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_SCI1_3_5_7_9),
    },
    {
        .pin = BSP_IO_PORT_04_PIN_03,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PORT_DIRECTION_OUTPUT | (uint32_t)IOPORT_CFG_PORT_OUTPUT_LOW),
    },
    {
        .pin = BSP_IO_PORT_04_PIN_07,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_USB_FS),
    },
    {
        .pin = BSP_IO_PORT_04_PIN_10,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_SCI0_2_4_6_8),
    },
    {
        .pin = BSP_IO_PORT_04_PIN_11,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_SCI0_2_4_6_8),
    },
    {
        .pin = BSP_IO_PORT_09_PIN_14,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_USB_FS),
    },
    {
        .pin = BSP_IO_PORT_09_PIN_15,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_USB_FS),
    },
};
const ioport_cfg_t g_bsp_pin_cfg = {
    .number_of_pins = sizeof(g_bsp_pin_cfg_data) / sizeof(ioport_pin_cfg_t),
    .p_pin_cfg_data = &g_bsp_pin_cfg_data[0],
};
