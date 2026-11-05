/*!
    \file    gd32f4xx_dci.c
    \brief   DCI driver

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

#include "gd32f4xx_dci.h"

/*!
    \brief    DCI deinit
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dci_deinit(void)
{
    rcu_periph_reset_enable(RCU_DCIRST);
    rcu_periph_reset_disable(RCU_DCIRST);
}

/*!
    \brief    initialize DCI registers
    \param[in]  dci_struct: DCI parameter initialization structure
                members of the structure and the member values are shown as below:
                capture_mode    : DCI_CAPTURE_MODE_CONTINUOUS, DCI_CAPTURE_MODE_SNAPSHOT
                colck_polarity  : DCI_CK_POLARITY_FALLING, DCI_CK_POLARITY_RISING
                hsync_polarity  : DCI_HSYNC_POLARITY_LOW, DCI_HSYNC_POLARITY_HIGH
                vsync_polarity  : DCI_VSYNC_POLARITY_LOW, DCI_VSYNC_POLARITY_HIGH
                frame_rate      : DCI_FRAME_RATE_ALL, DCI_FRAME_RATE_1_2, DCI_FRAME_RATE_1_4
                interface_format: DCI_INTERFACE_FORMAT_8BITS, DCI_INTERFACE_FORMAT_10BITS,
                                      DCI_INTERFACE_FORMAT_12BITS, DCI_INTERFACE_FORMAT_14BITS
    \param[out] none
    \retval     none
*/
void dci_init(dci_parameter_struct *dci_struct)
{
    uint32_t reg = 0U;
    /* disable capture function and DCI */
    DCI_CTL &= ~(DCI_CTL_CAP | DCI_CTL_DCIEN);
    /* configure DCI parameter */
    reg |= dci_struct->capture_mode;
    reg |= dci_struct->clock_polarity;
    reg |= dci_struct->hsync_polarity;
    reg |= dci_struct->vsync_polarity;
    reg |= dci_struct->frame_rate;
    reg |= dci_struct->interface_format;

    DCI_CTL = reg;
}

/*!
    \brief    enable DCI function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dci_enable(void)
{
    DCI_CTL |= DCI_CTL_DCIEN;
}

/*!
    \brief    disable DCI function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dci_disable(void)
{
    DCI_CTL &= ~DCI_CTL_DCIEN;
}

/*!
    \brief    enable DCI capture
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dci_capture_enable(void)
{
    DCI_CTL |= DCI_CTL_CAP;
}

/*!
    \brief    disable DCI capture
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dci_capture_disable(void)
{
    DCI_CTL &= ~DCI_CTL_CAP;
}

/*!
    \brief    enable DCI jpeg mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dci_jpeg_enable(void)
{
    DCI_CTL |= DCI_CTL_JM;
}

/*!
    \brief    disable DCI jpeg mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dci_jpeg_disable(void)
{
    DCI_CTL &= ~DCI_CTL_JM;
}

/*!
    \brief    enable cropping window function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dci_crop_window_enable(void)
{
    DCI_CTL |= DCI_CTL_WDEN;
}

/*!
    \brief    disable cropping window function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dci_crop_window_disable(void)
{
    DCI_CTL &= ~DCI_CTL_WDEN;
}

/*!
    \brief    configure DCI cropping window
    \param[in]  start_x: window horizontal start position
    \param[in]  start_y: window vertical start position
    \param[in]  size_width: window horizontal size
    \param[in]  size_height: window vertical size
    \param[out] none
    \retval     none
*/
void dci_crop_window_config(uint16_t start_x, uint16_t start_y, uint16_t size_width, uint16_t size_height)
{
    DCI_CWSPOS = ((uint32_t)start_x | ((uint32_t)start_y << 16));
    DCI_CWSZ = ((uint32_t)size_width | ((uint32_t)size_height << 16));
}

/*!
    \brief    enable embedded synchronous mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dci_embedded_sync_enable(void)
{
    DCI_CTL |= DCI_CTL_ESM;
}

/*!
    \brief    disble embedded synchronous mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dci_embedded_sync_disable(void)
{
    DCI_CTL &= ~DCI_CTL_ESM;
}
/*!
    \brief    config synchronous codes in embedded synchronous mode
    \param[in]  frame_start: frame start code in embedded synchronous mode
    \param[in]  line_start: line start code in embedded synchronous mode
    \param[in]  line_end: line end code in embedded synchronous mode
    \param[in]  frame_end: frame end code in embedded synchronous mode
    \param[out] none
    \retval     none
*/
void dci_sync_codes_config(uint8_t frame_start, uint8_t line_start, uint8_t line_end, uint8_t frame_end)
{
    DCI_SC = ((uint32_t)frame_start | ((uint32_t)line_start << 8) | ((uint32_t)line_end << 16) | ((uint32_t)frame_end << 24));
}

