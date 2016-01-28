#ifndef _RTL8195A_DAC_H_
#define _RTL8195A_DAC_H_

//================ Register Bit Field ==========================
//2 REG_DAC0_FIFO_WR

#define BIT_SHIFT_DAC0_FIFO_WO 0
#define BIT_MASK_DAC0_FIFO_WO 0xffffffffL
#define BIT_DAC0_FIFO_WO(x)                   	(((x) & BIT_MASK_DAC0_FIFO_WO) << BIT_SHIFT_DAC0_FIFO_WO)
#define BIT_CTRL_DAC0_FIFO_WO(x)              	(((x) & BIT_MASK_DAC0_FIFO_WO) << BIT_SHIFT_DAC0_FIFO_WO)
#define BIT_GET_DAC0_FIFO_WO(x)               	(((x) >> BIT_SHIFT_DAC0_FIFO_WO) & BIT_MASK_DAC0_FIFO_WO)


//2 REG_DAC_CTRL

#define BIT_SHIFT_DAC_DELTA_SIGMA 25
#define BIT_MASK_DAC_DELTA_SIGMA 0x7
#define BIT_DAC_DELTA_SIGMA(x)                	(((x) & BIT_MASK_DAC_DELTA_SIGMA) << BIT_SHIFT_DAC_DELTA_SIGMA)
#define BIT_CTRL_DAC_DELTA_SIGMA(x)           	(((x) & BIT_MASK_DAC_DELTA_SIGMA) << BIT_SHIFT_DAC_DELTA_SIGMA)
#define BIT_GET_DAC_DELTA_SIGMA(x)            	(((x) >> BIT_SHIFT_DAC_DELTA_SIGMA) & BIT_MASK_DAC_DELTA_SIGMA)

#define BIT_DAC_BYPASS_DSC                    	BIT(24)
#define BIT_SHIFT_DAC_BYPASS_DSC              	24
#define BIT_MASK_DAC_BYPASS_DSC               	0x1
#define BIT_CTRL_DAC_BYPASS_DSC(x)            	(((x) & BIT_MASK_DAC_BYPASS_DSC) << BIT_SHIFT_DAC_BYPASS_DSC)


#define BIT_SHIFT_DAC_DSC_DBG_SEL 19
#define BIT_MASK_DAC_DSC_DBG_SEL 0x3
#define BIT_DAC_DSC_DBG_SEL(x)                	(((x) & BIT_MASK_DAC_DSC_DBG_SEL) << BIT_SHIFT_DAC_DSC_DBG_SEL)
#define BIT_CTRL_DAC_DSC_DBG_SEL(x)           	(((x) & BIT_MASK_DAC_DSC_DBG_SEL) << BIT_SHIFT_DAC_DSC_DBG_SEL)
#define BIT_GET_DAC_DSC_DBG_SEL(x)            	(((x) >> BIT_SHIFT_DAC_DSC_DBG_SEL) & BIT_MASK_DAC_DSC_DBG_SEL)


#define BIT_SHIFT_DAC_DBG_SEL 16
#define BIT_MASK_DAC_DBG_SEL 0x7
#define BIT_DAC_DBG_SEL(x)                    	(((x) & BIT_MASK_DAC_DBG_SEL) << BIT_SHIFT_DAC_DBG_SEL)
#define BIT_CTRL_DAC_DBG_SEL(x)               	(((x) & BIT_MASK_DAC_DBG_SEL) << BIT_SHIFT_DAC_DBG_SEL)
#define BIT_GET_DAC_DBG_SEL(x)                	(((x) >> BIT_SHIFT_DAC_DBG_SEL) & BIT_MASK_DAC_DBG_SEL)


#define BIT_SHIFT_DAC_BURST_SIZE 8
#define BIT_MASK_DAC_BURST_SIZE 0xf
#define BIT_DAC_BURST_SIZE(x)                 	(((x) & BIT_MASK_DAC_BURST_SIZE) << BIT_SHIFT_DAC_BURST_SIZE)
#define BIT_CTRL_DAC_BURST_SIZE(x)            	(((x) & BIT_MASK_DAC_BURST_SIZE) << BIT_SHIFT_DAC_BURST_SIZE)
#define BIT_GET_DAC_BURST_SIZE(x)             	(((x) >> BIT_SHIFT_DAC_BURST_SIZE) & BIT_MASK_DAC_BURST_SIZE)

