/*
 * Copyright  2017 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_ELCDIF_H_
#define _FSL_ELCDIF_H_

#include "fsl_common.h"

/*!
 * @addtogroup elcdif
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief eLCDIF driver version */
#define FSL_ELCDIF_DRIVER_VERSION (MAKE_VERSION(2, 0, 1)) /*!< Version 2.0.1. */
/*@}*/

/* All IRQ flags in CTRL1 register. */
#define ELCDIF_CTRL1_IRQ_MASK                                                                         \
    (LCDIF_CTRL1_BM_ERROR_IRQ_MASK | LCDIF_CTRL1_OVERFLOW_IRQ_MASK | LCDIF_CTRL1_UNDERFLOW_IRQ_MASK | \
     LCDIF_CTRL1_CUR_FRAME_DONE_IRQ_MASK | LCDIF_CTRL1_VSYNC_EDGE_IRQ_MASK)

/* All IRQ enable control bits in CTRL1 register. */
#define ELCDIF_CTRL1_IRQ_EN_MASK                                                                               \
    (LCDIF_CTRL1_BM_ERROR_IRQ_EN_MASK | LCDIF_CTRL1_OVERFLOW_IRQ_EN_MASK | LCDIF_CTRL1_UNDERFLOW_IRQ_EN_MASK | \
     LCDIF_CTRL1_CUR_FRAME_DONE_IRQ_EN_MASK | LCDIF_CTRL1_VSYNC_EDGE_IRQ_EN_MASK)

/* All IRQ flags in AS_CTRL register. */
#if defined(LCDIF_AS_CTRL_CSI_SYNC_ON_IRQ_MASK)
#define ELCDIF_AS_CTRL_IRQ_MASK (LCDIF_AS_CTRL_CSI_SYNC_ON_IRQ_MASK)
#else
#define ELCDIF_AS_CTRL_IRQ_MASK 0U
#endif

/* All IRQ enable control bits in AS_CTRL register. */
#if defined(LCDIF_AS_CTRL_CSI_SYNC_ON_IRQ_EN_MASK)
#define ELCDIF_AS_CTRL_IRQ_EN_MASK (LCDIF_AS_CTRL_CSI_SYNC_ON_IRQ_EN_MASK)
#else
#define ELCDIF_AS_CTRL_IRQ_EN_MASK 0U
#endif

#if ((ELCDIF_CTRL1_IRQ_MASK & ELCDIF_AS_CTRL_IRQ_MASK) || (ELCDIF_AS_CTRL_IRQ_MASK & ELCDIF_AS_CTRL_IRQ_EN_MASK))
#error Interrupt bits overlap, need to update the interrupt functions.
#endif

/* LUT memory entery number. */
#define ELCDIF_LUT_ENTRY_NUM 256

/*!
 * @brief eLCDIF signal polarity flags
 */
enum _elcdif_polarity_flags
{
    kELCDIF_VsyncActiveLow            = 0U,                            /*!< VSYNC active low. */
    kELCDIF_VsyncActiveHigh           = LCDIF_VDCTRL0_VSYNC_POL_MASK,  /*!< VSYNC active high. */
    kELCDIF_HsyncActiveLow            = 0U,                            /*!< HSYNC active low. */
    kELCDIF_HsyncActiveHigh           = LCDIF_VDCTRL0_HSYNC_POL_MASK,  /*!< HSYNC active high. */
    kELCDIF_DataEnableActiveLow       = 0U,                            /*!< Data enable line active low. */
    kELCDIF_DataEnableActiveHigh      = LCDIF_VDCTRL0_ENABLE_POL_MASK, /*!< Data enable line active high. */
    kELCDIF_DriveDataOnFallingClkEdge = 0U, /*!< Drive data on falling clock edge, capture data
                                                 on rising clock edge. */
    kELCDIF_DriveDataOnRisingClkEdge = LCDIF_VDCTRL0_DOTCLK_POL_MASK, /*!< Drive data on falling
                                                                        clock edge, capture data
                                                                        on rising clock edge. */
};

