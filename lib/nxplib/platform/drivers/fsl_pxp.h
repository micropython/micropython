/*
 * Copyright  2017 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_PXP_H_
#define _FSL_PXP_H_

#include "fsl_common.h"

/* Compatibility macro map. */
#if defined(PXP_AS_CTRL_ALPHA_INVERT_MASK) && (!defined(PXP_AS_CTRL_ALPHA0_INVERT_MASK))
#define PXP_AS_CTRL_ALPHA0_INVERT_MASK PXP_AS_CTRL_ALPHA_INVERT_MASK
#endif

#if defined(PXP_AS_CTRL_ALPHA_INVERT_MASK) && (!defined(PXP_AS_CTRL_ALPHA_INVERT_MASK))
#define PXP_AS_CTRL_ALPHA0_INVERT_MASK PXP_AS_CTRL_ALPHA_INVERT_MASK
#endif

#if defined(PXP_STAT_IRQ_MASK) && (!defined(PXP_STAT_IRQ0_MASK))
#define PXP_STAT_IRQ0_MASK PXP_STAT_IRQ_MASK
#endif

#if defined(PXP_STAT_AXI_READ_ERROR_MASK) && (!defined(PXP_STAT_AXI_READ_ERROR_0_MASK))
#define PXP_STAT_AXI_READ_ERROR_0_MASK PXP_STAT_AXI_READ_ERROR_MASK
#endif

#if defined(PXP_STAT_AXI_WRITE_ERROR_MASK) && (!defined(PXP_STAT_AXI_WRITE_ERROR_0_MASK))
#define PXP_STAT_AXI_WRITE_ERROR_0_MASK PXP_STAT_AXI_WRITE_ERROR_MASK
#endif

/*!
 * @addtogroup pxp_driver
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* PXP global LUT table is 16K. */
#define PXP_LUT_TABLE_BYTE (16 * 1024)
/* Intenral memory for LUT, the size is 256 bytes. */
#define PXP_INTERNAL_RAM_LUT_BYTE (256)

/*! @name Driver version */
/*@{*/
#define FSL_PXP_DRIVER_VERSION (MAKE_VERSION(2, 0, 1)) /*!< Version 2.0.1 */
/*@}*/

/* This macto indicates whether the rotate sub module is shared by process surface and output buffer. */
#if defined(PXP_CTRL_ROT_POS_MASK)
#define PXP_SHARE_ROTATE 1
#else
#define PXP_SHARE_ROTATE 0
#endif

/*! @brief PXP interrupts to enable. */
enum _pxp_interrupt_enable
{
    kPXP_CommandLoadInterruptEnable = PXP_CTRL_NEXT_IRQ_ENABLE_MASK, /*!< Interrupt to show that the command set
                                                                         by @ref PXP_SetNextCommand has been loaded. */
    kPXP_CompleteInterruptEnable = PXP_CTRL_IRQ_ENABLE_MASK,         /*!< PXP process completed. */
#if !(defined(FSL_FEATURE_PXP_HAS_NO_LUT) && FSL_FEATURE_PXP_HAS_NO_LUT)
    kPXP_LutDmaLoadInterruptEnable = PXP_CTRL_LUT_DMA_IRQ_ENABLE_MASK, /*!< The LUT table has been loaded by DMA. */
#endif
};

/*!
 * @brief PXP status flags.
 *
 * @note These enumerations are meant to be OR'd together to form a bit mask.
 */
enum _pxp_flags
{
    kPXP_CommandLoadFlag = PXP_STAT_NEXT_IRQ_MASK, /*!< The command set by @ref PXP_SetNextCommand
                                                     has been loaded, could set new command. */
    kPXP_CompleteFlag = PXP_STAT_IRQ0_MASK,        /*!< PXP process completed. */
#if !(defined(FSL_FEATURE_PXP_HAS_NO_LUT) && FSL_FEATURE_PXP_HAS_NO_LUT)
    kPXP_LutDmaLoadFlag = PXP_STAT_LUT_DMA_LOAD_DONE_IRQ_MASK, /*!< The LUT table has been loaded by DMA. */
#endif
    kPXP_Axi0ReadErrorFlag = PXP_STAT_AXI_READ_ERROR_0_MASK,   /*!< PXP encountered an AXI read error
                                                              and processing has been terminated. */
    kPXP_Axi0WriteErrorFlag = PXP_STAT_AXI_WRITE_ERROR_0_MASK, /*!< PXP encountered an AXI write error
                                                            and processing has been terminated. */
#if defined(PXP_STAT_AXI_READ_ERROR_1_MASK)
    kPXP_Axi1ReadErrorFlag = PXP_STAT_AXI_READ_ERROR_1_MASK,   /*!< PXP encountered an AXI read error
                                                              and processing has been terminated. */
    kPXP_Axi1WriteErrorFlag = PXP_STAT_AXI_WRITE_ERROR_1_MASK, /*!< PXP encountered an AXI write error
                                                            and processing has been terminated. */
#endif
};

/*! @brief PXP output flip mode. */
typedef enum _pxp_flip_mode
{
    kPXP_FlipDisable    = 0U,    /*!< Flip disable. */
    kPXP_FlipHorizontal = 0x01U, /*!< Horizontal flip. */
    kPXP_FlipVertical   = 0x02U, /*!< Vertical flip. */
    kPXP_FlipBoth       = 0x03U, /*!< Flip both directions. */
} pxp_flip_mode_t;

/*! @brief PXP rotate mode. */
typedef enum _pxp_rotate_position
{
    kPXP_RotateOutputBuffer = 0U, /*!< Rotate the output buffer. */
    kPXP_RotateProcessSurface,    /*!< Rotate the process surface. */
} pxp_rotate_position_t;

/*! @brief PXP rotate degree. */
typedef enum _pxp_rotate_degree
{
    kPXP_Rotate0 = 0U, /*!< Clock wise rotate 0 deg. */
    kPXP_Rotate90,     /*!< Clock wise rotate 90 deg. */
    kPXP_Rotate180,    /*!< Clock wise rotate 180 deg. */
    kPXP_Rotate270,    /*!< Clock wise rotate 270 deg. */
} pxp_rotate_degree_t;

/*! @brief PXP interlaced output mode. */
typedef enum _pxp_interlaced_output_mode
{
    kPXP_OutputProgressive = 0U, /*!< All data written in progressive format to output buffer 0. */
    kPXP_OutputField0,           /*!< Only write field 0 data to output buffer 0. */
    kPXP_OutputField1,           /*!< Only write field 1 data to output buffer 0. */
    kPXP_OutputInterlaced,       /*!< Field 0 write to buffer 0, field 1 write to buffer 1. */
} pxp_interlaced_output_mode_t;

