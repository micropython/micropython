/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _RTL8195A_ADC_H_
#define _RTL8195A_ADC_H_


//================ Register Bit Field ==========================
//2 REG_ADC_FIFO_READ

#define BIT_SHIFT_ADC_FIFO_RO 0
#define BIT_MASK_ADC_FIFO_RO 0xffffffffL
#define BIT_ADC_FIFO_RO(x)                    	(((x) & BIT_MASK_ADC_FIFO_RO) << BIT_SHIFT_ADC_FIFO_RO)
#define BIT_CTRL_ADC_FIFO_RO(x)               	(((x) & BIT_MASK_ADC_FIFO_RO) << BIT_SHIFT_ADC_FIFO_RO)
#define BIT_GET_ADC_FIFO_RO(x)                	(((x) >> BIT_SHIFT_ADC_FIFO_RO) & BIT_MASK_ADC_FIFO_RO)


//2 REG_ADC_CONTROL

#define BIT_SHIFT_ADC_DBG_SEL 24
#define BIT_MASK_ADC_DBG_SEL 0x7
#define BIT_ADC_DBG_SEL(x)                    	(((x) & BIT_MASK_ADC_DBG_SEL) << BIT_SHIFT_ADC_DBG_SEL)
#define BIT_CTRL_ADC_DBG_SEL(x)               	(((x) & BIT_MASK_ADC_DBG_SEL) << BIT_SHIFT_ADC_DBG_SEL)
#define BIT_GET_ADC_DBG_SEL(x)                	(((x) >> BIT_SHIFT_ADC_DBG_SEL) & BIT_MASK_ADC_DBG_SEL)


#define BIT_SHIFT_ADC_THRESHOLD 16
#define BIT_MASK_ADC_THRESHOLD 0x3f
#define BIT_ADC_THRESHOLD(x)                  	(((x) & BIT_MASK_ADC_THRESHOLD) << BIT_SHIFT_ADC_THRESHOLD)
#define BIT_CTRL_ADC_THRESHOLD(x)             	(((x) & BIT_MASK_ADC_THRESHOLD) << BIT_SHIFT_ADC_THRESHOLD)
#define BIT_GET_ADC_THRESHOLD(x)              	(((x) >> BIT_SHIFT_ADC_THRESHOLD) & BIT_MASK_ADC_THRESHOLD)


#define BIT_SHIFT_ADC_BURST_SIZE 8
#define BIT_MASK_ADC_BURST_SIZE 0x1f
#define BIT_ADC_BURST_SIZE(x)                 	(((x) & BIT_MASK_ADC_BURST_SIZE) << BIT_SHIFT_ADC_BURST_SIZE)
#define BIT_CTRL_ADC_BURST_SIZE(x)            	(((x) & BIT_MASK_ADC_BURST_SIZE) << BIT_SHIFT_ADC_BURST_SIZE)
#define BIT_GET_ADC_BURST_SIZE(x)             	(((x) >> BIT_SHIFT_ADC_BURST_SIZE) & BIT_MASK_ADC_BURST_SIZE)

#define BIT_ADC_ENDIAN                        	BIT(3)
#define BIT_SHIFT_ADC_ENDIAN                  	3
#define BIT_MASK_ADC_ENDIAN                   	0x1
#define BIT_CTRL_ADC_ENDIAN(x)                	(((x) & BIT_MASK_ADC_ENDIAN) << BIT_SHIFT_ADC_ENDIAN)

#define BIT_ADC_OVERWRITE                     	BIT(2)
#define BIT_SHIFT_ADC_OVERWRITE               	2
#define BIT_MASK_ADC_OVERWRITE                	0x1
#define BIT_CTRL_ADC_OVERWRITE(x)             	(((x) & BIT_MASK_ADC_OVERWRITE) << BIT_SHIFT_ADC_OVERWRITE)

#define BIT_ADC_ONESHOT                       	BIT(1)
#define BIT_SHIFT_ADC_ONESHOT                 	1
#define BIT_MASK_ADC_ONESHOT                  	0x1
#define BIT_CTRL_ADC_ONESHOT(x)               	(((x) & BIT_MASK_ADC_ONESHOT) << BIT_SHIFT_ADC_ONESHOT)

