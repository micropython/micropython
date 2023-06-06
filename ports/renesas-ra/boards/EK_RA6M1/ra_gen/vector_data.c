/* generated vector source file - do not edit */
#include "bsp_api.h"
/* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
#if VECTOR_DATA_IRQ_COUNT > 0
BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_MAX_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
{
    [0] = sci_uart_rxi_isr,                     /* SCI0 RXI (Receive data full) */
    [1] = sci_uart_txi_isr,         /* SCI0 TXI (Transmit data empty) */
    [2] = sci_uart_tei_isr,         /* SCI0 TEI (Transmit end) */
    [3] = sci_uart_eri_isr,         /* SCI0 ERI (Receive error) */
    [4] = sci_uart_rxi_isr,         /* SCI2 RXI (Received data full) */
    [5] = sci_uart_txi_isr,         /* SCI2 TXI (Transmit data empty) */
    [6] = sci_uart_tei_isr,         /* SCI2 TEI (Transmit end) */
    [7] = sci_uart_eri_isr,         /* SCI2 ERI (Receive error) */
    [8] = sci_uart_rxi_isr,         /* SCI8 RXI (Received data full) */
    [9] = sci_uart_txi_isr,         /* SCI8 TXI (Transmit data empty) */
    [10] = sci_uart_tei_isr,         /* SCI8 TEI (Transmit end) */
    [11] = sci_uart_eri_isr,         /* SCI8 ERI (Receive error) */
    [12] = rtc_alarm_periodic_isr,         /* RTC ALARM (Alarm interrupt) */
    [13] = rtc_alarm_periodic_isr,         /* RTC PERIOD (Periodic interrupt) */
    [14] = rtc_carry_isr,         /* RTC CARRY (Carry interrupt) */
    [15] = agt_int_isr,         /* AGT0 INT (AGT interrupt) */
    [16] = agt_int_isr,         /* AGT1 INT (AGT interrupt) */
    [17] = r_icu_isr,         /* ICU IRQ0 (External pin interrupt 0) */
    [18] = r_icu_isr,         /* ICU IRQ1 (External pin interrupt 1) */
    [19] = r_icu_isr,         /* ICU IRQ2 (External pin interrupt 2) */
    [20] = r_icu_isr,         /* ICU IRQ3 (External pin interrupt 3) */
    [21] = r_icu_isr,         /* ICU IRQ4 (External pin interrupt 4) */
    [22] = r_icu_isr,         /* ICU IRQ5 (External pin interrupt 5) */
    [23] = r_icu_isr,         /* ICU IRQ6 (External pin interrupt 6) */
    [24] = r_icu_isr,         /* ICU IRQ7 (External pin interrupt 7) */
    [25] = r_icu_isr,         /* ICU IRQ8 (External pin interrupt 8) */
    [26] = r_icu_isr,         /* ICU IRQ9 (External pin interrupt 9) */
    [27] = r_icu_isr,         /* ICU IRQ10 (External pin interrupt 10) */
    [28] = r_icu_isr,         /* ICU IRQ11 (External pin interrupt 11) */
    [29] = r_icu_isr,         /* ICU IRQ12 (External pin interrupt 12) */
    [30] = r_icu_isr,         /* ICU IRQ13 (External pin interrupt 13) */
    [31] = spi_rxi_isr,         /* SPI0 RXI (Receive buffer full) */
    [32] = spi_txi_isr,         /* SPI0 TXI (Transmit buffer empty) */
    [33] = spi_tei_isr,         /* SPI0 TEI (Transmission complete event) */
    [34] = spi_eri_isr,         /* SPI0 ERI (Error) */
    [35] = iic_master_rxi_isr,         /* IIC0 RXI (Receive data full) */
    [36] = iic_master_txi_isr,         /* IIC0 TXI (Transmit data empty) */
    [37] = iic_master_tei_isr,         /* IIC0 TEI (Transmit end) */
    [38] = iic_master_eri_isr,         /* IIC0 ERI (Transfer error) */
};
const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_MAX_ENTRIES] =
{
    [0] = BSP_PRV_IELS_ENUM(EVENT_SCI0_RXI),         /* SCI0 RXI (Receive data full) */
    [1] = BSP_PRV_IELS_ENUM(EVENT_SCI0_TXI),         /* SCI0 TXI (Transmit data empty) */
    [2] = BSP_PRV_IELS_ENUM(EVENT_SCI0_TEI),         /* SCI0 TEI (Transmit end) */
    [3] = BSP_PRV_IELS_ENUM(EVENT_SCI0_ERI),         /* SCI0 ERI (Receive error) */
    [4] = BSP_PRV_IELS_ENUM(EVENT_SCI2_RXI),         /* SCI2 RXI (Received data full) */
    [5] = BSP_PRV_IELS_ENUM(EVENT_SCI2_TXI),         /* SCI2 TXI (Transmit data empty) */
    [6] = BSP_PRV_IELS_ENUM(EVENT_SCI2_TEI),         /* SCI2 TEI (Transmit end) */
    [7] = BSP_PRV_IELS_ENUM(EVENT_SCI2_ERI),         /* SCI2 ERI (Receive error) */
    [8] = BSP_PRV_IELS_ENUM(EVENT_SCI8_RXI),         /* SCI8 RXI (Received data full) */
    [9] = BSP_PRV_IELS_ENUM(EVENT_SCI8_TXI),         /* SCI8 TXI (Transmit data empty) */
    [10] = BSP_PRV_IELS_ENUM(EVENT_SCI8_TEI),         /* SCI8 TEI (Transmit end) */
    [11] = BSP_PRV_IELS_ENUM(EVENT_SCI8_ERI),         /* SCI8 ERI (Receive error) */
    [12] = BSP_PRV_IELS_ENUM(EVENT_RTC_ALARM),         /* RTC ALARM (Alarm interrupt) */
    [13] = BSP_PRV_IELS_ENUM(EVENT_RTC_PERIOD),         /* RTC PERIOD (Periodic interrupt) */
    [14] = BSP_PRV_IELS_ENUM(EVENT_RTC_CARRY),         /* RTC CARRY (Carry interrupt) */
    [15] = BSP_PRV_IELS_ENUM(EVENT_AGT0_INT),         /* AGT0 INT (AGT interrupt) */
    [16] = BSP_PRV_IELS_ENUM(EVENT_AGT1_INT),         /* AGT1 INT (AGT interrupt) */
    [17] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ0),         /* ICU IRQ0 (External pin interrupt 0) */
    [18] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ1),         /* ICU IRQ1 (External pin interrupt 1) */
    [19] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ2),         /* ICU IRQ2 (External pin interrupt 2) */
    [20] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ3),         /* ICU IRQ3 (External pin interrupt 3) */
    [21] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ4),         /* ICU IRQ4 (External pin interrupt 4) */
    [22] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ5),         /* ICU IRQ5 (External pin interrupt 5) */
    [23] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ6),         /* ICU IRQ6 (External pin interrupt 6) */
    [24] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ7),         /* ICU IRQ7 (External pin interrupt 7) */
    [25] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ8),         /* ICU IRQ8 (External pin interrupt 8) */
    [26] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ9),         /* ICU IRQ9 (External pin interrupt 9) */
    [27] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ10),         /* ICU IRQ10 (External pin interrupt 10) */
    [28] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ11),         /* ICU IRQ11 (External pin interrupt 11) */
    [29] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ12),         /* ICU IRQ12 (External pin interrupt 12) */
    [30] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ13),         /* ICU IRQ13 (External pin interrupt 13) */
    [31] = BSP_PRV_IELS_ENUM(EVENT_SPI0_RXI),         /* SPI0 RXI (Receive buffer full) */
    [32] = BSP_PRV_IELS_ENUM(EVENT_SPI0_TXI),         /* SPI0 TXI (Transmit buffer empty) */
    [33] = BSP_PRV_IELS_ENUM(EVENT_SPI0_TEI),         /* SPI0 TEI (Transmission complete event) */
    [34] = BSP_PRV_IELS_ENUM(EVENT_SPI0_ERI),         /* SPI0 ERI (Error) */
    [35] = BSP_PRV_IELS_ENUM(EVENT_IIC0_RXI),         /* IIC0 RXI (Receive data full) */
    [36] = BSP_PRV_IELS_ENUM(EVENT_IIC0_TXI),         /* IIC0 TXI (Transmit data empty) */
    [37] = BSP_PRV_IELS_ENUM(EVENT_IIC0_TEI),         /* IIC0 TEI (Transmit end) */
    [38] = BSP_PRV_IELS_ENUM(EVENT_IIC0_ERI),         /* IIC0 ERI (Transfer error) */
};
#endif
