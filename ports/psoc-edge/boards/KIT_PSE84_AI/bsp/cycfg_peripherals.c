/*******************************************************************************
 * File Name: cycfg_peripherals.c
 *
 * Description:
 * Analog configuration
 * This file was automatically generated and should not be modified.
 * Configurator Backend 3.60.0
 * device-db 4.34.0.9502
 * mtb-dsl-pse8xxgp 1.2.0.895
 *
 *******************************************************************************
 * Copyright 2026 Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/

#include "cycfg_peripherals.h"

#define CYBSP_PWM_LED_CTRL_INPUT_DISABLED 0x7U
#define emUSB_OS_Timer_INPUT_DISABLED 0x7U
#define CYBSP_GENERAL_PURPOSE_TIMER_INPUT_DISABLED 0x7U
#define CYBSP_PWM_DVP_CAM_CTRL_INPUT_DISABLED 0x7U
#define CYBSP_DEAD_TIME_PWM_INPUT_DISABLED 0x7U

cy_stc_autanalog_cfg_t autonomous_analog_cfg =
{
    .prb = NULL,
    .ac = &CYBSP_AUTONOMOUS_CONTROLLER_cfg,
    .ctb =
    {
        NULL, 
        NULL, 
    },
    .ptcomp =
    {
        NULL, 
    },
    .dac =
    {
        NULL, 
        NULL, 
    },
    .sar =
    {
        &CYBSP_SAR_ADC_cfg, 
    },
};
cy_stc_autanalog_stt_t autonomous_analog_stt[] =
{
    {
        .ac = &CYBSP_AUTONOMOUS_CONTROLLER_stt[0U],
        .prb = NULL,
        .ctb =
        {
            NULL, 
            NULL, 
        },
        .ptcomp =
        {
            NULL, 
        },
        .dac =
        {
            NULL, 
            NULL, 
        },
        .sar =
        {
            &CYBSP_SAR_ADC_stt[0U], 
        },
    },
    {
        .ac = &CYBSP_AUTONOMOUS_CONTROLLER_stt[1U],
        .prb = NULL,
        .ctb =
        {
            NULL, 
            NULL, 
        },
        .ptcomp =
        {
            NULL, 
        },
        .dac =
        {
            NULL, 
            NULL, 
        },
        .sar =
        {
            &CYBSP_SAR_ADC_stt[1U], 
        },
    },
    {
        .ac = &CYBSP_AUTONOMOUS_CONTROLLER_stt[2U],
        .prb = NULL,
        .ctb =
        {
            NULL, 
            NULL, 
        },
        .ptcomp =
        {
            NULL, 
        },
        .dac =
        {
            NULL, 
            NULL, 
        },
        .sar =
        {
            &CYBSP_SAR_ADC_stt[2U], 
        },
    },
};
cy_stc_autanalog_t autonomous_analog_init =
{
    .configuration = &autonomous_analog_cfg,
    .numSttEntries = sizeof(autonomous_analog_stt)/sizeof(autonomous_analog_stt[0U]),
    .stateTransitionTable = &autonomous_analog_stt[0U],
};
cy_en_autanalog_ac_out_trigger_mask_t CYBSP_AUTONOMOUS_CONTROLLER_out_trig_mask[] =
{
    CY_AUTANALOG_AC_OUT_TRIG_MASK_EMPTY, 
    CY_AUTANALOG_AC_OUT_TRIG_MASK_EMPTY, 
    CY_AUTANALOG_AC_OUT_TRIG_MASK_EMPTY, 
    CY_AUTANALOG_AC_OUT_TRIG_MASK_EMPTY, 
    CY_AUTANALOG_AC_OUT_TRIG_MASK_EMPTY, 
    CY_AUTANALOG_AC_OUT_TRIG_MASK_EMPTY, 
    CY_AUTANALOG_AC_OUT_TRIG_MASK_EMPTY, 
    CY_AUTANALOG_AC_OUT_TRIG_MASK_EMPTY, 
};
cy_stc_autanalog_ac_t CYBSP_AUTONOMOUS_CONTROLLER_cfg =
{
    .gpioOutEn =   CY_AUTANALOG_STT_AC_GPIO_OUT_DISABLED,
    .mask =
    {
        &CYBSP_AUTONOMOUS_CONTROLLER_out_trig_mask[0U], 
        &CYBSP_AUTONOMOUS_CONTROLLER_out_trig_mask[1U], 
        &CYBSP_AUTONOMOUS_CONTROLLER_out_trig_mask[2U], 
        &CYBSP_AUTONOMOUS_CONTROLLER_out_trig_mask[3U], 
        &CYBSP_AUTONOMOUS_CONTROLLER_out_trig_mask[4U], 
        &CYBSP_AUTONOMOUS_CONTROLLER_out_trig_mask[5U], 
        &CYBSP_AUTONOMOUS_CONTROLLER_out_trig_mask[6U], 
        &CYBSP_AUTONOMOUS_CONTROLLER_out_trig_mask[7U], 
    },
    .timer =
    {
        .enable = false,
        .clkSrc = CY_AUTANALOG_TIMER_CLK_LP,
        .period = 0U,
    },
};
cy_stc_autanalog_stt_ac_t CYBSP_AUTONOMOUS_CONTROLLER_stt[] =
{
    {
        .unlock = true,
        .lpMode = false,
        .condition = CY_AUTANALOG_STT_AC_CONDITION_BLOCK_READY,
        .action = CY_AUTANALOG_STT_AC_ACTION_WAIT_FOR,
        .branchState = 1U,
        .trigInt = false,
        .count = 0U,
        .unlockGpioOut = false,
        .gpioOut = CY_AUTANALOG_STT_AC_GPIO_OUT_DISABLED,
    },
    {
        .unlock = false,
        .lpMode = false,
        .condition = CY_AUTANALOG_STT_AC_CONDITION_SAR_DONE,
        .action = CY_AUTANALOG_STT_AC_ACTION_WAIT_FOR,
        .branchState = 0U,
        .trigInt = false,
        .count = 0U,
        .unlockGpioOut = false,
        .gpioOut = CY_AUTANALOG_STT_AC_GPIO_OUT_DISABLED,
    },
    {
        .unlock = false,
        .lpMode = false,
        .condition = CY_AUTANALOG_STT_AC_CONDITION_FALSE,
        .action = CY_AUTANALOG_STT_AC_ACTION_STOP,
        .branchState = 0U,
        .trigInt = false,
        .count = 0U,
        .unlockGpioOut = false,
        .gpioOut = CY_AUTANALOG_STT_AC_GPIO_OUT_DISABLED,
    },
};
cy_stc_autanalog_sar_hs_chan_t CYBSP_SAR_ADC_gpio_ch_cfg[] =
{
    {
        .posPin = CY_AUTANALOG_SAR_PIN_GPIO1,
        .hsDiffEn = false,
        .sign = false,
        .posCoeff = CY_AUTANALOG_SAR_CH_COEFF_DISABLED,
        .negPin = CY_AUTANALOG_SAR_PIN_GPIO0,
        .accShift = false,
        .negCoeff = CY_AUTANALOG_SAR_CH_COEFF_DISABLED,
        .hsLimit = CY_AUTANALOG_SAR_LIMIT_STATUS_DISABLED,
        .fifoSel = CY_AUTANALOG_FIFO_DISABLED,
    },
};
cy_stc_autanalog_sar_sta_hs_t CYBSP_SAR_ADC_sta_hs_cfg =
{
    .hsVref = CY_AUTANALOG_SAR_VREF_VDDA,
    .hsSampleTime =
    {
        31U, 
        31U, 
        31U, 
        31U, 
    },
    .hsGpioChan =
    {
        &CYBSP_SAR_ADC_gpio_ch_cfg[0U], 
        NULL, 
        NULL, 
        NULL, 
        NULL, 
        NULL, 
        NULL, 
        NULL, 
    },
    .hsGpioResultMask =   CY_AUTANALOG_SAR_CHAN_MASK_GPIO0,
};
cy_stc_autanalog_sar_sta_t CYBSP_SAR_ADC_sta_cfg =
{
    .lpStaCfg = NULL,
    .hsStaCfg = &CYBSP_SAR_ADC_sta_hs_cfg,
    .posBufPwr = CY_AUTANALOG_SAR_BUF_PWR_OFF,
    .negBufPwr = CY_AUTANALOG_SAR_BUF_PWR_OFF,
    .accMode = CY_AUTANALOG_SAR_ACC_DISABLED,
    .startupCal = CY_AUTANALOG_SAR_CAL_DISABLED,
    .chanID = false,
    .shiftMode = false,
    .intMuxChan =
    {
        NULL, 
        NULL, 
        NULL, 
        NULL, 
        NULL, 
        NULL, 
        NULL, 
        NULL, 
        NULL, 
        NULL, 
        NULL, 
        NULL, 
        NULL, 
        NULL, 
        NULL, 
        NULL, 
    },
    .limitCond =
    {
        NULL, 
        NULL, 
        NULL, 
        NULL, 
    },
    .muxResultMask =   CY_AUTANALOG_SAR_CHAN_MASK_MUX_DISABLED,
    .firResultMask =   CY_AUTANALOG_SAR_MASK_FIR_DISABLED,
};
cy_stc_autanalog_sar_seq_tab_hs_t CYBSP_SAR_ADC_seq_hs_cfg[] =
{
    {
        .chanEn = CY_AUTANALOG_SAR_CHAN_MASK_GPIO0,
        .muxMode = CY_AUTANALOG_SAR_CHAN_CFG_MUX_DISABLED,
        .mux0Sel = CY_AUTANALOG_SAR_CHAN_CFG_MUX0,
        .mux1Sel = CY_AUTANALOG_SAR_CHAN_CFG_MUX0,
        .sampleTimeEn = true,
        .sampleTime = CY_AUTANALOG_SAR_SAMPLE_TIME0,
        .accEn = false,
        .accCount = CY_AUTANALOG_SAR_ACC_CNT2,
        .calReq = CY_AUTANALOG_SAR_CAL_DISABLED,
        .nextAction = CY_AUTANALOG_SAR_NEXT_ACTION_GO_TO_ENTRY_ADDR,
    },
};
cy_stc_autanalog_sar_t CYBSP_SAR_ADC_cfg =
{
    .sarStaCfg = &CYBSP_SAR_ADC_sta_cfg,
    .hsSeqTabNum = sizeof(CYBSP_SAR_ADC_seq_hs_cfg)/sizeof(CYBSP_SAR_ADC_seq_hs_cfg[0U]),
    .hsSeqTabArr = &CYBSP_SAR_ADC_seq_hs_cfg[0U],
    .lpSeqTabNum = 0U,
    .lpSeqTabArr = NULL,
    .firNum = 0U,
    .firCfg = NULL,
    .fifoCfg = NULL,
};
cy_stc_autanalog_stt_sar_t CYBSP_SAR_ADC_stt[] =
{
    {
        .unlock = true,
        .enable = true,
        .trigger = false,
        .entryState = 0U,
    },
    {
        .unlock = true,
        .enable = true,
        .trigger = true,
        .entryState = 0U,
    },
    {
        .unlock = true,
        .enable = true,
        .trigger = false,
        .entryState = 0U,
    },
};

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_ADC)
mtb_hal_adc_configurator_t CYBSP_SAR_ADC_hal_config =
{
    .config = &CYBSP_SAR_ADC_cfg,
    .num_channels = sizeof(CYBSP_SAR_ADC_seq_hs_cfg)/sizeof(CYBSP_SAR_ADC_seq_hs_cfg[0U]),
    .clock = NULL,
    .adc_index = 0U,
};
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_ADC) */