#define BIT_ADC_COMP_ONLY                     	BIT(0)
#define BIT_SHIFT_ADC_COMP_ONLY               	0
#define BIT_MASK_ADC_COMP_ONLY                	0x1
#define BIT_CTRL_ADC_COMP_ONLY(x)             	(((x) & BIT_MASK_ADC_COMP_ONLY) << BIT_SHIFT_ADC_COMP_ONLY)


//2 REG_ADC_INTR_EN
#define BIT_ADC_AWAKE_CPU_EN                  	BIT(7)
#define BIT_SHIFT_ADC_AWAKE_CPU_EN            	7
#define BIT_MASK_ADC_AWAKE_CPU_EN             	0x1
#define BIT_CTRL_ADC_AWAKE_CPU_EN(x)          	(((x) & BIT_MASK_ADC_AWAKE_CPU_EN) << BIT_SHIFT_ADC_AWAKE_CPU_EN)

#define BIT_ADC_FIFO_RD_ERROR_EN              	BIT(6)
#define BIT_SHIFT_ADC_FIFO_RD_ERROR_EN        	6
#define BIT_MASK_ADC_FIFO_RD_ERROR_EN         	0x1
#define BIT_CTRL_ADC_FIFO_RD_ERROR_EN(x)      	(((x) & BIT_MASK_ADC_FIFO_RD_ERROR_EN) << BIT_SHIFT_ADC_FIFO_RD_ERROR_EN)

#define BIT_ADC_FIFO_RD_REQ_EN                	BIT(5)
#define BIT_SHIFT_ADC_FIFO_RD_REQ_EN          	5
#define BIT_MASK_ADC_FIFO_RD_REQ_EN           	0x1
#define BIT_CTRL_ADC_FIFO_RD_REQ_EN(x)        	(((x) & BIT_MASK_ADC_FIFO_RD_REQ_EN) << BIT_SHIFT_ADC_FIFO_RD_REQ_EN)

#define BIT_ADC_FIFO_FULL_EN                  	BIT(4)
#define BIT_SHIFT_ADC_FIFO_FULL_EN            	4
#define BIT_MASK_ADC_FIFO_FULL_EN             	0x1
#define BIT_CTRL_ADC_FIFO_FULL_EN(x)          	(((x) & BIT_MASK_ADC_FIFO_FULL_EN) << BIT_SHIFT_ADC_FIFO_FULL_EN)

#define BIT_ADC_COMP_3_EN                     	BIT(3)
#define BIT_SHIFT_ADC_COMP_3_EN               	3
#define BIT_MASK_ADC_COMP_3_EN                	0x1
#define BIT_CTRL_ADC_COMP_3_EN(x)             	(((x) & BIT_MASK_ADC_COMP_3_EN) << BIT_SHIFT_ADC_COMP_3_EN)

#define BIT_ADC_COMP_2_EN                     	BIT(2)
#define BIT_SHIFT_ADC_COMP_2_EN               	2
#define BIT_MASK_ADC_COMP_2_EN                	0x1
#define BIT_CTRL_ADC_COMP_2_EN(x)             	(((x) & BIT_MASK_ADC_COMP_2_EN) << BIT_SHIFT_ADC_COMP_2_EN)

#define BIT_ADC_COMP_1_EN                     	BIT(1)
#define BIT_SHIFT_ADC_COMP_1_EN               	1
#define BIT_MASK_ADC_COMP_1_EN                	0x1
#define BIT_CTRL_ADC_COMP_1_EN(x)             	(((x) & BIT_MASK_ADC_COMP_1_EN) << BIT_SHIFT_ADC_COMP_1_EN)

#define BIT_ADC_COMP_0_EN                     	BIT(0)
#define BIT_SHIFT_ADC_COMP_0_EN               	0
#define BIT_MASK_ADC_COMP_0_EN                	0x1
#define BIT_CTRL_ADC_COMP_0_EN(x)             	(((x) & BIT_MASK_ADC_COMP_0_EN) << BIT_SHIFT_ADC_COMP_0_EN)