/*!
    \brief    config synchronous codes unmask in embedded synchronous mode
    \param[in]  frame_start: frame start code unmask bits in embedded synchronous mode
    \param[in]  line_start: line start code unmask bits in embedded synchronous mode
    \param[in]  line_end: line end code unmask bits in embedded synchronous mode
    \param[in]  frame_end: frame end code unmask bits in embedded synchronous mode
    \param[out] none
    \retval     none
*/
void dci_sync_codes_unmask_config(uint8_t frame_start, uint8_t line_start, uint8_t line_end, uint8_t frame_end)
{
    DCI_SCUMSK = ((uint32_t)frame_start | ((uint32_t)line_start << 8) | ((uint32_t)line_end << 16) | ((uint32_t)frame_end << 24));
}

/*!
    \brief    read DCI data register
    \param[in]  none
    \param[out] none
    \retval     data
*/
uint32_t dci_data_read(void)
{
    return DCI_DATA;
}

/*!
    \brief    get specified flag
    \param[in]  flag:
      \arg         DCI_FLAG_HS: HS line status
      \arg         DCI_FLAG_VS: VS line status
      \arg         DCI_FLAG_FV:FIFO valid
      \arg         DCI_FLAG_EF: end of frame flag
      \arg         DCI_FLAG_OVR: FIFO overrun flag
      \arg         DCI_FLAG_ESE: embedded synchronous error flag
      \arg         DCI_FLAG_VSYNC: vsync flag
      \arg         DCI_FLAG_EL: end of line flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus dci_flag_get(uint32_t flag)
{
    uint32_t stat = 0U;
    
    FlagStatus ret = RESET;
    
    if(flag >> 31) {
        /* get flag status from DCI_STAT1 register */
        stat = DCI_STAT1;
    } else {
        /* get flag status from DCI_STAT0 register */
        stat = DCI_STAT0;
    }

    if(flag & stat) {
        ret = SET;
    } else {
        ret = RESET;
    }
    
    return ret;
}

/*!
    \brief    enable specified DCI interrupt
    \param[in]  interrupt:
      \arg         DCI_INT_EF: end of frame interrupt
      \arg         DCI_INT_OVR: FIFO overrun interrupt
      \arg         DCI_INT_ESE: embedded synchronous error interrupt
      \arg         DCI_INT_VSYNC: vsync interrupt
      \arg         DCI_INT_EL: end of line interrupt
    \param[out] none
    \retval     none
*/
void dci_interrupt_enable(uint32_t interrupt)
{
    DCI_INTEN |= interrupt;
}

/*!
    \brief    disable specified DCI interrupt
    \param[in]  interrupt:
      \arg         DCI_INT_EF: end of frame interrupt
      \arg         DCI_INT_OVR: FIFO overrun interrupt
      \arg         DCI_INT_ESE: embedded synchronous error interrupt
      \arg         DCI_INT_VSYNC: vsync interrupt
      \arg         DCI_INT_EL: end of line interrupt
    \param[out] none
    \retval     none
*/
void dci_interrupt_disable(uint32_t interrupt)
{
    DCI_INTEN &= ~interrupt;
}

/*!
    \brief    clear specified interrupt flag
    \param[in]  int_flag:
      \arg         DCI_INT_EF: end of frame interrupt
      \arg         DCI_INT_OVR: FIFO overrun interrupt
      \arg         DCI_INT_ESE: embedded synchronous error interrupt
      \arg         DCI_INT_VSYNC: vsync interrupt
      \arg         DCI_INT_EL: end of line interrupt
    \param[out] none
    \retval     none
*/
void dci_interrupt_flag_clear(uint32_t int_flag)
{
    DCI_INTC |= int_flag;
}

/*!
    \brief    get specified interrupt flag
    \param[in]  int_flag:
      \arg         DCI_INT_FLAG_EF: end of frame interrupt flag
      \arg         DCI_INT_FLAG_OVR: FIFO overrun interrupt flag
      \arg         DCI_INT_FLAG_ESE: embedded synchronous error interrupt flag
      \arg         DCI_INT_FLAG_VSYNC: vsync interrupt flag
      \arg         DCI_INT_FLAG_EL: end of line interrupt flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus dci_interrupt_flag_get(uint32_t int_flag)
{
  FlagStatus ret = RESET;  
  
    if(RESET == (DCI_INTF & int_flag)) {
        ret = RESET;
    } else {
        ret = SET;
    }
    
    return ret;
}


