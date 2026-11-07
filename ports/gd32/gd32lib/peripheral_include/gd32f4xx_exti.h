/*!
    \file    gd32f4xx_exti.h
    \brief   definitions for the EXTI
    \version 2026-02-05, V3.3.3, firmware for GD32F4xx
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#ifndef GD32F4XX_EXTI_H
#define GD32F4XX_EXTI_H

#include "gd32f4xx.h"

/* EXTI definitions */
#define EXTI                         EXTI_BASE

/* registers definitions */
#define EXTI_INTEN                   REG32(EXTI + 0x00U)      /*!< interrupt enable register */
#define EXTI_EVEN                    REG32(EXTI + 0x04U)      /*!< event enable register */
#define EXTI_RTEN                    REG32(EXTI + 0x08U)      /*!< rising edge trigger enable register */
#define EXTI_FTEN                    REG32(EXTI + 0x0CU)      /*!< falling trigger enable register */
#define EXTI_SWIEV                   REG32(EXTI + 0x10U)      /*!< software interrupt event register */
#define EXTI_PD                      REG32(EXTI + 0x14U)      /*!< pending register */

/* bits definitions */
/* EXTI_INTEN */
#define EXTI_INTEN_INTEN0            BIT(0)                   /*!< interrupt from line 0 */
#define EXTI_INTEN_INTEN1            BIT(1)                   /*!< interrupt from line 1 */
#define EXTI_INTEN_INTEN2            BIT(2)                   /*!< interrupt from line 2 */
#define EXTI_INTEN_INTEN3            BIT(3)                   /*!< interrupt from line 3 */
#define EXTI_INTEN_INTEN4            BIT(4)                   /*!< interrupt from line 4 */
#define EXTI_INTEN_INTEN5            BIT(5)                   /*!< interrupt from line 5 */
#define EXTI_INTEN_INTEN6            BIT(6)                   /*!< interrupt from line 6 */
#define EXTI_INTEN_INTEN7            BIT(7)                   /*!< interrupt from line 7 */
#define EXTI_INTEN_INTEN8            BIT(8)                   /*!< interrupt from line 8 */
#define EXTI_INTEN_INTEN9            BIT(9)                   /*!< interrupt from line 9 */
#define EXTI_INTEN_INTEN10           BIT(10)                  /*!< interrupt from line 10 */
#define EXTI_INTEN_INTEN11           BIT(11)                  /*!< interrupt from line 11 */
#define EXTI_INTEN_INTEN12           BIT(12)                  /*!< interrupt from line 12 */
#define EXTI_INTEN_INTEN13           BIT(13)                  /*!< interrupt from line 13 */
#define EXTI_INTEN_INTEN14           BIT(14)                  /*!< interrupt from line 14 */
#define EXTI_INTEN_INTEN15           BIT(15)                  /*!< interrupt from line 15 */
#define EXTI_INTEN_INTEN16           BIT(16)                  /*!< interrupt from line 16 */
#define EXTI_INTEN_INTEN17           BIT(17)                  /*!< interrupt from line 17 */
#define EXTI_INTEN_INTEN18           BIT(18)                  /*!< interrupt from line 18 */
#define EXTI_INTEN_INTEN19           BIT(19)                  /*!< interrupt from line 19 */
#define EXTI_INTEN_INTEN20           BIT(20)                  /*!< interrupt from line 20 */
#define EXTI_INTEN_INTEN21           BIT(21)                  /*!< interrupt from line 21 */
#define EXTI_INTEN_INTEN22           BIT(22)                  /*!< interrupt from line 22 */