//2 REG_ADC_INTR_STS
#define BIT_ADC_FIFO_THRESHOLD                	BIT(7)
#define BIT_SHIFT_ADC_FIFO_THRESHOLD          	7
#define BIT_MASK_ADC_FIFO_THRESHOLD           	0x1
#define BIT_CTRL_ADC_FIFO_THRESHOLD(x)        	(((x) & BIT_MASK_ADC_FIFO_THRESHOLD) << BIT_SHIFT_ADC_FIFO_THRESHOLD)

#define BIT_ADC_FIFO_RD_ERROR_ST              	BIT(6)
#define BIT_SHIFT_ADC_FIFO_RD_ERROR_ST        	6
#define BIT_MASK_ADC_FIFO_RD_ERROR_ST         	0x1
#define BIT_CTRL_ADC_FIFO_RD_ERROR_ST(x)      	(((x) & BIT_MASK_ADC_FIFO_RD_ERROR_ST) << BIT_SHIFT_ADC_FIFO_RD_ERROR_ST)

#define BIT_ADC_FIFO_RD_REQ_ST                	BIT(5)
#define BIT_SHIFT_ADC_FIFO_RD_REQ_ST          	5
#define BIT_MASK_ADC_FIFO_RD_REQ_ST           	0x1
#define BIT_CTRL_ADC_FIFO_RD_REQ_ST(x)        	(((x) & BIT_MASK_ADC_FIFO_RD_REQ_ST) << BIT_SHIFT_ADC_FIFO_RD_REQ_ST)

#define BIT_ADC_FIFO_FULL_ST                  	BIT(4)
#define BIT_SHIFT_ADC_FIFO_FULL_ST            	4
#define BIT_MASK_ADC_FIFO_FULL_ST             	0x1
#define BIT_CTRL_ADC_FIFO_FULL_ST(x)          	(((x) & BIT_MASK_ADC_FIFO_FULL_ST) << BIT_SHIFT_ADC_FIFO_FULL_ST)

#define BIT_ADC_COMP_3_ST                     	BIT(3)
#define BIT_SHIFT_ADC_COMP_3_ST               	3
#define BIT_MASK_ADC_COMP_3_ST                	0x1
#define BIT_CTRL_ADC_COMP_3_ST(x)             	(((x) & BIT_MASK_ADC_COMP_3_ST) << BIT_SHIFT_ADC_COMP_3_ST)

#define BIT_ADC_COMP_2_ST                     	BIT(2)
#define BIT_SHIFT_ADC_COMP_2_ST               	2
#define BIT_MASK_ADC_COMP_2_ST                	0x1
#define BIT_CTRL_ADC_COMP_2_ST(x)             	(((x) & BIT_MASK_ADC_COMP_2_ST) << BIT_SHIFT_ADC_COMP_2_ST)

#define BIT_ADC_COMP_1_ST                     	BIT(1)
#define BIT_SHIFT_ADC_COMP_1_ST               	1
#define BIT_MASK_ADC_COMP_1_ST                	0x1
#define BIT_CTRL_ADC_COMP_1_ST(x)             	(((x) & BIT_MASK_ADC_COMP_1_ST) << BIT_SHIFT_ADC_COMP_1_ST)

#define BIT_ADC_COMP_0_ST                     	BIT(0)
#define BIT_SHIFT_ADC_COMP_0_ST               	0
#define BIT_MASK_ADC_COMP_0_ST                	0x1
#define BIT_CTRL_ADC_COMP_0_ST(x)             	(((x) & BIT_MASK_ADC_COMP_0_ST) << BIT_SHIFT_ADC_COMP_0_ST)


//2 REG_ADC_COMP_VALUE_L

#define BIT_SHIFT_ADC_COMP_TH_1 16
#define BIT_MASK_ADC_COMP_TH_1 0xffff
#define BIT_ADC_COMP_TH_1(x)                  	(((x) & BIT_MASK_ADC_COMP_TH_1) << BIT_SHIFT_ADC_COMP_TH_1)
#define BIT_CTRL_ADC_COMP_TH_1(x)             	(((x) & BIT_MASK_ADC_COMP_TH_1) << BIT_SHIFT_ADC_COMP_TH_1)
#define BIT_GET_ADC_COMP_TH_1(x)              	(((x) >> BIT_SHIFT_ADC_COMP_TH_1) & BIT_MASK_ADC_COMP_TH_1)


