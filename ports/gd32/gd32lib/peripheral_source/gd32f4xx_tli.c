/*!
    \file    gd32f4xx_tli.c
    \brief   TLI driver

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

#include "gd32f4xx_tli.h"

#define TLI_DEFAULT_VALUE   0x00000000U
#define TLI_OPAQUE_VALUE    0x000000FFU

/*!
    \brief      deinitialize TLI registers
    \param[in]  none
    \param[out] none
    \retval     none
*/
void tli_deinit(void)
{
    rcu_periph_reset_enable(RCU_TLIRST);
    rcu_periph_reset_disable(RCU_TLIRST);
}

/*!
    \brief      initialize the parameters of TLI parameter structure with the default values, it is suggested
                that call this function after a tli_parameter_struct structure is defined
    \param[in]  none
    \param[out] tli_struct: the data needed to initialize TLI
                  synpsz_vpsz: size of the vertical synchronous pulse
                  synpsz_hpsz: size of the horizontal synchronous pulse
                  backpsz_vbpsz: size of the vertical back porch plus synchronous pulse
                  backpsz_hbpsz: size of the horizontal back porch plus synchronous pulse
                  activesz_vasz: size of the vertical active area width plus back porch and synchronous pulse
                  activesz_hasz: size of the horizontal active area width plus back porch and synchronous pulse
                  totalsz_vtsz: vertical total size of the display, including active area, back porch, synchronous
                  totalsz_htsz: vorizontal total size of the display, including active area, back porch, synchronous
                  backcolor_red: background value red
                  backcolor_green: background value green
                  backcolor_blue: background value blue
                  signalpolarity_hs: TLI_HSYN_ACTLIVE_LOW,TLI_HSYN_ACTLIVE_HIGH
                  signalpolarity_vs: TLI_VSYN_ACTLIVE_LOW,TLI_VSYN_ACTLIVE_HIGH
                  signalpolarity_de: TLI_DE_ACTLIVE_LOW,TLI_DE_ACTLIVE_HIGH
                  signalpolarity_pixelck: TLI_PIXEL_CLOCK_TLI,TLI_PIXEL_CLOCK_INVERTEDTLI
    \retval     none
*/
void tli_struct_para_init(tli_parameter_struct *tli_struct)
{
    /* initialize the struct parameters with default values */
    tli_struct->synpsz_vpsz = TLI_DEFAULT_VALUE;
    tli_struct->synpsz_hpsz = TLI_DEFAULT_VALUE;
    tli_struct->backpsz_vbpsz = TLI_DEFAULT_VALUE;
    tli_struct->backpsz_hbpsz = TLI_DEFAULT_VALUE;
    tli_struct->activesz_vasz = TLI_DEFAULT_VALUE;
    tli_struct->activesz_hasz = TLI_DEFAULT_VALUE;
    tli_struct->totalsz_vtsz = TLI_DEFAULT_VALUE;
    tli_struct->totalsz_htsz = TLI_DEFAULT_VALUE;
    tli_struct->backcolor_red = TLI_DEFAULT_VALUE;
    tli_struct->backcolor_green = TLI_DEFAULT_VALUE;
    tli_struct->backcolor_blue = TLI_DEFAULT_VALUE;
    tli_struct->signalpolarity_hs = TLI_HSYN_ACTLIVE_LOW;
    tli_struct->signalpolarity_vs = TLI_VSYN_ACTLIVE_LOW;
    tli_struct->signalpolarity_de = TLI_DE_ACTLIVE_LOW;
    tli_struct->signalpolarity_pixelck = TLI_PIXEL_CLOCK_TLI;
}