#define BIT_DAC_FILTER_SETTLE                 	BIT(4)
#define BIT_SHIFT_DAC_FILTER_SETTLE           	4
#define BIT_MASK_DAC_FILTER_SETTLE            	0x1
#define BIT_CTRL_DAC_FILTER_SETTLE(x)         	(((x) & BIT_MASK_DAC_FILTER_SETTLE) << BIT_SHIFT_DAC_FILTER_SETTLE)

#define BIT_DAC_OV_OPTION                     	BIT(3)
#define BIT_SHIFT_DAC_OV_OPTION               	3
#define BIT_MASK_DAC_OV_OPTION                	0x1
#define BIT_CTRL_DAC_OV_OPTION(x)             	(((x) & BIT_MASK_DAC_OV_OPTION) << BIT_SHIFT_DAC_OV_OPTION)

#define BIT_DAC_ENDIAN                        	BIT(2)
#define BIT_SHIFT_DAC_ENDIAN                  	2
#define BIT_MASK_DAC_ENDIAN                   	0x1
#define BIT_CTRL_DAC_ENDIAN(x)                	(((x) & BIT_MASK_DAC_ENDIAN) << BIT_SHIFT_DAC_ENDIAN)

#define BIT_DAC_SPEED                         	BIT(1)
#define BIT_SHIFT_DAC_SPEED                   	1
#define BIT_MASK_DAC_SPEED                    	0x1
#define BIT_CTRL_DAC_SPEED(x)                 	(((x) & BIT_MASK_DAC_SPEED) << BIT_SHIFT_DAC_SPEED)

#define BIT_DAC_FIFO_EN                       	BIT(0)
#define BIT_SHIFT_DAC_FIFO_EN                 	0
#define BIT_MASK_DAC_FIFO_EN                  	0x1
#define BIT_CTRL_DAC_FIFO_EN(x)               	(((x) & BIT_MASK_DAC_FIFO_EN) << BIT_SHIFT_DAC_FIFO_EN)


//2 REG_DAC_INTR_CTRL
#define BIT_DAC_DSC_OVERFLOW1_EN              	BIT(6)
#define BIT_SHIFT_DAC_DSC_OVERFLOW1_EN        	6
#define BIT_MASK_DAC_DSC_OVERFLOW1_EN         	0x1
#define BIT_CTRL_DAC_DSC_OVERFLOW1_EN(x)      	(((x) & BIT_MASK_DAC_DSC_OVERFLOW1_EN) << BIT_SHIFT_DAC_DSC_OVERFLOW1_EN)

#define BIT_DAC_DSC_OVERFLOW0_EN              	BIT(5)
#define BIT_SHIFT_DAC_DSC_OVERFLOW0_EN        	5
#define BIT_MASK_DAC_DSC_OVERFLOW0_EN         	0x1
#define BIT_CTRL_DAC_DSC_OVERFLOW0_EN(x)      	(((x) & BIT_MASK_DAC_DSC_OVERFLOW0_EN) << BIT_SHIFT_DAC_DSC_OVERFLOW0_EN)

#define BIT_DAC__WRITE_ERROR_EN               	BIT(4)
#define BIT_SHIFT_DAC__WRITE_ERROR_EN         	4
#define BIT_MASK_DAC__WRITE_ERROR_EN          	0x1
#define BIT_CTRL_DAC__WRITE_ERROR_EN(x)       	(((x) & BIT_MASK_DAC__WRITE_ERROR_EN) << BIT_SHIFT_DAC__WRITE_ERROR_EN)

#define BIT_DAC_FIFO_STOP_EN                  	BIT(3)
#define BIT_SHIFT_DAC_FIFO_STOP_EN            	3
#define BIT_MASK_DAC_FIFO_STOP_EN             	0x1
#define BIT_CTRL_DAC_FIFO_STOP_EN(x)          	(((x) & BIT_MASK_DAC_FIFO_STOP_EN) << BIT_SHIFT_DAC_FIFO_STOP_EN)