const cy_stc_i3c_config_t CYBSP_I3C_CONTROLLER_config =
{
    .i3cMode = CY_I3C_CONTROLLER,
    .i3cBusMode = CY_I3C_BUS_PURE,
    .useDma = false,
    .manualDataRate = false,
    .i3cClockHz = 12500000U,
    .i3cSclRate = 12500000U,
    .openDrainSclRate = 4000000U,
    .txEmptyBufThld = CY_I3C_1_WORD_DEPTH,
    .rxBufThld = CY_I3C_1_WORD_DEPTH,
    .txBufStartThld = CY_I3C_1_WORD_DEPTH,
    .rxBufStartThld = CY_I3C_1_WORD_DEPTH,
    .ibaInclude = false,
    .hotJoinCtrl = false,
    .dynamicAddr = 8U,
    .cmdQueueEmptyThld = 1U,
    .respQueueThld = 0U,
    .ibiQueueThld = 0U,
    .ibiDataThld = 15U,
    .extLowCnt1 = 11U,
    .extLowCnt2 = 16U,
    .extLowCnt3 = 27U,
    .extLowCnt4 = 58U,
    .extTerminationLowCnt = 0U,
    .sdaHoldTime = 1U,
    .busFreeTime = 32U,
    .openDrainLowCnt = 0U,
    .openDrainHighCnt = 0U,
    .pushPullLowCnt = 0U,
    .pushPullHighCnt = 0U,
    .i2cFMLowCnt = 0U,
    .i2cFMHighCnt = 0U,
    .i2cFMPlusLowCnt = 0U,
    .i2cFMPlusHighCnt = 0U,
    .adaptiveI2CI3C = false,
    .staticAddress = 0U,
    .pid = 0U,
    .dcr = 0U,
    .speedLimit = false,
    .hdrCapable = false,
    .deviceRoleCap = CY_I3C_SECONDARY_CONTROLLER,
    .hotjoinEnable = false,
    .busAvailTime = 0U,
    .busIdleTime = 0U,
};
const cy_stc_pdm_pcm_config_v2_t CYBSP_PDM_config =
{
    .clkDiv = 7,
    .clksel = CY_PDM_PCM_SEL_SRSS_CLOCK,
    .halverate = CY_PDM_PCM_RATE_FULL,
    .route = 4,
    .fir0_coeff_user_value = false,
    .fir1_coeff_user_value = false,
};
const cy_stc_pdm_pcm_channel_config_t channel_2_config =
{
    .sampledelay = 1,
    .wordSize = CY_PDM_PCM_WSIZE_16_BIT,
    .signExtension = true,
    .rxFifoTriggerLevel = 31,
    .fir0_enable = false,
    .cic_decim_code = CY_PDM_PCM_CHAN_CIC_DECIM_32,
    .fir0_decim_code = CY_PDM_PCM_CHAN_FIR0_DECIM_1,
    .fir0_scale = 0,
    .fir1_decim_code = CY_PDM_PCM_CHAN_FIR1_DECIM_3,
    .fir1_scale = 10,
    .dc_block_disable = false,
    .dc_block_code = CY_PDM_PCM_CHAN_DCBLOCK_CODE_16,
};
const cy_stc_pdm_pcm_channel_config_t channel_3_config =
{
    .sampledelay = 5,
    .wordSize = CY_PDM_PCM_WSIZE_16_BIT,
    .signExtension = true,
    .rxFifoTriggerLevel = 31,
    .fir0_enable = false,
    .cic_decim_code = CY_PDM_PCM_CHAN_CIC_DECIM_32,
    .fir0_decim_code = CY_PDM_PCM_CHAN_FIR0_DECIM_1,
    .fir0_scale = 0,
    .fir1_decim_code = CY_PDM_PCM_CHAN_FIR1_DECIM_3,
    .fir1_scale = 10,
    .dc_block_disable = false,
    .dc_block_code = CY_PDM_PCM_CHAN_DCBLOCK_CODE_16,
};
const cy_stc_scb_i2c_config_t CYBSP_I2C_CONTROLLER_config =
{
    .i2cMode = CY_SCB_I2C_MASTER,
    .useRxFifo = true,
    .useTxFifo = true,
    .slaveAddress = 0U,
    .slaveAddressMask = 0U,
    .acceptAddrInFifo = false,
    .ackGeneralAddr = false,
    .enableWakeFromSleep = false,
    .enableDigitalFilter = false,
    .lowPhaseDutyCycle = 16,
    .highPhaseDutyCycle = 9,
};

#if defined (COMPONENT_MTB_HAL)
const mtb_hal_peri_div_t CYBSP_I2C_CONTROLLER_clock_ref =
{
    .clk_dst = (en_clk_dst_t)CYBSP_I2C_CONTROLLER_CLK_DIV_GRP_NUM,
    .div_type = CYBSP_I2C_CONTROLLER_CLK_DIV_HW,
    .div_num = CYBSP_I2C_CONTROLLER_CLK_DIV_NUM,
};
const mtb_hal_clock_t CYBSP_I2C_CONTROLLER_hal_clock =
{
    .clock_ref = &CYBSP_I2C_CONTROLLER_clock_ref,
    .interface = &mtb_hal_clock_peri_interface,
};
#endif /* defined (COMPONENT_MTB_HAL) */

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_I2C)
const mtb_hal_i2c_configurator_t CYBSP_I2C_CONTROLLER_hal_config =
{
    .base = CYBSP_I2C_CONTROLLER_HW,
    .config = &CYBSP_I2C_CONTROLLER_config,
    .clock = &CYBSP_I2C_CONTROLLER_hal_clock,
};
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_I2C) */

const cy_stc_scb_uart_config_t CYBSP_DEBUG_UART_config =
{
    .uartMode = CY_SCB_UART_STANDARD,
    .enableMultiProcessorMode = false,
    .smartCardRetryOnNack = false,
    .irdaInvertRx = false,
    .irdaEnableLowPowerReceiver = false,
    .oversample = 10,
    .enableMsbFirst = false,
    .dataWidth = 8UL,
    .parity = CY_SCB_UART_PARITY_NONE,
    .stopBits = CY_SCB_UART_STOP_BITS_1,
    .enableInputFilter = false,
    .breakWidth = 11UL,
    .dropOnFrameError = false,
    .dropOnParityError = false,
    .breaklevel = false,
    .receiverAddress = 0x0UL,
    .receiverAddressMask = 0x0UL,
    .acceptAddrInFifo = false,
    .enableCts = false,
    .ctsPolarity = CY_SCB_UART_ACTIVE_LOW,
    .rtsRxFifoLevel = 0UL,
    .rtsPolarity = CY_SCB_UART_ACTIVE_LOW,
    .rxFifoTriggerLevel = 63UL,
    .rxFifoIntEnableMask = 0UL,
    .txFifoTriggerLevel = 63UL,
    .txFifoIntEnableMask = 0UL,
};