/*!
 * @brief The eLCDIF interrupts to enable.
 */
enum _elcdif_interrupt_enable
{
    kELCDIF_BusMasterErrorInterruptEnable  = LCDIF_CTRL1_BM_ERROR_IRQ_EN_MASK,  /*!< Bus master error interrupt. */
    kELCDIF_TxFifoOverflowInterruptEnable  = LCDIF_CTRL1_OVERFLOW_IRQ_EN_MASK,  /*!< TXFIFO overflow interrupt. */
    kELCDIF_TxFifoUnderflowInterruptEnable = LCDIF_CTRL1_UNDERFLOW_IRQ_EN_MASK, /*!< TXFIFO underflow interrupt. */
    kELCDIF_CurFrameDoneInterruptEnable =
        LCDIF_CTRL1_CUR_FRAME_DONE_IRQ_EN_MASK, /*!< Interrupt when hardware enters vertical blanking state. */
    kELCDIF_VsyncEdgeInterruptEnable =
        LCDIF_CTRL1_VSYNC_EDGE_IRQ_EN_MASK, /*!< Interrupt when hardware encounters VSYNC edge. */
#if defined(LCDIF_AS_CTRL_CSI_SYNC_ON_IRQ_EN_MASK)
    kELCDIF_SciSyncOnInterruptEnable =
        LCDIF_AS_CTRL_CSI_SYNC_ON_IRQ_EN_MASK, /*!< Interrupt when eLCDIF lock with CSI input. */
#endif
};

/*!
 * @brief The eLCDIF interrupt status flags.
 */
enum _elcdif_interrupt_flags
{
    kELCDIF_BusMasterError  = LCDIF_CTRL1_BM_ERROR_IRQ_MASK,  /*!< Bus master error interrupt. */
    kELCDIF_TxFifoOverflow  = LCDIF_CTRL1_OVERFLOW_IRQ_MASK,  /*!< TXFIFO overflow interrupt. */
    kELCDIF_TxFifoUnderflow = LCDIF_CTRL1_UNDERFLOW_IRQ_MASK, /*!< TXFIFO underflow interrupt. */
    kELCDIF_CurFrameDone =
        LCDIF_CTRL1_CUR_FRAME_DONE_IRQ_MASK,             /*!< Interrupt when hardware enters vertical blanking state. */
    kELCDIF_VsyncEdge = LCDIF_CTRL1_VSYNC_EDGE_IRQ_MASK, /*!< Interrupt when hardware encounters VSYNC edge. */
#if defined(LCDIF_AS_CTRL_CSI_SYNC_ON_IRQ_MASK)
    kELCDIF_SciSyncOn = LCDIF_AS_CTRL_CSI_SYNC_ON_IRQ_MASK, /*!< Interrupt when eLCDIF lock with CSI input. */
#endif
};

/*!
 * @brief eLCDIF status flags
 */
enum _elcdif_status_flags
{
    kELCDIF_LFifoFull   = LCDIF_STAT_LFIFO_FULL_MASK,   /*!< LFIFO full. */
    kELCDIF_LFifoEmpty  = LCDIF_STAT_LFIFO_EMPTY_MASK,  /*!< LFIFO empty. */
    kELCDIF_TxFifoFull  = LCDIF_STAT_TXFIFO_FULL_MASK,  /*!< TXFIFO full. */
    kELCDIF_TxFifoEmpty = LCDIF_STAT_TXFIFO_EMPTY_MASK, /*!< TXFIFO empty. */
#if defined(LCDIF_STAT_BUSY_MASK)
    kELCDIF_LcdControllerBusy = LCDIF_STAT_BUSY_MASK, /*!< The external LCD controller busy signal. */
#endif
#if defined(LCDIF_STAT_DVI_CURRENT_FIELD_MASK)
    kELCDIF_CurDviField2 = LCDIF_STAT_DVI_CURRENT_FIELD_MASK, /*!< Current DVI filed, if set, then current filed is 2,
                                                               otherwise current filed is 1. */
#endif
};

