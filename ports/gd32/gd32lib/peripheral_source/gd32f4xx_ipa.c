/*!
    \file    gd32f4xx_ipa.c
    \brief   IPA driver

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

#include "gd32f4xx_ipa.h"

#define IPA_DEFAULT_VALUE   0x00000000U

/*!
    \brief    deinitialize IPA registers
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ipa_deinit(void)
{
    rcu_periph_reset_enable(RCU_IPARST);
    rcu_periph_reset_disable(RCU_IPARST);
}

/*!
    \brief    enable IPA transfer
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ipa_transfer_enable(void)
{
    IPA_CTL |= IPA_CTL_TEN;
}

/*!
    \brief    enable IPA transfer hang up
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ipa_transfer_hangup_enable(void)
{
    IPA_CTL |= IPA_CTL_THU;
}

/*!
    \brief    disable IPA transfer hang up
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ipa_transfer_hangup_disable(void)
{
    IPA_CTL &= ~(IPA_CTL_THU);
}

/*!
    \brief    enable IPA transfer stop
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ipa_transfer_stop_enable(void)
{
    IPA_CTL |= IPA_CTL_TST;
}

/*!
    \brief    disable IPA transfer stop
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ipa_transfer_stop_disable(void)
{
    IPA_CTL &= ~(IPA_CTL_TST);
}
/*!
    \brief    enable IPA foreground LUT loading
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ipa_foreground_lut_loading_enable(void)
{
    IPA_FPCTL |= IPA_FPCTL_FLLEN;
}

/*!
    \brief    enable IPA background LUT loading
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ipa_background_lut_loading_enable(void)
{
    IPA_BPCTL |= IPA_BPCTL_BLLEN;
}

/*!
    \brief    set pixel format convert mode, the function is invalid when the IPA transfer is enabled
    \param[in]  pfcm: pixel format convert mode
                only one parameter can be selected which is shown as below:
      \arg        IPA_FGTODE: foreground memory to destination memory without pixel format convert
      \arg        IPA_FGTODE_PF_CONVERT: foreground memory to destination memory with pixel format convert
      \arg        IPA_FGBGTODE: blending foreground and background memory to destination memory
      \arg        IPA_FILL_UP_DE: fill up destination memory with specific color
    \param[out] none
    \retval     none
*/
void ipa_pixel_format_convert_mode_set(uint32_t pfcm)
{
    IPA_CTL &= ~(IPA_CTL_PFCM);
    IPA_CTL |= pfcm;
}

/*!
    \brief    initialize the structure of IPA foreground parameter struct with the default values, it is
                suggested that call this function after an ipa_foreground_parameter_struct structure is defined
    \param[in]  none
    \param[out] foreground_struct: the data needed to initialize foreground
                  foreground_memaddr: foreground memory base address
                  foreground_lineoff: foreground line offset
                  foreground_prealpha: foreground pre-defined alpha value
                  foreground_alpha_algorithm: IPA_FG_ALPHA_MODE_0,IPA_FG_ALPHA_MODE_1,IPA_FG_ALPHA_MODE_2
                  foreground_pf: foreground pixel format(FOREGROUND_PPF_ARGB8888,FOREGROUND_PPF_RGB888,FOREGROUND_PPF_RGB565,
                            FOREGROUND_PPF_ARG1555,FOREGROUND_PPF_ARGB4444,FOREGROUND_PPF_L8,FOREGROUND_PPF_AL44,
                            FOREGROUND_PPF_AL88,FOREGROUND_PPF_L4,FOREGROUND_PPF_A8,FOREGROUND_PPF_A4)
                  foreground_prered: foreground pre-defined red value
                  foreground_pregreen: foreground pre-defined green value
                  foreground_preblue: foreground pre-defined blue value
    \retval     none
*/
void ipa_foreground_struct_para_init(ipa_foreground_parameter_struct *foreground_struct)
{
    /* initialize the struct parameters with default values */
    foreground_struct->foreground_memaddr = IPA_DEFAULT_VALUE;
    foreground_struct->foreground_lineoff = IPA_DEFAULT_VALUE;
    foreground_struct->foreground_prealpha = IPA_DEFAULT_VALUE;
    foreground_struct->foreground_alpha_algorithm = IPA_FG_ALPHA_MODE_0;
    foreground_struct->foreground_pf = FOREGROUND_PPF_ARGB8888;
    foreground_struct->foreground_prered = IPA_DEFAULT_VALUE;
    foreground_struct->foreground_pregreen = IPA_DEFAULT_VALUE;
    foreground_struct->foreground_preblue = IPA_DEFAULT_VALUE;
}

