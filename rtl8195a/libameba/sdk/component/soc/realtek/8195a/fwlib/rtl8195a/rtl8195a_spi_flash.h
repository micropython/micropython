#ifndef _RTL8195A_SPI_FLASH_H
#define _RTL8195A_SPI_FLASH_H

#define CPU_OPT_WIDTH 0x1F

//2 REG_NOT_VALID

//2 REG_SPIC_CTRLR0

#define BIT_SHIFT_CK_MTIMES 23
#define BIT_MASK_CK_MTIMES 0x1f
#define BIT_CK_MTIMES(x)                      	(((x) & BIT_MASK_CK_MTIMES) << BIT_SHIFT_CK_MTIMES)
#define BIT_CTRL_CK_MTIMES(x)                 	(((x) & BIT_MASK_CK_MTIMES) << BIT_SHIFT_CK_MTIMES)
#define BIT_GET_CK_MTIMES(x)                  	(((x) >> BIT_SHIFT_CK_MTIMES) & BIT_MASK_CK_MTIMES)

#define BIT_FAST_RD                           	BIT(22)
#define BIT_SHIFT_FAST_RD                     	22
#define BIT_MASK_FAST_RD                      	0x1
#define BIT_CTRL_FAST_RD(x)                   	(((x) & BIT_MASK_FAST_RD) << BIT_SHIFT_FAST_RD)


#define BIT_SHIFT_CMD_CH 20
#define BIT_MASK_CMD_CH 0x3
#define BIT_CMD_CH(x)                         	(((x) & BIT_MASK_CMD_CH) << BIT_SHIFT_CMD_CH)
#define BIT_CTRL_CMD_CH(x)                    	(((x) & BIT_MASK_CMD_CH) << BIT_SHIFT_CMD_CH)
#define BIT_GET_CMD_CH(x)                     	(((x) >> BIT_SHIFT_CMD_CH) & BIT_MASK_CMD_CH)


#define BIT_SHIFT_DATA_CH 18
#define BIT_MASK_DATA_CH 0x3
#define BIT_DATA_CH(x)                        	(((x) & BIT_MASK_DATA_CH) << BIT_SHIFT_DATA_CH)
#define BIT_CTRL_DATA_CH(x)                   	(((x) & BIT_MASK_DATA_CH) << BIT_SHIFT_DATA_CH)
#define BIT_GET_DATA_CH(x)                    	(((x) >> BIT_SHIFT_DATA_CH) & BIT_MASK_DATA_CH)


#define BIT_SHIFT_ADDR_CH 16
#define BIT_MASK_ADDR_CH 0x3
#define BIT_ADDR_CH(x)                        	(((x) & BIT_MASK_ADDR_CH) << BIT_SHIFT_ADDR_CH)
#define BIT_CTRL_ADDR_CH(x)                   	(((x) & BIT_MASK_ADDR_CH) << BIT_SHIFT_ADDR_CH)
#define BIT_GET_ADDR_CH(x)                    	(((x) >> BIT_SHIFT_ADDR_CH) & BIT_MASK_ADDR_CH)


#define BIT_SHIFT_TMOD 8
#define BIT_MASK_TMOD 0x3
#define BIT_TMOD(x)                           	(((x) & BIT_MASK_TMOD) << BIT_SHIFT_TMOD)
#define BIT_CTRL_TMOD(x)                      	(((x) & BIT_MASK_TMOD) << BIT_SHIFT_TMOD)
#define BIT_GET_TMOD(x)                       	(((x) >> BIT_SHIFT_TMOD) & BIT_MASK_TMOD)

#define BIT_SCPOL                             	BIT(7)
#define BIT_SHIFT_SCPOL                       	7
#define BIT_MASK_SCPOL                        	0x1
#define BIT_CTRL_SCPOL(x)                     	(((x) & BIT_MASK_SCPOL) << BIT_SHIFT_SCPOL)

#define BIT_SCPH                              	BIT(6)
#define BIT_SHIFT_SCPH                        	6
#define BIT_MASK_SCPH                         	0x1
#define BIT_CTRL_SCPH(x)                      	(((x) & BIT_MASK_SCPH) << BIT_SHIFT_SCPH)

//2 REG_SPIC_CTRLR1

#define BIT_SHIFT_NDF 0
#define BIT_MASK_NDF 0xfff
#define BIT_NDF(x)                            	(((x) & BIT_MASK_NDF) << BIT_SHIFT_NDF)
#define BIT_CTRL_NDF(x)                       	(((x) & BIT_MASK_NDF) << BIT_SHIFT_NDF)
#define BIT_GET_NDF(x)                        	(((x) >> BIT_SHIFT_NDF) & BIT_MASK_NDF)


//2 REG_SPIC_SSIENR
#define BIT_ATCK_CMD                          	BIT(1)
#define BIT_SHIFT_ATCK_CMD                    	1
#define BIT_MASK_ATCK_CMD                     	0x1
#define BIT_CTRL_ATCK_CMD(x)                  	(((x) & BIT_MASK_ATCK_CMD) << BIT_SHIFT_ATCK_CMD)

#define BIT_SPIC_EN                           	BIT(0)
#define BIT_SHIFT_SPIC_EN                     	0
#define BIT_MASK_SPIC_EN                      	0x1
#define BIT_CTRL_SPIC_EN(x)                   	(((x) & BIT_MASK_SPIC_EN) << BIT_SHIFT_SPIC_EN)

//2 REG_SPIC_MWCR

//2 REG_SPIC_SER
#define BIT_SER                               	BIT(0)
#define BIT_SHIFT_SER                         	0
#define BIT_MASK_SER                          	0x1
#define BIT_CTRL_SER(x)                       	(((x) & BIT_MASK_SER) << BIT_SHIFT_SER)

//2 REG_SPIC_BAUDR

#define BIT_SHIFT_SCKDV 0
#define BIT_MASK_SCKDV 0xffff
#define BIT_SCKDV(x)                          	(((x) & BIT_MASK_SCKDV) << BIT_SHIFT_SCKDV)
#define BIT_CTRL_SCKDV(x)                     	(((x) & BIT_MASK_SCKDV) << BIT_SHIFT_SCKDV)
#define BIT_GET_SCKDV(x)                      	(((x) >> BIT_SHIFT_SCKDV) & BIT_MASK_SCKDV)


//2 REG_SPIC_TXFTLR

#define BIT_SHIFT_TFT 0
#define BIT_MASK_TFT 0x1f
#define BIT_TFT(x)                            	(((x) & BIT_MASK_TFT) << BIT_SHIFT_TFT)
#define BIT_CTRL_TFT(x)                       	(((x) & BIT_MASK_TFT) << BIT_SHIFT_TFT)
#define BIT_GET_TFT(x)                        	(((x) >> BIT_SHIFT_TFT) & BIT_MASK_TFT)


//2 REG_SPIC_RXFTLR

#define BIT_SHIFT_RFT 0
#define BIT_MASK_RFT 0x1f
#define BIT_RFT(x)                            	(((x) & BIT_MASK_RFT) << BIT_SHIFT_RFT)
#define BIT_CTRL_RFT(x)                       	(((x) & BIT_MASK_RFT) << BIT_SHIFT_RFT)
#define BIT_GET_RFT(x)                        	(((x) >> BIT_SHIFT_RFT) & BIT_MASK_RFT)


//2 REG_SPIC_TXFLR

#define BIT_SHIFT_TXFL 0
#define BIT_MASK_TXFL 0x3f
#define BIT_TXFL(x)                           	(((x) & BIT_MASK_TXFL) << BIT_SHIFT_TXFL)
#define BIT_CTRL_TXFL(x)                      	(((x) & BIT_MASK_TXFL) << BIT_SHIFT_TXFL)
#define BIT_GET_TXFL(x)                       	(((x) >> BIT_SHIFT_TXFL) & BIT_MASK_TXFL)


//2 REG_SPIC_RXFLR

