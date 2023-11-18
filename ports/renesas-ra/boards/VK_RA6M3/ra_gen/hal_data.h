/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_usb_basic.h"
#include "r_usb_basic_api.h"
#include "r_usb_pcdc_api.h"
#include "r_jpeg.h"
#include "r_dtc.h"
#include "r_transfer_api.h"
#include "r_pdc.h"
#include "r_pdc_api.h"
#include "r_sci_i2c.h"
#include "r_i2c_master_api.h"
#include "r_glcdc.h"
#include "r_display_api.h"
#include "r_ether_phy.h"
#include "r_ether_phy_api.h"
#include "r_ether.h"
#include "r_ether_api.h"
#include "r_sdhi.h"
#include "r_sdmmc_api.h"
#include "r_qspi.h"
#include "r_spi_flash_api.h"
#include "r_iic_master.h"
#include "r_i2c_master_api.h"
#include "r_adc.h"
#include "r_adc_api.h"
#include "r_lpm.h"
#include "r_lpm_api.h"
#include "r_spi.h"
#include "r_agt.h"
#include "r_timer_api.h"
#include "r_flash_hp.h"
#include "r_flash_api.h"
#include "r_rtc.h"
#include "r_rtc_api.h"
#include "r_sci_uart.h"
#include "r_uart_api.h"
FSP_HEADER
/* Basic on USB Instance. */
extern const usb_instance_t g_basic0;

/** Access the USB instance using these structures when calling API functions directly (::p_api is not used). */
extern usb_instance_ctrl_t g_basic0_ctrl;
extern const usb_cfg_t g_basic0_cfg;

#ifndef NULL
void NULL(void *);
#endif

#if 2 == BSP_CFG_RTOS
#ifndef NULL
void NULL(usb_event_info_t *, usb_hdl_t, usb_onoff_t);
#endif
#endif
/** CDC Driver on USB Instance. */
extern const jpeg_instance_t g_jpeg0;

/** Access the JPEG instance using these structures when calling API functions directly (::p_api is not used). */
extern jpeg_instance_ctrl_t g_jpeg0_ctrl;
extern const jpeg_cfg_t g_jpeg0_cfg;

#if JPEG_CFG_DECODE_ENABLE
#ifndef NULL
void NULL(jpeg_callback_args_t *p_args);
#endif
#endif

#if JPEG_CFG_ENCODE_ENABLE
#ifndef jpg_ISR
void jpg_ISR(jpeg_callback_args_t *p_args);
#endif
#endif
/* Transfer on DTC Instance. */
extern const transfer_instance_t g_transfer5;

/** Access the DTC instance using these structures when calling API functions directly (::p_api is not used). */
extern dtc_instance_ctrl_t g_transfer5_ctrl;
extern const transfer_cfg_t g_transfer5_cfg;
/** PDC on PDC instance */
extern const pdc_instance_t g_cam0;
/** Access the PDC instance using these structures when calling API functions directly (::p_api is not used). */
extern pdc_instance_ctrl_t g_cam0_ctrl;
extern pdc_cfg_t g_cam0_cfg;
#ifndef cam_ISR
void cam_ISR(pdc_callback_args_t *p_args);
#endif
#ifdef g_cam_buffer
#define PDC_ON_PDC_BUFFER_USED_g_cam0 (0)
#else
#define PDC_ON_PDC_BUFFER_USED_g_cam0 (1)
#endif
#if PDC_ON_PDC_BUFFER_USED_g_cam0
/** Buffer where captured image will be stored for application usage */
extern uint8_t g_cam_buffer[1][320 * 240 * 2];
#endif
/* Transfer on DTC Instance. */
extern const transfer_instance_t g_transfer4;

/** Access the DTC instance using these structures when calling API functions directly (::p_api is not used). */
extern dtc_instance_ctrl_t g_transfer4_ctrl;
extern const transfer_cfg_t g_transfer4_cfg;
/* Transfer on DTC Instance. */
extern const transfer_instance_t g_transfer3;

/** Access the DTC instance using these structures when calling API functions directly (::p_api is not used). */
extern dtc_instance_ctrl_t g_transfer3_ctrl;
extern const transfer_cfg_t g_transfer3_cfg;
extern const i2c_master_cfg_t g_i2c_touch_cfg;
/* I2C on SCI Instance. */
extern const i2c_master_instance_t g_i2c_touch;
#ifndef touch_i2c_callback
void touch_i2c_callback(i2c_master_callback_args_t *p_args);
#endif

