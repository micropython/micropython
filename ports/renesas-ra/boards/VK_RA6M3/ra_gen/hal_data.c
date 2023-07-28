/* generated HAL source file - do not edit */
#include "hal_data.h"

/* Macros to tie dynamic ELC links to ADC_TRIGGER_SYNC_ELC option in adc_trigger_t. */
#define ADC_TRIGGER_ADC0        ADC_TRIGGER_SYNC_ELC
#define ADC_TRIGGER_ADC0_B      ADC_TRIGGER_SYNC_ELC
#define ADC_TRIGGER_ADC1        ADC_TRIGGER_SYNC_ELC
#define ADC_TRIGGER_ADC1_B      ADC_TRIGGER_SYNC_ELC
usb_instance_ctrl_t g_basic0_ctrl;

#if !defined(g_usb_descriptor)
extern usb_descriptor_t g_usb_descriptor;
#endif
#define RA_NOT_DEFINED (1)
const usb_cfg_t g_basic0_cfg = { .usb_mode = USB_MODE_PERI, .usb_speed =
                                     USB_SPEED_FS, .module_number = 0, .type = USB_CLASS_PCDC,
                                 #if defined(g_usb_descriptor)
                                 .p_usb_reg = g_usb_descriptor,
                                 #else
                                 .p_usb_reg = &g_usb_descriptor,
                                 #endif
                                 .usb_complience_cb = NULL,
                                 #if defined(VECTOR_NUMBER_USBFS_INT)
                                 .irq = VECTOR_NUMBER_USBFS_INT,
                                 #else
                                 .irq = FSP_INVALID_VECTOR,
                                 #endif
                                 #if defined(VECTOR_NUMBER_USBFS_RESUME)
                                 .irq_r = VECTOR_NUMBER_USBFS_RESUME,
                                 #else
                                 .irq_r = FSP_INVALID_VECTOR,
                                 #endif
                                 #if defined(VECTOR_NUMBER_USBFS_FIFO_0)
                                 .irq_d0 = VECTOR_NUMBER_USBFS_FIFO_0,
                                 #else
                                 .irq_d0 = FSP_INVALID_VECTOR,
                                 #endif
                                 #if defined(VECTOR_NUMBER_USBFS_FIFO_1)
                                 .irq_d1 = VECTOR_NUMBER_USBFS_FIFO_1,
                                 #else
                                 .irq_d1 = FSP_INVALID_VECTOR,
                                 #endif
                                 #if defined(VECTOR_NUMBER_USBHS_USB_INT_RESUME)
                                 .hsirq = VECTOR_NUMBER_USBHS_USB_INT_RESUME,
                                 #else
                                 .hsirq = FSP_INVALID_VECTOR,
                                 #endif
                                 #if defined(VECTOR_NUMBER_USBHS_FIFO_0)
                                 .hsirq_d0 = VECTOR_NUMBER_USBHS_FIFO_0,
                                 #else
                                 .hsirq_d0 = FSP_INVALID_VECTOR,
                                 #endif
                                 #if defined(VECTOR_NUMBER_USBHS_FIFO_1)
                                 .hsirq_d1 = VECTOR_NUMBER_USBHS_FIFO_1,
                                 #else
                                 .hsirq_d1 = FSP_INVALID_VECTOR,
                                 #endif
                                 .ipl = (12), .ipl_r = (12), .ipl_d0 = (12), .ipl_d1 = (12), .hsipl =
                                     (12), .hsipl_d0 = (12), .hsipl_d1 = (12),
                                 #if (BSP_CFG_RTOS != 0)
                                 .p_usb_apl_callback = NULL,
                                 #else
                                 .p_usb_apl_callback = NULL,
                                 #endif
                                 #if defined(NULL)
                                 .p_context = NULL,
                                 #else
                                 .p_context = &NULL,
                                 #endif
                                 #if (RA_NOT_DEFINED == RA_NOT_DEFINED)
                                 #else
                                 .p_transfer_tx = &RA_NOT_DEFINED,
                                 #endif
                                 #if (RA_NOT_DEFINED == RA_NOT_DEFINED)
                                 #else
                                 .p_transfer_rx = &RA_NOT_DEFINED,
                                 #endif
};
#undef RA_NOT_DEFINED

/* Instance structure to use this module. */
const usb_instance_t g_basic0 = { .p_ctrl = &g_basic0_ctrl, .p_cfg =
                                      &g_basic0_cfg, .p_api = &g_usb_on_usb, };

#if JPEG_CFG_ENCODE_ENABLE

/* Luminance quantization table */ static const uint8_t quant_table_0[] = {16, 11, 10, 16, 24, 40, 51, 61, 12, 12, 14, 19, 26, 58, 60, 55, 14, 13, 16, 24, 40, 57, 69, 56, 14, 17, 22, 29, 51, 87, 80, 62, 18, 22, 37, 56, 68, 109, 103, 77, 24, 35, 55, 64, 81, 104, 113, 92, 49, 64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99, }; /* Chrominance quantization table */ static const uint8_t quant_table_1[] = {17, 18, 24, 47, 99, 99, 99, 99, 18, 21, 26, 66, 99, 99, 99, 99, 24, 26, 56, 99, 99, 99, 99, 99, 47, 66, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, };

/* Luminance DC coefficients */
static const uint8_t huffm_dc_table_0[] =
{
    /* Table K.3 ITU-T81 specification */
    0x00, 0x01, 0x05, 0x01,
    0x01, 0x01, 0x01, 0x01,
    0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B,
};

/* Chrominance DC coefficients */
static const uint8_t huffm_dc_table_1[] =
{
    /* Table K.4 ITU-T81 specification */
    0x00, 0x03, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B,
};

/* Luminance AC coefficients */
static const uint8_t huffm_ac_table_0[] =
{
    /* Table K.5 ITU-T81 specification */
    0x00, 0x02, 0x01, 0x03,
    0x03, 0x02, 0x04, 0x03,
    0x05, 0x05, 0x04, 0x04,
    0x00, 0x00, 0x01, 0x7D,
    0x01, 0x02, 0x03, 0x00,
    0x04, 0x11, 0x05, 0x12,
    0x21, 0x31, 0x41, 0x06,
    0x13, 0x51, 0x61, 0x07,
    0x22, 0x71, 0x14, 0x32,
    0x81, 0x91, 0xA1, 0x08,
    0x23, 0x42, 0xB1, 0xC1,
    0x15, 0x52, 0xD1, 0xF0,
    0x24, 0x33, 0x62, 0x72,
    0x82, 0x09, 0x0A, 0x16,
    0x17, 0x18, 0x19, 0x1A,
    0x25, 0x26, 0x27, 0x28,
    0x29, 0x2A, 0x34, 0x35,
    0x36, 0x37, 0x38, 0x39,
    0x3A, 0x43, 0x44, 0x45,
    0x46, 0x47, 0x48, 0x49,
    0x4A, 0x53, 0x54, 0x55,
    0x56, 0x57, 0x58, 0x59,
    0x5A, 0x63, 0x64, 0x65,
    0x66, 0x67, 0x68, 0x69,
    0x6A, 0x73, 0x74, 0x75,
    0x76, 0x77, 0x78, 0x79,
    0x7A, 0x83, 0x84, 0x85,
    0x86, 0x87, 0x88, 0x89,
    0x8A, 0x92, 0x93, 0x94,
    0x95, 0x96, 0x97, 0x98,
    0x99, 0x9A, 0xA2, 0xA3,
    0xA4, 0xA5, 0xA6, 0xA7,
    0xA8, 0xA9, 0xAA, 0xB2,
    0xB3, 0xB4, 0xB5, 0xB6,
    0xB7, 0xB8, 0xB9, 0xBA,
    0xC2, 0xC3, 0xC4, 0xC5,
    0xC6, 0xC7, 0xC8, 0xC9,
    0xCA, 0xD2, 0xD3, 0xD4,
    0xD5, 0xD6, 0xD7, 0xD8,
    0xD9, 0xDA, 0xE1, 0xE2,
    0xE3, 0xE4, 0xE5, 0xE6,
    0xE7, 0xE8, 0xE9, 0xEA,
    0xF1, 0xF2, 0xF3, 0xF4,
    0xF5, 0xF6, 0xF7, 0xF8,
    0xF9, 0xFA,
};

/* Chrominance AC coefficients */
static const uint8_t huffm_ac_table_1[] =
{
    /* Table K.6 ITU-T81 specification */
    0x00, 0x02, 0x01, 0x02,
    0x04, 0x04, 0x03, 0x04,
    0x07, 0x05, 0x04, 0x04,
    0x00, 0x01, 0x02, 0x77,
    0x00, 0x01, 0x02, 0x03,
    0x11, 0x04, 0x05, 0x21,
    0x31, 0x06, 0x12, 0x41,
    0x51, 0x07, 0x61, 0x71,
    0x13, 0x22, 0x32, 0x81,
    0x08, 0x14, 0x42, 0x91,
    0xA1, 0xB1, 0xC1, 0x09,
    0x23, 0x33, 0x52, 0xF0,
    0x15, 0x62, 0x72, 0xD1,
    0x0A, 0x16, 0x24, 0x34,
    0xE1, 0x25, 0xF1, 0x17,
    0x18, 0x19, 0x1A, 0x26,
    0x27, 0x28, 0x29, 0x2A,
    0x35, 0x36, 0x37, 0x38,
    0x39, 0x3A, 0x43, 0x44,
    0x45, 0x46, 0x47, 0x48,
    0x49, 0x4A, 0x53, 0x54,
    0x55, 0x56, 0x57, 0x58,
    0x59, 0x5A, 0x63, 0x64,
    0x65, 0x66, 0x67, 0x68,
    0x69, 0x6A, 0x73, 0x74,
    0x75, 0x76, 0x77, 0x78,
    0x79, 0x7A, 0x82, 0x83,
    0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8A, 0x92,
    0x93, 0x94, 0x95, 0x96,
    0x97, 0x98, 0x99, 0x9A,
    0xA2, 0xA3, 0xA4, 0xA5,
    0xA6, 0xA7, 0xA8, 0xA9,
    0xAA, 0xB2, 0xB3, 0xB4,
    0xB5, 0xB6, 0xB7, 0xB8,
    0xB9, 0xBA, 0xC2, 0xC3,
    0xC4, 0xC5, 0xC6, 0xC7,
    0xC8, 0xC9, 0xCA, 0xD2,
    0xD3, 0xD4, 0xD5, 0xD6,
    0xD7, 0xD8, 0xD9, 0xDA,
    0xE2, 0xE3, 0xE4, 0xE5,
    0xE6, 0xE7, 0xE8, 0xE9,
    0xEA, 0xF2, 0xF3, 0xF4,
    0xF5, 0xF6, 0xF7, 0xF8,
    0xF9, 0xFA,
};
#endif

