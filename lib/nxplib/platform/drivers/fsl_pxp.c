/*
 * Copyright  2017 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_pxp.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.pxp"
#endif

/* The CSC2 coefficient is ###.####_#### */
#define PXP_CSC2_COEF_INT_WIDTH 2
#define PXP_CSC2_COEF_FRAC_WIDTH 8

/* Compatibility map macro. */
#if defined(PXP_PS_CLRKEYLOW_0_PIXEL_MASK) && (!defined(PXP_PS_CLRKEYLOW_PIXEL_MASK))
#define PS_CLRKEYLOW PS_CLRKEYLOW_0
#define PS_CLRKEYHIGH PS_CLRKEYHIGH_0
#endif
#if defined(PXP_AS_CLRKEYLOW_0_PIXEL_MASK) && (!defined(PXP_AS_CLRKEYLOW_PIXEL_MASK))
#define AS_CLRKEYLOW AS_CLRKEYLOW_0
#define AS_CLRKEYHIGH AS_CLRKEYHIGH_0
#endif

typedef union _u32_f32
{
    float f32;
    uint32_t u32;
} u32_f32_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Get the instance from the base address
 *
 * @param base PXP peripheral base address
 *
 * @return The PXP module instance
 */
static uint32_t PXP_GetInstance(PXP_Type *base);

#if !(defined(FSL_FEATURE_PXP_HAS_NO_CSC2) && FSL_FEATURE_PXP_HAS_NO_CSC2)
/*!
 * @brief Convert IEEE 754 float value to the value could be written to registers.
 *
 * This function converts the float value to integer value to set the scaler
 * and CSC parameters.
 *
 * This function is an alternative implemention of the following code with no
 * MISRA 2004 rule 10.4 error:
 *
 * @code
   return (uint32_t)(floatValue * (float)(1 << fracBits));
   @endcode
 *
 * @param floatValue The float value to convert.
 * @param intBits Bits number of integer part in result.
 * @param fracBits Bits number of fractional part in result.
 * @return The value to set to register.
 */
static uint32_t PXP_ConvertFloat(float floatValue, uint8_t intBits, uint8_t fracBits);
#endif

/*!
 * @brief Convert the desired scale fact to DEC and PS_SCALE.
 *
 * @param inputDimension Input dimension.
 * @param outputDimension Output dimension.
 * @param dec The decimation filter contr0l value.
 * @param scale The scale value set to register PS_SCALE.
 */
static void PXP_GetScalerParam(uint16_t inputDimension, uint16_t outputDimension, uint8_t *dec, uint32_t *scale);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief Pointers to PXP bases for each instance. */
static PXP_Type *const s_pxpBases[] = PXP_BASE_PTRS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Pointers to PXP clocks for each PXP submodule. */
static const clock_ip_name_t s_pxpClocks[] = PXP_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Code
 ******************************************************************************/
static uint32_t PXP_GetInstance(PXP_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_pxpBases); instance++)
    {
        if (s_pxpBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_pxpBases));

    return instance;
}

#if !(defined(FSL_FEATURE_PXP_HAS_NO_CSC2) && FSL_FEATURE_PXP_HAS_NO_CSC2)
static uint32_t PXP_ConvertFloat(float floatValue, uint8_t intBits, uint8_t fracBits)
{
    /* One bit reserved for sign bit. */
    assert(intBits + fracBits < 32);

    u32_f32_t u32_f32;
    uint32_t ret;

    u32_f32.f32        = floatValue;
    uint32_t floatBits = u32_f32.u32;
    int32_t expValue   = (int32_t)((floatBits & 0x7F800000U) >> 23U) - 127;

    ret = (floatBits & 0x007FFFFFU) | 0x00800000U;
    expValue += fracBits;

    if (expValue < 0)
    {
        return 0U;
    }
    else if (expValue > 23)
    {
        /* should not exceed 31-bit when left shift. */
        assert((expValue - 23) <= 7);
        ret <<= (expValue - 23);
    }
    else
    {
        ret >>= (23 - expValue);
    }

    /* Set the sign bit. */
    if (floatBits & 0x80000000U)
    {
        ret = ((~ret) + 1U) & ~(((uint32_t)-1) << (intBits + fracBits + 1));
    }

    return ret;
}
#endif