#define BIT_SHIFT_ADC_COMP_TH_0 0
#define BIT_MASK_ADC_COMP_TH_0 0xffff
#define BIT_ADC_COMP_TH_0(x)                  	(((x) & BIT_MASK_ADC_COMP_TH_0) << BIT_SHIFT_ADC_COMP_TH_0)
#define BIT_CTRL_ADC_COMP_TH_0(x)             	(((x) & BIT_MASK_ADC_COMP_TH_0) << BIT_SHIFT_ADC_COMP_TH_0)
#define BIT_GET_ADC_COMP_TH_0(x)              	(((x) >> BIT_SHIFT_ADC_COMP_TH_0) & BIT_MASK_ADC_COMP_TH_0)


//2 REG_ADC_COMP_VALUE_H

#define BIT_SHIFT_ADC_COMP_TH_3 16
#define BIT_MASK_ADC_COMP_TH_3 0xffff
#define BIT_ADC_COMP_TH_3(x)                  	(((x) & BIT_MASK_ADC_COMP_TH_3) << BIT_SHIFT_ADC_COMP_TH_3)
#define BIT_CTRL_ADC_COMP_TH_3(x)             	(((x) & BIT_MASK_ADC_COMP_TH_3) << BIT_SHIFT_ADC_COMP_TH_3)
#define BIT_GET_ADC_COMP_TH_3(x)              	(((x) >> BIT_SHIFT_ADC_COMP_TH_3) & BIT_MASK_ADC_COMP_TH_3)


#define BIT_SHIFT_ADC_COMP_TH_2 0
#define BIT_MASK_ADC_COMP_TH_2 0xffff
#define BIT_ADC_COMP_TH_2(x)                  	(((x) & BIT_MASK_ADC_COMP_TH_2) << BIT_SHIFT_ADC_COMP_TH_2)
#define BIT_CTRL_ADC_COMP_TH_2(x)             	(((x) & BIT_MASK_ADC_COMP_TH_2) << BIT_SHIFT_ADC_COMP_TH_2)
#define BIT_GET_ADC_COMP_TH_2(x)              	(((x) >> BIT_SHIFT_ADC_COMP_TH_2) & BIT_MASK_ADC_COMP_TH_2)


//2 REG_ADC_COMP_SET

#define BIT_SHIFT_ADC_GREATER_THAN 0
#define BIT_MASK_ADC_GREATER_THAN 0xf
#define BIT_ADC_GREATER_THAN(x)               	(((x) & BIT_MASK_ADC_GREATER_THAN) << BIT_SHIFT_ADC_GREATER_THAN)
#define BIT_CTRL_ADC_GREATER_THAN(x)          	(((x) & BIT_MASK_ADC_GREATER_THAN) << BIT_SHIFT_ADC_GREATER_THAN)
#define BIT_GET_ADC_GREATER_THAN(x)           	(((x) >> BIT_SHIFT_ADC_GREATER_THAN) & BIT_MASK_ADC_GREATER_THAN)


//2 REG_ADC_POWER

#define BIT_SHIFT_ADC_PWR_CUT_CNTR 16
#define BIT_MASK_ADC_PWR_CUT_CNTR 0xff
#define BIT_ADC_PWR_CUT_CNTR(x)               	(((x) & BIT_MASK_ADC_PWR_CUT_CNTR) << BIT_SHIFT_ADC_PWR_CUT_CNTR)
#define BIT_CTRL_ADC_PWR_CUT_CNTR(x)          	(((x) & BIT_MASK_ADC_PWR_CUT_CNTR) << BIT_SHIFT_ADC_PWR_CUT_CNTR)
#define BIT_GET_ADC_PWR_CUT_CNTR(x)           	(((x) >> BIT_SHIFT_ADC_PWR_CUT_CNTR) & BIT_MASK_ADC_PWR_CUT_CNTR)

