/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Vekatech Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/mpconfig.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/stream.h"
#include "extmod/machine_i2c.h"
#include "extmod/vfs.h"
#include "modmachine.h"
#include "hal_data.h"



#if MODULE_CAM_ENABLED

// White Balance
#define WB_NONE     0
#define WB_SUNNY    1
#define WB_CLOUDY   2
#define WB_OFFICE   3
#define WB_HOME     4

// Special Effect
#define EFFECT_NONE    0
#define EFFECT_NEG     1
#define EFFECT_BW      2
#define EFFECT_RED     3
#define EFFECT_GREEN   4
#define EFFECT_BLUE    5
#define EFFECT_RETRO   6

#define CAM     OV7725

#define COM3_V_FLIP   (1 << 7)
#define COM3_H_MIRROR (1 << 6)

#define COM7_RESET  (1 << 7)
#define COM7_QVGA   (1 << 6)
#define COM7_RGB565 (3 << 1)
#define ETBL          0xFFu

#define __3(a, b, c) a##b##c
#define _3(a, b, c) __3(a, b, c)

#define CCD(item) _3(CAM, _, item)

enum CCD (REGs)
{
    CCD(GAIN) = 0x00,
    CCD(BLUE),             // 0x01
    CCD(RED),              // 0x02
    CCD(GREEN),            // 0x03
    CCD(BAVG) = 0x05,
    CCD(GAVG),             // 0x06
    CCD(RAVG),             // 0x07
    CCD(AECH),             // 0x08
    CCD(COM2),             // 0x09
    CCD(PID),              // 0x0A
    CCD(VER),              // 0x0B
    CCD(COM3),             // 0x0C
    CCD(COM4),             // 0x0D
    CCD(COM5),             // 0x0E
    CCD(COM6),             // 0x0F
    CCD(AEC),              // 0x10
    CCD(CLKRC),            // 0x11
    CCD(COM7),             // 0x12
    CCD(COM8),             // 0x13
    CCD(COM9),             // 0x14
    CCD(COM10),            // 0x15
    CCD(REG16),            // 0x16
    CCD(HSTART),           // 0x17
    CCD(HSIZE),            // 0x18
    CCD(VSTRT),            // 0x19
    CCD(VSIZE),            // 0x1A
    CCD(PSHFT),            // 0x1B
    CCD(MIDH),             // 0x1C
    CCD(MIDL),             // 0x1D
    CCD(LAEC) = 0x1F,
    CCD(COM11),            // 0x20
    CCD(BDBase) = 0x22,
    CCD(BDMStep),          // 0x23
    CCD(AEW),              // 0x24
    CCD(AEB),              // 0x25
    CCD(VPT),              // 0x26
    CCD(REG28) = 0x28,
    CCD(HOutSize),         // 0x29
    CCD(EXHCH),            // 0x2A
    CCD(EXHCL),            // 0x2B
    CCD(VOutSize),         // 0x2C
    CCD(ADVFL),            // 0x2D
    CCD(ADVFH),            // 0x2E
    CCD(YAVE),             // 0x2F
    CCD(LumHTh),           // 0x30
    CCD(LumLTh),           // 0x31
    CCD(HREF),             // 0x32
    CCD(DM_LNL),           // 0x33
    CCD(DM_LNH),           // 0x34  Dummy Row High 8 Bits
    CCD(ADoff_B),          // 0x35  AD Offset Compensation Value for B Channel
    CCD(ADoff_R),          // 0x36  AD Offset Compensation Value for R Channel
    CCD(ADoff_Gb),         // 0x37  AD Offset Compensation Value for Gb Channel
    CCD(ADoff_Gr),         // 0x38  AD Offset Compensation Value for Gr Channel
    CCD(Off_B),            // 0x39  Analog Process B Channel Offset Compensation Value
    CCD(Off_R),            // 0x3A  Analog Process R Channel Offset Compensation Value
    CCD(Off_Gb),           // 0x3B  Analog Process Gb Channel Offset Compensation Value
    CCD(Off_Gr),           // 0x3C  Analog Process Gr Channel Offset Compensation Valu
    CCD(COM12),            // 0x3D
    CCD(TGT_B) = 0x42,
    CCD(FixGain) = 0x4D,
    CCD(UFix) = 0x60,              // U Channel Fixed Value Output
    CCD(VFix),             // 0x61  //V Channel Fixed Value Outpu
    CCD(AWBb_blk),         // 0x62  //AWB Option for Advanced AWB
    CCD(AWB_Ctrl0),        // 0x63
    CCD(DSP_Ctrl1),        // 0x64
    CCD(DSP_Ctrl2),        // 0x65
    CCD(DSP_Ctrl3),        // 0x66
    CCD(DSP_Ctrl4),        // 0x67
    CCD(AWBCtrl3) = 0x6B,
    CCD(GAM1) = 0x7E,
    CCD(GAM2),             // 0x7F
    CCD(GAM3),             // 0x80
    CCD(GAM4),             // 0x81
    CCD(GAM5),             // 0x82
    CCD(GAM6),             // 0x83
    CCD(GAM7),             // 0x84
    CCD(GAM8),             // 0x85
    CCD(GAM9),             // 0x86
    CCD(GAM10),            // 0x87
    CCD(GAM11),            // 0x88
    CCD(GAM12),            // 0x89
    CCD(GAM13),            // 0x8A
    CCD(GAM14),            // 0x8B
    CCD(GAM15),            // 0x8C
    CCD(SLOP),             // 0x8D
    CCD(EDGE1) = 0x90,
    CCD(DNSOff),           // 0x91
    CCD(EDGE2),            // 0x92
    CCD(EDGE3),            // 0x93
    CCD(MTX1),             // 0x94
    CCD(MTX2),             // 0x95
    CCD(MTX3),             // 0x96
    CCD(MTX4),             // 0x97
    CCD(MTX5),             // 0x98
    CCD(MTX6),             // 0x99
    CCD(MTX_Ctrl),         // 0x9A
    CCD(BRIGHT),           // 0x9B
    CCD(CNST),             // 0x9C
    CCD(UVADJ0) = 0x9E,
    CCD(SDE) = 0xA6,
    CCD(USAT) = 0xA7,
    CCD(VSAT) = 0xA8,
    CCD(SIGN) = 0xAB
};