/* EXTI_EVEN */
#define EXTI_EVEN_EVEN0              BIT(0)                   /*!< event from line 0 */
#define EXTI_EVEN_EVEN1              BIT(1)                   /*!< event from line 1 */
#define EXTI_EVEN_EVEN2              BIT(2)                   /*!< event from line 2 */
#define EXTI_EVEN_EVEN3              BIT(3)                   /*!< event from line 3 */
#define EXTI_EVEN_EVEN4              BIT(4)                   /*!< event from line 4 */
#define EXTI_EVEN_EVEN5              BIT(5)                   /*!< event from line 5 */
#define EXTI_EVEN_EVEN6              BIT(6)                   /*!< event from line 6 */
#define EXTI_EVEN_EVEN7              BIT(7)                   /*!< event from line 7 */
#define EXTI_EVEN_EVEN8              BIT(8)                   /*!< event from line 8 */
#define EXTI_EVEN_EVEN9              BIT(9)                   /*!< event from line 9 */
#define EXTI_EVEN_EVEN10             BIT(10)                  /*!< event from line 10 */
#define EXTI_EVEN_EVEN11             BIT(11)                  /*!< event from line 11 */
#define EXTI_EVEN_EVEN12             BIT(12)                  /*!< event from line 12 */
#define EXTI_EVEN_EVEN13             BIT(13)                  /*!< event from line 13 */
#define EXTI_EVEN_EVEN14             BIT(14)                  /*!< event from line 14 */
#define EXTI_EVEN_EVEN15             BIT(15)                  /*!< event from line 15 */
#define EXTI_EVEN_EVEN16             BIT(16)                  /*!< event from line 16 */
#define EXTI_EVEN_EVEN17             BIT(17)                  /*!< event from line 17 */
#define EXTI_EVEN_EVEN18             BIT(18)                  /*!< event from line 18 */
#define EXTI_EVEN_EVEN19             BIT(19)                  /*!< event from line 19 */
#define EXTI_EVEN_EVEN20             BIT(20)                  /*!< event from line 20 */
#define EXTI_EVEN_EVEN21             BIT(21)                  /*!< event from line 21 */
#define EXTI_EVEN_EVEN22             BIT(22)                  /*!< event from line 22 */

/* EXTI_RTEN */
#define EXTI_RTEN_RTEN0              BIT(0)                   /*!< rising edge from line 0 */
#define EXTI_RTEN_RTEN1              BIT(1)                   /*!< rising edge from line 1 */
#define EXTI_RTEN_RTEN2              BIT(2)                   /*!< rising edge from line 2 */
#define EXTI_RTEN_RTEN3              BIT(3)                   /*!< rising edge from line 3 */
#define EXTI_RTEN_RTEN4              BIT(4)                   /*!< rising edge from line 4 */
#define EXTI_RTEN_RTEN5              BIT(5)                   /*!< rising edge from line 5 */
#define EXTI_RTEN_RTEN6              BIT(6)                   /*!< rising edge from line 6 */
#define EXTI_RTEN_RTEN7              BIT(7)                   /*!< rising edge from line 7 */
#define EXTI_RTEN_RTEN8              BIT(8)                   /*!< rising edge from line 8 */
#define EXTI_RTEN_RTEN9              BIT(9)                   /*!< rising edge from line 9 */
#define EXTI_RTEN_RTEN10             BIT(10)                  /*!< rising edge from line 10 */
#define EXTI_RTEN_RTEN11             BIT(11)                  /*!< rising edge from line 11 */
#define EXTI_RTEN_RTEN12             BIT(12)                  /*!< rising edge from line 12 */
#define EXTI_RTEN_RTEN13             BIT(13)                  /*!< rising edge from line 13 */
#define EXTI_RTEN_RTEN14             BIT(14)                  /*!< rising edge from line 14 */
#define EXTI_RTEN_RTEN15             BIT(15)                  /*!< rising edge from line 15 */
#define EXTI_RTEN_RTEN16             BIT(16)                  /*!< rising edge from line 16 */
#define EXTI_RTEN_RTEN17             BIT(17)                  /*!< rising edge from line 17 */
#define EXTI_RTEN_RTEN18             BIT(18)                  /*!< rising edge from line 18 */
#define EXTI_RTEN_RTEN19             BIT(19)                  /*!< rising edge from line 19 */
#define EXTI_RTEN_RTEN20             BIT(20)                  /*!< rising edge from line 20 */
#define EXTI_RTEN_RTEN21             BIT(21)                  /*!< rising edge from line 21 */
#define EXTI_RTEN_RTEN22             BIT(22)                  /*!< rising edge from line 22 */