#define BIT_ADC_FIFO_ON_ST                    	BIT(11)
#define BIT_SHIFT_ADC_FIFO_ON_ST              	11
#define BIT_MASK_ADC_FIFO_ON_ST               	0x1
#define BIT_CTRL_ADC_FIFO_ON_ST(x)            	(((x) & BIT_MASK_ADC_FIFO_ON_ST) << BIT_SHIFT_ADC_FIFO_ON_ST)

#define BIT_ADC_ISO_ON_ST                     	BIT(10)
#define BIT_SHIFT_ADC_ISO_ON_ST               	10
#define BIT_MASK_ADC_ISO_ON_ST                	0x1
#define BIT_CTRL_ADC_ISO_ON_ST(x)             	(((x) & BIT_MASK_ADC_ISO_ON_ST) << BIT_SHIFT_ADC_ISO_ON_ST)

#define BIT_ADC_PWR33_ON_ST                   	BIT(9)
#define BIT_SHIFT_ADC_PWR33_ON_ST             	9
#define BIT_MASK_ADC_PWR33_ON_ST              	0x1
#define BIT_CTRL_ADC_PWR33_ON_ST(x)           	(((x) & BIT_MASK_ADC_PWR33_ON_ST) << BIT_SHIFT_ADC_PWR33_ON_ST)

#define BIT_ADC_PWR12_ON_ST                   	BIT(8)
#define BIT_SHIFT_ADC_PWR12_ON_ST             	8
#define BIT_MASK_ADC_PWR12_ON_ST              	0x1
#define BIT_CTRL_ADC_PWR12_ON_ST(x)           	(((x) & BIT_MASK_ADC_PWR12_ON_ST) << BIT_SHIFT_ADC_PWR12_ON_ST)

#define BIT_ADC_ISO_MANUAL                    	BIT(3)
#define BIT_SHIFT_ADC_ISO_MANUAL              	3
#define BIT_MASK_ADC_ISO_MANUAL               	0x1
#define BIT_CTRL_ADC_ISO_MANUAL(x)            	(((x) & BIT_MASK_ADC_ISO_MANUAL) << BIT_SHIFT_ADC_ISO_MANUAL)

#define BIT_ADC_PWR33_MANUAL                  	BIT(2)
#define BIT_SHIFT_ADC_PWR33_MANUAL            	2
#define BIT_MASK_ADC_PWR33_MANUAL             	0x1
#define BIT_CTRL_ADC_PWR33_MANUAL(x)          	(((x) & BIT_MASK_ADC_PWR33_MANUAL) << BIT_SHIFT_ADC_PWR33_MANUAL)

#define BIT_ADC_PWR12_MANUAL                  	BIT(1)
#define BIT_SHIFT_ADC_PWR12_MANUAL            	1
#define BIT_MASK_ADC_PWR12_MANUAL             	0x1
#define BIT_CTRL_ADC_PWR12_MANUAL(x)          	(((x) & BIT_MASK_ADC_PWR12_MANUAL) << BIT_SHIFT_ADC_PWR12_MANUAL)

#define BIT_ADC_PWR_AUTO                      	BIT(0)
#define BIT_SHIFT_ADC_PWR_AUTO                	0
#define BIT_MASK_ADC_PWR_AUTO                 	0x1
#define BIT_CTRL_ADC_PWR_AUTO(x)              	(((x) & BIT_MASK_ADC_PWR_AUTO) << BIT_SHIFT_ADC_PWR_AUTO)


//2 REG_ADC_ANAPAR_AD0

#define BIT_SHIFT_ADC_ANAPAR_AD0 2
#define BIT_MASK_ADC_ANAPAR_AD0 0x3fffffff
#define BIT_ADC_ANAPAR_AD0(x)                 	(((x) & BIT_MASK_ADC_ANAPAR_AD0) << BIT_SHIFT_ADC_ANAPAR_AD0)
#define BIT_CTRL_ADC_ANAPAR_AD0(x)            	(((x) & BIT_MASK_ADC_ANAPAR_AD0) << BIT_SHIFT_ADC_ANAPAR_AD0)
#define BIT_GET_ADC_ANAPAR_AD0(x)             	(((x) >> BIT_SHIFT_ADC_ANAPAR_AD0) & BIT_MASK_ADC_ANAPAR_AD0)