#define BIT_SHIFT_RXFL 0
#define BIT_MASK_RXFL 0x3f
#define BIT_RXFL(x)                           	(((x) & BIT_MASK_RXFL) << BIT_SHIFT_RXFL)
#define BIT_CTRL_RXFL(x)                      	(((x) & BIT_MASK_RXFL) << BIT_SHIFT_RXFL)
#define BIT_GET_RXFL(x)                       	(((x) >> BIT_SHIFT_RXFL) & BIT_MASK_RXFL)


//2 REG_SPIC_SR
#define BIT_TXE                               	BIT(5)
#define BIT_SHIFT_TXE                         	5
#define BIT_MASK_TXE                          	0x1
#define BIT_CTRL_TXE(x)                       	(((x) & BIT_MASK_TXE) << BIT_SHIFT_TXE)

#define BIT_RFF                               	BIT(4)
#define BIT_SHIFT_RFF                         	4
#define BIT_MASK_RFF                          	0x1
#define BIT_CTRL_RFF(x)                       	(((x) & BIT_MASK_RFF) << BIT_SHIFT_RFF)

#define BIT_RFNE                              	BIT(3)
#define BIT_SHIFT_RFNE                        	3
#define BIT_MASK_RFNE                         	0x1
#define BIT_CTRL_RFNE(x)                      	(((x) & BIT_MASK_RFNE) << BIT_SHIFT_RFNE)

#define BIT_TFE                               	BIT(2)
#define BIT_SHIFT_TFE                         	2
#define BIT_MASK_TFE                          	0x1
#define BIT_CTRL_TFE(x)                       	(((x) & BIT_MASK_TFE) << BIT_SHIFT_TFE)

#define BIT_TFNF                              	BIT(1)
#define BIT_SHIFT_TFNF                        	1
#define BIT_MASK_TFNF                         	0x1
#define BIT_CTRL_TFNF(x)                      	(((x) & BIT_MASK_TFNF) << BIT_SHIFT_TFNF)

#define BIT_BUSY                              	BIT(0)
#define BIT_SHIFT_BUSY                        	0
#define BIT_MASK_BUSY                         	0x1
#define BIT_CTRL_BUSY(x)                      	(((x) & BIT_MASK_BUSY) << BIT_SHIFT_BUSY)

//2 REG_SPIC_IMR
#define BIT_TXSIM                             	BIT(9)
#define BIT_SHIFT_TXSIM                       	9
#define BIT_MASK_TXSIM                        	0x1
#define BIT_CTRL_TXSIM(x)                     	(((x) & BIT_MASK_TXSIM) << BIT_SHIFT_TXSIM)

#define BIT_ACEIM                             	BIT(8)
#define BIT_SHIFT_ACEIM                       	8
#define BIT_MASK_ACEIM                        	0x1
#define BIT_CTRL_ACEIM(x)                     	(((x) & BIT_MASK_ACEIM) << BIT_SHIFT_ACEIM)

#define BIT_BYEIM                             	BIT(7)
#define BIT_SHIFT_BYEIM                       	7
#define BIT_MASK_BYEIM                        	0x1
#define BIT_CTRL_BYEIM(x)                     	(((x) & BIT_MASK_BYEIM) << BIT_SHIFT_BYEIM)

#define BIT_WBEIM                             	BIT(6)
#define BIT_SHIFT_WBEIM                       	6
#define BIT_MASK_WBEIM                        	0x1
#define BIT_CTRL_WBEIM(x)                     	(((x) & BIT_MASK_WBEIM) << BIT_SHIFT_WBEIM)

#define BIT_FSEIM                             	BIT(5)
#define BIT_SHIFT_FSEIM                       	5
#define BIT_MASK_FSEIM                        	0x1
#define BIT_CTRL_FSEIM(x)                     	(((x) & BIT_MASK_FSEIM) << BIT_SHIFT_FSEIM)

#define BIT_RXFIM                             	BIT(4)
#define BIT_SHIFT_RXFIM                       	4
#define BIT_MASK_RXFIM                        	0x1
#define BIT_CTRL_RXFIM(x)                     	(((x) & BIT_MASK_RXFIM) << BIT_SHIFT_RXFIM)

#define BIT_RXOIM                             	BIT(3)
#define BIT_SHIFT_RXOIM                       	3
#define BIT_MASK_RXOIM                        	0x1
#define BIT_CTRL_RXOIM(x)                     	(((x) & BIT_MASK_RXOIM) << BIT_SHIFT_RXOIM)

#define BIT_RXUIM                             	BIT(2)
#define BIT_SHIFT_RXUIM                       	2
#define BIT_MASK_RXUIM                        	0x1
#define BIT_CTRL_RXUIM(x)                     	(((x) & BIT_MASK_RXUIM) << BIT_SHIFT_RXUIM)

#define BIT_TXOIM                             	BIT(1)
#define BIT_SHIFT_TXOIM                       	1
#define BIT_MASK_TXOIM                        	0x1
#define BIT_CTRL_TXOIM(x)                     	(((x) & BIT_MASK_TXOIM) << BIT_SHIFT_TXOIM)

#define BIT_TXEIM                             	BIT(0)
#define BIT_SHIFT_TXEIM                       	0
#define BIT_MASK_TXEIM                        	0x1
#define BIT_CTRL_TXEIM(x)                     	(((x) & BIT_MASK_TXEIM) << BIT_SHIFT_TXEIM)

//2 REG_SPIC_ISR
#define BIT_TXSIS                             	BIT(9)
#define BIT_SHIFT_TXSIS                       	9
#define BIT_MASK_TXSIS                        	0x1
#define BIT_CTRL_TXSIS(x)                     	(((x) & BIT_MASK_TXSIS) << BIT_SHIFT_TXSIS)

#define BIT_ACEIS                             	BIT(8)
#define BIT_SHIFT_ACEIS                       	8
#define BIT_MASK_ACEIS                        	0x1
#define BIT_CTRL_ACEIS(x)                     	(((x) & BIT_MASK_ACEIS) << BIT_SHIFT_ACEIS)

#define BIT_BYEIS                             	BIT(7)
#define BIT_SHIFT_BYEIS                       	7
#define BIT_MASK_BYEIS                        	0x1
#define BIT_CTRL_BYEIS(x)                     	(((x) & BIT_MASK_BYEIS) << BIT_SHIFT_BYEIS)

#define BIT_WBEIS                             	BIT(6)
#define BIT_SHIFT_WBEIS                       	6
#define BIT_MASK_WBEIS                        	0x1
#define BIT_CTRL_WBEIS(x)                     	(((x) & BIT_MASK_WBEIS) << BIT_SHIFT_WBEIS)

#define BIT_FSEIS                             	BIT(5)
#define BIT_SHIFT_FSEIS                       	5
#define BIT_MASK_FSEIS                        	0x1
#define BIT_CTRL_FSEIS(x)                     	(((x) & BIT_MASK_FSEIS) << BIT_SHIFT_FSEIS)

#define BIT_RXFIS                             	BIT(4)
#define BIT_SHIFT_RXFIS                       	4
#define BIT_MASK_RXFIS                        	0x1
#define BIT_CTRL_RXFIS(x)                     	(((x) & BIT_MASK_RXFIS) << BIT_SHIFT_RXFIS)

#define BIT_RXOIS                             	BIT(3)
#define BIT_SHIFT_RXOIS                       	3
#define BIT_MASK_RXOIS                        	0x1
#define BIT_CTRL_RXOIS(x)                     	(((x) & BIT_MASK_RXOIS) << BIT_SHIFT_RXOIS)

#define BIT_RXUIS                             	BIT(2)
#define BIT_SHIFT_RXUIS                       	2
#define BIT_MASK_RXUIS                        	0x1
#define BIT_CTRL_RXUIS(x)                     	(((x) & BIT_MASK_RXUIS) << BIT_SHIFT_RXUIS)

#define BIT_TXOIS                             	BIT(1)
#define BIT_SHIFT_TXOIS                       	1
#define BIT_MASK_TXOIS                        	0x1
#define BIT_CTRL_TXOIS(x)                     	(((x) & BIT_MASK_TXOIS) << BIT_SHIFT_TXOIS)