/*!
    \brief    initialize foreground parameters
    \param[in]  foreground_struct: the data needed to initialize foreground
                  foreground_memaddr: foreground memory base address
                  foreground_lineoff: foreground line offset
                  foreground_prealpha: foreground pre-defined alpha value
                  foreground_alpha_algorithm: IPA_FG_ALPHA_MODE_0,IPA_FG_ALPHA_MODE_1,IPA_FG_ALPHA_MODE_2
                  foreground_pf: foreground pixel format(FOREGROUND_PPF_ARGB8888,FOREGROUND_PPF_RGB888,FOREGROUND_PPF_RGB565,
                            FOREGROUND_PPF_ARG1555,FOREGROUND_PPF_ARGB4444,FOREGROUND_PPF_L8,FOREGROUND_PPF_AL44,
                            FOREGROUND_PPF_AL88,FOREGROUND_PPF_L4,FOREGROUND_PPF_A8,FOREGROUND_PPF_A4)
                  foreground_prered: foreground pre-defined red value
                  foreground_pregreen: foreground pre-defined green value
                  foreground_preblue: foreground pre-defined blue value
    \param[out] none
    \retval     none
*/
void ipa_foreground_init(ipa_foreground_parameter_struct *foreground_struct)
{
    FlagStatus tempflag = RESET;
    if(RESET != (IPA_CTL & IPA_CTL_TEN)) {
        tempflag = SET;
        /* reset the TEN in order to configure the following bits */
        IPA_CTL &= ~IPA_CTL_TEN;
    }

    /* foreground memory base address configuration */
    IPA_FMADDR &= ~(IPA_FMADDR_FMADDR);
    IPA_FMADDR = foreground_struct->foreground_memaddr;
    /* foreground line offset configuration */
    IPA_FLOFF &= ~(IPA_FLOFF_FLOFF);
    IPA_FLOFF = foreground_struct->foreground_lineoff;
    /* foreground pixel format pre-defined alpha, alpha calculation algorithm configuration */
    IPA_FPCTL &= ~(IPA_FPCTL_FPDAV | IPA_FPCTL_FAVCA | IPA_FPCTL_FPF);
    IPA_FPCTL |= (foreground_struct->foreground_prealpha << 24U);
    IPA_FPCTL |= foreground_struct->foreground_alpha_algorithm;
    IPA_FPCTL |= foreground_struct->foreground_pf;
    /* foreground pre-defined red green blue configuration */
    IPA_FPV &= ~(IPA_FPV_FPDRV | IPA_FPV_FPDGV | IPA_FPV_FPDBV);
    IPA_FPV |= ((foreground_struct->foreground_prered << 16U) | (foreground_struct->foreground_pregreen << 8U)
                | (foreground_struct->foreground_preblue));

    if(SET == tempflag) {
        /* restore the state of TEN */
        IPA_CTL |= IPA_CTL_TEN;
    }
}