/*! @brief PXP output buffer format. */
typedef enum _pxp_output_pixel_format
{
    kPXP_OutputPixelFormatARGB8888  = 0x0,  /*!< 32-bit pixels with alpha. */
    kPXP_OutputPixelFormatRGB888    = 0x4,  /*!< 32-bit pixels without alpha (unpacked 24-bit format) */
    kPXP_OutputPixelFormatRGB888P   = 0x5,  /*!< 24-bit pixels without alpha (packed 24-bit format) */
    kPXP_OutputPixelFormatARGB1555  = 0x8,  /*!< 16-bit pixels with alpha. */
    kPXP_OutputPixelFormatARGB4444  = 0x9,  /*!< 16-bit pixels with alpha. */
    kPXP_OutputPixelFormatRGB555    = 0xC,  /*!< 16-bit pixels without alpha. */
    kPXP_OutputPixelFormatRGB444    = 0xD,  /*!< 16-bit pixels without alpha. */
    kPXP_OutputPixelFormatRGB565    = 0xE,  /*!< 16-bit pixels without alpha. */
    kPXP_OutputPixelFormatYUV1P444  = 0x10, /*!< 32-bit pixels (1-plane XYUV unpacked). */
    kPXP_OutputPixelFormatUYVY1P422 = 0x12, /*!< 16-bit pixels (1-plane U0,Y0,V0,Y1 interleaved bytes) */
    kPXP_OutputPixelFormatVYUY1P422 = 0x13, /*!< 16-bit pixels (1-plane V0,Y0,U0,Y1 interleaved bytes) */
    kPXP_OutputPixelFormatY8        = 0x14, /*!< 8-bit monochrome pixels (1-plane Y luma output) */
    kPXP_OutputPixelFormatY4        = 0x15, /*!< 4-bit monochrome pixels (1-plane Y luma, 4 bit truncation) */
    kPXP_OutputPixelFormatYUV2P422  = 0x18, /*!< 16-bit pixels (2-plane UV interleaved bytes) */
    kPXP_OutputPixelFormatYUV2P420  = 0x19, /*!< 16-bit pixels (2-plane UV) */
    kPXP_OutputPixelFormatYVU2P422  = 0x1A, /*!< 16-bit pixels (2-plane VU interleaved bytes) */
    kPXP_OutputPixelFormatYVU2P420  = 0x1B, /*!< 16-bit pixels (2-plane VU) */
} pxp_output_pixel_format_t;

/*! @brief PXP output buffer configuration. */
typedef struct _pxp_output_buffer_config
{
    pxp_output_pixel_format_t pixelFormat;       /*!< Output buffer pixel format. */
    pxp_interlaced_output_mode_t interlacedMode; /*!< Interlaced output mode. */
    uint32_t buffer0Addr;                        /*!< Output buffer 0 address. */
    uint32_t buffer1Addr;                        /*!< Output buffer 1 address, used for UV data in YUV 2-plane mode, or
                                                      field 1 in output interlaced mode. */
    uint16_t pitchBytes;                         /*!< Number of bytes between two vertically adjacent pixels. */
    uint16_t width;                              /*!< Pixels per line. */
    uint16_t height;                             /*!< How many lines in output buffer. */
} pxp_output_buffer_config_t;

/*! @brief PXP process surface buffer pixel format. */
typedef enum _pxp_ps_pixel_format
{
    kPXP_PsPixelFormatRGB888    = 0x4,  /*!< 32-bit pixels without alpha (unpacked 24-bit format) */
    kPXP_PsPixelFormatRGB555    = 0xC,  /*!< 16-bit pixels without alpha. */
    kPXP_PsPixelFormatRGB444    = 0xD,  /*!< 16-bit pixels without alpha. */
    kPXP_PsPixelFormatRGB565    = 0xE,  /*!< 16-bit pixels without alpha. */
    kPXP_PsPixelFormatYUV1P444  = 0x10, /*!< 32-bit pixels (1-plane XYUV unpacked). */
    kPXP_PsPixelFormatUYVY1P422 = 0x12, /*!< 16-bit pixels (1-plane U0,Y0,V0,Y1 interleaved bytes) */
    kPXP_PsPixelFormatVYUY1P422 = 0x13, /*!< 16-bit pixels (1-plane V0,Y0,U0,Y1 interleaved bytes) */
    kPXP_PsPixelFormatY8        = 0x14, /*!< 8-bit monochrome pixels (1-plane Y luma output) */
    kPXP_PsPixelFormatY4        = 0x15, /*!< 4-bit monochrome pixels (1-plane Y luma, 4 bit truncation) */
    kPXP_PsPixelFormatYUV2P422  = 0x18, /*!< 16-bit pixels (2-plane UV interleaved bytes) */
    kPXP_PsPixelFormatYUV2P420  = 0x19, /*!< 16-bit pixels (2-plane UV) */
    kPXP_PsPixelFormatYVU2P422  = 0x1A, /*!< 16-bit pixels (2-plane VU interleaved bytes) */
    kPXP_PsPixelFormatYVU2P420  = 0x1B, /*!< 16-bit pixels (2-plane VU) */
    kPXP_PsPixelFormatYVU422    = 0x1E, /*!< 16-bit pixels (3-plane) */
    kPXP_PsPixelFormatYVU420    = 0x1F, /*!< 16-bit pixels (3-plane) */
} pxp_ps_pixel_format_t;

/*! @brief PXP process surface buffer configuration. */
typedef struct _pxp_ps_buffer_config
{
    pxp_ps_pixel_format_t pixelFormat; /*!< PS buffer pixel format. */
    bool swapByte;                     /*!< For each 16 bit word, set true to swap the two bytes. */
    uint32_t bufferAddr;               /*!< Input buffer address for the first panel. */
    uint32_t bufferAddrU;              /*!< Input buffer address for the second panel. */
    uint32_t bufferAddrV;              /*!< Input buffer address for the third panel. */
    uint16_t pitchBytes;               /*!< Number of bytes between two vertically adjacent pixels. */
} pxp_ps_buffer_config_t;

/*! @brief PXP alpha surface buffer pixel format. */
typedef enum _pxp_as_pixel_format
{
    kPXP_AsPixelFormatARGB8888 = 0x0, /*!< 32-bit pixels with alpha. */
    kPXP_AsPixelFormatRGB888   = 0x4, /*!< 32-bit pixels without alpha (unpacked 24-bit format) */
    kPXP_AsPixelFormatARGB1555 = 0x8, /*!< 16-bit pixels with alpha. */
    kPXP_AsPixelFormatARGB4444 = 0x9, /*!< 16-bit pixels with alpha. */
    kPXP_AsPixelFormatRGB555   = 0xC, /*!< 16-bit pixels without alpha. */
    kPXP_AsPixelFormatRGB444   = 0xD, /*!< 16-bit pixels without alpha. */
    kPXP_AsPixelFormatRGB565   = 0xE, /*!< 16-bit pixels without alpha. */
} pxp_as_pixel_format_t;

