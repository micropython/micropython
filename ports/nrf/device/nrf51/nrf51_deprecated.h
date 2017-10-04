/* Copyright (c) 2016, Nordic Semiconductor ASA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   * Neither the name of Nordic Semiconductor ASA nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
 
#ifndef NRF51_DEPRECATED_H
#define NRF51_DEPRECATED_H

/*lint ++flb "Enter library region */

/* This file is given to prevent your SW from not compiling with the updates made to nrf51.h and 
 * nrf51_bitfields.h. The macros defined in this file were available previously. Do not use these
 * macros on purpose. Use the ones defined in nrf51.h and nrf51_bitfields.h instead.
 */

/* NVMC */
/* The register ERASEPROTECTEDPAGE is called ERASEPCR0 in the documentation. */
#define ERASEPROTECTEDPAGE   ERASEPCR0

 
/* LPCOMP */
/* The interrupt ISR was renamed. Adding old name to the macros. */
#define LPCOMP_COMP_IRQHandler          LPCOMP_IRQHandler
#define LPCOMP_COMP_IRQn                LPCOMP_IRQn
/* Corrected typo in RESULT register. */
#define LPCOMP_RESULT_RESULT_Bellow     LPCOMP_RESULT_RESULT_Below
 
 
/* MPU */
/* The field MPU.PERR0.LPCOMP_COMP was renamed. Added into deprecated in case somebody was using the macros defined for it. */
#define MPU_PERR0_LPCOMP_COMP_Pos           MPU_PERR0_LPCOMP_Pos
#define MPU_PERR0_LPCOMP_COMP_Msk           MPU_PERR0_LPCOMP_Msk
#define MPU_PERR0_LPCOMP_COMP_InRegion1     MPU_PERR0_LPCOMP_InRegion1
#define MPU_PERR0_LPCOMP_COMP_InRegion0     MPU_PERR0_LPCOMP_InRegion0
 
 
/* POWER */
/* The field POWER.RAMON.OFFRAM3 was eliminated. Added into deprecated in case somebody was using the macros defined for it. */
#define POWER_RAMON_OFFRAM3_Pos         (19UL)                                  
#define POWER_RAMON_OFFRAM3_Msk         (0x1UL << POWER_RAMON_OFFRAM3_Pos)      
#define POWER_RAMON_OFFRAM3_RAM3Off     (0UL)                                   
#define POWER_RAMON_OFFRAM3_RAM3On      (1UL)                                   
/* The field POWER.RAMON.OFFRAM2 was eliminated. Added into deprecated in case somebody was using the macros defined for it. */
#define POWER_RAMON_OFFRAM2_Pos         (18UL)                                  
#define POWER_RAMON_OFFRAM2_Msk         (0x1UL << POWER_RAMON_OFFRAM2_Pos)      
#define POWER_RAMON_OFFRAM2_RAM2Off     (0UL)                                   
#define POWER_RAMON_OFFRAM2_RAM2On      (1UL)                                  
/* The field POWER.RAMON.ONRAM3 was eliminated. Added into deprecated in case somebody was using the macros defined for it. */
#define POWER_RAMON_ONRAM3_Pos          (3UL)                                  
#define POWER_RAMON_ONRAM3_Msk          (0x1UL << POWER_RAMON_ONRAM3_Pos)      
#define POWER_RAMON_ONRAM3_RAM3Off      (0UL)                                  
#define POWER_RAMON_ONRAM3_RAM3On       (1UL)                                  
/* The field POWER.RAMON.ONRAM2 was eliminated. Added into deprecated in case somebody was using the macros defined for it. */
#define POWER_RAMON_ONRAM2_Pos          (2UL)                                  
#define POWER_RAMON_ONRAM2_Msk          (0x1UL << POWER_RAMON_ONRAM2_Pos)       
#define POWER_RAMON_ONRAM2_RAM2Off      (0UL)                                  
#define POWER_RAMON_ONRAM2_RAM2On       (1UL)                                 

 
/* RADIO */
/* The enumerated value RADIO.TXPOWER.TXPOWER.Neg40dBm was renamed. Added into deprecated with the new macro name. */
#define RADIO_TXPOWER_TXPOWER_Neg40dBm  RADIO_TXPOWER_TXPOWER_Neg30dBm      
/* The name of the field SKIPADDR was corrected. Old macros added for compatibility. */
#define RADIO_CRCCNF_SKIP_ADDR_Pos      RADIO_CRCCNF_SKIPADDR_Pos 
#define RADIO_CRCCNF_SKIP_ADDR_Msk      RADIO_CRCCNF_SKIPADDR_Msk 
#define RADIO_CRCCNF_SKIP_ADDR_Include  RADIO_CRCCNF_SKIPADDR_Include 
#define RADIO_CRCCNF_SKIP_ADDR_Skip     RADIO_CRCCNF_SKIPADDR_Skip 
/* The name of the field PLLLOCK was corrected. Old macros added for compatibility. */
#define RADIO_TEST_PLL_LOCK_Pos         RADIO_TEST_PLLLOCK_Pos 
#define RADIO_TEST_PLL_LOCK_Msk         RADIO_TEST_PLLLOCK_Msk 
#define RADIO_TEST_PLL_LOCK_Disabled    RADIO_TEST_PLLLOCK_Disabled 
#define RADIO_TEST_PLL_LOCK_Enabled     RADIO_TEST_PLLLOCK_Enabled 
/* The name of the field CONSTCARRIER was corrected. Old macros added for compatibility. */
#define RADIO_TEST_CONST_CARRIER_Pos        RADIO_TEST_CONSTCARRIER_Pos 
#define RADIO_TEST_CONST_CARRIER_Msk        RADIO_TEST_CONSTCARRIER_Msk 
#define RADIO_TEST_CONST_CARRIER_Disabled   RADIO_TEST_CONSTCARRIER_Disabled 
#define RADIO_TEST_CONST_CARRIER_Enabled    RADIO_TEST_CONSTCARRIER_Enabled 


