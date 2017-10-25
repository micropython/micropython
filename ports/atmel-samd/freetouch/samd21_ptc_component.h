/*
 * FreeTouch, a QTouch-compatible library - tested on ATSAMD21 only!
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Limor 'ladyada' Fried for Adafruit Industries
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

// This is similar to the component definitions found in
// sam0/utils/cmsis/samd21/include/component but for the PTC.
#ifndef ADAFRUIT_FREETOUCH_PTC_COMPONENT_H
#define ADAFRUIT_FREETOUCH_PTC_COMPONENT_H

#include "compiler.h"

#undef ENABLE

/*************** CONTROL A register ***************/
#define PTC_REG_CONTROLA   0x42004C00
#define PTC_BIT_ENABLE     0x02
#define PTC_BIT_RUNINSTBY  0x04


typedef union {
  struct {
    uint8_t   SWRESET:1;
    uint8_t   ENABLE:1;
    uint8_t   RUNINSTANDBY:1;
    uint8_t   __pad0__:5;
  } bit;
  uint8_t reg;
} PTC_REG_CONTROLA_Type;

/*************** CONTROL B register ***************/

#define PTC_REG_CONTROLB 0x42004C01
#define PTC_BIT_SYNCFLAG 0x80

typedef union {
  struct {
    uint8_t   __pad0__:7;
    uint8_t   SYNCFLAG:1;
  } bit;
  uint8_t reg;
} PTC_REG_CONTROLB_Type;

/*************** UNK4C04 register ***************/

#define PTC_REG_UNK4C04 0x42004C04

typedef union {
  uint8_t reg;
} PTC_REG_UNK4C04_Type;


/*************** CONTROL C register ***************/

#define PTC_REG_CONTROLC 0x42004C05
#define PTC_BIT_INIT 0x01

typedef union {
  struct {
    uint8_t   INIT:1;
    uint8_t   __pad0__:7;
  } bit;
  uint8_t reg;
} PTC_REG_CONTROLC_Type;



/*************** INT registers ***************/

typedef union {
  struct {
    uint8_t   EOC:1;
    uint8_t   WCO:1;
    uint8_t   __pad0__:6;
  } bit;
  uint8_t reg;
} PTC_REG_INT_Type;


#define PTC_REG_INTDISABLE 0x42004C08
#define PTC_REG_INTENABLE 0x42004C09
#define PTC_BIT_EOCINTEN  0x01
#define PTC_BIT_WCOINTEN  0x02

#define PTC_REG_INTFLAGS 0x42004C0A
#define PTC_BIT_EOCINTFLAG 0x01
#define PTC_BIT_WCOINTFLAG 0x02


/*************** FREQ CONTROL reg ***************/

typedef union {
  struct {
    uint8_t   SAMPLEDELAY:4;
    uint8_t   FREQSPREADEN:1;
    uint8_t   __pad0__:3;
  } bit;
  uint8_t reg;
} PTC_REG_FREQCONTROL_Type;

#define PTC_REG_FREQCONTROL 0x42004C0C
#define PTC_BIT_FREQSPREADEN 0x10
#define PTC_REG_SAMPLEDELAY_MASK 0x0F

/*************** CONVERT CONTROL reg ***************/

typedef union {
  struct {
    uint8_t   ADCACCUM:3;
    uint8_t   __pad0__:4;
    uint8_t   CONVERT:1;
  } bit;
  uint8_t reg;
} __attribute__ ((packed)) PTC_REG_CONVCONTROL_Type;


#define PTC_REG_CONVCONTROL 0x42004C0D
#define PTC_BIT_CONVSTARTED 0x80
#define PTC_REG_ADCACC_MASK 0x07


/*************** Y SELECT L+H reg ***************/

typedef union {
  struct {
    uint8_t   Y0:1;
    uint8_t   Y1:1;
    uint8_t   Y2:1;
    uint8_t   Y3:1;
    uint8_t   Y4:1;
    uint8_t   Y5:1;
    uint8_t   Y6:1;
    uint8_t   Y7:1;
  } bit;
  uint8_t reg;
} __attribute__ ((packed)) PTC_REG_YSELECTL_Type;