/*! @brief PXP alphs surface buffer configuration. */
typedef struct _pxp_as_buffer_config
{
    pxp_as_pixel_format_t pixelFormat; /*!< AS buffer pixel format. */
    uint32_t bufferAddr;               /*!< Input buffer address. */
    uint16_t pitchBytes;               /*!< Number of bytes between two vertically adjacent pixels. */
} pxp_as_buffer_config_t;

/*!
 * @brief PXP alpha mode during blending.
 */
typedef enum _pxp_alpha_mode
{
    kPXP_AlphaEmbedded, /*!< The alpha surface pixel alpha value will be used for blend. */
    kPXP_AlphaOverride, /*!< The user defined alpha value will be used for blend directly. */
    kPXP_AlphaMultiply, /*!< The alpha surface pixel alpha value scaled the user defined
                             alpha value will be used for blend, for example, pixel alpha set
                             set to 200, user defined alpha set to 100, then the reault alpha
                             is 200 * 100 / 255. */
    kPXP_AlphaRop       /*!< Raster operation. */
} pxp_alpha_mode_t;

/*!
 * @brief PXP ROP mode during blending.
 *
 * Explanation:
 * - AS: Alpha surface
 * - PS: Process surface
 * - nAS: Alpha surface NOT value
 * - nPS: Process surface NOT value
 */
typedef enum _pxp_rop_mode
{
    kPXP_RopMaskAs     = 0x0, /*!< AS AND PS. */
    kPXP_RopMaskNotAs  = 0x1, /*!< nAS AND PS. */
    kPXP_RopMaskAsNot  = 0x2, /*!< AS AND nPS. */
    kPXP_RopMergeAs    = 0x3, /*!< AS OR PS. */
    kPXP_RopMergeNotAs = 0x4, /*!< nAS OR PS. */
    kPXP_RopMergeAsNot = 0x5, /*!< AS OR nPS. */
    kPXP_RopNotCopyAs  = 0x6, /*!< nAS. */
    kPXP_RopNot        = 0x7, /*!< nPS. */
    kPXP_RopNotMaskAs  = 0x8, /*!< AS NAND PS. */
    kPXP_RopNotMergeAs = 0x9, /*!< AS NOR PS. */
    kPXP_RopXorAs      = 0xA, /*!< AS XOR PS. */
    kPXP_RopNotXorAs   = 0xB  /*!< AS XNOR PS. */
} pxp_rop_mode_t;

/*!
 * @brief PXP alpha surface blending configuration.
 */
typedef struct _pxp_as_blend_config
{
    uint8_t alpha;    /*!< User defined alpha value, only used when @ref alphaMode is @ref kPXP_AlphaOverride or @ref
                         kPXP_AlphaRop. */
    bool invertAlpha; /*!< Set true to invert the alpha. */
    pxp_alpha_mode_t alphaMode; /*!< Alpha mode. */
    pxp_rop_mode_t ropMode;     /*!< ROP mode, only valid when @ref alphaMode is @ref kPXP_AlphaRop. */
} pxp_as_blend_config_t;

/*! @brief PXP process block size. */
typedef enum _pxp_block_size
{
    kPXP_BlockSize8 = 0U, /*!< Process 8x8 pixel blocks. */
    kPXP_BlockSize16,     /*!< Process 16x16 pixel blocks. */
} pxp_block_size_t;

/*! @brief PXP CSC1 mode. */
typedef enum _pxp_csc1_mode
{
    kPXP_Csc1YUV2RGB = 0U, /*!< YUV to RGB. */
    kPXP_Csc1YCbCr2RGB,    /*!< YCbCr to RGB. */
} pxp_csc1_mode_t;

/*! @brief PXP CSC2 mode. */
typedef enum _pxp_csc2_mode
{
    kPXP_Csc2YUV2RGB = 0U, /*!< YUV to RGB. */
    kPXP_Csc2YCbCr2RGB,    /*!< YCbCr to RGB. */
    kPXP_Csc2RGB2YUV,      /*!< RGB to YUV. */
    kPXP_Csc2RGB2YCbCr,    /*!< RGB to YCbCr. */
} pxp_csc2_mode_t;

/*!
 * @brief PXP CSC2 configuration.
 *
 * Converting from YUV/YCbCr color spaces to the RGB color space uses the
 * following equation structure:
 *
 * R = A1(Y+D1) + A2(U+D2) + A3(V+D3)
 * G = B1(Y+D1) + B2(U+D2) + B3(V+D3)
 * B = C1(Y+D1) + C2(U+D2) + C3(V+D3)
 *
 * Converting from the RGB color space to YUV/YCbCr color spaces uses the
 * following equation structure:
 *
 * Y = A1*R + A2*G + A3*B + D1
 * U = B1*R + B2*G + B3*B + D2
 * V = C1*R + C2*G + C3*B + D3
 */
typedef struct _pxp_csc2_config
{
    pxp_csc2_mode_t mode; /*!< Convertion mode. */
    float A1;             /*!< A1. */
    float A2;             /*!< A2. */
    float A3;             /*!< A3. */
    float B1;             /*!< B1. */
    float B2;             /*!< B2. */
    float B3;             /*!< B3. */
    float C1;             /*!< C1. */
    float C2;             /*!< C2. */
    float C3;             /*!< C3. */
    int16_t D1;           /*!< D1. */
    int16_t D2;           /*!< D2. */
    int16_t D3;           /*!< D3. */
} pxp_csc2_config_t;

#if !(defined(FSL_FEATURE_PXP_HAS_NO_LUT) && FSL_FEATURE_PXP_HAS_NO_LUT)
/*! @brief PXP LUT lookup mode. */
typedef enum _pxp_lut_lookup_mode
{
    kPXP_LutCacheRGB565 = 0U, /*!< LUT ADDR = R[7:3],G[7:2],B[7:3]. Use all 16KB of LUT
                                   for indirect cached 128KB lookup. */
    kPXP_LutDirectY8,         /*!< LUT ADDR = 16'b0,Y[7:0]. Use the first 256 bytes of LUT.
                                   Only third data path byte is tranformed. */
    kPXP_LutDirectRGB444,     /*!< LUT ADDR = R[7:4],G[7:4],B[7:4]. Use one 8KB bank of LUT
                                   selected by @ref PXP_Select8kLutBank. */
    kPXP_LutDirectRGB454,     /*!< LUT ADDR = R[7:4],G[7:3],B[7:4]. Use all 16KB of LUT. */
} pxp_lut_lookup_mode_t;