jpeg_instance_ctrl_t g_jpeg0_ctrl;
const jpeg_cfg_t g_jpeg0_cfg = { .jedi_ipl = (12), .jdti_ipl = (12),
                                 #if defined(VECTOR_NUMBER_JPEG_JEDI)
                                 .jedi_irq = VECTOR_NUMBER_JPEG_JEDI,
                                 #else
                                 .jedi_irq = FSP_INVALID_VECTOR,
                                 #endif
                                 .jdti_irq = VECTOR_NUMBER_JPEG_JDTI,

                                 #if JPEG_CFG_DECODE_ENABLE && JPEG_CFG_ENCODE_ENABLE
                                 .default_mode = (JPEG_MODE_ENCODE),
                                 #endif

                                 #if JPEG_CFG_DECODE_ENABLE
                                 .decode_input_data_order = JPEG_DATA_ORDER_NORMAL,
                                 .decode_output_data_order = JPEG_DATA_ORDER_NORMAL,
                                 .pixel_format = JPEG_DECODE_PIXEL_FORMAT_RGB565,
                                 .alpha_value = 255,
                                 .p_decode_callback = NULL,
                                 .p_decode_context = NULL,
                                 #endif

                                 #if JPEG_CFG_ENCODE_ENABLE
                                 .encode_input_data_order = JPEG_DATA_ORDER_NORMAL,
                                 .encode_output_data_order = JPEG_DATA_ORDER_NORMAL,
                                 .dri_marker = 0,
                                 .horizontal_resolution = 320,
                                 .vertical_resolution = 240,
                                 .horizontal_stride_pixels = 640,
                                 .p_quant_luma_table = quant_table_0,
                                 .p_quant_chroma_table = quant_table_1,
                                 .p_huffman_luma_ac_table = huffm_ac_table_0,
                                 .p_huffman_luma_dc_table = huffm_dc_table_0,
                                 .p_huffman_chroma_ac_table = huffm_ac_table_1,
                                 .p_huffman_chroma_dc_table = huffm_dc_table_1,
                                 .p_encode_callback = jpg_ISR,
                                 .p_encode_context = NULL,
                                 #endif
};

const jpeg_instance_t g_jpeg0 = { .p_api = (jpeg_api_t const *)&g_jpeg_on_jpeg,
                                  .p_ctrl = &g_jpeg0_ctrl, .p_cfg = &g_jpeg0_cfg };
dtc_instance_ctrl_t g_transfer5_ctrl;

transfer_info_t g_transfer5_info = { .transfer_settings_word_b.dest_addr_mode =
                                         TRANSFER_ADDR_MODE_INCREMENTED, .transfer_settings_word_b.repeat_area =
                                         TRANSFER_REPEAT_AREA_SOURCE, .transfer_settings_word_b.irq =
                                         TRANSFER_IRQ_END, .transfer_settings_word_b.chain_mode =
                                         TRANSFER_CHAIN_MODE_DISABLED, .transfer_settings_word_b.src_addr_mode =
                                         TRANSFER_ADDR_MODE_FIXED, .transfer_settings_word_b.size =
                                         TRANSFER_SIZE_4_BYTE, .transfer_settings_word_b.mode =
                                         TRANSFER_MODE_BLOCK, .p_dest = (void *)NULL,
                                     .p_src = (void const *)NULL, .num_blocks = 1, .length = 8, };

const dtc_extended_cfg_t g_transfer5_cfg_extend = { .activation_source =
                                                        VECTOR_NUMBER_PDC_RECEIVE_DATA_READY, };
const transfer_cfg_t g_transfer5_cfg = { .p_info = &g_transfer5_info,
                                         .p_extend = &g_transfer5_cfg_extend, };

/* Instance structure to use this module. */
const transfer_instance_t g_transfer5 = { .p_ctrl = &g_transfer5_ctrl, .p_cfg =
                                              &g_transfer5_cfg, .p_api = &g_transfer_on_dtc };
#define RA_NOT_DEFINED (UINT32_MAX)
#if (RA_NOT_DEFINED) != (RA_NOT_DEFINED)

/* If the transfer module is DMAC, define a DMAC transfer callback. */
#include "r_dmac.h"
extern void r_pdc_transfer_callback(pdc_instance_ctrl_t *p_ctrl);

void g_cam0_transfer_callback(dmac_callback_args_t *p_args) {
    r_pdc_transfer_callback((pdc_instance_ctrl_t *)p_args->p_context);
}
#endif
#undef RA_NOT_DEFINED
#if PDC_ON_PDC_BUFFER_USED_g_cam0
uint8_t g_cam_buffer[1][320 * 240 * 2] BSP_ALIGN_VARIABLE(32) BSP_PLACE_IN_SECTION(".noinit");
#endif
pdc_instance_ctrl_t g_cam0_ctrl;
pdc_cfg_t g_cam0_cfg = { .bytes_per_pixel = 2, .clock_division =
                             PDC_CLOCK_DIVISION_6, .endian = PDC_ENDIAN_LITTLE, .hsync_polarity =
                             PDC_HSYNC_POLARITY_HIGH, .vsync_polarity = PDC_VSYNC_POLARITY_LOW,
                         .p_buffer = (uint8_t *)g_cam_buffer, .p_callback = cam_ISR,
                         /** If NULL then do not add & */
                         #if defined(NULL)
                         .p_context = NULL,
                         #else
                         .p_context = &NULL,
                         #endif
                         .p_extend = NULL, .p_lower_lvl_transfer = &g_transfer5,
                         .x_capture_pixels = 320, .y_capture_pixels = 240,
                         .x_capture_start_pixel = 192, .y_capture_start_pixel = 26, .pdc_ipl =
                             (1), .transfer_req_ipl = (1),
                         #if defined(VECTOR_NUMBER_PDC_INT)
                         .pdc_irq = VECTOR_NUMBER_PDC_INT,
                         #else
                         .pdc_irq = FSP_INVALID_VECTOR,
                         #endif
                         #if defined(VECTOR_NUMBER_PDC_RECEIVE_DATA_READY)
                         .transfer_req_irq = VECTOR_NUMBER_PDC_RECEIVE_DATA_READY,
                         #else
                         .transfer_req_irq = FSP_INVALID_VECTOR,
                         #endif
};

const pdc_instance_t g_cam0 = { .p_ctrl = &g_cam0_ctrl, .p_cfg = &g_cam0_cfg,
                                .p_api = &g_pdc_on_pdc, };
dtc_instance_ctrl_t g_transfer4_ctrl;

transfer_info_t g_transfer4_info = { .transfer_settings_word_b.dest_addr_mode =
                                         TRANSFER_ADDR_MODE_INCREMENTED, .transfer_settings_word_b.repeat_area =
                                         TRANSFER_REPEAT_AREA_DESTINATION, .transfer_settings_word_b.irq =
                                         TRANSFER_IRQ_END, .transfer_settings_word_b.chain_mode =
                                         TRANSFER_CHAIN_MODE_DISABLED, .transfer_settings_word_b.src_addr_mode =
                                         TRANSFER_ADDR_MODE_FIXED, .transfer_settings_word_b.size =
                                         TRANSFER_SIZE_1_BYTE, .transfer_settings_word_b.mode =
                                         TRANSFER_MODE_NORMAL, .p_dest = (void *)NULL, .p_src =
                                         (void const *)NULL, .num_blocks = 0, .length = 0, };

const dtc_extended_cfg_t g_transfer4_cfg_extend = { .activation_source =
                                                        VECTOR_NUMBER_SCI0_RXI, };
const transfer_cfg_t g_transfer4_cfg = { .p_info = &g_transfer4_info,
                                         .p_extend = &g_transfer4_cfg_extend, };

/* Instance structure to use this module. */
const transfer_instance_t g_transfer4 = { .p_ctrl = &g_transfer4_ctrl, .p_cfg =
                                              &g_transfer4_cfg, .p_api = &g_transfer_on_dtc };
dtc_instance_ctrl_t g_transfer3_ctrl;

transfer_info_t g_transfer3_info = { .transfer_settings_word_b.dest_addr_mode =
                                         TRANSFER_ADDR_MODE_FIXED, .transfer_settings_word_b.repeat_area =
                                         TRANSFER_REPEAT_AREA_SOURCE, .transfer_settings_word_b.irq =
                                         TRANSFER_IRQ_END, .transfer_settings_word_b.chain_mode =
                                         TRANSFER_CHAIN_MODE_DISABLED, .transfer_settings_word_b.src_addr_mode =
                                         TRANSFER_ADDR_MODE_INCREMENTED, .transfer_settings_word_b.size =
                                         TRANSFER_SIZE_1_BYTE, .transfer_settings_word_b.mode =
                                         TRANSFER_MODE_NORMAL, .p_dest = (void *)NULL, .p_src =
                                         (void const *)NULL, .num_blocks = 0, .length = 0, };

const dtc_extended_cfg_t g_transfer3_cfg_extend = { .activation_source =
                                                        VECTOR_NUMBER_SCI0_TXI, };
const transfer_cfg_t g_transfer3_cfg = { .p_info = &g_transfer3_info,
                                         .p_extend = &g_transfer3_cfg_extend, };

/* Instance structure to use this module. */
const transfer_instance_t g_transfer3 = { .p_ctrl = &g_transfer3_ctrl, .p_cfg =
                                              &g_transfer3_cfg, .p_api = &g_transfer_on_dtc };
#include "r_sci_i2c_cfg.h"
sci_i2c_instance_ctrl_t g_i2c_touch_ctrl;
const sci_i2c_extended_cfg_t g_i2c_touch_cfg_extend =
{
    /* Actual calculated bitrate: 399693. Actual SDA delay: 300 ns. */ .clock_settings.clk_divisor_value =
        0, .clock_settings.brr_value = 6,
    .clock_settings.mddr_value = 191,
    .clock_settings.bitrate_modulation = true,
    .clock_settings.cycles_value = 31, .clock_settings.snfr_value =
        (1),
};

