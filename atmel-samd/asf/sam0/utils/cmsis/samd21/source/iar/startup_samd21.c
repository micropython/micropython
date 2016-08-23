/**
 * \file
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

typedef void (*intfunc) (void);
typedef union { intfunc __fun; void * __ptr; } intvec_elem;

void __iar_program_start(void);
int __low_level_init(void);

/* Default empty handler */
void Dummy_Handler(void);

/* Cortex-M0+ core handlers */
#pragma weak NMI_Handler              = Dummy_Handler
#pragma weak HardFault_Handler        = Dummy_Handler
#pragma weak SVC_Handler              = Dummy_Handler
#pragma weak PendSV_Handler           = Dummy_Handler
#pragma weak SysTick_Handler          = Dummy_Handler

/* Peripherals handlers */
#pragma weak PM_Handler               = Dummy_Handler
#pragma weak SYSCTRL_Handler          = Dummy_Handler
#pragma weak WDT_Handler              = Dummy_Handler
#pragma weak RTC_Handler              = Dummy_Handler
#pragma weak EIC_Handler              = Dummy_Handler
#pragma weak NVMCTRL_Handler          = Dummy_Handler
#pragma weak DMAC_Handler             = Dummy_Handler
#ifdef       ID_USB
#pragma weak USB_Handler              = Dummy_Handler
#endif
#pragma weak EVSYS_Handler            = Dummy_Handler
#pragma weak SERCOM0_Handler          = Dummy_Handler
#pragma weak SERCOM1_Handler          = Dummy_Handler
#pragma weak SERCOM2_Handler          = Dummy_Handler
#pragma weak SERCOM3_Handler          = Dummy_Handler
#ifdef       ID_SERCOM4
#pragma weak SERCOM4_Handler          = Dummy_Handler
#endif
#ifdef       ID_SERCOM5
#pragma weak SERCOM5_Handler          = Dummy_Handler
#endif
#pragma weak TCC0_Handler             = Dummy_Handler
#pragma weak TCC1_Handler             = Dummy_Handler
#pragma weak TCC2_Handler             = Dummy_Handler
#pragma weak TC3_Handler              = Dummy_Handler
#pragma weak TC4_Handler              = Dummy_Handler
#pragma weak TC5_Handler              = Dummy_Handler
#ifdef       ID_TC6
#pragma weak TC6_Handler              = Dummy_Handler
#endif
#ifdef       ID_TC7
#pragma weak TC7_Handler              = Dummy_Handler
#endif
#ifdef       ID_ADC
#pragma weak ADC_Handler              = Dummy_Handler
#endif
#ifdef       ID_AC
#pragma weak AC_Handler               = Dummy_Handler
#endif
#ifdef       ID_DAC
#pragma weak DAC_Handler              = Dummy_Handler
#endif
#ifdef       ID_PTC
#pragma weak PTC_Handler              = Dummy_Handler
#endif
#ifdef       ID_I2S
#pragma weak I2S_Handler              = Dummy_Handler
#endif
#ifdef       ID_AC1
#pragma weak AC1_Handler              = Dummy_Handler
#endif

/* Exception Table */
#pragma language = extended
#pragma segment  = "CSTACK"

/* The name "__vector_table" has special meaning for C-SPY: */
/* it is where the SP start value is found, and the NVIC vector */
/* table register (VTOR) is initialized to this address if != 0 */