/*! @brief PXP LUT output mode. */
typedef enum _pxp_lut_out_mode
{
    kPXP_LutOutY8 = 1U,     /*!< R/Y byte lane 2 lookup, bytes 1,0 bypassed. */
    kPXP_LutOutRGBW4444CFA, /*!< Byte lane 2 = CFA_Y8, byte lane 1,0 = RGBW4444. */
    kPXP_LutOutRGB888,      /*!< RGB565->RGB888 conversion for Gamma correction. */
} pxp_lut_out_mode_t;

/*! @brief PXP LUT 8K bank index used when lookup mode is @ref kPXP_LutDirectRGB444. */
typedef enum _pxp_lut_8k_bank
{
    kPXP_Lut8kBank0 = 0U, /*!< The first 8K bank used. */
    kPXP_Lut8kBank1,      /*!< The second 8K bank used. */
} pxp_lut_8k_bank_t;

/*! @brief PXP LUT configuration. */
typedef struct _pxp_lut_config
{
    pxp_lut_lookup_mode_t lookupMode; /*!< Look up mode. */
    pxp_lut_out_mode_t outMode;       /*!< Out mode. */
    uint32_t cfaValue;                /*!< The CFA value used when look up mode is @ref kPXP_LutOutRGBW4444CFA. */
} pxp_lut_config_t;
#endif /* FSL_FEATURE_PXP_HAS_NO_LUT */

/*! @brief PXP internal memory. */
typedef enum _pxp_ram
{
    kPXP_RamDither0Lut = 0U, /*!< Dither 0 LUT memory. */
    kPXP_RamDither1Lut = 3U, /*!< Dither 1 LUT memory. */
    kPXP_RamDither2Lut = 4U, /*!< Dither 2 LUT memory. */
} pxp_ram_t;

/*! @brief PXP dither mode. */
enum _pxp_dither_mode
{
    kPXP_DitherPassThrough = 0U, /*!< Pass through, no dither. */
    kPXP_DitherOrdered     = 3U, /*!< Ordered dither. */
    kPXP_DitherQuantOnly   = 4U, /*!< No dithering, only quantization. */
};

/*! @brief PXP dither LUT mode. */
enum _pxp_dither_lut_mode
{
    kPXP_DitherLutOff = 0U,   /*!< The LUT memory is not used for LUT, could be used as ordered dither index matrix. */
    kPXP_DitherLutPreDither,  /*!< Use LUT at the pre-dither stage, The pre-dither LUT could only be used in Floyd mode
                                   or Atkinson mode, which are not supported by current PXP module. */
    kPXP_DitherLutPostDither, /*!< Use LUT at the post-dither stage. */
};

/*! @brief PXP dither matrix size. */
enum _pxp_dither_matrix_size
{
    kPXP_DitherMatrix8 = 1, /*!< The dither index matrix is 8x8. */
    kPXP_DitherMatrix16,    /*!< The dither index matrix is 16x16. */
};

/*! @brief PXP dither final LUT data. */
typedef struct _pxp_dither_final_lut_data
{
    uint32_t data_3_0;   /*!< Data 3 to data 0. Data 0 is the least significant byte. */
    uint32_t data_7_4;   /*!< Data 7 to data 4. Data 4 is the least significant byte. */
    uint32_t data_11_8;  /*!< Data 11 to data 8. Data 8 is the least significant byte. */
    uint32_t data_15_12; /*!< Data 15 to data 12. Data 12 is the least significant byte. */
} pxp_dither_final_lut_data_t;

/*! @brief PXP dither configuration. */
typedef struct _pxp_dither_config
{
    uint32_t enableDither0 : 1;  /*!< Enable dither engine 0 or not, set 1 to enable, 0 to disable. */
    uint32_t enableDither1 : 1;  /*!< Enable dither engine 1 or not, set 1 to enable, 0 to disable. */
    uint32_t enableDither2 : 1;  /*!< Enable dither engine 2 or not, set 1 to enable, 0 to disable. */
    uint32_t ditherMode0 : 3;    /*!< Dither mode for dither engine 0. See @ref _pxp_dither_mode. */
    uint32_t ditherMode1 : 3;    /*!< Dither mode for dither engine 1. See @ref _pxp_dither_mode. */
    uint32_t ditherMode2 : 3;    /*!< Dither mode for dither engine 2. See @ref _pxp_dither_mode. */
    uint32_t quantBitNum : 3;    /*!< Number of bits quantize down to, the valid value is 1~7. */
    uint32_t lutMode : 2;        /*!< How to use the memory LUT, see @ref _pxp_dither_lut_mode. This must be set to @ref
                                    kPXP_DitherLutOff
                                      if any dither engine uses @ref kPXP_DitherOrdered mode. */
    uint32_t idxMatrixSize0 : 2; /*!< Size of index matrix used for dither for dither engine 0, see @ref
                                    _pxp_dither_matrix_size. */
    uint32_t idxMatrixSize1 : 2; /*!< Size of index matrix used for dither for dither engine 1, see @ref
                                    _pxp_dither_matrix_size. */
    uint32_t idxMatrixSize2 : 2; /*!< Size of index matrix used for dither for dither engine 2, see @ref
                                    _pxp_dither_matrix_size. */
    uint32_t enableFinalLut : 1; /*!< Enable the final LUT, set 1 to enable, 0 to disable. */
    uint32_t : 8;
} pxp_dither_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief Initialize the PXP.
 *
 * This function enables the PXP peripheral clock, and resets the PXP registers
 * to default status.
 *
 * @param base PXP peripheral base address.
 */
void PXP_Init(PXP_Type *base);

/*!
 * @brief De-initialize the PXP.
 *
 * This function disables the PXP peripheral clock.
 *
 * @param base PXP peripheral base address.
 */
void PXP_Deinit(PXP_Type *base);

/*!
 * @brief Reset the PXP.
 *
 * This function resets the PXP peripheral registers to default status.
 *
 * @param base PXP peripheral base address.
 */
void PXP_Reset(PXP_Type *base);
/* @} */

/*!
 * @name Global operations
 * @{
 */

/*!
 * @brief Start process.
 *
 * Start PXP process using current configuration.
 *
 * @param base PXP peripheral base address.
 */
static inline void PXP_Start(PXP_Type *base)
{
    base->CTRL_SET = PXP_CTRL_ENABLE_MASK;
}