const i2c_master_cfg_t g_i2c_touch_cfg = { .channel = 0, .rate =
                                               I2C_MASTER_RATE_FAST, .slave = 0x38, .addr_mode =
                                               I2C_MASTER_ADDR_MODE_7BIT,
#define RA_NOT_DEFINED (1)
                                           #if (RA_NOT_DEFINED == g_transfer3)
                                           .p_transfer_tx = NULL,
                                           #else
                                           .p_transfer_tx = &g_transfer3,
                                           #endif
                                           #if (RA_NOT_DEFINED == g_transfer4)
                                           .p_transfer_rx = NULL,
                                           #else
                                           .p_transfer_rx = &g_transfer4,
                                           #endif
#undef RA_NOT_DEFINED
                                           .p_callback = touch_i2c_callback, .p_context = NULL,
                                           #if defined(VECTOR_NUMBER_SCI0_RXI) && SCI_I2C_CFG_DTC_ENABLE
                                           .rxi_irq = VECTOR_NUMBER_SCI0_RXI,
                                           #else
                                           .rxi_irq = FSP_INVALID_VECTOR,
                                           #endif
                                           #if defined(VECTOR_NUMBER_SCI0_TXI)
                                           .txi_irq = VECTOR_NUMBER_SCI0_TXI,
                                           #else
                                           .txi_irq = FSP_INVALID_VECTOR,
                                           #endif
                                           #if defined(VECTOR_NUMBER_SCI0_TEI)
                                           .tei_irq = VECTOR_NUMBER_SCI0_TEI,
                                           #else
                                           .tei_irq = FSP_INVALID_VECTOR,
                                           #endif
                                           .ipl = (6), /* (6) is unused */
                                           .p_extend = &g_i2c_touch_cfg_extend, };
/* Instance structure to use this module. */
const i2c_master_instance_t g_i2c_touch = { .p_ctrl = &g_i2c_touch_ctrl,
                                            .p_cfg = &g_i2c_touch_cfg, .p_api = &g_i2c_master_on_sci };
/** Display framebuffer */
#if GLCDC_CFG_LAYER_1_ENABLE
uint8_t fb_background[1][DISPLAY_BUFFER_STRIDE_BYTES_INPUT0 * DISPLAY_VSIZE_INPUT0] BSP_ALIGN_VARIABLE(64) BSP_PLACE_IN_SECTION(".fb0");
#else
/** Graphics Layer 1 is specified not to be used when starting */
#endif
#if GLCDC_CFG_LAYER_2_ENABLE
uint8_t fb_foreground[2][DISPLAY_BUFFER_STRIDE_BYTES_INPUT1 * DISPLAY_VSIZE_INPUT1] BSP_ALIGN_VARIABLE(64) BSP_PLACE_IN_SECTION(".bss");
#else
/** Graphics Layer 2 is specified not to be used when starting */
#endif

#if GLCDC_CFG_CLUT_ENABLE
/** Display CLUT buffer to be used for updating CLUT */
static uint32_t CLUT_buffer[256];

/** Display CLUT configuration(only used if using CLUT format) */
display_clut_cfg_t g_display0_clut_cfg_glcdc =
{
    .p_base = (uint32_t *)CLUT_buffer,
    .start = 0,                         /* User have to update this setting when using */
    .size = 256                         /* User have to update this setting when using */
};
#else
/** CLUT is specified not to be used */
#endif

#if (false)
#define GLCDC_CFG_CORRECTION_GAMMA_TABLE_CONST   const
#define GLCDC_CFG_CORRECTION_GAMMA_TABLE_CAST    (uint16_t *)
#define GLCDC_CFG_CORRECTION_GAMMA_CFG_CAST      (display_gamma_correction_t *)
#else
#define GLCDC_CFG_CORRECTION_GAMMA_TABLE_CONST
#define GLCDC_CFG_CORRECTION_GAMMA_TABLE_CAST
#define GLCDC_CFG_CORRECTION_GAMMA_CFG_CAST
#endif

#if ((GLCDC_CFG_CORRECTION_GAMMA_ENABLE_R | GLCDC_CFG_CORRECTION_GAMMA_ENABLE_G | GLCDC_CFG_CORRECTION_GAMMA_ENABLE_B) && GLCDC_CFG_COLOR_CORRECTION_ENABLE)
/** Gamma correction tables */
        #if GLCDC_CFG_CORRECTION_GAMMA_ENABLE_R
static GLCDC_CFG_CORRECTION_GAMMA_TABLE_CONST uint16_t glcdc_gamma_r_gain[DISPLAY_GAMMA_CURVE_ELEMENT_NUM] = {1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024};
static GLCDC_CFG_CORRECTION_GAMMA_TABLE_CONST uint16_t glcdc_gamma_r_threshold[DISPLAY_GAMMA_CURVE_ELEMENT_NUM] = {0, 64, 128, 192, 256, 320, 384, 448, 512, 576, 640, 704, 768, 832, 896, 960};
        #endif
        #if GLCDC_CFG_CORRECTION_GAMMA_ENABLE_G
static GLCDC_CFG_CORRECTION_GAMMA_TABLE_CONST uint16_t glcdc_gamma_g_gain[DISPLAY_GAMMA_CURVE_ELEMENT_NUM] = {1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024};
static GLCDC_CFG_CORRECTION_GAMMA_TABLE_CONST uint16_t glcdc_gamma_g_threshold[DISPLAY_GAMMA_CURVE_ELEMENT_NUM] = {0, 64, 128, 192, 256, 320, 384, 448, 512, 576, 640, 704, 768, 832, 896, 960};
        #endif
        #if GLCDC_CFG_CORRECTION_GAMMA_ENABLE_B
static GLCDC_CFG_CORRECTION_GAMMA_TABLE_CONST uint16_t glcdc_gamma_b_gain[DISPLAY_GAMMA_CURVE_ELEMENT_NUM] = {1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024};
static GLCDC_CFG_CORRECTION_GAMMA_TABLE_CONST uint16_t glcdc_gamma_b_threshold[DISPLAY_GAMMA_CURVE_ELEMENT_NUM] = {0, 64, 128, 192, 256, 320, 384, 448, 512, 576, 640, 704, 768, 832, 896, 960};
        #endif
GLCDC_CFG_CORRECTION_GAMMA_TABLE_CONST display_gamma_correction_t g_display0_gamma_cfg =
{
    .r =
    {
        .enable = GLCDC_CFG_CORRECTION_GAMMA_ENABLE_R,
        #if GLCDC_CFG_CORRECTION_GAMMA_ENABLE_R
        .gain = GLCDC_CFG_CORRECTION_GAMMA_TABLE_CAST glcdc_gamma_r_gain,
        .threshold = GLCDC_CFG_CORRECTION_GAMMA_TABLE_CAST glcdc_gamma_r_threshold
        #else
        .gain = NULL,
        .threshold = NULL
            #endif
    },
    .g =
    {
        .enable = GLCDC_CFG_CORRECTION_GAMMA_ENABLE_G,
        #if GLCDC_CFG_CORRECTION_GAMMA_ENABLE_G
        .gain = GLCDC_CFG_CORRECTION_GAMMA_TABLE_CAST glcdc_gamma_g_gain,
        .threshold = GLCDC_CFG_CORRECTION_GAMMA_TABLE_CAST glcdc_gamma_g_threshold
        #else
        .gain = NULL,
        .threshold = NULL
            #endif
    },
    .b =
    {
        .enable = GLCDC_CFG_CORRECTION_GAMMA_ENABLE_B,
        #if GLCDC_CFG_CORRECTION_GAMMA_ENABLE_B
        .gain = GLCDC_CFG_CORRECTION_GAMMA_TABLE_CAST glcdc_gamma_b_gain,
        .threshold = GLCDC_CFG_CORRECTION_GAMMA_TABLE_CAST glcdc_gamma_b_threshold
        #else
        .gain = NULL,
        .threshold = NULL
            #endif
    }
};
#endif

/** Display device extended configuration */
const glcdc_extended_cfg_t g_display0_extend_cfg = { .tcon_hsync =
                                                         GLCDC_TCON_PIN_0, .tcon_vsync = GLCDC_TCON_PIN_1, .tcon_de =
                                                         GLCDC_TCON_PIN_2, .correction_proc_order =
                                                         GLCDC_CORRECTION_PROC_ORDER_BRIGHTNESS_CONTRAST2GAMMA, .clksrc =
                                                         GLCDC_CLK_SRC_INTERNAL, .clock_div_ratio = GLCDC_PANEL_CLK_DIVISOR_24,
                                                     .dithering_mode = GLCDC_DITHERING_MODE_TRUNCATE, .dithering_pattern_A =
                                                         GLCDC_DITHERING_PATTERN_11, .dithering_pattern_B =
                                                         GLCDC_DITHERING_PATTERN_11, .dithering_pattern_C =
                                                         GLCDC_DITHERING_PATTERN_11, .dithering_pattern_D =
                                                         GLCDC_DITHERING_PATTERN_11 };

/** Display control block instance */
glcdc_instance_ctrl_t g_display0_ctrl;

