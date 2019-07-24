/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_FXOS_H_
#define _FSL_FXOS_H_

#include "fsl_common.h"

#define FXOS8700CQ_ACCEL_RESOLUTION_BITS 14

/*
 *  STATUS Register
 */
#define STATUS_00_REG 0x00

#define ZYXOW_MASK 0x80
#define ZOW_MASK 0x40
#define YOW_MASK 0x20
#define XOW_MASK 0x10
#define ZYXDR_MASK 0x08
#define ZDR_MASK 0x04
#define YDR_MASK 0x02
#define XDR_MASK 0x01

/*
 *  F_STATUS FIFO Status Register
 */
#define F_STATUS_REG 0x00

#define F_OVF_MASK 0x80
#define F_WMRK_FLAG_MASK 0x40
#define F_CNT5_MASK 0x20
#define F_CNT4_MASK 0x10
#define F_CNT3_MASK 0x08
#define F_CNT2_MASK 0x04
#define F_CNT1_MASK 0x02
#define F_CNT0_MASK 0x01
#define F_CNT_MASK 0x3F

/*
 *  XYZ Data Registers
 */
#define OUT_X_MSB_REG 0x01
#define OUT_X_LSB_REG 0x02
#define OUT_Y_MSB_REG 0x03
#define OUT_Y_LSB_REG 0x04
#define OUT_Z_MSB_REG 0x05
#define OUT_Z_LSB_REG 0x06

/*
 *  F_SETUP FIFO Setup Register
 */
#define F_SETUP_REG 0x09

#define F_MODE1_MASK 0x80
#define F_MODE0_MASK 0x40
#define F_WMRK5_MASK 0x20
#define F_WMRK4_MASK 0x10
#define F_WMRK3_MASK 0x08
#define F_WMRK2_MASK 0x04
#define F_WMRK1_MASK 0x02
#define F_WMRK0_MASK 0x01
#define F_MODE_MASK 0xC0
#define F_WMRK_MASK 0x3F

#define F_MODE_DISABLED 0x00
#define F_MODE_CIRCULAR (F_MODE0_MASK)
#define F_MODE_FILL (F_MODE1_MASK)
#define F_MODE_TRIGGER (F_MODE1_MASK + F_MODE0_MASK)

/*
 *  TRIG_CFG FIFO Trigger Configuration Register
 */
#define TRIG_CFG_REG 0x0A

#define TRIG_TRANS_MASK 0x20
#define TRIG_LNDPRT_MASK 0x10
#define TRIG_PULSE_MASK 0x08
#define TRIG_FF_MT_MASK 0x04

/*
 *  SYSMOD System Mode Register
 */
#define SYSMOD_REG 0x0B

#define FGERR_MASK 0x80 /* MMA8451 only */
#define FGT_4_MASK 0x40 /* MMA8451 only */
#define FGT_3_MASK 0x20 /* MMA8451 only */
#define FGT_2_MASK 0x10 /* MMA8451 only */
#define FGT_1_MASK 0x08 /* MMA8451 only */
#define FGT_0_MASK 0x04 /* MMA8451 only */
#define FGT_MASK 0x7C   /* MMA8451 only */
#define SYSMOD1_MASK 0x02
#define SYSMOD0_MASK 0x01
#define SYSMOD_MASK 0x03

#define SYSMOD_STANDBY 0x00
#define SYSMOD_WAKE (SYSMOD0_MASK)
#define SYSMOD_SLEEP (SYSMOD1_MASK)

/*
 *  INT_SOURCE System Interrupt Status Register
 */
#define INT_SOURCE_REG 0x0C

#define SRC_ASLP_MASK 0x80
#define SRC_FIFO_MASK 0x40
#define SRC_TRANS_MASK 0x20
#define SRC_LNDPRT_MASK 0x10
#define SRC_PULSE_MASK 0x08
#define SRC_FF_MT_MASK 0x04
#define SRC_DRDY_MASK 0x01

/*
 *  WHO_AM_I Device ID Register
 */
#define WHO_AM_I_REG 0x0D

/* Content */
#define kFXOS_WHO_AM_I_Device_ID 0xC7

/* XYZ_DATA_CFG Sensor Data Configuration Register */
#define XYZ_DATA_CFG_REG 0x0E

#define HPF_OUT_MASK 0x10
#define FS1_MASK 0x02
#define FS0_MASK 0x01
#define FS_MASK 0x03