#define BIT_ADC_AUDIO_EN                      	BIT(1)
#define BIT_SHIFT_ADC_AUDIO_EN                	1
#define BIT_MASK_ADC_AUDIO_EN                 	0x1
#define BIT_CTRL_ADC_AUDIO_EN(x)              	(((x) & BIT_MASK_ADC_AUDIO_EN) << BIT_SHIFT_ADC_AUDIO_EN)

#define BIT_ADC_EN_MANUAL                     	BIT(0)
#define BIT_SHIFT_ADC_EN_MANUAL               	0
#define BIT_MASK_ADC_EN_MANUAL                	0x1
#define BIT_CTRL_ADC_EN_MANUAL(x)             	(((x) & BIT_MASK_ADC_EN_MANUAL) << BIT_SHIFT_ADC_EN_MANUAL)


//2 REG_ADC_ANAPAR_AD1

#define BIT_SHIFT_ADC_ANAPAR_AD1 0
#define BIT_MASK_ADC_ANAPAR_AD1 0xffffffffL
#define BIT_ADC_ANAPAR_AD1(x)                 	(((x) & BIT_MASK_ADC_ANAPAR_AD1) << BIT_SHIFT_ADC_ANAPAR_AD1)
#define BIT_CTRL_ADC_ANAPAR_AD1(x)            	(((x) & BIT_MASK_ADC_ANAPAR_AD1) << BIT_SHIFT_ADC_ANAPAR_AD1)
#define BIT_GET_ADC_ANAPAR_AD1(x)             	(((x) >> BIT_SHIFT_ADC_ANAPAR_AD1) & BIT_MASK_ADC_ANAPAR_AD1)


//2 REG_ADC_ANAPAR_AD2

#define BIT_SHIFT_ADC_ANAPAR_AD2 0
#define BIT_MASK_ADC_ANAPAR_AD2 0xffffffffL
#define BIT_ADC_ANAPAR_AD2(x)                 	(((x) & BIT_MASK_ADC_ANAPAR_AD2) << BIT_SHIFT_ADC_ANAPAR_AD2)
#define BIT_CTRL_ADC_ANAPAR_AD2(x)            	(((x) & BIT_MASK_ADC_ANAPAR_AD2) << BIT_SHIFT_ADC_ANAPAR_AD2)
#define BIT_GET_ADC_ANAPAR_AD2(x)             	(((x) >> BIT_SHIFT_ADC_ANAPAR_AD2) & BIT_MASK_ADC_ANAPAR_AD2)


//2 REG_ADC_ANAPAR_AD3

#define BIT_SHIFT_ADC_ANAPAR_AD3 0
#define BIT_MASK_ADC_ANAPAR_AD3 0xffffffffL
#define BIT_ADC_ANAPAR_AD3(x)                 	(((x) & BIT_MASK_ADC_ANAPAR_AD3) << BIT_SHIFT_ADC_ANAPAR_AD3)
#define BIT_CTRL_ADC_ANAPAR_AD3(x)            	(((x) & BIT_MASK_ADC_ANAPAR_AD3) << BIT_SHIFT_ADC_ANAPAR_AD3)
#define BIT_GET_ADC_ANAPAR_AD3(x)             	(((x) >> BIT_SHIFT_ADC_ANAPAR_AD3) & BIT_MASK_ADC_ANAPAR_AD3)


//2 REG_ADC_ANAPAR_AD4

#define BIT_SHIFT_ADC_ANAPAR_AD4 0
#define BIT_MASK_ADC_ANAPAR_AD4 0xffffffffL
#define BIT_ADC_ANAPAR_AD4(x)                 	(((x) & BIT_MASK_ADC_ANAPAR_AD4) << BIT_SHIFT_ADC_ANAPAR_AD4)
#define BIT_CTRL_ADC_ANAPAR_AD4(x)            	(((x) & BIT_MASK_ADC_ANAPAR_AD4) << BIT_SHIFT_ADC_ANAPAR_AD4)
#define BIT_GET_ADC_ANAPAR_AD4(x)             	(((x) >> BIT_SHIFT_ADC_ANAPAR_AD4) & BIT_MASK_ADC_ANAPAR_AD4)