/** Display interface configuration */
const display_cfg_t g_display0_cfg =
{
    /** Input1(Graphics1 layer) configuration */
    .input[0] =
    {
        #if GLCDC_CFG_LAYER_1_ENABLE
        .p_base = (uint32_t *)&fb_background[0],
        #else
        .p_base = NULL,
        #endif
        .hsize = DISPLAY_HSIZE_INPUT0, .vsize = DISPLAY_VSIZE_INPUT0,
        .hstride = DISPLAY_BUFFER_STRIDE_PIXELS_INPUT0, .format =
            DISPLAY_IN_FORMAT_16BITS_RGB565,
        .line_descending_enable = false, .lines_repeat_enable = false,
        .lines_repeat_times = 0
    },

    /** Input2(Graphics2 layer) configuration */
    .input[1] =
    {
        #if GLCDC_CFG_LAYER_2_ENABLE
        .p_base = (uint32_t *)&fb_foreground[0],
        #else
        .p_base = NULL,
        #endif
        .hsize = DISPLAY_HSIZE_INPUT1, .vsize = DISPLAY_VSIZE_INPUT1,
        .hstride = DISPLAY_BUFFER_STRIDE_PIXELS_INPUT1, .format =
            DISPLAY_IN_FORMAT_16BITS_RGB565,
        .line_descending_enable = false, .lines_repeat_enable = false,
        .lines_repeat_times = 0
    },

    /** Input1(Graphics1 layer) layer configuration */
    .layer[0] =
    { .coordinate = { .x = 0, .y = 0 }, .bg_color = { .byte = { .a = 255,
                                                                .r = 255, .g = 255, .b = 255 } }, .fade_control =
          DISPLAY_FADE_CONTROL_NONE, .fade_speed = 0 },

    /** Input2(Graphics2 layer) layer configuration */
    .layer[1] =
    { .coordinate = { .x = 0, .y = 0 }, .bg_color = { .byte = { .a = 255,
                                                                .r = 255, .g = 255, .b = 255 } }, .fade_control =
          DISPLAY_FADE_CONTROL_NONE, .fade_speed = 0 },

    /** Output configuration */
    .output =
    { .htiming = { .total_cyc = 525, .display_cyc = 480,
                   .back_porch = 40, .sync_width = 1,
                   .sync_polarity =
                       DISPLAY_SIGNAL_POLARITY_LOACTIVE },
      .vtiming =
      { .total_cyc = 316, .display_cyc = 272,
        .back_porch = 8,
        .sync_width = 1,
        .sync_polarity =
            DISPLAY_SIGNAL_POLARITY_LOACTIVE },
      .format = DISPLAY_OUT_FORMAT_24BITS_RGB888,
      .endian = DISPLAY_ENDIAN_LITTLE, .color_order =
          DISPLAY_COLOR_ORDER_RGB,
      .data_enable_polarity =
          DISPLAY_SIGNAL_POLARITY_HIACTIVE,
      .sync_edge = DISPLAY_SIGNAL_SYNC_EDGE_RISING,
      .bg_color = { .byte = { .a = 255, .r = 0, .g =
                                  255, .b = 0 } },
      #if (GLCDC_CFG_COLOR_CORRECTION_ENABLE)
      .brightness =
      {
          .enable = false,
          .r = 512,
          .g = 512,
          .b = 512
      },
      .contrast =
      {
          .enable = false,
          .r = 128,
          .g = 128,
          .b = 128
      },
      #if (GLCDC_CFG_CORRECTION_GAMMA_ENABLE_R | GLCDC_CFG_CORRECTION_GAMMA_ENABLE_G | GLCDC_CFG_CORRECTION_GAMMA_ENABLE_B)
      #if false
      .p_gamma_correction = GLCDC_CFG_CORRECTION_GAMMA_CFG_CAST(&g_display0_gamma_cfg),
      #else
      .p_gamma_correction = &g_display0_gamma_cfg,
      #endif
      #else
      .p_gamma_correction = NULL,
      #endif
      #endif
      .dithering_on = false },

    /** Display device callback function pointer */
    .p_callback = lcd_Vsync_ISR, .p_context = NULL,

    /** Display device extended configuration */
    .p_extend = (void *)(&g_display0_extend_cfg),

    .line_detect_ipl = (2), .underflow_1_ipl = (BSP_IRQ_DISABLED),
    .underflow_2_ipl = (BSP_IRQ_DISABLED),

    #if defined(VECTOR_NUMBER_GLCDC_LINE_DETECT)
    .line_detect_irq = VECTOR_NUMBER_GLCDC_LINE_DETECT,
    #else
    .line_detect_irq = FSP_INVALID_VECTOR,
    #endif
    #if defined(VECTOR_NUMBER_GLCDC_UNDERFLOW_1)
    .underflow_1_irq = VECTOR_NUMBER_GLCDC_UNDERFLOW_1,
    #else
    .underflow_1_irq = FSP_INVALID_VECTOR,
    #endif
    #if defined(VECTOR_NUMBER_GLCDC_UNDERFLOW_2)
    .underflow_2_irq = VECTOR_NUMBER_GLCDC_UNDERFLOW_2,
    #else
    .underflow_2_irq = FSP_INVALID_VECTOR,
    #endif
};

#if GLCDC_CFG_LAYER_1_ENABLE
/** Display on GLCDC run-time configuration(for the graphics1 layer) */
display_runtime_cfg_t g_display0_runtime_cfg_bg =
{
    .input =
    {
        #if (true)
        .p_base = (uint32_t *)&fb_background[0],
        #else
        .p_base = NULL,
        #endif
        .hsize = DISPLAY_HSIZE_INPUT0,
        .vsize = DISPLAY_VSIZE_INPUT0,
        .hstride = DISPLAY_BUFFER_STRIDE_PIXELS_INPUT0,
        .format = DISPLAY_IN_FORMAT_16BITS_RGB565,
        .line_descending_enable = false,
        .lines_repeat_enable = false,
        .lines_repeat_times = 0
    },
    .layer =
    {
        .coordinate = {
            .x = 0,
            .y = 0
        },
        .bg_color =
        {
            .byte =
            {
                .a = 255,
                .r = 255,
                .g = 255,
                .b = 255
            }
        },
        .fade_control = DISPLAY_FADE_CONTROL_NONE,
        .fade_speed = 0
    }
};
#endif
#if GLCDC_CFG_LAYER_2_ENABLE
/** Display on GLCDC run-time configuration(for the graphics2 layer) */
display_runtime_cfg_t g_display0_runtime_cfg_fg =
{
    .input =
    {
        #if (false)
        .p_base = (uint32_t *)&fb_foreground[0],
        #else
        .p_base = NULL,
        #endif
        .hsize = DISPLAY_HSIZE_INPUT1,
        .vsize = DISPLAY_VSIZE_INPUT1,
        .hstride = DISPLAY_BUFFER_STRIDE_PIXELS_INPUT1,
        .format = DISPLAY_IN_FORMAT_16BITS_RGB565,
        .line_descending_enable = false,
        .lines_repeat_enable = false,
        .lines_repeat_times = 0
    },
    .layer =
    {
        .coordinate = {
            .x = 0,
            .y = 0
        },
        .bg_color =
        {
            .byte =
            {
                .a = 255,
                .r = 255,
                .g = 255,
                .b = 255
            }
        },
        .fade_control = DISPLAY_FADE_CONTROL_NONE,
        .fade_speed = 0
    }
};
#endif

/* Instance structure to use this module. */
const display_instance_t g_display0 = { .p_ctrl = &g_display0_ctrl, .p_cfg =
                                            (display_cfg_t *)&g_display0_cfg, .p_api =
                                            (display_api_t *)&g_display_on_glcdc };
ether_phy_instance_ctrl_t g_ether_phy0_ctrl;

const ether_phy_cfg_t g_ether_phy0_cfg = {

    .channel = 0, .phy_lsi_address = 5, .phy_reset_wait_time = 0x00020000,
    .mii_bit_access_wait_time = 8, .phy_lsi_type =
        ETHER_PHY_LSI_TYPE_ICS1894, .flow_control =
        ETHER_PHY_FLOW_CONTROL_DISABLE, .mii_type =
        ETHER_PHY_MII_TYPE_RMII, .p_context = NULL, .p_extend = NULL,

};
/* Instance structure to use this module. */
const ether_phy_instance_t g_ether_phy0 = { .p_ctrl = &g_ether_phy0_ctrl,
                                            .p_cfg = &g_ether_phy0_cfg, .p_api = &g_ether_phy_on_ether_phy };
ether_instance_ctrl_t g_ether0_ctrl;

uint8_t g_ether0_mac_address[6] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 };

__attribute__((__aligned__(16))) ether_instance_descriptor_t g_ether0_tx_descriptors[4] ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(16))) ether_instance_descriptor_t g_ether0_rx_descriptors[4] ETHER_BUFFER_PLACE_IN_SECTION;

__attribute__((__aligned__(32))) uint8_t g_ether0_ether_buffer0[1536] ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32))) uint8_t g_ether0_ether_buffer1[1536] ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32))) uint8_t g_ether0_ether_buffer2[1536] ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32))) uint8_t g_ether0_ether_buffer3[1536] ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32))) uint8_t g_ether0_ether_buffer4[1536] ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32))) uint8_t g_ether0_ether_buffer5[1536] ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32))) uint8_t g_ether0_ether_buffer6[1536] ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32))) uint8_t g_ether0_ether_buffer7[1536] ETHER_BUFFER_PLACE_IN_SECTION;

uint8_t *pp_g_ether0_ether_buffers[8] = { (uint8_t *)&g_ether0_ether_buffer0[0],
                                          (uint8_t *)&g_ether0_ether_buffer1[0],
                                          (uint8_t *)&g_ether0_ether_buffer2[0],
                                          (uint8_t *)&g_ether0_ether_buffer3[0],
                                          (uint8_t *)&g_ether0_ether_buffer4[0],
                                          (uint8_t *)&g_ether0_ether_buffer5[0],
                                          (uint8_t *)&g_ether0_ether_buffer6[0],
                                          (uint8_t *)&g_ether0_ether_buffer7[0], };

const ether_extended_cfg_t g_ether0_extended_cfg_t = { .p_rx_descriptors =
                                                           g_ether0_rx_descriptors, .p_tx_descriptors = g_ether0_tx_descriptors, };

const ether_cfg_t g_ether0_cfg = { .channel = 0, .zerocopy =
                                       ETHER_ZEROCOPY_DISABLE, .multicast = ETHER_MULTICAST_ENABLE,
                                   .promiscuous = ETHER_PROMISCUOUS_DISABLE, .flow_control =
                                       ETHER_FLOW_CONTROL_DISABLE, .padding = ETHER_PADDING_DISABLE,
                                   .padding_offset = 1, .broadcast_filter = 0, .p_mac_address =
                                       g_ether0_mac_address,

                                   .num_tx_descriptors = 4, .num_rx_descriptors = 4,

                                   .pp_ether_buffers = pp_g_ether0_ether_buffers,

                                   .ether_buffer_size = 1536,

                                   #if defined(VECTOR_NUMBER_EDMAC0_EINT)
                                   .irq = VECTOR_NUMBER_EDMAC0_EINT,
                                   #else
                                   .irq = FSP_INVALID_VECTOR,
                                   #endif

                                   .interrupt_priority = (12),

                                   .p_callback = ETH_IRQHandler, .p_ether_phy_instance = &g_ether_phy0,
                                   .p_context = NULL, .p_extend = &g_ether0_extended_cfg_t, };

/* Instance structure to use this module. */
const ether_instance_t g_ether0 = { .p_ctrl = &g_ether0_ctrl, .p_cfg =
                                        &g_ether0_cfg, .p_api = &g_ether_on_ether };
dtc_instance_ctrl_t g_transfer2_ctrl;

transfer_info_t g_transfer2_info = { .transfer_settings_word_b.dest_addr_mode =
                                         TRANSFER_ADDR_MODE_FIXED, .transfer_settings_word_b.repeat_area =
                                         TRANSFER_REPEAT_AREA_SOURCE, .transfer_settings_word_b.irq =
                                         TRANSFER_IRQ_END, .transfer_settings_word_b.chain_mode =
                                         TRANSFER_CHAIN_MODE_DISABLED, .transfer_settings_word_b.src_addr_mode =
                                         TRANSFER_ADDR_MODE_INCREMENTED, .transfer_settings_word_b.size =
                                         TRANSFER_SIZE_4_BYTE, .transfer_settings_word_b.mode =
                                         TRANSFER_MODE_NORMAL, .p_dest = (void *)NULL, .p_src =
                                         (void const *)NULL, .num_blocks = 0, .length = 128, };

const dtc_extended_cfg_t g_transfer2_cfg_extend = { .activation_source =
                                                        VECTOR_NUMBER_SDHIMMC1_DMA_REQ, };
const transfer_cfg_t g_transfer2_cfg = { .p_info = &g_transfer2_info,
                                         .p_extend = &g_transfer2_cfg_extend, };

/* Instance structure to use this module. */
const transfer_instance_t g_transfer2 = { .p_ctrl = &g_transfer2_ctrl, .p_cfg =
                                              &g_transfer2_cfg, .p_api = &g_transfer_on_dtc };