/*!
    \brief      initialize TLI display timing parameters
    \param[in]  tli_struct: the data needed to initialize TLI
                  synpsz_vpsz: size of the vertical synchronous pulse
                  synpsz_hpsz: size of the horizontal synchronous pulse
                  backpsz_vbpsz: size of the vertical back porch plus synchronous pulse
                  backpsz_hbpsz: size of the horizontal back porch plus synchronous pulse
                  activesz_vasz: size of the vertical active area width plus back porch and synchronous pulse
                  activesz_hasz: size of the horizontal active area width plus back porch and synchronous pulse
                  totalsz_vtsz: vertical total size of the display, including active area, back porch, synchronous
                  totalsz_htsz: vorizontal total size of the display, including active area, back porch, synchronous
                  backcolor_red: background value red
                  backcolor_green: background value green
                  backcolor_blue: background value blue
                  signalpolarity_hs: TLI_HSYN_ACTLIVE_LOW,TLI_HSYN_ACTLIVE_HIGH
                  signalpolarity_vs: TLI_VSYN_ACTLIVE_LOW,TLI_VSYN_ACTLIVE_HIGH
                  signalpolarity_de: TLI_DE_ACTLIVE_LOW,TLI_DE_ACTLIVE_HIGH
                  signalpolarity_pixelck: TLI_PIXEL_CLOCK_TLI,TLI_PIXEL_CLOCK_INVERTEDTLI
    \param[out] none
    \retval     none
*/
void tli_init(tli_parameter_struct *tli_struct)
{
    /* synchronous pulse size configuration */
    TLI_SPSZ &= ~(TLI_SPSZ_VPSZ | TLI_SPSZ_HPSZ);
    TLI_SPSZ = (uint32_t)((uint32_t)tli_struct->synpsz_vpsz | ((uint32_t)tli_struct->synpsz_hpsz << 16U));
    /* back-porch size configuration */
    TLI_BPSZ &= ~(TLI_BPSZ_VBPSZ | TLI_BPSZ_HBPSZ);
    TLI_BPSZ = (uint32_t)((uint32_t)tli_struct->backpsz_vbpsz | ((uint32_t)tli_struct->backpsz_hbpsz << 16U));
    /* active size configuration */
    TLI_ASZ &= ~(TLI_ASZ_VASZ | TLI_ASZ_HASZ);
    TLI_ASZ = (tli_struct->activesz_vasz | (tli_struct->activesz_hasz << 16U));
    /* total size configuration */
    TLI_TSZ &= ~(TLI_TSZ_VTSZ | TLI_TSZ_HTSZ);
    TLI_TSZ = (tli_struct->totalsz_vtsz | (tli_struct->totalsz_htsz << 16U));
    /* background color configuration */
    TLI_BGC &= ~(TLI_BGC_BVB | (TLI_BGC_BVG) | (TLI_BGC_BVR));
    TLI_BGC = (tli_struct->backcolor_blue | (tli_struct->backcolor_green << 8U) | (tli_struct->backcolor_red << 16U));
    TLI_CTL &= ~(TLI_CTL_HPPS | TLI_CTL_VPPS | TLI_CTL_DEPS | TLI_CTL_CLKPS);
    TLI_CTL |= (tli_struct->signalpolarity_hs | tli_struct->signalpolarity_vs | \
                tli_struct->signalpolarity_de | tli_struct->signalpolarity_pixelck);

}

/*!
    \brief      configure TLI dither function
    \param[in]  dither_stat
                only one parameter can be selected which is shown as below:
      \arg        TLI_DITHER_ENABLE
      \arg        TLI_DITHER_DISABLE
    \param[out] none
    \retval     none
*/
void tli_dither_config(uint8_t dither_stat)
{
    if(TLI_DITHER_ENABLE == dither_stat) {
        TLI_CTL |= TLI_CTL_DFEN;
    } else {
        TLI_CTL &= ~(TLI_CTL_DFEN);
    }
}

/*!
    \brief      enable TLI
    \param[in]  none
    \param[out] none
    \retval     none
*/
void tli_enable(void)
{
    TLI_CTL |= TLI_CTL_TLIEN;
}

