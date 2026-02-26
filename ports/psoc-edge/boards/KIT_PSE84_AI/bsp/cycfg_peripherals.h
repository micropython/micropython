/*******************************************************************************
 * File Name: cycfg_peripherals.h
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

#if !defined(CYCFG_PERIPHERALS_H)
#define CYCFG_PERIPHERALS_H

#include "cycfg_notices.h"
#include "cy_autanalog.h"
#include "cy_sysclk.h"
#include "cy_i3c.h"
#include "cy_pdm_pcm_v2.h"
#include "cy_scb_i2c.h"
#include "cy_scb_uart.h"
#include "cy_scb_spi.h"
#include "cy_sd_host.h"
#include "cy_smif.h"
#include "cycfg_qspi_memslot.h"
#include "cy_mcwdt.h"
#include "cy_tdm.h"
#include "cy_tcpwm_pwm.h"
#include "cycfg_routing.h"
#include "cy_tcpwm_counter.h"

#if defined (COMPONENT_MTB_HAL)
#include "mtb_hal.h"
#include "cycfg_peripheral_clocks.h"
#include "mtb_hal_hw_types.h"
#include "mtb_hal_clock.h"
#endif /* defined (COMPONENT_MTB_HAL) */

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

#define CYBSP_AUTONOMOUS_ANALOG_ENABLED 1U
#define AUTANALOG_CLOCK_DIV_PRIO_HS_DEFAULT 20
#define CYBSP_AUTONOMOUS_ANALOG_lppass_IRQ pass_interrupt_lppass_IRQn
#define CYBSP_AUTONOMOUS_ANALOG_fifo_IRQ pass_interrupt_fifo_IRQn
#define CYBSP_AUTONOMOUS_CONTROLLER_ENABLED 1U
#define CYBSP_SAR_ADC_ENABLED 1U
#define CYBSP_AUTONOMOUS_CONTROLLER_STATE_0_ENABLED 1U
#define CYBSP_AUTONOMOUS_CONTROLLER_STATE_1_ENABLED 1U
#define CYBSP_AUTONOMOUS_CONTROLLER_STATE_2_ENABLED 1U
#define CYBSP_SAR_ADC_GPIO_CH_0_ENABLED 1U
#define CYBSP_SAR_ADC_SCAN_GRP_0_ENABLED 1U
#define CYBSP_SAR_ADC_SCAN_GRP_0_SCAN_0_ENABLED 1U
#define CYBSP_I3C_CONTROLLER_ENABLED 1U
#define CYBSP_I3C_CONTROLLER_HW I3C_CORE
#define CYBSP_I3C_CONTROLLER_IRQ i3c_interrupt_IRQn
#define CYBSP_PDM_ENABLED 1U
#define CYBSP_PDM_HW PDM0
#define CYBSP_PDM_CHANNEL_2_IRQ pdm_0_interrupts_2_IRQn
#define CYBSP_PDM_CHANNEL_3_IRQ pdm_0_interrupts_3_IRQn
#define CYBSP_I2C_CONTROLLER_ENABLED 1U
#define CYBSP_I2C_CONTROLLER_HW SCB0
#define CYBSP_I2C_CONTROLLER_IRQ scb_0_interrupt_IRQn
#define CYBSP_DEBUG_UART_ENABLED 1U
#define CYBSP_DEBUG_UART_HW SCB2
#define CYBSP_DEBUG_UART_IRQ scb_2_interrupt_IRQn
#define CYBSP_SPI_CONTROLLER_ENABLED 1U
#define CYBSP_SPI_CONTROLLER_HW SCB3
#define CYBSP_SPI_CONTROLLER_IRQ scb_3_interrupt_IRQn
#define CYBSP_BT_UART_ENABLED 1U
#define CYBSP_BT_UART_HW SCB4
#define CYBSP_BT_UART_IRQ scb_4_interrupt_IRQn
#define CYBSP_I2C_CAM_CONTROLLER_ENABLED 1U
#define CYBSP_I2C_DISPLAY_CONTROLLER_ENABLED CYBSP_I2C_CAM_CONTROLLER_ENABLED
#define CYBSP_I2C_CAM_CONTROLLER_HW SCB5
#define CYBSP_I2C_DISPLAY_CONTROLLER_HW CYBSP_I2C_CAM_CONTROLLER_HW
#define CYBSP_I2C_CAM_CONTROLLER_IRQ scb_5_interrupt_IRQn
#define CYBSP_I2C_DISPLAY_CONTROLLER_IRQ CYBSP_I2C_CAM_CONTROLLER_IRQ
#define CYBSP_WIFI_SDIO_ENABLED 1U
#define CYBSP_WIFI_SDIO_HW SDHC0
#define CYBSP_WIFI_SDIO_IRQ sdhc_0_interrupt_general_IRQn
#define CYBSP_USB_DEVICE_0_ENABLED 1U
#define CYBSP_SMIF_CORE_0_XSPI_FLASH_ENABLED 1U
#define CYBSP_SMIF_CORE_0_XSPI_FLASH_HW SMIF0_CORE
#define CYBSP_SMIF_CORE_0_XSPI_FLASH_IRQ smif_0_smif0_interrupt_nsec_IRQn
#define CYBSP_SMIF_CORE_0_XSPI_FLASH_MEMORY_MODE_ALIGMENT_ERROR (0UL)
#define CYBSP_SMIF_CORE_0_XSPI_FLASH_RX_DATA_FIFO_UNDERFLOW (0UL)
#define CYBSP_SMIF_CORE_0_XSPI_FLASH_TX_COMMAND_FIFO_OVERFLOW (0UL)
#define CYBSP_SMIF_CORE_0_XSPI_FLASH_TX_DATA_FIFO_OVERFLOW (0UL)
#define CYBSP_SMIF_CORE_0_XSPI_FLASH_RX_DMA_TRIGGER_OUT_USED (0UL)
#define CYBSP_SMIF_CORE_0_XSPI_FLASH_TX_DMA_TRIGGER_OUT_USED (0UL)
#define CYBSP_SMIF_CORE_0_XSPI_FLASH_RX_FIFO_TRIGGER_LEVEL (0UL)
#define CYBSP_SMIF_CORE_0_XSPI_FLASH_TX_FIFO_TRIGGER_LEVEL (0UL)
#define CYBSP_SMIF_CORE_0_XSPI_FLASH_DATALINES0_1 (1UL)
#define CYBSP_SMIF_CORE_0_XSPI_FLASH_DATALINES2_3 (1UL)
#define CYBSP_SMIF_CORE_0_XSPI_FLASH_DATALINES4_5 (0UL)
#define CYBSP_SMIF_CORE_0_XSPI_FLASH_DATALINES6_7 (0UL)
#define CYBSP_SMIF_CORE_0_XSPI_FLASH_SS0 (0UL)
#define CYBSP_SMIF_CORE_0_XSPI_FLASH_SS1 (1UL)
#define CYBSP_SMIF_CORE_0_XSPI_FLASH_SS2 (0UL)
#define CYBSP_SMIF_CORE_0_XSPI_FLASH_SS3 (0UL)
#define CYBSP_SMIF_CORE_0_XSPI_FLASH_DESELECT_DELAY 7
#define CYBSP_SMIF_CORE_1_PSRAM_ENABLED 1U
#define CYBSP_SMIF_CORE_1_PSRAM_HW SMIF1_CORE
#define CYBSP_SMIF_CORE_1_PSRAM_IRQ smif_1_smif0_interrupt_nsec_IRQn
#define CYBSP_SMIF_CORE_1_PSRAM_MEMORY_MODE_ALIGMENT_ERROR (0UL)
#define CYBSP_SMIF_CORE_1_PSRAM_RX_DATA_FIFO_UNDERFLOW (0UL)
#define CYBSP_SMIF_CORE_1_PSRAM_TX_COMMAND_FIFO_OVERFLOW (0UL)
#define CYBSP_SMIF_CORE_1_PSRAM_TX_DATA_FIFO_OVERFLOW (0UL)
#define CYBSP_SMIF_CORE_1_PSRAM_RX_DMA_TRIGGER_OUT_USED (0UL)
#define CYBSP_SMIF_CORE_1_PSRAM_TX_DMA_TRIGGER_OUT_USED (0UL)
#define CYBSP_SMIF_CORE_1_PSRAM_RX_FIFO_TRIGGER_LEVEL (0UL)
#define CYBSP_SMIF_CORE_1_PSRAM_TX_FIFO_TRIGGER_LEVEL (0UL)
#define CYBSP_SMIF_CORE_1_PSRAM_DATALINES0_1 (1UL)
#define CYBSP_SMIF_CORE_1_PSRAM_DATALINES2_3 (1UL)
#define CYBSP_SMIF_CORE_1_PSRAM_DATALINES4_5 (1UL)
#define CYBSP_SMIF_CORE_1_PSRAM_DATALINES6_7 (1UL)
#define CYBSP_SMIF_CORE_1_PSRAM_SS0 (0UL)
#define CYBSP_SMIF_CORE_1_PSRAM_SS1 (0UL)
#define CYBSP_SMIF_CORE_1_PSRAM_SS2 (1UL)
#define CYBSP_SMIF_CORE_1_PSRAM_SS3 (0UL)
#define CYBSP_SMIF_CORE_1_PSRAM_DESELECT_DELAY 7
#define CYBSP_CM33_LPTIMER_0_ENABLED 1U
#define CYBSP_CM33_LPTIMER_0_HW MCWDT_STRUCT0
#define CYBSP_CM33_LPTIMER_0_IRQ srss_interrupt_mcwdt_0_IRQn
#define CYBSP_CM55_LPTIMER_1_ENABLED 1U
#define CYBSP_CM55_LPTIMER_1_HW MCWDT_STRUCT1
#define CYBSP_CM55_LPTIMER_1_IRQ srss_interrupt_mcwdt_1_IRQn
#define CYBSP_TDM_CONTROLLER_0_ENABLED 1U
#define CYBSP_TDM_CONTROLLER_0_HW TDM_STRUCT0
#define CYBSP_TDM_CONTROLLER_0_TX_HW TDM_STRUCT0_TX
#define CYBSP_TDM_CONTROLLER_0_RX_HW TDM_STRUCT0_RX
#define CYBSP_TDM_CONTROLLER_0_TX_IRQ tdm_0_interrupts_tx_0_IRQn
#define CYBSP_TDM_CONTROLLER_0_RX_IRQ tdm_0_interrupts_rx_0_IRQn
#define CYBSP_PWM_LED_CTRL_ENABLED 1U
#define CYBSP_PWM_LED_CTRL_HW TCPWM0
#define CYBSP_PWM_LED_CTRL_NUM 0UL
#define CYBSP_USB_OS_TIMER_COUNTER_ENABLED 1U
#define emUSB_OS_Timer_HW TCPWM0
#define emUSB_OS_Timer_NUM 1UL
#define emUSB_OS_Timer_IRQ tcpwm_0_interrupts_1_IRQn
#define CYBSP_GENERAL_PURPOSE_TIMER_ENABLED 1U
#define CYBSP_GENERAL_PURPOSE_TIMER_HW TCPWM0
#define CYBSP_GENERAL_PURPOSE_TIMER_NUM 2UL
#define CYBSP_GENERAL_PURPOSE_TIMER_IRQ tcpwm_0_interrupts_2_IRQn
#define CYBSP_PWM_DVP_CAM_CTRL_ENABLED 1U
#define CYBSP_PWM_DVP_CAM_CTRL_HW TCPWM0
#define CYBSP_PWM_DVP_CAM_CTRL_NUM 4UL
#define CYBSP_DEAD_TIME_PWM_ENABLED 1U
#define CYBSP_DEAD_TIME_PWM_HW TCPWM0
#define CYBSP_DEAD_TIME_PWM_NUM 7UL