#define RA_NOT_DEFINED (UINT32_MAX)
#if (RA_NOT_DEFINED) != (RA_NOT_DEFINED)

/* If the transfer module is DMAC, define a DMAC transfer callback. */
#include "r_dmac.h"
extern void r_sdhi_transfer_callback(sdhi_instance_ctrl_t *p_ctrl);

void g_sdmmc0_dmac_callback(dmac_callback_args_t *p_args) {
    r_sdhi_transfer_callback((sdhi_instance_ctrl_t *)p_args->p_context);
}
#endif
#undef RA_NOT_DEFINED

sdhi_instance_ctrl_t g_sdmmc0_ctrl;
sdmmc_cfg_t g_sdmmc0_cfg = { .bus_width = SDMMC_BUS_WIDTH_4_BITS, .channel = 1,
                             .p_callback = sdhi_ISR, .p_context = NULL, .block_size = 512,
                             .card_detect = SDMMC_CARD_DETECT_CD, .write_protect =
                                 SDMMC_WRITE_PROTECT_WP,

                             .p_extend = NULL, .p_lower_lvl_transfer = &g_transfer2,

                             .access_ipl = (12), .sdio_ipl = BSP_IRQ_DISABLED, .card_ipl = (12),
                             .dma_req_ipl = (12),
                             #if defined(VECTOR_NUMBER_SDHIMMC1_ACCS)
                             .access_irq = VECTOR_NUMBER_SDHIMMC1_ACCS,
                             #else
                             .access_irq = FSP_INVALID_VECTOR,
                             #endif
                             #if defined(VECTOR_NUMBER_SDHIMMC1_CARD)
                             .card_irq = VECTOR_NUMBER_SDHIMMC1_CARD,
                             #else
                             .card_irq = FSP_INVALID_VECTOR,
                             #endif
                             .sdio_irq = FSP_INVALID_VECTOR,
                             #if defined(VECTOR_NUMBER_SDHIMMC1_DMA_REQ)
                             .dma_req_irq = VECTOR_NUMBER_SDHIMMC1_DMA_REQ,
                             #else
                             .dma_req_irq = FSP_INVALID_VECTOR,
                             #endif
};
/* Instance structure to use this module. */
const sdmmc_instance_t g_sdmmc0 = { .p_ctrl = &g_sdmmc0_ctrl, .p_cfg =
                                        &g_sdmmc0_cfg, .p_api = &g_sdmmc_on_sdhi };
qspi_instance_ctrl_t g_qspi0_ctrl;

static const spi_flash_erase_command_t g_qspi0_erase_command_list[] = {
    #if 4096 > 0
    { .command = 0x20, .size = 4096 },
    #endif
    #if 32768 > 0
    { .command = 0x52, .size = 32768 },
    #endif
    #if 65536 > 0
    { .command = 0xD8, .size = 65536 },
    #endif
    #if 0xC7 > 0
    { .command = 0xC7, .size = SPI_FLASH_ERASE_SIZE_CHIP_ERASE },
    #endif
};
static const qspi_extended_cfg_t g_qspi0_extended_cfg = {
    .min_qssl_deselect_cycles = QSPI_QSSL_MIN_HIGH_LEVEL_9_QSPCLK,
    .qspclk_div = QSPI_QSPCLK_DIV_2,
};
const spi_flash_cfg_t g_qspi0_cfg = { .spi_protocol =
                                          SPI_FLASH_PROTOCOL_EXTENDED_SPI, .read_mode =
                                          SPI_FLASH_READ_MODE_FAST_READ_QUAD_IO, .address_bytes =
                                          SPI_FLASH_ADDRESS_BYTES_3, .dummy_clocks =
                                          SPI_FLASH_DUMMY_CLOCKS_DEFAULT, .page_program_address_lines =
                                          SPI_FLASH_DATA_LINES_1, .page_size_bytes = 256, .page_program_command =
                                          0x32, .write_enable_command = 0x06, .status_command = 0x05,
                                      .write_status_bit = 0, .xip_enter_command = 0x20, .xip_exit_command =
                                          0xFF, .p_erase_command_list = &g_qspi0_erase_command_list[0],
                                      .erase_command_list_length = sizeof(g_qspi0_erase_command_list)
                                          / sizeof(g_qspi0_erase_command_list[0]), .p_extend =
                                          &g_qspi0_extended_cfg, };
/** This structure encompasses everything that is needed to use an instance of this interface. */
const spi_flash_instance_t g_qspi0 = { .p_ctrl = &g_qspi0_ctrl, .p_cfg =
                                           &g_qspi0_cfg, .p_api = &g_qspi_on_spi_flash, };
iic_master_instance_ctrl_t g_i2c_master1_ctrl;
const iic_master_extended_cfg_t g_i2c_master1_extend =
{ .timeout_mode = IIC_MASTER_TIMEOUT_MODE_SHORT, .timeout_scl_low =
      IIC_MASTER_TIMEOUT_SCL_LOW_ENABLED,
  /* Actual calculated bitrate: 98945. Actual calculated duty cycle: 51%. */ .clock_settings.brl_value =
      15, .clock_settings.brh_value = 16,
  .clock_settings.cks_value = 4, };
const i2c_master_cfg_t g_i2c_master1_cfg = { .channel = 1, .rate =
                                                 I2C_MASTER_RATE_STANDARD, .slave = 0x00, .addr_mode =
                                                 I2C_MASTER_ADDR_MODE_7BIT,
#define RA_NOT_DEFINED (1)
                                             #if (RA_NOT_DEFINED == RA_NOT_DEFINED)
                                             .p_transfer_tx = NULL,
                                             #else
                                             .p_transfer_tx = &RA_NOT_DEFINED,
                                             #endif
                                             #if (RA_NOT_DEFINED == RA_NOT_DEFINED)
                                             .p_transfer_rx = NULL,
                                             #else
                                             .p_transfer_rx = &RA_NOT_DEFINED,
                                             #endif
#undef RA_NOT_DEFINED
                                             .p_callback = callback_iic, .p_context = NULL,
                                             #if defined(VECTOR_NUMBER_IIC1_RXI)
                                             .rxi_irq = VECTOR_NUMBER_IIC1_RXI,
                                             #else
                                             .rxi_irq = FSP_INVALID_VECTOR,
                                             #endif
                                             #if defined(VECTOR_NUMBER_IIC1_TXI)
                                             .txi_irq = VECTOR_NUMBER_IIC1_TXI,
                                             #else
                                             .txi_irq = FSP_INVALID_VECTOR,
                                             #endif
                                             #if defined(VECTOR_NUMBER_IIC1_TEI)
                                             .tei_irq = VECTOR_NUMBER_IIC1_TEI,
                                             #else
                                             .tei_irq = FSP_INVALID_VECTOR,
                                             #endif
                                             #if defined(VECTOR_NUMBER_IIC1_ERI)
                                             .eri_irq = VECTOR_NUMBER_IIC1_ERI,
                                             #else
                                             .eri_irq = FSP_INVALID_VECTOR,
                                             #endif
                                             .ipl = (12), .p_extend = &g_i2c_master1_extend, };
/* Instance structure to use this module. */
const i2c_master_instance_t g_i2c_master1 = { .p_ctrl = &g_i2c_master1_ctrl,
                                              .p_cfg = &g_i2c_master1_cfg, .p_api = &g_i2c_master_on_iic };
adc_instance_ctrl_t g_adc1_ctrl;
const adc_extended_cfg_t g_adc1_cfg_extend = { .add_average_count = ADC_ADD_OFF,
                                               .clearing = ADC_CLEAR_AFTER_READ_ON, .trigger_group_b =
                                                   ADC_TRIGGER_SYNC_ELC, .double_trigger_mode =
                                                   ADC_DOUBLE_TRIGGER_DISABLED, .adc_vref_control =
                                                   ADC_VREF_CONTROL_VREFH, .enable_adbuf = 0,
                                               #if defined(VECTOR_NUMBER_ADC1_WINDOW_A)
                                               .window_a_irq = VECTOR_NUMBER_ADC1_WINDOW_A,
                                               #else
                                               .window_a_irq = FSP_INVALID_VECTOR,
                                               #endif
                                               .window_a_ipl = (BSP_IRQ_DISABLED),
                                               #if defined(VECTOR_NUMBER_ADC1_WINDOW_B)
                                               .window_b_irq = VECTOR_NUMBER_ADC1_WINDOW_B,
                                               #else
                                               .window_b_irq = FSP_INVALID_VECTOR,
                                               #endif
                                               .window_b_ipl = (BSP_IRQ_DISABLED), };
const adc_cfg_t g_adc1_cfg = { .unit = 1, .mode = ADC_MODE_SINGLE_SCAN,
                               .resolution = ADC_RESOLUTION_12_BIT, .alignment =
                                   (adc_alignment_t)ADC_ALIGNMENT_RIGHT, .trigger =
                                   ADC_TRIGGER_SOFTWARE, .p_callback = NULL,
                               /** If NULL then do not add & */
                               #if defined(NULL)
                               .p_context = NULL,
                               #else
                               .p_context = &NULL,
                               #endif
                               .p_extend = &g_adc1_cfg_extend,
                               #if defined(VECTOR_NUMBER_ADC1_SCAN_END)
                               .scan_end_irq = VECTOR_NUMBER_ADC1_SCAN_END,
                               #else
                               .scan_end_irq = FSP_INVALID_VECTOR,
                               #endif
                               .scan_end_ipl = (BSP_IRQ_DISABLED),
                               #if defined(VECTOR_NUMBER_ADC1_SCAN_END_B)
                               .scan_end_b_irq = VECTOR_NUMBER_ADC1_SCAN_END_B,
                               #else
                               .scan_end_b_irq = FSP_INVALID_VECTOR,
                               #endif
                               .scan_end_b_ipl = (BSP_IRQ_DISABLED), };
#if ((0) | (0))
const adc_window_cfg_t g_adc1_window_cfg =
{
    .compare_mask = 0,
    .compare_mode_mask = 0,
    .compare_cfg = (0) | (0) | (0) | (ADC_COMPARE_CFG_EVENT_OUTPUT_OR),
    .compare_ref_low = 0,
    .compare_ref_high = 0,
    .compare_b_channel = (ADC_WINDOW_B_CHANNEL_0),
    .compare_b_mode = (ADC_WINDOW_B_MODE_LESS_THAN_OR_OUTSIDE),
    .compare_b_ref_low = 0,
    .compare_b_ref_high = 0,
};
#endif
const adc_channel_cfg_t g_adc1_channel_cfg = { .scan_mask = 0,
                                               .scan_mask_group_b = 0, .priority_group_a = ADC_GROUP_A_PRIORITY_OFF,
                                               .add_mask = 0, .sample_hold_mask = 0, .sample_hold_states = 24,
                                               #if ((0) | (0))
                                               .p_window_cfg = (adc_window_cfg_t *)&g_adc1_window_cfg,
                                               #else
                                               .p_window_cfg = NULL,
                                               #endif
};
/* Instance structure to use this module. */
const adc_instance_t g_adc1 = { .p_ctrl = &g_adc1_ctrl, .p_cfg = &g_adc1_cfg,
                                .p_channel_cfg = &g_adc1_channel_cfg, .p_api = &g_adc_on_adc };
