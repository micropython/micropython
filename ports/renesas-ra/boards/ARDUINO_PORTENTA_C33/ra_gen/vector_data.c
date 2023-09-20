/* generated vector source file - do not edit */
#include "bsp_api.h"
/* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
#if VECTOR_DATA_IRQ_COUNT > 0
BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_MAX_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
{
    [0] = sci_uart_rxi_isr,                     /* SCI9 RXI (Received data full) */
    [1] = sci_uart_txi_isr,         /* SCI9 TXI (Transmit data empty) */
    [2] = sci_uart_tei_isr,         /* SCI9 TEI (Transmit end) */
    [3] = sci_uart_eri_isr,         /* SCI9 ERI (Receive error) */
    [4] = rtc_alarm_periodic_isr,         /* RTC ALARM (Alarm interrupt) */
    [5] = rtc_alarm_periodic_isr,         /* RTC PERIOD (Periodic interrupt) */
    [6] = rtc_carry_isr,         /* RTC CARRY (Carry interrupt) */
    [7] = r_icu_isr,         /* ICU IRQ0 (External pin interrupt 0) */
    [8] = usbfs_interrupt_handler,         /* USBFS INT (USBFS interrupt) */
    [9] = usbfs_resume_handler,         /* USBFS RESUME (USBFS resume interrupt) */
    [10] = usbfs_d0fifo_handler,         /* USBFS FIFO 0 (DMA transfer request 0) */
    [11] = usbfs_d1fifo_handler,         /* USBFS FIFO 1 (DMA transfer request 1) */
    [12] = usbhs_interrupt_handler,         /* USBHS USB INT RESUME (USBHS interrupt) */
    [13] = usbhs_d0fifo_handler,         /* USBHS FIFO 0 (DMA transfer request 0) */
    [14] = usbhs_d1fifo_handler,         /* USBHS FIFO 1 (DMA transfer request 1) */
    [15] = iic_master_rxi_isr,         /* IIC0 RXI (Receive data full) */
    [16] = iic_master_txi_isr,         /* IIC0 TXI (Transmit data empty) */
    [17] = iic_master_tei_isr,         /* IIC0 TEI (Transmit end) */
    [18] = iic_master_eri_isr,         /* IIC0 ERI (Transfer error) */
    [19] = agt_int_isr,         /* AGT0 INT (AGT interrupt) */
    [20] = agt_int_isr,         /* AGT1 INT (AGT interrupt) */
    [21] = sci_uart_rxi_isr,         /* SCI8 RXI (Received data full) */
    [22] = sci_uart_txi_isr,         /* SCI8 TXI (Transmit data empty) */
    [23] = sci_uart_tei_isr,         /* SCI8 TEI (Transmit end) */
    [24] = sci_uart_eri_isr,         /* SCI8 ERI (Receive error) */
    [25] = spi_rxi_isr,         /* SPI1 RXI (Receive buffer full) */
    [26] = spi_txi_isr,         /* SPI1 TXI (Transmit buffer empty) */
    [27] = spi_tei_isr,         /* SPI1 TEI (Transmission complete event) */
    [28] = spi_eri_isr,         /* SPI1 ERI (Error) */
    [29] = r_icu_isr,         /* ICU IRQ2 (External pin interrupt 2) */
};
const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_MAX_ENTRIES] =
{
    [0] = BSP_PRV_IELS_ENUM(EVENT_SCI9_RXI),         /* SCI9 RXI (Received data full) */
    [1] = BSP_PRV_IELS_ENUM(EVENT_SCI9_TXI),         /* SCI9 TXI (Transmit data empty) */
    [2] = BSP_PRV_IELS_ENUM(EVENT_SCI9_TEI),         /* SCI9 TEI (Transmit end) */
    [3] = BSP_PRV_IELS_ENUM(EVENT_SCI9_ERI),         /* SCI9 ERI (Receive error) */
    [4] = BSP_PRV_IELS_ENUM(EVENT_RTC_ALARM),         /* RTC ALARM (Alarm interrupt) */
    [5] = BSP_PRV_IELS_ENUM(EVENT_RTC_PERIOD),         /* RTC PERIOD (Periodic interrupt) */
    [6] = BSP_PRV_IELS_ENUM(EVENT_RTC_CARRY),         /* RTC CARRY (Carry interrupt) */
    [7] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ0),         /* ICU IRQ0 (External pin interrupt 0) */
    [8] = BSP_PRV_IELS_ENUM(EVENT_USBFS_INT),         /* USBFS INT (USBFS interrupt) */
    [9] = BSP_PRV_IELS_ENUM(EVENT_USBFS_RESUME),         /* USBFS RESUME (USBFS resume interrupt) */
    [10] = BSP_PRV_IELS_ENUM(EVENT_USBFS_FIFO_0),         /* USBFS FIFO 0 (DMA transfer request 0) */
    [11] = BSP_PRV_IELS_ENUM(EVENT_USBFS_FIFO_1),         /* USBFS FIFO 1 (DMA transfer request 1) */
    [12] = BSP_PRV_IELS_ENUM(EVENT_USBHS_USB_INT_RESUME),         /* USBHS USB INT RESUME (USBHS interrupt) */
    [13] = BSP_PRV_IELS_ENUM(EVENT_USBHS_FIFO_0),         /* USBHS FIFO 0 (DMA transfer request 0) */
    [14] = BSP_PRV_IELS_ENUM(EVENT_USBHS_FIFO_1),         /* USBHS FIFO 1 (DMA transfer request 1) */
    [15] = BSP_PRV_IELS_ENUM(EVENT_IIC0_RXI),         /* IIC0 RXI (Receive data full) */
    [16] = BSP_PRV_IELS_ENUM(EVENT_IIC0_TXI),         /* IIC0 TXI (Transmit data empty) */
    [17] = BSP_PRV_IELS_ENUM(EVENT_IIC0_TEI),         /* IIC0 TEI (Transmit end) */
    [18] = BSP_PRV_IELS_ENUM(EVENT_IIC0_ERI),         /* IIC0 ERI (Transfer error) */
    [19] = BSP_PRV_IELS_ENUM(EVENT_AGT0_INT),         /* AGT0 INT (AGT interrupt) */
    [20] = BSP_PRV_IELS_ENUM(EVENT_AGT1_INT),         /* AGT1 INT (AGT interrupt) */
    [21] = BSP_PRV_IELS_ENUM(EVENT_SCI8_RXI),         /* SCI8 RXI (Received data full) */
    [22] = BSP_PRV_IELS_ENUM(EVENT_SCI8_TXI),         /* SCI8 TXI (Transmit data empty) */
    [23] = BSP_PRV_IELS_ENUM(EVENT_SCI8_TEI),         /* SCI8 TEI (Transmit end) */
    [24] = BSP_PRV_IELS_ENUM(EVENT_SCI8_ERI),         /* SCI8 ERI (Receive error) */
    [25] = BSP_PRV_IELS_ENUM(EVENT_SPI1_RXI),         /* SPI1 RXI (Receive buffer full) */
    [26] = BSP_PRV_IELS_ENUM(EVENT_SPI1_TXI),         /* SPI1 TXI (Transmit buffer empty) */
    [27] = BSP_PRV_IELS_ENUM(EVENT_SPI1_TEI),         /* SPI1 TEI (Transmission complete event) */
    [28] = BSP_PRV_IELS_ENUM(EVENT_SPI1_ERI),         /* SPI1 ERI (Error) */
    [29] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ2),         /* ICU IRQ2 (External pin interrupt 2) */
};
#endif