extern const sci_i2c_extended_cfg_t g_i2c_touch_cfg_extend;
extern sci_i2c_instance_ctrl_t g_i2c_touch_ctrl;
#define GLCDC_CFG_LAYER_1_ENABLE (true)
#define GLCDC_CFG_LAYER_2_ENABLE (false)

#define GLCDC_CFG_CLUT_ENABLE (false)

#define GLCDC_CFG_CORRECTION_GAMMA_ENABLE_R       (false)
#define GLCDC_CFG_CORRECTION_GAMMA_ENABLE_G       (false)
#define GLCDC_CFG_CORRECTION_GAMMA_ENABLE_B       (false)

/* Display on GLCDC Instance. */
extern const display_instance_t g_display0;
extern display_runtime_cfg_t g_display0_runtime_cfg_fg;
extern display_runtime_cfg_t g_display0_runtime_cfg_bg;

/** Access the GLCDC instance using these structures when calling API functions directly (::p_api is not used). */
extern glcdc_instance_ctrl_t g_display0_ctrl;
extern const display_cfg_t g_display0_cfg;

#if ((GLCDC_CFG_CORRECTION_GAMMA_ENABLE_R | GLCDC_CFG_CORRECTION_GAMMA_ENABLE_G | GLCDC_CFG_CORRECTION_GAMMA_ENABLE_B) && GLCDC_CFG_COLOR_CORRECTION_ENABLE && !(false))
extern display_gamma_correction_t g_display0_gamma_cfg;
#endif

#if GLCDC_CFG_CLUT_ENABLE
extern display_clut_cfg_t g_display0_clut_cfg_glcdc;
#endif

#ifndef lcd_Vsync_ISR
void lcd_Vsync_ISR(display_callback_args_t *p_args);
#endif

#define DISPLAY_IN_FORMAT_16BITS_RGB565_0
#if defined(DISPLAY_IN_FORMAT_32BITS_RGB888_0) || defined(DISPLAY_IN_FORMAT_32BITS_ARGB8888_0)
#define DISPLAY_BITS_PER_PIXEL_INPUT0 (32)
#elif defined(DISPLAY_IN_FORMAT_16BITS_RGB565_0) || defined(DISPLAY_IN_FORMAT_16BITS_ARGB1555_0) || defined(DISPLAY_IN_FORMAT_16BITS_ARGB4444_0)
#define DISPLAY_BITS_PER_PIXEL_INPUT0 (16)
#elif defined(DISPLAY_IN_FORMAT_CLUT8_0)
#define DISPLAY_BITS_PER_PIXEL_INPUT0 (8)
#elif defined(DISPLAY_IN_FORMAT_CLUT4_0)
#define DISPLAY_BITS_PER_PIXEL_INPUT0 (4)
#else
#define DISPLAY_BITS_PER_PIXEL_INPUT0 (1)
#endif
#define DISPLAY_HSIZE_INPUT0                 (480)
#define DISPLAY_VSIZE_INPUT0                 (272)
#define DISPLAY_BUFFER_STRIDE_BYTES_INPUT0   (((DISPLAY_HSIZE_INPUT0 * DISPLAY_BITS_PER_PIXEL_INPUT0 + 0x1FF) >> 9) << 6)
#define DISPLAY_BUFFER_STRIDE_PIXELS_INPUT0  ((DISPLAY_BUFFER_STRIDE_BYTES_INPUT0 * 8) / DISPLAY_BITS_PER_PIXEL_INPUT0)
#if GLCDC_CFG_LAYER_1_ENABLE
extern uint8_t fb_background[1][DISPLAY_BUFFER_STRIDE_BYTES_INPUT0 * DISPLAY_VSIZE_INPUT0];
#endif