extern cy_stc_autanalog_cfg_t autonomous_analog_cfg;
extern cy_stc_autanalog_stt_t autonomous_analog_stt[];
extern cy_stc_autanalog_t autonomous_analog_init;
extern cy_en_autanalog_ac_out_trigger_mask_t CYBSP_AUTONOMOUS_CONTROLLER_out_trig_mask[];
extern cy_stc_autanalog_ac_t CYBSP_AUTONOMOUS_CONTROLLER_cfg;
extern cy_stc_autanalog_stt_ac_t CYBSP_AUTONOMOUS_CONTROLLER_stt[];
extern cy_stc_autanalog_sar_hs_chan_t CYBSP_SAR_ADC_gpio_ch_cfg[];
extern cy_stc_autanalog_sar_sta_hs_t CYBSP_SAR_ADC_sta_hs_cfg;
extern cy_stc_autanalog_sar_sta_t CYBSP_SAR_ADC_sta_cfg;
extern cy_stc_autanalog_sar_seq_tab_hs_t CYBSP_SAR_ADC_seq_hs_cfg[];
extern cy_stc_autanalog_sar_t CYBSP_SAR_ADC_cfg;
extern cy_stc_autanalog_stt_sar_t CYBSP_SAR_ADC_stt[];

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_ADC)
extern mtb_hal_adc_configurator_t CYBSP_SAR_ADC_hal_config;
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_ADC) */