// Video buffer components
typedef union
{
    struct
    {
        uint16_t B : 5;
        uint16_t G : 6;
        uint16_t R : 5;
    } CLR;
    uint8_t dat[2];
    uint16_t data;
} RGB_pixel_t;

typedef union
{
    struct
    {
        uint8_t Y0; // Y
        uint8_t U;  // Cb
        uint8_t Y1; // Y
        uint8_t V;  // Cr
    } CMP;
    uint8_t dat[4];
    uint16_t luma[2];
    uint32_t chroma;
    uint32_t data;
} YUY2_macropixel_t;

#define VIDEO_FRAME(name, w, h, p)              \
    union                                           \
    {                                               \
        uint8_t frameBUF[w * h * p];        \
        RGB_pixel_t RGB[w * h];               \
        RGB_pixel_t RGB_2D[h][w];           \
        YUY2_macropixel_t YUY2[(w * h) / 2];          \
        YUY2_macropixel_t YUY2_2D[h][w / 2];        \
        uint16_t lumaPLANE[w * h];         \
        uint16_t lumaPLANE_2D[h][w];     \
        uint32_t chromaPLANE[(w * h) / 2];   \
        uint32_t chromaPLANE_2D[h][w / 2]; \
    } name

typedef VIDEO_FRAME(VideoBuffer_t, 320, 240, 2);
typedef VIDEO_FRAME(DisplayBuffer_t, 480, 272, 2);

const uint8_t CCD(InitRegTable)[][2] = {
    {CCD(COM2), 0x02},
    {CCD(COM4), 0x41},
    {CCD(COM6), 0xC5},
    #if 1 /* 30fps(24MHz) */
    {CCD(CLKRC), 0x04},
    #else /* 60fps(48MHz) */
    {CCD(CLKRC), 0x00},
    #endif
    {CCD(COM7), COM7_QVGA},
    {CCD(COM9), 0x1F},
    {CCD(COM10), 0x40},

    // {CCD(HSTART)   , 0x23}, //0x22 23  VGA
    // {CCD(HSIZE)    , 0xA0}, //0xA4 A0  VGA
    // {CCD(VSTRT)    , 0x07}, //         VGA
    // {CCD(VSIZE)    , 0xF0}, //         VGA

    {CCD(HSTART), 0x3F},    // 3E 3F    QVGA
    {CCD(HSIZE), 0x54},     // 54 50    QVGA
    {CCD(VSTRT), 0x03},     //         QVGA
    {CCD(VSIZE), 0x78},     //         QVGA

    {CCD(BDBase), 0x99},
    {CCD(BDMStep), 0x02},
    {CCD(AEW), 0x60},
    {CCD(AEB), 0x50},
    {CCD(VPT), 0xA1},

    // {CCD(HOutSize) , 0xA0}, //         VGA
    {CCD(HOutSize), 0x50},  //         QVGA

    {CCD(EXHCH), 0x00},
    {CCD(EXHCL), 0x00},

    // {CCD(VOutSize) , 0xF0}, //         VGA
    {CCD(VOutSize), 0x78},      // 0x3C     QVGA

    {CCD(HREF), 0x00},
    {CCD(DM_LNL), 0x01},

    // {ADoff_Gb , 0x00},
    // {ADoff_Gr , 0x00},
    // {Off_Gb   , 0x00},
    // {Off_Gr   , 0x00},

    {CCD(COM12), 0x03},
    {CCD(TGT_B), 0x7F},
    {CCD(FixGain), 0x09},

    // {UFix     , 0x00}, //U Channel Fixed Value Output
    // {VFix     , 0x00}, //0x61  //V Channel Fixed Value Outpu

    {CCD(AWB_Ctrl0), 0xE0},
    {CCD(DSP_Ctrl1), 0xFF},
    {CCD(DSP_Ctrl2), 0x20},
    {CCD(DSP_Ctrl3), 0x00},
    {CCD(DSP_Ctrl4), 0x48},
    {CCD(AWBCtrl3), 0xAA},
    {CCD(GAM1), 0x04},
    {CCD(GAM2), 0x0E},
    {CCD(GAM3), 0x20},
    {CCD(GAM4), 0x43},
    {CCD(GAM5), 0x53},
    {CCD(GAM6), 0x61},
    {CCD(GAM7), 0x6D},
    {CCD(GAM8), 0x76},
    {CCD(GAM9), 0x7E},
    {CCD(GAM10), 0x86},
    {CCD(GAM11), 0x94},
    {CCD(GAM12), 0xA1},
    {CCD(GAM13), 0xBA},
    {CCD(GAM14), 0xCF},
    {CCD(GAM15), 0xE3},
    {CCD(SLOP), 0x26},
    {CCD(EDGE1), 0x05},
    {CCD(DNSOff), 0x01},
    {CCD(EDGE2), 0x05},
    {CCD(EDGE3), 0x00},
    {CCD(MTX1), 0x80},
    {CCD(MTX2), 0x7B},
    {CCD(MTX3), 0x06},
    {CCD(MTX4), 0x1E},
    {CCD(MTX5), 0x69},
    {CCD(MTX6), 0x86},
    {CCD(MTX_Ctrl), 0x1E},
    {CCD(BRIGHT), 0x00},
    {CCD(CNST), 0x20},
    {CCD(UVADJ0), 0x81},
    {CCD(SDE), 0x04},
    // {SIGN     , 0x07},
    {ETBL, ETBL}
};