/*!
    \brief    initialize the structure of IPA background parameter struct with the default values, it is
                suggested that call this function after an ipa_background_parameter_struct structure is defined
    \param[in]  none
    \param[out] background_struct: the data needed to initialize background
                  background_memaddr: background memory base address
                  background_lineoff: background line offset
                  background_prealpha: background pre-defined alpha value
                  background_alpha_algorithm: IPA_BG_ALPHA_MODE_0,IPA_BG_ALPHA_MODE_1,IPA_BG_ALPHA_MODE_2
                  background_pf: background pixel format(BACKGROUND_PPF_ARGB8888,BACKGROUND_PPF_RGB888,BACKGROUND_PPF_RGB565,
                            BACKGROUND_PPF_ARG1555,BACKGROUND_PPF_ARGB4444,BACKGROUND_PPF_L8,BACKGROUND_PPF_AL44,
                            BACKGROUND_PPF_AL88,BACKGROUND_PPF_L4,BACKGROUND_PPF_A8,BACKGROUND_PPF_A4)
                  background_prered: background pre-defined red value
                  background_pregreen: background pre-defined green value
                  background_preblue: background pre-defined blue value
    \retval     none
*/
void ipa_background_struct_para_init(ipa_background_parameter_struct *background_struct)
{
    /* initialize the struct parameters with default values */
    background_struct->background_memaddr = IPA_DEFAULT_VALUE;
    background_struct->background_lineoff = IPA_DEFAULT_VALUE;
    background_struct->background_prealpha = IPA_DEFAULT_VALUE;
    background_struct->background_alpha_algorithm = IPA_BG_ALPHA_MODE_0;
    background_struct->background_pf = BACKGROUND_PPF_ARGB8888;
    background_struct->background_prered = IPA_DEFAULT_VALUE;
    background_struct->background_pregreen = IPA_DEFAULT_VALUE;
    background_struct->background_preblue = IPA_DEFAULT_VALUE;
}

/*!
    \brief    initialize background parameters
    \param[in]  background_struct: the data needed to initialize background
                  background_memaddr: background memory base address
                  background_lineoff: background line offset
                  background_prealpha: background pre-defined alpha value
                  background_alpha_algorithm: IPA_BG_ALPHA_MODE_0,IPA_FG_ALPHA_MODE_1,IPA_FG_ALPHA_MODE_2
                  background_pf: background pixel format(BACKGROUND_PPF_ARGB8888,BACKGROUND_PPF_RGB888,BACKGROUND_PPF_RGB565,
                            BACKGROUND_PPF_ARG1555,BACKGROUND_PPF_ARGB4444,BACKGROUND_PPF_L8,BACKGROUND_PPF_AL44,
                            BACKGROUND_PPF_AL88,BACKGROUND_PPF_L4,BACKGROUND_PPF_A8,BACKGROUND_PPF_A4)
                  background_prered: background pre-defined red value
                  background_pregreen: background pre-defined green value
                  background_preblue: background pre-defined blue value
    \param[out] none
    \retval     none
*/
void ipa_background_init(ipa_background_parameter_struct *background_struct)
{
    FlagStatus tempflag = RESET;
    if(RESET != (IPA_CTL & IPA_CTL_TEN)) {
        tempflag = SET;
        /* reset the TEN in order to configure the following bits */
        IPA_CTL &= ~IPA_CTL_TEN;
    }

    /* background memory base address configuration */
    IPA_BMADDR &= ~(IPA_BMADDR_BMADDR);
    IPA_BMADDR = background_struct->background_memaddr;
    /* background line offset configuration */
    IPA_BLOFF &= ~(IPA_BLOFF_BLOFF);
    IPA_BLOFF = background_struct->background_lineoff;
    /* background pixel format pre-defined alpha, alpha calculation algorithm configuration */
    IPA_BPCTL &= ~(IPA_BPCTL_BPDAV | IPA_BPCTL_BAVCA | IPA_BPCTL_BPF);
    IPA_BPCTL |= (background_struct->background_prealpha << 24U);
    IPA_BPCTL |= background_struct->background_alpha_algorithm;
    IPA_BPCTL |= background_struct->background_pf;
    /* background pre-defined red green blue configuration */
    IPA_BPV &= ~(IPA_BPV_BPDRV | IPA_BPV_BPDGV | IPA_BPV_BPDBV);
    IPA_BPV |= ((background_struct->background_prered << 16U) | (background_struct->background_pregreen << 8U)
                | (background_struct->background_preblue));

    if(SET == tempflag) {
        /* restore the state of TEN */
        IPA_CTL |= IPA_CTL_TEN;
    }
}

