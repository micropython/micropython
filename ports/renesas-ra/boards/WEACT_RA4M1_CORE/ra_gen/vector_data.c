/* generated vector source file - do not edit */
        #include "bsp_api.h"
/* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
        #if VECTOR_DATA_IRQ_COUNT > 0
BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_MAX_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
{
    [0] = rtc_alarm_periodic_isr,                     /* RTC ALARM (Alarm interrupt) */
    [1] = rtc_alarm_periodic_isr,         /* RTC PERIOD (Periodic interrupt) */
    [2] = rtc_carry_isr,         /* RTC CARRY (Carry interrupt) */
    [3] = spi_rxi_isr,         /* SPI0 RXI (Receive buffer full) */
    [4] = spi_txi_isr,         /* SPI0 TXI (Transmit buffer empty) */
    [5] = spi_tei_isr,         /* SPI0 TEI (Transmission complete event) */
    [6] = spi_eri_isr,         /* SPI0 ERI (Error) */
    [7] = sci_uart_rxi_isr,         /* SCI9 RXI (Received data full) */
    [8] = sci_uart_txi_isr,         /* SCI9 TXI (Transmit data empty) */
    [9] = sci_uart_tei_isr,         /* SCI9 TEI (Transmit end) */
    [10] = sci_uart_eri_isr,         /* SCI9 ERI (Receive error) */
    [11] = r_icu_isr,         /* ICU IRQ0 (External pin interrupt 0) */
    [12] = usbfs_d0fifo_handler,         /* USBFS FIFO 0 (DMA transfer request 0) */
    [13] = usbfs_d1fifo_handler,         /* USBFS FIFO 1 (DMA transfer request 1) */
    [14] = usbfs_resume_handler,         /* USBFS RESUME (USBFS resume interrupt) */
    [15] = usbfs_interrupt_handler,         /* USBFS INT (USBFS interrupt) */
};
const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_MAX_ENTRIES] =
{
    [0] = BSP_PRV_IELS_ENUM(EVENT_RTC_ALARM),         /* RTC ALARM (Alarm interrupt) */
    [1] = BSP_PRV_IELS_ENUM(EVENT_RTC_PERIOD),         /* RTC PERIOD (Periodic interrupt) */
    [2] = BSP_PRV_IELS_ENUM(EVENT_RTC_CARRY),         /* RTC CARRY (Carry interrupt) */
    [3] = BSP_PRV_IELS_ENUM(EVENT_SPI0_RXI),         /* SPI0 RXI (Receive buffer full) */
    [4] = BSP_PRV_IELS_ENUM(EVENT_SPI0_TXI),         /* SPI0 TXI (Transmit buffer empty) */
    [5] = BSP_PRV_IELS_ENUM(EVENT_SPI0_TEI),         /* SPI0 TEI (Transmission complete event) */
    [6] = BSP_PRV_IELS_ENUM(EVENT_SPI0_ERI),         /* SPI0 ERI (Error) */
    [7] = BSP_PRV_IELS_ENUM(EVENT_SCI9_RXI),         /* SCI9 RXI (Received data full) */
    [8] = BSP_PRV_IELS_ENUM(EVENT_SCI9_TXI),         /* SCI9 TXI (Transmit data empty) */
    [9] = BSP_PRV_IELS_ENUM(EVENT_SCI9_TEI),         /* SCI9 TEI (Transmit end) */
    [10] = BSP_PRV_IELS_ENUM(EVENT_SCI9_ERI),         /* SCI9 ERI (Receive error) */
    [11] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ0),         /* ICU IRQ0 (External pin interrupt 0) */
    [12] = BSP_PRV_IELS_ENUM(EVENT_USBFS_FIFO_0),         /* USBFS FIFO 0 (DMA transfer request 0) */
    [13] = BSP_PRV_IELS_ENUM(EVENT_USBFS_FIFO_1),         /* USBFS FIFO 1 (DMA transfer request 1) */
    [14] = BSP_PRV_IELS_ENUM(EVENT_USBFS_RESUME),         /* USBFS RESUME (USBFS resume interrupt) */
    [15] = BSP_PRV_IELS_ENUM(EVENT_USBFS_INT),         /* USBFS INT (USBFS interrupt) */
};
        #endif