typedef union {
  struct {
    uint8_t   Y8:1;
    uint8_t   Y9:1;
    uint8_t   Y10:1;
    uint8_t   Y11:1;
    uint8_t   Y12:1;
    uint8_t   Y13:1;
    uint8_t   Y14:1;
    uint8_t   Y15:1;
  } bit;
  uint8_t reg;
} __attribute__ ((packed)) PTC_REG_YSELECTH_Type;

#define PTC_REG_YSELECT_L 0x42004C10
#define PTC_REG_YSELECT_H 0x42004C11

#define PTC_REG_YENABLE_L 0x42004C14
#define PTC_REG_YENABLE_H 0x42004C15


/*************** X SELECT L+H reg ***************/

typedef union {
  struct {
    uint8_t   X0:1;
    uint8_t   X1:1;
    uint8_t   X2:1;
    uint8_t   X3:1;
    uint8_t   X4:1;
    uint8_t   X5:1;
    uint8_t   X6:1;
    uint8_t   X7:1;
  } bit;
  uint8_t reg;
} __attribute__ ((packed)) PTC_REG_XSELECTL_Type;

typedef union {
  struct {
    uint8_t   X8:1;
    uint8_t   X9:1;
    uint8_t   X10:1;
    uint8_t   X11:1;
    uint8_t   X12:1;
    uint8_t   X13:1;
    uint8_t   X14:1;
    uint8_t   X15:1;
  } bit;
  uint8_t reg;
} __attribute__ ((packed)) PTC_REG_XSELECTH_Type;


#define PTC_REG_XSELECT_L 0x42004C12
#define PTC_REG_XSELECT_H 0x42004C13

#define PTC_REG_XENABLE_L 0x42004C16
#define PTC_REG_XENABLE_H 0x42004C17

/*************** Compensation Cap reg ***************/

typedef union {
  struct {
    uint8_t   VALUE:8;
  } bit;
  uint8_t reg;
} __attribute__ ((packed)) PTC_REG_COMPCAPL_Type;

typedef union {
  struct {
    uint8_t   VALUE:6;
    uint8_t   __pad0__:2;
  } bit;
  uint8_t reg;
} __attribute__ ((packed)) PTC_REG_COMPCAPH_Type;

#define PTC_REG_COMPCAPL    0x42004C18
#define PTC_REG_COMPCAPH    0x42004C19

/*************** Int Cap reg ***************/

typedef union {
  struct {
    uint8_t   VALUE:6;
    uint8_t   __pad0__:2;
  } bit;
  uint8_t reg;
} __attribute__ ((packed)) PTC_REG_INTCAP_Type;


#define PTC_REG_INTCAP    0x42004C1A

/*************** Series resistor reg ***************/

typedef union {
  struct {
    uint8_t   RESISTOR:2;
    uint8_t   __pad0__:6;
  } bit;
  uint8_t reg;
} __attribute__ ((packed)) PTC_REG_SERRES_Type;

#define PTC_REG_SERIESRES    0x42004C1B

/*************** conversion result reg ***************/

typedef union {
  struct {
    uint8_t   LOWBYTE;
    uint8_t   HIGHBYTE;
  } byte;
  uint16_t reg;
} __attribute__ ((packed)) PTC_REG_CONVRESULT_Type;

#define PTC_REG_CONVRESULT_L 0x42004C1C
#define PTC_REG_CONVRESULT_H 0x42004C1D

/*************** burst mode reg ***************/

typedef union {
  struct {
    uint8_t   __pad0__:2;
    uint8_t   CTSLOWPOWER:1;
    uint8_t   __pad1__:1;
    uint8_t   BURSTMODE:4;
  } bit;
  uint8_t reg;
} __attribute__ ((packed)) PTC_REG_BURSTMODE_Type;


#define PTC_REG_BURSTMODE     0x42004C20
#define PTC_REG_BURSTMODE_MASK   0xF0
#define PTC_BIT_CTSLOWPOWER   0x04