/*!
    \brief    disable TLI
    \param[in]  none
    \param[out] none
    \retval     none
*/
void tli_disable(void)
{
    TLI_CTL &= ~(TLI_CTL_TLIEN);
}

/*!
    \brief      configure TLI reload mode
    \param[in]  reload_mod
                only one parameter can be selected which is shown as below:
      \arg        TLI_FRAME_BLANK_RELOAD_EN
      \arg        TLI_REQUEST_RELOAD_EN
    \param[out] none
    \retval     none
*/
void tli_reload_config(uint8_t reload_mod)
{
    if(TLI_FRAME_BLANK_RELOAD_EN == reload_mod) {
        /* the layer configuration will be reloaded at frame blank */
        TLI_RL |= TLI_RL_FBR;
    } else {
        /* the layer configuration will be reloaded after this bit sets */
        TLI_RL |= TLI_RL_RQR;
    }
}

/*!
    \brief      initialize the parameters of TLI layer structure with the default values, it is suggested
                that call this function after a tli_layer_parameter_struct structure is defined
    \param[in]  none
    \param[out] layer_struct: TLI Layer parameter struct
                  layer_window_rightpos: window right position
                  layer_window_leftpos: window left position
                  layer_window_bottompos: window bottom position
                  layer_window_toppos: window top position
                  layer_ppf: LAYER_PPF_ARGB8888,LAYER_PPF_RGB888,LAYER_PPF_RGB565,
                                 LAYER_PPF_ARG1555,LAYER_PPF_ARGB4444,LAYER_PPF_L8,
                                 LAYER_PPF_AL44,LAYER_PPF_AL88
                  layer_sa: specified alpha
                  layer_default_alpha: the default color alpha
                  layer_default_red: the default color red
                  layer_default_green: the default color green
                  layer_default_blue: the default color blue
                  layer_acf1: LAYER_ACF1_SA,LAYER_ACF1_PASA
                  layer_acf2: LAYER_ACF2_SA,LAYER_ACF2_PASA
                  layer_frame_bufaddr: frame buffer base address
                  layer_frame_buf_stride_offset: frame buffer stride offset
                  layer_frame_line_length: frame line length
                  layer_frame_total_line_number: frame total line number
    \retval     none
*/
void tli_layer_struct_para_init(tli_layer_parameter_struct *layer_struct)
{
    /* initialize the struct parameters with default values */
    layer_struct->layer_window_rightpos = TLI_DEFAULT_VALUE;
    layer_struct->layer_window_leftpos = TLI_DEFAULT_VALUE;
    layer_struct->layer_window_bottompos = TLI_DEFAULT_VALUE;
    layer_struct->layer_window_toppos = TLI_DEFAULT_VALUE;
    layer_struct->layer_ppf = LAYER_PPF_ARGB8888;
    layer_struct->layer_sa = TLI_OPAQUE_VALUE;
    layer_struct->layer_default_alpha = TLI_DEFAULT_VALUE;
    layer_struct->layer_default_red = TLI_DEFAULT_VALUE;
    layer_struct->layer_default_green = TLI_DEFAULT_VALUE;
    layer_struct->layer_default_blue = TLI_DEFAULT_VALUE;
    layer_struct->layer_acf1 = LAYER_ACF1_PASA;
    layer_struct->layer_acf2 = LAYER_ACF2_PASA;
    layer_struct->layer_frame_bufaddr = TLI_DEFAULT_VALUE;
    layer_struct->layer_frame_buf_stride_offset = TLI_DEFAULT_VALUE;
    layer_struct->layer_frame_line_length = TLI_DEFAULT_VALUE;
    layer_struct->layer_frame_total_line_number = TLI_DEFAULT_VALUE;
}