#if defined (COMPONENT_MTB_HAL)
const mtb_hal_peri_div_t CYBSP_DEBUG_UART_clock_ref =
{
    .clk_dst = (en_clk_dst_t)CYBSP_DEBUG_UART_CLK_DIV_GRP_NUM,
    .div_type = CYBSP_DEBUG_UART_CLK_DIV_HW,
    .div_num = CYBSP_DEBUG_UART_CLK_DIV_NUM,
};
const mtb_hal_clock_t CYBSP_DEBUG_UART_hal_clock =
{
    .clock_ref = &CYBSP_DEBUG_UART_clock_ref,
    .interface = &mtb_hal_clock_peri_interface,
};
#endif /* defined (COMPONENT_MTB_HAL) */

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_UART)
const mtb_hal_uart_configurator_t CYBSP_DEBUG_UART_hal_config =
{
    .base = CYBSP_DEBUG_UART_HW,
    .clock = &CYBSP_DEBUG_UART_hal_clock,
    .tx_pin = 7,
#if defined (COMPONENT_MW_ASYNC_TRANSFER)
    .rts_pin = 0xFF,
#endif /* defined (COMPONENT_MW_ASYNC_TRANSFER) */
    .tx_port = 6,
#if defined (COMPONENT_MW_ASYNC_TRANSFER)
    .rts_port = 0xFF,
    .rts_enable = 0UL,
#endif /* defined (COMPONENT_MW_ASYNC_TRANSFER) */
};
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_UART) */

const cy_stc_scb_spi_config_t CYBSP_SPI_CONTROLLER_config =
{
    .spiMode = CY_SCB_SPI_MASTER,
    .subMode = CY_SCB_SPI_MOTOROLA,
    .sclkMode = CY_SCB_SPI_CPHA0_CPOL0,
    .parity = CY_SCB_SPI_PARITY_NONE,
    .dropOnParityError = false,
    .oversample = 4,
    .rxDataWidth = 8UL,
    .txDataWidth = 8UL,
    .enableMsbFirst = true,
    .enableInputFilter = false,
    .enableFreeRunSclk = false,
    .enableMisoLateSample = true,
    .enableTransferSeparation = false,
    .ssPolarity = ((CY_SCB_SPI_ACTIVE_LOW << CY_SCB_SPI_SLAVE_SELECT0) | \
                                         (CY_SCB_SPI_ACTIVE_LOW << CY_SCB_SPI_SLAVE_SELECT1) | \
                                         (CY_SCB_SPI_ACTIVE_LOW << CY_SCB_SPI_SLAVE_SELECT2) | \
                                         (CY_SCB_SPI_ACTIVE_LOW << CY_SCB_SPI_SLAVE_SELECT3)),
    .ssSetupDelay = false,
    .ssHoldDelay = false,
    .ssInterFrameDelay = false,
    .enableWakeFromSleep = false,
    .rxFifoTriggerLevel = 63UL,
    .rxFifoIntEnableMask = 0UL,
    .txFifoTriggerLevel = 63UL,
    .txFifoIntEnableMask = 0UL,
    .masterSlaveIntEnableMask = 0UL,
};

#if defined (COMPONENT_MTB_HAL)
const mtb_hal_peri_div_t CYBSP_SPI_CONTROLLER_clock_ref =
{
    .clk_dst = (en_clk_dst_t)PCLK_SCB3_CLOCK_SCB_EN,
    .div_type = CY_SYSCLK_DIV_8_BIT,
    .div_num = 1,
};
const mtb_hal_clock_t CYBSP_SPI_CONTROLLER_hal_clock =
{
    .clock_ref = &CYBSP_SPI_CONTROLLER_clock_ref,
    .interface = &mtb_hal_clock_peri_interface,
};
#endif /* defined (COMPONENT_MTB_HAL) */

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_SPI)
const mtb_hal_spi_configurator_t CYBSP_SPI_CONTROLLER_hal_config =
{
    .base = CYBSP_SPI_CONTROLLER_HW,
    .clock = &CYBSP_SPI_CONTROLLER_hal_clock,
    .config = &CYBSP_SPI_CONTROLLER_config,
};
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_SPI) */

const cy_stc_scb_uart_config_t CYBSP_BT_UART_config =
{
    .uartMode = CY_SCB_UART_STANDARD,
    .enableMultiProcessorMode = false,
    .smartCardRetryOnNack = false,
    .irdaInvertRx = false,
    .irdaEnableLowPowerReceiver = false,
    .oversample = 8,
    .enableMsbFirst = false,
    .dataWidth = 8UL,
    .parity = CY_SCB_UART_PARITY_NONE,
    .stopBits = CY_SCB_UART_STOP_BITS_1,
    .enableInputFilter = false,
    .breakWidth = 11UL,
    .dropOnFrameError = false,
    .dropOnParityError = false,
    .breaklevel = false,
    .receiverAddress = 0x0UL,
    .receiverAddressMask = 0x0UL,
    .acceptAddrInFifo = false,
    .enableCts = true,
    .ctsPolarity = CY_SCB_UART_ACTIVE_LOW,
    .rtsRxFifoLevel = 63,
    .rtsPolarity = CY_SCB_UART_ACTIVE_LOW,
    .rxFifoTriggerLevel = 63UL,
    .rxFifoIntEnableMask = 0UL,
    .txFifoTriggerLevel = 63UL,
    .txFifoIntEnableMask = 0UL,
};

#if defined (COMPONENT_MTB_HAL)
const mtb_hal_peri_div_t CYBSP_BT_UART_clock_ref =
{
    .clk_dst = (en_clk_dst_t)CYBSP_BT_UART_CLK_DIV_GRP_NUM,
    .div_type = CYBSP_BT_UART_CLK_DIV_HW,
    .div_num = CYBSP_BT_UART_CLK_DIV_NUM,
};
const mtb_hal_clock_t CYBSP_BT_UART_hal_clock =
{
    .clock_ref = &CYBSP_BT_UART_clock_ref,
    .interface = &mtb_hal_clock_peri_interface,
};
#endif /* defined (COMPONENT_MTB_HAL) */

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_UART)
const mtb_hal_uart_configurator_t CYBSP_BT_UART_hal_config =
{
    .base = CYBSP_BT_UART_HW,
    .clock = &CYBSP_BT_UART_hal_clock,
    .tx_pin = 1,
#if defined (COMPONENT_MW_ASYNC_TRANSFER)
    .rts_pin = 3,
#endif /* defined (COMPONENT_MW_ASYNC_TRANSFER) */
    .tx_port = 10,
#if defined (COMPONENT_MW_ASYNC_TRANSFER)
    .rts_port = 10,
    .rts_enable = 1UL,
#endif /* defined (COMPONENT_MW_ASYNC_TRANSFER) */
};
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_UART) */

const cy_stc_scb_i2c_config_t CYBSP_I2C_CAM_CONTROLLER_config =
{
    .i2cMode = CY_SCB_I2C_MASTER_SLAVE,
    .useRxFifo = true,
    .useTxFifo = true,
    .slaveAddress = 8,
    .slaveAddressMask = 254,
    .acceptAddrInFifo = false,
    .ackGeneralAddr = false,
    .enableWakeFromSleep = false,
    .enableDigitalFilter = false,
    .lowPhaseDutyCycle = 0,
    .highPhaseDutyCycle = 0,
};

#if defined (COMPONENT_MTB_HAL)
const mtb_hal_peri_div_t CYBSP_I2C_CAM_CONTROLLER_clock_ref =
{
    .clk_dst = (en_clk_dst_t)CYBSP_I2C_CONTROLLER_3V3_CLK_DIV_GRP_NUM,
    .div_type = CYBSP_I2C_CONTROLLER_3V3_CLK_DIV_HW,
    .div_num = CYBSP_I2C_CONTROLLER_3V3_CLK_DIV_NUM,
};
const mtb_hal_clock_t CYBSP_I2C_CAM_CONTROLLER_hal_clock =
{
    .clock_ref = &CYBSP_I2C_CAM_CONTROLLER_clock_ref,
    .interface = &mtb_hal_clock_peri_interface,
};
#endif /* defined (COMPONENT_MTB_HAL) */

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_I2C)
const mtb_hal_i2c_configurator_t CYBSP_I2C_CAM_CONTROLLER_hal_config =
{
    .base = CYBSP_I2C_CAM_CONTROLLER_HW,
    .config = &CYBSP_I2C_CAM_CONTROLLER_config,
    .clock = &CYBSP_I2C_CAM_CONTROLLER_hal_clock,
};
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_I2C) */