/*!
 * @brief The pixel format.
 *
 * This enumerator should be defined together with the array s_pixelFormatReg.
 * To support new pixel format, enhance this enumerator and s_pixelFormatReg.
 */
typedef enum _elcdif_pixel_format
{
    kELCDIF_PixelFormatRAW8   = 0,   /*!< RAW 8 bit, four data use 32 bits. */
    kELCDIF_PixelFormatRGB565 = 1,   /*!< RGB565, two pixel use 32 bits. */
    kELCDIF_PixelFormatRGB666 = 2,   /*!< RGB666 unpacked, one pixel uses 32 bits, high byte unused,
                                          upper 2 bits of other bytes unused. */
    kELCDIF_PixelFormatXRGB8888 = 3, /*!< XRGB8888 unpacked, one pixel uses 32 bits, high byte unused. */
    kELCDIF_PixelFormatRGB888   = 4, /*!< RGB888 packed, one pixel uses 24 bits. */
} elcdif_pixel_format_t;

/*! @brief The LCD data bus type.  */
typedef enum _elcdif_lcd_data_bus
{
    kELCDIF_DataBus8Bit  = LCDIF_CTRL_LCD_DATABUS_WIDTH(1), /*!< 8-bit data bus. */
    kELCDIF_DataBus16Bit = LCDIF_CTRL_LCD_DATABUS_WIDTH(0), /*!< 16-bit data bus, support RGB565. */
    kELCDIF_DataBus18Bit = LCDIF_CTRL_LCD_DATABUS_WIDTH(2), /*!< 18-bit data bus, support RGB666. */
    kELCDIF_DataBus24Bit = LCDIF_CTRL_LCD_DATABUS_WIDTH(3), /*!< 24-bit data bus, support RGB888. */
} elcdif_lcd_data_bus_t;

/*!
 * @brief The register value when using different pixel format.
 *
 * These register bits control the pixel format:
 * - CTRL[DATA_FORMAT_24_BIT]
 * - CTRL[DATA_FORMAT_18_BIT]
 * - CTRL[DATA_FORMAT_16_BIT]
 * - CTRL[WORD_LENGTH]
 * - CTRL1[BYTE_PACKING_FORMAT]
 */
typedef struct _elcdif_pixel_format_reg
{
    uint32_t regCtrl;  /*!< Value of register CTRL. */
    uint32_t regCtrl1; /*!< Value of register CTRL1. */
} elcdif_pixel_format_reg_t;

/*!
 * @brief eLCDIF configure structure for RGB mode (DOTCLK mode).
 */
typedef struct _elcdif_rgb_mode_config
{
    uint16_t panelWidth;    /*!< Display panel width, pixels per line. */
    uint16_t panelHeight;   /*!< Display panel height, how many lines per panel. */
    uint8_t hsw;            /*!< HSYNC pulse width. */
    uint8_t hfp;            /*!< Horizontal front porch. */
    uint8_t hbp;            /*!< Horizontal back porch. */
    uint8_t vsw;            /*!< VSYNC pulse width. */
    uint8_t vfp;            /*!< Vrtical front porch. */
    uint8_t vbp;            /*!< Vertical back porch. */
    uint32_t polarityFlags; /*!< OR'ed value of @ref _elcdif_polarity_flags, used to contol the signal polarity. */
    uint32_t bufferAddr;    /*!< Frame buffer address. */
    elcdif_pixel_format_t pixelFormat; /*!< Pixel format. */
    elcdif_lcd_data_bus_t dataBus;     /*!< LCD data bus. */
} elcdif_rgb_mode_config_t;

/*!
 * @brief eLCDIF alpha surface pixel format.
 */