#define FULL_SCALE_2G 0x00
#define FULL_SCALE_4G (FS0_MASK)
#define FULL_SCALE_8G (FS1_MASK)

/* HP_FILTER_CUTOFF High Pass Filter Register */
#define HP_FILTER_CUTOFF_REG 0x0F

#define PULSE_HPF_BYP_MASK 0x20
#define PULSE_LPF_EN_MASK 0x10
#define SEL1_MASK 0x02
#define SEL0_MASK 0x01
#define SEL_MASK 0x03

/*
 *  PL_STATUS Portrait/Landscape Status Register
 */
#define PL_STATUS_REG 0x10

#define NEWLP_MASK 0x80
#define LO_MASK 0x40
#define LAPO1_MASK 0x04
#define LAPO0_MASK 0x02
#define BAFRO_MASK 0x01
#define LAPO_MASK 0x06

/*
 *  PL_CFG Portrait/Landscape Configuration Register
 */
#define PL_CFG_REG 0x11

#define DBCNTM_MASK 0x80
#define PL_EN_MASK 0x40

/*
 *  PL_COUNT Portrait/Landscape Debounce Register
 */
#define PL_COUNT_REG 0x12

/*
 *  PL_BF_ZCOMP Back/Front and Z Compensation Register
 */
#define PL_BF_ZCOMP_REG 0x13

#define BKFR1_MASK 0x80
#define BKFR0_MASK 0x40
#define ZLOCK2_MASK 0x04
#define ZLOCK1_MASK 0x02
#define ZLOCK0_MASK 0x01
#define BKFR_MASK 0xC0
#define ZLOCK_MASK 0x07

/*
 *  PL_P_L_THS Portrait to Landscape Threshold Register
 */
#define PL_P_L_THS_REG 0x14

#define P_L_THS4_MASK 0x80
#define P_L_THS3_MASK 0x40
#define P_L_THS2_MASK 0x20
#define P_L_THS1_MASK 0x10
#define P_L_THS0_MASK 0x08
#define HYS2_MASK 0x04
#define HYS1_MASK 0x02
#define HYS0_MASK 0x01
#define P_L_THS_MASK 0xF8
#define HYS_MASK 0x07

/*
 *  FF_MT_CFG Freefall and Motion Configuration Register
 */
#define FF_MT_CFG_REG 0x15

#define ELE_MASK 0x80
#define OAE_MASK 0x40
#define ZEFE_MASK 0x20
#define YEFE_MASK 0x10
#define XEFE_MASK 0x08

/*
 *  FF_MT_SRC Freefall and Motion Source Registers
 */
#define FF_MT_SRC_REG 0x16

#define EA_MASK 0x80
#define ZHE_MASK 0x20
#define ZHP_MASK 0x10
#define YHE_MASK 0x08
#define YHP_MASK 0x04
#define XHE_MASK 0x02
#define XHP_MASK 0x01

/*
 *  FF_MT_THS Freefall and Motion Threshold Registers
 *  TRANSIENT_THS Transient Threshold Register
 */
#define FT_MT_THS_REG 0x17
#define TRANSIENT_THS_REG 0x1F

#define DBCNTM_MASK 0x80
#define THS6_MASK 0x40
#define THS5_MASK 0x20
#define THS4_MASK 0x10
#define THS3_MASK 0x08
#define THS2_MASK 0x04
#define TXS1_MASK 0x02
#define THS0_MASK 0x01
#define THS_MASK 0x7F

/* FF_MT_COUNT Freefall Motion Count Registers */
#define FF_MT_COUNT_REG 0x18

/* TRANSIENT_CFG Transient Configuration Register */
#define TRANSIENT_CFG_REG 0x1D

#define TELE_MASK 0x10
#define ZTEFE_MASK 0x08
#define YTEFE_MASK 0x04
#define XTEFE_MASK 0x02
#define HPF_BYP_MASK 0x01

/* TRANSIENT_SRC Transient Source Register */
#define TRANSIENT_SRC_REG 0x1E

#define TEA_MASK 0x40
#define ZTRANSE_MASK 0x20
#define Z_TRANS_POL_MASK 0x10
#define YTRANSE_MASK 0x08
#define Y_TRANS_POL_MASK 0x04
#define XTRANSE_MASK 0x02
#define X_TRANS_POL_MASK 0x01