#define BIT_TXEIS                             	BIT(0)
#define BIT_SHIFT_TXEIS                       	0
#define BIT_MASK_TXEIS                        	0x1
#define BIT_CTRL_TXEIS(x)                     	(((x) & BIT_MASK_TXEIS) << BIT_SHIFT_TXEIS)

//2 REG_SPIC_RISR
#define BIT_ACEIR                             	BIT(8)
#define BIT_SHIFT_ACEIR                       	8
#define BIT_MASK_ACEIR                        	0x1
#define BIT_CTRL_ACEIR(x)                     	(((x) & BIT_MASK_ACEIR) << BIT_SHIFT_ACEIR)

#define BIT_BYEIR                             	BIT(7)
#define BIT_SHIFT_BYEIR                       	7
#define BIT_MASK_BYEIR                        	0x1
#define BIT_CTRL_BYEIR(x)                     	(((x) & BIT_MASK_BYEIR) << BIT_SHIFT_BYEIR)

#define BIT_WBEIR                             	BIT(6)
#define BIT_SHIFT_WBEIR                       	6
#define BIT_MASK_WBEIR                        	0x1
#define BIT_CTRL_WBEIR(x)                     	(((x) & BIT_MASK_WBEIR) << BIT_SHIFT_WBEIR)

#define BIT_FSEIR                             	BIT(5)
#define BIT_SHIFT_FSEIR                       	5
#define BIT_MASK_FSEIR                        	0x1
#define BIT_CTRL_FSEIR(x)                     	(((x) & BIT_MASK_FSEIR) << BIT_SHIFT_FSEIR)

#define BIT_RXFIR                             	BIT(4)
#define BIT_SHIFT_RXFIR                       	4
#define BIT_MASK_RXFIR                        	0x1
#define BIT_CTRL_RXFIR(x)                     	(((x) & BIT_MASK_RXFIR) << BIT_SHIFT_RXFIR)

#define BIT_RXOIR                             	BIT(3)
#define BIT_SHIFT_RXOIR                       	3
#define BIT_MASK_RXOIR                        	0x1
#define BIT_CTRL_RXOIR(x)                     	(((x) & BIT_MASK_RXOIR) << BIT_SHIFT_RXOIR)

#define BIT_RXUIR                             	BIT(2)
#define BIT_SHIFT_RXUIR                       	2
#define BIT_MASK_RXUIR                        	0x1
#define BIT_CTRL_RXUIR(x)                     	(((x) & BIT_MASK_RXUIR) << BIT_SHIFT_RXUIR)

#define BIT_TXOIR                             	BIT(1)
#define BIT_SHIFT_TXOIR                       	1
#define BIT_MASK_TXOIR                        	0x1
#define BIT_CTRL_TXOIR(x)                     	(((x) & BIT_MASK_TXOIR) << BIT_SHIFT_TXOIR)

#define BIT_TXEIR                             	BIT(0)
#define BIT_SHIFT_TXEIR                       	0
#define BIT_MASK_TXEIR                        	0x1
#define BIT_CTRL_TXEIR(x)                     	(((x) & BIT_MASK_TXEIR) << BIT_SHIFT_TXEIR)

//2 REG_SPIC_TXOICR
#define BIT_TXOICR                            	BIT(0)
#define BIT_SHIFT_TXOICR                      	0
#define BIT_MASK_TXOICR                       	0x1
#define BIT_CTRL_TXOICR(x)                    	(((x) & BIT_MASK_TXOICR) << BIT_SHIFT_TXOICR)

//2 REG_SPIC_RXOICR
#define BIT_RXOCIR                            	BIT(0)
#define BIT_SHIFT_RXOCIR                      	0
#define BIT_MASK_RXOCIR                       	0x1
#define BIT_CTRL_RXOCIR(x)                    	(((x) & BIT_MASK_RXOCIR) << BIT_SHIFT_RXOCIR)

//2 REG_SPC_RXUICR
#define BIT_RXUICR                            	BIT(0)
#define BIT_SHIFT_RXUICR                      	0
#define BIT_MASK_RXUICR                       	0x1
#define BIT_CTRL_RXUICR(x)                    	(((x) & BIT_MASK_RXUICR) << BIT_SHIFT_RXUICR)

//2 REG_SPIC_MSTICR
#define BIT_MSTICR                            	BIT(0)
#define BIT_SHIFT_MSTICR                      	0
#define BIT_MASK_MSTICR                       	0x1
#define BIT_CTRL_MSTICR(x)                    	(((x) & BIT_MASK_MSTICR) << BIT_SHIFT_MSTICR)

//2 REG_SPIC_ICR

#define BIT_SHIFT_ICR 0
#define BIT_MASK_ICR 0xff
#define BIT_ICR(x)                            	(((x) & BIT_MASK_ICR) << BIT_SHIFT_ICR)
#define BIT_CTRL_ICR(x)                       	(((x) & BIT_MASK_ICR) << BIT_SHIFT_ICR)
#define BIT_GET_ICR(x)                        	(((x) >> BIT_SHIFT_ICR) & BIT_MASK_ICR)


//2 REG_SPIC_DMACR

//2 REG_SPIC_DMATDLR0

//2 REG_SPIC_DMATDLR1

//2 REG_SPIC_IDR

#define BIT_SHIFT_IDCODE 0
#define BIT_MASK_IDCODE 0xffffffffL
#define BIT_IDCODE(x)                         	(((x) & BIT_MASK_IDCODE) << BIT_SHIFT_IDCODE)
#define BIT_CTRL_IDCODE(x)                    	(((x) & BIT_MASK_IDCODE) << BIT_SHIFT_IDCODE)
#define BIT_GET_IDCODE(x)                     	(((x) >> BIT_SHIFT_IDCODE) & BIT_MASK_IDCODE)


//2 REG_SPIC_VERSION

#define BIT_SHIFT_SPIC_VERSION 0
#define BIT_MASK_SPIC_VERSION 0xffffffffL
#define BIT_SPIC_VERSION(x)                   	(((x) & BIT_MASK_SPIC_VERSION) << BIT_SHIFT_SPIC_VERSION)
#define BIT_CTRL_SPIC_VERSION(x)              	(((x) & BIT_MASK_SPIC_VERSION) << BIT_SHIFT_SPIC_VERSION)
#define BIT_GET_SPIC_VERSION(x)               	(((x) >> BIT_SHIFT_SPIC_VERSION) & BIT_MASK_SPIC_VERSION)


//2 REG_SPIC_DR0

#define BIT_SHIFT_DR0 0
#define BIT_MASK_DR0 0xffffffffL
#define BIT_DR0(x)                            	(((x) & BIT_MASK_DR0) << BIT_SHIFT_DR0)
#define BIT_CTRL_DR0(x)                       	(((x) & BIT_MASK_DR0) << BIT_SHIFT_DR0)
#define BIT_GET_DR0(x)                        	(((x) >> BIT_SHIFT_DR0) & BIT_MASK_DR0)


//2 REG_SPIC_DR1

#define BIT_SHIFT_DR1 0
#define BIT_MASK_DR1 0xffffffffL
#define BIT_DR1(x)                            	(((x) & BIT_MASK_DR1) << BIT_SHIFT_DR1)
#define BIT_CTRL_DR1(x)                       	(((x) & BIT_MASK_DR1) << BIT_SHIFT_DR1)
#define BIT_GET_DR1(x)                        	(((x) >> BIT_SHIFT_DR1) & BIT_MASK_DR1)


//2 REG_SPIC_DR2

#define BIT_SHIFT_DR2 0
#define BIT_MASK_DR2 0xffffffffL
#define BIT_DR2(x)                            	(((x) & BIT_MASK_DR2) << BIT_SHIFT_DR2)
#define BIT_CTRL_DR2(x)                       	(((x) & BIT_MASK_DR2) << BIT_SHIFT_DR2)
#define BIT_GET_DR2(x)                        	(((x) >> BIT_SHIFT_DR2) & BIT_MASK_DR2)


//2 REG_SPIC_DR3