static void PXP_GetScalerParam(uint16_t inputDimension, uint16_t outputDimension, uint8_t *dec, uint32_t *scale)
{
    uint32_t scaleFact = ((uint32_t)inputDimension << 12U) / outputDimension;

    if (scaleFact >= (16U << 12U))
    {
        /* Desired fact is two large, use the largest support value. */
        *dec   = 3U;
        *scale = 0x2000U;
    }
    else
    {
        if (scaleFact > (8U << 12U))
        {
            *dec = 3U;
        }
        else if (scaleFact > (4U << 12U))
        {
            *dec = 2U;
        }
        else if (scaleFact > (2U << 12U))
        {
            *dec = 1U;
        }
        else
        {
            *dec = 0U;
        }

        *scale = scaleFact >> (*dec);

        if (0U == *scale)
        {
            *scale = 1U;
        }
    }
}

/*!
 * brief Initialize the PXP.
 *
 * This function enables the PXP peripheral clock, and resets the PXP registers
 * to default status.
 *
 * param base PXP peripheral base address.
 */
void PXP_Init(PXP_Type *base)
{
    uint32_t ctrl = 0U;
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    uint32_t instance = PXP_GetInstance(base);
    CLOCK_EnableClock(s_pxpClocks[instance]);
#endif

    PXP_Reset(base);

/* Enable the process engine in primary processing flow. */
#if defined(PXP_CTRL_ENABLE_ROTATE0_MASK)
    ctrl |= PXP_CTRL_ENABLE_ROTATE0_MASK;
#endif
#if defined(PXP_CTRL_ENABLE_ROTATE1_MASK)
    ctrl |= PXP_CTRL_ENABLE_ROTATE1_MASK;
#endif
#if defined(PXP_CTRL_ENABLE_CSC2_MASK)
    ctrl |= PXP_CTRL_ENABLE_CSC2_MASK;
#endif
#if defined(PXP_CTRL_ENABLE_LUT_MASK)
    ctrl |= PXP_CTRL_ENABLE_LUT_MASK;
#endif
#if defined(PXP_CTRL_ENABLE_PS_AS_OUT_MASK)
    ctrl |= PXP_CTRL_ENABLE_PS_AS_OUT_MASK;
#endif

    base->CTRL = ctrl;
}

/*!
 * brief De-initialize the PXP.
 *
 * This function disables the PXP peripheral clock.
 *
 * param base PXP peripheral base address.
 */
void PXP_Deinit(PXP_Type *base)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    uint32_t instance = PXP_GetInstance(base);
    CLOCK_DisableClock(s_pxpClocks[instance]);
#endif
}

/*!
 * brief Reset the PXP.
 *
 * This function resets the PXP peripheral registers to default status.
 *
 * param base PXP peripheral base address.
 */
void PXP_Reset(PXP_Type *base)
{
    base->CTRL_SET = PXP_CTRL_SFTRST_MASK;
    base->CTRL_CLR = (PXP_CTRL_SFTRST_MASK | PXP_CTRL_CLKGATE_MASK);
}

/*!
 * brief Set the alpha surface input buffer configuration.
 *
 * param base PXP peripheral base address.
 * param config Pointer to the configuration.
 */
void PXP_SetAlphaSurfaceBufferConfig(PXP_Type *base, const pxp_as_buffer_config_t *config)
{
    assert(config);

    base->AS_CTRL = (base->AS_CTRL & ~PXP_AS_CTRL_FORMAT_MASK) | PXP_AS_CTRL_FORMAT(config->pixelFormat);

    base->AS_BUF   = config->bufferAddr;
    base->AS_PITCH = config->pitchBytes;
}

/*!
 * brief Set the alpha surface blending configuration.
 *
 * param base PXP peripheral base address.
 * param config Pointer to the configuration structure.
 */
