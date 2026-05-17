/* generated vector header file - do not edit */
#ifndef VECTOR_DATA_H
#define VECTOR_DATA_H
#ifdef __cplusplus
extern "C" {
#endif
/* Number of interrupts allocated */
#ifndef VECTOR_DATA_IRQ_COUNT
#define VECTOR_DATA_IRQ_COUNT    (21)
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

/* Vector table allocations */
#define VECTOR_NUMBER_SCI0_RXI ((IRQn_Type)0)  /* SCI0 RXI (Receive data full) */
#define SCI0_RXI_IRQn          ((IRQn_Type)0)  /* SCI0 RXI (Receive data full) */
#define VECTOR_NUMBER_SCI0_TXI ((IRQn_Type)1)  /* SCI0 TXI (Transmit data empty) */
#define SCI0_TXI_IRQn          ((IRQn_Type)1)  /* SCI0 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI0_TEI ((IRQn_Type)2)  /* SCI0 TEI (Transmit end) */
#define SCI0_TEI_IRQn          ((IRQn_Type)2)  /* SCI0 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI0_ERI ((IRQn_Type)3)  /* SCI0 ERI (Receive error) */
#define SCI0_ERI_IRQn          ((IRQn_Type)3)  /* SCI0 ERI (Receive error) */
#define VECTOR_NUMBER_SCI1_RXI ((IRQn_Type)4)  /* SCI1 RXI (Received data full) */
#define SCI1_RXI_IRQn          ((IRQn_Type)4)  /* SCI1 RXI (Received data full) */
#define VECTOR_NUMBER_SCI1_TXI ((IRQn_Type)5)  /* SCI1 TXI (Transmit data empty) */
#define SCI1_TXI_IRQn          ((IRQn_Type)5)  /* SCI1 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI1_TEI ((IRQn_Type)6)  /* SCI1 TEI (Transmit end) */
#define SCI1_TEI_IRQn          ((IRQn_Type)6)  /* SCI1 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI1_ERI ((IRQn_Type)7)  /* SCI1 ERI (Receive error) */
#define SCI1_ERI_IRQn          ((IRQn_Type)7)  /* SCI1 ERI (Receive error) */
#define VECTOR_NUMBER_SCI2_RXI ((IRQn_Type)8)  /* SCI2 RXI (Received data full) */
#define SCI2_RXI_IRQn          ((IRQn_Type)8)  /* SCI2 RXI (Received data full) */
#define VECTOR_NUMBER_SCI2_TXI ((IRQn_Type)9)  /* SCI2 TXI (Transmit data empty) */
#define SCI2_TXI_IRQn          ((IRQn_Type)9)  /* SCI2 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI2_TEI ((IRQn_Type)10)  /* SCI2 TEI (Transmit end) */
#define SCI2_TEI_IRQn          ((IRQn_Type)10)  /* SCI2 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI2_ERI ((IRQn_Type)11)  /* SCI2 ERI (Receive error) */
#define SCI2_ERI_IRQn          ((IRQn_Type)11)  /* SCI2 ERI (Receive error) */
#define VECTOR_NUMBER_RTC_ALARM ((IRQn_Type)12)  /* RTC ALARM (Alarm interrupt) */
#define RTC_ALARM_IRQn          ((IRQn_Type)12)  /* RTC ALARM (Alarm interrupt) */
#define VECTOR_NUMBER_RTC_PERIOD ((IRQn_Type)13)  /* RTC PERIOD (Periodic interrupt) */
#define RTC_PERIOD_IRQn          ((IRQn_Type)13)  /* RTC PERIOD (Periodic interrupt) */
#define VECTOR_NUMBER_RTC_CARRY ((IRQn_Type)14)  /* RTC CARRY (Carry interrupt) */
#define RTC_CARRY_IRQn          ((IRQn_Type)14)  /* RTC CARRY (Carry interrupt) */
#define VECTOR_NUMBER_AGT0_INT ((IRQn_Type)15)  /* AGT0 INT (AGT interrupt) */
#define AGT0_INT_IRQn          ((IRQn_Type)15)  /* AGT0 INT (AGT interrupt) */
#define VECTOR_NUMBER_ICU_IRQ0 ((IRQn_Type)16)  /* ICU IRQ0 (External pin interrupt 0) */
#define ICU_IRQ0_IRQn          ((IRQn_Type)16)  /* ICU IRQ0 (External pin interrupt 0) */
#define VECTOR_NUMBER_SPI0_RXI ((IRQn_Type)17)  /* SPI0 RXI (Receive buffer full) */
#define SPI0_RXI_IRQn          ((IRQn_Type)17)  /* SPI0 RXI (Receive buffer full) */
#define VECTOR_NUMBER_SPI0_TXI ((IRQn_Type)18)  /* SPI0 TXI (Transmit buffer empty) */
#define SPI0_TXI_IRQn          ((IRQn_Type)18)  /* SPI0 TXI (Transmit buffer empty) */
#define VECTOR_NUMBER_SPI0_TEI ((IRQn_Type)19)  /* SPI0 TEI (Transmission complete event) */
#define SPI0_TEI_IRQn          ((IRQn_Type)19)  /* SPI0 TEI (Transmission complete event) */
#define VECTOR_NUMBER_SPI0_ERI ((IRQn_Type)20)  /* SPI0 ERI (Error) */
#define SPI0_ERI_IRQn          ((IRQn_Type)20)  /* SPI0 ERI (Error) */
#ifdef __cplusplus
}
#endif
#endif /* VECTOR_DATA_H */