/* TRANSIENT_COUNT Transient Debounce Register */
#define TRANSIENT_COUNT_REG 0x20

/* PULSE_CFG Pulse Configuration Register */
#define PULSE_CFG_REG 0x21

#define DPA_MASK 0x80
#define PELE_MASK 0x40
#define ZDPEFE_MASK 0x20
#define ZSPEFE_MASK 0x10
#define YDPEFE_MASK 0x08
#define YSPEFE_MASK 0x04
#define XDPEFE_MASK 0x02
#define XSPEFE_MASK 0x01

/* PULSE_SRC Pulse Source Register */
#define PULSE_SRC_REG 0x22

#define PEA_MASK 0x80
#define AXZ_MASK 0x40
#define AXY_MASK 0x20
#define AXX_MASK 0x10
#define DPE_MASK 0x08
#define POLZ_MASK 0x04
#define POLY_MASK 0x02
#define POLX_MASK 0x01

/* PULSE_THS XYZ Pulse Threshold Registers */
#define PULSE_THSX_REG 0x23
#define PULSE_THSY_REG 0x24
#define PULSE_THSZ_REG 0x25

#define PTHS_MASK 0x7F

/* PULSE_TMLT Pulse Time Window Register */
#define PULSE_TMLT_REG 0x26

/* PULSE_LTCY Pulse Latency Timer Register */
#define PULSE_LTCY_REG 0x27

/* PULSE_WIND Second Pulse Time Window Register */
#define PULSE_WIND_REG 0x28

/* ASLP_COUNT Auto Sleep Inactivity Timer Register */
#define ASLP_COUNT_REG 0x29

/* CTRL_REG1 System Control 1 Register */
#define CTRL_REG1 0x2A

#define ASLP_RATE1_MASK 0x80
#define ASLP_RATE0_MASK 0x40
#define DR2_MASK 0x20
#define DR1_MASK 0x10
#define DR0_MASK 0x08
#define LNOISE_MASK 0x04
#define FREAD_MASK 0x02
#define ACTIVE_MASK 0x01
#define ASLP_RATE_MASK 0xC0
#define DR_MASK 0x38

#define ASLP_RATE_20MS 0x00
#define ASLP_RATE_80MS (ASLP_RATE0_MASK)
#define ASLP_RATE_160MS (ASLP_RATE1_MASK)
#define ASLP_RATE_640MS (ASLP_RATE1_MASK + ASLP_RATE0_MASK)

#define ASLP_RATE_50HZ (ASLP_RATE_20MS)
#define ASLP_RATE_12_5HZ (ASLP_RATE_80MS)
#define ASLP_RATE_6_25HZ (ASLP_RATE_160MS)
#define ASLP_RATE_1_56HZ (ASLP_RATE_640MS)

#define HYB_ASLP_RATE_25HZ (ASLP_RATE_20MS)
#define HYB_ASLP_RATE_6_25HZ (ASLP_RATE_80MS)
#define HYB_ASLP_RATE_1_56HZ (ASLP_RATE_160MS)
#define HYB_ASLP_RATE_0_8HZ (ASLP_RATE_640MS)

#define DATA_RATE_1250US 0x00
#define DATA_RATE_2500US (DR0_MASK)
#define DATA_RATE_5MS (DR1_MASK)
#define DATA_RATE_10MS (DR1_MASK + DR0_MASK)
#define DATA_RATE_20MS (DR2_MASK)
#define DATA_RATE_80MS (DR2_MASK + DR0_MASK)
#define DATA_RATE_160MS (DR2_MASK + DR1_MASK)
#define DATA_RATE_640MS (DR2_MASK + DR1_MASK + DR0_MASK)

#define DATA_RATE_800HZ (DATA_RATE_1250US)
#define DATA_RATE_400HZ (DATA_RATE_2500US)
#define DATA_RATE_200HZ (DATA_RATE_5MS)
#define DATA_RATE_100HZ (DATA_RATE_10MS)
#define DATA_RATE_50HZ (DATA_RATE_20MS)
#define DATA_RATE_12_5HZ (DATA_RATE_80MS)
#define DATA_RATE_6_25HZ (DATA_RATE_160MS)
#define DATA_RATE_1_56HZ (DATA_RATE_640MS)

