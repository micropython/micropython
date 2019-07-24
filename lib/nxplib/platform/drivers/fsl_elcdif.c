/*
 * Copyright  2017 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_elcdif.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.elcdif"
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief Get instance number for ELCDIF module.
 *
 * @param base ELCDIF peripheral base address
 */
static uint32_t ELCDIF_GetInstance(LCDIF_Type *base);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief Pointers to ELCDIF bases for each instance. */
static LCDIF_Type *const s_elcdifBases[] = LCDIF_BASE_PTRS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Pointers to eLCDIF apb_clk for each instance. */
static const clock_ip_name_t s_elcdifApbClocks[] = LCDIF_CLOCKS;
#if defined(LCDIF_PERIPH_CLOCKS)
/*! @brief Pointers to eLCDIF pix_clk for each instance. */
static const clock_ip_name_t s_elcdifPixClocks[] = LCDIF_PERIPH_CLOCKS;
#endif
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*! @brief The control register value to select different pixel format. */
elcdif_pixel_format_reg_t s_pixelFormatReg[] = {
    /* kELCDIF_PixelFormatRAW8 */
    {/* Register CTRL. */
     LCDIF_CTRL_WORD_LENGTH(1U),
     /* Register CTRL1. */
     LCDIF_CTRL1_BYTE_PACKING_FORMAT(0x0FU)},
    /* kELCDIF_PixelFormatRGB565 */
    {/* Register CTRL. */
     LCDIF_CTRL_WORD_LENGTH(0U),
     /* Register CTRL1. */
     LCDIF_CTRL1_BYTE_PACKING_FORMAT(0x0FU)},
    /* kELCDIF_PixelFormatRGB666 */
    {/* Register CTRL. */
     LCDIF_CTRL_WORD_LENGTH(3U) | LCDIF_CTRL_DATA_FORMAT_24_BIT(1U),
     /* Register CTRL1. */
     LCDIF_CTRL1_BYTE_PACKING_FORMAT(0x07U)},
    /* kELCDIF_PixelFormatXRGB8888 */
    {/* Register CTRL. 24-bit. */
     LCDIF_CTRL_WORD_LENGTH(3U),
     /* Register CTRL1. */
     LCDIF_CTRL1_BYTE_PACKING_FORMAT(0x07U)},
    /* kELCDIF_PixelFormatRGB888 */
    {/* Register CTRL. 24-bit. */
     LCDIF_CTRL_WORD_LENGTH(3U),
     /* Register CTRL1. */
     LCDIF_CTRL1_BYTE_PACKING_FORMAT(0x0FU)},
};

/*******************************************************************************
 * Codes
 ******************************************************************************/
static uint32_t ELCDIF_GetInstance(LCDIF_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_elcdifBases); instance++)
    {
        if (s_elcdifBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_elcdifBases));

    return instance;
}

/*!
 * brief Initializes the eLCDIF to work in RGB mode (DOTCLK mode).
 *
 * This function ungates the eLCDIF clock and configures the eLCDIF peripheral according
 * to the configuration structure.
 *
 * param base eLCDIF peripheral base address.
 * param config Pointer to the configuration structure.
 */