/*!
    \brief    initialize the structure of IPA destination parameter struct with the default values, it is
                suggested that call this function after an ipa_destination_parameter_struct structure is defined
    \param[in]  none
    \param[out] destination_struct: the data needed to initialize destination parameter
                  destination_pf: IPA_DPF_ARGB8888,IPA_DPF_RGB888,IPA_DPF_RGB565,IPA_DPF_ARGB1555,
                              IPA_DPF_ARGB4444,refer to ipa_dpf_enum
                  destination_lineoff: destination line offset
                  destination_prealpha: destination pre-defined alpha value
                  destination_prered: destination pre-defined red value
                  destination_pregreen: destination pre-defined green value
                  destination_preblue: destination pre-defined blue value
                  destination_memaddr: destination memory base address
                  image_width: width of the image to be processed
                  image_height: height of the image to be processed
    \retval     none
*/
void ipa_destination_struct_para_init(ipa_destination_parameter_struct *destination_struct)
{
    /* initialize the struct parameters with default values */
    destination_struct->destination_pf = IPA_DPF_ARGB8888;
    destination_struct->destination_lineoff = IPA_DEFAULT_VALUE;
    destination_struct->destination_prealpha = IPA_DEFAULT_VALUE;
    destination_struct->destination_prered = IPA_DEFAULT_VALUE;
    destination_struct->destination_pregreen = IPA_DEFAULT_VALUE;
    destination_struct->destination_preblue = IPA_DEFAULT_VALUE;
    destination_struct->destination_memaddr = IPA_DEFAULT_VALUE;
    destination_struct->image_width = IPA_DEFAULT_VALUE;
    destination_struct->image_height = IPA_DEFAULT_VALUE;
}