extern const cy_stc_i3c_config_t CYBSP_I3C_CONTROLLER_config;
extern const cy_stc_pdm_pcm_config_v2_t CYBSP_PDM_config;
extern const cy_stc_pdm_pcm_channel_config_t channel_2_config;
extern const cy_stc_pdm_pcm_channel_config_t channel_3_config;
extern const cy_stc_scb_i2c_config_t CYBSP_I2C_CONTROLLER_config;

#if defined (COMPONENT_MTB_HAL)
extern const mtb_hal_peri_div_t CYBSP_I2C_CONTROLLER_clock_ref;
extern const mtb_hal_clock_t CYBSP_I2C_CONTROLLER_hal_clock;
#endif /* defined (COMPONENT_MTB_HAL) */

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_I2C)
extern const mtb_hal_i2c_configurator_t CYBSP_I2C_CONTROLLER_hal_config;
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_I2C) */

extern const cy_stc_scb_uart_config_t CYBSP_DEBUG_UART_config;

#if defined (COMPONENT_MTB_HAL)
extern const mtb_hal_peri_div_t CYBSP_DEBUG_UART_clock_ref;
extern const mtb_hal_clock_t CYBSP_DEBUG_UART_hal_clock;
#endif /* defined (COMPONENT_MTB_HAL) */

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_UART)
extern const mtb_hal_uart_configurator_t CYBSP_DEBUG_UART_hal_config;
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_UART) */