/* for hybrid (TO, Aug 2012) */
#define HYB_DATA_RATE_400HZ (DATA_RATE_1250US)
#define HYB_DATA_RATE_200HZ (DATA_RATE_2500US)
#define HYB_DATA_RATE_100HZ (DATA_RATE_5MS)
#define HYB_DATA_RATE_50HZ (DATA_RATE_10MS)
#define HYB_DATA_RATE_25HZ (DATA_RATE_20MS)
#define HYB_DATA_RATE_6_25HZ (DATA_RATE_80MS)
#define HYB_DATA_RATE_3_15HZ (DATA_RATE_160MS)
#define HYB_DATA_RATE_0_8HZ (DATA_RATE_640MS)

#define ACTIVE (ACTIVE_MASK)
#define STANDBY 0x00

/* CTRL_REG2 System Control 2 Register */
#define CTRL_REG2 0x2B

#define ST_MASK 0x80
#define RST_MASK 0x40
#define SMODS1_MASK 0x10
#define SMODS0_MASK 0x08
#define SLPE_MASK 0x04
#define MODS1_MASK 0x02
#define MODS0_MASK 0x01
#define SMODS_MASK 0x18
#define MODS_MASK 0x03

#define SMOD_NORMAL 0x00
#define SMOD_LOW_NOISE (SMODS0_MASK)
#define SMOD_HIGH_RES (SMODS1_MASK)
#define SMOD_LOW_POWER (SMODS1_MASK + SMODS0_MASK)

#define MOD_NORMAL 0x00
#define MOD_LOW_NOISE (MODS0_MASK)
#define MOD_HIGH_RES (MODS1_MASK)
#define MOD_LOW_POWER (MODS1_MASK + MODS0_MASK)

/* CTRL_REG3 Interrupt Control Register */
#define CTRL_REG3 0x2C

#define FIFO_GATE_MASK 0x80
#define WAKE_TRANS_MASK 0x40
#define WAKE_LNDPRT_MASK 0x20
#define WAKE_PULSE_MASK 0x10
#define WAKE_FF_MT_MASK 0x08
#define IPOL_MASK 0x02
#define PP_OD_MASK 0x01

/* CTRL_REG4 Interrupt Enable Register */
#define CTRL_REG4 0x2D

#define INT_EN_ASLP_MASK 0x80
#define INT_EN_FIFO_MASK 0x40
#define INT_EN_TRANS_MASK 0x20
#define INT_EN_LNDPRT_MASK 0x10
#define INT_EN_PULSE_MASK 0x08
#define INT_EN_FF_MT_MASK 0x04
#define INT_EN_DRDY_MASK 0x01

/* CTRL_REG5 Interrupt Configuration Register */
#define CTRL_REG5 0x2E

#define INT_CFG_ASLP_MASK 0x80
#define INT_CFG_FIFO_MASK 0x40
#define INT_CFG_TRANS_MASK 0x20
#define INT_CFG_LNDPRT_MASK 0x10
#define INT_CFG_PULSE_MASK 0x08
#define INT_CFG_FF_MT_MASK 0x04
#define INT_CFG_DRDY_MASK 0x01

/* XYZ Offset Correction Registers */
#define OFF_X_REG 0x2F
#define OFF_Y_REG 0x30
#define OFF_Z_REG 0x31

/* M_DR_STATUS Register */
#define M_DR_STATUS_REG 0x32

#define ZYXOW_MASK 0x80
#define ZOW_MASK 0x40
#define YOW_MASK 0x20
#define XOW_MASK 0x10
#define ZYXDR_MASK 0x08
#define ZDR_MASK 0x04
#define YDR_MASK 0x02
#define XDR_MASK 0x01

/* MAG XYZ Data Registers */
#define M_OUT_X_MSB_REG 0x33
#define M_OUT_X_LSB_REG 0x34
#define M_OUT_Y_MSB_REG 0x35
#define M_OUT_Y_LSB_REG 0x36
#define M_OUT_Z_MSB_REG 0x37
#define M_OUT_Z_LSB_REG 0x38

/* MAG CMP Data Registers */
#define CMP_X_MSB_REG 0x39
#define CMP_X_LSB_REG 0x3A
#define CMP_Y_MSB_REG 0x3B
#define CMP_Y_LSB_REG 0x3C
#define CMP_Z_MSB_REG 0x3D
#define CMP_Z_LSB_REG 0x3E

