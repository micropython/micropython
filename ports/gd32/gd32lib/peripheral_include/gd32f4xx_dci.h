/*!
    \file    gd32f4xx_dci.h
    \brief   definitions for the DCI

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

#ifndef GD32F4XX_DCI_H
#define GD32F4XX_DCI_H

#include "gd32f4xx.h"

/* DCI definitions */
#define DCI                       DCI_BASE

/* registers definitions */
#define DCI_CTL                   REG32(DCI + 0x00U) /*!< DCI control register */
#define DCI_STAT0                 REG32(DCI + 0x04U) /*!< DCI status register 0 */
#define DCI_STAT1                 REG32(DCI + 0x08U) /*!< DCI status register 1 */
#define DCI_INTEN                 REG32(DCI + 0x0CU) /*!< DCI interrupt enable register */
#define DCI_INTF                  REG32(DCI + 0x10U) /*!< DCI interrupt flag register */
#define DCI_INTC                  REG32(DCI + 0x14U) /*!< DCI interrupt clear register */
#define DCI_SC                    REG32(DCI + 0x18U) /*!< DCI synchronization codes register */
#define DCI_SCUMSK                REG32(DCI + 0x1CU) /*!< DCI synchronization codes unmask register */
#define DCI_CWSPOS                REG32(DCI + 0x20U) /*!< DCI cropping window start position register */
#define DCI_CWSZ                  REG32(DCI + 0x24U) /*!< DCI cropping window size register */
#define DCI_DATA                  REG32(DCI + 0x28U) /*!< DCI data register */

/* bits definitions */
/* DCI_CTL */
#define DCI_CTL_CAP               BIT(0)             /*!< capture enable */
#define DCI_CTL_SNAP              BIT(1)             /*!< snapshot mode */
#define DCI_CTL_WDEN              BIT(2)             /*!< window enable */
#define DCI_CTL_JM                BIT(3)             /*!< JPEG mode */
#define DCI_CTL_ESM               BIT(4)             /*!< embedded synchronous mode */
#define DCI_CTL_CKS               BIT(5)             /*!< clock polarity selection */
#define DCI_CTL_HPS               BIT(6)             /*!< horizontal polarity selection */
#define DCI_CTL_VPS               BIT(7)             /*!< vertical polarity selection */
#define DCI_CTL_FR                BITS(8,9)          /*!< frame rate */
#define DCI_CTL_DCIF              BITS(10,11)        /*!< digital camera interface format */
#define DCI_CTL_DCIEN             BIT(14)            /*!< DCI enable */

/* DCI_STAT0 */
#define DCI_STAT0_HS              BIT(0)            /*!< HS line status */
#define DCI_STAT0_VS              BIT(1)            /*!< VS line status */
#define DCI_STAT0_FV              BIT(2)            /*!< FIFO valid */

/* DCI_STAT1 */
#define DCI_STAT1_EFF             BIT(0)            /*!< end of frame flag */
#define DCI_STAT1_OVRF            BIT(1)            /*!< FIFO overrun flag */
#define DCI_STAT1_ESEF            BIT(2)            /*!< embedded synchronous error flag */
#define DCI_STAT1_VSF             BIT(3)            /*!< vsync flag */
#define DCI_STAT1_ELF             BIT(4)            /*!< end of line flag */

/* DCI_INTEN */
#define DCI_INTEN_EFIE            BIT(0)            /*!< end of frame interrupt enable */
#define DCI_INTEN_OVRIE           BIT(1)            /*!< FIFO overrun interrupt enable */
#define DCI_INTEN_ESEIE           BIT(2)            /*!< embedded synchronous error interrupt enable */
#define DCI_INTEN_VSIE            BIT(3)            /*!< vsync interrupt enable */
#define DCI_INTEN_ELIE            BIT(4)            /*!< end of line interrupt enable */

/* DCI_INTF */
#define DCI_INTF_EFIF             BIT(0)            /*!< end of frame interrupt flag */
#define DCI_INTF_OVRIF            BIT(1)            /*!< FIFO overrun interrupt flag */
#define DCI_INTF_ESEIF            BIT(2)            /*!< embedded synchronous error interrupt flag */
#define DCI_INTF_VSIF             BIT(3)            /*!< vsync interrupt flag  */
#define DCI_INTF_ELIF             BIT(4)            /*!< end of line interrupt flag */