void PXP_SetAlphaSurfaceBlendConfig(PXP_Type *base, const pxp_as_blend_config_t *config)
{
    assert(config);
    uint32_t reg;

    reg = base->AS_CTRL;
    reg &=
        ~(PXP_AS_CTRL_ALPHA0_INVERT_MASK | PXP_AS_CTRL_ROP_MASK | PXP_AS_CTRL_ALPHA_MASK | PXP_AS_CTRL_ALPHA_CTRL_MASK);
    reg |= (PXP_AS_CTRL_ROP(config->ropMode) | PXP_AS_CTRL_ALPHA(config->alpha) |
            PXP_AS_CTRL_ALPHA_CTRL(config->alphaMode));

    if (config->invertAlpha)
    {
        reg |= PXP_AS_CTRL_ALPHA0_INVERT_MASK;
    }

    base->AS_CTRL = reg;
}

/*!
 * brief Set the alpha surface position in output buffer.
 *
 * param base PXP peripheral base address.
 * param upperLeftX X of the upper left corner.
 * param upperLeftY Y of the upper left corner.
 * param lowerRightX X of the lower right corner.
 * param lowerRightY Y of the lower right corner.
 */
void PXP_SetAlphaSurfacePosition(
    PXP_Type *base, uint16_t upperLeftX, uint16_t upperLeftY, uint16_t lowerRightX, uint16_t lowerRightY)
{
    base->OUT_AS_ULC = PXP_OUT_AS_ULC_Y(upperLeftY) | PXP_OUT_AS_ULC_X(upperLeftX);
    base->OUT_AS_LRC = PXP_OUT_AS_LRC_Y(lowerRightY) | PXP_OUT_AS_LRC_X(lowerRightX);
}

/*!
 * brief Set the alpha surface overlay color key.
 *
 * If a pixel in the current overlay image with a color that falls in the range
 * from the p colorKeyLow to p colorKeyHigh range, it will use the process surface
 * pixel value for that location. If no PS image is present or if the PS image also
 * matches its colorkey range, the PS background color is used.
 *
 * param base PXP peripheral base address.
 * param colorKeyLow Color key low range.
 * param colorKeyHigh Color key high range.
 *
 * note Colorkey operations are higher priority than alpha or ROP operations
 */
void PXP_SetAlphaSurfaceOverlayColorKey(PXP_Type *base, uint32_t colorKeyLow, uint32_t colorKeyHigh)
{
    base->AS_CLRKEYLOW  = colorKeyLow;
    base->AS_CLRKEYHIGH = colorKeyHigh;
}

/*!
 * brief Set the process surface input buffer configuration.
 *
 * param base PXP peripheral base address.
 * param config Pointer to the configuration.
 */
void PXP_SetProcessSurfaceBufferConfig(PXP_Type *base, const pxp_ps_buffer_config_t *config)
{
    assert(config);

    base->PS_CTRL = ((base->PS_CTRL & ~(PXP_PS_CTRL_FORMAT_MASK | PXP_PS_CTRL_WB_SWAP_MASK)) |
                     PXP_PS_CTRL_FORMAT(config->pixelFormat) | PXP_PS_CTRL_WB_SWAP(config->swapByte));

    base->PS_BUF   = config->bufferAddr;
    base->PS_UBUF  = config->bufferAddrU;
    base->PS_VBUF  = config->bufferAddrV;
    base->PS_PITCH = config->pitchBytes;
}

/*!
 * brief Set the process surface scaler configuration.
 *
 * The valid down scale fact is 1/(2^12) ~ 16.
 *
 * param base PXP peripheral base address.
 * param inputWidth Input image width.
 * param inputHeight Input image height.
 * param outputWidth Output image width.
 * param outputHeight Output image height.
 */
void PXP_SetProcessSurfaceScaler(
    PXP_Type *base, uint16_t inputWidth, uint16_t inputHeight, uint16_t outputWidth, uint16_t outputHeight)
{
    uint8_t decX, decY;
    uint32_t scaleX, scaleY;

    PXP_GetScalerParam(inputWidth, outputWidth, &decX, &scaleX);
    PXP_GetScalerParam(inputHeight, outputHeight, &decY, &scaleY);

    base->PS_CTRL = (base->PS_CTRL & ~(PXP_PS_CTRL_DECX_MASK | PXP_PS_CTRL_DECY_MASK)) | PXP_PS_CTRL_DECX(decX) |
                    PXP_PS_CTRL_DECY(decY);

    base->PS_SCALE = PXP_PS_SCALE_XSCALE(scaleX) | PXP_PS_SCALE_YSCALE(scaleY);
}