/*!
    \brief      initialize TLI layer
    \param[in]  layerx: LAYERx(x=0,1)
    \param[in]  layer_struct: TLI Layer parameter struct
                  layer_window_rightpos: window right position
                  layer_window_leftpos: window left position
                  layer_window_bottompos: window bottom position
                  layer_window_toppos: window top position
                  layer_ppf: LAYER_PPF_ARGB8888,LAYER_PPF_RGB888,LAYER_PPF_RGB565,
                                 LAYER_PPF_ARG1555,LAYER_PPF_ARGB4444,LAYER_PPF_L8,
                                 LAYER_PPF_AL44,LAYER_PPF_AL88
                  layer_sa: specified alpha
                  layer_default_alpha: the default color alpha
                  layer_default_red: the default color red
                  layer_default_green: the default color green
                  layer_default_blue: the default color blue
                  layer_acf1: LAYER_ACF1_SA,LAYER_ACF1_PASA
                  layer_acf2: LAYER_ACF2_SA,LAYER_ACF2_PASA
                  layer_frame_bufaddr: frame buffer base address
                  layer_frame_buf_stride_offset: frame buffer stride offset
                  layer_frame_line_length: frame line length
                  layer_frame_total_line_number: frame total line number
    \param[out] none
    \retval     none
*/
void tli_layer_init(uint32_t layerx, tli_layer_parameter_struct *layer_struct)
{
    /* configure layer window horizontal position */
    TLI_LxHPOS(layerx) &= ~(TLI_LxHPOS_WLP | (TLI_LxHPOS_WRP));
    TLI_LxHPOS(layerx) = (uint32_t)((uint32_t)layer_struct->layer_window_leftpos | ((uint32_t)layer_struct->layer_window_rightpos << 16U));
    /* configure layer window vertical position */
    TLI_LxVPOS(layerx) &= ~(TLI_LxVPOS_WTP | (TLI_LxVPOS_WBP));
    TLI_LxVPOS(layerx) = (uint32_t)((uint32_t)layer_struct->layer_window_toppos | ((uint32_t)layer_struct->layer_window_bottompos << 16U));
    /* configure layer packeted pixel format */
    TLI_LxPPF(layerx) &= ~(TLI_LxPPF_PPF);
    TLI_LxPPF(layerx) = layer_struct->layer_ppf;
    /* configure layer specified alpha */
    TLI_LxSA(layerx) &= ~(TLI_LxSA_SA);
    TLI_LxSA(layerx) = layer_struct->layer_sa;
    /* configure layer default color */
    TLI_LxDC(layerx) &= ~(TLI_LxDC_DCB | (TLI_LxDC_DCG) | (TLI_LxDC_DCR) | (TLI_LxDC_DCA));
    TLI_LxDC(layerx) = (uint32_t)((uint32_t)layer_struct->layer_default_blue | ((uint32_t)layer_struct->layer_default_green << 8U)
                                  | ((uint32_t)layer_struct->layer_default_red << 16U)
                                  | ((uint32_t)layer_struct->layer_default_alpha << 24U));

    /* configure layer alpha calculation factors */
    TLI_LxBLEND(layerx) &= ~(TLI_LxBLEND_ACF2 | (TLI_LxBLEND_ACF1));
    TLI_LxBLEND(layerx) = ((layer_struct->layer_acf2) | (layer_struct->layer_acf1));
    /* configure layer frame buffer base address */
    TLI_LxFBADDR(layerx) &= ~(TLI_LxFBADDR_FBADD);
    TLI_LxFBADDR(layerx) = (layer_struct->layer_frame_bufaddr);
    /* configure layer frame line length */
    TLI_LxFLLEN(layerx) &= ~(TLI_LxFLLEN_FLL | (TLI_LxFLLEN_STDOFF));
    TLI_LxFLLEN(layerx) = (uint32_t)((uint32_t)layer_struct->layer_frame_line_length | ((uint32_t)layer_struct->layer_frame_buf_stride_offset << 16U));
    /* configure layer frame total line number */
    TLI_LxFTLN(layerx) &= ~(TLI_LxFTLN_FTLN);
    TLI_LxFTLN(layerx) = (uint32_t)(layer_struct->layer_frame_total_line_number);

}