typedef enum _elcdif_as_pixel_format
{
    kELCDIF_AsPixelFormatARGB8888 = 0x0, /*!< 32-bit pixels with alpha. */
    kELCDIF_AsPixelFormatRGB888   = 0x4, /*!< 32-bit pixels without alpha (unpacked 24-bit format) */
    kELCDIF_AsPixelFormatARGB1555 = 0x8, /*!< 16-bit pixels with alpha. */
    kELCDIF_AsPixelFormatARGB4444 = 0x9, /*!< 16-bit pixels with alpha. */
    kELCDIF_AsPixelFormatRGB555   = 0xC, /*!< 16-bit pixels without alpha. */
    kELCDIF_AsPixelFormatRGB444   = 0xD, /*!< 16-bit pixels without alpha. */
    kELCDIF_AsPixelFormatRGB565   = 0xE, /*!< 16-bit pixels without alpha. */
} elcdif_as_pixel_format_t;

/*!
 * @brief eLCDIF alpha surface buffer configuration.
 */
typedef struct _elcdif_as_buffer_config
{
    uint32_t bufferAddr;                  /*!< Buffer address. */
    elcdif_as_pixel_format_t pixelFormat; /*!< Pixel format. */
} elcdif_as_buffer_config_t;

/*!
 * @brief eLCDIF alpha mode during blending.
 */
typedef enum _elcdif_alpha_mode
{
    kELCDIF_AlphaEmbedded, /*!< The alpha surface pixel alpha value will be used for blend. */
    kELCDIF_AlphaOverride, /*!< The user defined alpha value will be used for blend directly. */
    kELCDIF_AlphaMultiply, /*!< The alpha surface pixel alpha value scaled the user defined
                             alpha value will be used for blend, for example, pixel alpha set
                             set to 200, user defined alpha set to 100, then the reault alpha
                             is 200 * 100 / 255. */
    kELCDIF_AlphaRop       /*!< Raster operation. */
} elcdif_alpha_mode_t;

/*!
 * @brief eLCDIF ROP mode during blending.
 *
 * Explanation:
 * - AS: Alpha surface
 * - PS: Process surface
 * - nAS: Alpha surface NOT value
 * - nPS: Process surface NOT value
 */
typedef enum _elcdif_rop_mode
{
    kELCDIF_RopMaskAs     = 0x0, /*!< AS AND PS. */
    kELCDIF_RopMaskNotAs  = 0x1, /*!< nAS AND PS. */
    kELCDIF_RopMaskAsNot  = 0x2, /*!< AS AND nPS. */
    kELCDIF_RopMergeAs    = 0x3, /*!< AS OR PS. */
    kELCDIF_RopMergeNotAs = 0x4, /*!< nAS OR PS. */
    kELCDIF_RopMergeAsNot = 0x5, /*!< AS OR nPS. */
    kELCDIF_RopNotCopyAs  = 0x6, /*!< nAS. */
    kELCDIF_RopNot        = 0x7, /*!< nPS. */
    kELCDIF_RopNotMaskAs  = 0x8, /*!< AS NAND PS. */
    kELCDIF_RopNotMergeAs = 0x9, /*!< AS NOR PS. */
    kELCDIF_RopXorAs      = 0xA, /*!< AS XOR PS. */
    kELCDIF_RopNotXorAs   = 0xB  /*!< AS XNOR PS. */
} elcdif_rop_mode_t;

/*!
 * @brief eLCDIF alpha surface blending configuration.
 */
typedef struct _elcdif_as_blend_config
{
    uint8_t alpha;    /*!< User defined alpha value, only used when @ref alphaMode is @ref kELCDIF_AlphaOverride or @ref
                         kELCDIF_AlphaRop. */
    bool invertAlpha; /*!< Set true to invert the alpha. */
    elcdif_alpha_mode_t alphaMode; /*!< Alpha mode. */
    elcdif_rop_mode_t ropMode;     /*!< ROP mode, only valid when @ref alphaMode is @ref kELCDIF_AlphaRop. */
} elcdif_as_blend_config_t;

/*!
 * @brief eLCDIF LUT
 *
 * The Lookup Table (LUT) is used to expand the 8 bits pixel to 24 bits pixel
 * before output to external displayer.
 *
 * There are two 256x24 bits LUT memory in LCDIF, the LSB of frame buffer address
 * determins which memory to use.
 */