/*!
 * brief Set the process surface position in output buffer.
 *
 * param base PXP peripheral base address.
 * param upperLeftX X of the upper left corner.
 * param upperLeftY Y of the upper left corner.
 * param lowerRightX X of the lower right corner.
 * param lowerRightY Y of the lower right corner.
 */
void PXP_SetProcessSurfacePosition(
    PXP_Type *base, uint16_t upperLeftX, uint16_t upperLeftY, uint16_t lowerRightX, uint16_t lowerRightY)
{
    base->OUT_PS_ULC = PXP_OUT_PS_ULC_Y(upperLeftY) | PXP_OUT_PS_ULC_X(upperLeftX);
    base->OUT_PS_LRC = PXP_OUT_PS_LRC_Y(lowerRightY) | PXP_OUT_PS_LRC_X(lowerRightX);
}

/*!
 * brief Set the process surface color key.
 *
 * If the PS image matches colorkey range, the PS background color is output. Set
 * p colorKeyLow to 0xFFFFFFFF and p colorKeyHigh to 0 will disable the colorkeying.
 *
 * param base PXP peripheral base address.
 * param colorKeyLow Color key low range.
 * param colorKeyHigh Color key high range.
 */
void PXP_SetProcessSurfaceColorKey(PXP_Type *base, uint32_t colorKeyLow, uint32_t colorKeyHigh)
{
    base->PS_CLRKEYLOW  = colorKeyLow;
    base->PS_CLRKEYHIGH = colorKeyHigh;
}

/*!
 * brief Set the PXP outpt buffer configuration.
 *
 * param base PXP peripheral base address.
 * param config Pointer to the configuration.
 */
void PXP_SetOutputBufferConfig(PXP_Type *base, const pxp_output_buffer_config_t *config)
{
    assert(config);

    base->OUT_CTRL = (base->OUT_CTRL & ~(PXP_OUT_CTRL_FORMAT_MASK | PXP_OUT_CTRL_INTERLACED_OUTPUT_MASK)) |
                     PXP_OUT_CTRL_FORMAT(config->pixelFormat) | PXP_OUT_CTRL_INTERLACED_OUTPUT(config->interlacedMode);

    base->OUT_BUF  = config->buffer0Addr;
    base->OUT_BUF2 = config->buffer1Addr;

    base->OUT_PITCH = config->pitchBytes;
    base->OUT_LRC   = PXP_OUT_LRC_Y(config->height - 1U) | PXP_OUT_LRC_X(config->width - 1U);

/*
 * The dither store size must be set to the same with the output buffer size,
 * otherwise the dither engine could not work.
 */
#if defined(PXP_DITHER_STORE_SIZE_CH0_OUT_WIDTH_MASK)
    base->DITHER_STORE_SIZE_CH0 = PXP_DITHER_STORE_SIZE_CH0_OUT_WIDTH(config->width - 1U) |
                                  PXP_DITHER_STORE_SIZE_CH0_OUT_HEIGHT(config->height - 1U);
#endif
}

#if !(defined(FSL_FEATURE_PXP_HAS_NO_CSC2) && FSL_FEATURE_PXP_HAS_NO_CSC2)
/*!
 * brief Set the CSC2 configuration.
 *
 * The CSC2 module receives pixels in any color space and can convert the pixels
 * into any of RGB, YUV, or YCbCr color spaces. The output pixels are passed
 * onto the LUT and rotation engine for further processing
 *
 * param base PXP peripheral base address.
 * param config Pointer to the configuration.
 */