#define BIT_DAC_FIFO_OVERFLOW_EN              	BIT(2)
#define BIT_SHIFT_DAC_FIFO_OVERFLOW_EN        	2
#define BIT_MASK_DAC_FIFO_OVERFLOW_EN         	0x1
#define BIT_CTRL_DAC_FIFO_OVERFLOW_EN(x)      	(((x) & BIT_MASK_DAC_FIFO_OVERFLOW_EN) << BIT_SHIFT_DAC_FIFO_OVERFLOW_EN)

#define BIT_DAC_FIFO_WR_REQ_EN                	BIT(1)
#define BIT_SHIFT_DAC_FIFO_WR_REQ_EN          	1
#define BIT_MASK_DAC_FIFO_WR_REQ_EN           	0x1
#define BIT_CTRL_DAC_FIFO_WR_REQ_EN(x)        	(((x) & BIT_MASK_DAC_FIFO_WR_REQ_EN) << BIT_SHIFT_DAC_FIFO_WR_REQ_EN)

#define BIT_DAC_FIFO_FULL_EN                  	BIT(0)
#define BIT_SHIFT_DAC_FIFO_FULL_EN            	0
#define BIT_MASK_DAC_FIFO_FULL_EN             	0x1
#define BIT_CTRL_DAC_FIFO_FULL_EN(x)          	(((x) & BIT_MASK_DAC_FIFO_FULL_EN) << BIT_SHIFT_DAC_FIFO_FULL_EN)


//2 REG_DAC_INTR_STS
#define BIT_DAC_DSC_OVERFLOW1_ST              	BIT(6)
#define BIT_SHIFT_DAC_DSC_OVERFLOW1_ST        	6
#define BIT_MASK_DAC_DSC_OVERFLOW1_ST         	0x1
#define BIT_CTRL_DAC_DSC_OVERFLOW1_ST(x)      	(((x) & BIT_MASK_DAC_DSC_OVERFLOW1_ST) << BIT_SHIFT_DAC_DSC_OVERFLOW1_ST)

#define BIT_DAC_DSC_OVERFLOW0_ST              	BIT(5)
#define BIT_SHIFT_DAC_DSC_OVERFLOW0_ST        	5
#define BIT_MASK_DAC_DSC_OVERFLOW0_ST         	0x1
#define BIT_CTRL_DAC_DSC_OVERFLOW0_ST(x)      	(((x) & BIT_MASK_DAC_DSC_OVERFLOW0_ST) << BIT_SHIFT_DAC_DSC_OVERFLOW0_ST)

#define BIT_DAC__WRITE_ERROR_ST               	BIT(4)
#define BIT_SHIFT_DAC__WRITE_ERROR_ST         	4
#define BIT_MASK_DAC__WRITE_ERROR_ST          	0x1
#define BIT_CTRL_DAC__WRITE_ERROR_ST(x)       	(((x) & BIT_MASK_DAC__WRITE_ERROR_ST) << BIT_SHIFT_DAC__WRITE_ERROR_ST)

#define BIT_DAC_FIFO_STOP_ST                  	BIT(3)
#define BIT_SHIFT_DAC_FIFO_STOP_ST            	3
#define BIT_MASK_DAC_FIFO_STOP_ST             	0x1
#define BIT_CTRL_DAC_FIFO_STOP_ST(x)          	(((x) & BIT_MASK_DAC_FIFO_STOP_ST) << BIT_SHIFT_DAC_FIFO_STOP_ST)

#define BIT_DAC_FIFO_OVERFLOW_ST              	BIT(2)
#define BIT_SHIFT_DAC_FIFO_OVERFLOW_ST        	2
#define BIT_MASK_DAC_FIFO_OVERFLOW_ST         	0x1
#define BIT_CTRL_DAC_FIFO_OVERFLOW_ST(x)      	(((x) & BIT_MASK_DAC_FIFO_OVERFLOW_ST) << BIT_SHIFT_DAC_FIFO_OVERFLOW_ST)

