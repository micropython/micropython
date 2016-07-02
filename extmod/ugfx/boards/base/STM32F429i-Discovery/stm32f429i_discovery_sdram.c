#include "ch.h"
#include "hal.h"

#include "stm32f429i_discovery_sdram.h"
#include "stm32f4xx_fmc.h"

/**
  * @brief  Configures the FMC and GPIOs to interface with the SDRAM memory.
  *         This function must be called before any read/write operation
  *         on the SDRAM.
  * @param  None
  * @retval None
  */
void SDRAM_Init(void)
{
  FMC_SDRAMInitTypeDef  FMC_SDRAMInitStructure;
  FMC_SDRAMTimingInitTypeDef  FMC_SDRAMTimingInitStructure; 
  
  /* Enable FMC clock */
  rccEnableAHB3(RCC_AHB3ENR_FMCEN, FALSE);
 
/* FMC Configuration ---------------------------------------------------------*/
/* FMC SDRAM Bank configuration */   
  /* Timing configuration for 84 Mhz of SD clock frequency (168Mhz/2) */
  /* TMRD: 2 Clock cycles */
  FMC_SDRAMTimingInitStructure.FMC_LoadToActiveDelay = 2;      
  /* TXSR: min=70ns (6x11.90ns) */
  FMC_SDRAMTimingInitStructure.FMC_ExitSelfRefreshDelay = 7;
  /* TRAS: min=42ns (4x11.90ns) max=120k (ns) */
  FMC_SDRAMTimingInitStructure.FMC_SelfRefreshTime = 4;
  /* TRC:  min=63 (6x11.90ns) */        
  FMC_SDRAMTimingInitStructure.FMC_RowCycleDelay = 7;         
  /* TWR:  2 Clock cycles */
  FMC_SDRAMTimingInitStructure.FMC_WriteRecoveryTime = 2;      
  /* TRP:  15ns => 2x11.90ns */
  FMC_SDRAMTimingInitStructure.FMC_RPDelay = 2;                
  /* TRCD: 15ns => 2x11.90ns */
  FMC_SDRAMTimingInitStructure.FMC_RCDDelay = 2;

/* FMC SDRAM control configuration */
  FMC_SDRAMInitStructure.FMC_Bank = FMC_Bank2_SDRAM;
  /* Row addressing: [7:0] */
  FMC_SDRAMInitStructure.FMC_ColumnBitsNumber = FMC_ColumnBits_Number_8b;
  /* Column addressing: [11:0] */
  FMC_SDRAMInitStructure.FMC_RowBitsNumber = FMC_RowBits_Number_12b;
  FMC_SDRAMInitStructure.FMC_SDMemoryDataWidth = SDRAM_MEMORY_WIDTH;
  FMC_SDRAMInitStructure.FMC_InternalBankNumber = FMC_InternalBank_Number_4;
  FMC_SDRAMInitStructure.FMC_CASLatency = SDRAM_CAS_LATENCY; 
  FMC_SDRAMInitStructure.FMC_WriteProtection = FMC_Write_Protection_Disable;
  FMC_SDRAMInitStructure.FMC_SDClockPeriod = SDCLOCK_PERIOD;  
  FMC_SDRAMInitStructure.FMC_ReadBurst = SDRAM_READBURST;
  FMC_SDRAMInitStructure.FMC_ReadPipeDelay = FMC_ReadPipe_Delay_1;
  FMC_SDRAMInitStructure.FMC_SDRAMTimingStruct = &FMC_SDRAMTimingInitStructure;
  
  /* FMC SDRAM bank initialization */
  FMC_SDRAMInit(&FMC_SDRAMInitStructure); 
  
  /* FMC SDRAM device initialization sequence */
  SDRAM_InitSequence(); 
  
}