#define DISPLAY_IN_FORMAT_16BITS_RGB565_1
#if defined(DISPLAY_IN_FORMAT_32BITS_RGB888_1) || defined(DISPLAY_IN_FORMAT_32BITS_ARGB8888_1)
#define DISPLAY_BITS_PER_PIXEL_INPUT1 (32)
#elif defined(DISPLAY_IN_FORMAT_16BITS_RGB565_1) || defined(DISPLAY_IN_FORMAT_16BITS_ARGB1555_1) || defined(DISPLAY_IN_FORMAT_16BITS_ARGB4444_1)
#define DISPLAY_BITS_PER_PIXEL_INPUT1 (16)
#elif defined(DISPLAY_IN_FORMAT_CLUT8_1)
#define DISPLAY_BITS_PER_PIXEL_INPUT1 (8)
#elif defined(DISPLAY_IN_FORMAT_CLUT4_1)
#define DISPLAY_BITS_PER_PIXEL_INPUT1 (4)
#else
#define DISPLAY_BITS_PER_PIXEL_INPUT1 (1)
#endif
#define DISPLAY_HSIZE_INPUT1                 (480)
#define DISPLAY_VSIZE_INPUT1                 (272)
#define DISPLAY_BUFFER_STRIDE_BYTES_INPUT1   (((DISPLAY_HSIZE_INPUT1 * DISPLAY_BITS_PER_PIXEL_INPUT1 + 0x1FF) >> 9) << 6)
#define DISPLAY_BUFFER_STRIDE_PIXELS_INPUT1  ((DISPLAY_BUFFER_STRIDE_BYTES_INPUT1 * 8) / DISPLAY_BITS_PER_PIXEL_INPUT1)
#if GLCDC_CFG_LAYER_2_ENABLE
extern uint8_t fb_foreground[2][DISPLAY_BUFFER_STRIDE_BYTES_INPUT1 * DISPLAY_VSIZE_INPUT1];
#endif
#ifndef ETHER_PHY_LSI_TYPE_KIT_COMPONENT
#define ETHER_PHY_LSI_TYPE_KIT_COMPONENT ETHER_PHY_LSI_TYPE_DEFAULT
#endif

/** ether_phy on ether_phy Instance. */
extern const ether_phy_instance_t g_ether_phy0;

/** Access the Ethernet PHY instance using these structures when calling API functions directly (::p_api is not used). */
extern ether_phy_instance_ctrl_t g_ether_phy0_ctrl;
extern const ether_phy_cfg_t g_ether_phy0_cfg;
#if (BSP_FEATURE_TZ_HAS_TRUSTZONE == 1) && (BSP_TZ_SECURE_BUILD != 1) && (BSP_TZ_NONSECURE_BUILD != 1) && (BSP_FEATURE_ETHER_SUPPORTS_TZ_SECURE == 0)
#define ETHER_BUFFER_PLACE_IN_SECTION BSP_PLACE_IN_SECTION(".ns_buffer.eth")
#else
#define ETHER_BUFFER_PLACE_IN_SECTION
#endif

/** ether on ether Instance. */
extern const ether_instance_t g_ether0;

/** Access the Ethernet instance using these structures when calling API functions directly (::p_api is not used). */
extern ether_instance_ctrl_t g_ether0_ctrl;
extern const ether_cfg_t g_ether0_cfg;

#ifndef ETH_IRQHandler
void ETH_IRQHandler(ether_callback_args_t *p_args);
#endif
/* Transfer on DTC Instance. */
extern const transfer_instance_t g_transfer2;

/** Access the DTC instance using these structures when calling API functions directly (::p_api is not used). */
extern dtc_instance_ctrl_t g_transfer2_ctrl;
extern const transfer_cfg_t g_transfer2_cfg;
/** SDMMC on SDMMC Instance. */
extern const sdmmc_instance_t g_sdmmc0;

/** Access the SDMMC instance using these structures when calling API functions directly (::p_api is not used). */
extern sdhi_instance_ctrl_t g_sdmmc0_ctrl;
extern sdmmc_cfg_t g_sdmmc0_cfg;

#ifndef sdhi_ISR
void sdhi_ISR(sdmmc_callback_args_t *p_args);
#endif
extern const spi_flash_instance_t g_qspi0;
extern qspi_instance_ctrl_t g_qspi0_ctrl;
extern const spi_flash_cfg_t g_qspi0_cfg;
/* I2C Master on IIC Instance. */
extern const i2c_master_instance_t g_i2c_master1;

/** Access the I2C Master instance using these structures when calling API functions directly (::p_api is not used). */
extern iic_master_instance_ctrl_t g_i2c_master1_ctrl;
extern const i2c_master_cfg_t g_i2c_master1_cfg;

#ifndef callback_iic
void callback_iic(i2c_master_callback_args_t *p_args);
#endif
/** ADC on ADC Instance. */
extern const adc_instance_t g_adc1;

/** Access the ADC instance using these structures when calling API functions directly (::p_api is not used). */
extern adc_instance_ctrl_t g_adc1_ctrl;
extern const adc_cfg_t g_adc1_cfg;
extern const adc_channel_cfg_t g_adc1_channel_cfg;

#ifndef NULL
void NULL(adc_callback_args_t *p_args);
#endif

#ifndef NULL
#define ADC_DMAC_CHANNELS_PER_BLOCK_NULL  0
#endif
/** ADC on ADC Instance. */
extern const adc_instance_t g_adc0;

/** Access the ADC instance using these structures when calling API functions directly (::p_api is not used). */
extern adc_instance_ctrl_t g_adc0_ctrl;
extern const adc_cfg_t g_adc0_cfg;
extern const adc_channel_cfg_t g_adc0_channel_cfg;

