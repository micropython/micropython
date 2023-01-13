/* generated vector header file - do not edit */
#ifndef VECTOR_DATA_H
#define VECTOR_DATA_H
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
#define VECTOR_NUMBER_SCI0_TXI ((IRQn_Type)1)  /* SCI0 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI0_TEI ((IRQn_Type)2)  /* SCI0 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI0_ERI ((IRQn_Type)3)  /* SCI0 ERI (Receive error) */
#define VECTOR_NUMBER_SCI1_RXI ((IRQn_Type)4)  /* SCI1 RXI (Received data full) */
#define VECTOR_NUMBER_SCI1_TXI ((IRQn_Type)5)  /* SCI1 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI1_TEI ((IRQn_Type)6)  /* SCI1 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI1_ERI ((IRQn_Type)7)  /* SCI1 ERI (Receive error) */
#define VECTOR_NUMBER_SCI2_RXI ((IRQn_Type)8)  /* SCI2 RXI (Received data full) */
#define VECTOR_NUMBER_SCI2_TXI ((IRQn_Type)9)  /* SCI2 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI2_TEI ((IRQn_Type)10)  /* SCI2 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI2_ERI ((IRQn_Type)11)  /* SCI2 ERI (Receive error) */
#define VECTOR_NUMBER_RTC_ALARM ((IRQn_Type)12)  /* RTC ALARM (Alarm interrupt) */
#define VECTOR_NUMBER_RTC_PERIOD ((IRQn_Type)13)  /* RTC PERIOD (Periodic interrupt) */
#define VECTOR_NUMBER_RTC_CARRY ((IRQn_Type)14)  /* RTC CARRY (Carry interrupt) */
#define VECTOR_NUMBER_AGT0_INT ((IRQn_Type)15)  /* AGT0 INT (AGT interrupt) */
#define VECTOR_NUMBER_ICU_IRQ0 ((IRQn_Type)16)  /* ICU IRQ0 (External pin interrupt 0) */
#define VECTOR_NUMBER_SPI0_RXI ((IRQn_Type)17)  /* SPI0 RXI (Receive buffer full) */
#define VECTOR_NUMBER_SPI0_TXI ((IRQn_Type)18)  /* SPI0 TXI (Transmit buffer empty) */
#define VECTOR_NUMBER_SPI0_TEI ((IRQn_Type)19)  /* SPI0 TEI (Transmission complete event) */
#define VECTOR_NUMBER_SPI0_ERI ((IRQn_Type)20)  /* SPI0 ERI (Error) */
typedef enum IRQn
{
    Reset_IRQn = -15,
    NonMaskableInt_IRQn = -14,
    HardFault_IRQn = -13,
    MemoryManagement_IRQn = -12,
    BusFault_IRQn = -11,
    UsageFault_IRQn = -10,
    SecureFault_IRQn = -9,
    SVCall_IRQn = -5,
    DebugMonitor_IRQn = -4,
    PendSV_IRQn = -2,
    SysTick_IRQn = -1,
    SCI0_RXI_IRQn = 0, /* SCI0 RXI (Receive data full) */
    SCI0_TXI_IRQn = 1, /* SCI0 TXI (Transmit data empty) */
    SCI0_TEI_IRQn = 2, /* SCI0 TEI (Transmit end) */
    SCI0_ERI_IRQn = 3, /* SCI0 ERI (Receive error) */
    SCI1_RXI_IRQn = 4, /* SCI1 RXI (Received data full) */
    SCI1_TXI_IRQn = 5, /* SCI1 TXI (Transmit data empty) */
    SCI1_TEI_IRQn = 6, /* SCI1 TEI (Transmit end) */
    SCI1_ERI_IRQn = 7, /* SCI1 ERI (Receive error) */
    SCI2_RXI_IRQn = 8, /* SCI2 RXI (Received data full) */
    SCI2_TXI_IRQn = 9, /* SCI2 TXI (Transmit data empty) */
    SCI2_TEI_IRQn = 10, /* SCI2 TEI (Transmit end) */
    SCI2_ERI_IRQn = 11, /* SCI2 ERI (Receive error) */
    RTC_ALARM_IRQn = 12, /* RTC ALARM (Alarm interrupt) */
    RTC_PERIOD_IRQn = 13, /* RTC PERIOD (Periodic interrupt) */
    RTC_CARRY_IRQn = 14, /* RTC CARRY (Carry interrupt) */
    AGT0_INT_IRQn = 15, /* AGT0 INT (AGT interrupt) */
    ICU_IRQ0_IRQn = 16, /* ICU IRQ0 (External pin interrupt 0) */
    SPI0_RXI_IRQn = 17, /* SPI0 RXI (Receive buffer full) */
    SPI0_TXI_IRQn = 18, /* SPI0 TXI (Transmit buffer empty) */
    SPI0_TEI_IRQn = 19, /* SPI0 TEI (Transmission complete event) */
    SPI0_ERI_IRQn = 20, /* SPI0 ERI (Error) */
} IRQn_Type;
#endif /* VECTOR_DATA_H */