typedef enum _elcdif_lut
{
    kELCDIF_Lut0 = 0, /*!< LUT 0. */
    kELCDIF_Lut1,     /*!< LUT 1. */
} elcdif_lut_t;

/*******************************************************************************
 * APIs
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @name eLCDIF initialization and de-initialization
 * @{
 */

/*!
 * @brief Initializes the eLCDIF to work in RGB mode (DOTCLK mode).
 *
 * This function ungates the eLCDIF clock and configures the eLCDIF peripheral according
 * to the configuration structure.
 *
 * @param base eLCDIF peripheral base address.
 * @param config Pointer to the configuration structure.
 */
void ELCDIF_RgbModeInit(LCDIF_Type *base, const elcdif_rgb_mode_config_t *config);

/*!
 * @brief Gets the eLCDIF default configuration structure for RGB (DOTCLK) mode.
 *
 * This function sets the configuration structure to default values.
 * The default configuration is set to the following values.
 * @code
    config->panelWidth = 480U;
    config->panelHeight = 272U;
    config->hsw = 41;
    config->hfp = 4;
    config->hbp = 8;
    config->vsw = 10;
    config->vfp = 4;
    config->vbp = 2;
    config->polarityFlags = kELCDIF_VsyncActiveLow |
                            kELCDIF_HsyncActiveLow |
                            kELCDIF_DataEnableActiveLow |
                            kELCDIF_DriveDataOnFallingClkEdge;
    config->bufferAddr = 0U;
    config->pixelFormat = kELCDIF_PixelFormatRGB888;
    config->dataBus = kELCDIF_DataBus24Bit;
    @code
 *
 * @param config Pointer to the eLCDIF configuration structure.
 */
void ELCDIF_RgbModeGetDefaultConfig(elcdif_rgb_mode_config_t *config);

/*!
 * @brief Deinitializes the eLCDIF peripheral.
 *
 * @param base eLCDIF peripheral base address.
 */
void ELCDIF_Deinit(LCDIF_Type *base);

/* @} */

/*!
 * @name Module operation
 * @{
 */

/*!
 * @brief Set the pixel format in RGB (DOTCLK) mode.
 *
 * @param base eLCDIF peripheral base address.
 * @param pixelFormat The pixel format.
 */
void ELCDIF_RgbModeSetPixelFormat(LCDIF_Type *base, elcdif_pixel_format_t pixelFormat);

/*!
 * @brief Start to display in RGB (DOTCLK) mode.
 *
 * @param base eLCDIF peripheral base address.
 */
static inline void ELCDIF_RgbModeStart(LCDIF_Type *base)
{
    base->CTRL_SET = LCDIF_CTRL_RUN_MASK | LCDIF_CTRL_DOTCLK_MODE_MASK;
}

/*!
 * @brief Stop display in RGB (DOTCLK) mode and wait until finished.
 *
 * @param base eLCDIF peripheral base address.
 */
void ELCDIF_RgbModeStop(LCDIF_Type *base);

/*!
 * @brief Set the next frame buffer address to display.
 *
 * @param base eLCDIF peripheral base address.
 * @param bufferAddr The frame buffer address to set.
 */
static inline void ELCDIF_SetNextBufferAddr(LCDIF_Type *base, uint32_t bufferAddr)
{
    base->NEXT_BUF = bufferAddr;
}

/*!
 * @brief Reset the eLCDIF peripheral.
 *
 * @param base eLCDIF peripheral base address.
 */
void ELCDIF_Reset(LCDIF_Type *base);

#if !(defined(FSL_FEATURE_LCDIF_HAS_NO_RESET_PIN) && FSL_FEATURE_LCDIF_HAS_NO_RESET_PIN)
/*!
 * @brief Pull up or down the reset pin for the externel LCD controller.
 *
 * @param base eLCDIF peripheral base address.
 * @param pullUp True to pull up reset pin, false to pull down.
 */