extern const cy_stc_scb_spi_config_t CYBSP_SPI_CONTROLLER_config;

#if defined (COMPONENT_MTB_HAL)
extern const mtb_hal_peri_div_t CYBSP_SPI_CONTROLLER_clock_ref;
extern const mtb_hal_clock_t CYBSP_SPI_CONTROLLER_hal_clock;
#endif /* defined (COMPONENT_MTB_HAL) */

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_SPI)
extern const mtb_hal_spi_configurator_t CYBSP_SPI_CONTROLLER_hal_config;
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_SPI) */

extern const cy_stc_scb_uart_config_t CYBSP_BT_UART_config;

#if defined (COMPONENT_MTB_HAL)
extern const mtb_hal_peri_div_t CYBSP_BT_UART_clock_ref;
extern const mtb_hal_clock_t CYBSP_BT_UART_hal_clock;
#endif /* defined (COMPONENT_MTB_HAL) */

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_UART)
extern const mtb_hal_uart_configurator_t CYBSP_BT_UART_hal_config;
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_UART) */

extern const cy_stc_scb_i2c_config_t CYBSP_I2C_CAM_CONTROLLER_config;

#define CYBSP_I2C_DISPLAY_CONTROLLER_config CYBSP_I2C_CAM_CONTROLLER_config

