/* generated vector source file - do not edit */
#include "bsp_api.h"
/* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
#if VECTOR_DATA_IRQ_COUNT > 0
BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_MAX_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
{
    [0] = sci_uart_rxi_isr,         /* SCI6 RXI (Received data full) */
    [1] = sci_uart_txi_isr,         /* SCI6 TXI (Transmit data empty) */
    [2] = sci_uart_tei_isr,         /* SCI6 TEI (Transmit end) */
    [3] = sci_uart_eri_isr,         /* SCI6 ERI (Receive error) */
    [4] = sci_uart_rxi_isr,         /* SCI7 RXI (Received data full) */
    [5] = sci_uart_txi_isr,         /* SCI7 TXI (Transmit data empty) */
    [6] = sci_uart_tei_isr,         /* SCI7 TEI (Transmit end) */
    [7] = sci_uart_eri_isr,         /* SCI7 ERI (Receive error) */
    [8] = sci_uart_rxi_isr,         /* SCI9 RXI (Received data full) */
    [9] = sci_uart_txi_isr,         /* SCI9 TXI (Transmit data empty) */
    [10] = sci_uart_tei_isr,         /* SCI9 TEI (Transmit end) */
    [11] = sci_uart_eri_isr,         /* SCI9 ERI (Receive error) */
    [12] = rtc_alarm_periodic_isr,         /* RTC ALARM (Alarm interrupt) */
    [13] = rtc_alarm_periodic_isr,         /* RTC PERIOD (Periodic interrupt) */
    [14] = rtc_carry_isr,         /* RTC CARRY (Carry interrupt) */
    [15] = agt_int_isr,         /* AGT0 INT (AGT interrupt) */
    [16] = agt_int_isr,         /* AGT1 INT (AGT interrupt) */
    [17] = r_icu_isr,         /* ICU IRQ7 (External pin interrupt 7) */
    [18] = r_icu_isr,         /* ICU IRQ13 (External pin interrupt 13) */
    [19] = r_icu_isr,         /* ICU IRQ14 (External pin interrupt 14) */
    [20] = spi_rxi_isr,         /* SPI0 RXI (Receive buffer full) */
    [21] = spi_txi_isr,         /* SPI0 TXI (Transmit buffer empty) */
    [22] = spi_tei_isr,         /* SPI0 TEI (Transmission complete event) */
    [23] = spi_eri_isr,         /* SPI0 ERI (Error) */
    [24] = iic_master_rxi_isr,         /* IIC2 RXI (Receive data full) */
    [25] = iic_master_txi_isr,         /* IIC2 TXI (Transmit data empty) */
    [26] = iic_master_tei_isr,         /* IIC2 TEI (Transmit end) */
    [27] = iic_master_eri_isr,         /* IIC2 ERI (Transfer error) */
    [28] = sdhimmc_accs_isr,         /* SDHIMMC0 ACCS (Card access) */
    [29] = sdhimmc_card_isr,         /* SDHIMMC0 CARD (Card detect) */
    [30] = sdhimmc_dma_req_isr,         /* SDHIMMC0 DMA REQ (DMA transfer request) */
};
const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_MAX_ENTRIES] =
{
    [0] = BSP_PRV_IELS_ENUM(EVENT_SCI6_RXI),         /* SCI6 RXI (Received data full) */
    [1] = BSP_PRV_IELS_ENUM(EVENT_SCI6_TXI),         /* SCI6 TXI (Transmit data empty) */
    [2] = BSP_PRV_IELS_ENUM(EVENT_SCI6_TEI),         /* SCI6 TEI (Transmit end) */
    [3] = BSP_PRV_IELS_ENUM(EVENT_SCI6_ERI),         /* SCI6 ERI (Receive error) */
    [4] = BSP_PRV_IELS_ENUM(EVENT_SCI7_RXI),         /* SCI7 RXI (Received data full) */
    [5] = BSP_PRV_IELS_ENUM(EVENT_SCI7_TXI),         /* SCI7 TXI (Transmit data empty) */
    [6] = BSP_PRV_IELS_ENUM(EVENT_SCI7_TEI),         /* SCI7 TEI (Transmit end) */
    [7] = BSP_PRV_IELS_ENUM(EVENT_SCI7_ERI),         /* SCI7 ERI (Receive error) */
    [8] = BSP_PRV_IELS_ENUM(EVENT_SCI9_RXI),         /* SCI9 RXI (Received data full) */
    [9] = BSP_PRV_IELS_ENUM(EVENT_SCI9_TXI),         /* SCI9 TXI (Transmit data empty) */
    [10] = BSP_PRV_IELS_ENUM(EVENT_SCI9_TEI),         /* SCI9 TEI (Transmit end) */
    [11] = BSP_PRV_IELS_ENUM(EVENT_SCI9_ERI),         /* SCI9 ERI (Receive error) */
    [12] = BSP_PRV_IELS_ENUM(EVENT_RTC_ALARM),         /* RTC ALARM (Alarm interrupt) */
    [13] = BSP_PRV_IELS_ENUM(EVENT_RTC_PERIOD),         /* RTC PERIOD (Periodic interrupt) */
    [14] = BSP_PRV_IELS_ENUM(EVENT_RTC_CARRY),         /* RTC CARRY (Carry interrupt) */
    [15] = BSP_PRV_IELS_ENUM(EVENT_AGT0_INT),         /* AGT0 INT (AGT interrupt) */
    [16] = BSP_PRV_IELS_ENUM(EVENT_AGT1_INT),         /* AGT1 INT (AGT interrupt) */
    [17] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ7),         /* ICU IRQ7 (External pin interrupt 7) */
    [18] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ13),         /* ICU IRQ13 (External pin interrupt 13) */
    [19] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ14),         /* ICU IRQ14 (External pin interrupt 14) */
    [20] = BSP_PRV_IELS_ENUM(EVENT_SPI0_RXI),         /* SPI0 RXI (Receive buffer full) */
    [21] = BSP_PRV_IELS_ENUM(EVENT_SPI0_TXI),         /* SPI0 TXI (Transmit buffer empty) */
    [22] = BSP_PRV_IELS_ENUM(EVENT_SPI0_TEI),         /* SPI0 TEI (Transmission complete event) */
    [23] = BSP_PRV_IELS_ENUM(EVENT_SPI0_ERI),         /* SPI0 ERI (Error) */
    [24] = BSP_PRV_IELS_ENUM(EVENT_IIC2_RXI),         /* IIC2 RXI (Receive data full) */
    [25] = BSP_PRV_IELS_ENUM(EVENT_IIC2_TXI),         /* IIC2 TXI (Transmit data empty) */
    [26] = BSP_PRV_IELS_ENUM(EVENT_IIC2_TEI),         /* IIC2 TEI (Transmit end) */
    [27] = BSP_PRV_IELS_ENUM(EVENT_IIC2_ERI),         /* IIC2 ERI (Transfer error) */
    [28] = BSP_PRV_IELS_ENUM(EVENT_SDHIMMC0_ACCS),         /* SDHIMMC0 ACCS (Card access) */
    [29] = BSP_PRV_IELS_ENUM(EVENT_SDHIMMC0_CARD),         /* SDHIMMC0 CARD (Card detect) */
    [30] = BSP_PRV_IELS_ENUM(EVENT_SDHIMMC0_DMA_REQ),         /* SDHIMMC0 DMA REQ (DMA transfer request) */
};
#endif