static inline void ELCDIF_PullUpResetPin(LCDIF_Type *base, bool pullUp)
{
    if (pullUp)
    {
        base->CTRL1_SET = LCDIF_CTRL1_RESET_MASK;
    }
    else
    {
        base->CTRL1_CLR = LCDIF_CTRL1_RESET_MASK;
    }
}
#endif

/*!
 * @brief Enable or disable the hand shake with PXP.
 *
 * @param base eLCDIF peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void ELCDIF_EnablePxpHandShake(LCDIF_Type *base, bool enable)
{
    if (enable)
    {
        base->CTRL_SET = LCDIF_CTRL_ENABLE_PXP_HANDSHAKE_MASK;
    }
    else
    {
        base->CTRL_CLR = LCDIF_CTRL_ENABLE_PXP_HANDSHAKE_MASK;
    }
}

/* @} */

/*!
 * @name Status
 * @{
 */

/*!
 * @brief Get the CRC value of the frame sent out.
 *
 * When a frame is sent complete (the interrupt @ref kELCDIF_CurFrameDone assert), this function
 * can be used to get the CRC value of the frame sent.
 *
 * @param base eLCDIF peripheral base address.
 * @return The CRC value.
 *
 * @note The CRC value is dependent on the LCD_DATABUS_WIDTH.
 */
static inline uint32_t ELCDIF_GetCrcValue(LCDIF_Type *base)
{
    return base->CRC_STAT;
}

/*!
 * @brief Get the bus master error virtual address.
 *
 * When bus master error occurs (the interrupt kELCDIF_BusMasterError assert), this function
 * can get the virtual address at which the AXI master received an error
 * response from the slave.
 *
 * @param base eLCDIF peripheral base address.
 * @return The error virtual address.
 */
static inline uint32_t ELCDIF_GetBusMasterErrorAddr(LCDIF_Type *base)
{
    return base->BM_ERROR_STAT;
}

/*!
 * @brief Get the eLCDIF status.
 *
 * The status flags are returned as a mask value, application could check the
 * corresponding bit. Example:
 *
 * @code
   uint32_t statusFlags;
   statusFlags = ELCDIF_GetStatus(LCDIF);

   // If LFIFO is full.
   if (kELCDIF_LFifoFull & statusFlags)
   {
       // ...;
   }
   // If TXFIFO is empty.
   if (kELCDIF_TxFifoEmpty & statusFlags)
   {
       // ...;
   }
   @endcode
 *
 * @param base eLCDIF peripheral base address.
 * @return The mask value of status flags, it is OR'ed value of @ref _elcdif_status_flags.
 */
static inline uint32_t ELCDIF_GetStatus(LCDIF_Type *base)
{
    return base->STAT & (LCDIF_STAT_LFIFO_FULL_MASK | LCDIF_STAT_LFIFO_EMPTY_MASK | LCDIF_STAT_TXFIFO_FULL_MASK |
                         LCDIF_STAT_TXFIFO_EMPTY_MASK
#if defined(LCDIF_STAT_BUSY_MASK)
                         | LCDIF_STAT_BUSY_MASK
#endif
#if defined(LCDIF_STAT_DVI_CURRENT_FIELD_MASK)
                         | LCDIF_STAT_DVI_CURRENT_FIELD_MASK
#endif
                        );
}

/*!
 * @brief Get current count in Latency buffer (LFIFO).
 *
 * @param base eLCDIF peripheral base address.
 * @return The LFIFO current count
 */
static inline uint32_t ELCDIF_GetLFifoCount(LCDIF_Type *base)
{
    return (base->STAT & LCDIF_STAT_LFIFO_COUNT_MASK) >> LCDIF_STAT_LFIFO_COUNT_SHIFT;
}

/* @} */

/*!
 * @name Interrupts
 * @{
 */

/*!
 * @brief Enables eLCDIF interrupt requests.
 *
 * @param base eLCDIF peripheral base address.
 * @param mask interrupt source, OR'ed value of _elcdif_interrupt_enable.
 */
