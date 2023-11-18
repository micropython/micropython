/* generated vector header file - do not edit */
#ifndef VECTOR_DATA_H
#define VECTOR_DATA_H
#ifdef __cplusplus
extern "C" {
#endif
/* Number of interrupts allocated */
#ifndef VECTOR_DATA_IRQ_COUNT
#define VECTOR_DATA_IRQ_COUNT    (48)
#endif
/* ISR prototypes */
void sci_uart_rxi_isr(void);
void sci_uart_txi_isr(void);
void sci_uart_tei_isr(void);
void sci_uart_eri_isr(void);
void rtc_alarm_periodic_isr(void);
void rtc_carry_isr(void);
void agt_int_isr(void);
void r_icu_isr(void);
void spi_rxi_isr(void);
void spi_txi_isr(void);
void spi_tei_isr(void);
void spi_eri_isr(void);
void iic_master_rxi_isr(void);
void iic_master_txi_isr(void);
void iic_master_tei_isr(void);
void iic_master_eri_isr(void);
void sdhimmc_accs_isr(void);
void sdhimmc_card_isr(void);
void sdhimmc_dma_req_isr(void);
void ether_eint_isr(void);
void glcdc_line_detect_isr(void);
void sci_i2c_txi_isr(void);
void sci_i2c_tei_isr(void);
void sci_i2c_rxi_isr(void);
void pdc_int_isr(void);
void pdc_transfer_req_isr(void);
void jpeg_jedi_isr(void);
void jpeg_jdti_isr(void);
void usbfs_interrupt_handler(void);
void usbfs_resume_handler(void);
void usbfs_d0fifo_handler(void);
void usbfs_d1fifo_handler(void);
void usbhs_interrupt_handler(void);
void usbhs_d0fifo_handler(void);
void usbhs_d1fifo_handler(void);

/* Vector table allocations */
#define VECTOR_NUMBER_SCI7_RXI ((IRQn_Type)0)  /* SCI7 RXI (Received data full) */
#define SCI7_RXI_IRQn          ((IRQn_Type)0)  /* SCI7 RXI (Received data full) */
#define VECTOR_NUMBER_SCI7_TXI ((IRQn_Type)1)  /* SCI7 TXI (Transmit data empty) */
#define SCI7_TXI_IRQn          ((IRQn_Type)1)  /* SCI7 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI7_TEI ((IRQn_Type)2)  /* SCI7 TEI (Transmit end) */
#define SCI7_TEI_IRQn          ((IRQn_Type)2)  /* SCI7 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI7_ERI ((IRQn_Type)3)  /* SCI7 ERI (Receive error) */
#define SCI7_ERI_IRQn          ((IRQn_Type)3)  /* SCI7 ERI (Receive error) */
#define VECTOR_NUMBER_SCI8_RXI ((IRQn_Type)4)  /* SCI8 RXI (Received data full) */
#define SCI8_RXI_IRQn          ((IRQn_Type)4)  /* SCI8 RXI (Received data full) */
#define VECTOR_NUMBER_SCI8_TXI ((IRQn_Type)5)  /* SCI8 TXI (Transmit data empty) */
#define SCI8_TXI_IRQn          ((IRQn_Type)5)  /* SCI8 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI8_TEI ((IRQn_Type)6)  /* SCI8 TEI (Transmit end) */
#define SCI8_TEI_IRQn          ((IRQn_Type)6)  /* SCI8 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI8_ERI ((IRQn_Type)7)  /* SCI8 ERI (Receive error) */
#define SCI8_ERI_IRQn          ((IRQn_Type)7)  /* SCI8 ERI (Receive error) */
#define VECTOR_NUMBER_SCI9_RXI ((IRQn_Type)8)  /* SCI9 RXI (Received data full) */
#define SCI9_RXI_IRQn          ((IRQn_Type)8)  /* SCI9 RXI (Received data full) */
#define VECTOR_NUMBER_SCI9_TXI ((IRQn_Type)9)  /* SCI9 TXI (Transmit data empty) */
#define SCI9_TXI_IRQn          ((IRQn_Type)9)  /* SCI9 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI9_TEI ((IRQn_Type)10)  /* SCI9 TEI (Transmit end) */
#define SCI9_TEI_IRQn          ((IRQn_Type)10)  /* SCI9 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI9_ERI ((IRQn_Type)11)  /* SCI9 ERI (Receive error) */
#define SCI9_ERI_IRQn          ((IRQn_Type)11)  /* SCI9 ERI (Receive error) */
#define VECTOR_NUMBER_RTC_ALARM ((IRQn_Type)12)  /* RTC ALARM (Alarm interrupt) */
#define RTC_ALARM_IRQn          ((IRQn_Type)12)  /* RTC ALARM (Alarm interrupt) */
#define VECTOR_NUMBER_RTC_PERIOD ((IRQn_Type)13)  /* RTC PERIOD (Periodic interrupt) */
#define RTC_PERIOD_IRQn          ((IRQn_Type)13)  /* RTC PERIOD (Periodic interrupt) */
#define VECTOR_NUMBER_RTC_CARRY ((IRQn_Type)14)  /* RTC CARRY (Carry interrupt) */
#define RTC_CARRY_IRQn          ((IRQn_Type)14)  /* RTC CARRY (Carry interrupt) */
#define VECTOR_NUMBER_AGT0_INT ((IRQn_Type)15)  /* AGT0 INT (AGT interrupt) */
#define AGT0_INT_IRQn          ((IRQn_Type)15)  /* AGT0 INT (AGT interrupt) */
#define VECTOR_NUMBER_AGT1_INT ((IRQn_Type)16)  /* AGT1 INT (AGT interrupt) */
#define AGT1_INT_IRQn          ((IRQn_Type)16)  /* AGT1 INT (AGT interrupt) */
#define VECTOR_NUMBER_ICU_IRQ7 ((IRQn_Type)17)  /* ICU IRQ7 (External pin interrupt 7) */
#define ICU_IRQ7_IRQn          ((IRQn_Type)17)  /* ICU IRQ7 (External pin interrupt 7) */
#define VECTOR_NUMBER_ICU_IRQ11 ((IRQn_Type)18)  /* ICU IRQ11 (External pin interrupt 11) */
#define ICU_IRQ11_IRQn          ((IRQn_Type)18)  /* ICU IRQ11 (External pin interrupt 11) */
#define VECTOR_NUMBER_ICU_IRQ12 ((IRQn_Type)19)  /* ICU IRQ12 (External pin interrupt 12) */
#define ICU_IRQ12_IRQn          ((IRQn_Type)19)  /* ICU IRQ12 (External pin interrupt 12) */
#define VECTOR_NUMBER_ICU_IRQ13 ((IRQn_Type)20)  /* ICU IRQ13 (External pin interrupt 13) */
#define ICU_IRQ13_IRQn          ((IRQn_Type)20)  /* ICU IRQ13 (External pin interrupt 13) */
#define VECTOR_NUMBER_SPI1_RXI ((IRQn_Type)21)  /* SPI1 RXI (Receive buffer full) */
#define SPI1_RXI_IRQn          ((IRQn_Type)21)  /* SPI1 RXI (Receive buffer full) */
#define VECTOR_NUMBER_SPI1_TXI ((IRQn_Type)22)  /* SPI1 TXI (Transmit buffer empty) */
#define SPI1_TXI_IRQn          ((IRQn_Type)22)  /* SPI1 TXI (Transmit buffer empty) */
#define VECTOR_NUMBER_SPI1_TEI ((IRQn_Type)23)  /* SPI1 TEI (Transmission complete event) */
#define SPI1_TEI_IRQn          ((IRQn_Type)23)  /* SPI1 TEI (Transmission complete event) */
#define VECTOR_NUMBER_SPI1_ERI ((IRQn_Type)24)  /* SPI1 ERI (Error) */
#define SPI1_ERI_IRQn          ((IRQn_Type)24)  /* SPI1 ERI (Error) */
#define VECTOR_NUMBER_IIC1_RXI ((IRQn_Type)25)  /* IIC1 RXI (Receive data full) */
#define IIC1_RXI_IRQn          ((IRQn_Type)25)  /* IIC1 RXI (Receive data full) */
#define VECTOR_NUMBER_IIC1_TXI ((IRQn_Type)26)  /* IIC1 TXI (Transmit data empty) */
#define IIC1_TXI_IRQn          ((IRQn_Type)26)  /* IIC1 TXI (Transmit data empty) */
#define VECTOR_NUMBER_IIC1_TEI ((IRQn_Type)27)  /* IIC1 TEI (Transmit end) */
#define IIC1_TEI_IRQn          ((IRQn_Type)27)  /* IIC1 TEI (Transmit end) */
#define VECTOR_NUMBER_IIC1_ERI ((IRQn_Type)28)  /* IIC1 ERI (Transfer error) */
#define IIC1_ERI_IRQn          ((IRQn_Type)28)  /* IIC1 ERI (Transfer error) */
#define VECTOR_NUMBER_SDHIMMC1_ACCS ((IRQn_Type)29)  /* SDHIMMC1 ACCS (Card access) */
#define SDHIMMC1_ACCS_IRQn          ((IRQn_Type)29)  /* SDHIMMC1 ACCS (Card access) */
#define VECTOR_NUMBER_SDHIMMC1_CARD ((IRQn_Type)30)  /* SDHIMMC1 CARD (Card detect) */
#define SDHIMMC1_CARD_IRQn          ((IRQn_Type)30)  /* SDHIMMC1 CARD (Card detect) */
#define VECTOR_NUMBER_SDHIMMC1_DMA_REQ ((IRQn_Type)31)  /* SDHIMMC1 DMA REQ (DMA transfer request) */
#define SDHIMMC1_DMA_REQ_IRQn          ((IRQn_Type)31)  /* SDHIMMC1 DMA REQ (DMA transfer request) */
#define VECTOR_NUMBER_EDMAC0_EINT ((IRQn_Type)32)  /* EDMAC0 EINT (EDMAC 0 interrupt) */
#define EDMAC0_EINT_IRQn          ((IRQn_Type)32)  /* EDMAC0 EINT (EDMAC 0 interrupt) */
#define VECTOR_NUMBER_GLCDC_LINE_DETECT ((IRQn_Type)33)  /* GLCDC LINE DETECT (Specified line) */
#define GLCDC_LINE_DETECT_IRQn          ((IRQn_Type)33)  /* GLCDC LINE DETECT (Specified line) */
#define VECTOR_NUMBER_SCI0_TXI ((IRQn_Type)34)  /* SCI0 TXI (Transmit data empty) */
#define SCI0_TXI_IRQn          ((IRQn_Type)34)  /* SCI0 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI0_TEI ((IRQn_Type)35)  /* SCI0 TEI (Transmit end) */
#define SCI0_TEI_IRQn          ((IRQn_Type)35)  /* SCI0 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI0_RXI ((IRQn_Type)36)  /* SCI0 RXI (Receive data full) */
#define SCI0_RXI_IRQn          ((IRQn_Type)36)  /* SCI0 RXI (Receive data full) */
#define VECTOR_NUMBER_PDC_INT ((IRQn_Type)37)  /* PDC INT (Error interrupt) */
#define PDC_INT_IRQn          ((IRQn_Type)37)  /* PDC INT (Error interrupt) */
#define VECTOR_NUMBER_PDC_RECEIVE_DATA_READY ((IRQn_Type)38)  /* PDC RECEIVE DATA READY (Receive data ready interrupt) */
#define PDC_RECEIVE_DATA_READY_IRQn          ((IRQn_Type)38)  /* PDC RECEIVE DATA READY (Receive data ready interrupt) */
#define VECTOR_NUMBER_JPEG_JEDI ((IRQn_Type)39)  /* JPEG JEDI (Compression/decompression process interrupt) */
#define JPEG_JEDI_IRQn          ((IRQn_Type)39)  /* JPEG JEDI (Compression/decompression process interrupt) */
#define VECTOR_NUMBER_JPEG_JDTI ((IRQn_Type)40)  /* JPEG JDTI (Data transfer interrupt) */
#define JPEG_JDTI_IRQn          ((IRQn_Type)40)  /* JPEG JDTI (Data transfer interrupt) */
#define VECTOR_NUMBER_USBFS_INT ((IRQn_Type)41)  /* USBFS INT (USBFS interrupt) */
#define USBFS_INT_IRQn          ((IRQn_Type)41)  /* USBFS INT (USBFS interrupt) */
#define VECTOR_NUMBER_USBFS_RESUME ((IRQn_Type)42)  /* USBFS RESUME (USBFS resume interrupt) */
#define USBFS_RESUME_IRQn          ((IRQn_Type)42)  /* USBFS RESUME (USBFS resume interrupt) */
#define VECTOR_NUMBER_USBFS_FIFO_0 ((IRQn_Type)43)  /* USBFS FIFO 0 (DMA transfer request 0) */
#define USBFS_FIFO_0_IRQn          ((IRQn_Type)43)  /* USBFS FIFO 0 (DMA transfer request 0) */
#define VECTOR_NUMBER_USBFS_FIFO_1 ((IRQn_Type)44)  /* USBFS FIFO 1 (DMA transfer request 1) */
#define USBFS_FIFO_1_IRQn          ((IRQn_Type)44)  /* USBFS FIFO 1 (DMA transfer request 1) */
#define VECTOR_NUMBER_USBHS_USB_INT_RESUME ((IRQn_Type)45)  /* USBHS USB INT RESUME (USBHS interrupt) */
#define USBHS_USB_INT_RESUME_IRQn          ((IRQn_Type)45)  /* USBHS USB INT RESUME (USBHS interrupt) */
#define VECTOR_NUMBER_USBHS_FIFO_0 ((IRQn_Type)46)  /* USBHS FIFO 0 (DMA transfer request 0) */
#define USBHS_FIFO_0_IRQn          ((IRQn_Type)46)  /* USBHS FIFO 0 (DMA transfer request 0) */
#define VECTOR_NUMBER_USBHS_FIFO_1 ((IRQn_Type)47)  /* USBHS FIFO 1 (DMA transfer request 1) */
#define USBHS_FIFO_1_IRQn          ((IRQn_Type)47)  /* USBHS FIFO 1 (DMA transfer request 1) */
#ifdef __cplusplus
}
#endif
#endif /* VECTOR_DATA_H */