/* EXTI_FTEN */
#define EXTI_FTEN_FTEN0              BIT(0)                   /*!< falling edge from line 0 */
#define EXTI_FTEN_FTEN1              BIT(1)                   /*!< falling edge from line 1 */
#define EXTI_FTEN_FTEN2              BIT(2)                   /*!< falling edge from line 2 */
#define EXTI_FTEN_FTEN3              BIT(3)                   /*!< falling edge from line 3 */
#define EXTI_FTEN_FTEN4              BIT(4)                   /*!< falling edge from line 4 */
#define EXTI_FTEN_FTEN5              BIT(5)                   /*!< falling edge from line 5 */
#define EXTI_FTEN_FTEN6              BIT(6)                   /*!< falling edge from line 6 */
#define EXTI_FTEN_FTEN7              BIT(7)                   /*!< falling edge from line 7 */
#define EXTI_FTEN_FTEN8              BIT(8)                   /*!< falling edge from line 8 */
#define EXTI_FTEN_FTEN9              BIT(9)                   /*!< falling edge from line 9 */
#define EXTI_FTEN_FTEN10             BIT(10)                  /*!< falling edge from line 10 */
#define EXTI_FTEN_FTEN11             BIT(11)                  /*!< falling edge from line 11 */
#define EXTI_FTEN_FTEN12             BIT(12)                  /*!< falling edge from line 12 */
#define EXTI_FTEN_FTEN13             BIT(13)                  /*!< falling edge from line 13 */
#define EXTI_FTEN_FTEN14             BIT(14)                  /*!< falling edge from line 14 */
#define EXTI_FTEN_FTEN15             BIT(15)                  /*!< falling edge from line 15 */
#define EXTI_FTEN_FTEN16             BIT(16)                  /*!< falling edge from line 16 */
#define EXTI_FTEN_FTEN17             BIT(17)                  /*!< falling edge from line 17 */
#define EXTI_FTEN_FTEN18             BIT(18)                  /*!< falling edge from line 18 */
#define EXTI_FTEN_FTEN19             BIT(19)                  /*!< falling edge from line 19 */
#define EXTI_FTEN_FTEN20             BIT(20)                  /*!< falling edge from line 20 */
#define EXTI_FTEN_FTEN21             BIT(21)                  /*!< falling edge from line 21 */
#define EXTI_FTEN_FTEN22             BIT(22)                  /*!< falling edge from line 22 */

