/**
 * \file
 *
 * \brief gcc starttup file for SAMD21
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include "samd21.h"

/* Initialize segments */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;

/** \cond DOXYGEN_SHOULD_SKIP_THIS */
int main(void);
/** \endcond */

void __libc_init_array(void);

/* Default empty handler */
void Dummy_Handler(void);

/* Cortex-M0+ core handlers */
void NMI_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void HardFault_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SVC_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PendSV_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SysTick_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));

/* Peripherals handlers */
void PM_Handler              ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SYSCTRL_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void WDT_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void RTC_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void EIC_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void NVMCTRL_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void DMAC_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#ifdef ID_USB
void USB_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#endif
void EVSYS_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SERCOM0_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SERCOM1_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SERCOM2_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SERCOM3_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#ifdef ID_SERCOM4
void SERCOM4_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#endif
#ifdef ID_SERCOM5
void SERCOM5_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#endif
void TCC0_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TCC1_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TCC2_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC3_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC4_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC5_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#ifdef ID_TC6
void TC6_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#endif
#ifdef ID_TC7
void TC7_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#endif
#ifdef ID_ADC
void ADC_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#endif
#ifdef ID_AC
void AC_Handler              ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#endif
#ifdef ID_DAC
void DAC_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#endif
#ifdef ID_PTC
void PTC_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#endif
#ifdef ID_I2S
void I2S_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#endif
#ifdef ID_AC1
void AC1_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#endif

/* Exception Table */
__attribute__ ((section(".vectors")))
const DeviceVectors exception_table = {

        /* Configure Initial Stack Pointer, using linker-generated symbols */
        .pvStack                = (void*) (&_estack),

        .pfnReset_Handler       = (void*) Reset_Handler,
        .pfnNMI_Handler         = (void*) NMI_Handler,
        .pfnHardFault_Handler   = (void*) HardFault_Handler,
        .pvReservedM12          = (void*) (0UL), /* Reserved */
        .pvReservedM11          = (void*) (0UL), /* Reserved */
        .pvReservedM10          = (void*) (0UL), /* Reserved */
        .pvReservedM9           = (void*) (0UL), /* Reserved */
        .pvReservedM8           = (void*) (0UL), /* Reserved */
        .pvReservedM7           = (void*) (0UL), /* Reserved */
        .pvReservedM6           = (void*) (0UL), /* Reserved */
        .pfnSVC_Handler         = (void*) SVC_Handler,
        .pvReservedM4           = (void*) (0UL), /* Reserved */
        .pvReservedM3           = (void*) (0UL), /* Reserved */
        .pfnPendSV_Handler      = (void*) PendSV_Handler,
        .pfnSysTick_Handler     = (void*) SysTick_Handler,

        /* Configurable interrupts */
        .pfnPM_Handler          = (void*) PM_Handler,             /*  0 Power Manager */
        .pfnSYSCTRL_Handler     = (void*) SYSCTRL_Handler,        /*  1 System Control */
        .pfnWDT_Handler         = (void*) WDT_Handler,            /*  2 Watchdog Timer */
        .pfnRTC_Handler         = (void*) RTC_Handler,            /*  3 Real-Time Counter */
        .pfnEIC_Handler         = (void*) EIC_Handler,            /*  4 External Interrupt Controller */
        .pfnNVMCTRL_Handler     = (void*) NVMCTRL_Handler,        /*  5 Non-Volatile Memory Controller */
        .pfnDMAC_Handler        = (void*) DMAC_Handler,           /*  6 Direct Memory Access Controller */
#ifdef ID_USB
        .pfnUSB_Handler         = (void*) USB_Handler,            /*  7 Universal Serial Bus */
#else
        .pvReserved7            = (void*) (0UL),                  /*  7 Reserved */
#endif
        .pfnEVSYS_Handler       = (void*) EVSYS_Handler,          /*  8 Event System Interface */
        .pfnSERCOM0_Handler     = (void*) SERCOM0_Handler,        /*  9 Serial Communication Interface 0 */
        .pfnSERCOM1_Handler     = (void*) SERCOM1_Handler,        /* 10 Serial Communication Interface 1 */
        .pfnSERCOM2_Handler     = (void*) SERCOM2_Handler,        /* 11 Serial Communication Interface 2 */
        .pfnSERCOM3_Handler     = (void*) SERCOM3_Handler,        /* 12 Serial Communication Interface 3 */
#ifdef ID_SERCOM4
        .pfnSERCOM4_Handler     = (void*) SERCOM4_Handler,        /* 13 Serial Communication Interface 4 */
#else
        .pvReserved13           = (void*) (0UL),                  /* 13 Reserved */
#endif
#ifdef ID_SERCOM5
        .pfnSERCOM5_Handler     = (void*) SERCOM5_Handler,        /* 14 Serial Communication Interface 5 */
#else
        .pvReserved14           = (void*) (0UL),                  /* 14 Reserved */
#endif
        .pfnTCC0_Handler        = (void*) TCC0_Handler,           /* 15 Timer Counter Control 0 */
        .pfnTCC1_Handler        = (void*) TCC1_Handler,           /* 16 Timer Counter Control 1 */
        .pfnTCC2_Handler        = (void*) TCC2_Handler,           /* 17 Timer Counter Control 2 */
        .pfnTC3_Handler         = (void*) TC3_Handler,            /* 18 Basic Timer Counter 0 */
        .pfnTC4_Handler         = (void*) TC4_Handler,            /* 19 Basic Timer Counter 1 */
        .pfnTC5_Handler         = (void*) TC5_Handler,            /* 20 Basic Timer Counter 2 */
#ifdef ID_TC6
        .pfnTC6_Handler         = (void*) TC6_Handler,            /* 21 Basic Timer Counter 3 */
#else
        .pvReserved21           = (void*) (0UL),                  /* 21 Reserved */
#endif
#ifdef ID_TC7
        .pfnTC7_Handler         = (void*) TC7_Handler,            /* 22 Basic Timer Counter 4 */
#else
        .pvReserved22           = (void*) (0UL),                  /* 22 Reserved */
#endif
#ifdef ID_ADC
        .pfnADC_Handler         = (void*) ADC_Handler,            /* 23 Analog Digital Converter */
#else
        .pvReserved23           = (void*) (0UL),                  /* 23 Reserved */
#endif
#ifdef ID_AC
        .pfnAC_Handler          = (void*) AC_Handler,             /* 24 Analog Comparators 0 */
#else
        .pvReserved24           = (void*) (0UL),                  /* 24 Reserved */
#endif
#ifdef ID_DAC
        .pfnDAC_Handler         = (void*) DAC_Handler,            /* 25 Digital Analog Converter */
#else
        .pvReserved25           = (void*) (0UL),                  /* 25 Reserved */
#endif
#ifdef ID_PTC
        .pfnPTC_Handler         = (void*) PTC_Handler,            /* 26 Peripheral Touch Controller */
#else
        .pvReserved26           = (void*) (0UL),                  /* 26 Reserved */
#endif
#ifdef ID_I2S
        .pfnI2S_Handler         = (void*) I2S_Handler,            /* 27 Inter-IC Sound Interface */
#else
        .pvReserved27           = (void*) (0UL),                  /* 27 Reserved */
#endif
#ifdef ID_AC1
        .pfnAC1_Handler         = (void*) AC1_Handler             /* 28 Analog Comparators 1 */
#else
        .pvReserved28           = (void*) (0UL)                   /* 28 Reserved */
#endif
};