void PXP_SetCsc2Config(PXP_Type *base, const pxp_csc2_config_t *config)
{
    assert(config);

    base->CSC2_CTRL = (base->CSC2_CTRL & ~PXP_CSC2_CTRL_CSC_MODE_MASK) | PXP_CSC2_CTRL_CSC_MODE(config->mode);

    base->CSC2_COEF0 =
        (PXP_ConvertFloat(config->A1, PXP_CSC2_COEF_INT_WIDTH, PXP_CSC2_COEF_FRAC_WIDTH) << PXP_CSC2_COEF0_A1_SHIFT) |
        (PXP_ConvertFloat(config->A2, PXP_CSC2_COEF_INT_WIDTH, PXP_CSC2_COEF_FRAC_WIDTH) << PXP_CSC2_COEF0_A2_SHIFT);

    base->CSC2_COEF1 =
        (PXP_ConvertFloat(config->A3, PXP_CSC2_COEF_INT_WIDTH, PXP_CSC2_COEF_FRAC_WIDTH) << PXP_CSC2_COEF1_A3_SHIFT) |
        (PXP_ConvertFloat(config->B1, PXP_CSC2_COEF_INT_WIDTH, PXP_CSC2_COEF_FRAC_WIDTH) << PXP_CSC2_COEF1_B1_SHIFT);

    base->CSC2_COEF2 =
        (PXP_ConvertFloat(config->B2, PXP_CSC2_COEF_INT_WIDTH, PXP_CSC2_COEF_FRAC_WIDTH) << PXP_CSC2_COEF2_B2_SHIFT) |
        (PXP_ConvertFloat(config->B3, PXP_CSC2_COEF_INT_WIDTH, PXP_CSC2_COEF_FRAC_WIDTH) << PXP_CSC2_COEF2_B3_SHIFT);

    base->CSC2_COEF3 =
        (PXP_ConvertFloat(config->C1, PXP_CSC2_COEF_INT_WIDTH, PXP_CSC2_COEF_FRAC_WIDTH) << PXP_CSC2_COEF3_C1_SHIFT) |
        (PXP_ConvertFloat(config->C2, PXP_CSC2_COEF_INT_WIDTH, PXP_CSC2_COEF_FRAC_WIDTH) << PXP_CSC2_COEF3_C2_SHIFT);

    base->CSC2_COEF4 =
        (PXP_ConvertFloat(config->C3, PXP_CSC2_COEF_INT_WIDTH, PXP_CSC2_COEF_FRAC_WIDTH) << PXP_CSC2_COEF4_C3_SHIFT) |
        PXP_CSC2_COEF4_D1(config->D1);

    base->CSC2_COEF5 = PXP_CSC2_COEF5_D2(config->D2) | PXP_CSC2_COEF5_D3(config->D3);
}
#endif

/*!
 * brief Set the CSC1 mode.
 *
 * The CSC1 module receives scaled YUV/YCbCr444 pixels from the scale engine and
 * converts the pixels to the RGB888 color space. It could only be used by process
 * surface.
 *
 * param base PXP peripheral base address.
 * param mode The conversion mode.
 */
void PXP_SetCsc1Mode(PXP_Type *base, pxp_csc1_mode_t mode)
{
    /*
     * The equations used for Colorspace conversion are:
     *
     * R = C0*(Y+Y_OFFSET)                   + C1(V+UV_OFFSET)
     * G = C0*(Y+Y_OFFSET) + C3(U+UV_OFFSET) + C2(V+UV_OFFSET)
     * B = C0*(Y+Y_OFFSET) + C4(U+UV_OFFSET)
     */

    if (kPXP_Csc1YUV2RGB == mode)
    {
        base->CSC1_COEF0 = (base->CSC1_COEF0 & ~(PXP_CSC1_COEF0_C0_MASK | PXP_CSC1_COEF0_Y_OFFSET_MASK |
                                                 PXP_CSC1_COEF0_UV_OFFSET_MASK | PXP_CSC1_COEF0_YCBCR_MODE_MASK)) |
                           PXP_CSC1_COEF0_C0(0x100U)         /* 1.00. */
                           | PXP_CSC1_COEF0_Y_OFFSET(0x0U)   /* 0. */
                           | PXP_CSC1_COEF0_UV_OFFSET(0x0U); /* 0. */
        base->CSC1_COEF1 = PXP_CSC1_COEF1_C1(0x0123U)        /* 1.140. */
                           | PXP_CSC1_COEF1_C4(0x0208U);     /* 2.032. */
        base->CSC1_COEF2 = PXP_CSC1_COEF2_C2(0x076BU)        /* -0.851. */
                           | PXP_CSC1_COEF2_C3(0x079BU);     /* -0.394. */
    }
    else
    {
        base->CSC1_COEF0 = (base->CSC1_COEF0 &
                            ~(PXP_CSC1_COEF0_C0_MASK | PXP_CSC1_COEF0_Y_OFFSET_MASK | PXP_CSC1_COEF0_UV_OFFSET_MASK)) |
                           PXP_CSC1_COEF0_YCBCR_MODE_MASK | PXP_CSC1_COEF0_C0(0x12AU) /* 1.164. */
                           | PXP_CSC1_COEF0_Y_OFFSET(0x1F0U)                          /* -16. */
                           | PXP_CSC1_COEF0_UV_OFFSET(0x180U);                        /* -128. */
        base->CSC1_COEF1 = PXP_CSC1_COEF1_C1(0x0198U)                                 /* 1.596. */
                           | PXP_CSC1_COEF1_C4(0x0204U);                              /* 2.017. */
        base->CSC1_COEF2 = PXP_CSC1_COEF2_C2(0x0730U)                                 /* -0.813. */
                           | PXP_CSC1_COEF2_C3(0x079CU);                              /* -0.392. */
    }
}