/*!
    \brief      reconfigure window position
    \param[in]  layerx: LAYERx(x=0,1)
    \param[in]  offset_x: new horizontal offset
    \param[in]  offset_y: new vertical offset
    \param[out] none
    \retval     none
*/
void tli_layer_window_offset_modify(uint32_t layerx, uint16_t offset_x, uint16_t offset_y)
{
    /* configure window start position */
    uint32_t layer_ppf, line_num, hstart, vstart;
    uint32_t line_length = 0U;
    TLI_LxHPOS(layerx) &= ~(TLI_LxHPOS_WLP | (TLI_LxHPOS_WRP));
    TLI_LxVPOS(layerx) &= ~(TLI_LxVPOS_WTP | (TLI_LxVPOS_WBP));
    hstart = (uint32_t)offset_x + (((TLI_BPSZ & TLI_BPSZ_HBPSZ) >> 16U) + 1U);
    vstart = (uint32_t)offset_y + ((TLI_BPSZ & TLI_BPSZ_VBPSZ) + 1U);
    line_num = (TLI_LxFTLN(layerx) & TLI_LxFTLN_FTLN);
    layer_ppf = (TLI_LxPPF(layerx) & TLI_LxPPF_PPF);
    /* the bytes of a line equal TLI_LxFLLEN_FLL bits value minus 3 */
    switch(layer_ppf) {
    case LAYER_PPF_ARGB8888:
        /* each pixel includes 4bytes, when pixel format is ARGB8888 */
        line_length = (((TLI_LxFLLEN(layerx) & TLI_LxFLLEN_FLL) - 3U) / 4U);
        break;
    case LAYER_PPF_RGB888:
        /* each pixel includes 3bytes, when pixel format is RGB888 */
        line_length = (((TLI_LxFLLEN(layerx) & TLI_LxFLLEN_FLL) - 3U) / 3U);
        break;
    case LAYER_PPF_RGB565:
    case LAYER_PPF_ARGB1555:
    case LAYER_PPF_ARGB4444:
    case LAYER_PPF_AL88:
        /* each pixel includes 2bytes, when pixel format is RGB565,ARG1555,ARGB4444 or AL88 */
        line_length = (((TLI_LxFLLEN(layerx) & TLI_LxFLLEN_FLL) - 3U) / 2U);
        break;
    case LAYER_PPF_L8:
    case LAYER_PPF_AL44:
        /* each pixel includes 1byte, when pixel format is L8 or AL44 */
        line_length = (((TLI_LxFLLEN(layerx) & TLI_LxFLLEN_FLL) - 3U));
        break;
    default:
        break;
    }
    /* reconfigure window position */
    TLI_LxHPOS(layerx) = (hstart | ((hstart + line_length - 1U) << 16U));
    TLI_LxVPOS(layerx) = (vstart | ((vstart + line_num - 1U) << 16U));
}

/*!
    \brief      initialize the parameters of TLI layer LUT structure with the default values, it is suggested
                that call this function after a tli_layer_lut_parameter_struct structure is defined
    \param[in]  none
    \param[out] lut_struct: TLI layer LUT parameter struct
                  layer_table_addr: look up table write address
                  layer_lut_channel_red: red channel of a LUT entry
                  layer_lut_channel_green: green channel of a LUT entry
                  layer_lut_channel_blue: blue channel of a LUT entry
    \retval     none
*/
void tli_lut_struct_para_init(tli_layer_lut_parameter_struct *lut_struct)
{
    /* initialize the struct parameters with default values */
    lut_struct->layer_table_addr = TLI_DEFAULT_VALUE;
    lut_struct->layer_lut_channel_red = TLI_DEFAULT_VALUE;
    lut_struct->layer_lut_channel_green = TLI_DEFAULT_VALUE;
    lut_struct->layer_lut_channel_blue = TLI_DEFAULT_VALUE;
}

