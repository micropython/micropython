/* generated vector source file - do not edit */
#include "bsp_api.h"
/* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
#if VECTOR_DATA_IRQ_COUNT > 0
BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_MAX_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
{
    [0] = sci_uart_rxi_isr,         /* SCI1 RXI (Received data full) */
    [1] = sci_uart_txi_isr,         /* SCI1 TXI (Transmit data empty) */
    [2] = sci_uart_tei_isr,         /* SCI1 TEI (Transmit end) */
    [3] = sci_uart_eri_isr,         /* SCI1 ERI (Receive error) */
    [4] = sci_uart_rxi_isr,         /* SCI9 RXI (Received data full) */
    [5] = sci_uart_txi_isr,         /* SCI9 TXI (Transmit data empty) */
    [6] = sci_uart_tei_isr,         /* SCI9 TEI (Transmit end) */
    [7] = sci_uart_eri_isr,         /* SCI9 ERI (Receive error) */
    [8] = rtc_alarm_periodic_isr,   /* RTC ALARM (Alarm interrupt) */
    [9] = rtc_alarm_periodic_isr,   /* RTC PERIOD (Periodic interrupt) */
    [10] = rtc_carry_isr,           /* RTC CARRY (Carry interrupt) */
    [11] = agt_int_isr,             /* AGT0 INT (AGT interrupt) */
    [12] = agt_int_isr,             /* AGT1 INT (AGT interrupt) */
    [13] = r_icu_isr,               /* ICU IRQ9 (External pin interrupt 9) */
    [14] = spi_rxi_isr,             /* SPI0 RXI (Receive buffer full) */
    [15] = spi_txi_isr,             /* SPI0 TXI (Transmit buffer empty) */
    [16] = spi_tei_isr,             /* SPI0 TEI (Transmission complete event) */
    [17] = spi_eri_isr,             /* SPI0 ERI (Error) */
    [18] = iic_master_rxi_isr,      /* IIC1 RXI (Receive data full) */
    [19] = iic_master_txi_isr,      /* IIC1 TXI (Transmit data empty) */
    [20] = iic_master_tei_isr,      /* IIC1 TEI (Transmit end) */
    [21] = iic_master_eri_isr,      /* IIC1 ERI (Transfer error) */
};
const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_MAX_ENTRIES] =
{
    [0] = BSP_PRV_IELS_ENUM(EVENT_SCI1_RXI),         /* SCI1 RXI (Received data full) */
    [1] = BSP_PRV_IELS_ENUM(EVENT_SCI1_TXI),         /* SCI1 TXI (Transmit data empty) */
    [2] = BSP_PRV_IELS_ENUM(EVENT_SCI1_TEI),         /* SCI1 TEI (Transmit end) */
    [3] = BSP_PRV_IELS_ENUM(EVENT_SCI1_ERI),         /* SCI1 ERI (Receive error) */
    [4] = BSP_PRV_IELS_ENUM(EVENT_SCI9_RXI),         /* SCI9 RXI (Received data full) */
    [5] = BSP_PRV_IELS_ENUM(EVENT_SCI9_TXI),         /* SCI9 TXI (Transmit data empty) */
    [6] = BSP_PRV_IELS_ENUM(EVENT_SCI9_TEI),         /* SCI9 TEI (Transmit end) */
    [7] = BSP_PRV_IELS_ENUM(EVENT_SCI9_ERI),         /* SCI9 ERI (Receive error) */
    [8] = BSP_PRV_IELS_ENUM(EVENT_RTC_ALARM),        /* RTC ALARM (Alarm interrupt) */
    [9] = BSP_PRV_IELS_ENUM(EVENT_RTC_PERIOD),       /* RTC PERIOD (Periodic interrupt) */
    [10] = BSP_PRV_IELS_ENUM(EVENT_RTC_CARRY),       /* RTC CARRY (Carry interrupt) */
    [11] = BSP_PRV_IELS_ENUM(EVENT_AGT0_INT),        /* AGT0 INT (AGT interrupt) */
    [12] = BSP_PRV_IELS_ENUM(EVENT_AGT1_INT),        /* AGT1 INT (AGT interrupt) */
    [13] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ9),        /* ICU IRQ9 (External pin interrupt 9) */
    [14] = BSP_PRV_IELS_ENUM(EVENT_SPI0_RXI),        /* SPI0 RXI (Receive buffer full) */
    [15] = BSP_PRV_IELS_ENUM(EVENT_SPI0_TXI),        /* SPI0 TXI (Transmit buffer empty) */
    [16] = BSP_PRV_IELS_ENUM(EVENT_SPI0_TEI),        /* SPI0 TEI (Transmission complete event) */
    [17] = BSP_PRV_IELS_ENUM(EVENT_SPI0_ERI),        /* SPI0 ERI (Error) */
    [18] = BSP_PRV_IELS_ENUM(EVENT_IIC1_RXI),        /* IIC1 RXI (Receive data full) */
    [19] = BSP_PRV_IELS_ENUM(EVENT_IIC1_TXI),        /* IIC1 TXI (Transmit data empty) */
    [20] = BSP_PRV_IELS_ENUM(EVENT_IIC1_TEI),        /* IIC1 TEI (Transmit end) */
    [21] = BSP_PRV_IELS_ENUM(EVENT_IIC1_ERI),        /* IIC1 ERI (Transfer error) */
};
#endif