typedef struct _machine_cam_obj_t {
    mp_obj_base_t base;
    mp_buffer_info_t fb;
    mp_obj_base_t *i2c;    // machine_i2c_obj_t* i2c;
    uint32_t jpeg_size;
    uint8_t isinited;
    uint8_t isshowing;
    mp_hal_pin_obj_t pwr;  // backlight ctr (ON OFF)
    mp_hal_pin_obj_t rst;  // enable ctr (ON OFF)
} machine_cam_obj_t;

volatile uint8_t CAM_rdy = 0;
static volatile uint8_t CAM_capture = 0;
static volatile uint8_t JPEG_complete = 0;

uint8_t g_JPEG[64 * 1024] BSP_ALIGN_VARIABLE(8) BSP_PLACE_IN_SECTION(".sram");

STATIC machine_cam_obj_t machine_cam_obj = {{&machine_cam_type}, {NULL, 0, 0}, MP_OBJ_NULL, 0, 0, 0, pin_P611, pin_P612};

void SWP(void) {
    VideoBuffer_t *Vid = (VideoBuffer_t *)g_cam_buffer[0];

    for (int i = 0, UV = (g_cam0_cfg.x_capture_pixels * g_cam0_cfg.y_capture_pixels) / 2; i < UV; i++)
    {
        Vid->YUY2[i].CMP.U -= 128;
        Vid->YUY2[i].CMP.V -= 128;
    }
}

void SnapShot() {
    VideoBuffer_t *Vid = (VideoBuffer_t *)g_cam_buffer[0];
    DisplayBuffer_t *Dis = (DisplayBuffer_t *)machine_cam_obj.fb.buf;

    for (int y = 0; y < g_cam0_cfg.y_capture_pixels; y++) {
        for (int x = 0; x < g_cam0_cfg.x_capture_pixels; x++)
        {
            // Dis->RGB_2D[y][x] = Vid->RGB_2D[y][x];
            Dis->RGB_2D[y][x].dat[0] = (Vid->RGB_2D[y][x].dat[0] >> 3);
            Dis->RGB_2D[y][x].dat[1] = (Vid->RGB_2D[y][x].dat[1] << 3);
            Dis->RGB_2D[y][x].dat[0] |= (Vid->RGB_2D[y][x].dat[1] & 0xE0);
            Dis->RGB_2D[y][x].dat[1] |= (Vid->RGB_2D[y][x].dat[0] & 0x07);
        }
    }
}

void next_img() {
    CAM_rdy = 0;
    if (machine_cam_obj.isshowing) {
        R_PDC_CaptureStart(&g_cam0_ctrl, g_cam_buffer[0]);
    }
}

void cam_ISR(pdc_callback_args_t *p_args) {
    if (p_args->event == PDC_EVENT_TRANSFER_COMPLETE) {
        if (machine_cam_obj.isshowing) {
            CAM_rdy = 1;
        } else {
            CAM_capture = 1;
        }
    }
}

void jpg_ISR(jpeg_callback_args_t *p_args) {
    if (NULL != p_args) {
        if (p_args->status == (JPEG_STATUS_IDLE | JPEG_STATUS_OPERATION_COMPLETE)) {
            machine_cam_obj.jpeg_size = p_args->image_size;
            JPEG_complete = 1;
        }
    }
}