/* DCI_INTC */
#define DCI_INTC_EFFC             BIT(0)            /*!< clear end of frame flag */
#define DCI_INTC_OVRFC            BIT(1)            /*!< clear FIFO overrun flag */
#define DCI_INTC_ESEFC            BIT(2)            /*!< clear embedded synchronous error flag */
#define DCI_INTC_VSFC             BIT(3)            /*!< vsync flag clear */
#define DCI_INTC_ELFC             BIT(4)            /*!< end of line flag clear */

/* DCI_SC */
#define DCI_SC_FS                 BITS(0,7)         /*!< frame start code in embedded synchronous mode */
#define DCI_SC_LS                 BITS(8,15)        /*!< line start code in embedded synchronous mode */
#define DCI_SC_LE                 BITS(16,23)       /*!< line end code in embedded synchronous mode */
#define DCI_SC_FE                 BITS(24,31)       /*!< frame end code in embedded synchronous mode */

/* DCI_SCUNMSK */
#define DCI_SCUMSK_FSM            BITS(0,7)         /*!< frame start code unmask bits in embedded synchronous mode */
#define DCI_SCUMSK_LSM            BITS(8,15)        /*!< line start code unmask bits in embedded synchronous mode */
#define DCI_SCUMSK_LEM            BITS(16,23)       /*!< line end code unmask bits in embedded synchronous mode */
#define DCI_SCUMSK_FEM            BITS(24,31)       /*!< frame end code unmask bits in embedded synchronous mode */

/* DCI_CWSPOS */
#define DCI_CWSPOS_WHSP           BITS(0,13)        /*!< window horizontal start position */
#define DCI_CWSPOS_WVSP           BITS(16,28)       /*!< window vertical start position */

/* DCI_CWSZ */
#define DCI_CWSZ_WHSZ             BITS(0,13)        /*!< window horizontal size */
#define DCI_CWSZ_WVSZ             BITS(16,29)       /*!< window vertical size */

/* constants definitions */
/* DCI parameter structure definitions */
typedef struct
{   
    uint32_t capture_mode;                                           /*!< DCI capture mode: continuous or snapshot */
    uint32_t clock_polarity;                                         /*!< clock polarity selection */
    uint32_t hsync_polarity;                                         /*!< horizontal polarity selection */
    uint32_t vsync_polarity;                                         /*!< vertical polarity selection */
    uint32_t frame_rate;                                             /*!< frame capture rate */
    uint32_t interface_format;                                       /*!< digital camera interface format */
}dci_parameter_struct;                                                         

#define DCI_CAPTURE_MODE_CONTINUOUS   ((uint32_t)0x00000000U)        /*!< continuous capture mode */
#define DCI_CAPTURE_MODE_SNAPSHOT     DCI_CTL_SNAP                   /*!< snapshot capture mode */

#define DCI_CK_POLARITY_FALLING       ((uint32_t)0x00000000U)        /*!< capture at falling edge */
#define DCI_CK_POLARITY_RISING        DCI_CTL_CKS                    /*!< capture at rising edge */

#define DCI_HSYNC_POLARITY_LOW        ((uint32_t)0x00000000U)        /*!< low level during blanking period */
#define DCI_HSYNC_POLARITY_HIGH       DCI_CTL_HPS                    /*!< high level during blanking period */

#define DCI_VSYNC_POLARITY_LOW        ((uint32_t)0x00000000U)        /*!< low level during blanking period */
#define DCI_VSYNC_POLARITY_HIGH       DCI_CTL_VPS                    /*!< high level during blanking period*/
 
#define CTL_FR(regval)                (BITS(8,9)&((uint32_t)(regval) << 8U))    
#define DCI_FRAME_RATE_ALL            CTL_FR(0)                      /*!< capture all frames */
#define DCI_FRAME_RATE_1_2            CTL_FR(1)                      /*!< capture one in 2 frames */
#define DCI_FRAME_RATE_1_4            CTL_FR(2)                      /*!< capture one in 4 frames */

#define CTL_DCIF(regval)              (BITS(10,11)&((uint32_t)(regval) << 10U)) 
#define DCI_INTERFACE_FORMAT_8BITS    CTL_DCIF(0)                    /*!< 8-bit data on every pixel clock */
#define DCI_INTERFACE_FORMAT_10BITS   CTL_DCIF(1)                    /*!< 10-bit data on every pixel clock */
#define DCI_INTERFACE_FORMAT_12BITS   CTL_DCIF(2)                    /*!< 12-bit data on every pixel clock */
#define DCI_INTERFACE_FORMAT_14BITS   CTL_DCIF(3)                    /*!< 14-bit data on every pixel clock */