cy_en_sd_host_card_capacity_t CYBSP_WIFI_SDIO_cardCapacity = CY_SD_HOST_SDSC;
cy_en_sd_host_card_type_t CYBSP_WIFI_SDIO_cardType = CY_SD_HOST_NOT_EMMC;
uint32_t CYBSP_WIFI_SDIO_rca = 0u;
const cy_stc_sd_host_init_config_t CYBSP_WIFI_SDIO_config =
{
    .emmc = false,
    .dmaType = CY_SD_HOST_DMA_ADMA2,
    .enableLedControl = false,
};
cy_stc_sd_host_sd_card_config_t CYBSP_WIFI_SDIO_card_cfg =
{
    .lowVoltageSignaling = false,
    .busWidth = CY_SD_HOST_BUS_WIDTH_4_BIT,
    .cardType = &CYBSP_WIFI_SDIO_cardType,
    .rca = &CYBSP_WIFI_SDIO_rca,
    .cardCapacity = &CYBSP_WIFI_SDIO_cardCapacity,
};

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_SDHC)
const mtb_hal_peri_div_t CYBSP_WIFI_SDIO_clock_ref =
{
    .clk_dst = (en_clk_dst_t)CYBSP_SDHC_0_CLK_DIV_GRP_NUM,
    .div_type = CYBSP_SDHC_0_CLK_DIV_HW,
    .div_num = CYBSP_SDHC_0_CLK_DIV_NUM,
};
const mtb_hal_clock_t CYBSP_WIFI_SDIO_hal_clock =
{
    .clock_ref = &CYBSP_WIFI_SDIO_clock_ref,
    .interface = &mtb_hal_clock_peri_interface,
};
const mtb_hal_sdhc_configurator_t CYBSP_WIFI_SDIO_sdhc_hal_config =
{
    .base = CYBSP_WIFI_SDIO_HW,
    .host_config = &CYBSP_WIFI_SDIO_config,
    .card_config = &CYBSP_WIFI_SDIO_card_cfg,
    .clock = &CYBSP_WIFI_SDIO_hal_clock,
    .gpios = {.clk = { .pin_num = 0, .port_num = 12}, .cmd = { .pin_num = 0, .port_num = 21}, .data = {{ .pin_num = 1, .port_num = 12}, { .pin_num = 2, .port_num = 12}, { .pin_num = 4, .port_num = 12}, { .pin_num = 5, .port_num = 12}, { .pin_num = NC, .port_num = NC}, { .pin_num = NC, .port_num = NC}, { .pin_num = NC, .port_num = NC}, { .pin_num = NC, .port_num = NC}}},
};
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_SDHC) */

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_SDIO)
const mtb_hal_sdio_configurator_t CYBSP_WIFI_SDIO_sdio_hal_config =
{
    .base = CYBSP_WIFI_SDIO_HW,
    .host_config = &CYBSP_WIFI_SDIO_config,
    .clock = &CYBSP_WIFI_SDIO_hal_clock,
    .gpios = {.clk = { .pin_num = 0, .port_num = 12}, .cmd = { .pin_num = 0, .port_num = 21}, .data = {{ .pin_num = 1, .port_num = 12}, { .pin_num = 2, .port_num = 12}, { .pin_num = 4, .port_num = 12}, { .pin_num = 5, .port_num = 12}}},
};
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_SDIO) */

const cy_stc_smif_config_t CYBSP_SMIF_CORE_0_XSPI_FLASH_config =
{
    .mode = (uint32_t)CY_SMIF_NORMAL,
    .deselectDelay = CYBSP_SMIF_CORE_0_XSPI_FLASH_DESELECT_DELAY,
    .blockEvent = (uint32_t)CY_SMIF_BUS_ERROR,
    .inputFrequencyMHz = 200,
    .enable_internal_dll = false,
    .dll_divider_value = CY_SMIF_DLL_DIVIDE_BY_2,
    .rx_capture_mode = CY_SMIF_SEL_NORMAL_SPI,
    .mdl_tap = CY_SMIF_MDL_8_TAP_DELAY,
    .device0_sdl_tap = CY_SMIF_SDL_8_TAP_DELAY,
    .device1_sdl_tap = CY_SMIF_SDL_8_TAP_DELAY,
    .device2_sdl_tap = CY_SMIF_SDL_8_TAP_DELAY,
    .device3_sdl_tap = CY_SMIF_SDL_8_TAP_DELAY,
    .tx_sdr_extra = CY_SMIF_TX_TWO_PERIOD_AHEAD,
};

#if defined (COMPONENT_MTB_HAL)
const mtb_hal_hf_clock_t CYBSP_SMIF_CORE_0_XSPI_FLASH_clock_ref =
{
    .inst_num = 3U,
};
const mtb_hal_clock_t CYBSP_SMIF_CORE_0_XSPI_FLASH_hal_clock =
{
    .clock_ref = &CYBSP_SMIF_CORE_0_XSPI_FLASH_clock_ref,
    .interface = &mtb_hal_clock_hf_interface,
};
#endif /* defined (COMPONENT_MTB_HAL) */

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_MEMORYSPI)
const mtb_hal_memoryspi_configurator_t CYBSP_SMIF_CORE_0_XSPI_FLASH_hal_config =
{
    .base = SMIF0_CORE,
    .clock = &CYBSP_SMIF_CORE_0_XSPI_FLASH_hal_clock,
    .config = &CYBSP_SMIF_CORE_0_XSPI_FLASH_config,
    .csel = {false, true, false, false},
    .irqs = CYBSP_SMIF_CORE_0_XSPI_FLASH_MEMORY_MODE_ALIGMENT_ERROR << 5 | CYBSP_SMIF_CORE_0_XSPI_FLASH_RX_DATA_FIFO_UNDERFLOW << 4 | CYBSP_SMIF_CORE_0_XSPI_FLASH_TX_COMMAND_FIFO_OVERFLOW << 3 | CYBSP_SMIF_CORE_0_XSPI_FLASH_TX_DATA_FIFO_OVERFLOW << 2 | (uint8_t)(CYBSP_SMIF_CORE_0_XSPI_FLASH_RX_FIFO_TRIGGER_LEVEL != 0) << 1 | (uint8_t)(CYBSP_SMIF_CORE_0_XSPI_FLASH_TX_FIFO_TRIGGER_LEVEL != 0) << 0,
    .dmas = CYBSP_SMIF_CORE_0_XSPI_FLASH_RX_DMA_TRIGGER_OUT_USED << 1 | CYBSP_SMIF_CORE_0_XSPI_FLASH_TX_DMA_TRIGGER_OUT_USED << 0,
};
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_MEMORYSPI) */

const cy_stc_smif_config_t CYBSP_SMIF_CORE_1_PSRAM_config =
{
    .mode = (uint32_t)CY_SMIF_NORMAL,
    .deselectDelay = CYBSP_SMIF_CORE_1_PSRAM_DESELECT_DELAY,
    .blockEvent = (uint32_t)CY_SMIF_BUS_ERROR,
    .inputFrequencyMHz = 400,
    .enable_internal_dll = false,
    .dll_divider_value = CY_SMIF_DLL_DIVIDE_BY_2,
    .rx_capture_mode = CY_SMIF_SEL_NORMAL_SPI,
    .mdl_tap = CY_SMIF_MDL_8_TAP_DELAY,
    .device0_sdl_tap = CY_SMIF_SDL_8_TAP_DELAY,
    .device1_sdl_tap = CY_SMIF_SDL_8_TAP_DELAY,
    .device2_sdl_tap = CY_SMIF_SDL_8_TAP_DELAY,
    .device3_sdl_tap = CY_SMIF_SDL_8_TAP_DELAY,
    .tx_sdr_extra = CY_SMIF_TX_TWO_PERIOD_AHEAD,
};

#if defined (COMPONENT_MTB_HAL)
const mtb_hal_hf_clock_t CYBSP_SMIF_CORE_1_PSRAM_clock_ref =
{
    .inst_num = 4U,
};
const mtb_hal_clock_t CYBSP_SMIF_CORE_1_PSRAM_hal_clock =
{
    .clock_ref = &CYBSP_SMIF_CORE_1_PSRAM_clock_ref,
    .interface = &mtb_hal_clock_hf_interface,
};
#endif /* defined (COMPONENT_MTB_HAL) */

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_MEMORYSPI)
const mtb_hal_memoryspi_configurator_t CYBSP_SMIF_CORE_1_PSRAM_hal_config =
{
    .base = SMIF1_CORE,
    .clock = &CYBSP_SMIF_CORE_1_PSRAM_hal_clock,
    .config = &CYBSP_SMIF_CORE_1_PSRAM_config,
    .csel = {false, false, true, false},
    .irqs = CYBSP_SMIF_CORE_1_PSRAM_MEMORY_MODE_ALIGMENT_ERROR << 5 | CYBSP_SMIF_CORE_1_PSRAM_RX_DATA_FIFO_UNDERFLOW << 4 | CYBSP_SMIF_CORE_1_PSRAM_TX_COMMAND_FIFO_OVERFLOW << 3 | CYBSP_SMIF_CORE_1_PSRAM_TX_DATA_FIFO_OVERFLOW << 2 | (uint8_t)(CYBSP_SMIF_CORE_1_PSRAM_RX_FIFO_TRIGGER_LEVEL != 0) << 1 | (uint8_t)(CYBSP_SMIF_CORE_1_PSRAM_TX_FIFO_TRIGGER_LEVEL != 0) << 0,
    .dmas = CYBSP_SMIF_CORE_1_PSRAM_RX_DMA_TRIGGER_OUT_USED << 1 | CYBSP_SMIF_CORE_1_PSRAM_TX_DMA_TRIGGER_OUT_USED << 0,
};
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_MEMORYSPI) */