#if defined (COMPONENT_MTB_HAL)
extern const mtb_hal_peri_div_t CYBSP_I2C_CAM_CONTROLLER_clock_ref;
#define CYBSP_I2C_DISPLAY_CONTROLLER_clock_ref CYBSP_I2C_CAM_CONTROLLER_clock_ref
extern const mtb_hal_clock_t CYBSP_I2C_CAM_CONTROLLER_hal_clock;
#define CYBSP_I2C_DISPLAY_CONTROLLER_hal_clock CYBSP_I2C_CAM_CONTROLLER_hal_clock
#endif /* defined (COMPONENT_MTB_HAL) */

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_I2C)
extern const mtb_hal_i2c_configurator_t CYBSP_I2C_CAM_CONTROLLER_hal_config;
#define CYBSP_I2C_DISPLAY_CONTROLLER_hal_config CYBSP_I2C_CAM_CONTROLLER_hal_config
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_I2C) */

extern cy_en_sd_host_card_capacity_t CYBSP_WIFI_SDIO_cardCapacity;
extern cy_en_sd_host_card_type_t CYBSP_WIFI_SDIO_cardType;
extern uint32_t CYBSP_WIFI_SDIO_rca;
extern const cy_stc_sd_host_init_config_t CYBSP_WIFI_SDIO_config;
extern cy_stc_sd_host_sd_card_config_t CYBSP_WIFI_SDIO_card_cfg;

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_SDHC)
extern const mtb_hal_peri_div_t CYBSP_WIFI_SDIO_clock_ref;
extern const mtb_hal_clock_t CYBSP_WIFI_SDIO_hal_clock;
extern const mtb_hal_sdhc_configurator_t CYBSP_WIFI_SDIO_sdhc_hal_config;
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_SDHC) */

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_SDIO)
extern const mtb_hal_sdio_configurator_t CYBSP_WIFI_SDIO_sdio_hal_config;
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_SDIO) */

extern const cy_stc_smif_config_t CYBSP_SMIF_CORE_0_XSPI_FLASH_config;

#if defined (COMPONENT_MTB_HAL)
extern const mtb_hal_hf_clock_t CYBSP_SMIF_CORE_0_XSPI_FLASH_clock_ref;
extern const mtb_hal_clock_t CYBSP_SMIF_CORE_0_XSPI_FLASH_hal_clock;
#endif /* defined (COMPONENT_MTB_HAL) */

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_MEMORYSPI)
extern const mtb_hal_memoryspi_configurator_t CYBSP_SMIF_CORE_0_XSPI_FLASH_hal_config;
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_MEMORYSPI) */

extern const cy_stc_smif_config_t CYBSP_SMIF_CORE_1_PSRAM_config;

#if defined (COMPONENT_MTB_HAL)
extern const mtb_hal_hf_clock_t CYBSP_SMIF_CORE_1_PSRAM_clock_ref;
extern const mtb_hal_clock_t CYBSP_SMIF_CORE_1_PSRAM_hal_clock;
#endif /* defined (COMPONENT_MTB_HAL) */

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_MEMORYSPI)
extern const mtb_hal_memoryspi_configurator_t CYBSP_SMIF_CORE_1_PSRAM_hal_config;
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_MEMORYSPI) */