/* FICR */
/* The registers FICR.SIZERAMBLOCK0, FICR.SIZERAMBLOCK1, FICR.SIZERAMBLOCK2 and FICR.SIZERAMBLOCK3 were renamed into an array. */
#define SIZERAMBLOCK0   SIZERAMBLOCKS                   
#define SIZERAMBLOCK1   SIZERAMBLOCKS                   
#define SIZERAMBLOCK2   SIZERAMBLOCK[2]                 /*!< Note that this macro will disapear when SIZERAMBLOCK array is eliminated. SIZERAMBLOCK is a deprecated array. */
#define SIZERAMBLOCK3   SIZERAMBLOCK[3]                 /*!< Note that this macro will disapear when SIZERAMBLOCK array is eliminated. SIZERAMBLOCK is a deprecated array. */
/* The registers FICR.DEVICEID0 and FICR.DEVICEID1 were renamed into an array. */
#define DEVICEID0       DEVICEID[0]                     
#define DEVICEID1       DEVICEID[1]                     
/* The registers FICR.ER0, FICR.ER1, FICR.ER2 and FICR.ER3 were renamed into an array. */
#define ER0             ER[0]                           
#define ER1             ER[1]                          
#define ER2             ER[2]                       
#define ER3             ER[3]                      
/* The registers FICR.IR0, FICR.IR1, FICR.IR2 and FICR.IR3 were renamed into an array. */
#define IR0             IR[0]                         
#define IR1             IR[1]                         
#define IR2             IR[2]                         
#define IR3             IR[3]                          
/* The registers FICR.DEVICEADDR0 and FICR.DEVICEADDR1 were renamed into an array. */
#define DEVICEADDR0     DEVICEADDR[0]                  
#define DEVICEADDR1     DEVICEADDR[1]                  