const cy_stc_mcwdt_config_t CYBSP_CM33_LPTIMER_0_config =
{
    .c0Match = 32768U,
    .c1Match = 32768U,
    .c0Mode = CY_MCWDT_MODE_INT,
    .c1Mode = CY_MCWDT_MODE_INT,
    .c2ToggleBit = 0U,
    .c2Mode = CY_MCWDT_MODE_NONE,
    .c0ClearOnMatch = false,
    .c1ClearOnMatch = false,
    .c0c1Cascade = true,
    .c1c2Cascade = false,
    .c0LowerLimitMode = CY_MCWDT_LOWER_LIMIT_MODE_NOTHING,
    .c0LowerLimit = 0U,
    .c1LowerLimitMode = CY_MCWDT_LOWER_LIMIT_MODE_NOTHING,
    .c1LowerLimit = 0U,
};

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_LPTIMER)
const mtb_hal_lptimer_configurator_t CYBSP_CM33_LPTIMER_0_hal_config =
{
    .base = CYBSP_CM33_LPTIMER_0_HW,
    .lfclk_freqhz = 32768,
};
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_LPTIMER) */

const cy_stc_mcwdt_config_t CYBSP_CM55_LPTIMER_1_config =
{
    .c0Match = 32768U,
    .c1Match = 32768U,
    .c0Mode = CY_MCWDT_MODE_INT,
    .c1Mode = CY_MCWDT_MODE_INT,
    .c2ToggleBit = 0U,
    .c2Mode = CY_MCWDT_MODE_NONE,
    .c0ClearOnMatch = false,
    .c1ClearOnMatch = false,
    .c0c1Cascade = true,
    .c1c2Cascade = false,
    .c0LowerLimitMode = CY_MCWDT_LOWER_LIMIT_MODE_NOTHING,
    .c0LowerLimit = 0U,
    .c1LowerLimitMode = CY_MCWDT_LOWER_LIMIT_MODE_NOTHING,
    .c1LowerLimit = 0U,
};

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_LPTIMER)
const mtb_hal_lptimer_configurator_t CYBSP_CM55_LPTIMER_1_hal_config =
{
    .base = CYBSP_CM55_LPTIMER_1_HW,
    .lfclk_freqhz = 32768,
};
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_LPTIMER) */

cy_stc_tdm_config_tx_t CYBSP_TDM_CONTROLLER_0_tx_config =
{
    .enable = true,
    .masterMode = CY_TDM_DEVICE_MASTER,
    .wordSize = CY_TDM_SIZE_16,
    .format = CY_TDM_LEFT_DELAYED,
    .clkDiv = 4,
    .clkSel = CY_TDM_SEL_SRSS_CLK0,
    .sckPolarity = CY_TDM_CLK,
    .fsyncPolarity = CY_TDM_SIGN_INVERTED,
    .fsyncFormat = CY_TDM_CH_PERIOD,
    .channelNum = 2,
    .channelSize = 16,
    .fifoTriggerLevel = 64,
    .chEn = 0x3,
    .signalInput = 0,
    .i2sMode = true,
};
cy_stc_tdm_config_rx_t CYBSP_TDM_CONTROLLER_0_rx_config =
{
    .enable = false,
    .masterMode = CY_TDM_DEVICE_SLAVE,
    .wordSize = CY_TDM_SIZE_16,
    .signExtend = CY_ZERO_EXTEND,
    .format = CY_TDM_LEFT_DELAYED,
    .clkDiv = 16,
    .clkSel = CY_TDM_SEL_SRSS_CLK0,
    .sckPolarity = CY_TDM_CLK,
    .fsyncPolarity = CY_TDM_SIGN,
    .lateSample = false,
    .fsyncFormat = CY_TDM_BIT_PERIOD,
    .channelNum = 2,
    .channelSize = 16,
    .fifoTriggerLevel = 0,
    .chEn = 0x3,
    .signalInput = 0,
    .i2sMode = true,
};
const cy_stc_tdm_config_t CYBSP_TDM_CONTROLLER_0_config =
{
    .tx_config = &CYBSP_TDM_CONTROLLER_0_tx_config,
    .rx_config = &CYBSP_TDM_CONTROLLER_0_rx_config,
};
const cy_stc_tcpwm_pwm_config_t CYBSP_PWM_LED_CTRL_config =
{
    .pwmMode = CY_TCPWM_PWM_MODE_PWM,
    .clockPrescaler = CY_TCPWM_PWM_PRESCALER_DIVBY_1,
    .pwmAlignment = CY_TCPWM_PWM_LEFT_ALIGN,
    .deadTimeClocks = 0,
    .runMode = CY_TCPWM_PWM_CONTINUOUS,
    .period0 = 2000,
    .period1 = 32768,
    .enablePeriodSwap = false,
    .compare0 = 1000,
    .compare1 = 16384,
    .enableCompareSwap = false,
    .interruptSources = (CY_TCPWM_INT_ON_TC & 0U) | (CY_TCPWM_INT_ON_CC0 & 0U) | (CY_TCPWM_INT_ON_CC1 & 0U),
    .invertPWMOut = CY_TCPWM_PWM_INVERT_DISABLE,
    .invertPWMOutN = CY_TCPWM_PWM_INVERT_ENABLE,
    .killMode = CY_TCPWM_PWM_ASYNC_KILL,
    .swapInputMode = CYBSP_PWM_LED_CTRL_INPUT_DISABLED & 0x3U,
    .swapInput = CY_TCPWM_INPUT_0,
    .reloadInputMode = CYBSP_PWM_LED_CTRL_INPUT_DISABLED & 0x3U,
    .reloadInput = CY_TCPWM_INPUT_0,
    .startInputMode = CYBSP_PWM_LED_CTRL_INPUT_DISABLED & 0x3U,
    .startInput = CY_TCPWM_INPUT_0,
    .killInputMode = CYBSP_PWM_LED_CTRL_INPUT_DISABLED & 0x3U,
    .killInput = CY_TCPWM_INPUT_0,
    .countInputMode = CYBSP_PWM_LED_CTRL_INPUT_DISABLED & 0x3U,
    .countInput = CY_TCPWM_INPUT_1,
    .swapOverflowUnderflow = false,
    .immediateKill = false,
    .tapsEnabled = 45,
    .compare2 = CY_TCPWM_GRP_CNT_CC0_DEFAULT,
    .compare3 = CY_TCPWM_GRP_CNT_CC0_BUFF_DEFAULT,
    .enableCompare1Swap = false,
    .compare0MatchUp = true,
    .compare0MatchDown = false,
    .compare1MatchUp = true,
    .compare1MatchDown = false,
    .kill1InputMode = CYBSP_PWM_LED_CTRL_INPUT_DISABLED & 0x3U,
    .kill1Input = CY_TCPWM_INPUT_0,
    .pwmOnDisable = CY_TCPWM_PWM_OUTPUT_HIGHZ,
    .trigger0Event = CY_TCPWM_CNT_TRIGGER_ON_DISABLED,
    .trigger1Event = CY_TCPWM_CNT_TRIGGER_ON_DISABLED,
    .reloadLineSelect = false,
    .line_out_sel = CY_TCPWM_OUTPUT_PWM_SIGNAL,
    .linecompl_out_sel = CY_TCPWM_OUTPUT_INVERTED_PWM_SIGNAL,
    .line_out_sel_buff = CY_TCPWM_OUTPUT_PWM_SIGNAL,
    .linecompl_out_sel_buff = CY_TCPWM_OUTPUT_INVERTED_PWM_SIGNAL,
    .deadTimeClocks_linecompl_out = 0,
#if defined (CY_IP_MXS40TCPWM)
    .hrpwm_enable = false,
    .hrpwm_input_freq = CY_TCPWM_HRPWM_FREQ_80MHZ_OR_100MHZ,
    .kill_line_polarity = CY_TCPWM_LINEOUT_AND_LINECMPOUT_IS_LOW,
    .deadTimeClocksBuff = 0,
    .deadTimeClocksBuff_linecompl_out = 0,
    .buffer_swap_enable = false,
    .glitch_filter_enable = false,
    .gf_depth = CY_GLITCH_FILTER_DEPTH_SUPPORT_VALUE_0,
    .dithering_mode = CY_TCPWM_DITHERING_DISABLE,
    .period_dithering_value = 128,
    .duty_dithering_value = 128,
    .limiter = CY_TCPWM_DITHERING_LIMITER_7,
    .pwm_tc_sync_kill_dt = false,
    .pwm_sync_kill_dt = false,
#endif /* defined (CY_IP_MXS40TCPWM) */
};

#if defined (COMPONENT_MTB_HAL)
const mtb_hal_peri_div_t CYBSP_PWM_LED_CTRL_clock_ref =
{
    .clk_dst = (en_clk_dst_t)PCLK_TCPWM0_CLOCK_COUNTER_EN0,
    .div_type = CY_SYSCLK_DIV_16_BIT,
    .div_num = 3,
};
const mtb_hal_clock_t CYBSP_PWM_LED_CTRL_hal_clock =
{
    .clock_ref = &CYBSP_PWM_LED_CTRL_clock_ref,
    .interface = &mtb_hal_clock_peri_interface,
};
#endif /* defined (COMPONENT_MTB_HAL) */

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_PWM)
const mtb_hal_pwm_configurator_t CYBSP_PWM_LED_CTRL_hal_config =
{
    .base = CYBSP_PWM_LED_CTRL_HW,
    .clock = &CYBSP_PWM_LED_CTRL_hal_clock,
    .group = 0UL,
    .cntnum = 0UL,
    .max_count = 2000,
};
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_PWM) */