#define BIT_SHIFT_DR3 0
#define BIT_MASK_DR3 0xffffffffL
#define BIT_DR3(x)                            	(((x) & BIT_MASK_DR3) << BIT_SHIFT_DR3)
#define BIT_CTRL_DR3(x)                       	(((x) & BIT_MASK_DR3) << BIT_SHIFT_DR3)
#define BIT_GET_DR3(x)                        	(((x) >> BIT_SHIFT_DR3) & BIT_MASK_DR3)


//2 REG_SPIC_DR4

#define BIT_SHIFT_DR4 0
#define BIT_MASK_DR4 0xffffffffL
#define BIT_DR4(x)                            	(((x) & BIT_MASK_DR4) << BIT_SHIFT_DR4)
#define BIT_CTRL_DR4(x)                       	(((x) & BIT_MASK_DR4) << BIT_SHIFT_DR4)
#define BIT_GET_DR4(x)                        	(((x) >> BIT_SHIFT_DR4) & BIT_MASK_DR4)


//2 REG_SPIC_DR5

#define BIT_SHIFT_DR5 0
#define BIT_MASK_DR5 0xffffffffL
#define BIT_DR5(x)                            	(((x) & BIT_MASK_DR5) << BIT_SHIFT_DR5)
#define BIT_CTRL_DR5(x)                       	(((x) & BIT_MASK_DR5) << BIT_SHIFT_DR5)
#define BIT_GET_DR5(x)                        	(((x) >> BIT_SHIFT_DR5) & BIT_MASK_DR5)


//2 REG_SPIC_DR6

#define BIT_SHIFT_DR6 0
#define BIT_MASK_DR6 0xffffffffL
#define BIT_DR6(x)                            	(((x) & BIT_MASK_DR6) << BIT_SHIFT_DR6)
#define BIT_CTRL_DR6(x)                       	(((x) & BIT_MASK_DR6) << BIT_SHIFT_DR6)
#define BIT_GET_DR6(x)                        	(((x) >> BIT_SHIFT_DR6) & BIT_MASK_DR6)


//2 REG_SPIC_DR7

#define BIT_SHIFT_DR7 0
#define BIT_MASK_DR7 0xffffffffL
#define BIT_DR7(x)                            	(((x) & BIT_MASK_DR7) << BIT_SHIFT_DR7)
#define BIT_CTRL_DR7(x)                       	(((x) & BIT_MASK_DR7) << BIT_SHIFT_DR7)
#define BIT_GET_DR7(x)                        	(((x) >> BIT_SHIFT_DR7) & BIT_MASK_DR7)


//2 REG_SPIC_DR8

#define BIT_SHIFT_DR8 0
#define BIT_MASK_DR8 0xffffffffL
#define BIT_DR8(x)                            	(((x) & BIT_MASK_DR8) << BIT_SHIFT_DR8)
#define BIT_CTRL_DR8(x)                       	(((x) & BIT_MASK_DR8) << BIT_SHIFT_DR8)
#define BIT_GET_DR8(x)                        	(((x) >> BIT_SHIFT_DR8) & BIT_MASK_DR8)


//2 REG_SPIC_DR9

#define BIT_SHIFT_DR9 0
#define BIT_MASK_DR9 0xffffffffL
#define BIT_DR9(x)                            	(((x) & BIT_MASK_DR9) << BIT_SHIFT_DR9)
#define BIT_CTRL_DR9(x)                       	(((x) & BIT_MASK_DR9) << BIT_SHIFT_DR9)
#define BIT_GET_DR9(x)                        	(((x) >> BIT_SHIFT_DR9) & BIT_MASK_DR9)


//2 REG_SPIC_DR10

#define BIT_SHIFT_DR10 0
#define BIT_MASK_DR10 0xffffffffL
#define BIT_DR10(x)                           	(((x) & BIT_MASK_DR10) << BIT_SHIFT_DR10)
#define BIT_CTRL_DR10(x)                      	(((x) & BIT_MASK_DR10) << BIT_SHIFT_DR10)
#define BIT_GET_DR10(x)                       	(((x) >> BIT_SHIFT_DR10) & BIT_MASK_DR10)


//2 REG_SPIC_DR11

#define BIT_SHIFT_DR11 0
#define BIT_MASK_DR11 0xffffffffL
#define BIT_DR11(x)                           	(((x) & BIT_MASK_DR11) << BIT_SHIFT_DR11)
#define BIT_CTRL_DR11(x)                      	(((x) & BIT_MASK_DR11) << BIT_SHIFT_DR11)
#define BIT_GET_DR11(x)                       	(((x) >> BIT_SHIFT_DR11) & BIT_MASK_DR11)


//2 REG_SPIC_DR12

#define BIT_SHIFT_DR12 0
#define BIT_MASK_DR12 0xffffffffL
#define BIT_DR12(x)                           	(((x) & BIT_MASK_DR12) << BIT_SHIFT_DR12)
#define BIT_CTRL_DR12(x)                      	(((x) & BIT_MASK_DR12) << BIT_SHIFT_DR12)
#define BIT_GET_DR12(x)                       	(((x) >> BIT_SHIFT_DR12) & BIT_MASK_DR12)


//2 REG_SPIC_DR13

#define BIT_SHIFT_DR13 0
#define BIT_MASK_DR13 0xffffffffL
#define BIT_DR13(x)                           	(((x) & BIT_MASK_DR13) << BIT_SHIFT_DR13)
#define BIT_CTRL_DR13(x)                      	(((x) & BIT_MASK_DR13) << BIT_SHIFT_DR13)
#define BIT_GET_DR13(x)                       	(((x) >> BIT_SHIFT_DR13) & BIT_MASK_DR13)


//2 REG_SPIC_DR14

#define BIT_SHIFT_DR14 0
#define BIT_MASK_DR14 0xffffffffL
#define BIT_DR14(x)                           	(((x) & BIT_MASK_DR14) << BIT_SHIFT_DR14)
#define BIT_CTRL_DR14(x)                      	(((x) & BIT_MASK_DR14) << BIT_SHIFT_DR14)
#define BIT_GET_DR14(x)                       	(((x) >> BIT_SHIFT_DR14) & BIT_MASK_DR14)


//2 REG_SPIC_DR15

#define BIT_SHIFT_DR15 0
#define BIT_MASK_DR15 0xffffffffL
#define BIT_DR15(x)                           	(((x) & BIT_MASK_DR15) << BIT_SHIFT_DR15)
#define BIT_CTRL_DR15(x)                      	(((x) & BIT_MASK_DR15) << BIT_SHIFT_DR15)
#define BIT_GET_DR15(x)                       	(((x) >> BIT_SHIFT_DR15) & BIT_MASK_DR15)


//2 REG_SPIC_DR16

#define BIT_SHIFT_DR16 0
#define BIT_MASK_DR16 0xffffffffL
#define BIT_DR16(x)                           	(((x) & BIT_MASK_DR16) << BIT_SHIFT_DR16)
#define BIT_CTRL_DR16(x)                      	(((x) & BIT_MASK_DR16) << BIT_SHIFT_DR16)
#define BIT_GET_DR16(x)                       	(((x) >> BIT_SHIFT_DR16) & BIT_MASK_DR16)


//2 REG_SPIC_DR17

#define BIT_SHIFT_DR17 0
#define BIT_MASK_DR17 0xffffffffL
#define BIT_DR17(x)                           	(((x) & BIT_MASK_DR17) << BIT_SHIFT_DR17)
#define BIT_CTRL_DR17(x)                      	(((x) & BIT_MASK_DR17) << BIT_SHIFT_DR17)
#define BIT_GET_DR17(x)                       	(((x) >> BIT_SHIFT_DR17) & BIT_MASK_DR17)


//2 REG_SPIC_DR18

#define BIT_SHIFT_DR18 0
#define BIT_MASK_DR18 0xffffffffL
#define BIT_DR18(x)                           	(((x) & BIT_MASK_DR18) << BIT_SHIFT_DR18)
#define BIT_CTRL_DR18(x)                      	(((x) & BIT_MASK_DR18) << BIT_SHIFT_DR18)
#define BIT_GET_DR18(x)                       	(((x) >> BIT_SHIFT_DR18) & BIT_MASK_DR18)


//2 REG_SPIC_DR19