adc_instance_ctrl_t g_adc0_ctrl;
const adc_extended_cfg_t g_adc0_cfg_extend = { .add_average_count = ADC_ADD_OFF,
                                               .clearing = ADC_CLEAR_AFTER_READ_ON, .trigger_group_b =
                                                   ADC_TRIGGER_SYNC_ELC, .double_trigger_mode =
                                                   ADC_DOUBLE_TRIGGER_DISABLED, .adc_vref_control =
                                                   ADC_VREF_CONTROL_VREFH, .enable_adbuf = 0,
                                               #if defined(VECTOR_NUMBER_ADC0_WINDOW_A)
                                               .window_a_irq = VECTOR_NUMBER_ADC0_WINDOW_A,
                                               #else
                                               .window_a_irq = FSP_INVALID_VECTOR,
                                               #endif
                                               .window_a_ipl = (BSP_IRQ_DISABLED),
                                               #if defined(VECTOR_NUMBER_ADC0_WINDOW_B)
                                               .window_b_irq = VECTOR_NUMBER_ADC0_WINDOW_B,
                                               #else
                                               .window_b_irq = FSP_INVALID_VECTOR,
                                               #endif
                                               .window_b_ipl = (BSP_IRQ_DISABLED), };
const adc_cfg_t g_adc0_cfg = { .unit = 0, .mode = ADC_MODE_SINGLE_SCAN,
                               .resolution = ADC_RESOLUTION_12_BIT, .alignment =
                                   (adc_alignment_t)ADC_ALIGNMENT_RIGHT, .trigger =
                                   ADC_TRIGGER_SOFTWARE, .p_callback = NULL,
                               /** If NULL then do not add & */
                               #if defined(NULL)
                               .p_context = NULL,
                               #else
                               .p_context = &NULL,
                               #endif
                               .p_extend = &g_adc0_cfg_extend,
                               #if defined(VECTOR_NUMBER_ADC0_SCAN_END)
                               .scan_end_irq = VECTOR_NUMBER_ADC0_SCAN_END,
                               #else
                               .scan_end_irq = FSP_INVALID_VECTOR,
                               #endif
                               .scan_end_ipl = (BSP_IRQ_DISABLED),
                               #if defined(VECTOR_NUMBER_ADC0_SCAN_END_B)
                               .scan_end_b_irq = VECTOR_NUMBER_ADC0_SCAN_END_B,
                               #else
                               .scan_end_b_irq = FSP_INVALID_VECTOR,
                               #endif
                               .scan_end_b_ipl = (BSP_IRQ_DISABLED), };
#if ((0) | (0))
const adc_window_cfg_t g_adc0_window_cfg =
{
    .compare_mask = 0,
    .compare_mode_mask = 0,
    .compare_cfg = (0) | (0) | (0) | (ADC_COMPARE_CFG_EVENT_OUTPUT_OR),
    .compare_ref_low = 0,
    .compare_ref_high = 0,
    .compare_b_channel = (ADC_WINDOW_B_CHANNEL_0),
    .compare_b_mode = (ADC_WINDOW_B_MODE_LESS_THAN_OR_OUTSIDE),
    .compare_b_ref_low = 0,
    .compare_b_ref_high = 0,
};
#endif
const adc_channel_cfg_t g_adc0_channel_cfg = { .scan_mask = 0,
                                               .scan_mask_group_b = 0, .priority_group_a = ADC_GROUP_A_PRIORITY_OFF,
                                               .add_mask = 0, .sample_hold_mask = 0, .sample_hold_states = 24,
                                               #if ((0) | (0))
                                               .p_window_cfg = (adc_window_cfg_t *)&g_adc0_window_cfg,
                                               #else
                                               .p_window_cfg = NULL,
                                               #endif
};
/* Instance structure to use this module. */
const adc_instance_t g_adc0 = { .p_ctrl = &g_adc0_ctrl, .p_cfg = &g_adc0_cfg,
                                .p_channel_cfg = &g_adc0_channel_cfg, .p_api = &g_adc_on_adc };
lpm_instance_ctrl_t g_lpm0_ctrl;

const lpm_cfg_t g_lpm0_cfg = { .low_power_mode = LPM_MODE_SLEEP,
                               .standby_wake_sources = LPM_STANDBY_WAKE_SOURCE_RTCALM
                                   | (lpm_standby_wake_source_t)0,
                               #if BSP_FEATURE_LPM_HAS_SNOOZE
                               .snooze_cancel_sources = LPM_SNOOZE_CANCEL_SOURCE_NONE,
                               .snooze_request_source = LPM_SNOOZE_REQUEST_RXD0_FALLING,
                               .snooze_end_sources = (lpm_snooze_end_t)0,
                               .dtc_state_in_snooze = LPM_SNOOZE_DTC_DISABLE,
                               #endif
                               #if BSP_FEATURE_LPM_HAS_SBYCR_OPE
                               .output_port_enable = LPM_OUTPUT_PORT_ENABLE_RETAIN,
                               #endif
                               #if BSP_FEATURE_LPM_HAS_DEEP_STANDBY
                               .io_port_state = LPM_IO_PORT_NO_CHANGE,
                               .power_supply_state = LPM_POWER_SUPPLY_DEEPCUT0,
                               .deep_standby_cancel_source = (lpm_deep_standby_cancel_source_t)0,
                               .deep_standby_cancel_edge = (lpm_deep_standby_cancel_edge_t)0,
                               #endif
                               #if BSP_FEATURE_LPM_HAS_PDRAMSCR
                               .ram_retention_cfg.ram_retention = (uint8_t)(0),
                               .ram_retention_cfg.tcm_retention = false,
                               #endif
                               #if BSP_FEATURE_LPM_HAS_DPSBYCR_SRKEEP
                               .ram_retention_cfg.standby_ram_retention = false,
                               #endif
                               #if BSP_FEATURE_LPM_HAS_LDO_CONTROL
                               .ldo_standby_cfg.pll1_ldo = false,
                               .ldo_standby_cfg.pll2_ldo = false,
                               .ldo_standby_cfg.hoco_ldo = false,
                               #endif
                               .p_extend = NULL, };

const lpm_instance_t g_lpm0 = { .p_api = &g_lpm_on_lpm, .p_ctrl = &g_lpm0_ctrl,
                                .p_cfg = &g_lpm0_cfg };
dtc_instance_ctrl_t g_transfer1_ctrl;

transfer_info_t g_transfer1_info = { .transfer_settings_word_b.dest_addr_mode =
                                         TRANSFER_ADDR_MODE_INCREMENTED, .transfer_settings_word_b.repeat_area =
                                         TRANSFER_REPEAT_AREA_DESTINATION, .transfer_settings_word_b.irq =
                                         TRANSFER_IRQ_END, .transfer_settings_word_b.chain_mode =
                                         TRANSFER_CHAIN_MODE_DISABLED, .transfer_settings_word_b.src_addr_mode =
                                         TRANSFER_ADDR_MODE_FIXED, .transfer_settings_word_b.size =
                                         TRANSFER_SIZE_2_BYTE, .transfer_settings_word_b.mode =
                                         TRANSFER_MODE_NORMAL, .p_dest = (void *)NULL, .p_src =
                                         (void const *)NULL, .num_blocks = 0, .length = 0, };

const dtc_extended_cfg_t g_transfer1_cfg_extend = { .activation_source =
                                                        VECTOR_NUMBER_SPI1_RXI, };
const transfer_cfg_t g_transfer1_cfg = { .p_info = &g_transfer1_info,
                                         .p_extend = &g_transfer1_cfg_extend, };

/* Instance structure to use this module. */
const transfer_instance_t g_transfer1 = { .p_ctrl = &g_transfer1_ctrl, .p_cfg =
                                              &g_transfer1_cfg, .p_api = &g_transfer_on_dtc };
dtc_instance_ctrl_t g_transfer0_ctrl;

transfer_info_t g_transfer0_info = { .transfer_settings_word_b.dest_addr_mode =
                                         TRANSFER_ADDR_MODE_FIXED, .transfer_settings_word_b.repeat_area =
                                         TRANSFER_REPEAT_AREA_SOURCE, .transfer_settings_word_b.irq =
                                         TRANSFER_IRQ_END, .transfer_settings_word_b.chain_mode =
                                         TRANSFER_CHAIN_MODE_DISABLED, .transfer_settings_word_b.src_addr_mode =
                                         TRANSFER_ADDR_MODE_INCREMENTED, .transfer_settings_word_b.size =
                                         TRANSFER_SIZE_2_BYTE, .transfer_settings_word_b.mode =
                                         TRANSFER_MODE_NORMAL, .p_dest = (void *)NULL, .p_src =
                                         (void const *)NULL, .num_blocks = 0, .length = 0, };

const dtc_extended_cfg_t g_transfer0_cfg_extend = { .activation_source =
                                                        VECTOR_NUMBER_SPI1_TXI, };
const transfer_cfg_t g_transfer0_cfg = { .p_info = &g_transfer0_info,
                                         .p_extend = &g_transfer0_cfg_extend, };

/* Instance structure to use this module. */
const transfer_instance_t g_transfer0 = { .p_ctrl = &g_transfer0_ctrl, .p_cfg =
                                              &g_transfer0_cfg, .p_api = &g_transfer_on_dtc };
#define RA_NOT_DEFINED (UINT32_MAX)
#if (RA_NOT_DEFINED) != (RA_NOT_DEFINED)

/* If the transfer module is DMAC, define a DMAC transfer callback. */
#include "r_dmac.h"
extern void spi_tx_dmac_callback(spi_instance_ctrl_t const *const p_ctrl);

void g_spi1_tx_transfer_callback(dmac_callback_args_t *p_args) {
    FSP_PARAMETER_NOT_USED(p_args);
    spi_tx_dmac_callback(&g_spi1_ctrl);
}
#endif

#if (RA_NOT_DEFINED) != (RA_NOT_DEFINED)

/* If the transfer module is DMAC, define a DMAC transfer callback. */
#include "r_dmac.h"
extern void spi_rx_dmac_callback(spi_instance_ctrl_t const *const p_ctrl);