extern const cy_stc_mcwdt_config_t CYBSP_CM33_LPTIMER_0_config;

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_LPTIMER)
extern const mtb_hal_lptimer_configurator_t CYBSP_CM33_LPTIMER_0_hal_config;
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_LPTIMER) */

extern const cy_stc_mcwdt_config_t CYBSP_CM55_LPTIMER_1_config;

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_LPTIMER)
extern const mtb_hal_lptimer_configurator_t CYBSP_CM55_LPTIMER_1_hal_config;
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_LPTIMER) */

extern cy_stc_tdm_config_tx_t CYBSP_TDM_CONTROLLER_0_tx_config;
extern cy_stc_tdm_config_rx_t CYBSP_TDM_CONTROLLER_0_rx_config;
extern const cy_stc_tdm_config_t CYBSP_TDM_CONTROLLER_0_config;
extern const cy_stc_tcpwm_pwm_config_t CYBSP_PWM_LED_CTRL_config;

#if defined (COMPONENT_MTB_HAL)
extern const mtb_hal_peri_div_t CYBSP_PWM_LED_CTRL_clock_ref;
extern const mtb_hal_clock_t CYBSP_PWM_LED_CTRL_hal_clock;
#endif /* defined (COMPONENT_MTB_HAL) */

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_PWM)
extern const mtb_hal_pwm_configurator_t CYBSP_PWM_LED_CTRL_hal_config;
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_PWM) */

extern cy_stc_tcpwm_counter_config_t emUSB_OS_Timer_config;

#if defined (COMPONENT_MTB_HAL)
extern mtb_hal_peri_div_t emUSB_OS_Timer_clock_ref;
extern mtb_hal_clock_t emUSB_OS_Timer_hal_clock;
#endif /* defined (COMPONENT_MTB_HAL) */

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_TIMER)
extern mtb_hal_timer_configurator_t emUSB_OS_Timer_hal_config;
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_TIMER) */

extern const cy_stc_tcpwm_counter_config_t CYBSP_GENERAL_PURPOSE_TIMER_config;

#if defined (COMPONENT_MTB_HAL)
extern const mtb_hal_peri_div_t CYBSP_GENERAL_PURPOSE_TIMER_clock_ref;
extern const mtb_hal_clock_t CYBSP_GENERAL_PURPOSE_TIMER_hal_clock;
#endif /* defined (COMPONENT_MTB_HAL) */

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_TIMER)
extern const mtb_hal_timer_configurator_t CYBSP_GENERAL_PURPOSE_TIMER_hal_config;
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_TIMER) */

extern const cy_stc_tcpwm_pwm_config_t CYBSP_PWM_DVP_CAM_CTRL_config;

#if defined (COMPONENT_MTB_HAL)
extern const mtb_hal_peri_div_t CYBSP_PWM_DVP_CAM_CTRL_clock_ref;
extern const mtb_hal_clock_t CYBSP_PWM_DVP_CAM_CTRL_hal_clock;
#endif /* defined (COMPONENT_MTB_HAL) */

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_PWM)
extern const mtb_hal_pwm_configurator_t CYBSP_PWM_DVP_CAM_CTRL_hal_config;
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_PWM) */

extern const cy_stc_tcpwm_pwm_config_t CYBSP_DEAD_TIME_PWM_config;

#if defined (COMPONENT_MTB_HAL)
extern const mtb_hal_peri_div_t CYBSP_DEAD_TIME_PWM_clock_ref;
extern const mtb_hal_clock_t CYBSP_DEAD_TIME_PWM_hal_clock;
#endif /* defined (COMPONENT_MTB_HAL) */

#if defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_PWM)
extern const mtb_hal_pwm_configurator_t CYBSP_DEAD_TIME_PWM_hal_config;
#endif /* defined (COMPONENT_MTB_HAL) && (MTB_HAL_DRIVER_AVAILABLE_PWM) */

void init_cycfg_peripherals(void);

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

#endif /* CYCFG_PERIPHERALS_H */