/*!
    \brief      initialize TLI layer LUT
    \param[in]  layerx: LAYERx(x=0,1)
    \param[in]  lut_struct: TLI layer LUT parameter struct
                  layer_table_addr: look up table write address
                  layer_lut_channel_red: red channel of a LUT entry
                  layer_lut_channel_green: green channel of a LUT entry
                  layer_lut_channel_blue: blue channel of a LUT entry
    \param[out] none
    \retval     none
*/
void tli_lut_init(uint32_t layerx, tli_layer_lut_parameter_struct *lut_struct)
{
    TLI_LxLUT(layerx) = (uint32_t)(((uint32_t)lut_struct->layer_lut_channel_blue) | ((uint32_t)lut_struct->layer_lut_channel_green << 8U)
                                   | ((uint32_t)lut_struct->layer_lut_channel_red << 16U
                                      | ((uint32_t)lut_struct->layer_table_addr << 24U)));
}

/*!
    \brief      initialize TLI layer color key
    \param[in]  layerx: LAYERx(x=0,1)
    \param[in]  redkey: color key red
    \param[in]  greenkey: color key green
    \param[in]  bluekey: color key blue
    \param[out] none
    \retval     none
*/
void tli_color_key_init(uint32_t layerx, uint8_t redkey, uint8_t greenkey, uint8_t bluekey)
{
    TLI_LxCKEY(layerx) = (((uint32_t)bluekey) | ((uint32_t)greenkey << 8U) | ((uint32_t)redkey << 16U));
}

/*!
    \brief      enable TLI layer
    \param[in]  layerx: LAYERx(x=0,1)
    \param[out] none
    \retval     none
*/
void tli_layer_enable(uint32_t layerx)
{
    TLI_LxCTL(layerx) |= TLI_LxCTL_LEN;
}

/*!
    \brief      disable TLI layer
    \param[in]  layerx: LAYERx(x=0,1)
    \param[out] none
    \retval     none
*/
void tli_layer_disable(uint32_t layerx)
{
    TLI_LxCTL(layerx) &= ~(TLI_LxCTL_LEN);
}

/*!
    \brief      enable TLI layer color keying
    \param[in]  layerx: LAYERx(x=0,1)
    \param[out] none
    \retval     none
*/
void tli_color_key_enable(uint32_t layerx)
{
    TLI_LxCTL(layerx) |= TLI_LxCTL_CKEYEN;
}

/*!
    \brief      disable TLI layer color keying
    \param[in]  layerx: LAYERx(x=0,1)
    \param[out] none
    \retval     none
*/
void tli_color_key_disable(uint32_t layerx)
{
    TLI_LxCTL(layerx) &= ~(TLI_LxCTL_CKEYEN);
}

/*!
    \brief      enable TLI layer LUT
    \param[in]  layerx: LAYERx(x=0,1)
    \param[out] none
    \retval     none
*/
void tli_lut_enable(uint32_t layerx)
{
    TLI_LxCTL(layerx) |= TLI_LxCTL_LUTEN;
}

/*!
    \brief      disable TLI layer LUT
    \param[in]  layerx: LAYERx(x=0,1)
    \param[out] none
    \retval     none
*/
void tli_lut_disable(uint32_t layerx)
{
    TLI_LxCTL(layerx) &= ~(TLI_LxCTL_LUTEN);
}

/*!
    \brief      set line mark value
    \param[in]  line_num: line number
    \param[out] none
    \retval     none
*/
void tli_line_mark_set(uint16_t line_num)
{
    TLI_LM &= ~(TLI_LM_LM);
    TLI_LM = (uint32_t)line_num;
}