#ifndef NULL
void NULL(adc_callback_args_t *p_args);
#endif

#ifndef NULL
#define ADC_DMAC_CHANNELS_PER_BLOCK_NULL  0
#endif
/** lpm Instance */
extern const lpm_instance_t g_lpm0;

/** Access the LPM instance using these structures when calling API functions directly (::p_api is not used). */
extern lpm_instance_ctrl_t g_lpm0_ctrl;
extern const lpm_cfg_t g_lpm0_cfg;
/* Transfer on DTC Instance. */
extern const transfer_instance_t g_transfer1;

/** Access the DTC instance using these structures when calling API functions directly (::p_api is not used). */
extern dtc_instance_ctrl_t g_transfer1_ctrl;
extern const transfer_cfg_t g_transfer1_cfg;
/* Transfer on DTC Instance. */
extern const transfer_instance_t g_transfer0;

/** Access the DTC instance using these structures when calling API functions directly (::p_api is not used). */
extern dtc_instance_ctrl_t g_transfer0_ctrl;
extern const transfer_cfg_t g_transfer0_cfg;
/** SPI on SPI Instance. */
extern const spi_instance_t g_spi1;

/** Access the SPI instance using these structures when calling API functions directly (::p_api is not used). */
extern spi_instance_ctrl_t g_spi1_ctrl;
extern const spi_cfg_t g_spi1_cfg;

/** Callback used by SPI Instance. */
#ifndef spi_callback
void spi_callback(spi_callback_args_t *p_args);
#endif

#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == g_transfer0)
#define g_spi1_P_TRANSFER_TX (NULL)
#else
#define g_spi1_P_TRANSFER_TX (&g_transfer0)
#endif
#if (RA_NOT_DEFINED == g_transfer1)
#define g_spi1_P_TRANSFER_RX (NULL)
#else
#define g_spi1_P_TRANSFER_RX (&g_transfer1)
#endif
#undef RA_NOT_DEFINED
/** AGT Timer Instance */
extern const timer_instance_t g_timer1;

/** Access the AGT instance using these structures when calling API functions directly (::p_api is not used). */
extern agt_instance_ctrl_t g_timer1_ctrl;
extern const timer_cfg_t g_timer1_cfg;

#ifndef callback_agt
void callback_agt(timer_callback_args_t *p_args);
#endif
/** AGT Timer Instance */
extern const timer_instance_t g_timer0;

/** Access the AGT instance using these structures when calling API functions directly (::p_api is not used). */
extern agt_instance_ctrl_t g_timer0_ctrl;
extern const timer_cfg_t g_timer0_cfg;

#ifndef callback_agt
void callback_agt(timer_callback_args_t *p_args);
#endif
/* Flash on Flash HP Instance */
extern const flash_instance_t g_flash0;

/** Access the Flash HP instance using these structures when calling API functions directly (::p_api is not used). */
extern flash_hp_instance_ctrl_t g_flash0_ctrl;
extern const flash_cfg_t g_flash0_cfg;

#ifndef NULL
void NULL(flash_callback_args_t *p_args);
#endif
/* RTC Instance. */
extern const rtc_instance_t g_rtc0;

/** Access the RTC instance using these structures when calling API functions directly (::p_api is not used). */
extern rtc_instance_ctrl_t g_rtc0_ctrl;
extern const rtc_cfg_t g_rtc0_cfg;

#ifndef NULL
void NULL(rtc_callback_args_t *p_args);
#endif
/** UART on SCI Instance. */
extern const uart_instance_t g_uart9;

/** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_uart_instance_ctrl_t g_uart9_ctrl;
extern const uart_cfg_t g_uart9_cfg;
extern const sci_uart_extended_cfg_t g_uart9_cfg_extend;

#ifndef user_uart_callback
void user_uart_callback(uart_callback_args_t *p_args);
#endif
/** UART on SCI Instance. */
extern const uart_instance_t g_uart8;

/** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_uart_instance_ctrl_t g_uart8_ctrl;
extern const uart_cfg_t g_uart8_cfg;
extern const sci_uart_extended_cfg_t g_uart8_cfg_extend;

#ifndef user_uart_callback
void user_uart_callback(uart_callback_args_t *p_args);
#endif
/** UART on SCI Instance. */
extern const uart_instance_t g_uart7;

/** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_uart_instance_ctrl_t g_uart7_ctrl;
extern const uart_cfg_t g_uart7_cfg;
extern const sci_uart_extended_cfg_t g_uart7_cfg_extend;

#ifndef user_uart_callback
void user_uart_callback(uart_callback_args_t *p_args);
#endif
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
