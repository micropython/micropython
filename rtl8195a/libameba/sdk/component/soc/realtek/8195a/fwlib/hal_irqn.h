/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _HAL_IRQN_H_
#define _HAL_IRQN_H_

#define PERIPHERAL_IRQ_BASE_NUM     64

typedef enum _IRQn_Type_ {
#if 0
/******  Cortex-M3 Processor Exceptions Numbers ********/  
    NON_MASKABLE_INT_IRQ            = -14,
    HARD_FAULT_IRQ                  = -13,
    MEM_MANAGE_FAULT_IRQ            = -12,
    BUS_FAULT_IRQ                   = -11,
    USAGE_FAULT_IRQ                 = -10,
    SVCALL_IRQ                      = -5,
    DEBUG_MONITOR_IRQ               = -4,
    PENDSVC_IRQ                     = -2,
    SYSTICK_IRQ                     = -1,
#else    
/******  Cortex-M3 Processor Exceptions Numbers ********/  
    NonMaskableInt_IRQn             = -14,      /*!< 2 Non Maskable Interrupt                         */
    HardFault_IRQn                  = -13,              /*!<   3  Hard Fault, all classes of Fault                                 */
    MemoryManagement_IRQn           = -12,      /*!< 4 Cortex-M3 Memory Management Interrupt          */
    BusFault_IRQn                   = -11,      /*!< 5 Cortex-M3 Bus Fault Interrupt                  */
    UsageFault_IRQn                 = -10,      /*!< 6 Cortex-M3 Usage Fault Interrupt                */
    SVCall_IRQn                     = -5,       /*!< 11 Cortex-M3 SV Call Interrupt                   */
    DebugMonitor_IRQn               = -4,       /*!< 12 Cortex-M3 Debug Monitor Interrupt             */
    PendSV_IRQn                     = -2,       /*!< 14 Cortex-M3 Pend SV Interrupt                   */
    SysTick_IRQn                    = -1,       /*!< 15 Cortex-M3 System Tick Interrupt               */
#endif
/******  RTL8195A Specific Interrupt Numbers ************/
    SYSTEM_ON_IRQ                   =  0,
    WDG_IRQ                         =  1,
    TIMER0_IRQ                      =  2,
    TIMER1_IRQ                      =  3,
    I2C3_IRQ                        =  4,
    TIMER2_7_IRQ                    =  5,
    SPI0_IRQ                        =  6,
    GPIO_IRQ                        =  7,
    UART0_IRQ                       =  8,
    SPI_FLASH_IRQ                   =  9,
    USB_OTG_IRQ                     =  10,
    SDIO_HOST_IRQ                   =  11,
    SDIO_DEVICE_IRQ                 =  12,
    I2S0_PCM0_IRQ                   =  13,
    I2S1_PCM1_IRQ                   =  14,
    WL_DMA_IRQ                      =  15,
    WL_PROTOCOL_IRQ                 =  16,
    CRYPTO_IRQ                      =  17,
    GMAC_IRQ						=  18,
    PERIPHERAL_IRQ                  =  19,
    GDMA0_CHANNEL0_IRQ              =  20,
    GDMA0_CHANNEL1_IRQ              =  21,
    GDMA0_CHANNEL2_IRQ              =  22,
    GDMA0_CHANNEL3_IRQ              =  23,
    GDMA0_CHANNEL4_IRQ              =  24,
    GDMA0_CHANNEL5_IRQ              =  25,
    GDMA1_CHANNEL0_IRQ              =  26,
    GDMA1_CHANNEL1_IRQ              =  27,
    GDMA1_CHANNEL2_IRQ              =  28,
    GDMA1_CHANNEL3_IRQ              =  29,
    GDMA1_CHANNEL4_IRQ              =  30,
    GDMA1_CHANNEL5_IRQ              =  31,

/******  RTL8195A Peripheral Interrupt Numbers ************/
    I2C0_IRQ                        =  64,// 0 + 64,
    I2C1_IRQ                        =  65,// 1 + 64,
    I2C2_IRQ                        =  66,// 2 + 64,
    SPI1_IRQ                        =  72,// 8 + 64,
    SPI2_IRQ                        =  73,// 9 + 64,
    UART1_IRQ                       =  80,// 16 + 64,
    UART2_IRQ                       =  81,// 17 + 64,
    UART_LOG_IRQ                    =  88,// 24 + 64,
    ADC_IRQ                         =  89,// 25 + 64,
    DAC0_IRQ                        =  91,// 27 + 64,
    DAC1_IRQ                        =  92,// 28 + 64,
    //RXI300_IRQ                      =  93// 29 + 64
    LP_EXTENSION_IRQ                =  93,// 29+64
    
    PTA_TRX_IRQ                     =  95,// 31+64
    RXI300_IRQ                      =  96,// 0+32 + 64
    NFC_IRQ                         =  97// 1+32+64
} IRQn_Type, *PIRQn_Type;


typedef VOID (*HAL_VECTOR_FUN) (VOID);

typedef enum _VECTOR_TABLE_TYPE_{
    DEDECATED_VECTRO_TABLE,
    PERIPHERAL_VECTOR_TABLE
}VECTOR_TABLE_TYPE, *PVECTOR_TABLE_TYPE;


typedef u32 (*IRQ_FUN)(VOID *Data);

typedef struct _IRQ_HANDLE_ {
    IRQ_FUN     IrqFun;
    IRQn_Type   IrqNum;
    u32         Data;
    u32         Priority;
}IRQ_HANDLE, *PIRQ_HANDLE;


#endif //_HAL_IRQN_H_