/* MAG XYZ Offset Correction Registers */
#define M_OFF_X_MSB_REG 0x3F
#define M_OFF_X_LSB_REG 0x40
#define M_OFF_Y_MSB_REG 0x41
#define M_OFF_Y_LSB_REG 0x42
#define M_OFF_Z_MSB_REG 0x43
#define M_OFF_Z_LSB_REG 0x44

/* MAG MAX XYZ Registers */
#define MAX_X_MSB_REG 0x45
#define MAX_X_LSB_REG 0x46
#define MAX_Y_MSB_REG 0x47
#define MAX_Y_LSB_REG 0x48
#define MAX_Z_MSB_REG 0x49
#define MAX_Z_LSB_REG 0x4A

/* MAG MIN XYZ Registers */
#define MIN_X_MSB_REG 0x4B
#define MIN_X_LSB_REG 0x4C
#define MIN_Y_MSB_REG 0x4D
#define MIN_Y_LSB_REG 0x4E
#define MIN_Z_MSB_REG 0x4F
#define MIN_Z_LSB_REG 0x50

/* TEMP Registers */
#define TEMP_REG 0x51

/* M_THS CONFIG Registers */
#define M_THS_CFG_REG 0x52

/* M_THS SRC Registers */
#define M_THS_SRC_REG 0x53

/* MAG THRESHOLD XYZ Registers */
#define M_THS_X_MSB_REG 0x54
#define M_THS_X_LSB_REG 0x55
#define M_THS_Y_MSB_REG 0x56
#define M_THS_Y_LSB_REG 0x57
#define M_THS_Z_MSB_REG 0x58
#define M_THS_Z_LSB_REG 0x59

/* M_THS COUNT Registers */
#define M_THS_COUNT 0x5A

/* MAG CTRL_REG1 System Control 1 Register */
#define M_CTRL_REG1 0x5B

#define M_ACAL_MASK 0x80
#define M_RST_MASK 0x40
#define M_OST_MASK 0x20
#define M_OSR2_MASK 0x10
#define M_OSR1_MASK 0x08
#define M_OSR0_MASK 0x04
#define M_HMS1_MASK 0x02
#define M_HMS0_MASK 0x01
#define M_OSR_MASK 0x1C
#define M_HMS_MASK 0x03

/* OSR Selections */
#define M_OSR_1_56_HZ 0x00
#define M_OSR_6_25_HZ M_OSR0_MASK
#define M_OSR_12_5_HZ M_OSR1_MASK
#define M_OSR_50_HZ M_OSR1_MASK + M_OSR0_MASK
#define M_OSR_100_HZ M_OSR2_MASK
#define M_OSR_200_HZ M_OSR2_MASK + M_OSR0_MASK
#define M_OSR_400_HZ M_OSR2_MASK + M_OSR1_MASK
#define M_OSR_800_HZ M_OSR2_MASK + M_OSR1_MASK + M_OSR0_MASK

/* Hybrid Mode Selection */
#define ACCEL_ACTIVE 0x00
#define MAG_ACTIVE M_HMS0_MASK
#define HYBRID_ACTIVE (M_HMS1_MASK | M_HMS0_MASK)

/* MAG CTRL_REG2 System Control 2 Register */
#define M_CTRL_REG2 0x5C

#define M_HYB_AUTOINC_MASK 0x20
#define M_MAXMIN_DIS_MASK 0x10
#define M_MAXMIN_DIS_THS_MASK 0x08
#define M_MAXMIN_RST_MASK 0x04
#define M_RST_CNT1_MASK 0x02
#define M_RST_CNT0_MASK 0x01

/* Mag Auto-Reset De-Gauss Frequency */
#define RST_ODR_CYCLE 0x00
#define RST_16_ODR_CYCLE M_RST_CNT0_MASK
#define RST_512_ODR_CYCLE M_RST_CNT1_MASK
#define RST_DISABLED M_RST_CNT1_MASK + M_RST_CNT0_MASK

/* MAG CTRL_REG3 System Control 3 Register */
#define M_CTRL_REG3 0x5D

#define M_RAW_MASK 0x80
#define M_ASLP_OS_2_MASK 0x40
#define M_ASLP_OS_1_MASK 0x20
#define M_ASLP_OS_0_MASK 0x10
#define M_THS_XYZ_MASK 0x08
#define M_ST_Z_MASK 0x04
#define M_ST_XY1_MASK 0x02
#define M_ST_XY0_MASK 0x01
#define M_ASLP_OSR_MASK 0x70
#define M_ST_XY_MASK 0x03