#define BIT_SHIFT_DR19 0
#define BIT_MASK_DR19 0xffffffffL
#define BIT_DR19(x)                           	(((x) & BIT_MASK_DR19) << BIT_SHIFT_DR19)
#define BIT_CTRL_DR19(x)                      	(((x) & BIT_MASK_DR19) << BIT_SHIFT_DR19)
#define BIT_GET_DR19(x)                       	(((x) >> BIT_SHIFT_DR19) & BIT_MASK_DR19)


//2 REG_SPIC_DR20

#define BIT_SHIFT_DR20 0
#define BIT_MASK_DR20 0xffffffffL
#define BIT_DR20(x)                           	(((x) & BIT_MASK_DR20) << BIT_SHIFT_DR20)
#define BIT_CTRL_DR20(x)                      	(((x) & BIT_MASK_DR20) << BIT_SHIFT_DR20)
#define BIT_GET_DR20(x)                       	(((x) >> BIT_SHIFT_DR20) & BIT_MASK_DR20)


//2 REG_SPIC_DR21

#define BIT_SHIFT_DR21 0
#define BIT_MASK_DR21 0xffffffffL
#define BIT_DR21(x)                           	(((x) & BIT_MASK_DR21) << BIT_SHIFT_DR21)
#define BIT_CTRL_DR21(x)                      	(((x) & BIT_MASK_DR21) << BIT_SHIFT_DR21)
#define BIT_GET_DR21(x)                       	(((x) >> BIT_SHIFT_DR21) & BIT_MASK_DR21)


//2 REG_SPIC_DR22

#define BIT_SHIFT_DR22 0
#define BIT_MASK_DR22 0xffffffffL
#define BIT_DR22(x)                           	(((x) & BIT_MASK_DR22) << BIT_SHIFT_DR22)
#define BIT_CTRL_DR22(x)                      	(((x) & BIT_MASK_DR22) << BIT_SHIFT_DR22)
#define BIT_GET_DR22(x)                       	(((x) >> BIT_SHIFT_DR22) & BIT_MASK_DR22)


//2 REG_SPIC_DR23

#define BIT_SHIFT_DR23 0
#define BIT_MASK_DR23 0xffffffffL
#define BIT_DR23(x)                           	(((x) & BIT_MASK_DR23) << BIT_SHIFT_DR23)
#define BIT_CTRL_DR23(x)                      	(((x) & BIT_MASK_DR23) << BIT_SHIFT_DR23)
#define BIT_GET_DR23(x)                       	(((x) >> BIT_SHIFT_DR23) & BIT_MASK_DR23)


//2 REG_SPIC_DR24

#define BIT_SHIFT_DR24 0
#define BIT_MASK_DR24 0xffffffffL
#define BIT_DR24(x)                           	(((x) & BIT_MASK_DR24) << BIT_SHIFT_DR24)
#define BIT_CTRL_DR24(x)                      	(((x) & BIT_MASK_DR24) << BIT_SHIFT_DR24)
#define BIT_GET_DR24(x)                       	(((x) >> BIT_SHIFT_DR24) & BIT_MASK_DR24)


//2 REG_SPIC_DR25

#define BIT_SHIFT_DR25 0
#define BIT_MASK_DR25 0xffffffffL
#define BIT_DR25(x)                           	(((x) & BIT_MASK_DR25) << BIT_SHIFT_DR25)
#define BIT_CTRL_DR25(x)                      	(((x) & BIT_MASK_DR25) << BIT_SHIFT_DR25)
#define BIT_GET_DR25(x)                       	(((x) >> BIT_SHIFT_DR25) & BIT_MASK_DR25)


//2 REG_SPIC_DR26

#define BIT_SHIFT_DR26 0
#define BIT_MASK_DR26 0xffffffffL
#define BIT_DR26(x)                           	(((x) & BIT_MASK_DR26) << BIT_SHIFT_DR26)
#define BIT_CTRL_DR26(x)                      	(((x) & BIT_MASK_DR26) << BIT_SHIFT_DR26)
#define BIT_GET_DR26(x)                       	(((x) >> BIT_SHIFT_DR26) & BIT_MASK_DR26)


//2 REG_SPIC_DR27

#define BIT_SHIFT_DR27 0
#define BIT_MASK_DR27 0xffffffffL
#define BIT_DR27(x)                           	(((x) & BIT_MASK_DR27) << BIT_SHIFT_DR27)
#define BIT_CTRL_DR27(x)                      	(((x) & BIT_MASK_DR27) << BIT_SHIFT_DR27)
#define BIT_GET_DR27(x)                       	(((x) >> BIT_SHIFT_DR27) & BIT_MASK_DR27)


//2 REG_SPIC_DR28

#define BIT_SHIFT_DR28 0
#define BIT_MASK_DR28 0xffffffffL
#define BIT_DR28(x)                           	(((x) & BIT_MASK_DR28) << BIT_SHIFT_DR28)
#define BIT_CTRL_DR28(x)                      	(((x) & BIT_MASK_DR28) << BIT_SHIFT_DR28)
#define BIT_GET_DR28(x)                       	(((x) >> BIT_SHIFT_DR28) & BIT_MASK_DR28)


//2 REG_SPIC_DR29

#define BIT_SHIFT_DR29 0
#define BIT_MASK_DR29 0xffffffffL
#define BIT_DR29(x)                           	(((x) & BIT_MASK_DR29) << BIT_SHIFT_DR29)
#define BIT_CTRL_DR29(x)                      	(((x) & BIT_MASK_DR29) << BIT_SHIFT_DR29)
#define BIT_GET_DR29(x)                       	(((x) >> BIT_SHIFT_DR29) & BIT_MASK_DR29)


//2 REG_SPIC_DR30

#define BIT_SHIFT_DR30 0
#define BIT_MASK_DR30 0xffffffffL
#define BIT_DR30(x)                           	(((x) & BIT_MASK_DR30) << BIT_SHIFT_DR30)
#define BIT_CTRL_DR30(x)                      	(((x) & BIT_MASK_DR30) << BIT_SHIFT_DR30)
#define BIT_GET_DR30(x)                       	(((x) >> BIT_SHIFT_DR30) & BIT_MASK_DR30)


//2 REG_SPIC_DR31

#define BIT_SHIFT_DR31 0
#define BIT_MASK_DR31 0xffffffffL
#define BIT_DR31(x)                           	(((x) & BIT_MASK_DR31) << BIT_SHIFT_DR31)
#define BIT_CTRL_DR31(x)                      	(((x) & BIT_MASK_DR31) << BIT_SHIFT_DR31)
#define BIT_GET_DR31(x)                       	(((x) >> BIT_SHIFT_DR31) & BIT_MASK_DR31)


//2 REG_SPIC_READ_FAST_SINGLE

#define BIT_SHIFT_FRD_CMD 0
#define BIT_MASK_FRD_CMD 0xff
#define BIT_FRD_CMD(x)                        	(((x) & BIT_MASK_FRD_CMD) << BIT_SHIFT_FRD_CMD)
#define BIT_CTRL_FRD_CMD(x)                   	(((x) & BIT_MASK_FRD_CMD) << BIT_SHIFT_FRD_CMD)
#define BIT_GET_FRD_CMD(x)                    	(((x) >> BIT_SHIFT_FRD_CMD) & BIT_MASK_FRD_CMD)


//2 REG_SPIC_READ_DUAL_DATA

#define BIT_SHIFT_RD_DUAL_O_CMD 0
#define BIT_MASK_RD_DUAL_O_CMD 0xff
#define BIT_RD_DUAL_O_CMD(x)                  	(((x) & BIT_MASK_RD_DUAL_O_CMD) << BIT_SHIFT_RD_DUAL_O_CMD)
#define BIT_CTRL_RD_DUAL_O_CMD(x)             	(((x) & BIT_MASK_RD_DUAL_O_CMD) << BIT_SHIFT_RD_DUAL_O_CMD)
#define BIT_GET_RD_DUAL_O_CMD(x)              	(((x) >> BIT_SHIFT_RD_DUAL_O_CMD) & BIT_MASK_RD_DUAL_O_CMD)