/*!
    \brief    initialize destination parameters
    \param[in]  destination_struct: the data needed to initialize destination parameters
                  destination_pf: IPA_DPF_ARGB8888,IPA_DPF_RGB888,IPA_DPF_RGB565,IPA_DPF_ARGB1555,
                                IPA_DPF_ARGB4444,refer to ipa_dpf_enum
                  destination_lineoff: destination line offset
                  destination_prealpha: destination pre-defined alpha value
                  destination_prered: destination pre-defined red value
                  destination_pregreen: destination pre-defined green value
                  destination_preblue: destination pre-defined blue value
                  destination_memaddr: destination memory base address
                  image_width: width of the image to be processed
                  image_height: height of the image to be processed
    \param[out] none
    \retval     none
*/
void ipa_destination_init(ipa_destination_parameter_struct *destination_struct)
{
    uint32_t destination_pixelformat;
    FlagStatus tempflag = RESET;
    if(RESET != (IPA_CTL & IPA_CTL_TEN)) {
        tempflag = SET;
        /* reset the TEN in order to configure the following bits */
        IPA_CTL &= ~IPA_CTL_TEN;
    }

    /* destination pixel format configuration */
    IPA_DPCTL &= ~(IPA_DPCTL_DPF);
    IPA_DPCTL = destination_struct->destination_pf;
    destination_pixelformat = destination_struct->destination_pf;
    /* destination pixel format ARGB8888 */
    switch(destination_pixelformat) {
    case IPA_DPF_ARGB8888:
        IPA_DPV &= ~(IPA_DPV_DPDBV_0 | (IPA_DPV_DPDGV_0) | (IPA_DPV_DPDRV_0) | (IPA_DPV_DPDAV_0));
        IPA_DPV = (destination_struct->destination_preblue | (destination_struct->destination_pregreen << 8U)
                   | (destination_struct->destination_prered << 16U)
                   | (destination_struct->destination_prealpha << 24U));
        break;
    /* destination pixel format RGB888 */
    case IPA_DPF_RGB888:
        IPA_DPV &= ~(IPA_DPV_DPDBV_1 | (IPA_DPV_DPDGV_1) | (IPA_DPV_DPDRV_1));
        IPA_DPV = (destination_struct->destination_preblue | (destination_struct->destination_pregreen << 8U)
                   | (destination_struct->destination_prered << 16U));
        break;
    /* destination pixel format RGB565 */
    case IPA_DPF_RGB565:
        IPA_DPV &= ~(IPA_DPV_DPDBV_2 | (IPA_DPV_DPDGV_2) | (IPA_DPV_DPDRV_2));
        IPA_DPV = (destination_struct->destination_preblue | (destination_struct->destination_pregreen << 5U)
                   | (destination_struct->destination_prered << 11U));
        break;
    /* destination pixel format ARGB1555 */
    case IPA_DPF_ARGB1555:
        IPA_DPV &= ~(IPA_DPV_DPDBV_3 | (IPA_DPV_DPDGV_3) | (IPA_DPV_DPDRV_3) | (IPA_DPV_DPDAV_3));
        IPA_DPV = (destination_struct->destination_preblue | (destination_struct->destination_pregreen << 5U)
                   | (destination_struct->destination_prered << 10U)
                   | (destination_struct->destination_prealpha << 15U));
        break;
    /* destination pixel format ARGB4444 */
    case IPA_DPF_ARGB4444:
        IPA_DPV &= ~(IPA_DPV_DPDBV_4 | (IPA_DPV_DPDGV_4) | (IPA_DPV_DPDRV_4) | (IPA_DPV_DPDAV_4));
        IPA_DPV = (destination_struct->destination_preblue | (destination_struct->destination_pregreen << 4U)
                   | (destination_struct->destination_prered << 8U)
                   | (destination_struct->destination_prealpha << 12U));
        break;
    default:
        break;
    }
    /* destination memory base address configuration */
    IPA_DMADDR &= ~(IPA_DMADDR_DMADDR);
    IPA_DMADDR = destination_struct->destination_memaddr;
    /* destination line offset configuration */
    IPA_DLOFF &= ~(IPA_DLOFF_DLOFF);
    IPA_DLOFF = destination_struct->destination_lineoff;
    /* image size configuration */
    IPA_IMS &= ~(IPA_IMS_HEIGHT | IPA_IMS_WIDTH);
    IPA_IMS |= ((destination_struct->image_width << 16U) | (destination_struct->image_height));

    if(SET == tempflag) {
        /* restore the state of TEN */
        IPA_CTL |= IPA_CTL_TEN;
    }
}

/*!
    \brief    initialize IPA foreground LUT parameters
    \param[in]  fg_lut_num: foreground LUT number of pixel
    \param[in]  fg_lut_pf: foreground LUT pixel format(IPA_LUT_PF_ARGB8888, IPA_LUT_PF_RGB888)
    \param[in]  fg_lut_addr: foreground LUT memory base address
    \param[out] none
    \retval     none
*/
void ipa_foreground_lut_init(uint8_t fg_lut_num, uint8_t fg_lut_pf, uint32_t fg_lut_addr)
{
    FlagStatus tempflag = RESET;
    if(RESET != (IPA_FPCTL & IPA_FPCTL_FLLEN)) {
        tempflag = SET;
        /* reset the FLLEN in order to configure the following bits */
        IPA_FPCTL &= ~IPA_FPCTL_FLLEN;
    }

    /* foreground LUT number of pixel configuration */
    IPA_FPCTL |= ((uint32_t)fg_lut_num << 8U);
    /* foreground LUT pixel format configuration */
    if(IPA_LUT_PF_RGB888 == fg_lut_pf) {
        IPA_FPCTL |= IPA_FPCTL_FLPF;
    } else {
        IPA_FPCTL &= ~(IPA_FPCTL_FLPF);
    }
    /* foreground LUT memory base address configuration */
    IPA_FLMADDR &= ~(IPA_FLMADDR_FLMADDR);
    IPA_FLMADDR = fg_lut_addr;

    if(SET == tempflag) {
        /* restore the state of FLLEN */
        IPA_FPCTL |= IPA_FPCTL_FLLEN;
    }
}