#define BIT_DAC_FIFO_WR_REQ_ST                	BIT(1)
#define BIT_SHIFT_DAC_FIFO_WR_REQ_ST          	1
#define BIT_MASK_DAC_FIFO_WR_REQ_ST           	0x1
#define BIT_CTRL_DAC_FIFO_WR_REQ_ST(x)        	(((x) & BIT_MASK_DAC_FIFO_WR_REQ_ST) << BIT_SHIFT_DAC_FIFO_WR_REQ_ST)

#define BIT_DAC_FIFO_FULL_ST                  	BIT(0)
#define BIT_SHIFT_DAC_FIFO_FULL_ST            	0
#define BIT_MASK_DAC_FIFO_FULL_ST             	0x1
#define BIT_CTRL_DAC_FIFO_FULL_ST(x)          	(((x) & BIT_MASK_DAC_FIFO_FULL_ST) << BIT_SHIFT_DAC_FIFO_FULL_ST)


//2 REG_DAC_PWR_CTRL

#define BIT_SHIFT_DAC_PWR_CUT_CNTR 16
#define BIT_MASK_DAC_PWR_CUT_CNTR 0xff
#define BIT_DAC_PWR_CUT_CNTR(x)               	(((x) & BIT_MASK_DAC_PWR_CUT_CNTR) << BIT_SHIFT_DAC_PWR_CUT_CNTR)
#define BIT_CTRL_DAC_PWR_CUT_CNTR(x)          	(((x) & BIT_MASK_DAC_PWR_CUT_CNTR) << BIT_SHIFT_DAC_PWR_CUT_CNTR)
#define BIT_GET_DAC_PWR_CUT_CNTR(x)           	(((x) >> BIT_SHIFT_DAC_PWR_CUT_CNTR) & BIT_MASK_DAC_PWR_CUT_CNTR)

#define BIT_ST_DAC_FIFO_ON                    	BIT(11)
#define BIT_SHIFT_ST_DAC_FIFO_ON              	11
#define BIT_MASK_ST_DAC_FIFO_ON               	0x1
#define BIT_CTRL_ST_DAC_FIFO_ON(x)            	(((x) & BIT_MASK_ST_DAC_FIFO_ON) << BIT_SHIFT_ST_DAC_FIFO_ON)

#define BIT_ST_DAC_ISO_ON                     	BIT(10)
#define BIT_SHIFT_ST_DAC_ISO_ON               	10
#define BIT_MASK_ST_DAC_ISO_ON                	0x1
#define BIT_CTRL_ST_DAC_ISO_ON(x)             	(((x) & BIT_MASK_ST_DAC_ISO_ON) << BIT_SHIFT_ST_DAC_ISO_ON)

#define BIT_ST_DAC_PWR33_ON                   	BIT(9)
#define BIT_SHIFT_ST_DAC_PWR33_ON             	9
#define BIT_MASK_ST_DAC_PWR33_ON              	0x1
#define BIT_CTRL_ST_DAC_PWR33_ON(x)           	(((x) & BIT_MASK_ST_DAC_PWR33_ON) << BIT_SHIFT_ST_DAC_PWR33_ON)

#define BIT_ST_DAC_PWR12_ON                   	BIT(8)
#define BIT_SHIFT_ST_DAC_PWR12_ON             	8
#define BIT_MASK_ST_DAC_PWR12_ON              	0x1
#define BIT_CTRL_ST_DAC_PWR12_ON(x)           	(((x) & BIT_MASK_ST_DAC_PWR12_ON) << BIT_SHIFT_ST_DAC_PWR12_ON)

#define BIT_DAC_ISO_MANU                      	BIT(3)
#define BIT_SHIFT_DAC_ISO_MANU                	3
#define BIT_MASK_DAC_ISO_MANU                 	0x1
#define BIT_CTRL_DAC_ISO_MANU(x)              	(((x) & BIT_MASK_DAC_ISO_MANU) << BIT_SHIFT_DAC_ISO_MANU)