/*!
 * @brief Enable or disable LCD hand shake.
 *
 * @param base PXP peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void PXP_EnableLcdHandShake(PXP_Type *base, bool enable)
{
#if defined(PXP_CTRL_ENABLE_LCD_HANDSHAKE_MASK)
    if (enable)
    {
        base->CTRL_SET = PXP_CTRL_ENABLE_LCD_HANDSHAKE_MASK;
    }
    else
    {
        base->CTRL_CLR = PXP_CTRL_ENABLE_LCD_HANDSHAKE_MASK;
    }
#else
    if (enable)
    {
        base->CTRL_SET = PXP_CTRL_ENABLE_LCD0_HANDSHAKE_MASK;
    }
    else
    {
        base->CTRL_CLR = PXP_CTRL_ENABLE_LCD0_HANDSHAKE_MASK;
    }
#endif
}

#if (defined(FSL_FEATURE_PXP_HAS_EN_REPEAT) && FSL_FEATURE_PXP_HAS_EN_REPEAT)
/*!
 * @brief Enable or disable continous run.
 *
 * If continous run not enabled, @ref PXP_Start starts the PXP process. When completed,
 * PXP enters idle mode and flag @ref kPXP_CompleteFlag asserts.
 *
 * If continous run enabled, the PXP will repeat based on the current configuration register
 * settings.
 *
 * @param base PXP peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void PXP_EnableContinousRun(PXP_Type *base, bool enable)
{
    if (enable)
    {
        base->CTRL_SET = PXP_CTRL_EN_REPEAT_MASK;
    }
    else
    {
        base->CTRL_CLR = PXP_CTRL_EN_REPEAT_MASK;
    }
}
#endif /* FSL_FEATURE_PXP_HAS_EN_REPEAT */

/*!
 * @brief Set the PXP processing block size
 *
 * This function chooses the pixel block size that PXP using during process.
 * Larger block size means better performace, but be careful that when PXP is
 * rotating, the output must be divisible by the block size selected.
 *
 * @param base PXP peripheral base address.
 * @param size The pixel block size.
 */
static inline void PXP_SetProcessBlockSize(PXP_Type *base, pxp_block_size_t size)
{
    base->CTRL = (base->CTRL & ~PXP_CTRL_BLOCK_SIZE_MASK) | PXP_CTRL_BLOCK_SIZE(size);
}

/* @} */

/*!
 * @name Status
 * @{
 */

/*!
 * @brief Gets PXP status flags.
 *
 * This function gets all PXP status flags. The flags are returned as the logical
 * OR value of the enumerators @ref _pxp_flags. To check a specific status,
 * compare the return value with enumerators in @ref _pxp_flags.
 * For example, to check whether the PXP has completed process, use like this:
 * @code
       if (kPXP_CompleteFlag & PXP_GetStatusFlags(PXP))
       {
           ...
       }
   @endcode
 *
 * @param base PXP peripheral base address.
 * @return PXP status flags which are OR'ed by the enumerators in the _pxp_flags.
 */
static inline uint32_t PXP_GetStatusFlags(PXP_Type *base)
{
#if defined(PXP_STAT_AXI_READ_ERROR_1_MASK)
    return base->STAT &
           (PXP_STAT_NEXT_IRQ_MASK | PXP_STAT_IRQ0_MASK | PXP_STAT_AXI_READ_ERROR_0_MASK |
            PXP_STAT_AXI_WRITE_ERROR_0_MASK | PXP_STAT_AXI_READ_ERROR_1_MASK | PXP_STAT_AXI_WRITE_ERROR_1_MASK);
#else
    return base->STAT & (PXP_STAT_NEXT_IRQ_MASK | PXP_STAT_IRQ0_MASK | PXP_STAT_AXI_READ_ERROR_0_MASK |
                         PXP_STAT_AXI_WRITE_ERROR_0_MASK);
#endif
}

/*!
 * @brief Clears status flags with the provided mask.
 *
 * This function clears PXP status flags with a provided mask.
 *
 * @param base PXP peripheral base address.
 * @param statusMask The status flags to be cleared; it is logical OR value of @ref _pxp_flags.
 */
static inline void PXP_ClearStatusFlags(PXP_Type *base, uint32_t statusMask)
{
    base->STAT_CLR = statusMask;
}

/*!
 * @brief Gets the AXI ID of the failing bus operation.
 *
 * @param base PXP peripheral base address.
 * @param axiIndex Whitch AXI to get
 *        - 0: AXI0
 *        - 1: AXI1
 * @return The AXI ID of the failing bus operation.
 */
static inline uint8_t PXP_GetAxiErrorId(PXP_Type *base, uint8_t axiIndex)
{
#if defined(PXP_STAT_AXI_ERROR_ID_1_MASK)
    if (0 == axiIndex)
    {
        return (uint8_t)((base->STAT & PXP_STAT_AXI_ERROR_ID_0_MASK) >> PXP_STAT_AXI_ERROR_ID_0_SHIFT);
    }
    else
    {
        return (uint8_t)((base->STAT & PXP_STAT_AXI_ERROR_ID_1_MASK) >> PXP_STAT_AXI_ERROR_ID_1_SHIFT);
    }
#else
    return (uint8_t)((base->STAT & PXP_STAT_AXI_ERROR_ID_MASK) >> PXP_STAT_AXI_ERROR_ID_SHIFT);
#endif
}

/* @} */

/*!
 * @name Interrupts
 * @{
 */

/*!
 * @brief Enables PXP interrupts according to the provided mask.
 *
 * This function enables the PXP interrupts according to the provided mask. The mask
 * is a logical OR of enumeration members. See @ref _pxp_interrupt_enable.
 * For example, to enable PXP process complete interrupt and command loaded
 * interrupt, do the following.
 * @code
    PXP_EnableInterrupts(PXP, kPXP_CommandLoadInterruptEnable | kPXP_CompleteInterruptEnable);
   @endcode
 *
 * @param base PXP peripheral base address.
 * @param mask The interrupts to enable. Logical OR of @ref _pxp_interrupt_enable.
 */
static inline void PXP_EnableInterrupts(PXP_Type *base, uint32_t mask)
{
    base->CTRL_SET = mask;
}

/*!
 * @brief Disables PXP interrupts according to the provided mask.
 *
 * This function disables the PXP interrupts according to the provided mask. The mask
 * is a logical OR of enumeration members. See @ref _pxp_interrupt_enable.
 *
 * @param base PXP peripheral base address.
 * @param mask The interrupts to disable. Logical OR of @ref _pxp_interrupt_enable.
 */
static inline void PXP_DisableInterrupts(PXP_Type *base, uint32_t mask)
{
    base->CTRL_CLR = mask;
}

/* @} */

/*!
 * @name Alpha surface
 * @{
 */

/*!
 * @brief Set the alpha surface input buffer configuration.
 *
 * @param base PXP peripheral base address.
 * @param config Pointer to the configuration.
 */
void PXP_SetAlphaSurfaceBufferConfig(PXP_Type *base, const pxp_as_buffer_config_t *config);

/*!
 * @brief Set the alpha surface blending configuration.
 *
 * @param base PXP peripheral base address.
 * @param config Pointer to the configuration structure.
 */
