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

/* Vector table allocations */
#define VECTOR_NUMBER_SCI1_RXI ((IRQn_Type)0)  /* SCI1 RXI (Received data full) */
#define SCI1_RXI_IRQn          ((IRQn_Type)0)  /* SCI1 RXI (Received data full) */
#define VECTOR_NUMBER_SCI1_TXI ((IRQn_Type)1)  /* SCI1 TXI (Transmit data empty) */
#define SCI1_TXI_IRQn          ((IRQn_Type)1)  /* SCI1 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI1_TEI ((IRQn_Type)2)  /* SCI1 TEI (Transmit end) */
#define SCI1_TEI_IRQn          ((IRQn_Type)2)  /* SCI1 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI1_ERI ((IRQn_Type)3)  /* SCI1 ERI (Receive error) */
#define SCI1_ERI_IRQn          ((IRQn_Type)3)  /* SCI1 ERI (Receive error) */
#define VECTOR_NUMBER_SCI4_RXI ((IRQn_Type)4)  /* SCI4 RXI (Received data full) */
#define SCI4_RXI_IRQn          ((IRQn_Type)4)  /* SCI4 RXI (Received data full) */
#define VECTOR_NUMBER_SCI4_TXI ((IRQn_Type)5)  /* SCI4 TXI (Transmit data empty) */
#define SCI4_TXI_IRQn          ((IRQn_Type)5)  /* SCI4 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI4_TEI ((IRQn_Type)6)  /* SCI4 TEI (Transmit end) */
#define SCI4_TEI_IRQn          ((IRQn_Type)6)  /* SCI4 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI4_ERI ((IRQn_Type)7)  /* SCI4 ERI (Receive error) */
#define SCI4_ERI_IRQn          ((IRQn_Type)7)  /* SCI4 ERI (Receive error) */
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
#define VECTOR_NUMBER_ICU_IRQ0 ((IRQn_Type)17)  /* ICU IRQ0 (External pin interrupt 0) */
#define ICU_IRQ0_IRQn          ((IRQn_Type)17)  /* ICU IRQ0 (External pin interrupt 0) */
#define VECTOR_NUMBER_ICU_IRQ1 ((IRQn_Type)18)  /* ICU IRQ1 (External pin interrupt 1) */
#define ICU_IRQ1_IRQn          ((IRQn_Type)18)  /* ICU IRQ1 (External pin interrupt 1) */
#define VECTOR_NUMBER_ICU_IRQ2 ((IRQn_Type)19)  /* ICU IRQ2 (External pin interrupt 2) */
#define ICU_IRQ2_IRQn          ((IRQn_Type)19)  /* ICU IRQ2 (External pin interrupt 2) */
#define VECTOR_NUMBER_ICU_IRQ3 ((IRQn_Type)20)  /* ICU IRQ3 (External pin interrupt 3) */
#define ICU_IRQ3_IRQn          ((IRQn_Type)20)  /* ICU IRQ3 (External pin interrupt 3) */
#define VECTOR_NUMBER_ICU_IRQ4 ((IRQn_Type)21)  /* ICU IRQ4 (External pin interrupt 4) */
#define ICU_IRQ4_IRQn          ((IRQn_Type)21)  /* ICU IRQ4 (External pin interrupt 4) */
#define VECTOR_NUMBER_SPI0_RXI ((IRQn_Type)22)  /* SPI0 RXI (Receive buffer full) */
#define SPI0_RXI_IRQn          ((IRQn_Type)22)  /* SPI0 RXI (Receive buffer full) */
#define VECTOR_NUMBER_SPI0_TXI ((IRQn_Type)23)  /* SPI0 TXI (Transmit buffer empty) */
#define SPI0_TXI_IRQn          ((IRQn_Type)23)  /* SPI0 TXI (Transmit buffer empty) */
#define VECTOR_NUMBER_SPI0_TEI ((IRQn_Type)24)  /* SPI0 TEI (Transmission complete event) */
#define SPI0_TEI_IRQn          ((IRQn_Type)24)  /* SPI0 TEI (Transmission complete event) */
#define VECTOR_NUMBER_SPI0_ERI ((IRQn_Type)25)  /* SPI0 ERI (Error) */
#define SPI0_ERI_IRQn          ((IRQn_Type)25)  /* SPI0 ERI (Error) */
#define VECTOR_NUMBER_IIC0_RXI ((IRQn_Type)26)  /* IIC0 RXI (Receive data full) */
#define IIC0_RXI_IRQn          ((IRQn_Type)26)  /* IIC0 RXI (Receive data full) */
#define VECTOR_NUMBER_IIC0_TXI ((IRQn_Type)27)  /* IIC0 TXI (Transmit data empty) */
#define IIC0_TXI_IRQn          ((IRQn_Type)27)  /* IIC0 TXI (Transmit data empty) */
#define VECTOR_NUMBER_IIC0_TEI ((IRQn_Type)28)  /* IIC0 TEI (Transmit end) */
#define IIC0_TEI_IRQn          ((IRQn_Type)28)  /* IIC0 TEI (Transmit end) */
#define VECTOR_NUMBER_IIC0_ERI ((IRQn_Type)29)  /* IIC0 ERI (Transfer error) */
#define IIC0_ERI_IRQn          ((IRQn_Type)29)  /* IIC0 ERI (Transfer error) */
#ifdef __cplusplus
}
#endif
#endif /* VECTOR_DATA_H */