//2 REG_SPIC_READ_DUAL_ADDR_DATA

#define BIT_SHIFT_RD_DUAL_IO_CMD 0
#define BIT_MASK_RD_DUAL_IO_CMD 0xff
#define BIT_RD_DUAL_IO_CMD(x)                 	(((x) & BIT_MASK_RD_DUAL_IO_CMD) << BIT_SHIFT_RD_DUAL_IO_CMD)
#define BIT_CTRL_RD_DUAL_IO_CMD(x)            	(((x) & BIT_MASK_RD_DUAL_IO_CMD) << BIT_SHIFT_RD_DUAL_IO_CMD)
#define BIT_GET_RD_DUAL_IO_CMD(x)             	(((x) >> BIT_SHIFT_RD_DUAL_IO_CMD) & BIT_MASK_RD_DUAL_IO_CMD)


//2 REG_SPIC_READ_QUAD_DATA

#define BIT_SHIFT_RD_QUAD_O_CMD 0
#define BIT_MASK_RD_QUAD_O_CMD 0xff
#define BIT_RD_QUAD_O_CMD(x)                  	(((x) & BIT_MASK_RD_QUAD_O_CMD) << BIT_SHIFT_RD_QUAD_O_CMD)
#define BIT_CTRL_RD_QUAD_O_CMD(x)             	(((x) & BIT_MASK_RD_QUAD_O_CMD) << BIT_SHIFT_RD_QUAD_O_CMD)
#define BIT_GET_RD_QUAD_O_CMD(x)              	(((x) >> BIT_SHIFT_RD_QUAD_O_CMD) & BIT_MASK_RD_QUAD_O_CMD)


//2 REG_SPIC_READ_QUAD_ADDR_DATA

#define BIT_SHIFT_RD_QUAD_IO_CMD 0
#define BIT_MASK_RD_QUAD_IO_CMD 0xff
#define BIT_RD_QUAD_IO_CMD(x)                 	(((x) & BIT_MASK_RD_QUAD_IO_CMD) << BIT_SHIFT_RD_QUAD_IO_CMD)
#define BIT_CTRL_RD_QUAD_IO_CMD(x)            	(((x) & BIT_MASK_RD_QUAD_IO_CMD) << BIT_SHIFT_RD_QUAD_IO_CMD)
#define BIT_GET_RD_QUAD_IO_CMD(x)             	(((x) >> BIT_SHIFT_RD_QUAD_IO_CMD) & BIT_MASK_RD_QUAD_IO_CMD)


//2 REG_SPIC_WRITE_SIGNLE

#define BIT_SHIFT_WR_CMD 0
#define BIT_MASK_WR_CMD 0xff
#define BIT_WR_CMD(x)                         	(((x) & BIT_MASK_WR_CMD) << BIT_SHIFT_WR_CMD)
#define BIT_CTRL_WR_CMD(x)                    	(((x) & BIT_MASK_WR_CMD) << BIT_SHIFT_WR_CMD)
#define BIT_GET_WR_CMD(x)                     	(((x) >> BIT_SHIFT_WR_CMD) & BIT_MASK_WR_CMD)


//2 REG_SPIC_WRITE_DUAL_DATA

#define BIT_SHIFT_WR_DUAL_I_CMD 0
#define BIT_MASK_WR_DUAL_I_CMD 0xff
#define BIT_WR_DUAL_I_CMD(x)                  	(((x) & BIT_MASK_WR_DUAL_I_CMD) << BIT_SHIFT_WR_DUAL_I_CMD)
#define BIT_CTRL_WR_DUAL_I_CMD(x)             	(((x) & BIT_MASK_WR_DUAL_I_CMD) << BIT_SHIFT_WR_DUAL_I_CMD)
#define BIT_GET_WR_DUAL_I_CMD(x)              	(((x) >> BIT_SHIFT_WR_DUAL_I_CMD) & BIT_MASK_WR_DUAL_I_CMD)


//2 REG_SPIC_WRITE_DUAL_ADDR_DATA

#define BIT_SHIFT_WR_DUAL_II_CMD 0
#define BIT_MASK_WR_DUAL_II_CMD 0xff
#define BIT_WR_DUAL_II_CMD(x)                 	(((x) & BIT_MASK_WR_DUAL_II_CMD) << BIT_SHIFT_WR_DUAL_II_CMD)
#define BIT_CTRL_WR_DUAL_II_CMD(x)            	(((x) & BIT_MASK_WR_DUAL_II_CMD) << BIT_SHIFT_WR_DUAL_II_CMD)
#define BIT_GET_WR_DUAL_II_CMD(x)             	(((x) >> BIT_SHIFT_WR_DUAL_II_CMD) & BIT_MASK_WR_DUAL_II_CMD)


//2 REG_SPIC_WRITE_QUAD_DATA

#define BIT_SHIFT_WR_QUAD_I_CMD 0
#define BIT_MASK_WR_QUAD_I_CMD 0xff
#define BIT_WR_QUAD_I_CMD(x)                  	(((x) & BIT_MASK_WR_QUAD_I_CMD) << BIT_SHIFT_WR_QUAD_I_CMD)
#define BIT_CTRL_WR_QUAD_I_CMD(x)             	(((x) & BIT_MASK_WR_QUAD_I_CMD) << BIT_SHIFT_WR_QUAD_I_CMD)
#define BIT_GET_WR_QUAD_I_CMD(x)              	(((x) >> BIT_SHIFT_WR_QUAD_I_CMD) & BIT_MASK_WR_QUAD_I_CMD)


//2 REG_SPIC_WRITE_QUAD_ADDR_DATA

#define BIT_SHIFT_WR_QUAD_II_CMD 0
#define BIT_MASK_WR_QUAD_II_CMD 0xff
#define BIT_WR_QUAD_II_CMD(x)                 	(((x) & BIT_MASK_WR_QUAD_II_CMD) << BIT_SHIFT_WR_QUAD_II_CMD)
#define BIT_CTRL_WR_QUAD_II_CMD(x)            	(((x) & BIT_MASK_WR_QUAD_II_CMD) << BIT_SHIFT_WR_QUAD_II_CMD)
#define BIT_GET_WR_QUAD_II_CMD(x)             	(((x) >> BIT_SHIFT_WR_QUAD_II_CMD) & BIT_MASK_WR_QUAD_II_CMD)


//2 REG_SPIC_WRITE_ENABLE

#define BIT_SHIFT_WR_EN_CMD 0
#define BIT_MASK_WR_EN_CMD 0xff
#define BIT_WR_EN_CMD(x)                      	(((x) & BIT_MASK_WR_EN_CMD) << BIT_SHIFT_WR_EN_CMD)
#define BIT_CTRL_WR_EN_CMD(x)                 	(((x) & BIT_MASK_WR_EN_CMD) << BIT_SHIFT_WR_EN_CMD)
#define BIT_GET_WR_EN_CMD(x)                  	(((x) >> BIT_SHIFT_WR_EN_CMD) & BIT_MASK_WR_EN_CMD)


//2 REG_SPIC_READ_STATUS

#define BIT_SHIFT_RD_ST_CMD 0
#define BIT_MASK_RD_ST_CMD 0xff
#define BIT_RD_ST_CMD(x)                      	(((x) & BIT_MASK_RD_ST_CMD) << BIT_SHIFT_RD_ST_CMD)
#define BIT_CTRL_RD_ST_CMD(x)                 	(((x) & BIT_MASK_RD_ST_CMD) << BIT_SHIFT_RD_ST_CMD)
#define BIT_GET_RD_ST_CMD(x)                  	(((x) >> BIT_SHIFT_RD_ST_CMD) & BIT_MASK_RD_ST_CMD)


//2 REG_SPIC_CTRLR2

#define BIT_SHIFT_FIFO_ENTRY 4
#define BIT_MASK_FIFO_ENTRY 0xf
#define BIT_FIFO_ENTRY(x)                     	(((x) & BIT_MASK_FIFO_ENTRY) << BIT_SHIFT_FIFO_ENTRY)
#define BIT_CTRL_FIFO_ENTRY(x)                	(((x) & BIT_MASK_FIFO_ENTRY) << BIT_SHIFT_FIFO_ENTRY)
#define BIT_GET_FIFO_ENTRY(x)                 	(((x) >> BIT_SHIFT_FIFO_ENTRY) & BIT_MASK_FIFO_ENTRY)