#define BIT_DAC_PWR33_MANU                    	BIT(2)
#define BIT_SHIFT_DAC_PWR33_MANU              	2
#define BIT_MASK_DAC_PWR33_MANU               	0x1
#define BIT_CTRL_DAC_PWR33_MANU(x)            	(((x) & BIT_MASK_DAC_PWR33_MANU) << BIT_SHIFT_DAC_PWR33_MANU)

#define BIT_DAC_PWR12_MANU                    	BIT(1)
#define BIT_SHIFT_DAC_PWR12_MANU              	1
#define BIT_MASK_DAC_PWR12_MANU               	0x1
#define BIT_CTRL_DAC_PWR12_MANU(x)            	(((x) & BIT_MASK_DAC_PWR12_MANU) << BIT_SHIFT_DAC_PWR12_MANU)

#define BIT_DAC_PWR_AUTO                      	BIT(0)
#define BIT_SHIFT_DAC_PWR_AUTO                	0
#define BIT_MASK_DAC_PWR_AUTO                 	0x1
#define BIT_CTRL_DAC_PWR_AUTO(x)              	(((x) & BIT_MASK_DAC_PWR_AUTO) << BIT_SHIFT_DAC_PWR_AUTO)


//2 REG_DAC_ANAPAR_DA0

#define BIT_SHIFT_PWR_ALL_CNTR 12
#define BIT_MASK_PWR_ALL_CNTR 0xfffff
#define BIT_PWR_ALL_CNTR(x)                   	(((x) & BIT_MASK_PWR_ALL_CNTR) << BIT_SHIFT_PWR_ALL_CNTR)
#define BIT_CTRL_PWR_ALL_CNTR(x)              	(((x) & BIT_MASK_PWR_ALL_CNTR) << BIT_SHIFT_PWR_ALL_CNTR)
#define BIT_GET_PWR_ALL_CNTR(x)               	(((x) >> BIT_SHIFT_PWR_ALL_CNTR) & BIT_MASK_PWR_ALL_CNTR)


#define BIT_SHIFT_PWR_FUP_CNTR 0
#define BIT_MASK_PWR_FUP_CNTR 0xfff
#define BIT_PWR_FUP_CNTR(x)                   	(((x) & BIT_MASK_PWR_FUP_CNTR) << BIT_SHIFT_PWR_FUP_CNTR)
#define BIT_CTRL_PWR_FUP_CNTR(x)              	(((x) & BIT_MASK_PWR_FUP_CNTR) << BIT_SHIFT_PWR_FUP_CNTR)
#define BIT_GET_PWR_FUP_CNTR(x)               	(((x) >> BIT_SHIFT_PWR_FUP_CNTR) & BIT_MASK_PWR_FUP_CNTR)


//2 REG_DAC_ANAPAR_DA1
#define BIT_FUP_EN                            	BIT(31)
#define BIT_SHIFT_FUP_EN                      	31
#define BIT_MASK_FUP_EN                       	0x1
#define BIT_CTRL_FUP_EN(x)                    	(((x) & BIT_MASK_FUP_EN) << BIT_SHIFT_FUP_EN)


#define BIT_SHIFT_ANAPAR_DA 8
#define BIT_MASK_ANAPAR_DA 0x7fffff
#define BIT_ANAPAR_DA(x)                      	(((x) & BIT_MASK_ANAPAR_DA) << BIT_SHIFT_ANAPAR_DA)
#define BIT_CTRL_ANAPAR_DA(x)                 	(((x) & BIT_MASK_ANAPAR_DA) << BIT_SHIFT_ANAPAR_DA)
#define BIT_GET_ANAPAR_DA(x)                  	(((x) >> BIT_SHIFT_ANAPAR_DA) & BIT_MASK_ANAPAR_DA)

#define BIT_D_POW_DACVREF                     	BIT(7)
#define BIT_SHIFT_D_POW_DACVREF               	7
#define BIT_MASK_D_POW_DACVREF                	0x1
#define BIT_CTRL_D_POW_DACVREF(x)             	(((x) & BIT_MASK_D_POW_DACVREF) << BIT_SHIFT_D_POW_DACVREF)