/*!
    \brief      get current displayed position
    \param[in]  none
    \param[out] none
    \retval     position of current pixel
*/
uint32_t tli_current_pos_get(void)
{
    return TLI_CPPOS;
}

/*!
    \brief      enable TLI interrupt
    \param[in]  int_flag: TLI interrupt flags
                one or more parameters can be selected which are shown as below:
      \arg        TLI_INT_LM: line mark interrupt
      \arg        TLI_INT_FE: FIFO error interrupt
      \arg        TLI_INT_TE: transaction error interrupt
      \arg        TLI_INT_LCR: layer configuration reloaded interrupt
    \param[out] none
    \retval     none
*/
void tli_interrupt_enable(uint32_t int_flag)
{
    TLI_INTEN |= (int_flag);
}

/*!
    \brief      disable TLI interrupt
    \param[in]  int_flag: TLI interrupt flags
                one or more parameters can be selected which are shown as below:
      \arg        TLI_INT_LM: line mark interrupt
      \arg        TLI_INT_FE: FIFO error interrupt
      \arg        TLI_INT_TE: transaction error interrupt
      \arg        TLI_INT_LCR: layer configuration reloaded interrupt
    \param[out] none
    \retval     none
*/
void tli_interrupt_disable(uint32_t int_flag)
{
    TLI_INTEN &= ~(int_flag);
}

/*!
    \brief      get TLI interrupt flag
    \param[in]  int_flag: TLI interrupt flags
                one or more parameters can be selected which are shown as below:
      \arg        TLI_INT_FLAG_LM: line mark interrupt flag
      \arg        TLI_INT_FLAG_FE: FIFO error interrupt flag
      \arg        TLI_INT_FLAG_TE: transaction error interrupt flag
      \arg        TLI_INT_FLAG_LCR: layer configuration reloaded interrupt flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus tli_interrupt_flag_get(uint32_t int_flag)
{
    FlagStatus status = RESET;
    uint32_t state;
    state = TLI_INTF;
    if(state & int_flag) {
        state = TLI_INTEN;
        /* check whether the corresponding bit in TLI_INTEN is set or not */
        if(state & int_flag) {
            status = SET;
        }
    }
    return status;
}

/*!
    \brief      clear TLI interrupt flag
    \param[in]  int_flag: TLI interrupt flags
                one or more parameters can be selected which are shown as below:
      \arg        TLI_INT_FLAG_LM: line mark interrupt flag
      \arg        TLI_INT_FLAG_FE: FIFO error interrupt flag
      \arg        TLI_INT_FLAG_TE: transaction error interrupt flag
      \arg        TLI_INT_FLAG_LCR: layer configuration reloaded interrupt flag
    \param[out] none
    \retval     none
*/
void tli_interrupt_flag_clear(uint32_t int_flag)
{
    TLI_INTC |= (int_flag);
}

/*!
    \brief      get TLI flag or state in TLI_INTF register or TLI_STAT register
    \param[in]  flag: TLI flags or states
                only one parameter can be selected which is shown as below:
      \arg        TLI_FLAG_VDE: current VDE state
      \arg        TLI_FLAG_HDE: current HDE state
      \arg        TLI_FLAG_VS: current VS status of the TLI
      \arg        TLI_FLAG_HS: current HS status of the TLI
      \arg        TLI_FLAG_LM: line mark interrupt flag
      \arg        TLI_FLAG_FE: FIFO error interrupt flag
      \arg        TLI_FLAG_TE: transaction error interrupt flag
      \arg        TLI_FLAG_LCR: layer configuration reloaded interrupt flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus tli_flag_get(uint32_t flag)
{
    FlagStatus status;
    uint32_t stat;
    /* choose which register to get flag or state */
    if(flag >> 31U) {
        stat = TLI_INTF;
    } else {
        stat = TLI_STAT;
    }
    if(flag & stat) {
        status = SET;
    } else {
        status = RESET;
    }
    return status;
}