/* OSR Selections */
#define M_ASLP_OSR_1_56_HZ 0x00
#define M_ASLP_OSR_6_25_HZ M_ASLP_OS_0_MASK
#define M_ASLP_OSR_12_5_HZ M_ASLP_OS_1_MASK
#define M_ASLP_OSR_50_HZ M_ASLP_OS_1_MASK + M_ASLP_OS_0_MASK
#define M_ASLP_OSR_100_HZ M_ASLP_OS_2_MASK
#define M_ASLP_OSR_200_HZ M_ASLP_OS_2_MASK + M_ASLP_OS_0_MASK
#define M_ASLP_OSR_400_HZ M_ASLP_OS_2_MASK + M_ASLP_OS_1_MASK
#define M_ASLP_OSR_800_HZ M_ASLP_OS_2_MASK + M_ASLP_OS_1_MASK + M_ASLP_OS_0_MASK

/* MAG INT SOURCE Register */
#define M_INT_SOURCE 0x5E

#define SRC_M_DRDY_MASK 0x04
#define SRC_M_VECM_MASK 0x02
#define SRC_M_THS_MASK 0x01

/* ACCEL VECTOR CONFIG Register */
#define A_VECM_CFG 0x5F

#define A_VECM_INIT_CFG_MASK 0x40
#define A_VECM_INIT_EN_MASK 0x20
#define A_VECM_WAKE_EN_MASK 0x10
#define A_VECM_EN_MASK 0x08
#define A_VECM_UPDM_MASK 0x04
#define A_VECM_INITM_MASK 0x02
#define A_VECM_ELE_MASK 0x01

/* ACCEL VECTOR THS MSB AND LSB Register */
#define A_VECM_THS_MSB 0x60

#define A_VECM_DBCNTM_MASK 0x80

#define A_VECM_THS_LSB 0x61

/* ACCEL VECTOR CNT Register */
#define A_VECM_CNT 0x62

/* ACCEL INITIAL XYZ VECTORS Register */
#define A_VECM_INITX_MSB 0x63
#define A_VECM_INITX_LSB 0x64
#define A_VECM_INITY_MSB 0x65
#define A_VECM_INITY_LSB 0x66
#define A_VECM_INITZ_MSB 0x67
#define A_VECM_INITZ_LSB 0x68

/* MAG VECTOR CONFIG Register */
#define M_VECM_CFG 0x69

#define M_VECM_INIT_CFG_MASK 0x40
#define M_VECM_INIT_EN_MASK 0x20
#define M_VECM_WAKE_EN_MASK 0x10
#define M_VECM_EN_MASK 0x08
#define M_VECM_UPDM_MASK 0x04
#define M_VECM_INITM_MASK 0x02
#define M_VECM_ELE_MASK 0x01

/* MAG VECTOR THS MSB AND LSB Register */
#define M_VECM_THS_MSB 0x6A

#define M_VECM_DBCNTM_MASK 0x80

#define M_VECM_THS_LSB 0x6B

/* MAG VECTOR CNT Register */
#define M_VECM_CNT 0x6C

/* MAG INITIAL XYZ VECTORS Register */
#define M_VECM_INITX_MSB 0x6D
#define M_VECM_INITX_LSB 0x6E
#define M_VECM_INITY_MSB 0x6F
#define M_VECM_INITY_LSB 0x70
#define M_VECM_INITZ_MSB 0x71
#define M_VECM_INITZ_LSB 0x72

/* ACCEL FFMT THS X MSB AND LSB Register */
#define A_FFMT_THS_X_MSB 0x73

#define A_FFMT_THS_XYZ_EN_MASK 0x80

#define A_FFMT_THS_X_LSB 0x74

#define A_FFMT_THS_X_LSB_MASK 0xFC

/* ACCEL FFMT THS Y MSB AND LSB Register */
#define A_FFMT_THS_Y_MSB 0x75

#define A_FFMT_THS_Y_EN_MASK 0x80

#define A_FFMT_THS_Y_LSB 0x76

#define A_FFMT_THS_Y_LSB_MASK 0xFC

/* ACCEL FFMT THS Z MSB AND LSB Register */
#define A_FFMT_THS_Z_MSB 0x77