/*!
    \brief    initialize IPA background LUT parameters
    \param[in]  bg_lut_num: background LUT number of pixel
    \param[in]  bg_lut_pf: background LUT pixel format(IPA_LUT_PF_ARGB8888, IPA_LUT_PF_RGB888)
    \param[in]  bg_lut_addr: background LUT memory base address
    \param[out] none
    \retval     none
*/
void ipa_background_lut_init(uint8_t bg_lut_num, uint8_t bg_lut_pf, uint32_t bg_lut_addr)
{
    FlagStatus tempflag = RESET;
    if(RESET != (IPA_BPCTL & IPA_BPCTL_BLLEN)) {
        tempflag = SET;
        /* reset the BLLEN in order to configure the following bits */
        IPA_BPCTL &= ~IPA_BPCTL_BLLEN;
    }

    /* background LUT number of pixel configuration */
    IPA_BPCTL |= ((uint32_t)bg_lut_num << 8U);
    /* background LUT pixel format configuration */
    if(IPA_LUT_PF_RGB888 == bg_lut_pf) {
        IPA_BPCTL |= IPA_BPCTL_BLPF;
    } else {
        IPA_BPCTL &= ~(IPA_BPCTL_BLPF);
    }
    /* background LUT memory base address configuration */
    IPA_BLMADDR &= ~(IPA_BLMADDR_BLMADDR);
    IPA_BLMADDR = bg_lut_addr;

    if(SET == tempflag) {
        /* restore the state of BLLEN */
        IPA_BPCTL |= IPA_BPCTL_BLLEN;
    }
}

/*!
    \brief    configure IPA line mark
    \param[in]  line_num: line number
    \param[out] none
    \retval     none
*/
void ipa_line_mark_config(uint16_t line_num)
{
    IPA_LM &= ~(IPA_LM_LM);
    IPA_LM = line_num;
}

/*!
    \brief    inter-timer enable or disable
    \param[in]  timer_cfg: IPA_INTER_TIMER_ENABLE,IPA_INTER_TIMER_DISABLE
    \param[out] none
    \retval     none
*/
void ipa_inter_timer_config(uint8_t timer_cfg)
{
    if(IPA_INTER_TIMER_ENABLE == timer_cfg) {
        IPA_ITCTL |= IPA_ITCTL_ITEN;
    } else {
        IPA_ITCTL &= ~(IPA_ITCTL_ITEN);
    }
}

/*!
    \brief    configure the number of clock cycles interval
    \param[in]  clk_num: the number of clock cycles
    \param[out] none
    \retval     none
*/
void ipa_interval_clock_num_config(uint8_t clk_num)
{
    /* NCCI[7:0] bits have no meaning if ITEN is '0' */
    IPA_ITCTL &= ~(IPA_ITCTL_NCCI);
    IPA_ITCTL |= ((uint32_t)clk_num << 8U);
}

/*!
    \brief    get IPA flag status in IPA_INTF register
    \param[in]  flag: IPA flags
                one or more parameters can be selected which are shown as below:
      \arg        IPA_FLAG_TAE: transfer access error interrupt flag
      \arg        IPA_FLAG_FTF: full transfer finish interrupt flag
      \arg        IPA_FLAG_TLM: transfer line mark interrupt flag
      \arg        IPA_FLAG_LAC: LUT access conflict interrupt flag
      \arg        IPA_FLAG_LLF: LUT loading finish interrupt flag
      \arg        IPA_FLAG_WCF: wrong configuration interrupt flag
    \param[out] none
    \retval     none
*/
FlagStatus ipa_flag_get(uint32_t flag)
{
    FlagStatus reval = RESET;

    if(RESET != (IPA_INTF & flag)) {
        reval = SET;
    } else {
        reval = RESET;
    }

    return reval;
}