#if !(defined(FSL_FEATURE_PXP_HAS_NO_LUT) && FSL_FEATURE_PXP_HAS_NO_LUT)
/*!
 * brief Set the LUT configuration.
 *
 * The lookup table (LUT) is used to modify pixels in a manner that is not linear
 * and that cannot be achieved by the color space conversion modules. To setup
 * the LUT, the complete workflow is:
 * 1. Use ref PXP_SetLutConfig to set the configuration, such as the lookup mode.
 * 2. Use ref PXP_LoadLutTable to load the lookup table to PXP.
 * 3. Use ref PXP_EnableLut to enable the function.
 *
 * param base PXP peripheral base address.
 * param config Pointer to the configuration.
 */
void PXP_SetLutConfig(PXP_Type *base, const pxp_lut_config_t *config)
{
    base->LUT_CTRL = (base->LUT_CTRL & ~(PXP_LUT_CTRL_OUT_MODE_MASK | PXP_LUT_CTRL_LOOKUP_MODE_MASK)) |
                     PXP_LUT_CTRL_LRU_UPD_MASK | /* Use Least Recently Used Policy Update Control. */
                     PXP_LUT_CTRL_OUT_MODE(config->outMode) | PXP_LUT_CTRL_LOOKUP_MODE(config->lookupMode);

    if (kPXP_LutOutRGBW4444CFA == config->outMode)
    {
        base->CFA = config->cfaValue;
    }
}

/*!
 * brief Set the look up table to PXP.
 *
 * If lookup mode is DIRECT mode, this function loads p bytesNum of values
 * from the address p memAddr into PXP LUT address p lutStartAddr. So this
 * function allows only update part of the PXP LUT.
 *
 * If lookup mode is CACHE mode, this function sets the new address to p memAddr
 * and invalid the PXP LUT cache.
 *
 * param base PXP peripheral base address.
 * param lookupMode Which lookup mode is used. Note that this parameter is only
 * used to distinguish DIRECT mode and CACHE mode, it does not change the register
 * value PXP_LUT_CTRL[LOOKUP_MODE]. To change that value, use function ref PXP_SetLutConfig.
 * param bytesNum How many bytes to set. This value must be divisable by 8.
 * param memAddr Address of look up table to set.
 * param lutStartAddr The LUT value will be loaded to LUT from index lutAddr. It should
 * be 8 bytes aligned.
 *
 * retval kStatus_Success Load successfully.
 * retval kStatus_InvalidArgument Failed because of invalid argument.
 */