#define BIT_D_POW_VREF2                       	BIT(6)
#define BIT_SHIFT_D_POW_VREF2                 	6
#define BIT_MASK_D_POW_VREF2                  	0x1
#define BIT_CTRL_D_POW_VREF2(x)               	(((x) & BIT_MASK_D_POW_VREF2) << BIT_SHIFT_D_POW_VREF2)

#define BIT_D_POW_MBIAS                       	BIT(5)
#define BIT_SHIFT_D_POW_MBIAS                 	5
#define BIT_MASK_D_POW_MBIAS                  	0x1
#define BIT_CTRL_D_POW_MBIAS(x)               	(((x) & BIT_MASK_D_POW_MBIAS) << BIT_SHIFT_D_POW_MBIAS)

#define BIT_D_POW_DIV4                        	BIT(4)
#define BIT_SHIFT_D_POW_DIV4                  	4
#define BIT_MASK_D_POW_DIV4                   	0x1
#define BIT_CTRL_D_POW_DIV4(x)                	(((x) & BIT_MASK_D_POW_DIV4) << BIT_SHIFT_D_POW_DIV4)

#define BIT_D_POW_DF1SE_R                     	BIT(3)
#define BIT_SHIFT_D_POW_DF1SE_R               	3
#define BIT_MASK_D_POW_DF1SE_R                	0x1
#define BIT_CTRL_D_POW_DF1SE_R(x)             	(((x) & BIT_MASK_D_POW_DF1SE_R) << BIT_SHIFT_D_POW_DF1SE_R)

#define BIT_D_POW_DF2SE_L                     	BIT(2)
#define BIT_SHIFT_D_POW_DF2SE_L               	2
#define BIT_MASK_D_POW_DF2SE_L                	0x1
#define BIT_CTRL_D_POW_DF2SE_L(x)             	(((x) & BIT_MASK_D_POW_DF2SE_L) << BIT_SHIFT_D_POW_DF2SE_L)

#define BIT_D_POW_DAC_R                       	BIT(1)
#define BIT_SHIFT_D_POW_DAC_R                 	1
#define BIT_MASK_D_POW_DAC_R                  	0x1
#define BIT_CTRL_D_POW_DAC_R(x)               	(((x) & BIT_MASK_D_POW_DAC_R) << BIT_SHIFT_D_POW_DAC_R)

#define BIT_D_POW_DAC_L                       	BIT(0)
#define BIT_SHIFT_D_POW_DAC_L                 	0
#define BIT_MASK_D_POW_DAC_L                  	0x1
#define BIT_CTRL_D_POW_DAC_L(x)               	(((x) & BIT_MASK_D_POW_DAC_L) << BIT_SHIFT_D_POW_DAC_L)


//================ Register Reg Field  =========================
#define REG_DAC0_FIFO_WR 0x0000
#define REG_DAC_CTRL 0x0004
#define REG_DAC_INTR_CTRL 0x0008
#define REG_DAC_INTR_STS 0x000C
#define REG_DAC_PWR_CTRL 0x0010
#define REG_DAC_ANAPAR_DA0 0x0014
#define REG_DAC_ANAPAR_DA1 0x0018


//================ DAC HAL related enumeration ==================


//================ DAC HAL Macro  ===========================
#define HAL_DAC_WRITE32(dacidx, addr, value)    HAL_WRITE32(DAC_REG_BASE+dacidx*0x800 \
                                                        ,addr,value)
#define HAL_DAC_READ32(dacidx, addr)            HAL_READ32(DAC_REG_BASE+dacidx*0x800,addr)


//================ DAC Function Prototypes  =====================
RTK_STATUS HalDACInit8195a(IN  VOID *Data);
RTK_STATUS HalDACDeInit8195a(IN  VOID *Data);
RTK_STATUS HalDACEnableRtl8195a(IN  VOID *Data);
RTK_STATUS HalDACIntrCtrl8195a(IN  VOID    *Data);
u8 HalDACSendRtl8195a(IN  VOID    *Data);
u32 HalDACReadRegRtl8195a(IN  VOID    *Data,IN  u8      I2CReg);

#endif