void PXP_SetAlphaSurfaceBlendConfig(PXP_Type *base, const pxp_as_blend_config_t *config);

/*!
 * @brief Set the alpha surface overlay color key.
 *
 * If a pixel in the current overlay image with a color that falls in the range
 * from the @p colorKeyLow to @p colorKeyHigh range, it will use the process surface
 * pixel value for that location. If no PS image is present or if the PS image also
 * matches its colorkey range, the PS background color is used.
 *
 * @param base PXP peripheral base address.
 * @param colorKeyLow Color key low range.
 * @param colorKeyHigh Color key high range.
 *
 * @note Colorkey operations are higher priority than alpha or ROP operations
 */
void PXP_SetAlphaSurfaceOverlayColorKey(PXP_Type *base, uint32_t colorKeyLow, uint32_t colorKeyHigh);

/*!
 * @brief Enable or disable the alpha surface color key.
 *
 * @param base PXP peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void PXP_EnableAlphaSurfaceOverlayColorKey(PXP_Type *base, bool enable)
{
    if (enable)
    {
        base->AS_CTRL |= PXP_AS_CTRL_ENABLE_COLORKEY_MASK;
    }
    {
        base->AS_CTRL &= ~PXP_AS_CTRL_ENABLE_COLORKEY_MASK;
    }
}

/*!
 * @brief Set the alpha surface position in output buffer.
 *
 * @param base PXP peripheral base address.
 * @param upperLeftX X of the upper left corner.
 * @param upperLeftY Y of the upper left corner.
 * @param lowerRightX X of the lower right corner.
 * @param lowerRightY Y of the lower right corner.
 */
void PXP_SetAlphaSurfacePosition(
    PXP_Type *base, uint16_t upperLeftX, uint16_t upperLeftY, uint16_t lowerRightX, uint16_t lowerRightY);
/* @} */

/*!
 * @name Process surface
 * @{
 */

/*!
 * @brief Set the back ground color of PS.
 *
 * @param base PXP peripheral base address.
 * @param backGroundColor Pixel value of the background color.
 */
static inline void PXP_SetProcessSurfaceBackGroundColor(PXP_Type *base, uint32_t backGroundColor)
{
#if defined(PXP_PS_BACKGROUND_0_COLOR_MASK)
    base->PS_BACKGROUND_0 = backGroundColor;
#else
    base->PS_BACKGROUND = backGroundColor;
#endif
}

/*!
 * @brief Set the process surface input buffer configuration.
 *
 * @param base PXP peripheral base address.
 * @param config Pointer to the configuration.
 */
void PXP_SetProcessSurfaceBufferConfig(PXP_Type *base, const pxp_ps_buffer_config_t *config);

/*!
 * @brief Set the process surface scaler configuration.
 *
 * The valid down scale fact is 1/(2^12) ~ 16.
 *
 * @param base PXP peripheral base address.
 * @param inputWidth Input image width.
 * @param inputHeight Input image height.
 * @param outputWidth Output image width.
 * @param outputHeight Output image height.
 */
void PXP_SetProcessSurfaceScaler(
    PXP_Type *base, uint16_t inputWidth, uint16_t inputHeight, uint16_t outputWidth, uint16_t outputHeight);

/*!
 * @brief Set the process surface position in output buffer.
 *
 * @param base PXP peripheral base address.
 * @param upperLeftX X of the upper left corner.
 * @param upperLeftY Y of the upper left corner.
 * @param lowerRightX X of the lower right corner.
 * @param lowerRightY Y of the lower right corner.
 */
void PXP_SetProcessSurfacePosition(
    PXP_Type *base, uint16_t upperLeftX, uint16_t upperLeftY, uint16_t lowerRightX, uint16_t lowerRightY);

/*!
 * @brief Set the process surface color key.
 *
 * If the PS image matches colorkey range, the PS background color is output. Set
 * @p colorKeyLow to 0xFFFFFFFF and @p colorKeyHigh to 0 will disable the colorkeying.
 *
 * @param base PXP peripheral base address.
 * @param colorKeyLow Color key low range.
 * @param colorKeyHigh Color key high range.
 */
void PXP_SetProcessSurfaceColorKey(PXP_Type *base, uint32_t colorKeyLow, uint32_t colorKeyHigh);
/* @} */

/*!
 * @name Output buffer
 * @{
 */

/*!
 * @brief Set the PXP outpt buffer configuration.
 *
 * @param base PXP peripheral base address.
 * @param config Pointer to the configuration.
 */
void PXP_SetOutputBufferConfig(PXP_Type *base, const pxp_output_buffer_config_t *config);

/*!
 * @brief Set the global overwritten alpha value.
 *
 * If global overwritten alpha is enabled, the alpha component in output buffer pixels
 * will be overwritten, otherwise the computed alpha value is used.
 *
 * @param base PXP peripheral base address.
 * @param alpha The alpha value.
 */
static inline void PXP_SetOverwrittenAlphaValue(PXP_Type *base, uint8_t alpha)
{
    base->OUT_CTRL = (base->OUT_CTRL & ~PXP_OUT_CTRL_ALPHA_MASK) | PXP_OUT_CTRL_ALPHA(alpha);
}

/*!
 * @brief Enable or disable the global overwritten alpha value.
 *
 * If global overwritten alpha is enabled, the alpha component in output buffer pixels
 * will be overwritten, otherwise the computed alpha value is used.
 *
 * @param base PXP peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void PXP_EnableOverWrittenAlpha(PXP_Type *base, bool enable)
{
    if (enable)
    {
        base->OUT_CTRL_SET = PXP_OUT_CTRL_ALPHA_OUTPUT_MASK;
    }
    else
    {
        base->OUT_CTRL_CLR = PXP_OUT_CTRL_ALPHA_OUTPUT_MASK;
    }
}

/*!
 * @brief Set the rotation configuration.
 *
 * The PXP could rotate the process surface or the output buffer. There are
 * two PXP versions:
 *  - Version 1: Only has one rotate sub module, the output buffer and process
 *    surface share the same rotate sub module, which means the process surface
 *    and output buffer could not be rotate at the same time. When pass in
 *    @ref kPXP_RotateOutputBuffer, the process surface could not use the rotate,
 *    Also when pass in @ref kPXP_RotateProcessSurface, output buffer could not
 *    use the rotate.
 *  - Version 2: Has two seperate rotate sub modules, the output buffer and
 *    process surface could configure the rotation independently.
 *
 * Upper layer could use the macro PXP_SHARE_ROTATE to check which version is.
 * PXP_SHARE_ROTATE=1 means version 1.
 *
 * @param base PXP peripheral base address.
 * @param position Rotate process surface or output buffer.
 * @param degree Rotate degree.
 * @param flipMode Flip mode.
 *
 * @note This function is different depends on the macro PXP_SHARE_ROTATE.
 */