static inline void ELCDIF_EnableInterrupts(LCDIF_Type *base, uint32_t mask)
{
    base->CTRL1_SET = (mask & ELCDIF_CTRL1_IRQ_EN_MASK);
#if !(defined(FSL_FEATURE_LCDIF_HAS_NO_AS) && FSL_FEATURE_LCDIF_HAS_NO_AS)
    base->AS_CTRL |= (mask & ELCDIF_AS_CTRL_IRQ_EN_MASK);
#endif
}

/*!
 * @brief Disables eLCDIF interrupt requests.
 *
 * @param base eLCDIF peripheral base address.
 * @param mask interrupt source, OR'ed value of _elcdif_interrupt_enable.
 */
static inline void ELCDIF_DisableInterrupts(LCDIF_Type *base, uint32_t mask)
{
    base->CTRL1_CLR = (mask & ELCDIF_CTRL1_IRQ_EN_MASK);
#if !(defined(FSL_FEATURE_LCDIF_HAS_NO_AS) && FSL_FEATURE_LCDIF_HAS_NO_AS)
    base->AS_CTRL &= ~(mask & ELCDIF_AS_CTRL_IRQ_EN_MASK);
#endif
}

/*!
 * @brief Get eLCDIF interrupt peding status.
 *
 * @param base eLCDIF peripheral base address.
 * @return Interrupt pending status, OR'ed value of _elcdif_interrupt_flags.
 */
static inline uint32_t ELCDIF_GetInterruptStatus(LCDIF_Type *base)
{
    uint32_t flags;

    flags = (base->CTRL1 & ELCDIF_CTRL1_IRQ_MASK);
#if !(defined(FSL_FEATURE_LCDIF_HAS_NO_AS) && FSL_FEATURE_LCDIF_HAS_NO_AS)
    flags |= (base->AS_CTRL & ELCDIF_AS_CTRL_IRQ_MASK);
#endif

    return flags;
}

/*!
 * @brief Clear eLCDIF interrupt peding status.
 *
 * @param base eLCDIF peripheral base address.
 * @param mask of the flags to clear, OR'ed value of _elcdif_interrupt_flags.
 */
static inline void ELCDIF_ClearInterruptStatus(LCDIF_Type *base, uint32_t mask)
{
    base->CTRL1_CLR = (mask & ELCDIF_CTRL1_IRQ_MASK);
#if !(defined(FSL_FEATURE_LCDIF_HAS_NO_AS) && FSL_FEATURE_LCDIF_HAS_NO_AS)
    base->AS_CTRL &= ~(mask & ELCDIF_AS_CTRL_IRQ_MASK);
#endif
}

/* @} */

#if !(defined(FSL_FEATURE_LCDIF_HAS_NO_AS) && FSL_FEATURE_LCDIF_HAS_NO_AS)
/*!
 * @name Alpha surface
 * @{
 */

/*!
 * @brief Set the configuration for alpha surface buffer.
 *
 * @param base eLCDIF peripheral base address.
 * @param config Pointer to the configuration structure.
 */
void ELCDIF_SetAlphaSurfaceBufferConfig(LCDIF_Type *base, const elcdif_as_buffer_config_t *config);

/*!
 * @brief Set the alpha surface blending configuration.
 *
 * @param base eLCDIF peripheral base address.
 * @param config Pointer to the configuration structure.
 */
void ELCDIF_SetAlphaSurfaceBlendConfig(LCDIF_Type *base, const elcdif_as_blend_config_t *config);

/*!
 * @brief Set the next alpha surface buffer address.
 *
 * @param base eLCDIF peripheral base address.
 * @param bufferAddr Alpha surface buffer address.
 */
static inline void ELCDIF_SetNextAlphaSurfaceBufferAddr(LCDIF_Type *base, uint32_t bufferAddr)
{
    base->AS_NEXT_BUF = bufferAddr;
}