cy_stc_tcpwm_counter_config_t emUSB_OS_Timer_config =
{
    .period = 99999,
    .clockPrescaler = CY_TCPWM_COUNTER_PRESCALER_DIVBY_1,
    .runMode = CY_TCPWM_COUNTER_CONTINUOUS,
    .countDirection = CY_TCPWM_COUNTER_COUNT_UP,
    .compareOrCapture = CY_TCPWM_COUNTER_MODE_CAPTURE,
    .compare0 = 16384U,
    .compare1 = 16384U,
    .enableCompareSwap = false,
    .interruptSources = (CY_TCPWM_INT_ON_TC & 0U) | (CY_TCPWM_INT_ON_CC0) | (CY_TCPWM_INT_ON_CC1 & 0U),
    .captureInputMode = emUSB_OS_Timer_INPUT_DISABLED & 0x3U,
    .captureInput = CY_TCPWM_INPUT_0,
    .reloadInputMode = emUSB_OS_Timer_INPUT_DISABLED & 0x3U,
    .reloadInput = CY_TCPWM_INPUT_0,
    .startInputMode = emUSB_OS_Timer_INPUT_DISABLED & 0x3U,
    .startInput = CY_TCPWM_INPUT_0,
    .stopInputMode = emUSB_OS_Timer_INPUT_DISABLED & 0x3U,
    .stopInput = CY_TCPWM_INPUT_0,
    .countInputMode = emUSB_OS_Timer_INPUT_DISABLED & 0x3U,
    .countInput = CY_TCPWM_INPUT_1,
    .capture1InputMode = emUSB_OS_Timer_INPUT_DISABLED & 0x3U,
    .capture1Input = CY_TCPWM_INPUT_0,
    .compare2 = CY_TCPWM_GRP_CNT_CC0_DEFAULT,
    .compare3 = CY_TCPWM_GRP_CNT_CC0_BUFF_DEFAULT,
    .enableCompare1Swap = false,
    .trigger0Event = CY_TCPWM_CNT_TRIGGER_ON_DISABLED,
    .trigger1Event = CY_TCPWM_CNT_TRIGGER_ON_DISABLED,
};

#if defined (COMPONENT_MTB_HAL)
mtb_hal_peri_div_t emUSB_OS_Timer_clock_ref =
{
    .clk_dst = (en_clk_dst_t)PCLK_TCPWM0_CLOCK_COUNTER_EN1,
    .div_type = CY_SYSCLK_DIV_8_BIT,
    .div_num = 2,
};
mtb_hal_clock_t emUSB_OS_Timer_hal_clock =
{
    .clock_ref = &emUSB_OS_Timer_clock_ref,
    .interface = &mtb_hal_clock_peri_interface,
};
#endif /* defined (COMPONENT_MTB_HAL) */

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_TIMER)
mtb_hal_timer_configurator_t emUSB_OS_Timer_hal_config =
{
    .tcpwm_base = emUSB_OS_Timer_HW,
    .clock = &emUSB_OS_Timer_hal_clock,
    .tcpwm_cntnum = 1U,
};
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_TIMER) */

const cy_stc_tcpwm_counter_config_t CYBSP_GENERAL_PURPOSE_TIMER_config =
{
    .period = 9999,
    .clockPrescaler = CY_TCPWM_COUNTER_PRESCALER_DIVBY_1,
    .runMode = CY_TCPWM_COUNTER_CONTINUOUS,
    .countDirection = CY_TCPWM_COUNTER_COUNT_UP,
    .compareOrCapture = CY_TCPWM_COUNTER_MODE_COMPARE,
    .compare0 = 9999,
    .compare1 = 16384,
    .enableCompareSwap = false,
    .interruptSources = (CY_TCPWM_INT_ON_TC ) | (CY_TCPWM_INT_ON_CC0 & 0U) | (CY_TCPWM_INT_ON_CC1 & 0U),
    .captureInputMode = CYBSP_GENERAL_PURPOSE_TIMER_INPUT_DISABLED & 0x3U,
    .captureInput = CY_TCPWM_INPUT_0,
    .reloadInputMode = CYBSP_GENERAL_PURPOSE_TIMER_INPUT_DISABLED & 0x3U,
    .reloadInput = CY_TCPWM_INPUT_0,
    .startInputMode = CYBSP_GENERAL_PURPOSE_TIMER_INPUT_DISABLED & 0x3U,
    .startInput = CY_TCPWM_INPUT_0,
    .stopInputMode = CYBSP_GENERAL_PURPOSE_TIMER_INPUT_DISABLED & 0x3U,
    .stopInput = CY_TCPWM_INPUT_0,
    .countInputMode = CYBSP_GENERAL_PURPOSE_TIMER_INPUT_DISABLED & 0x3U,
    .countInput = CY_TCPWM_INPUT_1,
    .capture1InputMode = CYBSP_GENERAL_PURPOSE_TIMER_INPUT_DISABLED & 0x3U,
    .capture1Input = CY_TCPWM_INPUT_0,
    .compare2 = CY_TCPWM_GRP_CNT_CC0_DEFAULT,
    .compare3 = CY_TCPWM_GRP_CNT_CC0_BUFF_DEFAULT,
    .enableCompare1Swap = false,
    .trigger0Event = CY_TCPWM_CNT_TRIGGER_ON_DISABLED,
    .trigger1Event = CY_TCPWM_CNT_TRIGGER_ON_DISABLED,
#if defined (CY_IP_MXS40TCPWM)
    .buffer_swap_enable = false,
    .direction_mode = CY_TCPWM_COUNTER_DIRECTION_DISABLE,
    .glitch_filter_enable = false,
    .gf_depth = CY_GLITCH_FILTER_DEPTH_SUPPORT_VALUE_0,
#endif /* defined (CY_IP_MXS40TCPWM) */
};

#if defined (COMPONENT_MTB_HAL)
const mtb_hal_peri_div_t CYBSP_GENERAL_PURPOSE_TIMER_clock_ref =
{
    .clk_dst = (en_clk_dst_t)PCLK_TCPWM0_CLOCK_COUNTER_EN2,
    .div_type = CY_SYSCLK_DIV_16_BIT,
    .div_num = 2,
};
const mtb_hal_clock_t CYBSP_GENERAL_PURPOSE_TIMER_hal_clock =
{
    .clock_ref = &CYBSP_GENERAL_PURPOSE_TIMER_clock_ref,
    .interface = &mtb_hal_clock_peri_interface,
};
#endif /* defined (COMPONENT_MTB_HAL) */

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_TIMER)
const mtb_hal_timer_configurator_t CYBSP_GENERAL_PURPOSE_TIMER_hal_config =
{
    .tcpwm_base = CYBSP_GENERAL_PURPOSE_TIMER_HW,
    .clock = &CYBSP_GENERAL_PURPOSE_TIMER_hal_clock,
    .tcpwm_cntnum = 2U,
};
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_TIMER) */