/*************** etc unused reg ***************/

#define PTC_REG_XYENABLE     0x42004C16
#define PTC_BIT_XYENABLE     0x02

#define PTC_REG_WCO_MODE     0x42004C21
#define PTC_REG_WCO_MODE_MASK   0x07

#define PTC_SET_WCO_THRESHHOLD_A_L 0x42004C24
#define PTC_SET_WCO_THRESHHOLD_A_H 0x42004C25
#define PTC_SET_WCO_THRESHHOLD_B_L 0x42004C26
#define PTC_SET_WCO_THRESHHOLD_B_H 0x42004C27

typedef struct {
  __IO PTC_REG_CONTROLA_Type CONTROLA;    // 0x42004C00
  __IO PTC_REG_CONTROLB_Type CONTROLB;    // 0x42004C01
  uint8_t   __pad4c02__;                // 0x42004C02 unknown
  uint8_t   __pad4c03__;                // 0x42004C03 unknown
  __IO PTC_REG_UNK4C04_Type  UNK4C04;   // 0x42004C04 unknown
  __IO PTC_REG_CONTROLC_Type CONTROLC;    // 0x42004C05
  uint8_t   __pad4c06__;                // 0x42004C06 unknown
  uint8_t   __pad4c07__;                // 0x42004C07 unknown
  __IO PTC_REG_INT_Type      INTDISABLE;  // 0x42004C08
  __IO PTC_REG_INT_Type      INTENABLE;   // 0x42004C09
  __IO PTC_REG_INT_Type      INTFLAGS;    // 0x42004C0A
  uint8_t   __pad4c0b__;                // 0x42004C0B unknown
  __IO PTC_REG_FREQCONTROL_Type FREQCONTROL;  //0x42004C0C
  __IO PTC_REG_CONVCONTROL_Type CONVCONTROL;  // 0x42004C0D
  uint8_t   __pad4c0e__;                // 0x42004C0E unknown
  uint8_t   __pad4c0f__;                // 0x42004C0F unknown
  __IO PTC_REG_YSELECTL_Type YSELECTL;  // 0x42004C10
  __IO PTC_REG_YSELECTL_Type YSELECTH;  // 0x42004C11
  __IO PTC_REG_XSELECTL_Type XSELECTL;  // 0x42004C12
  __IO PTC_REG_XSELECTL_Type XSELECTH;  // 0x42004C13
  __IO PTC_REG_YSELECTL_Type YENABLEL;  // 0x42004C14
  __IO PTC_REG_YSELECTL_Type YENABLEH;  // 0x42004C15
  __IO PTC_REG_XSELECTL_Type XENABLEL;  // 0x42004C16
  __IO PTC_REG_XSELECTL_Type XENABLEH;  // 0x42004C17

  __IO PTC_REG_COMPCAPL_Type COMPCAPL;  // 0x42004C18
  __IO PTC_REG_COMPCAPH_Type COMPCAPH;  // 0x42004C19
  __IO PTC_REG_INTCAP_Type   INTCAP;    // 0x42004C1A
  __IO PTC_REG_SERRES_Type   SERRES;    // 0x42004C1B

  __IO PTC_REG_CONVRESULT_Type RESULT;  // 0x42004C1C + 0x42004C1D
  uint8_t   __pad4c1e__;                // 0x42004C1E unknown
  uint8_t   __pad4c1f__;                // 0x42004C1F unknown
  __IO PTC_REG_BURSTMODE_Type BURSTMODE; // 0x42004C20
} Ptc;

#define PTC  (( Ptc *)0x42004C00U)

#define PTC_REG_INTDISABLE 0x42004C08
#define PTC_REG_INTENABLE 0x42004C09
#define PTC_BIT_EOCINTEN  0x01
#define PTC_BIT_WCOINTEN  0x02

#define PTC_REG_INTFLAGS 0x42004C0A

#endif  // ADAFRUIT_FREETOUCH_PTC_COMPONENT_H