/*-- GPIOs Configuration -----------------------------------------------------*/
/*
 +-------------------+--------------------+--------------------+--------------------+
 +                       SDRAM pins assignment                                      +
 +-------------------+--------------------+--------------------+--------------------+
 | PD0  <-> FMC_D2   | PE0  <-> FMC_NBL0  | PF0  <-> FMC_A0    | PG0  <-> FMC_A10   |
 | PD1  <-> FMC_D3   | PE1  <-> FMC_NBL1  | PF1  <-> FMC_A1    | PG1  <-> FMC_A11   |
 | PD8  <-> FMC_D13  | PE7  <-> FMC_D4    | PF2  <-> FMC_A2    | PG8  <-> FMC_SDCLK |
 | PD9  <-> FMC_D14  | PE8  <-> FMC_D5    | PF3  <-> FMC_A3    | PG15 <-> FMC_NCAS  |
 | PD10 <-> FMC_D15  | PE9  <-> FMC_D6    | PF4  <-> FMC_A4    |--------------------+ 
 | PD14 <-> FMC_D0   | PE10 <-> FMC_D7    | PF5  <-> FMC_A5    |   
 | PD15 <-> FMC_D1   | PE11 <-> FMC_D8    | PF11 <-> FMC_NRAS  | 
 +-------------------| PE12 <-> FMC_D9    | PF12 <-> FMC_A6    | 
                     | PE13 <-> FMC_D10   | PF13 <-> FMC_A7    |    
                     | PE14 <-> FMC_D11   | PF14 <-> FMC_A8    |
                     | PE15 <-> FMC_D12   | PF15 <-> FMC_A9    |
 +-------------------+--------------------+--------------------+
 | PB5 <-> FMC_SDCKE1| 
 | PB6 <-> FMC_SDNE1 | 
 | PC0 <-> FMC_SDNWE |
 +-------------------+  
  
*/
  
//  /* Common GPIO configuration */
//  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
//
//  /* GPIOB configuration */
//  GPIO_PinAFConfig(GPIOB, GPIO_PinSource5 , GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6 , GPIO_AF_FMC);
//
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5  | GPIO_Pin_6;
//
//  GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//  /* GPIOC configuration */
//  GPIO_PinAFConfig(GPIOC, GPIO_PinSource0 , GPIO_AF_FMC);
//
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
//
//  GPIO_Init(GPIOC, &GPIO_InitStructure);
//
//  /* GPIOD configuration */
//  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FMC);
//
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1  | GPIO_Pin_8 |
//                                GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 |
//                                GPIO_Pin_15;
//
//  GPIO_Init(GPIOD, &GPIO_InitStructure);
//
//  /* GPIOE configuration */
//  GPIO_PinAFConfig(GPIOE, GPIO_PinSource0 , GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOE, GPIO_PinSource1 , GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FMC);
//
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_7 |
//                                GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 |
//                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 |
//                                GPIO_Pin_14 | GPIO_Pin_15;
//
//  GPIO_Init(GPIOE, &GPIO_InitStructure);
//
//  /* GPIOF configuration */
//  GPIO_PinAFConfig(GPIOF, GPIO_PinSource0 , GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOF, GPIO_PinSource1 , GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOF, GPIO_PinSource2 , GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOF, GPIO_PinSource3 , GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOF, GPIO_PinSource4 , GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOF, GPIO_PinSource5 , GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOF, GPIO_PinSource11 , GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOF, GPIO_PinSource12 , GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOF, GPIO_PinSource13 , GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOF, GPIO_PinSource14 , GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOF, GPIO_PinSource15 , GPIO_AF_FMC);
//
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1 | GPIO_Pin_2 |
//                                GPIO_Pin_3  | GPIO_Pin_4 | GPIO_Pin_5 |
//                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 |
//                                GPIO_Pin_14 | GPIO_Pin_15;
//
//  GPIO_Init(GPIOF, &GPIO_InitStructure);
//
//  /* GPIOG configuration */
//  GPIO_PinAFConfig(GPIOG, GPIO_PinSource0 , GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOG, GPIO_PinSource1 , GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOG, GPIO_PinSource4 , GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOG, GPIO_PinSource5 , GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOG, GPIO_PinSource8 , GPIO_AF_FMC);
//  GPIO_PinAFConfig(GPIOG, GPIO_PinSource15 , GPIO_AF_FMC);
//
//
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 |
//                                GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_15;
//
//  GPIO_Init(GPIOG, &GPIO_InitStructure);

/**
  * @brief  Executes the SDRAM memory initialization sequence. 
  * @param  None. 
  * @retval None.
  */