const cy_stc_tcpwm_pwm_config_t CYBSP_PWM_DVP_CAM_CTRL_config =
{
    .pwmMode = CY_TCPWM_PWM_MODE_PWM,
    .clockPrescaler = CY_TCPWM_PWM_PRESCALER_DIVBY_1,
    .pwmAlignment = CY_TCPWM_PWM_LEFT_ALIGN,
    .deadTimeClocks = 0,
    .runMode = CY_TCPWM_PWM_CONTINUOUS,
    .period0 = 1,
    .period1 = 32768,
    .enablePeriodSwap = false,
    .compare0 = 1,
    .compare1 = 16384,
    .enableCompareSwap = false,
    .interruptSources = (CY_TCPWM_INT_ON_TC & 0U) | (CY_TCPWM_INT_ON_CC0 & 0U) | (CY_TCPWM_INT_ON_CC1 & 0U),
    .invertPWMOut = CY_TCPWM_PWM_INVERT_DISABLE,
    .invertPWMOutN = CY_TCPWM_PWM_INVERT_DISABLE,
    .killMode = CY_TCPWM_PWM_STOP_ON_KILL,
    .swapInputMode = CYBSP_PWM_DVP_CAM_CTRL_INPUT_DISABLED & 0x3U,
    .swapInput = CY_TCPWM_INPUT_0,
    .reloadInputMode = CYBSP_PWM_DVP_CAM_CTRL_INPUT_DISABLED & 0x3U,
    .reloadInput = CY_TCPWM_INPUT_0,
    .startInputMode = CYBSP_PWM_DVP_CAM_CTRL_INPUT_DISABLED & 0x3U,
    .startInput = CY_TCPWM_INPUT_0,
    .killInputMode = CYBSP_PWM_DVP_CAM_CTRL_INPUT_DISABLED & 0x3U,
    .killInput = CY_TCPWM_INPUT_0,
    .countInputMode = CYBSP_PWM_DVP_CAM_CTRL_INPUT_DISABLED & 0x3U,
    .countInput = CY_TCPWM_INPUT_1,
    .swapOverflowUnderflow = false,
    .immediateKill = false,
    .tapsEnabled = 45,
    .compare2 = CY_TCPWM_GRP_CNT_CC0_DEFAULT,
    .compare3 = CY_TCPWM_GRP_CNT_CC0_BUFF_DEFAULT,
    .enableCompare1Swap = false,
    .compare0MatchUp = true,
    .compare0MatchDown = false,
    .compare1MatchUp = true,
    .compare1MatchDown = false,
    .kill1InputMode = CYBSP_PWM_DVP_CAM_CTRL_INPUT_DISABLED & 0x3U,
    .kill1Input = CY_TCPWM_INPUT_0,
    .pwmOnDisable = CY_TCPWM_PWM_OUTPUT_HIGHZ,
    .trigger0Event = CY_TCPWM_CNT_TRIGGER_ON_DISABLED,
    .trigger1Event = CY_TCPWM_CNT_TRIGGER_ON_DISABLED,
    .reloadLineSelect = false,
    .line_out_sel = CY_TCPWM_OUTPUT_PWM_SIGNAL,
    .linecompl_out_sel = CY_TCPWM_OUTPUT_INVERTED_PWM_SIGNAL,
    .line_out_sel_buff = CY_TCPWM_OUTPUT_PWM_SIGNAL,
    .linecompl_out_sel_buff = CY_TCPWM_OUTPUT_INVERTED_PWM_SIGNAL,
    .deadTimeClocks_linecompl_out = 0,
#if defined (CY_IP_MXS40TCPWM)
    .hrpwm_enable = false,
    .hrpwm_input_freq = CY_TCPWM_HRPWM_FREQ_80MHZ_OR_100MHZ,
    .kill_line_polarity = CY_TCPWM_LINEOUT_AND_LINECMPOUT_IS_LOW,
    .deadTimeClocksBuff = 0,
    .deadTimeClocksBuff_linecompl_out = 0,
    .buffer_swap_enable = false,
    .glitch_filter_enable = false,
    .gf_depth = CY_GLITCH_FILTER_DEPTH_SUPPORT_VALUE_0,
    .dithering_mode = CY_TCPWM_DITHERING_DISABLE,
    .period_dithering_value = 128,
    .duty_dithering_value = 128,
    .limiter = CY_TCPWM_DITHERING_LIMITER_7,
    .pwm_tc_sync_kill_dt = false,
    .pwm_sync_kill_dt = false,
#endif /* defined (CY_IP_MXS40TCPWM) */
};

#if defined (COMPONENT_MTB_HAL)
const mtb_hal_peri_div_t CYBSP_PWM_DVP_CAM_CTRL_clock_ref =
{
    .clk_dst = (en_clk_dst_t)PCLK_TCPWM0_CLOCK_COUNTER_EN4,
    .div_type = CY_SYSCLK_DIV_16_5_BIT,
    .div_num = 1,
};
const mtb_hal_clock_t CYBSP_PWM_DVP_CAM_CTRL_hal_clock =
{
    .clock_ref = &CYBSP_PWM_DVP_CAM_CTRL_clock_ref,
    .interface = &mtb_hal_clock_peri_interface,
};
#endif /* defined (COMPONENT_MTB_HAL) */

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_PWM)
const mtb_hal_pwm_configurator_t CYBSP_PWM_DVP_CAM_CTRL_hal_config =
{
    .base = CYBSP_PWM_DVP_CAM_CTRL_HW,
    .clock = &CYBSP_PWM_DVP_CAM_CTRL_hal_clock,
    .group = 0UL,
    .cntnum = 4UL,
    .max_count = 1,
};
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_PWM) */

const cy_stc_tcpwm_pwm_config_t CYBSP_DEAD_TIME_PWM_config =
{
    .pwmMode = CY_TCPWM_PWM_MODE_DEADTIME,
    .clockPrescaler = CY_TCPWM_PWM_PRESCALER_DIVBY_1,
    .pwmAlignment = CY_TCPWM_PWM_LEFT_ALIGN,
    .deadTimeClocks = 100,
    .runMode = CY_TCPWM_PWM_CONTINUOUS,
    .period0 = 400,
    .period1 = 32768,
    .enablePeriodSwap = false,
    .compare0 = 200,
    .compare1 = 16384,
    .enableCompareSwap = false,
    .interruptSources = (CY_TCPWM_INT_ON_TC & 0U) | (CY_TCPWM_INT_ON_CC0 & 0U) | (CY_TCPWM_INT_ON_CC1 & 0U),
    .invertPWMOut = CY_TCPWM_PWM_INVERT_DISABLE,
    .invertPWMOutN = CY_TCPWM_PWM_INVERT_DISABLE,
    .killMode = CY_TCPWM_PWM_STOP_ON_KILL,
    .swapInputMode = CYBSP_DEAD_TIME_PWM_INPUT_DISABLED & 0x3U,
    .swapInput = CY_TCPWM_INPUT_0,
    .reloadInputMode = CYBSP_DEAD_TIME_PWM_INPUT_DISABLED & 0x3U,
    .reloadInput = CY_TCPWM_INPUT_0,
    .startInputMode = CYBSP_DEAD_TIME_PWM_INPUT_DISABLED & 0x3U,
    .startInput = CY_TCPWM_INPUT_0,
    .killInputMode = CYBSP_DEAD_TIME_PWM_INPUT_DISABLED & 0x3U,
    .killInput = CY_TCPWM_INPUT_0,
    .countInputMode = CYBSP_DEAD_TIME_PWM_INPUT_DISABLED & 0x3U,
    .countInput = CY_TCPWM_INPUT_1,
    .swapOverflowUnderflow = false,
    .immediateKill = false,
    .tapsEnabled = 45,
    .compare2 = CY_TCPWM_GRP_CNT_CC0_DEFAULT,
    .compare3 = CY_TCPWM_GRP_CNT_CC0_BUFF_DEFAULT,
    .enableCompare1Swap = false,
    .compare0MatchUp = true,
    .compare0MatchDown = false,
    .compare1MatchUp = true,
    .compare1MatchDown = false,
    .kill1InputMode = CYBSP_DEAD_TIME_PWM_INPUT_DISABLED & 0x3U,
    .kill1Input = CY_TCPWM_INPUT_0,
    .pwmOnDisable = CY_TCPWM_PWM_OUTPUT_HIGHZ,
    .trigger0Event = CY_TCPWM_CNT_TRIGGER_ON_DISABLED,
    .trigger1Event = CY_TCPWM_CNT_TRIGGER_ON_DISABLED,
    .reloadLineSelect = false,
    .line_out_sel = CY_TCPWM_OUTPUT_PWM_SIGNAL,
    .linecompl_out_sel = CY_TCPWM_OUTPUT_INVERTED_PWM_SIGNAL,
    .line_out_sel_buff = CY_TCPWM_OUTPUT_PWM_SIGNAL,
    .linecompl_out_sel_buff = CY_TCPWM_OUTPUT_INVERTED_PWM_SIGNAL,
    .deadTimeClocks_linecompl_out = 100,
#if defined (CY_IP_MXS40TCPWM)
    .hrpwm_enable = false,
    .hrpwm_input_freq = CY_TCPWM_HRPWM_FREQ_80MHZ_OR_100MHZ,
    .kill_line_polarity = CY_TCPWM_LINEOUT_AND_LINECMPOUT_IS_LOW,
    .deadTimeClocksBuff = 0,
    .deadTimeClocksBuff_linecompl_out = 0,
    .buffer_swap_enable = false,
    .glitch_filter_enable = false,
    .gf_depth = CY_GLITCH_FILTER_DEPTH_SUPPORT_VALUE_0,
    .dithering_mode = CY_TCPWM_DITHERING_DISABLE,
    .period_dithering_value = 128,
    .duty_dithering_value = 128,
    .limiter = CY_TCPWM_DITHERING_LIMITER_7,
    .pwm_tc_sync_kill_dt = false,
    .pwm_sync_kill_dt = false,
#endif /* defined (CY_IP_MXS40TCPWM) */
};

#if defined (COMPONENT_MTB_HAL)
const mtb_hal_peri_div_t CYBSP_DEAD_TIME_PWM_clock_ref =
{
    .clk_dst = (en_clk_dst_t)PCLK_TCPWM0_CLOCK_COUNTER_EN7,
    .div_type = CY_SYSCLK_DIV_8_BIT,
    .div_num = 3,
};
const mtb_hal_clock_t CYBSP_DEAD_TIME_PWM_hal_clock =
{
    .clock_ref = &CYBSP_DEAD_TIME_PWM_clock_ref,
    .interface = &mtb_hal_clock_peri_interface,
};
#endif /* defined (COMPONENT_MTB_HAL) */

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_PWM)
const mtb_hal_pwm_configurator_t CYBSP_DEAD_TIME_PWM_hal_config =
{
    .base = CYBSP_DEAD_TIME_PWM_HW,
    .clock = &CYBSP_DEAD_TIME_PWM_hal_clock,
    .group = 0UL,
    .cntnum = 7UL,
    .max_count = 400,
};
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_PWM) */

