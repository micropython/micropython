/* generated vector header file - do not edit */
#ifndef VECTOR_DATA_H
#define VECTOR_DATA_H
/* Number of interrupts allocated */
#ifndef VECTOR_DATA_IRQ_COUNT
#define VECTOR_DATA_IRQ_COUNT    (39)
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
#define VECTOR_NUMBER_SCI0_RXI ((IRQn_Type)0)  /* SCI0 RXI (Receive data full) */
#define VECTOR_NUMBER_SCI0_TXI ((IRQn_Type)1)  /* SCI0 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI0_TEI ((IRQn_Type)2)  /* SCI0 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI0_ERI ((IRQn_Type)3)  /* SCI0 ERI (Receive error) */
#define VECTOR_NUMBER_SCI2_RXI ((IRQn_Type)4)  /* SCI2 RXI (Received data full) */
#define VECTOR_NUMBER_SCI2_TXI ((IRQn_Type)5)  /* SCI2 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI2_TEI ((IRQn_Type)6)  /* SCI2 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI2_ERI ((IRQn_Type)7)  /* SCI2 ERI (Receive error) */
#define VECTOR_NUMBER_SCI8_RXI ((IRQn_Type)8)  /* SCI8 RXI (Received data full) */
#define VECTOR_NUMBER_SCI8_TXI ((IRQn_Type)9)  /* SCI8 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI8_TEI ((IRQn_Type)10)  /* SCI8 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI8_ERI ((IRQn_Type)11)  /* SCI8 ERI (Receive error) */
#define VECTOR_NUMBER_RTC_ALARM ((IRQn_Type)12)  /* RTC ALARM (Alarm interrupt) */
#define VECTOR_NUMBER_RTC_PERIOD ((IRQn_Type)13)  /* RTC PERIOD (Periodic interrupt) */
#define VECTOR_NUMBER_RTC_CARRY ((IRQn_Type)14)  /* RTC CARRY (Carry interrupt) */
#define VECTOR_NUMBER_AGT0_INT ((IRQn_Type)15)  /* AGT0 INT (AGT interrupt) */
#define VECTOR_NUMBER_AGT1_INT ((IRQn_Type)16)  /* AGT1 INT (AGT interrupt) */
#define VECTOR_NUMBER_ICU_IRQ0 ((IRQn_Type)17)  /* ICU IRQ0 (External pin interrupt 0) */
#define VECTOR_NUMBER_ICU_IRQ1 ((IRQn_Type)18)  /* ICU IRQ1 (External pin interrupt 1) */
#define VECTOR_NUMBER_ICU_IRQ2 ((IRQn_Type)19)  /* ICU IRQ2 (External pin interrupt 2) */
#define VECTOR_NUMBER_ICU_IRQ3 ((IRQn_Type)20)  /* ICU IRQ3 (External pin interrupt 3) */
#define VECTOR_NUMBER_ICU_IRQ4 ((IRQn_Type)21)  /* ICU IRQ4 (External pin interrupt 4) */
#define VECTOR_NUMBER_ICU_IRQ5 ((IRQn_Type)22)  /* ICU IRQ5 (External pin interrupt 5) */
#define VECTOR_NUMBER_ICU_IRQ6 ((IRQn_Type)23)  /* ICU IRQ6 (External pin interrupt 6) */
#define VECTOR_NUMBER_ICU_IRQ7 ((IRQn_Type)24)  /* ICU IRQ7 (External pin interrupt 7) */
#define VECTOR_NUMBER_ICU_IRQ8 ((IRQn_Type)25)  /* ICU IRQ8 (External pin interrupt 8) */
#define VECTOR_NUMBER_ICU_IRQ9 ((IRQn_Type)26)  /* ICU IRQ9 (External pin interrupt 9) */
#define VECTOR_NUMBER_ICU_IRQ10 ((IRQn_Type)27)  /* ICU IRQ10 (External pin interrupt 10) */
#define VECTOR_NUMBER_ICU_IRQ11 ((IRQn_Type)28)  /* ICU IRQ11 (External pin interrupt 11) */
#define VECTOR_NUMBER_ICU_IRQ12 ((IRQn_Type)29)  /* ICU IRQ12 (External pin interrupt 12) */
#define VECTOR_NUMBER_ICU_IRQ13 ((IRQn_Type)30)  /* ICU IRQ13 (External pin interrupt 13) */
#define VECTOR_NUMBER_SPI0_RXI ((IRQn_Type)31)  /* SPI0 RXI (Receive buffer full) */
#define VECTOR_NUMBER_SPI0_TXI ((IRQn_Type)32)  /* SPI0 TXI (Transmit buffer empty) */
#define VECTOR_NUMBER_SPI0_TEI ((IRQn_Type)33)  /* SPI0 TEI (Transmission complete event) */
#define VECTOR_NUMBER_SPI0_ERI ((IRQn_Type)34)  /* SPI0 ERI (Error) */
#define VECTOR_NUMBER_IIC0_RXI ((IRQn_Type)35)  /* IIC0 RXI (Receive data full) */
#define VECTOR_NUMBER_IIC0_TXI ((IRQn_Type)36)  /* IIC0 TXI (Transmit data empty) */
#define VECTOR_NUMBER_IIC0_TEI ((IRQn_Type)37)  /* IIC0 TEI (Transmit end) */
#define VECTOR_NUMBER_IIC0_ERI ((IRQn_Type)38)  /* IIC0 ERI (Transfer error) */
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
    SCI2_RXI_IRQn = 4, /* SCI2 RXI (Received data full) */
    SCI2_TXI_IRQn = 5, /* SCI2 TXI (Transmit data empty) */
    SCI2_TEI_IRQn = 6, /* SCI2 TEI (Transmit end) */
    SCI2_ERI_IRQn = 7, /* SCI2 ERI (Receive error) */
    SCI8_RXI_IRQn = 8, /* SCI8 RXI (Received data full) */
    SCI8_TXI_IRQn = 9, /* SCI8 TXI (Transmit data empty) */
    SCI8_TEI_IRQn = 10, /* SCI8 TEI (Transmit end) */
    SCI8_ERI_IRQn = 11, /* SCI8 ERI (Receive error) */
    RTC_ALARM_IRQn = 12, /* RTC ALARM (Alarm interrupt) */
    RTC_PERIOD_IRQn = 13, /* RTC PERIOD (Periodic interrupt) */
    RTC_CARRY_IRQn = 14, /* RTC CARRY (Carry interrupt) */
    AGT0_INT_IRQn = 15, /* AGT0 INT (AGT interrupt) */
    AGT1_INT_IRQn = 16, /* AGT1 INT (AGT interrupt) */
    ICU_IRQ0_IRQn = 17, /* ICU IRQ0 (External pin interrupt 0) */
    ICU_IRQ1_IRQn = 18, /* ICU IRQ1 (External pin interrupt 1) */
    ICU_IRQ2_IRQn = 19, /* ICU IRQ2 (External pin interrupt 2) */
    ICU_IRQ3_IRQn = 20, /* ICU IRQ3 (External pin interrupt 3) */
    ICU_IRQ4_IRQn = 21, /* ICU IRQ4 (External pin interrupt 4) */
    ICU_IRQ5_IRQn = 22, /* ICU IRQ5 (External pin interrupt 5) */
    ICU_IRQ6_IRQn = 23, /* ICU IRQ6 (External pin interrupt 6) */
    ICU_IRQ7_IRQn = 24, /* ICU IRQ7 (External pin interrupt 7) */
    ICU_IRQ8_IRQn = 25, /* ICU IRQ8 (External pin interrupt 8) */
    ICU_IRQ9_IRQn = 26, /* ICU IRQ9 (External pin interrupt 9) */
    ICU_IRQ10_IRQn = 27, /* ICU IRQ10 (External pin interrupt 10) */
    ICU_IRQ11_IRQn = 28, /* ICU IRQ11 (External pin interrupt 11) */
    ICU_IRQ12_IRQn = 29, /* ICU IRQ12 (External pin interrupt 12) */
    ICU_IRQ13_IRQn = 30, /* ICU IRQ13 (External pin interrupt 13) */
    SPI0_RXI_IRQn = 31, /* SPI0 RXI (Receive buffer full) */
    SPI0_TXI_IRQn = 32, /* SPI0 TXI (Transmit buffer empty) */
    SPI0_TEI_IRQn = 33, /* SPI0 TEI (Transmission complete event) */
    SPI0_ERI_IRQn = 34, /* SPI0 ERI (Error) */
    IIC0_RXI_IRQn = 35, /* IIC0 RXI (Receive data full) */
    IIC0_TXI_IRQn = 36, /* IIC0 TXI (Transmit data empty) */
    IIC0_TEI_IRQn = 37, /* IIC0 TEI (Transmit end) */
    IIC0_ERI_IRQn = 38, /* IIC0 ERI (Transfer error) */
} IRQn_Type;
#endif /* VECTOR_DATA_H */