/*!
 * @brief Set the overlay color key.
 *
 * If a pixel in the current overlay image with a color that falls in the range
 * from the @p colorKeyLow to @p colorKeyHigh range, it will use the process surface
 * pixel value for that location.
 *
 * @param base eLCDIF peripheral base address.
 * @param colorKeyLow Color key low range.
 * @param colorKeyHigh Color key high range.
 *
 * @note Colorkey operations are higher priority than alpha or ROP operations
 */
static inline void ELCDIF_SetOverlayColorKey(LCDIF_Type *base, uint32_t colorKeyLow, uint32_t colorKeyHigh)
{
    base->AS_CLRKEYLOW  = colorKeyLow;
    base->AS_CLRKEYHIGH = colorKeyHigh;
}

/*!
 * @brief Enable or disable the color key.
 *
 * @param base eLCDIF peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void ELCDIF_EnableOverlayColorKey(LCDIF_Type *base, bool enable)
{
    if (enable)
    {
        base->AS_CTRL |= LCDIF_AS_CTRL_ENABLE_COLORKEY_MASK;
    }
    else
    {
        base->AS_CTRL &= ~LCDIF_AS_CTRL_ENABLE_COLORKEY_MASK;
    }
}

/*!
 * @brief Enable or disable the alpha surface.
 *
 * @param base eLCDIF peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void ELCDIF_EnableAlphaSurface(LCDIF_Type *base, bool enable)
{
    if (enable)
    {
        base->AS_CTRL |= LCDIF_AS_CTRL_AS_ENABLE_MASK;
    }
    else
    {
        base->AS_CTRL &= ~LCDIF_AS_CTRL_AS_ENABLE_MASK;
    }
}

/*!
 * @brief Enable or disable the process surface.
 *
 * Process surface is the normal frame buffer. The process surface content
 * is controlled by @ref ELCDIF_SetNextBufferAddr.
 *
 * @param base eLCDIF peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void ELCDIF_EnableProcessSurface(LCDIF_Type *base, bool enable)
{
    if (enable)
    {
        base->AS_CTRL &= ~LCDIF_AS_CTRL_PS_DISABLE_MASK;
    }
    else
    {
        base->AS_CTRL |= LCDIF_AS_CTRL_PS_DISABLE_MASK;
    }
}

/* @} */
#endif /* FSL_FEATURE_LCDIF_HAS_NO_AS */

#if (defined(FSL_FEATURE_LCDIF_HAS_LUT) && FSL_FEATURE_LCDIF_HAS_LUT)
/*!
 * @name LUT
 *
 * The Lookup Table (LUT) is used to expand the 8 bits pixel to 24 bits pixel
 * before output to external displayer.
 *
 * There are two 256x24 bits LUT memory in LCDIF, the LSB of frame buffer address
 * determins which memory to use.
 *
 * @{
 */

/*!
 * @brief Enable or disable the LUT.
 *
 * @param base eLCDIF peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void ELCDIF_EnableLut(LCDIF_Type *base, bool enable)
{
    if (enable)
    {
        base->LUT_CTRL &= ~LCDIF_LUT_CTRL_LUT_BYPASS_MASK;
    }
    else
    {
        base->LUT_CTRL |= LCDIF_LUT_CTRL_LUT_BYPASS_MASK;
    }
}

/*!
 * @brief Load the LUT value.
 *
 * This function loads the LUT value to the specific LUT memory, user can
 * specify the start entry index.
 *
 * @param base eLCDIF peripheral base address.
 * @param lut Which LUT to load.
 * @param startIndex The start index of the LUT entry to update.
 * @param lutData The LUT data to load.
 * @param count Count of @p lutData.
 * @retval kStatus_Success Initialization success.
 * @retval kStatus_InvalidArgument Wrong argument.
 */
status_t ELCDIF_UpdateLut(
    LCDIF_Type *base, elcdif_lut_t lut, uint16_t startIndex, const uint32_t *lutData, uint16_t count);

/* @} */
#endif /* FSL_FEATURE_LCDIF_HAS_LUT */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/* @} */

#endif /*_FSL_ELCDIF_H_*/