void init_cycfg_peripherals(void)
{
    Cy_SysClk_PeriGroupSlaveInit(CY_MMIO_PASS_PERI_NR, CY_MMIO_PASS_GROUP_NR, CY_MMIO_PASS_SLAVE_NR, CY_MMIO_PASS_CLK_HF_NR);
    Cy_SysClk_PeriGroupSlaveInit(CY_MMIO_I3C_PERI_NR, CY_MMIO_I3C_GROUP_NR, CY_MMIO_I3C_SLAVE_NR, CY_MMIO_I3C_CLK_HF_NR);
    Cy_SysClk_PeriphAssignDivider(PCLK_I3C_CLOCK_I3C_EN, CY_SYSCLK_DIV_8_BIT, 0U);
#if defined (CY_DEVICE_CONFIGURATOR_IP_ENABLE_FEATURE)
    Cy_SysClk_PeriGroupSlaveInit(CY_MMIO_PDM0_PERI_NR, CY_MMIO_PDM0_GROUP_NR, CY_MMIO_PDM0_SLAVE_NR, CY_MMIO_PDM0_CLK_HF_NR);
#endif /* defined (CY_DEVICE_CONFIGURATOR_IP_ENABLE_FEATURE) */
    Cy_SysClk_PeriPclkAssignDivider(PCLK_PDM0_CLK_IF_SRSS, CY_SYSCLK_DIV_16_5_BIT, 1U);
    Cy_SysClk_PeriGroupSlaveInit(CY_MMIO_SCB0_PERI_NR, CY_MMIO_SCB0_GROUP_NR, CY_MMIO_SCB0_SLAVE_NR, CY_MMIO_SCB0_CLK_HF_NR);
    Cy_SysClk_PeriPclkAssignDivider(PCLK_SCB0_CLOCK_SCB_EN, CY_SYSCLK_DIV_16_BIT, 0U);
    Cy_SysClk_PeriGroupSlaveInit(CY_MMIO_SCB2_PERI_NR, CY_MMIO_SCB2_GROUP_NR, CY_MMIO_SCB2_SLAVE_NR, CY_MMIO_SCB2_CLK_HF_NR);
    Cy_SysClk_PeriPclkAssignDivider(PCLK_SCB2_CLOCK_SCB_EN, CY_SYSCLK_DIV_16_BIT, 1U);
    Cy_SysClk_PeriGroupSlaveInit(CY_MMIO_SCB3_PERI_NR, CY_MMIO_SCB3_GROUP_NR, CY_MMIO_SCB3_SLAVE_NR, CY_MMIO_SCB3_CLK_HF_NR);
    Cy_SysClk_PeriPclkAssignDivider(PCLK_SCB3_CLOCK_SCB_EN, CY_SYSCLK_DIV_8_BIT, 1U);
    Cy_SysClk_PeriGroupSlaveInit(CY_MMIO_SCB4_PERI_NR, CY_MMIO_SCB4_GROUP_NR, CY_MMIO_SCB4_SLAVE_NR, CY_MMIO_SCB4_CLK_HF_NR);
    Cy_SysClk_PeriPclkAssignDivider(PCLK_SCB4_CLOCK_SCB_EN, CY_SYSCLK_DIV_16_5_BIT, 0U);
    Cy_SysClk_PeriGroupSlaveInit(CY_MMIO_SCB5_PERI_NR, CY_MMIO_SCB5_GROUP_NR, CY_MMIO_SCB5_SLAVE_NR, CY_MMIO_SCB5_CLK_HF_NR);
    Cy_SysClk_PeriPclkAssignDivider(PCLK_SCB5_CLOCK_SCB_EN, CY_SYSCLK_DIV_8_BIT, 0U);
#if defined (CY_DEVICE_CONFIGURATOR_IP_ENABLE_FEATURE)
    Cy_SysClk_PeriGroupSlaveInit(CY_MMIO_SDHC0_PERI_NR, CY_MMIO_SDHC0_GROUP_NR, CY_MMIO_SDHC0_SLAVE_NR, CY_MMIO_SDHC0_CLK_HF_NR);
#endif /* defined (CY_DEVICE_CONFIGURATOR_IP_ENABLE_FEATURE) */
    Cy_SysClk_PeriPclkAssignDivider(PCLK_SDHC0_CLK_HF, CY_SYSCLK_DIV_8_BIT, 0U);
    Cy_SysClk_PeriGroupSlaveInit(CY_MMIO_USBHS_PERI_NR, CY_MMIO_USBHS_GROUP_NR, CY_MMIO_USBHS_SLAVE_NR, CY_MMIO_USBHS_CLK_HF_NR);
    Cy_SysClk_PeriGroupSlaveInit(CY_MMIO_SMIF0_PERI_NR, CY_MMIO_SMIF0_GROUP_NR, CY_MMIO_SMIF0_SLAVE_NR, CY_MMIO_SMIF0_CLK_HF_NR);
    Cy_SysClk_PeriGroupSlaveInit(CY_MMIO_SMIF01_PERI_NR, CY_MMIO_SMIF01_GROUP_NR, CY_MMIO_SMIF01_SLAVE_NR, CY_MMIO_SMIF01_CLK_HF_NR);
#if defined (CY_DEVICE_CONFIGURATOR_IP_ENABLE_FEATURE)
    Cy_SysClk_PeriGroupSlaveInit(CY_MMIO_TDM0_PERI_NR, CY_MMIO_TDM0_GROUP_NR, CY_MMIO_TDM0_SLAVE_NR, CY_MMIO_TDM0_CLK_HF_NR);
#endif /* defined (CY_DEVICE_CONFIGURATOR_IP_ENABLE_FEATURE) */
    Cy_SysClk_PeriPclkAssignDivider(PCLK_TDM0_CLK_IF_SRSS0, CY_SYSCLK_DIV_16_5_BIT, 0U);
#if defined (CY_DEVICE_CONFIGURATOR_IP_ENABLE_FEATURE)
    Cy_SysClk_PeriGroupSlaveInit(CY_MMIO_TCPWM0_PERI_NR, CY_MMIO_TCPWM0_GROUP_NR, CY_MMIO_TCPWM0_SLAVE_NR, CY_MMIO_TCPWM0_CLK_HF_NR);
#endif /* defined (CY_DEVICE_CONFIGURATOR_IP_ENABLE_FEATURE) */
    Cy_SysClk_PeriphAssignDivider(PCLK_TCPWM0_CLOCK_COUNTER_EN0, CY_SYSCLK_DIV_16_BIT, 3U);
#if defined (CY_DEVICE_CONFIGURATOR_IP_ENABLE_FEATURE)
    Cy_SysClk_PeriGroupSlaveInit(CY_MMIO_TCPWM0_PERI_NR, CY_MMIO_TCPWM0_GROUP_NR, CY_MMIO_TCPWM0_SLAVE_NR, CY_MMIO_TCPWM0_CLK_HF_NR);
#endif /* defined (CY_DEVICE_CONFIGURATOR_IP_ENABLE_FEATURE) */
    Cy_SysClk_PeriphAssignDivider(PCLK_TCPWM0_CLOCK_COUNTER_EN1, CY_SYSCLK_DIV_8_BIT, 2U);
#if defined (CY_DEVICE_CONFIGURATOR_IP_ENABLE_FEATURE)
    Cy_SysClk_PeriGroupSlaveInit(CY_MMIO_TCPWM0_PERI_NR, CY_MMIO_TCPWM0_GROUP_NR, CY_MMIO_TCPWM0_SLAVE_NR, CY_MMIO_TCPWM0_CLK_HF_NR);
#endif /* defined (CY_DEVICE_CONFIGURATOR_IP_ENABLE_FEATURE) */
    Cy_SysClk_PeriphAssignDivider(PCLK_TCPWM0_CLOCK_COUNTER_EN2, CY_SYSCLK_DIV_16_BIT, 2U);
#if defined (CY_DEVICE_CONFIGURATOR_IP_ENABLE_FEATURE)
    Cy_SysClk_PeriGroupSlaveInit(CY_MMIO_TCPWM0_PERI_NR, CY_MMIO_TCPWM0_GROUP_NR, CY_MMIO_TCPWM0_SLAVE_NR, CY_MMIO_TCPWM0_CLK_HF_NR);
#endif /* defined (CY_DEVICE_CONFIGURATOR_IP_ENABLE_FEATURE) */
    Cy_SysClk_PeriphAssignDivider(PCLK_TCPWM0_CLOCK_COUNTER_EN4, CY_SYSCLK_DIV_16_5_BIT, 1U);
#if defined (CY_DEVICE_CONFIGURATOR_IP_ENABLE_FEATURE)
    Cy_SysClk_PeriGroupSlaveInit(CY_MMIO_TCPWM0_PERI_NR, CY_MMIO_TCPWM0_GROUP_NR, CY_MMIO_TCPWM0_SLAVE_NR, CY_MMIO_TCPWM0_CLK_HF_NR);
#endif /* defined (CY_DEVICE_CONFIGURATOR_IP_ENABLE_FEATURE) */
    Cy_SysClk_PeriphAssignDivider(PCLK_TCPWM0_CLOCK_COUNTER_EN7, CY_SYSCLK_DIV_8_BIT, 3U);
}