STATIC mp_obj_t machine_cam_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    machine_cam_obj_t *self = &machine_cam_obj;

    enum { ARG_i2c };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c,  MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} }
    };

    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_arg_val_t init_args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, args, MP_ARRAY_SIZE(allowed_args), allowed_args, init_args);

    self->i2c = MP_OBJ_TO_PTR(init_args[ARG_i2c].u_obj);
    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t cam_init(mp_obj_t self_in) {
    machine_cam_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->i2c->type, protocol);

    if (!self->isinited) {
        if (FSP_SUCCESS == R_PDC_Open(&g_cam0_ctrl, &g_cam0_cfg)) {
            // int e;
            uint8_t regs[2] = {CCD(COM7), COM7_RESET};
            mp_machine_i2c_buf_t buff = {sizeof(regs), regs};

            self->isinited = 1;
            ra_gpio_config(self->pwr->pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_LOW_POWER, 0);
            ra_gpio_config(self->rst->pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_LOW_POWER, 0);
            /* De-assert Camera Power Down */
            ra_gpio_write(self->pwr->pin, 0);
            ra_gpio_write(self->rst->pin, 0);

            mp_hal_delay_ms(2);
            ra_gpio_write(self->rst->pin, 1);

            mp_hal_delay_ms(2);
            if (0 > i2c_p->transfer(self->i2c, 0x21, 1, &buff, MP_MACHINE_I2C_FLAG_STOP)) {
                mp_raise_ValueError(MP_ERROR_TEXT("I2C transfer error"));
            }
            mp_hal_delay_ms(2);

            buff.buf = (uint8_t *)CCD(InitRegTable)[0];

            do
            {
                if (0 > i2c_p->transfer(self->i2c, 0x21, 1, &buff, MP_MACHINE_I2C_FLAG_STOP)) {
                    mp_raise_ValueError(MP_ERROR_TEXT("I2C transfer error"));
                    break;
                }
                buff.buf += buff.len;
            } while (buff.buf[0] != ETBL);

            mp_hal_delay_ms(2);

            if (FSP_SUCCESS == R_JPEG_Open(&g_jpeg0_ctrl, &g_jpeg0_cfg)) {
                R_JPEG_OutputBufferSet(&g_jpeg0_ctrl, g_JPEG, sizeof(g_JPEG));
            } else {
                mp_raise_ValueError(MP_ERROR_TEXT("Can't init R_JPEG"));
            }
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("Can't init R_PDC"));
        }
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Olready inited!"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_cam_init_obj, cam_init);

STATIC mp_obj_t cam_deinit(mp_obj_t self_in) {
    machine_cam_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->isinited) {
        if (FSP_SUCCESS == R_PDC_Close(&g_cam0_ctrl)) {
            self->isinited = 0;
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("Can't close R_PDC"));
        }
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Not inited! call CAM.init() first"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_cam_deinit_obj, cam_deinit);

STATIC mp_obj_t cam_YUYV(mp_obj_t self_in, mp_obj_t file_name) {
    machine_cam_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->isinited) {
        if (!self->isshowing) {
            CAM_capture = 0;
            if (FSP_SUCCESS == R_PDC_CaptureStart(&g_cam0_ctrl, g_cam_buffer[0])) {
                // save in flash
                mp_obj_t file = MP_OBJ_NULL;
                mp_obj_t args[2] = {MP_OBJ_NEW_QSTR(MP_QSTR__slash_flash), MP_OBJ_NULL};
                mp_obj_list_t *list = mp_vfs_listdir(1, args);
                mp_uint_t len = 0;

                mp_obj_list_append(list, file_name);
                args[0] = file_name;
                args[1] = MP_OBJ_NEW_QSTR(MP_QSTR_wb);
                file = mp_vfs_open(MP_ARRAY_SIZE(args), &args[0], (mp_map_t *)&mp_const_empty_map);

                if (file == MP_OBJ_NULL) {
                    mp_raise_ValueError(MP_ERROR_TEXT("Can't create file"));
                } else {
                    int errcode;

                    while (!CAM_capture) {
                        ;
                    }

                    len = mp_stream_rw(file, g_cam_buffer[0], sizeof(g_cam_buffer[0]), &errcode, MP_STREAM_RW_WRITE | MP_STREAM_RW_ONCE);
                    if (errcode != 0) {
                        mp_raise_OSError(errcode);
                    }

                    if (len != sizeof(g_cam_buffer[0])) {
                        mp_raise_ValueError(MP_ERROR_TEXT("Can't write YUY2 file"));
                    }

                    mp_stream_close(file);
                }
            } else {
                mp_raise_ValueError(MP_ERROR_TEXT("Can't Start Capture R_PDC"));
            }
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("Not in YUV2 mode. Stop showing first!"));
        }
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Not inited! call CAM.init() first"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_cam_yuyv_obj, cam_YUYV);