#define A_FFMT_THS_Z_EN_MASK 0x80

#define A_FFMT_THS_Z_LSB 0x78

#define A_FFMT_THS_Z_LSB_MASK 0xFC

/* ACCEL TRANSIENT INIT Register */
#define A_TRAN_INIT_XYZ_MSB 0x79
#define A_TRAN_INIT_X_LSB 0x7A
#define A_TRAN_INIT_Y_LSB 0x7B
#define A_TRAN_INIT_Z_LSB 0x7C

/*! @brief Define I2C access function. */
typedef status_t (*I2C_SendFunc_t)(uint8_t deviceAddress, uint32_t subAddress, uint8_t subaddressSize, uint32_t txBuff);
typedef status_t (*I2C_ReceiveFunc_t)(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subaddressSize, uint8_t *rxBuff, uint8_t rxBuffSize);

/*! @brief fxos8700cq configure definition. This structure should be global.*/
typedef struct _fxos_handle
{
    /* Pointer to the user-defined I2C Send Data function. */
    I2C_SendFunc_t I2C_SendFunc;
    /* Pointer to the user-defined I2C Receive Data function. */
    I2C_ReceiveFunc_t I2C_ReceiveFunc;
    /* The I2C slave address . */
    uint8_t slaveAddress;
} fxos_handle_t;

typedef struct _fxos8700cq_data
{
    uint8_t accelXMSB;
    uint8_t accelXLSB;
    uint8_t accelYMSB;
    uint8_t accelYLSB;
    uint8_t accelZMSB;
    uint8_t accelZLSB;
    uint8_t magXMSB;
    uint8_t magXLSB;
    uint8_t magYMSB;
    uint8_t magYLSB;
    uint8_t magZMSB;
    uint8_t magZLSB;
} fxos_data_t;

/*! @brief fxos8700cq configure structure.*/
typedef struct _fxos_config
{
    /* Pointer to the user-defined I2C Send Data function. */
    I2C_SendFunc_t I2C_SendFunc;
    /* Pointer to the user-defined I2C Receive Data function. */
    I2C_ReceiveFunc_t I2C_ReceiveFunc;
    /* The I2C slave address . */
    uint8_t slaveAddress;
} fxos_config_t;

/*!
 * @addtogroup fxos_common
 * @{
 */

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Verify and initialize fxos_handleice: Hybrid mode with ODR=50Hz, Mag OSR=32, Acc OSR=Normal.
 *
 * @param fxos_handle The pointer to accel driver handle.
 * @param config  The configuration structure pointer to accel.
 *
 * @return kStatus_Success if success or kStatus_Fail if error.
 */
status_t FXOS_Init(fxos_handle_t *fxos_handle, fxos_config_t *config);

/*!
 * @brief Read data from sensors, assumes hyb_autoinc_mode is set in M_CTRL_REG2
 *
 * @param fxos_handle The pointer to accel driver handle.
 * @param sensorData The pointer to the buffer to hold sensor data
 *
 * @return kStatus_Success if success or kStatus_Fail if error.
 */
status_t FXOS_ReadSensorData(fxos_handle_t *fxos_handle, fxos_data_t *sensorData);

/*!
 * @brief Write value to register of sensor.
 *
 * @param handle The pointer to fxos8700cq driver handle.
 * @param reg Register address.
 * @param val Data want to write.
 *
 * @return kStatus_Success if success or kStatus_Fail if error.
 */
status_t FXOS_WriteReg(fxos_handle_t *handle, uint8_t reg, uint8_t val);

/*!
 * @brief Read n bytes start at register from sensor.
 *
 * @param handle The pointer to fxos8700cq driver handle.
 * @param reg Register address.
 * @param val The pointer to address which store data.
 * @param bytesNumber Number of bytes receiver.
 *
 * @return kStatus_Success if success or kStatus_Fail if error.
 */
status_t FXOS_ReadReg(fxos_handle_t *handle, uint8_t reg, uint8_t *val, uint8_t bytesNumber);

/*!
 * @brief Get device accelerator resolution bits.
 *
 * @return accelerator resolution bits.
 */
static inline uint8_t FXOS_GetResolutionBits(void)
{
    return FXOS8700CQ_ACCEL_RESOLUTION_BITS;
}

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _FSL_FXOS_H_ */