/**
 * \brief This is the code that gets called on processor reset.
 * To initialize the device, and call the main() routine.
 */
void Reset_Handler(void)
{
        uint32_t *pSrc, *pDest;

        /* Initialize the relocate segment */
        pSrc = &_etext;
        pDest = &_srelocate;

        if (pSrc != pDest) {
                for (; pDest < &_erelocate;) {
                        *pDest++ = *pSrc++;
                }
        }

        /* Clear the zero segment */
        for (pDest = &_szero; pDest < &_ezero;) {
                *pDest++ = 0;
        }

        /* Set the vector table base address */
        pSrc = (uint32_t *) & _sfixed;
        SCB->VTOR = ((uint32_t) pSrc & SCB_VTOR_TBLOFF_Msk);

        /* Change default QOS values to have the best performance and correct USB behaviour */
        SBMATRIX->SFR[SBMATRIX_SLAVE_HMCRAMC0].reg = 2;
#if defined(ID_USB)
        USB->DEVICE.QOSCTRL.bit.CQOS = 2;
        USB->DEVICE.QOSCTRL.bit.DQOS = 2;
#endif
        DMAC->QOSCTRL.bit.DQOS = 2;
        DMAC->QOSCTRL.bit.FQOS = 2;
        DMAC->QOSCTRL.bit.WRBQOS = 2;

        /* Overwriting the default value of the NVMCTRL.CTRLB.MANW bit (errata reference 13134) */
        NVMCTRL->CTRLB.bit.MANW = 1;

        /* Initialize the C library */
        __libc_init_array();

        /* Branch to main function */
        main();

        /* Infinite loop */
        while (1);
}

/**
 * \brief Default interrupt handler for unused IRQs.
 */
void Dummy_Handler(void)
{
        while (1) {
        }
}