STATIC mp_obj_t cam_JPEG(mp_obj_t self_in, mp_obj_t file_name) {
    machine_cam_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->isinited) {
        if (!self->isshowing) {
            CAM_capture = 0;
            if (FSP_SUCCESS == R_PDC_CaptureStart(&g_cam0_ctrl, g_cam_buffer[0])) {
                // save in flash
                mp_obj_t file = MP_OBJ_NULL;
                mp_obj_t args[2] = {MP_OBJ_NEW_QSTR(MP_QSTR__slash_flash), MP_OBJ_NULL};
                mp_obj_list_t *list = mp_vfs_listdir(1, args);
                mp_uint_t len = 0;

                mp_obj_list_append(list, file_name);
                args[0] = file_name;
                args[1] = MP_OBJ_NEW_QSTR(MP_QSTR_wb);
                file = mp_vfs_open(MP_ARRAY_SIZE(args), &args[0], (mp_map_t *)&mp_const_empty_map);

                if (file == MP_OBJ_NULL) {
                    mp_raise_ValueError(MP_ERROR_TEXT("Can't create file"));
                } else {
                    int errcode;

                    while (!CAM_capture) {
                        ;
                    }
                    SWP();
                    JPEG_complete = 0;
                    R_JPEG_InputBufferSet(&g_jpeg0_ctrl, g_cam_buffer[0], sizeof(g_cam_buffer[0]) * 2);
                    while (!JPEG_complete) {
                        ;
                    }

                    len = mp_stream_rw(file, g_JPEG, self->jpeg_size, &errcode, MP_STREAM_RW_WRITE | MP_STREAM_RW_ONCE);
                    if (errcode != 0) {
                        mp_raise_OSError(errcode);
                    }

                    if (len != self->jpeg_size) {
                        mp_raise_ValueError(MP_ERROR_TEXT("Can't write JPEG file"));
                    }

                    mp_stream_close(file);
                }
            } else {
                mp_raise_ValueError(MP_ERROR_TEXT("Can't Start Capture R_PDC"));
            }
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("Not in YUV2 mode. Stop showing first!"));
        }
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Not inited! call CAM.init() first"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_cam_jpeg_obj, cam_JPEG);

STATIC mp_obj_t cam_show_start(mp_obj_t self_in, mp_obj_t lcd) {
    machine_cam_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->isinited) {
        uint8_t regs[2] = {CCD(COM7), 0};
        mp_machine_i2c_buf_t buff = {sizeof(regs), regs};
        mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->i2c->type, protocol);

        buff.len--;
        if (0 > i2c_p->transfer(self->i2c, 0x21, 1, &buff, MP_MACHINE_I2C_FLAG_STOP)) {
            mp_raise_ValueError(MP_ERROR_TEXT("I2C transfer error"));
        }
        buff.buf += buff.len;
        if (0 > i2c_p->transfer(self->i2c, 0x21, 1, &buff, MP_MACHINE_I2C_FLAG_READ | MP_MACHINE_I2C_FLAG_STOP)) {
            mp_raise_ValueError(MP_ERROR_TEXT("I2C transfer error"));
        }
        buff.buf -= buff.len;
        buff.len++;

        regs[1] &= 0xF0;
        regs[1] |= COM7_RGB565;

        if (0 > i2c_p->transfer(self->i2c, 0x21, 1, &buff, MP_MACHINE_I2C_FLAG_STOP)) {
            mp_raise_ValueError(MP_ERROR_TEXT("I2C transfer error"));
        }
        mp_hal_delay_ms(2);

        CAM_capture = 0;
        mp_get_buffer_raise(lcd, &self->fb, MP_BUFFER_WRITE);
        self->isshowing = 1;
        CAM_rdy = 1;
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Not inited! call CAM.init() first"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_cam_show_start_obj, cam_show_start);

STATIC mp_obj_t cam_show_stop(mp_obj_t self_in) {
    machine_cam_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->isinited) {
        uint8_t regs[2] = {CCD(COM7), 0};
        mp_machine_i2c_buf_t buff = {sizeof(regs), regs};
        mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->i2c->type, protocol);

        buff.len--;
        if (0 > i2c_p->transfer(self->i2c, 0x21, 1, &buff, MP_MACHINE_I2C_FLAG_STOP)) {
            mp_raise_ValueError(MP_ERROR_TEXT("I2C transfer error"));
        }
        buff.buf += buff.len;
        if (0 > i2c_p->transfer(self->i2c, 0x21, 1, &buff, MP_MACHINE_I2C_FLAG_READ | MP_MACHINE_I2C_FLAG_STOP)) {
            mp_raise_ValueError(MP_ERROR_TEXT("I2C transfer error"));
        }
        buff.buf -= buff.len;
        buff.len++;

        regs[1] &= 0xF0;

        self->isshowing = 0;
        self->fb.buf = NULL;
        self->fb.len = 0;
        self->fb.typecode = 0;

        if (0 > i2c_p->transfer(self->i2c, 0x21, 1, &buff, MP_MACHINE_I2C_FLAG_STOP)) {
            mp_raise_ValueError(MP_ERROR_TEXT("I2C transfer error"));
        }
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Not inited! call CAM.init() first"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_cam_show_stop_obj, cam_show_stop);

