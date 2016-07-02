/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _FT5x06_H
#define _FT5x06_H

// Slave address
#define FT5x06_ADDR				(0x70 >> 1)

// Maximum timeout
#define FT5x06_TIMEOUT			0x3000

#define FT5x06_DEVICE_MODE        0x00
#define FT5x06_GESTURE_ID         0x01
#define FT5x06_TOUCH_POINTS       0x02

#define FT5x06_TOUCH1_EV_FLAG     0x03
#define FT5x06_TOUCH1_XH          0x03
#define FT5x06_TOUCH1_XL          0x04
#define FT5x06_TOUCH1_YH          0x05
#define FT5x06_TOUCH1_YL          0x06

#define FT5x06_TOUCH2_EV_FLAG     0x09
#define FT5x06_TOUCH2_XH          0x09
#define FT5x06_TOUCH2_XL          0x0A
#define FT5x06_TOUCH2_YH          0x0B
#define FT5x06_TOUCH2_YL          0x0C

#define FT5x06_TOUCH3_EV_FLAG     0x0F
#define FT5x06_TOUCH3_XH          0x0F
#define FT5x06_TOUCH3_XL          0x10
#define FT5x06_TOUCH3_YH          0x11
#define FT5x06_TOUCH3_YL          0x12

#define FT5x06_TOUCH4_EV_FLAG     0x15
#define FT5x06_TOUCH4_XH          0x15
#define FT5x06_TOUCH4_XL          0x16
#define FT5x06_TOUCH4_YH          0x17
#define FT5x06_TOUCH4_YL          0x18

#define FT5x06_TOUCH5_EV_FLAG     0x1B
#define FT5x06_TOUCH5_XH          0x1B
#define FT5x06_TOUCH5_XL          0x1C
#define FT5x06_TOUCH5_YH          0x1D
#define FT5x06_TOUCH5_YL          0x1E

#define FT5x06_ID_G_THGROUP              0x80
#define FT5x06_ID_G_THPEAK               0x81
#define FT5x06_ID_G_THCAL                0x82
#define FT5x06_ID_G_THWATER              0x83
#define FT5x06_ID_G_THTEMP               0x84
#define FT5x06_ID_G_THDIFF               0x85
#define FT5x06_ID_G_CTRL                 0x86
#define FT5x06_ID_G_TIME_ENTER_MONITOR   0x87
#define FT5x06_ID_G_PERIODACTIVE         0x88
#define FT5x06_ID_G_PERIODMONITOR        0x89
#define FT5x06_ID_G_AUTO_CLB_MODE        0xA0
#define FT5x06_ID_G_LIB_VERSION_H        0xA1
#define FT5x06_ID_G_LIB_VERSION_L        0xA2
#define FT5x06_ID_G_CIPHER               0xA3
#define FT5x06_ID_G_MODE                 0xA4
#define FT5x06_ID_G_PMODE                0xA5
#define FT5x06_ID_G_FIRMID               0xA6
#define FT5x06_ID_G_STATE                0xA7
#define FT5x06_ID_G_FT5201ID             0xA8
#define FT5x06_ID_G_ERR                  0xA9

#endif /* _FT5x06_H */