void SDRAM_InitSequence(void)
{
  FMC_SDRAMCommandTypeDef FMC_SDRAMCommandStructure;
  uint32_t tmpr = 0;
  
/* Step 3 --------------------------------------------------------------------*/
  /* Configure a clock configuration enable command */
  FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_CLK_Enabled;
  FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank2;
  FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
  FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
  /* Wait until the SDRAM controller is ready */ 
  while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
  /* Send the command */
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);  
  
  //In the ST example, this is 100ms, but the 429 RM says 100us is typical, and
  //the ISSI datasheet confirms this. 1ms seems plenty, and is much shorter than
  //refresh interval, meaning we won't risk losing contents if the SDRAM is in self-refresh
  //mode
/* Step 4 --------------------------------------------------------------------*/
  /* Insert 1 ms delay */
  chThdSleepMilliseconds(1);
    
/* Step 5 --------------------------------------------------------------------*/
  /* Configure a PALL (precharge all) command */ 
  FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_PALL;
  FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank2;
  FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
  FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
  /* Wait until the SDRAM controller is ready */ 
  while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
  /* Send the command */
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
  
/* Step 6 --------------------------------------------------------------------*/
  /* Configure a Auto-Refresh command */ 
  FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_AutoRefresh;
  FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank2;
  FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 4;
  FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
  /* Wait until the SDRAM controller is ready */ 
  while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
  /* Send the  first command */
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
  
  /* Wait until the SDRAM controller is ready */ 
  while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
  /* Send the second command */
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
  
/* Step 7 --------------------------------------------------------------------*/
  /* Program the external memory mode register */
  tmpr = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_2          |
                   SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
                   SDRAM_MODEREG_CAS_LATENCY_3           |
                   SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                   SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
  
  /* Configure a load Mode register command*/ 
  FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_LoadMode;
  FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank2;
  FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
  FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = tmpr;
  /* Wait until the SDRAM controller is ready */ 
  while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
  /* Send the command */
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
  
/* Step 8 --------------------------------------------------------------------*/

  /* Set the refresh rate counter */
  /* (7.81 us x Freq) - 20 */
  /* Set the device refresh counter */
  FMC_SetRefreshCount(683);
  /* Wait until the SDRAM controller is ready */ 
  while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
}


/**
  * @brief  Writes a Entire-word buffer to the SDRAM memory. 
  * @param  pBuffer: pointer to buffer. 
  * @param  uwWriteAddress: SDRAM memory internal address from which the data will be 
  *         written.
  * @param  uwBufferSize: number of words to write. 
  * @retval None.
  */
void SDRAM_WriteBuffer(uint32_t* pBuffer, uint32_t uwWriteAddress, uint32_t uwBufferSize)
{
  __IO uint32_t write_pointer = (uint32_t)uwWriteAddress;

  /* Disable write protection */
  FMC_SDRAMWriteProtectionConfig(FMC_Bank2_SDRAM, DISABLE);
  
  /* Wait until the SDRAM controller is ready */ 
  while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }

  /* While there is data to write */
  for (; uwBufferSize != 0; uwBufferSize--) 
  {
    /* Transfer data to the memory */
    *(uint32_t *) (SDRAM_BANK_ADDR + write_pointer) = *pBuffer++;

    /* Increment the address*/
    write_pointer += 4;
  }
    
}

/**
  * @brief  Reads data buffer from the SDRAM memory. 
  * @param  pBuffer: pointer to buffer. 
  * @param  ReadAddress: SDRAM memory internal address from which the data will be 
  *         read.
  * @param  uwBufferSize: number of words to write. 
  * @retval None.
  */
void SDRAM_ReadBuffer(uint32_t* pBuffer, uint32_t uwReadAddress, uint32_t uwBufferSize)
{
  __IO uint32_t write_pointer = (uint32_t)uwReadAddress;
  
   
  /* Wait until the SDRAM controller is ready */ 
  while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
  
  /* Read data */
  for(; uwBufferSize != 0x00; uwBufferSize--)
  {
   *pBuffer++ = *(__IO uint32_t *)(SDRAM_BANK_ADDR + write_pointer );
    
   /* Increment the address*/
    write_pointer += 4;
  } 
}