/* EXTI_SWIEV */
#define EXTI_SWIEV_SWIEV0            BIT(0)                   /*!< software interrupt/event request from line 0 */
#define EXTI_SWIEV_SWIEV1            BIT(1)                   /*!< software interrupt/event request from line 1 */
#define EXTI_SWIEV_SWIEV2            BIT(2)                   /*!< software interrupt/event request from line 2 */
#define EXTI_SWIEV_SWIEV3            BIT(3)                   /*!< software interrupt/event request from line 3 */
#define EXTI_SWIEV_SWIEV4            BIT(4)                   /*!< software interrupt/event request from line 4 */
#define EXTI_SWIEV_SWIEV5            BIT(5)                   /*!< software interrupt/event request from line 5 */
#define EXTI_SWIEV_SWIEV6            BIT(6)                   /*!< software interrupt/event request from line 6 */
#define EXTI_SWIEV_SWIEV7            BIT(7)                   /*!< software interrupt/event request from line 7 */
#define EXTI_SWIEV_SWIEV8            BIT(8)                   /*!< software interrupt/event request from line 8 */
#define EXTI_SWIEV_SWIEV9            BIT(9)                   /*!< software interrupt/event request from line 9 */
#define EXTI_SWIEV_SWIEV10           BIT(10)                  /*!< software interrupt/event request from line 10 */
#define EXTI_SWIEV_SWIEV11           BIT(11)                  /*!< software interrupt/event request from line 11 */
#define EXTI_SWIEV_SWIEV12           BIT(12)                  /*!< software interrupt/event request from line 12 */
#define EXTI_SWIEV_SWIEV13           BIT(13)                  /*!< software interrupt/event request from line 13 */
#define EXTI_SWIEV_SWIEV14           BIT(14)                  /*!< software interrupt/event request from line 14 */
#define EXTI_SWIEV_SWIEV15           BIT(15)                  /*!< software interrupt/event request from line 15 */
#define EXTI_SWIEV_SWIEV16           BIT(16)                  /*!< software interrupt/event request from line 16 */
#define EXTI_SWIEV_SWIEV17           BIT(17)                  /*!< software interrupt/event request from line 17 */
#define EXTI_SWIEV_SWIEV18           BIT(18)                  /*!< software interrupt/event request from line 18 */
#define EXTI_SWIEV_SWIEV19           BIT(19)                  /*!< software interrupt/event request from line 19 */
#define EXTI_SWIEV_SWIEV20           BIT(20)                  /*!< software interrupt/event request from line 20 */
#define EXTI_SWIEV_SWIEV21           BIT(21)                  /*!< software interrupt/event request from line 21 */
#define EXTI_SWIEV_SWIEV22           BIT(22)                  /*!< software interrupt/event request from line 22 */

/* EXTI_PD */
#define EXTI_PD_PD0                  BIT(0)                   /*!< interrupt pending status from line 0 */
#define EXTI_PD_PD1                  BIT(1)                   /*!< interrupt pending status from line 1 */
#define EXTI_PD_PD2                  BIT(2)                   /*!< interrupt pending status from line 2 */
#define EXTI_PD_PD3                  BIT(3)                   /*!< interrupt pending status from line 3 */
#define EXTI_PD_PD4                  BIT(4)                   /*!< interrupt pending status from line 4 */
#define EXTI_PD_PD5                  BIT(5)                   /*!< interrupt pending status from line 5 */
#define EXTI_PD_PD6                  BIT(6)                   /*!< interrupt pending status from line 6 */
#define EXTI_PD_PD7                  BIT(7)                   /*!< interrupt pending status from line 7 */
#define EXTI_PD_PD8                  BIT(8)                   /*!< interrupt pending status from line 8 */
#define EXTI_PD_PD9                  BIT(9)                   /*!< interrupt pending status from line 9 */
#define EXTI_PD_PD10                 BIT(10)                  /*!< interrupt pending status from line 10 */
#define EXTI_PD_PD11                 BIT(11)                  /*!< interrupt pending status from line 11 */
#define EXTI_PD_PD12                 BIT(12)                  /*!< interrupt pending status from line 12 */
#define EXTI_PD_PD13                 BIT(13)                  /*!< interrupt pending status from line 13 */
#define EXTI_PD_PD14                 BIT(14)                  /*!< interrupt pending status from line 14 */
#define EXTI_PD_PD15                 BIT(15)                  /*!< interrupt pending status from line 15 */
#define EXTI_PD_PD16                 BIT(16)                  /*!< interrupt pending status from line 16 */
#define EXTI_PD_PD17                 BIT(17)                  /*!< interrupt pending status from line 17 */
#define EXTI_PD_PD18                 BIT(18)                  /*!< interrupt pending status from line 18 */
#define EXTI_PD_PD19                 BIT(19)                  /*!< interrupt pending status from line 19 */
#define EXTI_PD_PD20                 BIT(20)                  /*!< interrupt pending status from line 20 */
#define EXTI_PD_PD21                 BIT(21)                  /*!< interrupt pending status from line 21 */
#define EXTI_PD_PD22                 BIT(22)                  /*!< interrupt pending status from line 22 */

