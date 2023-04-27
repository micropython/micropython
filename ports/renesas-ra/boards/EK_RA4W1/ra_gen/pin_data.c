/* generated pin source file - do not edit */
#include "bsp_api.h"
#include "r_ioport_api.h"
const ioport_pin_cfg_t g_bsp_pin_cfg_data[] = {
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
        .pin_cfg = ((uint32_t)IOPORT_CFG_PORT_DIRECTION_OUTPUT | (uint32_t)IOPORT_CFG_PORT_OUTPUT_HIGH),
    },
    {
        .pin = BSP_IO_PORT_01_PIN_06,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PORT_DIRECTION_OUTPUT | (uint32_t)IOPORT_CFG_PORT_OUTPUT_HIGH),
    },
    {
        .pin = BSP_IO_PORT_01_PIN_09,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_SCI1_3_5_7_9),
    },
    {
        .pin = BSP_IO_PORT_01_PIN_10,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_SCI1_3_5_7_9),
    },
    {
        .pin = BSP_IO_PORT_01_PIN_11,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PORT_DIRECTION_OUTPUT | (uint32_t)IOPORT_CFG_PORT_OUTPUT_HIGH),
    },
    {
        .pin = BSP_IO_PORT_02_PIN_04,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PORT_DIRECTION_OUTPUT | (uint32_t)IOPORT_CFG_PORT_OUTPUT_HIGH),
    },
    {
        .pin = BSP_IO_PORT_02_PIN_05,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_SCI0_2_4_6_8),
    },
    {
        .pin = BSP_IO_PORT_02_PIN_06,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_SCI0_2_4_6_8),
    },
    {
        .pin = BSP_IO_PORT_02_PIN_12,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_SCI1_3_5_7_9),
    },
    {
        .pin = BSP_IO_PORT_02_PIN_13,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_SCI1_3_5_7_9),
    },
    {
        .pin = BSP_IO_PORT_04_PIN_02,
        .pin_cfg = ((uint32_t)IOPORT_CFG_IRQ_ENABLE | (uint32_t)IOPORT_CFG_PORT_DIRECTION_INPUT),
    },
    {
        .pin = BSP_IO_PORT_04_PIN_04,
        .pin_cfg = ((uint32_t)IOPORT_CFG_PORT_DIRECTION_OUTPUT | (uint32_t)IOPORT_CFG_PORT_OUTPUT_HIGH),
    },
};
const ioport_cfg_t g_bsp_pin_cfg = {
    .number_of_pins = sizeof(g_bsp_pin_cfg_data) / sizeof(ioport_pin_cfg_t),
    .p_pin_cfg_data = &g_bsp_pin_cfg_data[0],
};