#define BIT_WR_SEQ                            	BIT(3)
#define BIT_SHIFT_WR_SEQ                      	3
#define BIT_MASK_WR_SEQ                       	0x1
#define BIT_CTRL_WR_SEQ(x)                    	(((x) & BIT_MASK_WR_SEQ) << BIT_SHIFT_WR_SEQ)

#define BIT_WPN_DNUM                          	BIT(2)
#define BIT_SHIFT_WPN_DNUM                    	2
#define BIT_MASK_WPN_DNUM                     	0x1
#define BIT_CTRL_WPN_DNUM(x)                  	(((x) & BIT_MASK_WPN_DNUM) << BIT_SHIFT_WPN_DNUM)

#define BIT_WPN_SET                           	BIT(1)
#define BIT_SHIFT_WPN_SET                     	1
#define BIT_MASK_WPN_SET                      	0x1
#define BIT_CTRL_WPN_SET(x)                   	(((x) & BIT_MASK_WPN_SET) << BIT_SHIFT_WPN_SET)

#define BIT_SO_DUM                            	BIT(0)
#define BIT_SHIFT_SO_DUM                      	0
#define BIT_MASK_SO_DUM                       	0x1
#define BIT_CTRL_SO_DUM(x)                    	(((x) & BIT_MASK_SO_DUM) << BIT_SHIFT_SO_DUM)

//2 REG_SPIC_FBAUDR

#define BIT_SHIFT_FSCKDV 0
#define BIT_MASK_FSCKDV 0xfff
#define BIT_FSCKDV(x)                         	(((x) & BIT_MASK_FSCKDV) << BIT_SHIFT_FSCKDV)
#define BIT_CTRL_FSCKDV(x)                    	(((x) & BIT_MASK_FSCKDV) << BIT_SHIFT_FSCKDV)
#define BIT_GET_FSCKDV(x)                     	(((x) >> BIT_SHIFT_FSCKDV) & BIT_MASK_FSCKDV)


//2 REG_SPIC_ADDR_LENGTH

#define BIT_SHIFT_ADDR_PHASE_LENGTH 0
#define BIT_MASK_ADDR_PHASE_LENGTH 0x3
#define BIT_ADDR_PHASE_LENGTH(x)              	(((x) & BIT_MASK_ADDR_PHASE_LENGTH) << BIT_SHIFT_ADDR_PHASE_LENGTH)
#define BIT_CTRL_ADDR_PHASE_LENGTH(x)         	(((x) & BIT_MASK_ADDR_PHASE_LENGTH) << BIT_SHIFT_ADDR_PHASE_LENGTH)
#define BIT_GET_ADDR_PHASE_LENGTH(x)          	(((x) >> BIT_SHIFT_ADDR_PHASE_LENGTH) & BIT_MASK_ADDR_PHASE_LENGTH)


//2 REG_SPIC_AUTO_LENGTH

#define BIT_SHIFT_CS_H_WR_DUM_LEN 28
#define BIT_MASK_CS_H_WR_DUM_LEN 0xf
#define BIT_CS_H_WR_DUM_LEN(x)                	(((x) & BIT_MASK_CS_H_WR_DUM_LEN) << BIT_SHIFT_CS_H_WR_DUM_LEN)
#define BIT_CTRL_CS_H_WR_DUM_LEN(x)           	(((x) & BIT_MASK_CS_H_WR_DUM_LEN) << BIT_SHIFT_CS_H_WR_DUM_LEN)
#define BIT_GET_CS_H_WR_DUM_LEN(x)            	(((x) >> BIT_SHIFT_CS_H_WR_DUM_LEN) & BIT_MASK_CS_H_WR_DUM_LEN)


#define BIT_SHIFT_CS_H_RD_DUM_LEN 26
#define BIT_MASK_CS_H_RD_DUM_LEN 0x3
#define BIT_CS_H_RD_DUM_LEN(x)                	(((x) & BIT_MASK_CS_H_RD_DUM_LEN) << BIT_SHIFT_CS_H_RD_DUM_LEN)
#define BIT_CTRL_CS_H_RD_DUM_LEN(x)           	(((x) & BIT_MASK_CS_H_RD_DUM_LEN) << BIT_SHIFT_CS_H_RD_DUM_LEN)
#define BIT_GET_CS_H_RD_DUM_LEN(x)            	(((x) >> BIT_SHIFT_CS_H_RD_DUM_LEN) & BIT_MASK_CS_H_RD_DUM_LEN)


#define BIT_SHIFT_AUTO_DUM_LEN 18
#define BIT_MASK_AUTO_DUM_LEN 0xff
#define BIT_AUTO_DUM_LEN(x)                   	(((x) & BIT_MASK_AUTO_DUM_LEN) << BIT_SHIFT_AUTO_DUM_LEN)
#define BIT_CTRL_AUTO_DUM_LEN(x)              	(((x) & BIT_MASK_AUTO_DUM_LEN) << BIT_SHIFT_AUTO_DUM_LEN)
#define BIT_GET_AUTO_DUM_LEN(x)               	(((x) >> BIT_SHIFT_AUTO_DUM_LEN) & BIT_MASK_AUTO_DUM_LEN)


#define BIT_SHIFT_AUTO_ADDR__LENGTH 16
#define BIT_MASK_AUTO_ADDR__LENGTH 0x3
#define BIT_AUTO_ADDR__LENGTH(x)              	(((x) & BIT_MASK_AUTO_ADDR__LENGTH) << BIT_SHIFT_AUTO_ADDR__LENGTH)
#define BIT_CTRL_AUTO_ADDR__LENGTH(x)         	(((x) & BIT_MASK_AUTO_ADDR__LENGTH) << BIT_SHIFT_AUTO_ADDR__LENGTH)
#define BIT_GET_AUTO_ADDR__LENGTH(x)          	(((x) >> BIT_SHIFT_AUTO_ADDR__LENGTH) & BIT_MASK_AUTO_ADDR__LENGTH)


#define BIT_SHIFT_RD_DUMMY_LENGTH 0
#define BIT_MASK_RD_DUMMY_LENGTH 0xffff
#define BIT_RD_DUMMY_LENGTH(x)                	(((x) & BIT_MASK_RD_DUMMY_LENGTH) << BIT_SHIFT_RD_DUMMY_LENGTH)
#define BIT_CTRL_RD_DUMMY_LENGTH(x)           	(((x) & BIT_MASK_RD_DUMMY_LENGTH) << BIT_SHIFT_RD_DUMMY_LENGTH)
#define BIT_GET_RD_DUMMY_LENGTH(x)            	(((x) >> BIT_SHIFT_RD_DUMMY_LENGTH) & BIT_MASK_RD_DUMMY_LENGTH)


//2 REG_SPIC_VALID_CMD
#define BIT_WR_BLOCKING                       	BIT(9)
#define BIT_SHIFT_WR_BLOCKING                 	9
#define BIT_MASK_WR_BLOCKING                  	0x1
#define BIT_CTRL_WR_BLOCKING(x)               	(((x) & BIT_MASK_WR_BLOCKING) << BIT_SHIFT_WR_BLOCKING)

#define BIT_WR_QUAD_II                        	BIT(8)
#define BIT_SHIFT_WR_QUAD_II                  	8
#define BIT_MASK_WR_QUAD_II                   	0x1
#define BIT_CTRL_WR_QUAD_II(x)                	(((x) & BIT_MASK_WR_QUAD_II) << BIT_SHIFT_WR_QUAD_II)

#define BIT_WR_QUAD_I                         	BIT(7)
#define BIT_SHIFT_WR_QUAD_I                   	7
#define BIT_MASK_WR_QUAD_I                    	0x1
#define BIT_CTRL_WR_QUAD_I(x)                 	(((x) & BIT_MASK_WR_QUAD_I) << BIT_SHIFT_WR_QUAD_I)