STATIC mp_obj_t camera_flip(mp_obj_t self_in, mp_obj_t onoff) {
    machine_cam_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->isinited) {
        int flg = mp_obj_get_int(onoff);
        uint8_t regs[2] = {CCD(COM3), 0x10};
        mp_machine_i2c_buf_t buff = {sizeof(regs), regs};
        mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->i2c->type, protocol);

        buff.len--;
        if (0 > i2c_p->transfer(self->i2c, 0x21, 1, &buff, MP_MACHINE_I2C_FLAG_STOP)) {
            mp_raise_ValueError(MP_ERROR_TEXT("I2C transfer error"));
        }
        buff.buf += buff.len;
        if (0 > i2c_p->transfer(self->i2c, 0x21, 1, &buff, MP_MACHINE_I2C_FLAG_READ | MP_MACHINE_I2C_FLAG_STOP)) {
            mp_raise_ValueError(MP_ERROR_TEXT("I2C transfer error"));
        }
        buff.buf -= buff.len;
        buff.len++;
        regs[1] = flg? (regs[1] | COM3_V_FLIP) : (regs[1] & ~COM3_V_FLIP);

        if (0 > i2c_p->transfer(self->i2c, 0x21, 1, &buff, MP_MACHINE_I2C_FLAG_STOP)) {
            mp_raise_ValueError(MP_ERROR_TEXT("I2C transfer error"));
        }
        mp_hal_delay_ms(2);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Not inited! call CAM.init() first"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_cam_flip_obj, camera_flip);

STATIC mp_obj_t camera_mirror(mp_obj_t self_in, mp_obj_t onoff) {
    machine_cam_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->isinited) {
        int flg = mp_obj_get_int(onoff);
        uint8_t regs[2] = {CCD(COM3), 0x10};
        mp_machine_i2c_buf_t buff = {sizeof(regs), regs};
        mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->i2c->type, protocol);

        buff.len--;
        if (0 > i2c_p->transfer(self->i2c, 0x21, 1, &buff, MP_MACHINE_I2C_FLAG_STOP)) {
            mp_raise_ValueError(MP_ERROR_TEXT("I2C transfer error"));
        }
        buff.buf += buff.len;
        if (0 > i2c_p->transfer(self->i2c, 0x21, 1, &buff, MP_MACHINE_I2C_FLAG_READ | MP_MACHINE_I2C_FLAG_STOP)) {
            mp_raise_ValueError(MP_ERROR_TEXT("I2C transfer error"));
        }
        buff.buf -= buff.len;
        buff.len++;
        regs[1] = flg? (regs[1] | COM3_H_MIRROR) : (regs[1] & ~COM3_H_MIRROR);

        if (0 > i2c_p->transfer(self->i2c, 0x21, 1, &buff, MP_MACHINE_I2C_FLAG_STOP)) {
            mp_raise_ValueError(MP_ERROR_TEXT("I2C transfer error"));
        }
        mp_hal_delay_ms(2);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Not inited! call CAM.init() first"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_cam_mirror_obj, camera_mirror);

STATIC mp_obj_t camera_contrast(mp_obj_t self_in, mp_obj_t cnst) {
    machine_cam_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->isinited) {
        int res = 0;
        int val = mp_obj_get_int(cnst);
        uint8_t regs[2] = {CCD(CNST), 0x20};
        mp_machine_i2c_buf_t buff = {sizeof(regs), regs};
        mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->i2c->type, protocol);

        if ((val > 4) || (val < -4)) {
            mp_raise_ValueError(MP_ERROR_TEXT("Contrast should be in range [-4 to 4]"));
        } else {
            res = regs[1];
            res += 0x04 * val;
            regs[1] = res;

            if (0 > i2c_p->transfer(self->i2c, 0x21, 1, &buff, MP_MACHINE_I2C_FLAG_STOP)) {
                mp_raise_ValueError(MP_ERROR_TEXT("I2C transfer error"));
            }
        }
        mp_hal_delay_ms(2);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Not inited! call CAM.init() first"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_cam_contrast_obj, camera_contrast);

STATIC mp_obj_t camera_saturation(mp_obj_t self_in, mp_obj_t sat) {
    machine_cam_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->isinited) {
        int res = 0;
        int val = mp_obj_get_int(sat);
        uint8_t regs[2] = {CCD(USAT), 0x40};
        mp_machine_i2c_buf_t buff = {sizeof(regs), regs};
        mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->i2c->type, protocol);

        if ((val > 4) || (val < -4)) {
            mp_raise_ValueError(MP_ERROR_TEXT("Saturation should be in range [-4 to 4]"));
        } else {
            res = regs[1];
            res += 0x04 * val;
            regs[1] = res;

            if (0 > i2c_p->transfer(self->i2c, 0x21, 1, &buff, MP_MACHINE_I2C_FLAG_STOP)) {
                mp_raise_ValueError(MP_ERROR_TEXT("I2C transfer error"));
            }

            regs[0] = CCD(VSAT);

            if (0 > i2c_p->transfer(self->i2c, 0x21, 1, &buff, MP_MACHINE_I2C_FLAG_STOP)) {
                mp_raise_ValueError(MP_ERROR_TEXT("I2C transfer error"));
            }
        }
        mp_hal_delay_ms(2);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Not inited! call CAM.init() first"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_cam_saturation_obj, camera_saturation);

