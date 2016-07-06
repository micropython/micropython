/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef STM32_LTDC_H
#define STM32_LTDC_H

// LTDC enable flags
#define LTDC_EF_ENABLE          (1 <<  0)   /**< LTDC enabled.*/
#define LTDC_EF_DITHER          (1 << 16)   /**< Dithering enabled.*/
#define LTDC_EF_PIXCLK_INVERT   (1 << 28)   /**< Inverted pixel clock.*/
#define LTDC_EF_DATAEN_HIGH     (1 << 29)   /**< Active-high data enable.*/
#define LTDC_EF_VSYNC_HIGH      (1 << 30)   /**< Active-high vsync.*/
#define LTDC_EF_HSYNC_HIGH      (1 << 31)   /**< Active-high hsync.*/

#define LTDC_EF_MASK			(LTDC_EF_ENABLE | LTDC_EF_DITHER | LTDC_EF_PIXCLK_INVERT | LTDC_EF_DATAEN_HIGH | LTDC_EF_VSYNC_HIGH | LTDC_EF_HSYNC_HIGH)

// LTDC layer enable flags
#define LTDC_LEF_ENABLE         (1 << 0)    /**< Layer enabled*/
#define LTDC_LEF_KEYING         (1 << 1)    /**< Color keying enabled.*/
#define LTDC_LEF_PALETTE        (1 << 4)    /**< Palette enabled.*/

#define LTDC_LEF_MASK			(LTDC_LEF_ENABLE | LTDC_LEF_KEYING | LTDC_LEF_PALETTE)

// LTDC pixel formats
#define LTDC_FMT_ARGB8888       0           /**< ARGB-8888 format.*/
#define LTDC_FMT_RGB888         1           /**< RGB-888 format.*/
#define LTDC_FMT_RGB565         2           /**< RGB-565 format.*/
#define LTDC_FMT_ARGB1555       3           /**< ARGB-1555 format.*/
#define LTDC_FMT_ARGB4444       4           /**< ARGB-4444 format.*/
#define LTDC_FMT_L8             5           /**< L-8 format.*/
#define LTDC_FMT_AL44           6           /**< AL-44 format.*/
#define LTDC_FMT_AL88           7           /**< AL-88 format.*/

// LTDC pixel format aliased raw masks
#define LTDC_XMASK_ARGB8888     0xFFFFFFFF  /**< ARGB-8888 aliased mask.*/
#define LTDC_XMASK_RGB888       0x00FFFFFF  /**< RGB-888 aliased mask.*/
#define LTDC_XMASK_RGB565       0x00F8FCF8  /**< RGB-565 aliased mask.*/
#define LTDC_XMASK_ARGB1555     0x80F8F8F8  /**< ARGB-1555 aliased mask.*/
#define LTDC_XMASK_ARGB4444     0xF0F0F0F0  /**< ARGB-4444 aliased mask.*/
#define LTDC_XMASK_L8           0x000000FF  /**< L-8 aliased mask.*/
#define LTDC_XMASK_AL44         0xF00000F0  /**< AL-44 aliased mask.*/
#define LTDC_XMASK_AL88         0xFF0000FF  /**< AL-88 aliased mask.*/

// LTDC blending factors
#define LTDC_BLEND_FIX1_FIX2    0x0405      /**<      cnst1; 1 -      cnst2 */
#define LTDC_BLEND_FIX1_MOD2    0x0407      /**<      cnst1; 1 - a2 * cnst2 */
#define LTDC_BLEND_MOD1_FIX2    0x0605      /**< a1 * cnst1; 1 -      cnst2 */
#define LTDC_BLEND_MOD1_MOD2    0x0607      /**< a1 * cnst1; 1 - a2 * cnst2 */

// LTDC parameter bounds
#define LTDC_MIN_SCREEN_WIDTH           1
#define LTDC_MIN_SCREEN_HEIGHT          1
#define LTDC_MAX_SCREEN_WIDTH           800
#define LTDC_MAX_SCREEN_HEIGHT          600

#define LTDC_MIN_HSYNC_WIDTH            1
#define LTDC_MIN_VSYNC_HEIGHT           1
#define LTDC_MAX_HSYNC_WIDTH            (1 << 12)
#define LTDC_MAX_VSYNC_HEIGHT           (1 << 11)