void ELCDIF_RgbModeInit(LCDIF_Type *base, const elcdif_rgb_mode_config_t *config)
{
    assert(config);
    assert(config->pixelFormat < ARRAY_SIZE(s_pixelFormatReg));

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    uint32_t instance = ELCDIF_GetInstance(base);
    /* Enable the clock. */
    CLOCK_EnableClock(s_elcdifApbClocks[instance]);
#if defined(LCDIF_PERIPH_CLOCKS)
    CLOCK_EnableClock(s_elcdifPixClocks[instance]);
#endif
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    /* Reset. */
    ELCDIF_Reset(base);

    base->CTRL = s_pixelFormatReg[(uint32_t)config->pixelFormat].regCtrl | (uint32_t)(config->dataBus) |
                 LCDIF_CTRL_DOTCLK_MODE_MASK |  /* RGB mode. */
                 LCDIF_CTRL_BYPASS_COUNT_MASK | /* Keep RUN bit set. */
                 LCDIF_CTRL_MASTER_MASK;

    base->CTRL1 = s_pixelFormatReg[(uint32_t)config->pixelFormat].regCtrl1;

    base->TRANSFER_COUNT = ((uint32_t)config->panelHeight << LCDIF_TRANSFER_COUNT_V_COUNT_SHIFT) |
                           ((uint32_t)config->panelWidth << LCDIF_TRANSFER_COUNT_H_COUNT_SHIFT);

    base->VDCTRL0 = LCDIF_VDCTRL0_ENABLE_PRESENT_MASK |         /* Data enable signal. */
                    LCDIF_VDCTRL0_VSYNC_PERIOD_UNIT_MASK |      /* VSYNC period in the unit of display clock. */
                    LCDIF_VDCTRL0_VSYNC_PULSE_WIDTH_UNIT_MASK | /* VSYNC pulse width in the unit of display clock. */
                    (uint32_t)config->polarityFlags | (uint32_t)config->vsw;

    base->VDCTRL1 = config->vsw + config->panelHeight + config->vfp + config->vbp;
    base->VDCTRL2 = ((uint32_t)config->hsw << LCDIF_VDCTRL2_HSYNC_PULSE_WIDTH_SHIFT) |
                    ((uint32_t)(config->hfp + config->hbp + config->panelWidth + config->hsw))
                        << LCDIF_VDCTRL2_HSYNC_PERIOD_SHIFT;

    base->VDCTRL3 = (((uint32_t)config->hbp + config->hsw) << LCDIF_VDCTRL3_HORIZONTAL_WAIT_CNT_SHIFT) |
                    (((uint32_t)config->vbp + config->vsw) << LCDIF_VDCTRL3_VERTICAL_WAIT_CNT_SHIFT);

    base->VDCTRL4 = LCDIF_VDCTRL4_SYNC_SIGNALS_ON_MASK |
                    ((uint32_t)config->panelWidth << LCDIF_VDCTRL4_DOTCLK_H_VALID_DATA_CNT_SHIFT);

    base->CUR_BUF  = config->bufferAddr;
    base->NEXT_BUF = config->bufferAddr;
}

/*!
 * brief Gets the eLCDIF default configuration structure for RGB (DOTCLK) mode.
 *
 * This function sets the configuration structure to default values.
 * The default configuration is set to the following values.
 * code
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
    code
 *
 * param config Pointer to the eLCDIF configuration structure.
 */
void ELCDIF_RgbModeGetDefaultConfig(elcdif_rgb_mode_config_t *config)
{
    assert(config);

    /* Initializes the configure structure to zero. */
    memset(config, 0, sizeof(*config));

    config->panelWidth    = 480U;
    config->panelHeight   = 272U;
    config->hsw           = 41;
    config->hfp           = 4;
    config->hbp           = 8;
    config->vsw           = 10;
    config->vfp           = 4;
    config->vbp           = 2;
    config->polarityFlags = kELCDIF_VsyncActiveLow | kELCDIF_HsyncActiveLow | kELCDIF_DataEnableActiveLow |
                            kELCDIF_DriveDataOnFallingClkEdge;
    config->bufferAddr  = 0U;
    config->pixelFormat = kELCDIF_PixelFormatRGB888;
    config->dataBus     = kELCDIF_DataBus24Bit;
}

/*!
 * brief Set the pixel format in RGB (DOTCLK) mode.
 *
 * param base eLCDIF peripheral base address.
 * param pixelFormat The pixel format.
 */
void ELCDIF_RgbModeSetPixelFormat(LCDIF_Type *base, elcdif_pixel_format_t pixelFormat)
{
    assert(pixelFormat < ARRAY_SIZE(s_pixelFormatReg));

    base->CTRL = (base->CTRL & ~(LCDIF_CTRL_WORD_LENGTH_MASK | LCDIF_CTRL_DATA_FORMAT_24_BIT_MASK |
                                 LCDIF_CTRL_DATA_FORMAT_18_BIT_MASK | LCDIF_CTRL_DATA_FORMAT_16_BIT_MASK)) |
                 s_pixelFormatReg[(uint32_t)pixelFormat].regCtrl;

    base->CTRL1 = s_pixelFormatReg[(uint32_t)pixelFormat].regCtrl1;
}

/*!
 * brief Deinitializes the eLCDIF peripheral.
 *
 * param base eLCDIF peripheral base address.
 */