/* PPI */
/* The tasks PPI.TASKS_CHGxEN and PPI.TASKS_CHGxDIS were renamed into an array of structs. */
#define TASKS_CHG0EN     TASKS_CHG[0].EN                    
#define TASKS_CHG0DIS    TASKS_CHG[0].DIS                  
#define TASKS_CHG1EN     TASKS_CHG[1].EN                    
#define TASKS_CHG1DIS    TASKS_CHG[1].DIS                  
#define TASKS_CHG2EN     TASKS_CHG[2].EN                   
#define TASKS_CHG2DIS    TASKS_CHG[2].DIS                  
#define TASKS_CHG3EN     TASKS_CHG[3].EN                    
#define TASKS_CHG3DIS    TASKS_CHG[3].DIS                  
/* The registers PPI.CHx_EEP and PPI.CHx_TEP were renamed into an array of structs. */
#define CH0_EEP          CH[0].EEP                          
#define CH0_TEP          CH[0].TEP                          
#define CH1_EEP          CH[1].EEP                         
#define CH1_TEP          CH[1].TEP                         
#define CH2_EEP          CH[2].EEP                          
#define CH2_TEP          CH[2].TEP                         
#define CH3_EEP          CH[3].EEP                          
#define CH3_TEP          CH[3].TEP                         
#define CH4_EEP          CH[4].EEP                         
#define CH4_TEP          CH[4].TEP                         
#define CH5_EEP          CH[5].EEP                          
#define CH5_TEP          CH[5].TEP                          
#define CH6_EEP          CH[6].EEP                          
#define CH6_TEP          CH[6].TEP                         
#define CH7_EEP          CH[7].EEP                          
#define CH7_TEP          CH[7].TEP                          
#define CH8_EEP          CH[8].EEP                         
#define CH8_TEP          CH[8].TEP                          
#define CH9_EEP          CH[9].EEP                          
#define CH9_TEP          CH[9].TEP                          
#define CH10_EEP         CH[10].EEP                         
#define CH10_TEP         CH[10].TEP                         
#define CH11_EEP         CH[11].EEP                         
#define CH11_TEP         CH[11].TEP                         
#define CH12_EEP         CH[12].EEP                         
#define CH12_TEP         CH[12].TEP                         
#define CH13_EEP         CH[13].EEP                         
#define CH13_TEP         CH[13].TEP                         
#define CH14_EEP         CH[14].EEP                         
#define CH14_TEP         CH[14].TEP                         
#define CH15_EEP         CH[15].EEP                         
#define CH15_TEP         CH[15].TEP                        
/* The registers PPI.CHG0, PPI.CHG1, PPI.CHG2 and PPI.CHG3 were renamed into an array. */
#define CHG0             CHG[0]                            
#define CHG1             CHG[1]                            
#define CHG2             CHG[2]                             
#define CHG3             CHG[3]                           
/* All bitfield macros for the CHGx registers therefore changed name. */
#define PPI_CHG0_CH15_Pos       PPI_CHG_CH15_Pos            
#define PPI_CHG0_CH15_Msk       PPI_CHG_CH15_Msk            
#define PPI_CHG0_CH15_Excluded  PPI_CHG_CH15_Excluded       
#define PPI_CHG0_CH15_Included  PPI_CHG_CH15_Included       
#define PPI_CHG0_CH14_Pos       PPI_CHG_CH14_Pos            
#define PPI_CHG0_CH14_Msk       PPI_CHG_CH14_Msk           
#define PPI_CHG0_CH14_Excluded  PPI_CHG_CH14_Excluded       
#define PPI_CHG0_CH14_Included  PPI_CHG_CH14_Included       
#define PPI_CHG0_CH13_Pos       PPI_CHG_CH13_Pos            
#define PPI_CHG0_CH13_Msk       PPI_CHG_CH13_Msk            
#define PPI_CHG0_CH13_Excluded  PPI_CHG_CH13_Excluded      
#define PPI_CHG0_CH13_Included  PPI_CHG_CH13_Included       
#define PPI_CHG0_CH12_Pos       PPI_CHG_CH12_Pos            
#define PPI_CHG0_CH12_Msk       PPI_CHG_CH12_Msk            
#define PPI_CHG0_CH12_Excluded  PPI_CHG_CH12_Excluded       
#define PPI_CHG0_CH12_Included  PPI_CHG_CH12_Included       
#define PPI_CHG0_CH11_Pos       PPI_CHG_CH11_Pos            
#define PPI_CHG0_CH11_Msk       PPI_CHG_CH11_Msk            
#define PPI_CHG0_CH11_Excluded  PPI_CHG_CH11_Excluded       
#define PPI_CHG0_CH11_Included  PPI_CHG_CH11_Included       
#define PPI_CHG0_CH10_Pos       PPI_CHG_CH10_Pos            
#define PPI_CHG0_CH10_Msk       PPI_CHG_CH10_Msk            
#define PPI_CHG0_CH10_Excluded  PPI_CHG_CH10_Excluded       
#define PPI_CHG0_CH10_Included  PPI_CHG_CH10_Included       
#define PPI_CHG0_CH9_Pos        PPI_CHG_CH9_Pos             
#define PPI_CHG0_CH9_Msk        PPI_CHG_CH9_Msk             
#define PPI_CHG0_CH9_Excluded   PPI_CHG_CH9_Excluded        
#define PPI_CHG0_CH9_Included   PPI_CHG_CH9_Included        
#define PPI_CHG0_CH8_Pos        PPI_CHG_CH8_Pos             
#define PPI_CHG0_CH8_Msk        PPI_CHG_CH8_Msk             
#define PPI_CHG0_CH8_Excluded   PPI_CHG_CH8_Excluded        
#define PPI_CHG0_CH8_Included   PPI_CHG_CH8_Included        
#define PPI_CHG0_CH7_Pos        PPI_CHG_CH7_Pos             
#define PPI_CHG0_CH7_Msk        PPI_CHG_CH7_Msk             
#define PPI_CHG0_CH7_Excluded   PPI_CHG_CH7_Excluded        
#define PPI_CHG0_CH7_Included   PPI_CHG_CH7_Included        
#define PPI_CHG0_CH6_Pos        PPI_CHG_CH6_Pos             
#define PPI_CHG0_CH6_Msk        PPI_CHG_CH6_Msk             
#define PPI_CHG0_CH6_Excluded   PPI_CHG_CH6_Excluded        
#define PPI_CHG0_CH6_Included   PPI_CHG_CH6_Included        
#define PPI_CHG0_CH5_Pos        PPI_CHG_CH5_Pos             
#define PPI_CHG0_CH5_Msk        PPI_CHG_CH5_Msk             
#define PPI_CHG0_CH5_Excluded   PPI_CHG_CH5_Excluded       
#define PPI_CHG0_CH5_Included   PPI_CHG_CH5_Included        
#define PPI_CHG0_CH4_Pos        PPI_CHG_CH4_Pos             
#define PPI_CHG0_CH4_Msk        PPI_CHG_CH4_Msk             
#define PPI_CHG0_CH4_Excluded   PPI_CHG_CH4_Excluded       
#define PPI_CHG0_CH4_Included   PPI_CHG_CH4_Included       
#define PPI_CHG0_CH3_Pos        PPI_CHG_CH3_Pos             
#define PPI_CHG0_CH3_Msk        PPI_CHG_CH3_Msk            
#define PPI_CHG0_CH3_Excluded   PPI_CHG_CH3_Excluded        
#define PPI_CHG0_CH3_Included   PPI_CHG_CH3_Included       
#define PPI_CHG0_CH2_Pos        PPI_CHG_CH2_Pos            
#define PPI_CHG0_CH2_Msk        PPI_CHG_CH2_Msk             
#define PPI_CHG0_CH2_Excluded   PPI_CHG_CH2_Excluded       
#define PPI_CHG0_CH2_Included   PPI_CHG_CH2_Included       
#define PPI_CHG0_CH1_Pos        PPI_CHG_CH1_Pos            
#define PPI_CHG0_CH1_Msk        PPI_CHG_CH1_Msk            
#define PPI_CHG0_CH1_Excluded   PPI_CHG_CH1_Excluded        
#define PPI_CHG0_CH1_Included   PPI_CHG_CH1_Included       
#define PPI_CHG0_CH0_Pos        PPI_CHG_CH0_Pos            
#define PPI_CHG0_CH0_Msk        PPI_CHG_CH0_Msk            
#define PPI_CHG0_CH0_Excluded   PPI_CHG_CH0_Excluded        
#define PPI_CHG0_CH0_Included   PPI_CHG_CH0_Included       
#define PPI_CHG1_CH15_Pos       PPI_CHG_CH15_Pos           
#define PPI_CHG1_CH15_Msk       PPI_CHG_CH15_Msk           
#define PPI_CHG1_CH15_Excluded  PPI_CHG_CH15_Excluded       
#define PPI_CHG1_CH15_Included  PPI_CHG_CH15_Included      
#define PPI_CHG1_CH14_Pos       PPI_CHG_CH14_Pos           
#define PPI_CHG1_CH14_Msk       PPI_CHG_CH14_Msk            
#define PPI_CHG1_CH14_Excluded  PPI_CHG_CH14_Excluded      
#define PPI_CHG1_CH14_Included  PPI_CHG_CH14_Included       
#define PPI_CHG1_CH13_Pos       PPI_CHG_CH13_Pos           
#define PPI_CHG1_CH13_Msk       PPI_CHG_CH13_Msk            
#define PPI_CHG1_CH13_Excluded  PPI_CHG_CH13_Excluded      
#define PPI_CHG1_CH13_Included  PPI_CHG_CH13_Included      
#define PPI_CHG1_CH12_Pos       PPI_CHG_CH12_Pos            
#define PPI_CHG1_CH12_Msk       PPI_CHG_CH12_Msk           
#define PPI_CHG1_CH12_Excluded  PPI_CHG_CH12_Excluded      
#define PPI_CHG1_CH12_Included  PPI_CHG_CH12_Included      
#define PPI_CHG1_CH11_Pos       PPI_CHG_CH11_Pos            
#define PPI_CHG1_CH11_Msk       PPI_CHG_CH11_Msk           
#define PPI_CHG1_CH11_Excluded  PPI_CHG_CH11_Excluded      
#define PPI_CHG1_CH11_Included  PPI_CHG_CH11_Included      
#define PPI_CHG1_CH10_Pos       PPI_CHG_CH10_Pos           
#define PPI_CHG1_CH10_Msk       PPI_CHG_CH10_Msk            
#define PPI_CHG1_CH10_Excluded  PPI_CHG_CH10_Excluded      
#define PPI_CHG1_CH10_Included  PPI_CHG_CH10_Included      
#define PPI_CHG1_CH9_Pos        PPI_CHG_CH9_Pos            
#define PPI_CHG1_CH9_Msk        PPI_CHG_CH9_Msk            
#define PPI_CHG1_CH9_Excluded   PPI_CHG_CH9_Excluded       
#define PPI_CHG1_CH9_Included   PPI_CHG_CH9_Included       
#define PPI_CHG1_CH8_Pos        PPI_CHG_CH8_Pos            
#define PPI_CHG1_CH8_Msk        PPI_CHG_CH8_Msk            
#define PPI_CHG1_CH8_Excluded   PPI_CHG_CH8_Excluded       
#define PPI_CHG1_CH8_Included   PPI_CHG_CH8_Included       
#define PPI_CHG1_CH7_Pos        PPI_CHG_CH7_Pos             
#define PPI_CHG1_CH7_Msk        PPI_CHG_CH7_Msk            
#define PPI_CHG1_CH7_Excluded   PPI_CHG_CH7_Excluded        
#define PPI_CHG1_CH7_Included   PPI_CHG_CH7_Included       
#define PPI_CHG1_CH6_Pos        PPI_CHG_CH6_Pos             
#define PPI_CHG1_CH6_Msk        PPI_CHG_CH6_Msk            
#define PPI_CHG1_CH6_Excluded   PPI_CHG_CH6_Excluded       
#define PPI_CHG1_CH6_Included   PPI_CHG_CH6_Included       
#define PPI_CHG1_CH5_Pos        PPI_CHG_CH5_Pos             
#define PPI_CHG1_CH5_Msk        PPI_CHG_CH5_Msk             
#define PPI_CHG1_CH5_Excluded   PPI_CHG_CH5_Excluded       
#define PPI_CHG1_CH5_Included   PPI_CHG_CH5_Included        
#define PPI_CHG1_CH4_Pos        PPI_CHG_CH4_Pos             
#define PPI_CHG1_CH4_Msk        PPI_CHG_CH4_Msk             
#define PPI_CHG1_CH4_Excluded   PPI_CHG_CH4_Excluded        
#define PPI_CHG1_CH4_Included   PPI_CHG_CH4_Included        
#define PPI_CHG1_CH3_Pos        PPI_CHG_CH3_Pos            
#define PPI_CHG1_CH3_Msk        PPI_CHG_CH3_Msk             
#define PPI_CHG1_CH3_Excluded   PPI_CHG_CH3_Excluded        
#define PPI_CHG1_CH3_Included   PPI_CHG_CH3_Included       
#define PPI_CHG1_CH2_Pos        PPI_CHG_CH2_Pos            
#define PPI_CHG1_CH2_Msk        PPI_CHG_CH2_Msk             
#define PPI_CHG1_CH2_Excluded   PPI_CHG_CH2_Excluded        
#define PPI_CHG1_CH2_Included   PPI_CHG_CH2_Included        
#define PPI_CHG1_CH1_Pos        PPI_CHG_CH1_Pos             
#define PPI_CHG1_CH1_Msk        PPI_CHG_CH1_Msk            
#define PPI_CHG1_CH1_Excluded   PPI_CHG_CH1_Excluded        
#define PPI_CHG1_CH1_Included   PPI_CHG_CH1_Included       
#define PPI_CHG1_CH0_Pos        PPI_CHG_CH0_Pos             
#define PPI_CHG1_CH0_Msk        PPI_CHG_CH0_Msk            
#define PPI_CHG1_CH0_Excluded   PPI_CHG_CH0_Excluded       
#define PPI_CHG1_CH0_Included   PPI_CHG_CH0_Included       
#define PPI_CHG2_CH15_Pos       PPI_CHG_CH15_Pos           
#define PPI_CHG2_CH15_Msk       PPI_CHG_CH15_Msk            
#define PPI_CHG2_CH15_Excluded  PPI_CHG_CH15_Excluded      
#define PPI_CHG2_CH15_Included  PPI_CHG_CH15_Included      
#define PPI_CHG2_CH14_Pos       PPI_CHG_CH14_Pos           
#define PPI_CHG2_CH14_Msk       PPI_CHG_CH14_Msk           
#define PPI_CHG2_CH14_Excluded  PPI_CHG_CH14_Excluded       
#define PPI_CHG2_CH14_Included  PPI_CHG_CH14_Included      
#define PPI_CHG2_CH13_Pos       PPI_CHG_CH13_Pos           
#define PPI_CHG2_CH13_Msk       PPI_CHG_CH13_Msk            
#define PPI_CHG2_CH13_Excluded  PPI_CHG_CH13_Excluded       
#define PPI_CHG2_CH13_Included  PPI_CHG_CH13_Included      
#define PPI_CHG2_CH12_Pos       PPI_CHG_CH12_Pos            
#define PPI_CHG2_CH12_Msk       PPI_CHG_CH12_Msk            
#define PPI_CHG2_CH12_Excluded  PPI_CHG_CH12_Excluded      
#define PPI_CHG2_CH12_Included  PPI_CHG_CH12_Included       
#define PPI_CHG2_CH11_Pos       PPI_CHG_CH11_Pos           
#define PPI_CHG2_CH11_Msk       PPI_CHG_CH11_Msk           
#define PPI_CHG2_CH11_Excluded  PPI_CHG_CH11_Excluded       
#define PPI_CHG2_CH11_Included  PPI_CHG_CH11_Included       
#define PPI_CHG2_CH10_Pos       PPI_CHG_CH10_Pos            
#define PPI_CHG2_CH10_Msk       PPI_CHG_CH10_Msk            
#define PPI_CHG2_CH10_Excluded  PPI_CHG_CH10_Excluded      
#define PPI_CHG2_CH10_Included  PPI_CHG_CH10_Included      
#define PPI_CHG2_CH9_Pos        PPI_CHG_CH9_Pos            
#define PPI_CHG2_CH9_Msk        PPI_CHG_CH9_Msk            
#define PPI_CHG2_CH9_Excluded   PPI_CHG_CH9_Excluded        
#define PPI_CHG2_CH9_Included   PPI_CHG_CH9_Included       
#define PPI_CHG2_CH8_Pos        PPI_CHG_CH8_Pos            
#define PPI_CHG2_CH8_Msk        PPI_CHG_CH8_Msk            
#define PPI_CHG2_CH8_Excluded   PPI_CHG_CH8_Excluded       
#define PPI_CHG2_CH8_Included   PPI_CHG_CH8_Included        
#define PPI_CHG2_CH7_Pos        PPI_CHG_CH7_Pos            
#define PPI_CHG2_CH7_Msk        PPI_CHG_CH7_Msk            
#define PPI_CHG2_CH7_Excluded   PPI_CHG_CH7_Excluded       
#define PPI_CHG2_CH7_Included   PPI_CHG_CH7_Included       
#define PPI_CHG2_CH6_Pos        PPI_CHG_CH6_Pos            
#define PPI_CHG2_CH6_Msk        PPI_CHG_CH6_Msk            
#define PPI_CHG2_CH6_Excluded   PPI_CHG_CH6_Excluded       
#define PPI_CHG2_CH6_Included   PPI_CHG_CH6_Included       
#define PPI_CHG2_CH5_Pos        PPI_CHG_CH5_Pos            
#define PPI_CHG2_CH5_Msk        PPI_CHG_CH5_Msk            
#define PPI_CHG2_CH5_Excluded   PPI_CHG_CH5_Excluded       
#define PPI_CHG2_CH5_Included   PPI_CHG_CH5_Included        
#define PPI_CHG2_CH4_Pos        PPI_CHG_CH4_Pos             
#define PPI_CHG2_CH4_Msk        PPI_CHG_CH4_Msk             
#define PPI_CHG2_CH4_Excluded   PPI_CHG_CH4_Excluded        
#define PPI_CHG2_CH4_Included   PPI_CHG_CH4_Included       
#define PPI_CHG2_CH3_Pos        PPI_CHG_CH3_Pos            
#define PPI_CHG2_CH3_Msk        PPI_CHG_CH3_Msk            
#define PPI_CHG2_CH3_Excluded   PPI_CHG_CH3_Excluded       
#define PPI_CHG2_CH3_Included   PPI_CHG_CH3_Included       
#define PPI_CHG2_CH2_Pos        PPI_CHG_CH2_Pos            
#define PPI_CHG2_CH2_Msk        PPI_CHG_CH2_Msk           
#define PPI_CHG2_CH2_Excluded   PPI_CHG_CH2_Excluded       
#define PPI_CHG2_CH2_Included   PPI_CHG_CH2_Included       
#define PPI_CHG2_CH1_Pos        PPI_CHG_CH1_Pos             
#define PPI_CHG2_CH1_Msk        PPI_CHG_CH1_Msk             
#define PPI_CHG2_CH1_Excluded   PPI_CHG_CH1_Excluded       
#define PPI_CHG2_CH1_Included   PPI_CHG_CH1_Included       
#define PPI_CHG2_CH0_Pos        PPI_CHG_CH0_Pos            
#define PPI_CHG2_CH0_Msk        PPI_CHG_CH0_Msk            
#define PPI_CHG2_CH0_Excluded   PPI_CHG_CH0_Excluded       
#define PPI_CHG2_CH0_Included   PPI_CHG_CH0_Included        
#define PPI_CHG3_CH15_Pos       PPI_CHG_CH15_Pos           
#define PPI_CHG3_CH15_Msk       PPI_CHG_CH15_Msk           
#define PPI_CHG3_CH15_Excluded  PPI_CHG_CH15_Excluded     
#define PPI_CHG3_CH15_Included  PPI_CHG_CH15_Included      
#define PPI_CHG3_CH14_Pos       PPI_CHG_CH14_Pos          
#define PPI_CHG3_CH14_Msk       PPI_CHG_CH14_Msk           
#define PPI_CHG3_CH14_Excluded  PPI_CHG_CH14_Excluded      
#define PPI_CHG3_CH14_Included  PPI_CHG_CH14_Included       
#define PPI_CHG3_CH13_Pos       PPI_CHG_CH13_Pos           
#define PPI_CHG3_CH13_Msk       PPI_CHG_CH13_Msk            
#define PPI_CHG3_CH13_Excluded  PPI_CHG_CH13_Excluded      
#define PPI_CHG3_CH13_Included  PPI_CHG_CH13_Included      
#define PPI_CHG3_CH12_Pos       PPI_CHG_CH12_Pos            
#define PPI_CHG3_CH12_Msk       PPI_CHG_CH12_Msk            
#define PPI_CHG3_CH12_Excluded  PPI_CHG_CH12_Excluded       
#define PPI_CHG3_CH12_Included  PPI_CHG_CH12_Included       
#define PPI_CHG3_CH11_Pos       PPI_CHG_CH11_Pos            
#define PPI_CHG3_CH11_Msk       PPI_CHG_CH11_Msk            
#define PPI_CHG3_CH11_Excluded  PPI_CHG_CH11_Excluded      
#define PPI_CHG3_CH11_Included  PPI_CHG_CH11_Included       
#define PPI_CHG3_CH10_Pos       PPI_CHG_CH10_Pos            
#define PPI_CHG3_CH10_Msk       PPI_CHG_CH10_Msk            
#define PPI_CHG3_CH10_Excluded  PPI_CHG_CH10_Excluded      
#define PPI_CHG3_CH10_Included  PPI_CHG_CH10_Included      
#define PPI_CHG3_CH9_Pos        PPI_CHG_CH9_Pos            
#define PPI_CHG3_CH9_Msk        PPI_CHG_CH9_Msk            
#define PPI_CHG3_CH9_Excluded   PPI_CHG_CH9_Excluded       
#define PPI_CHG3_CH9_Included   PPI_CHG_CH9_Included       
#define PPI_CHG3_CH8_Pos        PPI_CHG_CH8_Pos            
#define PPI_CHG3_CH8_Msk        PPI_CHG_CH8_Msk             
#define PPI_CHG3_CH8_Excluded   PPI_CHG_CH8_Excluded       
#define PPI_CHG3_CH8_Included   PPI_CHG_CH8_Included       
#define PPI_CHG3_CH7_Pos        PPI_CHG_CH7_Pos             
#define PPI_CHG3_CH7_Msk        PPI_CHG_CH7_Msk            
#define PPI_CHG3_CH7_Excluded   PPI_CHG_CH7_Excluded        
#define PPI_CHG3_CH7_Included   PPI_CHG_CH7_Included       
#define PPI_CHG3_CH6_Pos        PPI_CHG_CH6_Pos             
#define PPI_CHG3_CH6_Msk        PPI_CHG_CH6_Msk             
#define PPI_CHG3_CH6_Excluded   PPI_CHG_CH6_Excluded       
#define PPI_CHG3_CH6_Included   PPI_CHG_CH6_Included        
#define PPI_CHG3_CH5_Pos        PPI_CHG_CH5_Pos             
#define PPI_CHG3_CH5_Msk        PPI_CHG_CH5_Msk             
#define PPI_CHG3_CH5_Excluded   PPI_CHG_CH5_Excluded        
#define PPI_CHG3_CH5_Included   PPI_CHG_CH5_Included       
#define PPI_CHG3_CH4_Pos        PPI_CHG_CH4_Pos             
#define PPI_CHG3_CH4_Msk        PPI_CHG_CH4_Msk            
#define PPI_CHG3_CH4_Excluded   PPI_CHG_CH4_Excluded        
#define PPI_CHG3_CH4_Included   PPI_CHG_CH4_Included        
#define PPI_CHG3_CH3_Pos        PPI_CHG_CH3_Pos             
#define PPI_CHG3_CH3_Msk        PPI_CHG_CH3_Msk            
#define PPI_CHG3_CH3_Excluded   PPI_CHG_CH3_Excluded        
#define PPI_CHG3_CH3_Included   PPI_CHG_CH3_Included        
#define PPI_CHG3_CH2_Pos        PPI_CHG_CH2_Pos             
#define PPI_CHG3_CH2_Msk        PPI_CHG_CH2_Msk             
#define PPI_CHG3_CH2_Excluded   PPI_CHG_CH2_Excluded        
#define PPI_CHG3_CH2_Included   PPI_CHG_CH2_Included       
#define PPI_CHG3_CH1_Pos        PPI_CHG_CH1_Pos             
#define PPI_CHG3_CH1_Msk        PPI_CHG_CH1_Msk             
#define PPI_CHG3_CH1_Excluded   PPI_CHG_CH1_Excluded        
#define PPI_CHG3_CH1_Included   PPI_CHG_CH1_Included        
#define PPI_CHG3_CH0_Pos        PPI_CHG_CH0_Pos             
#define PPI_CHG3_CH0_Msk        PPI_CHG_CH0_Msk             
#define PPI_CHG3_CH0_Excluded   PPI_CHG_CH0_Excluded        
#define PPI_CHG3_CH0_Included   PPI_CHG_CH0_Included        



/*lint --flb "Leave library region" */

#endif /* NRF51_DEPRECATED_H */

