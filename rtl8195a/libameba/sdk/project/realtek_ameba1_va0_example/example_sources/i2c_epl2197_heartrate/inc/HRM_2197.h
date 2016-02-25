/*******************************************************************************
 HRM.h - Definition header
 *******************************************************************************/
#ifndef HRM_H
#define HRM_H

#include <stdint.h>

//------------------------------------------------------
#define HR_SAMPLE_RATE   25// Hz
#define HR_INTEG_MIN   	  HR_INTEG_40
#define HR_INTEG_BASE  	  HR_INTEG_250
#define HR_INTEG_MAX   	  HR_INTEG_250

#define HR_TH_HIGH		  63000
#define HR_TH_LOW		  30000
//------------------------------------------------------

// HRM I2C address & register sub-addresses
#define HR_SLAVE_ADDRESS  0x82

#define HR_FILTER_1		  0<<5
#define HR_FILTER_2		  1<<5
#define HR_FILTER_4		  2<<5
#define HR_FILTER_8		  3<<5
#define HR_FILTER_16	  4<<5
#define HR_FILTER_32	  5<<5
#define HR_FILTER_64	  6<<5
#define HR_FILTER_128	  7<<5

#define HR_MODE_HR          1<<4
#define HR_MODE_HRS         9<<4

#define HR_GAIN_MID		  1
#define HR_GAIN_LOW	  3

#define HR_INTEG_20		  5
#define HR_INTEG_25		  6
#define HR_INTEG_30		  7
#define HR_INTEG_40		  8
#define HR_INTEG_55		  9
#define HR_INTEG_70		  10
#define HR_INTEG_90		  11
#define HR_INTEG_110	  12
#define HR_INTEG_150	  13
#define HR_INTEG_200	  14
#define HR_INTEG_250	  15
#define HR_INTEG_350	  16
#define HR_INTEG_450	  17
#define HR_INTEG_550	  18

#define HR_OSR_64              0<<2
#define HR_OSR_256		  1<<2
#define HR_OSR_1024		  2<<2
#define HR_OSR_2048		  3<<2

#define HR_RESETN_RESET  0<<2
#define HR_RESETN_RUN	  1<<2

#define HR_PDRIVE_70MA    0<<4
#define HR_PDRIVE_35MA	  1<<4
#define HR_PDRIVE_200MA  2<<4
#define HR_PDRIVE_100MA  3<<4

#define HR_INT_FRAME	  1<<2
#define HR_INT_DISABLED  2<<2

#define HR_IR_DISABLE      0<<7
#define HR_IR_ENABLE       1<<7

//------------------------------------------------------

// Declarations
void init_hrm(void);
uint16_t read_hrm(void);

#endif /* HRM_H */