/*!
    \brief    clear IPA flag in IPA_INTF register
    \param[in]  flag: IPA flags
                one or more parameters can be selected which are shown as below:
      \arg        IPA_FLAG_TAE: transfer access error interrupt flag
      \arg        IPA_FLAG_FTF: full transfer finish interrupt flag
      \arg        IPA_FLAG_TLM: transfer line mark interrupt flag
      \arg        IPA_FLAG_LAC: LUT access conflict interrupt flag
      \arg        IPA_FLAG_LLF: LUT loading finish interrupt flag
      \arg        IPA_FLAG_WCF: wrong configuration interrupt flag
    \param[out] none
    \retval     none
*/
void ipa_flag_clear(uint32_t flag)
{
    IPA_INTC |= (flag);
}

/*!
    \brief    enable IPA interrupt
    \param[in]  int_flag: IPA interrupt flags
                one or more parameters can be selected which are shown as below:
      \arg        IPA_INT_TAE: transfer access error interrupt
      \arg        IPA_INT_FTF: full transfer finish interrupt
      \arg        IPA_INT_TLM: transfer line mark interrupt
      \arg        IPA_INT_LAC: LUT access conflict interrupt
      \arg        IPA_INT_LLF: LUT loading finish interrupt
      \arg        IPA_INT_WCF: wrong configuration interrupt
    \param[out] none
    \retval     none
*/
void ipa_interrupt_enable(uint32_t int_flag)
{
    IPA_CTL |= (int_flag);
}

/*!
    \brief    disable IPA interrupt
    \param[in]  int_flag: IPA interrupt flags
                one or more parameters can be selected which are shown as below:
      \arg        IPA_INT_TAE: transfer access error interrupt
      \arg        IPA_INT_FTF: full transfer finish interrupt
      \arg        IPA_INT_TLM: transfer line mark interrupt
      \arg        IPA_INT_LAC: LUT access conflict interrupt
      \arg        IPA_INT_LLF: LUT loading finish interrupt
      \arg        IPA_INT_WCF: wrong configuration interrupt
    \param[out] none
    \retval     none
*/
void ipa_interrupt_disable(uint32_t int_flag)
{
    IPA_CTL &= ~(int_flag);
}

/*!
    \brief    get IPA interrupt flag
    \param[in]  int_flag: IPA interrupt flag flags
                one or more parameters can be selected which are shown as below:
      \arg        IPA_INT_FLAG_TAE: transfer access error interrupt flag
      \arg        IPA_INT_FLAG_FTF: full transfer finish interrupt flag
      \arg        IPA_INT_FLAG_TLM: transfer line mark interrupt flag
      \arg        IPA_INT_FLAG_LAC: LUT access conflict interrupt flag
      \arg        IPA_INT_FLAG_LLF: LUT loading finish interrupt flag
      \arg        IPA_INT_FLAG_WCF: wrong configuration interrupt flag
    \param[out] none
    \retval     none
*/
FlagStatus ipa_interrupt_flag_get(uint32_t int_flag)
{
    FlagStatus reval = RESET;

    if(0U != (IPA_INTF & int_flag)) {
        reval = SET;
    } else {
        reval = RESET;
    }

    return reval;
}

/*!
    \brief    clear IPA interrupt flag
    \param[in]  int_flag: IPA interrupt flag flags
                one or more parameters can be selected which are shown as below:
      \arg        IPA_INT_FLAG_TAE: transfer access error interrupt flag
      \arg        IPA_INT_FLAG_FTF: full transfer finish interrupt flag
      \arg        IPA_INT_FLAG_TLM: transfer line mark interrupt flag
      \arg        IPA_INT_FLAG_LAC: LUT access conflict interrupt flag
      \arg        IPA_INT_FLAG_LLF: LUT loading finish interrupt flag
      \arg        IPA_INT_FLAG_WCF: wrong configuration interrupt flag
    \param[out] none
    \retval     none
*/
void ipa_interrupt_flag_clear(uint32_t int_flag)
{
    IPA_INTC |= (int_flag);
}