//2 REG_ADC_ANAPAR_AD5

#define BIT_SHIFT_ADC_ANAPAR_AD5 0
#define BIT_MASK_ADC_ANAPAR_AD5 0xffffffffL
#define BIT_ADC_ANAPAR_AD5(x)                 	(((x) & BIT_MASK_ADC_ANAPAR_AD5) << BIT_SHIFT_ADC_ANAPAR_AD5)
#define BIT_CTRL_ADC_ANAPAR_AD5(x)            	(((x) & BIT_MASK_ADC_ANAPAR_AD5) << BIT_SHIFT_ADC_ANAPAR_AD5)
#define BIT_GET_ADC_ANAPAR_AD5(x)             	(((x) >> BIT_SHIFT_ADC_ANAPAR_AD5) & BIT_MASK_ADC_ANAPAR_AD5)


//2 REG_ADC_CALI_DATA

#define BIT_SHIFT_ADC_CALI_DATA_6 16
#define BIT_MASK_ADC_CALI_DATA_6 0xffff
#define BIT_ADC_CALI_DATA_6(x)                	(((x) & BIT_MASK_ADC_CALI_DATA_6) << BIT_SHIFT_ADC_CALI_DATA_6)
#define BIT_CTRL_ADC_CALI_DATA_6(x)           	(((x) & BIT_MASK_ADC_CALI_DATA_6) << BIT_SHIFT_ADC_CALI_DATA_6)
#define BIT_GET_ADC_CALI_DATA_6(x)            	(((x) >> BIT_SHIFT_ADC_CALI_DATA_6) & BIT_MASK_ADC_CALI_DATA_6)


#define BIT_SHIFT_ADC_CALI_DATA_0 0
#define BIT_MASK_ADC_CALI_DATA_0 0xffff
#define BIT_ADC_CALI_DATA_0(x)                	(((x) & BIT_MASK_ADC_CALI_DATA_0) << BIT_SHIFT_ADC_CALI_DATA_0)
#define BIT_CTRL_ADC_CALI_DATA_0(x)           	(((x) & BIT_MASK_ADC_CALI_DATA_0) << BIT_SHIFT_ADC_CALI_DATA_0)
#define BIT_GET_ADC_CALI_DATA_0(x)            	(((x) >> BIT_SHIFT_ADC_CALI_DATA_0) & BIT_MASK_ADC_CALI_DATA_0)

//================ Register Reg Field  =========================
#define REG_ADC_FIFO_READ 0x0000
#define REG_ADC_CONTROL 0x0004
#define REG_ADC_INTR_EN 0x0008
#define REG_ADC_INTR_STS 0x000C
#define REG_ADC_COMP_VALUE_L 0x0010
#define REG_ADC_COMP_VALUE_H 0x0014
#define REG_ADC_COMP_SET 0x0018
#define REG_ADC_POWER 0x001C
#define REG_ADC_ANAPAR_AD0 0x0020
#define REG_ADC_ANAPAR_AD1 0x0024
#define REG_ADC_ANAPAR_AD2 0x0028
#define REG_ADC_ANAPAR_AD3 0x002C
#define REG_ADC_ANAPAR_AD4 0x0030
#define REG_ADC_ANAPAR_AD5 0x0034
#define REG_ADC_CALI_DATA 0x0038

//================ ADC HAL related enumeration ==================

//================ ADC Function Prototypes  =====================
#define HAL_ADC_WRITE32(addr, value)    HAL_WRITE32(ADC_REG_BASE,addr,value)
#define HAL_ADC_READ32(addr)            HAL_READ32(ADC_REG_BASE,addr)

RTK_STATUS HalADCInit8195a(IN VOID *Data);
RTK_STATUS HalADCDeInit8195a(IN VOID *Data);
RTK_STATUS HalADCEnableRtl8195a(IN VOID *Data);
RTK_STATUS HalADCIntrCtrl8195a(IN VOID *Data);
u32 HalADCReceiveRtl8195a(IN VOID *Data);
u32 HalADCReadRegRtl8195a(IN VOID *Data,IN  u8 I2CReg);

#endif