void g_spi1_rx_transfer_callback(dmac_callback_args_t *p_args) {
    FSP_PARAMETER_NOT_USED(p_args);
    spi_rx_dmac_callback(&g_spi1_ctrl);
}
#endif
#undef RA_NOT_DEFINED

spi_instance_ctrl_t g_spi1_ctrl;

/** SPI extended configuration for SPI HAL driver */
const spi_extended_cfg_t g_spi1_ext_cfg = { .spi_clksyn = SPI_SSL_MODE_CLK_SYN,
                                            .spi_comm = SPI_COMMUNICATION_FULL_DUPLEX, .ssl_polarity = SPI_SSLP_LOW,
                                            .ssl_select = SPI_SSL_SELECT_SSL0, .mosi_idle =
                                                SPI_MOSI_IDLE_VALUE_FIXING_DISABLE, .parity =
                                                SPI_PARITY_MODE_DISABLE, .byte_swap = SPI_BYTE_SWAP_DISABLE,
                                            .spck_div = {
                                                /* Actual calculated bitrate: 15000000. */ .spbr = 3, .brdv = 0
                                            },
                                            .spck_delay = SPI_DELAY_COUNT_1,
                                            .ssl_negation_delay = SPI_DELAY_COUNT_1, .next_access_delay =
                                                SPI_DELAY_COUNT_1 };

/** SPI configuration for SPI HAL driver */
const spi_cfg_t g_spi1_cfg = { .channel = 1,

                               #if defined(VECTOR_NUMBER_SPI1_RXI)
                               .rxi_irq = VECTOR_NUMBER_SPI1_RXI,
                               #else
                               .rxi_irq = FSP_INVALID_VECTOR,
                               #endif
                               #if defined(VECTOR_NUMBER_SPI1_TXI)
                               .txi_irq = VECTOR_NUMBER_SPI1_TXI,
                               #else
                               .txi_irq = FSP_INVALID_VECTOR,
                               #endif
                               #if defined(VECTOR_NUMBER_SPI1_TEI)
                               .tei_irq = VECTOR_NUMBER_SPI1_TEI,
                               #else
                               .tei_irq = FSP_INVALID_VECTOR,
                               #endif
                               #if defined(VECTOR_NUMBER_SPI1_ERI)
                               .eri_irq = VECTOR_NUMBER_SPI1_ERI,
                               #else
                               .eri_irq = FSP_INVALID_VECTOR,
                               #endif

                               .rxi_ipl = (12), .txi_ipl = (12), .tei_ipl = (12), .eri_ipl = (12),

                               .operating_mode = SPI_MODE_MASTER,

                               .clk_phase = SPI_CLK_PHASE_EDGE_ODD, .clk_polarity =
                                   SPI_CLK_POLARITY_LOW,

                               .mode_fault = SPI_MODE_FAULT_ERROR_DISABLE, .bit_order =
                                   SPI_BIT_ORDER_MSB_FIRST, .p_transfer_tx = g_spi1_P_TRANSFER_TX,
                               .p_transfer_rx = g_spi1_P_TRANSFER_RX, .p_callback = spi_callback,

                               .p_context = NULL, .p_extend = (void *)&g_spi1_ext_cfg, };

/* Instance structure to use this module. */
const spi_instance_t g_spi1 = { .p_ctrl = &g_spi1_ctrl, .p_cfg = &g_spi1_cfg,
                                .p_api = &g_spi_on_spi };
agt_instance_ctrl_t g_timer1_ctrl;
const agt_extended_cfg_t g_timer1_extend =
{ .count_source = AGT_CLOCK_PCLKB, .agto = AGT_PIN_CFG_DISABLED,
  .agtoab_settings_b.agtoa = AGT_PIN_CFG_DISABLED,
  .agtoab_settings_b.agtob = AGT_PIN_CFG_DISABLED,
  .measurement_mode = AGT_MEASURE_DISABLED, .agtio_filter =
      AGT_AGTIO_FILTER_NONE, .enable_pin =
      AGT_ENABLE_PIN_NOT_USED, .trigger_edge =
      AGT_TRIGGER_EDGE_RISING, };
const timer_cfg_t g_timer1_cfg =
{ .mode = TIMER_MODE_PERIODIC,
  /* Actual period: 0.0010922666666666667 seconds. Actual duty: 50%. */ .period_counts =
      (uint32_t)0x10000, .duty_cycle_counts = 0x8000,
  .source_div = (timer_source_div_t)0, .channel = 1,
  .p_callback = callback_agt,
  /** If NULL then do not add & */
  #if defined(NULL)
  .p_context = NULL,
  #else
  .p_context = &NULL,
  #endif
  .p_extend = &g_timer1_extend, .cycle_end_ipl = (5),
  #if defined(VECTOR_NUMBER_AGT1_INT)
  .cycle_end_irq = VECTOR_NUMBER_AGT1_INT,
  #else
  .cycle_end_irq = FSP_INVALID_VECTOR,
  #endif
};
/* Instance structure to use this module. */
const timer_instance_t g_timer1 = { .p_ctrl = &g_timer1_ctrl, .p_cfg =
                                        &g_timer1_cfg, .p_api = &g_timer_on_agt };
agt_instance_ctrl_t g_timer0_ctrl;
const agt_extended_cfg_t g_timer0_extend =
{ .count_source = AGT_CLOCK_PCLKB, .agto = AGT_PIN_CFG_DISABLED,
  .agtoab_settings_b.agtoa = AGT_PIN_CFG_DISABLED,
  .agtoab_settings_b.agtob = AGT_PIN_CFG_DISABLED,
  .measurement_mode = AGT_MEASURE_DISABLED, .agtio_filter =
      AGT_AGTIO_FILTER_NONE, .enable_pin =
      AGT_ENABLE_PIN_NOT_USED, .trigger_edge =
      AGT_TRIGGER_EDGE_RISING, };
const timer_cfg_t g_timer0_cfg =
{ .mode = TIMER_MODE_PERIODIC,
  /* Actual period: 0.0010922666666666667 seconds. Actual duty: 50%. */ .period_counts =
      (uint32_t)0x10000, .duty_cycle_counts = 0x8000,
  .source_div = (timer_source_div_t)0, .channel = 0,
  .p_callback = callback_agt,
  /** If NULL then do not add & */
  #if defined(NULL)
  .p_context = NULL,
  #else
  .p_context = &NULL,
  #endif
  .p_extend = &g_timer0_extend, .cycle_end_ipl = (5),
  #if defined(VECTOR_NUMBER_AGT0_INT)
  .cycle_end_irq = VECTOR_NUMBER_AGT0_INT,
  #else
  .cycle_end_irq = FSP_INVALID_VECTOR,
  #endif
};
/* Instance structure to use this module. */
const timer_instance_t g_timer0 = { .p_ctrl = &g_timer0_ctrl, .p_cfg =
                                        &g_timer0_cfg, .p_api = &g_timer_on_agt };
flash_hp_instance_ctrl_t g_flash0_ctrl;
const flash_cfg_t g_flash0_cfg = { .data_flash_bgo = false, .p_callback = NULL,
                                   .p_context = NULL,
                                   #if defined(VECTOR_NUMBER_FCU_FRDYI)
                                   .irq = VECTOR_NUMBER_FCU_FRDYI,
                                   #else
                                   .irq = FSP_INVALID_VECTOR,
                                   #endif
                                   #if defined(VECTOR_NUMBER_FCU_FIFERR)
                                   .err_irq = VECTOR_NUMBER_FCU_FIFERR,
                                   #else
                                   .err_irq = FSP_INVALID_VECTOR,
                                   #endif
                                   .err_ipl = (BSP_IRQ_DISABLED), .ipl = (BSP_IRQ_DISABLED), };
/* Instance structure to use this module. */
const flash_instance_t g_flash0 = { .p_ctrl = &g_flash0_ctrl, .p_cfg =
                                        &g_flash0_cfg, .p_api = &g_flash_on_flash_hp };
rtc_instance_ctrl_t g_rtc0_ctrl;
const rtc_error_adjustment_cfg_t g_rtc0_err_cfg = { .adjustment_mode =
                                                        RTC_ERROR_ADJUSTMENT_MODE_AUTOMATIC, .adjustment_period =
                                                        RTC_ERROR_ADJUSTMENT_PERIOD_10_SECOND, .adjustment_type =
                                                        RTC_ERROR_ADJUSTMENT_NONE, .adjustment_value = 0, };
const rtc_cfg_t g_rtc0_cfg = { .clock_source = RTC_CLOCK_SOURCE_LOCO,
                               .freq_compare_value_loco = 255, .p_err_cfg = &g_rtc0_err_cfg,
                               .p_callback = NULL, .p_context = NULL, .alarm_ipl = (14),
                               .periodic_ipl = (14), .carry_ipl = (14),
                               #if defined(VECTOR_NUMBER_RTC_ALARM)
                               .alarm_irq = VECTOR_NUMBER_RTC_ALARM,
                               #else
                               .alarm_irq = FSP_INVALID_VECTOR,
                               #endif
                               #if defined(VECTOR_NUMBER_RTC_PERIOD)
                               .periodic_irq = VECTOR_NUMBER_RTC_PERIOD,
                               #else
                               .periodic_irq = FSP_INVALID_VECTOR,
                               #endif
                               #if defined(VECTOR_NUMBER_RTC_CARRY)
                               .carry_irq = VECTOR_NUMBER_RTC_CARRY,
                               #else
                               .carry_irq = FSP_INVALID_VECTOR,
                               #endif
};
/* Instance structure to use this module. */
const rtc_instance_t g_rtc0 = { .p_ctrl = &g_rtc0_ctrl, .p_cfg = &g_rtc0_cfg,
                                .p_api = &g_rtc_on_rtc };
sci_uart_instance_ctrl_t g_uart9_ctrl;

baud_setting_t g_uart9_baud_setting = {
/* Baud rate calculated with 0.160% error. */ .semr_baudrate_bits_b.abcse = 0,
    .semr_baudrate_bits_b.abcs = 0, .semr_baudrate_bits_b.bgdm = 1,
    .cks = 0, .brr = 64, .mddr = (uint8_t)256, .semr_baudrate_bits_b.brme =
        false
};

/** UART extended configuration for UARTonSCI HAL driver */
const sci_uart_extended_cfg_t g_uart9_cfg_extend = {
    .clock = SCI_UART_CLOCK_INT, .rx_edge_start =
        SCI_UART_START_BIT_FALLING_EDGE, .noise_cancel =
        SCI_UART_NOISE_CANCELLATION_DISABLE, .rx_fifo_trigger =
        SCI_UART_RX_FIFO_TRIGGER_MAX, .p_baud_setting =
        &g_uart9_baud_setting,
    .flow_control = SCI_UART_FLOW_CONTROL_RTS,
    #if 0xFF != 0xFF
    .flow_control_pin = BSP_IO_PORT_FF_PIN_0xFF,
    #else
    .flow_control_pin = (bsp_io_port_pin_t)UINT16_MAX,
    #endif
    .rs485_setting = { .enable = SCI_UART_RS485_DISABLE, .polarity =
                           SCI_UART_RS485_DE_POLARITY_HIGH,
                       #if 0xFF != 0xFF
                       .de_control_pin = BSP_IO_PORT_FF_PIN_0xFF,
                       #else
                       .de_control_pin = (bsp_io_port_pin_t)UINT16_MAX,
                       #endif
    },
};