static inline void PXP_SetRotateConfig(PXP_Type *base,
                                       pxp_rotate_position_t position,
                                       pxp_rotate_degree_t degree,
                                       pxp_flip_mode_t flipMode)
{
#if PXP_SHARE_ROTATE
    base->CTRL =
        (base->CTRL & ~(PXP_CTRL_ROTATE_MASK | PXP_CTRL_ROT_POS_MASK | PXP_CTRL_VFLIP_MASK | PXP_CTRL_HFLIP_MASK)) |
        PXP_CTRL_ROTATE(degree) | PXP_CTRL_ROT_POS(position) | ((uint32_t)flipMode << PXP_CTRL_HFLIP_SHIFT);
#else
    uint32_t ctrl       = base->CTRL;

    if (kPXP_RotateOutputBuffer == position)
    {
        if ((degree != kPXP_Rotate0) || (flipMode != kPXP_FlipDisable))
        {
            base->DATA_PATH_CTRL0 =
                (base->DATA_PATH_CTRL0 & (~PXP_DATA_PATH_CTRL0_MUX12_SEL_MASK)) | PXP_DATA_PATH_CTRL0_MUX12_SEL(0);
        }
        else
        {
            base->DATA_PATH_CTRL0 =
                (base->DATA_PATH_CTRL0 & (~PXP_DATA_PATH_CTRL0_MUX12_SEL_MASK)) | PXP_DATA_PATH_CTRL0_MUX12_SEL(1);
        }
        ctrl &= ~(PXP_CTRL_HFLIP0_MASK | PXP_CTRL_VFLIP0_MASK | PXP_CTRL_ROTATE0_MASK);
        ctrl |= (PXP_CTRL_ROTATE0(degree) | ((uint32_t)flipMode << PXP_CTRL_HFLIP0_SHIFT));
    }
    else
    {
        if ((degree != kPXP_Rotate0) || (flipMode != kPXP_FlipDisable))
        {
            base->DATA_PATH_CTRL0 =
                (base->DATA_PATH_CTRL0 & (~PXP_DATA_PATH_CTRL0_MUX3_SEL_MASK)) | PXP_DATA_PATH_CTRL0_MUX3_SEL(1);
        }
        else
        {
            base->DATA_PATH_CTRL0 =
                (base->DATA_PATH_CTRL0 & (~PXP_DATA_PATH_CTRL0_MUX3_SEL_MASK)) | PXP_DATA_PATH_CTRL0_MUX3_SEL(0);
        }
        ctrl &= ~(PXP_CTRL_HFLIP1_MASK | PXP_CTRL_VFLIP1_MASK | PXP_CTRL_ROTATE1_MASK);
        ctrl |= (PXP_CTRL_ROTATE1(degree) | ((uint32_t)flipMode << PXP_CTRL_HFLIP1_SHIFT));
    }

    base->CTRL = ctrl;
#endif
}
/* @} */

/*!
 * @name Command queue
 * @{
 */

/*!
 * @brief Set the next command.
 *
 * The PXP supports a primitive ability to queue up one operation while the current
 * operation is running. Workflow:
 *
 * 1. Prepare the PXP register values except STAT, CSCCOEFn, NEXT in the memory
 * in the order they appear in the register map.
 * 2. Call this function sets the new operation to PXP.
 * 3. There are two methods to check whether the PXP has loaded the new operation.
 * The first method is using @ref PXP_IsNextCommandPending. If there is new operation
 * not loaded by the PXP, this function returns true. The second method is checking
 * the flag @ref kPXP_CommandLoadFlag, if command loaded, this flag asserts. User
 * could enable interrupt @ref kPXP_CommandLoadInterruptEnable to get the loaded
 * signal in interrupt way.
 * 4. When command loaded by PXP, a new command could be set using this function.
 *
 * @code
   uint32_t pxp_command1[48];
   uint32_t pxp_command2[48];

   // Prepare the register values.
   pxp_command1[0] = ...;
   pxp_command1[1] = ...;
   // ...
   pxp_command2[0] = ...;
   pxp_command2[1] = ...;
   // ...

   // Make sure no new command pending.
   while (PXP_IsNextCommandPending(PXP))
   {
   }

   // Set new operation.
   PXP_SetNextCommand(PXP, pxp_command1);

   // Wait for new command loaded. Here could check @ref kPXP_CommandLoadFlag too.
   while (PXP_IsNextCommandPending(PXP))
   {
   }

   PXP_SetNextCommand(PXP, pxp_command2);
   @endcode
 *
 * @param base PXP peripheral base address.
 * @param commandAddr Address of the new command.
 */
static inline void PXP_SetNextCommand(PXP_Type *base, void *commandAddr)
{
    /* Make sure commands have been saved to memory. */
    __DSB();

    base->NEXT = (uint32_t)commandAddr & PXP_NEXT_POINTER_MASK;
}

/*!
 * @brief Check whether the next command is pending.
 *
 * @param base UART peripheral base address.
 * @return True is pending, false is not.
 */
static inline bool PXP_IsNextCommandPending(PXP_Type *base)
{
    return (bool)(base->NEXT & PXP_NEXT_ENABLED_MASK);
}

/*!
 * @brief Cancel command set by @ref PXP_SetNextCommand
 *
 * @param base UART peripheral base address.
 */
static inline void PXP_CancelNextCommand(PXP_Type *base)
{
    /* Write PXP_NEXT_ENABLED_MASK to the register NEXT_CLR to canel the command. */
    *((volatile uint32_t *)(&(base->NEXT)) + 2U) = PXP_NEXT_ENABLED_MASK;
}

/* @} */

/*!
 * @name Color space conversion
 * @{
 */

#if !(defined(FSL_FEATURE_PXP_HAS_NO_CSC2) && FSL_FEATURE_PXP_HAS_NO_CSC2)
/*!
 * @brief Set the CSC2 configuration.
 *
 * The CSC2 module receives pixels in any color space and can convert the pixels
 * into any of RGB, YUV, or YCbCr color spaces. The output pixels are passed
 * onto the LUT and rotation engine for further processing
 *
 * @param base PXP peripheral base address.
 * @param config Pointer to the configuration.
 */
void PXP_SetCsc2Config(PXP_Type *base, const pxp_csc2_config_t *config);

/*!
 * @brief Enable or disable the CSC2.
 *
 * @param base PXP peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void PXP_EnableCsc2(PXP_Type *base, bool enable)
{
    if (enable)
    {
        base->CSC2_CTRL &= ~PXP_CSC2_CTRL_BYPASS_MASK;
    }
    else
    {
        base->CSC2_CTRL |= PXP_CSC2_CTRL_BYPASS_MASK;
    }
}
#endif /* FSL_FEATURE_PXP_HAS_NO_CSC2 */