/* constants definitions */
/* EXTI line number */
typedef enum { 
    EXTI_0      = BIT(0),                                     /*!< EXTI line 0 */
    EXTI_1      = BIT(1),                                     /*!< EXTI line 1 */
    EXTI_2      = BIT(2),                                     /*!< EXTI line 2 */
    EXTI_3      = BIT(3),                                     /*!< EXTI line 3 */
    EXTI_4      = BIT(4),                                     /*!< EXTI line 4 */
    EXTI_5      = BIT(5),                                     /*!< EXTI line 5 */
    EXTI_6      = BIT(6),                                     /*!< EXTI line 6 */
    EXTI_7      = BIT(7),                                     /*!< EXTI line 7 */
    EXTI_8      = BIT(8),                                     /*!< EXTI line 8 */
    EXTI_9      = BIT(9),                                     /*!< EXTI line 9 */
    EXTI_10     = BIT(10),                                    /*!< EXTI line 10 */
    EXTI_11     = BIT(11),                                    /*!< EXTI line 11 */
    EXTI_12     = BIT(12),                                    /*!< EXTI line 12 */
    EXTI_13     = BIT(13),                                    /*!< EXTI line 13 */
    EXTI_14     = BIT(14),                                    /*!< EXTI line 14 */
    EXTI_15     = BIT(15),                                    /*!< EXTI line 15 */
    EXTI_16     = BIT(16),                                    /*!< EXTI line 16 */
    EXTI_17     = BIT(17),                                    /*!< EXTI line 17 */
    EXTI_18     = BIT(18),                                    /*!< EXTI line 18 */
    EXTI_19     = BIT(19),                                    /*!< EXTI line 19 */
    EXTI_20     = BIT(20),                                    /*!< EXTI line 20 */    
    EXTI_21     = BIT(21),                                    /*!< EXTI line 21 */
    EXTI_22     = BIT(22)                                     /*!< EXTI line 22 */
} exti_line_enum;

/* external interrupt and event  */
typedef enum {
    EXTI_INTERRUPT   = 0,                                     /*!< EXTI interrupt mode */
    EXTI_EVENT                                                /*!< EXTI event mode */
} exti_mode_enum;

/* interrupt trigger mode */
typedef enum { 
    EXTI_TRIG_RISING = 0,                                     /*!< EXTI rising edge trigger */
    EXTI_TRIG_FALLING,                                        /*!< EXTI falling edge trigger */
    EXTI_TRIG_BOTH,                                           /*!< EXTI rising and falling edge trigger */
    EXTI_TRIG_NONE                                            /*!< none EXTI edge trigger */
} exti_trig_type_enum;

/* function declarations */
/* deinitialize the EXTI */
void exti_deinit(void);
/* initialize the EXTI line x */
void exti_init(exti_line_enum linex, exti_mode_enum mode, exti_trig_type_enum trig_type);
/* enable the interrupts from EXTI line x */
void exti_interrupt_enable(exti_line_enum linex);
/* disable the interrupts from EXTI line x */
void exti_interrupt_disable(exti_line_enum linex);
/* enable the events from EXTI line x */
void exti_event_enable(exti_line_enum linex);
/* disable the events from EXTI line x */
void exti_event_disable(exti_line_enum linex);
/* enable the software interrupt event from EXTI line x */
void exti_software_interrupt_enable(exti_line_enum linex);
/* disable the software interrupt event from EXTI line x */
void exti_software_interrupt_disable(exti_line_enum linex);

/* interrupt & flag functions */
/* get EXTI line x interrupt pending flag */
FlagStatus exti_flag_get(exti_line_enum linex);
/* clear EXTI line x interrupt pending flag */
void exti_flag_clear(exti_line_enum linex);
/* get EXTI line x interrupt pending flag */
FlagStatus exti_interrupt_flag_get(exti_line_enum linex);
/* clear EXTI line x interrupt pending flag */
void exti_interrupt_flag_clear(exti_line_enum linex);

#endif /* GD32F4XX_EXTI_H */