#define BIT_WR_DUAL_II                        	BIT(6)
#define BIT_SHIFT_WR_DUAL_II                  	6
#define BIT_MASK_WR_DUAL_II                   	0x1
#define BIT_CTRL_WR_DUAL_II(x)                	(((x) & BIT_MASK_WR_DUAL_II) << BIT_SHIFT_WR_DUAL_II)

#define BIT_WR_DUAL_I                         	BIT(5)
#define BIT_SHIFT_WR_DUAL_I                   	5
#define BIT_MASK_WR_DUAL_I                    	0x1
#define BIT_CTRL_WR_DUAL_I(x)                 	(((x) & BIT_MASK_WR_DUAL_I) << BIT_SHIFT_WR_DUAL_I)

#define BIT_RD_QUAD_IO                        	BIT(4)
#define BIT_SHIFT_RD_QUAD_IO                  	4
#define BIT_MASK_RD_QUAD_IO                   	0x1
#define BIT_CTRL_RD_QUAD_IO(x)                	(((x) & BIT_MASK_RD_QUAD_IO) << BIT_SHIFT_RD_QUAD_IO)

#define BIT_RD_QUAD_O                         	BIT(3)
#define BIT_SHIFT_RD_QUAD_O                   	3
#define BIT_MASK_RD_QUAD_O                    	0x1
#define BIT_CTRL_RD_QUAD_O(x)                 	(((x) & BIT_MASK_RD_QUAD_O) << BIT_SHIFT_RD_QUAD_O)

#define BIT_RD_DUAL_IO                        	BIT(2)
#define BIT_SHIFT_RD_DUAL_IO                  	2
#define BIT_MASK_RD_DUAL_IO                   	0x1
#define BIT_CTRL_RD_DUAL_IO(x)                	(((x) & BIT_MASK_RD_DUAL_IO) << BIT_SHIFT_RD_DUAL_IO)

#define BIT_RD_DUAL_I                         	BIT(1)
#define BIT_SHIFT_RD_DUAL_I                   	1
#define BIT_MASK_RD_DUAL_I                    	0x1
#define BIT_CTRL_RD_DUAL_I(x)                 	(((x) & BIT_MASK_RD_DUAL_I) << BIT_SHIFT_RD_DUAL_I)

#define BIT_FRD_SINGEL                        	BIT(0)
#define BIT_SHIFT_FRD_SINGEL                  	0
#define BIT_MASK_FRD_SINGEL                   	0x1
#define BIT_CTRL_FRD_SINGEL(x)                	(((x) & BIT_MASK_FRD_SINGEL) << BIT_SHIFT_FRD_SINGEL)

//2 REG_SPIC_FLASE_SIZE

#define BIT_SHIFT_FLASE_SIZE 0
#define BIT_MASK_FLASE_SIZE 0xf
#define BIT_FLASE_SIZE(x)                     	(((x) & BIT_MASK_FLASE_SIZE) << BIT_SHIFT_FLASE_SIZE)
#define BIT_CTRL_FLASE_SIZE(x)                	(((x) & BIT_MASK_FLASE_SIZE) << BIT_SHIFT_FLASE_SIZE)
#define BIT_GET_FLASE_SIZE(x)                 	(((x) >> BIT_SHIFT_FLASE_SIZE) & BIT_MASK_FLASE_SIZE)


//2 REG_SPIC_FLUSH_FIFO
#define BIT_FLUSH_FIFO                        	BIT(0)
#define BIT_SHIFT_FLUSH_FIFO                  	0
#define BIT_MASK_FLUSH_FIFO                   	0x1
#define BIT_CTRL_FLUSH_FIFO(x)                	(((x) & BIT_MASK_FLUSH_FIFO) << BIT_SHIFT_FLUSH_FIFO)

//=================== Register Address Definition ============================//
#define REG_SPIC_CTRLR0 0x0000//O
#define REG_SPIC_CTRLR1 0x0004//O
#define REG_SPIC_SSIENR 0x0008//O
#define REG_SPIC_MWCR 0x000C
#define REG_SPIC_SER 0x0010//O
#define REG_SPIC_BAUDR 0x0014//O
#define REG_SPIC_TXFTLR 0x0018
#define REG_SPIC_RXFTLR 0x001C//O
#define REG_SPIC_TXFLR 0x0020//O
#define REG_SPIC_RXFLR 0x0024
#define REG_SPIC_SR 0x0028
#define REG_SPIC_IMR 0x002C//O
#define REG_SPIC_ISR 0x0030
#define REG_SPIC_RISR 0x0034
#define REG_SPIC_TXOICR 0x0038
#define REG_SPIC_RXOICR 0x003C
#define REG_SPC_RXUICR 0x0040
#define REG_SPIC_MSTICR 0x0044
#define REG_SPIC_ICR 0x0048
#define REG_SPIC_DMACR 0x004C
#define REG_SPIC_DMATDLR0 0x0050
#define REG_SPIC_DMATDLR1 0x0054
#define REG_SPIC_IDR 0x0058
#define REG_SPIC_VERSION 0x005C
#define REG_SPIC_DR0 0x0060
#define REG_SPIC_DR1 0x0064
#define REG_SPIC_DR2 0x0068
#define REG_SPIC_DR3 0x006C
#define REG_SPIC_DR4 0x0070
#define REG_SPIC_DR5 0x0074
#define REG_SPIC_DR6 0x0078
#define REG_SPIC_DR7 0x007C
#define REG_SPIC_DR8 0x0080
#define REG_SPIC_DR9 0x0084
#define REG_SPIC_DR10 0x0088
#define REG_SPIC_DR11 0x008C
#define REG_SPIC_DR12 0x0090
#define REG_SPIC_DR13 0x0094
#define REG_SPIC_DR14 0x0098
#define REG_SPIC_DR15 0x009C
#define REG_SPIC_DR16 0x00A0
#define REG_SPIC_DR17 0x00A4
#define REG_SPIC_DR18 0x00A8
#define REG_SPIC_DR19 0x00AC
#define REG_SPIC_DR20 0x00B0
#define REG_SPIC_DR21 0x00B4
#define REG_SPIC_DR22 0x00B8
#define REG_SPIC_DR23 0x00BC
#define REG_SPIC_DR24 0x00C0
#define REG_SPIC_DR25 0x00C4
#define REG_SPIC_DR26 0x00C8
#define REG_SPIC_DR27 0x00CC
#define REG_SPIC_DR28 0x00D0
#define REG_SPIC_DR29 0x00D4
#define REG_SPIC_DR30 0x00D8
#define REG_SPIC_DR31 0x00DC
#define REG_SPIC_READ_FAST_SINGLE 0x00E0//O
#define REG_SPIC_READ_DUAL_DATA 0x00E4//O
#define REG_SPIC_READ_DUAL_ADDR_DATA 0x00E8//O
#define REG_SPIC_READ_QUAD_DATA 0x00EC//O
#define REG_SPIC_READ_QUAD_ADDR_DATA 0x00F0//O
#define REG_SPIC_WRITE_SIGNLE 0x00F4//O
#define REG_SPIC_WRITE_DUAL_DATA 0x00F8//O
#define REG_SPIC_WRITE_DUAL_ADDR_DATA 0x00FC//O
#define REG_SPIC_WRITE_QUAD_DATA 0x0100//O
#define REG_SPIC_WRITE_QUAD_ADDR_DATA 0x0104//O
#define REG_SPIC_WRITE_ENABLE 0x0108//O
#define REG_SPIC_READ_STATUS 0x010C//O
#define REG_SPIC_CTRLR2 0x0110//O
#define REG_SPIC_FBAUDR 0x0114//O
#define REG_SPIC_ADDR_LENGTH 0x0118//O
#define REG_SPIC_AUTO_LENGTH 0x011C//O
#define REG_SPIC_VALID_CMD 0x0120//O
#define REG_SPIC_FLASE_SIZE 0x0124//O
#define REG_SPIC_FLUSH_FIFO 0x0128//O

#endif  // end of "#ifndef _RTL8195A_SPI_FLASH_H"