/*!
 * @brief Set the CSC1 mode.
 *
 * The CSC1 module receives scaled YUV/YCbCr444 pixels from the scale engine and
 * converts the pixels to the RGB888 color space. It could only be used by process
 * surface.
 *
 * @param base PXP peripheral base address.
 * @param mode The conversion mode.
 */
void PXP_SetCsc1Mode(PXP_Type *base, pxp_csc1_mode_t mode);

/*!
 * @brief Enable or disable the CSC1.
 *
 * @param base PXP peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void PXP_EnableCsc1(PXP_Type *base, bool enable)
{
    if (enable)
    {
        base->CSC1_COEF0 &= ~PXP_CSC1_COEF0_BYPASS_MASK;
    }
    else
    {
        base->CSC1_COEF0 |= PXP_CSC1_COEF0_BYPASS_MASK;
    }
}
/* @} */

#if !(defined(FSL_FEATURE_PXP_HAS_NO_LUT) && FSL_FEATURE_PXP_HAS_NO_LUT)
/*!
 * @name LUT operations
 * @{
 */

/*!
 * @brief Set the LUT configuration.
 *
 * The lookup table (LUT) is used to modify pixels in a manner that is not linear
 * and that cannot be achieved by the color space conversion modules. To setup
 * the LUT, the complete workflow is:
 * 1. Use @ref PXP_SetLutConfig to set the configuration, such as the lookup mode.
 * 2. Use @ref PXP_LoadLutTable to load the lookup table to PXP.
 * 3. Use @ref PXP_EnableLut to enable the function.
 *
 * @param base PXP peripheral base address.
 * @param config Pointer to the configuration.
 */
void PXP_SetLutConfig(PXP_Type *base, const pxp_lut_config_t *config);

/*!
 * @brief Set the look up table to PXP.
 *
 * If lookup mode is DIRECT mode, this function loads @p bytesNum of values
 * from the address @p memAddr into PXP LUT address @p lutStartAddr. So this
 * function allows only update part of the PXP LUT.
 *
 * If lookup mode is CACHE mode, this function sets the new address to @p memAddr
 * and invalid the PXP LUT cache.
 *
 * @param base PXP peripheral base address.
 * @param lookupMode Which lookup mode is used. Note that this parameter is only
 * used to distinguish DIRECT mode and CACHE mode, it does not change the register
 * value PXP_LUT_CTRL[LOOKUP_MODE]. To change that value, use function @ref PXP_SetLutConfig.
 * @param bytesNum How many bytes to set. This value must be divisable by 8.
 * @param memAddr Address of look up table to set.
 * @param lutStartAddr The LUT value will be loaded to LUT from index lutAddr. It should
 * be 8 bytes aligned.
 *
 * @retval kStatus_Success Load successfully.
 * @retval kStatus_InvalidArgument Failed because of invalid argument.
 */
status_t PXP_LoadLutTable(
    PXP_Type *base, pxp_lut_lookup_mode_t lookupMode, uint32_t bytesNum, uint32_t memAddr, uint16_t lutStartAddr);

/*!
 * @brief Enable or disable the LUT.
 *
 * @param base PXP peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void PXP_EnableLut(PXP_Type *base, bool enable)
{
    if (enable)
    {
        base->LUT_CTRL &= ~PXP_LUT_CTRL_BYPASS_MASK;
    }
    else
    {
        base->LUT_CTRL |= PXP_LUT_CTRL_BYPASS_MASK;
    }
}

/*!
 * @brief Select the 8kB LUT bank in DIRECT_RGB444 mode.
 *
 * @param base PXP peripheral base address.
 * @param bank The bank to select.
 */
static inline void PXP_Select8kLutBank(PXP_Type *base, pxp_lut_8k_bank_t bank)
{
    base->LUT_CTRL = (base->LUT_CTRL & ~PXP_LUT_CTRL_SEL_8KB_MASK) | PXP_LUT_CTRL_SEL_8KB(bank);
}
/* @} */
#endif /* FSL_FEATURE_PXP_HAS_NO_LUT */

#if (defined(FSL_FEATURE_PXP_HAS_DITHER) && FSL_FEATURE_PXP_HAS_DITHER)
/*!
 * @name Dither
 * @{
 */

/*!
 * @brief Write data to the PXP internal memory.
 *
 * @param base PXP peripheral base address.
 * @param ram Which internal memory to write.
 * @param bytesNum How many bytes to write.
 * @param data Pointer to the data to write.
 * @param memStartAddr The start address in the internal memory to write the data.
 */
void PXP_SetInternalRamData(PXP_Type *base, pxp_ram_t ram, uint32_t bytesNum, uint8_t *data, uint16_t memStartAddr);

/*!
 * @brief Set the dither final LUT data.
 *
 * The dither final LUT is only applicble to dither engine 0. It takes the bits[7:4]
 * of the output pixel and looks up and 8 bit value from the 16 value LUT to generate
 * the final output pixel to the next process module.
 *
 * @param base PXP peripheral base address.
 * @param data Pointer to the LUT data to set.
 */
void PXP_SetDitherFinalLutData(PXP_Type *base, const pxp_dither_final_lut_data_t *data);

/*!
 * @brief Set the configuration for the dither block.
 *
 * If the pre-dither LUT, post-dither LUT or ordered dither is used, please call
 * @ref PXP_SetInternalRamData to set the LUT data to internal memory.
 *
 * If the final LUT is used, please call @ref PXP_SetDitherFinalLutData to set
 * the LUT data.
 *
 * @param base PXP peripheral base address.
 * @param config Pointer to the configuration.
 *
 * @note When using ordered dithering, please set the PXP process block size same
 * with the ordered dithering matrix size using function @ref PXP_SetProcessBlockSize.
 */
static inline void PXP_SetDitherConfig(PXP_Type *base, const pxp_dither_config_t *config)
{
    base->DITHER_CTRL = *((const uint32_t *)config) & 0x00FFFFFFU;
}

/*!
 * @brief Enable or disable dither engine in the PXP process path.
 *
 * After the initialize function @ref PXP_Init, the dither engine is disabled and not
 * use in the PXP processing path. This function enables the dither engine and
 * routes the dither engine output to the output buffer. When the dither engine
 * is enabled using this function, @ref PXP_SetDitherConfig must be called to
 * configure dither engine correctly, otherwise there is not output to the output
 * buffer.
 *
 * @param base PXP peripheral base address.
 * @param enable Pass in true to enable, false to disable.
 */
void PXP_EnableDither(PXP_Type *base, bool enable);

/* @} */

#endif /* FSL_FEATURE_PXP_HAS_DITHER */

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _FSL_PXP_H_ */