status_t PXP_LoadLutTable(
    PXP_Type *base, pxp_lut_lookup_mode_t lookupMode, uint32_t bytesNum, uint32_t memAddr, uint16_t lutStartAddr)
{
    if (kPXP_LutCacheRGB565 == lookupMode)
    {
        /* Make sure the previous memory write is finished, especially the LUT data memory. */
        __DSB();

        base->LUT_EXTMEM = memAddr;
        /* Invalid cache. */
        base->LUT_CTRL |= PXP_LUT_CTRL_INVALID_MASK;
    }
    else
    {
        /* Number of bytes must be divisable by 8. */
        if ((bytesNum & 0x07U) || (bytesNum < 8U) || (lutStartAddr & 0x07U) ||
            (bytesNum + lutStartAddr > PXP_LUT_TABLE_BYTE))
        {
            return kStatus_InvalidArgument;
        }

        base->LUT_EXTMEM = memAddr;
        base->LUT_ADDR   = PXP_LUT_ADDR_ADDR(lutStartAddr) | PXP_LUT_ADDR_NUM_BYTES(bytesNum);

        base->STAT_CLR = PXP_STAT_LUT_DMA_LOAD_DONE_IRQ_MASK;

        /* Start DMA transfer. */
        base->LUT_CTRL |= PXP_LUT_CTRL_DMA_START_MASK;

        __DSB();

        /* Wait for transfer completed. */
        while (!(base->STAT & PXP_STAT_LUT_DMA_LOAD_DONE_IRQ_MASK))
        {
        }
    }

    return kStatus_Success;
}
#endif /* FSL_FEATURE_PXP_HAS_NO_LUT */

#if (defined(FSL_FEATURE_PXP_HAS_DITHER) && FSL_FEATURE_PXP_HAS_DITHER)
/*!
 * brief Write data to the PXP internal memory.
 *
 * param base PXP peripheral base address.
 * param ram Which internal memory to write.
 * param bytesNum How many bytes to write.
 * param data Pointer to the data to write.
 * param memStartAddr The start address in the internal memory to write the data.
 */
void PXP_SetInternalRamData(PXP_Type *base, pxp_ram_t ram, uint32_t bytesNum, uint8_t *data, uint16_t memStartAddr)
{
    assert((memStartAddr + bytesNum) <= PXP_INTERNAL_RAM_LUT_BYTE);

    base->INIT_MEM_CTRL =
        PXP_INIT_MEM_CTRL_ADDR(memStartAddr) | PXP_INIT_MEM_CTRL_SELECT(ram) | PXP_INIT_MEM_CTRL_START_MASK;

    while (bytesNum--)
    {
        base->INIT_MEM_DATA = (uint32_t)(*data);
        data++;
    }

    base->INIT_MEM_CTRL = 0U;
}

/*!
 * brief Set the dither final LUT data.
 *
 * The dither final LUT is only applicble to dither engine 0. It takes the bits[7:4]
 * of the output pixel and looks up and 8 bit value from the 16 value LUT to generate
 * the final output pixel to the next process module.
 *
 * param base PXP peripheral base address.
 * param data Pointer to the LUT data to set.
 */
void PXP_SetDitherFinalLutData(PXP_Type *base, const pxp_dither_final_lut_data_t *data)
{
    base->DITHER_FINAL_LUT_DATA0 = data->data_3_0;
    base->DITHER_FINAL_LUT_DATA1 = data->data_7_4;
    base->DITHER_FINAL_LUT_DATA2 = data->data_11_8;
    base->DITHER_FINAL_LUT_DATA3 = data->data_15_12;
}

/*!
 * brief Enable or disable dither engine in the PXP process path.
 *
 * After the initialize function ref PXP_Init, the dither engine is disabled and not
 * use in the PXP processing path. This function enables the dither engine and
 * routes the dither engine output to the output buffer. When the dither engine
 * is enabled using this function, ref PXP_SetDitherConfig must be called to
 * configure dither engine correctly, otherwise there is not output to the output
 * buffer.
 *
 * param base PXP peripheral base address.
 * param enable Pass in true to enable, false to disable.
 */
void PXP_EnableDither(PXP_Type *base, bool enable)
{
    if (enable)
    {
        base->CTRL_SET = PXP_CTRL_ENABLE_DITHER_MASK;
        /* Route dither output to output buffer. */
        base->DATA_PATH_CTRL0 &= ~PXP_DATA_PATH_CTRL0_MUX14_SEL_MASK;
    }
    else
    {
        base->CTRL_CLR = PXP_CTRL_ENABLE_DITHER_MASK;
        /* Route MUX 12 output to output buffer. */
        base->DATA_PATH_CTRL0 |= PXP_DATA_PATH_CTRL0_MUX14_SEL(1U);
    }
}
#endif /* FSL_FEATURE_PXP_HAS_DITHER */