/* DCI interrupt constants definitions */
#define DCI_INT_EF                    BIT(0)                         /*!< end of frame interrupt */
#define DCI_INT_OVR                   BIT(1)                         /*!< FIFO overrun interrupt */
#define DCI_INT_ESE                   BIT(2)                         /*!< embedded synchronous error interrupt */
#define DCI_INT_VSYNC                 BIT(3)                         /*!< vsync interrupt */
#define DCI_INT_EL                    BIT(4)                         /*!< end of line interrupt */

/* DCI interrupt flag definitions */
#define DCI_INT_FLAG_EF               BIT(0)                         /*!< end of frame interrupt flag */
#define DCI_INT_FLAG_OVR              BIT(1)                         /*!< FIFO overrun interrupt flag */
#define DCI_INT_FLAG_ESE              BIT(2)                         /*!< embedded synchronous error interrupt flag */
#define DCI_INT_FLAG_VSYNC            BIT(3)                         /*!< vsync interrupt flag */
#define DCI_INT_FLAG_EL               BIT(4)                         /*!< end of line interrupt flag */

/* DCI flag definitions */  
#define DCI_FLAG_HS                   DCI_STAT0_HS                   /*!< HS line status */
#define DCI_FLAG_VS                   DCI_STAT0_VS                   /*!< VS line status */
#define DCI_FLAG_FV                   DCI_STAT0_FV                   /*!< FIFO valid */
#define DCI_FLAG_EF                   (DCI_STAT1_EFF | BIT(31))      /*!< end of frame flag */
#define DCI_FLAG_OVR                  (DCI_STAT1_OVRF | BIT(31))     /*!< FIFO overrun flag */
#define DCI_FLAG_ESE                  (DCI_STAT1_ESEF | BIT(31))     /*!< embedded synchronous error flag */
#define DCI_FLAG_VSYNC                (DCI_STAT1_VSF | BIT(31))      /*!< vsync flag */
#define DCI_FLAG_EL                   (DCI_STAT1_ELF | BIT(31))      /*!< end of line flag */

/* function declarations */
/* initialization functions */
/* DCI deinit */
void dci_deinit(void);
/* initialize DCI registers */
void dci_init(dci_parameter_struct* dci_struct);

/* enable DCI function */
void dci_enable(void);
/* disable DCI function */
void dci_disable(void);
/* enable DCI capture */
void dci_capture_enable(void);
/* disable DCI capture */
void dci_capture_disable(void);
/* enable DCI jpeg mode */
void dci_jpeg_enable(void);
/* disable DCI jpeg mode */
void dci_jpeg_disable(void);

/* function configuration */
/* enable cropping window function */
void dci_crop_window_enable(void);
/* disable cropping window function */
void dci_crop_window_disable(void);
/* configure DCI cropping window */
void dci_crop_window_config(uint16_t start_x, uint16_t start_y, uint16_t size_width, uint16_t size_height);

/* enable embedded synchronous mode */
void dci_embedded_sync_enable(void);
/* disable embedded synchronous mode */
void dci_embedded_sync_disable(void);
/* configure synchronous codes in embedded synchronous mode */
void dci_sync_codes_config(uint8_t frame_start, uint8_t line_start, uint8_t line_end, uint8_t frame_end);
/* configure synchronous codes unmask in embedded synchronous mode */
void dci_sync_codes_unmask_config(uint8_t frame_start, uint8_t line_start, uint8_t line_end, uint8_t frame_end);

/* read DCI data register */
uint32_t dci_data_read(void);

/* interrupt & flag functions */
/* get specified flag */
FlagStatus dci_flag_get(uint32_t flag);
/* enable specified DCI interrupt */
void dci_interrupt_enable(uint32_t interrupt);
/* disable specified DCI interrupt */
void dci_interrupt_disable(uint32_t interrupt);


/* get specified interrupt flag */
FlagStatus dci_interrupt_flag_get(uint32_t int_flag);
/* clear specified interrupt flag */
void dci_interrupt_flag_clear(uint32_t int_flag);

#endif /* GD32F4XX_DCI_H */