#pragma section  = ".intvec"
#pragma location = ".intvec"
const DeviceVectors __vector_table[] = {
        (void*) __sfe("CSTACK"),
        (void*) Reset_Handler,
        (void*) NMI_Handler,
        (void*) HardFault_Handler,
        (void*) (0UL), /* Reserved */
        (void*) (0UL), /* Reserved */
        (void*) (0UL), /* Reserved */
        (void*) (0UL), /* Reserved */
        (void*) (0UL), /* Reserved */
        (void*) (0UL), /* Reserved */
        (void*) (0UL), /* Reserved */
        (void*) SVC_Handler,
        (void*) (0UL), /* Reserved */
        (void*) (0UL), /* Reserved */
        (void*) PendSV_Handler,
        (void*) SysTick_Handler,

        /* Configurable interrupts */
        (void*) PM_Handler,             /*  0 Power Manager */
        (void*) SYSCTRL_Handler,        /*  1 System Control */
        (void*) WDT_Handler,            /*  2 Watchdog Timer */
        (void*) RTC_Handler,            /*  3 Real-Time Counter */
        (void*) EIC_Handler,            /*  4 External Interrupt Controller */
        (void*) NVMCTRL_Handler,        /*  5 Non-Volatile Memory Controller */
        (void*) DMAC_Handler,           /*  6 Direct Memory Access Controller */
#ifdef ID_USB
        (void*) USB_Handler,            /*  7 Universal Serial Bus */
#else
        (void*) (0UL),                  /*  7 Reserved */
#endif
        (void*) EVSYS_Handler,          /*  8 Event System Interface */
        (void*) SERCOM0_Handler,        /*  9 Serial Communication Interface 0 */
        (void*) SERCOM1_Handler,        /* 10 Serial Communication Interface 1 */
        (void*) SERCOM2_Handler,        /* 11 Serial Communication Interface 2 */
        (void*) SERCOM3_Handler,        /* 12 Serial Communication Interface 3 */
#ifdef ID_SERCOM4
        (void*) SERCOM4_Handler,        /* 13 Serial Communication Interface 4 */
#else
        (void*) (0UL),                  /* 13 Reserved */
#endif
#ifdef ID_SERCOM5
        (void*) SERCOM5_Handler,        /* 14 Serial Communication Interface 5 */
#else
        (void*) (0UL),                  /* 14 Reserved */
#endif
        (void*) TCC0_Handler,           /* 15 Timer Counter Control 0 */
        (void*) TCC1_Handler,           /* 16 Timer Counter Control 1 */
        (void*) TCC2_Handler,           /* 17 Timer Counter Control 2 */
        (void*) TC3_Handler,            /* 18 Basic Timer Counter 0 */
        (void*) TC4_Handler,            /* 19 Basic Timer Counter 1 */
        (void*) TC5_Handler,            /* 20 Basic Timer Counter 2 */
#ifdef ID_TC6
        (void*) TC6_Handler,            /* 21 Basic Timer Counter 3 */
#else
        (void*) (0UL),                  /* 21 Reserved */
#endif
#ifdef ID_TC7
        (void*) TC7_Handler,            /* 22 Basic Timer Counter 4 */
#else
        (void*) (0UL),                  /* 22 Reserved */
#endif
#ifdef ID_ADC
        (void*) ADC_Handler,            /* 23 Analog Digital Converter */
#else
        (void*) (0UL),                  /* 23 Reserved */
#endif
#ifdef ID_AC
        (void*) AC_Handler,             /* 24 Analog Comparators 0 */
#else
        (void*) (0UL),                  /* 24 Reserved */
#endif
#ifdef ID_DAC
        (void*) DAC_Handler,            /* 25 Digital Analog Converter */
#else
        (void*) (0UL),                  /* 25 Reserved */
#endif
#ifdef ID_PTC
        (void*) PTC_Handler,            /* 26 Peripheral Touch Controller */
#else
        (void*) (0UL),                  /* 26 Reserved */
#endif
#ifdef ID_I2S
        (void*) I2S_Handler,            /* 27 Inter-IC Sound Interface */
#else
        (void*) (0UL),                  /* 27 Reserved */
#endif
#ifdef ID_AC1
        (void*) AC1_Handler             /* 28 Analog Comparators 1 */
#else
        (void*) (0UL)                   /* 28 Reserved */
#endif
};

/**------------------------------------------------------------------------------
 * This is the code that gets called on processor reset. To initialize the
 * device.
 *------------------------------------------------------------------------------*/
int __low_level_init(void)
{
        uint32_t *pSrc = __section_begin(".intvec");

        SCB->VTOR = ((uint32_t) pSrc & SCB_VTOR_TBLOFF_Msk);

        return 1; /* if return 0, the data sections will not be initialized */
}

/**------------------------------------------------------------------------------
 * This is the code that gets called on processor reset. To initialize the
 * device.
 *------------------------------------------------------------------------------*/
void Reset_Handler(void)
{
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

        __iar_program_start();
}

/**
 * \brief Default interrupt handler for unused IRQs.
 */
void Dummy_Handler(void)
{
        while (1) {
        }
}