/** UART interface configuration */
const uart_cfg_t g_uart9_cfg = { .channel = 9, .data_bits = UART_DATA_BITS_8,
                                 .parity = UART_PARITY_OFF, .stop_bits = UART_STOP_BITS_1, .p_callback =
                                     user_uart_callback, .p_context = NULL, .p_extend =
                                     &g_uart9_cfg_extend,
#define RA_NOT_DEFINED (1)
                                 #if (RA_NOT_DEFINED == RA_NOT_DEFINED)
                                 .p_transfer_tx = NULL,
                                 #else
                                 .p_transfer_tx = &RA_NOT_DEFINED,
                                 #endif
                                 #if (RA_NOT_DEFINED == RA_NOT_DEFINED)
                                 .p_transfer_rx = NULL,
                                 #else
                                 .p_transfer_rx = &RA_NOT_DEFINED,
                                 #endif
#undef RA_NOT_DEFINED
                                 .rxi_ipl = (12), .txi_ipl = (12), .tei_ipl = (12), .eri_ipl = (12),
                                 #if defined(VECTOR_NUMBER_SCI9_RXI)
                                 .rxi_irq = VECTOR_NUMBER_SCI9_RXI,
                                 #else
                                 .rxi_irq = FSP_INVALID_VECTOR,
                                 #endif
                                 #if defined(VECTOR_NUMBER_SCI9_TXI)
                                 .txi_irq = VECTOR_NUMBER_SCI9_TXI,
                                 #else
                                 .txi_irq = FSP_INVALID_VECTOR,
                                 #endif
                                 #if defined(VECTOR_NUMBER_SCI9_TEI)
                                 .tei_irq = VECTOR_NUMBER_SCI9_TEI,
                                 #else
                                 .tei_irq = FSP_INVALID_VECTOR,
                                 #endif
                                 #if defined(VECTOR_NUMBER_SCI9_ERI)
                                 .eri_irq = VECTOR_NUMBER_SCI9_ERI,
                                 #else
                                 .eri_irq = FSP_INVALID_VECTOR,
                                 #endif
};

/* Instance structure to use this module. */
const uart_instance_t g_uart9 = { .p_ctrl = &g_uart9_ctrl,
                                  .p_cfg = &g_uart9_cfg, .p_api = &g_uart_on_sci };
sci_uart_instance_ctrl_t g_uart8_ctrl;

baud_setting_t g_uart8_baud_setting = {
/* Baud rate calculated with 0.160% error. */ .semr_baudrate_bits_b.abcse = 0,
    .semr_baudrate_bits_b.abcs = 0, .semr_baudrate_bits_b.bgdm = 1,
    .cks = 0, .brr = 64, .mddr = (uint8_t)256, .semr_baudrate_bits_b.brme =
        false
};

/** UART extended configuration for UARTonSCI HAL driver */
const sci_uart_extended_cfg_t g_uart8_cfg_extend = {
    .clock = SCI_UART_CLOCK_INT, .rx_edge_start =
        SCI_UART_START_BIT_FALLING_EDGE, .noise_cancel =
        SCI_UART_NOISE_CANCELLATION_DISABLE, .rx_fifo_trigger =
        SCI_UART_RX_FIFO_TRIGGER_MAX, .p_baud_setting =
        &g_uart8_baud_setting,
    .flow_control = SCI_UART_FLOW_CONTROL_RTS,
    #if 0xFF != 0xFF
    .flow_control_pin = BSP_IO_PORT_FF_PIN_0xFF,
    #else
    .flow_control_pin = (bsp_io_port_pin_t)UINT16_MAX,
    #endif
    .rs485_setting = { .enable = SCI_UART_RS485_DISABLE, .polarity =
                           SCI_UART_RS485_DE_POLARITY_HIGH,
                       #if 0xFF != 0xFF
                       .de_control_pin = BSP_IO_PORT_FF_PIN_0xFF,
                       #else
                       .de_control_pin = (bsp_io_port_pin_t)UINT16_MAX,
                       #endif
    },
};

/** UART interface configuration */
const uart_cfg_t g_uart8_cfg = { .channel = 8, .data_bits = UART_DATA_BITS_8,
                                 .parity = UART_PARITY_OFF, .stop_bits = UART_STOP_BITS_1, .p_callback =
                                     user_uart_callback, .p_context = NULL, .p_extend =
                                     &g_uart8_cfg_extend,
#define RA_NOT_DEFINED (1)
                                 #if (RA_NOT_DEFINED == RA_NOT_DEFINED)
                                 .p_transfer_tx = NULL,
                                 #else
                                 .p_transfer_tx = &RA_NOT_DEFINED,
                                 #endif
                                 #if (RA_NOT_DEFINED == RA_NOT_DEFINED)
                                 .p_transfer_rx = NULL,
                                 #else
                                 .p_transfer_rx = &RA_NOT_DEFINED,
                                 #endif
#undef RA_NOT_DEFINED
                                 .rxi_ipl = (12), .txi_ipl = (12), .tei_ipl = (12), .eri_ipl = (12),
                                 #if defined(VECTOR_NUMBER_SCI8_RXI)
                                 .rxi_irq = VECTOR_NUMBER_SCI8_RXI,
                                 #else
                                 .rxi_irq = FSP_INVALID_VECTOR,
                                 #endif
                                 #if defined(VECTOR_NUMBER_SCI8_TXI)
                                 .txi_irq = VECTOR_NUMBER_SCI8_TXI,
                                 #else
                                 .txi_irq = FSP_INVALID_VECTOR,
                                 #endif
                                 #if defined(VECTOR_NUMBER_SCI8_TEI)
                                 .tei_irq = VECTOR_NUMBER_SCI8_TEI,
                                 #else
                                 .tei_irq = FSP_INVALID_VECTOR,
                                 #endif
                                 #if defined(VECTOR_NUMBER_SCI8_ERI)
                                 .eri_irq = VECTOR_NUMBER_SCI8_ERI,
                                 #else
                                 .eri_irq = FSP_INVALID_VECTOR,
                                 #endif
};

/* Instance structure to use this module. */
const uart_instance_t g_uart8 = { .p_ctrl = &g_uart8_ctrl,
                                  .p_cfg = &g_uart8_cfg, .p_api = &g_uart_on_sci };
sci_uart_instance_ctrl_t g_uart7_ctrl;

baud_setting_t g_uart7_baud_setting = {
/* Baud rate calculated with 0.160% error. */ .semr_baudrate_bits_b.abcse = 0,
    .semr_baudrate_bits_b.abcs = 0, .semr_baudrate_bits_b.bgdm = 1,
    .cks = 0, .brr = 64, .mddr = (uint8_t)256, .semr_baudrate_bits_b.brme =
        false
};

/** UART extended configuration for UARTonSCI HAL driver */
const sci_uart_extended_cfg_t g_uart7_cfg_extend = {
    .clock = SCI_UART_CLOCK_INT, .rx_edge_start =
        SCI_UART_START_BIT_FALLING_EDGE, .noise_cancel =
        SCI_UART_NOISE_CANCELLATION_DISABLE, .rx_fifo_trigger =
        SCI_UART_RX_FIFO_TRIGGER_MAX, .p_baud_setting =
        &g_uart7_baud_setting,
    .flow_control = SCI_UART_FLOW_CONTROL_RTS,
    #if 0xFF != 0xFF
    .flow_control_pin = BSP_IO_PORT_FF_PIN_0xFF,
    #else
    .flow_control_pin = (bsp_io_port_pin_t)UINT16_MAX,
    #endif
    .rs485_setting = { .enable = SCI_UART_RS485_DISABLE, .polarity =
                           SCI_UART_RS485_DE_POLARITY_HIGH,
                       #if 0xFF != 0xFF
                       .de_control_pin = BSP_IO_PORT_FF_PIN_0xFF,
                       #else
                       .de_control_pin = (bsp_io_port_pin_t)UINT16_MAX,
                       #endif
    },
};

/** UART interface configuration */
const uart_cfg_t g_uart7_cfg = { .channel = 7, .data_bits = UART_DATA_BITS_8,
                                 .parity = UART_PARITY_OFF, .stop_bits = UART_STOP_BITS_1, .p_callback =
                                     user_uart_callback, .p_context = NULL, .p_extend =
                                     &g_uart7_cfg_extend,
#define RA_NOT_DEFINED (1)
                                 #if (RA_NOT_DEFINED == RA_NOT_DEFINED)
                                 .p_transfer_tx = NULL,
                                 #else
                                 .p_transfer_tx = &RA_NOT_DEFINED,
                                 #endif
                                 #if (RA_NOT_DEFINED == RA_NOT_DEFINED)
                                 .p_transfer_rx = NULL,
                                 #else
                                 .p_transfer_rx = &RA_NOT_DEFINED,
                                 #endif
#undef RA_NOT_DEFINED
                                 .rxi_ipl = (12), .txi_ipl = (12), .tei_ipl = (12), .eri_ipl = (12),
                                 #if defined(VECTOR_NUMBER_SCI7_RXI)
                                 .rxi_irq = VECTOR_NUMBER_SCI7_RXI,
                                 #else
                                 .rxi_irq = FSP_INVALID_VECTOR,
                                 #endif
                                 #if defined(VECTOR_NUMBER_SCI7_TXI)
                                 .txi_irq = VECTOR_NUMBER_SCI7_TXI,
                                 #else
                                 .txi_irq = FSP_INVALID_VECTOR,
                                 #endif
                                 #if defined(VECTOR_NUMBER_SCI7_TEI)
                                 .tei_irq = VECTOR_NUMBER_SCI7_TEI,
                                 #else
                                 .tei_irq = FSP_INVALID_VECTOR,
                                 #endif
                                 #if defined(VECTOR_NUMBER_SCI7_ERI)
                                 .eri_irq = VECTOR_NUMBER_SCI7_ERI,
                                 #else
                                 .eri_irq = FSP_INVALID_VECTOR,
                                 #endif
};

/* Instance structure to use this module. */
const uart_instance_t g_uart7 = { .p_ctrl = &g_uart7_ctrl,
                                  .p_cfg = &g_uart7_cfg, .p_api = &g_uart_on_sci };
void g_hal_init(void) {
    g_common_init();
}