#define LTDC_MIN_HBP_WIDTH              0
#define LTDC_MIN_VBP_HEIGHT             0
#define LTDC_MAX_HBP_WIDTH              (1 << 12)
#define LTDC_MAX_VBP_HEIGHT             (1 << 11)

#define LTDC_MIN_ACC_HBP_WIDTH          1
#define LTDC_MIN_ACC_VBP_HEIGHT         1
#define LTDC_MAX_ACC_HBP_WIDTH          (1 << 12)
#define LTDC_MAX_ACC_VBP_HEIGHT         (1 << 11)

#define LTDC_MIN_HFP_WIDTH              0
#define LTDC_MIN_VFP_HEIGHT             0
#define LTDC_MAX_HFP_WIDTH              (1 << 12)
#define LTDC_MAX_VFP_HEIGHT             (1 << 11)

#define LTDC_MIN_ACTIVE_WIDTH           0
#define LTDC_MIN_ACTIVE_HEIGHT          0
#define LTDC_MAX_ACTIVE_WIDTH           (1 << 12)
#define LTDC_MAX_ACTIVE_HEIGHT          (1 << 11)

#define LTDC_MIN_ACC_ACTIVE_WIDTH       1
#define LTDC_MIN_ACC_ACTIVE_HEIGHT      1
#define LTDC_MAX_ACC_ACTIVE_WIDTH       (1 << 12)
#define LTDC_MAX_ACC_ACTIVE_HEIGHT      (1 << 11)

#define LTDC_MIN_ACC_TOTAL_WIDTH        1
#define LTDC_MIN_ACC_TOTAL_HEIGHT       1
#define LTDC_MAX_ACC_TOTAL_WIDTH        (1 << 12)
#define LTDC_MAX_ACC_TOTAL_HEIGHT       (1 << 11)

#define LTDC_MIN_LINE_INTERRUPT_POS     0
#define LTDC_MAX_LINE_INTERRUPT_POS     ((1 << 11) - 1)

#define LTDC_MIN_WINDOW_HSTART          0
#define LTDC_MIN_WINDOW_HSTART          0
#define LTDC_MAX_WINDOW_HSTOP           ((1 << 12) - 1)
#define LTDC_MAX_WINDOW_HSTOP           ((1 << 12) - 1)

#define LTDC_MIN_WINDOW_VSTART          0
#define LTDC_MIN_WINDOW_VSTART          0
#define LTDC_MAX_WINDOW_VSTOP           ((1 << 11) - 1)
#define LTDC_MAX_WINDOW_VSTOP           ((1 << 11) - 1)

#define LTDC_MIN_FRAME_WIDTH_BYTES      0
#define LTDC_MIN_FRAME_HEIGHT_LINES     0
#define LTDC_MIN_FRAME_PITCH_BYTES      0
#define LTDC_MAX_FRAME_WIDTH_BYTES      ((1 << 13) - 1 - 3)
#define LTDC_MAX_FRAME_HEIGHT_LINES     ((1 << 11) - 1)
#define LTDC_MAX_FRAME_PITCH_BYTES      ((1 << 13) - 1)

#define LTDC_MIN_PIXFMT_ID              0
#define LTDC_MAX_PIXFMT_ID              7

#define LTDC_MAX_PALETTE_LENGTH         256

// LTDC basic ARGB-8888 colors.
#define LTDC_COLOR_BLACK        0xFF000000
#define LTDC_COLOR_MAROON       0xFF800000
#define LTDC_COLOR_GREEN        0xFF008000
#define LTDC_COLOR_OLIVE        0xFF808000
#define LTDC_COLOR_NAVY         0xFF000080
#define LTDC_COLOR_PURPLE       0xFF800080
#define LTDC_COLOR_TEAL         0xFF008080
#define LTDC_COLOR_SILVER       0xFFC0C0C0
#define LTDC_COLOR_GRAY         0xFF808080
#define LTDC_COLOR_RED          0xFFFF0000
#define LTDC_COLOR_LIME         0xFF00FF00
#define LTDC_COLOR_YELLOW       0xFFFFFF00
#define LTDC_COLOR_BLUE         0xFF0000FF
#define LTDC_COLOR_FUCHSIA      0xFFFF00FF
#define LTDC_COLOR_AQUA         0xFF00FFFF
#define LTDC_COLOR_WHITE        0xFFFFFFFF

#endif /* STM32_LTDC_H */