void ELCDIF_Deinit(LCDIF_Type *base)
{
    ELCDIF_Reset(base);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    uint32_t instance = ELCDIF_GetInstance(base);
/* Disable the clock. */
#if defined(LCDIF_PERIPH_CLOCKS)
    CLOCK_DisableClock(s_elcdifPixClocks[instance]);
#endif
    CLOCK_DisableClock(s_elcdifApbClocks[instance]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief Stop display in RGB (DOTCLK) mode and wait until finished.
 *
 * param base eLCDIF peripheral base address.
 */
void ELCDIF_RgbModeStop(LCDIF_Type *base)
{
    base->CTRL_CLR = LCDIF_CTRL_DOTCLK_MODE_MASK;

    /* Wait for data transfer finished. */
    while (base->CTRL & LCDIF_CTRL_DOTCLK_MODE_MASK)
    {
    }
}

/*!
 * brief Reset the eLCDIF peripheral.
 *
 * param base eLCDIF peripheral base address.
 */
void ELCDIF_Reset(LCDIF_Type *base)
{
    volatile uint32_t i = 0x100;

    /* Disable the clock gate. */
    base->CTRL_CLR = LCDIF_CTRL_CLKGATE_MASK;
    /* Confirm the clock gate is disabled. */
    while (base->CTRL & LCDIF_CTRL_CLKGATE_MASK)
    {
    }

    /* Reset the block. */
    base->CTRL_SET = LCDIF_CTRL_SFTRST_MASK;
    /* Confirm the reset bit is set. */
    while (!(base->CTRL & LCDIF_CTRL_SFTRST_MASK))
    {
    }

    /* Delay for the reset. */
    while (i--)
    {
    }

    /* Bring the module out of reset. */
    base->CTRL_CLR = LCDIF_CTRL_SFTRST_MASK;
    /* Disable the clock gate. */
    base->CTRL_CLR = LCDIF_CTRL_CLKGATE_MASK;
}

#if !(defined(FSL_FEATURE_LCDIF_HAS_NO_AS) && FSL_FEATURE_LCDIF_HAS_NO_AS)
/*!
 * brief Set the configuration for alpha surface buffer.
 *
 * param base eLCDIF peripheral base address.
 * param config Pointer to the configuration structure.
 */
void ELCDIF_SetAlphaSurfaceBufferConfig(LCDIF_Type *base, const elcdif_as_buffer_config_t *config)
{
    assert(config);

    base->AS_CTRL     = (base->AS_CTRL & ~LCDIF_AS_CTRL_FORMAT_MASK) | LCDIF_AS_CTRL_FORMAT(config->pixelFormat);
    base->AS_BUF      = config->bufferAddr;
    base->AS_NEXT_BUF = config->bufferAddr;
}

/*!
 * brief Set the alpha surface blending configuration.
 *
 * param base eLCDIF peripheral base address.
 * param config Pointer to the configuration structure.
 */
void ELCDIF_SetAlphaSurfaceBlendConfig(LCDIF_Type *base, const elcdif_as_blend_config_t *config)
{
    assert(config);
    uint32_t reg;

    reg = base->AS_CTRL;
    reg &= ~(LCDIF_AS_CTRL_ALPHA_INVERT_MASK | LCDIF_AS_CTRL_ROP_MASK | LCDIF_AS_CTRL_ALPHA_MASK |
             LCDIF_AS_CTRL_ALPHA_CTRL_MASK);
    reg |= (LCDIF_AS_CTRL_ROP(config->ropMode) | LCDIF_AS_CTRL_ALPHA(config->alpha) |
            LCDIF_AS_CTRL_ALPHA_CTRL(config->alphaMode));

    if (config->invertAlpha)
    {
        reg |= LCDIF_AS_CTRL_ALPHA_INVERT_MASK;
    }

    base->AS_CTRL = reg;
}
#endif /* FSL_FEATURE_LCDIF_HAS_NO_AS */

#if (defined(FSL_FEATURE_LCDIF_HAS_LUT) && FSL_FEATURE_LCDIF_HAS_LUT)
/*!
 * brief Load the LUT value.
 *
 * This function loads the LUT value to the specific LUT memory, user can
 * specify the start entry index.
 *
 * param base eLCDIF peripheral base address.
 * param lut Which LUT to load.
 * param startIndex The start index of the LUT entry to update.
 * param lutData The LUT data to load.
 * param count Count of p lutData.
 * retval kStatus_Success Initialization success.
 * retval kStatus_InvalidArgument Wrong argument.
 */
status_t ELCDIF_UpdateLut(
    LCDIF_Type *base, elcdif_lut_t lut, uint16_t startIndex, const uint32_t *lutData, uint16_t count)
{
    volatile uint32_t *regLutAddr;
    volatile uint32_t *regLutData;
    uint32_t i;

    /* Only has 256 entries. */
    if (startIndex + count > ELCDIF_LUT_ENTRY_NUM)
    {
        return kStatus_InvalidArgument;
    }

    if (kELCDIF_Lut0 == lut)
    {
        regLutAddr = &(base->LUT0_ADDR);
        regLutData = &(base->LUT0_DATA);
    }
    else
    {
        regLutAddr = &(base->LUT1_ADDR);
        regLutData = &(base->LUT1_DATA);
    }

    *regLutAddr = startIndex;

    for (i = 0; i < count; i++)
    {
        *regLutData = lutData[i];
    }

    return kStatus_Success;
}
#endif /* FSL_FEATURE_LCDIF_HAS_LUT */
