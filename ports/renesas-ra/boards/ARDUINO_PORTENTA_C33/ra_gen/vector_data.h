/* generated vector header file - do not edit */
#ifndef VECTOR_DATA_H
#define VECTOR_DATA_H
#ifdef __cplusplus
extern "C" {
#endif
/* Number of interrupts allocated */
#ifndef VECTOR_DATA_IRQ_COUNT
#define VECTOR_DATA_IRQ_COUNT    (30)
#endif
/* ISR prototypes */
void sci_uart_rxi_isr(void);
void sci_uart_txi_isr(void);
void sci_uart_tei_isr(void);
void sci_uart_eri_isr(void);
void rtc_alarm_periodic_isr(void);
void rtc_carry_isr(void);
void r_icu_isr(void);
void usbfs_interrupt_handler(void);
void usbfs_resume_handler(void);
void usbfs_d0fifo_handler(void);
void usbfs_d1fifo_handler(void);
void usbhs_interrupt_handler(void);
void usbhs_d0fifo_handler(void);
void usbhs_d1fifo_handler(void);
void iic_master_rxi_isr(void);
void iic_master_txi_isr(void);
void iic_master_tei_isr(void);
void iic_master_eri_isr(void);
void agt_int_isr(void);
void spi_rxi_isr(void);
void spi_txi_isr(void);
void spi_tei_isr(void);
void spi_eri_isr(void);

/* Vector table allocations */
#define VECTOR_NUMBER_SCI9_RXI ((IRQn_Type)0)  /* SCI9 RXI (Received data full) */
#define SCI9_RXI_IRQn          ((IRQn_Type)0)  /* SCI9 RXI (Received data full) */
#define VECTOR_NUMBER_SCI9_TXI ((IRQn_Type)1)  /* SCI9 TXI (Transmit data empty) */
#define SCI9_TXI_IRQn          ((IRQn_Type)1)  /* SCI9 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI9_TEI ((IRQn_Type)2)  /* SCI9 TEI (Transmit end) */
#define SCI9_TEI_IRQn          ((IRQn_Type)2)  /* SCI9 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI9_ERI ((IRQn_Type)3)  /* SCI9 ERI (Receive error) */
#define SCI9_ERI_IRQn          ((IRQn_Type)3)  /* SCI9 ERI (Receive error) */
#define VECTOR_NUMBER_RTC_ALARM ((IRQn_Type)4)  /* RTC ALARM (Alarm interrupt) */
#define RTC_ALARM_IRQn          ((IRQn_Type)4)  /* RTC ALARM (Alarm interrupt) */
#define VECTOR_NUMBER_RTC_PERIOD ((IRQn_Type)5)  /* RTC PERIOD (Periodic interrupt) */
#define RTC_PERIOD_IRQn          ((IRQn_Type)5)  /* RTC PERIOD (Periodic interrupt) */
#define VECTOR_NUMBER_RTC_CARRY ((IRQn_Type)6)  /* RTC CARRY (Carry interrupt) */
#define RTC_CARRY_IRQn          ((IRQn_Type)6)  /* RTC CARRY (Carry interrupt) */
#define VECTOR_NUMBER_ICU_IRQ0 ((IRQn_Type)7)  /* ICU IRQ0 (External pin interrupt 0) */
#define ICU_IRQ0_IRQn          ((IRQn_Type)7)  /* ICU IRQ0 (External pin interrupt 0) */
#define VECTOR_NUMBER_USBFS_INT ((IRQn_Type)8)  /* USBFS INT (USBFS interrupt) */
#define USBFS_INT_IRQn          ((IRQn_Type)8)  /* USBFS INT (USBFS interrupt) */
#define VECTOR_NUMBER_USBFS_RESUME ((IRQn_Type)9)  /* USBFS RESUME (USBFS resume interrupt) */
#define USBFS_RESUME_IRQn          ((IRQn_Type)9)  /* USBFS RESUME (USBFS resume interrupt) */
#define VECTOR_NUMBER_USBFS_FIFO_0 ((IRQn_Type)10)  /* USBFS FIFO 0 (DMA transfer request 0) */
#define USBFS_FIFO_0_IRQn          ((IRQn_Type)10)  /* USBFS FIFO 0 (DMA transfer request 0) */
#define VECTOR_NUMBER_USBFS_FIFO_1 ((IRQn_Type)11)  /* USBFS FIFO 1 (DMA transfer request 1) */
#define USBFS_FIFO_1_IRQn          ((IRQn_Type)11)  /* USBFS FIFO 1 (DMA transfer request 1) */
#define VECTOR_NUMBER_USBHS_USB_INT_RESUME ((IRQn_Type)12)  /* USBHS USB INT RESUME (USBHS interrupt) */
#define USBHS_USB_INT_RESUME_IRQn          ((IRQn_Type)12)  /* USBHS USB INT RESUME (USBHS interrupt) */
#define VECTOR_NUMBER_USBHS_FIFO_0 ((IRQn_Type)13)  /* USBHS FIFO 0 (DMA transfer request 0) */
#define USBHS_FIFO_0_IRQn          ((IRQn_Type)13)  /* USBHS FIFO 0 (DMA transfer request 0) */
#define VECTOR_NUMBER_USBHS_FIFO_1 ((IRQn_Type)14)  /* USBHS FIFO 1 (DMA transfer request 1) */
#define USBHS_FIFO_1_IRQn          ((IRQn_Type)14)  /* USBHS FIFO 1 (DMA transfer request 1) */
#define VECTOR_NUMBER_IIC0_RXI ((IRQn_Type)15)  /* IIC0 RXI (Receive data full) */
#define IIC0_RXI_IRQn          ((IRQn_Type)15)  /* IIC0 RXI (Receive data full) */
#define VECTOR_NUMBER_IIC0_TXI ((IRQn_Type)16)  /* IIC0 TXI (Transmit data empty) */
#define IIC0_TXI_IRQn          ((IRQn_Type)16)  /* IIC0 TXI (Transmit data empty) */
#define VECTOR_NUMBER_IIC0_TEI ((IRQn_Type)17)  /* IIC0 TEI (Transmit end) */
#define IIC0_TEI_IRQn          ((IRQn_Type)17)  /* IIC0 TEI (Transmit end) */
#define VECTOR_NUMBER_IIC0_ERI ((IRQn_Type)18)  /* IIC0 ERI (Transfer error) */
#define IIC0_ERI_IRQn          ((IRQn_Type)18)  /* IIC0 ERI (Transfer error) */
#define VECTOR_NUMBER_AGT0_INT ((IRQn_Type)19)  /* AGT0 INT (AGT interrupt) */
#define AGT0_INT_IRQn          ((IRQn_Type)19)  /* AGT0 INT (AGT interrupt) */
#define VECTOR_NUMBER_AGT1_INT ((IRQn_Type)20)  /* AGT1 INT (AGT interrupt) */
#define AGT1_INT_IRQn          ((IRQn_Type)20)  /* AGT1 INT (AGT interrupt) */
#define VECTOR_NUMBER_SCI8_RXI ((IRQn_Type)21)  /* SCI8 RXI (Received data full) */
#define SCI8_RXI_IRQn          ((IRQn_Type)21)  /* SCI8 RXI (Received data full) */
#define VECTOR_NUMBER_SCI8_TXI ((IRQn_Type)22)  /* SCI8 TXI (Transmit data empty) */
#define SCI8_TXI_IRQn          ((IRQn_Type)22)  /* SCI8 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI8_TEI ((IRQn_Type)23)  /* SCI8 TEI (Transmit end) */
#define SCI8_TEI_IRQn          ((IRQn_Type)23)  /* SCI8 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI8_ERI ((IRQn_Type)24)  /* SCI8 ERI (Receive error) */
#define SCI8_ERI_IRQn          ((IRQn_Type)24)  /* SCI8 ERI (Receive error) */
#define VECTOR_NUMBER_SPI1_RXI ((IRQn_Type)25)  /* SPI1 RXI (Receive buffer full) */
#define SPI1_RXI_IRQn          ((IRQn_Type)25)  /* SPI1 RXI (Receive buffer full) */
#define VECTOR_NUMBER_SPI1_TXI ((IRQn_Type)26)  /* SPI1 TXI (Transmit buffer empty) */
#define SPI1_TXI_IRQn          ((IRQn_Type)26)  /* SPI1 TXI (Transmit buffer empty) */
#define VECTOR_NUMBER_SPI1_TEI ((IRQn_Type)27)  /* SPI1 TEI (Transmission complete event) */
#define SPI1_TEI_IRQn          ((IRQn_Type)27)  /* SPI1 TEI (Transmission complete event) */
#define VECTOR_NUMBER_SPI1_ERI ((IRQn_Type)28)  /* SPI1 ERI (Error) */
#define SPI1_ERI_IRQn          ((IRQn_Type)28)  /* SPI1 ERI (Error) */
#define VECTOR_NUMBER_ICU_IRQ2 ((IRQn_Type)29)  /* ICU IRQ2 (External pin interrupt 2) */
#define ICU_IRQ2_IRQn          ((IRQn_Type)29)  /* ICU IRQ2 (External pin interrupt 2) */
#ifdef __cplusplus
}
#endif
#endif /* VECTOR_DATA_H */