STATIC mp_obj_t camera_brightness(mp_obj_t self_in, mp_obj_t bright) {
    machine_cam_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->isinited) {
        int val = mp_obj_get_int(bright);
        uint8_t regs[2] = {CCD(SIGN), 0};
        mp_machine_i2c_buf_t buff = {sizeof(regs), regs};
        mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->i2c->type, protocol);

        buff.len--;
        if (0 > i2c_p->transfer(self->i2c, 0x21, 1, &buff, MP_MACHINE_I2C_FLAG_STOP)) {
            mp_raise_ValueError(MP_ERROR_TEXT("I2C transfer error"));
        }
        buff.buf += buff.len;
        if (0 > i2c_p->transfer(self->i2c, 0x21, 1, &buff, MP_MACHINE_I2C_FLAG_READ | MP_MACHINE_I2C_FLAG_STOP)) {
            mp_raise_ValueError(MP_ERROR_TEXT("I2C transfer error"));
        }
        buff.buf -= buff.len;
        buff.len++;

        if ((val > 4) || (val < -4)) {
            mp_raise_ValueError(MP_ERROR_TEXT("Saturation should be in range [-4 to 4]"));
        } else {
            if (val < 0) {
                regs[1] |= 0x08;
            } else {
                regs[1] &= 0xF7;
            }

            if (0 > i2c_p->transfer(self->i2c, 0x21, 1, &buff, MP_MACHINE_I2C_FLAG_STOP)) {
                mp_raise_ValueError(MP_ERROR_TEXT("I2C transfer error"));
            }

            regs[0] = CCD(BRIGHT);
            regs[1] = 0x10 * abs(val);

            if (val < 0) {
                regs[1] -= 0x10;
            }

            if (0 > i2c_p->transfer(self->i2c, 0x21, 1, &buff, MP_MACHINE_I2C_FLAG_STOP)) {
                mp_raise_ValueError(MP_ERROR_TEXT("I2C transfer error"));
            }
        }
        mp_hal_delay_ms(2);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Not inited! call CAM.init() first"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_cam_brightness_obj, camera_brightness);

STATIC mp_obj_t camera_speffect(mp_obj_t self_in, mp_obj_t what) {
    machine_cam_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->isinited) {
        int val = mp_obj_get_int(what);
        uint8_t regs[3][2] = {{CCD(SDE), 0}, {CCD(UFix), 0x80}, {CCD(VFix), 0x80}};
        mp_machine_i2c_buf_t buff = {sizeof(regs[0]), regs[0]};
        mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->i2c->type, protocol);

        switch (val)
        {
            case EFFECT_NONE:
                regs[0][1] = 0x06;
                regs[1][1] = 0x80;
                regs[2][1] = 0x80;
                break;

            case EFFECT_NEG:
                regs[0][1] = 0x46;
                regs[1][1] = 0x80;
                regs[2][1] = 0x80;
                break;

            case EFFECT_BW:
                regs[0][1] = 0x26;
                regs[1][1] = 0x80;
                regs[2][1] = 0x80;
                break;

            case EFFECT_RED:
                regs[0][1] = 0x1E;
                regs[1][1] = 0x80;
                regs[2][1] = 0xC0;
                break;

            case EFFECT_GREEN:
                regs[0][1] = 0x1E;
                regs[1][1] = 0x60;
                regs[2][1] = 0x60;
                break;

            case EFFECT_BLUE:
                regs[0][1] = 0x1E;
                regs[1][1] = 0xA0;
                regs[2][1] = 0x40;
                break;

            case EFFECT_RETRO:
                regs[0][1] = 0x1E;
                regs[1][1] = 0x40;
                regs[2][1] = 0xA0;
                break;

            default:
                mp_raise_ValueError(MP_ERROR_TEXT("Wrong argument!"));
                break;
        }

        val = MP_ARRAY_SIZE(regs);
        do
        {
            if (0 > i2c_p->transfer(self->i2c, 0x21, 1, &buff, MP_MACHINE_I2C_FLAG_STOP)) {
                mp_raise_ValueError(MP_ERROR_TEXT("I2C transfer error"));
                break;
            }
            buff.buf += buff.len;
        } while (--val);
        mp_hal_delay_ms(2);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Not inited! call CAM.init() first"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_cam_speffect_obj, camera_speffect);

STATIC mp_obj_t camera_whitebalance(mp_obj_t self_in, mp_obj_t white) {
    machine_cam_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->isinited) {
        int val = mp_obj_get_int(white);
        uint8_t regs[6][2] = {{CCD(COM8), 0}, {CCD(BLUE), 0x80}, {CCD(RED), 0x80}, {CCD(COM5), 0x65}, {CCD(ADVFL), 0}, {CCD(ADVFH), 0}};
        mp_machine_i2c_buf_t buff = {sizeof(regs[0]), regs[0]};
        mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->i2c->type, protocol);

        switch (val)
        {
            case WB_NONE:
                regs[0][1] = 0xff;
                regs[1][1] = 0x80;
                regs[2][1] = 0x80;
                break;

            case WB_SUNNY:
                regs[0][1] = 0xfd;
                regs[1][1] = 0x5a;
                regs[2][1] = 0x5c;
                break;

            case WB_CLOUDY:
                regs[0][1] = 0xfd;
                regs[1][1] = 0x58;
                regs[2][1] = 0x60;
                break;

            case WB_OFFICE:
                regs[0][1] = 0xfd;
                regs[1][1] = 0x84;
                regs[2][1] = 0x4c;
                break;

            case WB_HOME:
                regs[0][1] = 0xfd;
                regs[1][1] = 0x96;
                regs[2][1] = 0x40;
                break;

            default:
                mp_raise_ValueError(MP_ERROR_TEXT("Wrong argument!"));
                break;
        }

        val = MP_ARRAY_SIZE(regs);
        do
        {
            if (0 > i2c_p->transfer(self->i2c, 0x21, 1, &buff, MP_MACHINE_I2C_FLAG_STOP)) {
                mp_raise_ValueError(MP_ERROR_TEXT("I2C transfer error"));
                break;
            }
            buff.buf += buff.len;
        } while (--val);
        mp_hal_delay_ms(2);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Not inited! call CAM.init() first"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_cam_whitebalance_obj, camera_whitebalance);


STATIC const mp_rom_map_elem_t cam_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init),                MP_ROM_PTR(&machine_cam_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),              MP_ROM_PTR(&machine_cam_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_jpeg),                MP_ROM_PTR(&machine_cam_jpeg_obj) },
    { MP_ROM_QSTR(MP_QSTR_yuyv),                MP_ROM_PTR(&machine_cam_yuyv_obj) },

    { MP_ROM_QSTR(MP_QSTR_show_start),          MP_ROM_PTR(&machine_cam_show_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_show_stop),           MP_ROM_PTR(&machine_cam_show_stop_obj) },
    // control
    { MP_ROM_QSTR(MP_QSTR_flip),                MP_ROM_PTR(&machine_cam_flip_obj) },
    { MP_ROM_QSTR(MP_QSTR_mirror),              MP_ROM_PTR(&machine_cam_mirror_obj) },
    { MP_ROM_QSTR(MP_QSTR_contrast),            MP_ROM_PTR(&machine_cam_contrast_obj) },
    { MP_ROM_QSTR(MP_QSTR_saturation),          MP_ROM_PTR(&machine_cam_saturation_obj) },
    { MP_ROM_QSTR(MP_QSTR_brightness),          MP_ROM_PTR(&machine_cam_brightness_obj) },
    { MP_ROM_QSTR(MP_QSTR_speffect),            MP_ROM_PTR(&machine_cam_speffect_obj) },
    { MP_ROM_QSTR(MP_QSTR_whitebalance),        MP_ROM_PTR(&machine_cam_whitebalance_obj) },
    // Constants
    { MP_ROM_QSTR(MP_QSTR_WB_NONE),         MP_ROM_INT(WB_NONE) },
    { MP_ROM_QSTR(MP_QSTR_WB_SUNNY),        MP_ROM_INT(WB_SUNNY) },
    { MP_ROM_QSTR(MP_QSTR_WB_CLOUDY),       MP_ROM_INT(WB_CLOUDY) },
    { MP_ROM_QSTR(MP_QSTR_WB_OFFICE),       MP_ROM_INT(WB_OFFICE) },
    { MP_ROM_QSTR(MP_QSTR_WB_HOME),         MP_ROM_INT(WB_HOME) },

    { MP_ROM_QSTR(MP_QSTR_EFFECT_NONE),     MP_ROM_INT(EFFECT_NONE) },
    { MP_ROM_QSTR(MP_QSTR_EFFECT_NEG),      MP_ROM_INT(EFFECT_NEG) },
    { MP_ROM_QSTR(MP_QSTR_EFFECT_BW),       MP_ROM_INT(EFFECT_BW) },
    { MP_ROM_QSTR(MP_QSTR_EFFECT_RED),      MP_ROM_INT(EFFECT_RED) },
    { MP_ROM_QSTR(MP_QSTR_EFFECT_GREEN),    MP_ROM_INT(EFFECT_GREEN) },
    { MP_ROM_QSTR(MP_QSTR_EFFECT_BLUE),     MP_ROM_INT(EFFECT_BLUE) },
    { MP_ROM_QSTR(MP_QSTR_EFFECT_RETRO),    MP_ROM_INT(EFFECT_RETRO) },
};
STATIC MP_DEFINE_CONST_DICT(cam_locals_dict, cam_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_cam_type,
    MP_QSTR_CAM,
    MP_TYPE_FLAG_NONE,
    make_new, machine_cam_make_new,
    locals_dict, &cam_locals_dict
    );

#else
void next_img() {
}

void SnapShot(void *fb) {
}
#endif
