/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_i2c.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file provides all the I2C firmware functions.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#include "ch32v30x_i2c.h"
#include "ch32v30x_rcc.h"

/* I2C SPE mask */
#define CTLR1_PE_Set             ((uint16_t)0x0001)
#define CTLR1_PE_Reset           ((uint16_t)0xFFFE)

/* I2C START mask */
#define CTLR1_START_Set          ((uint16_t)0x0100)
#define CTLR1_START_Reset        ((uint16_t)0xFEFF)

/* I2C STOP mask */
#define CTLR1_STOP_Set           ((uint16_t)0x0200)
#define CTLR1_STOP_Reset         ((uint16_t)0xFDFF)

/* I2C ACK mask */
#define CTLR1_ACK_Set            ((uint16_t)0x0400)
#define CTLR1_ACK_Reset          ((uint16_t)0xFBFF)

/* I2C ENGC mask */
#define CTLR1_ENGC_Set           ((uint16_t)0x0040)
#define CTLR1_ENGC_Reset         ((uint16_t)0xFFBF)

/* I2C SWRST mask */
#define CTLR1_SWRST_Set          ((uint16_t)0x8000)
#define CTLR1_SWRST_Reset        ((uint16_t)0x7FFF)

/* I2C PEC mask */
#define CTLR1_PEC_Set            ((uint16_t)0x1000)
#define CTLR1_PEC_Reset          ((uint16_t)0xEFFF)

/* I2C ENPEC mask */
#define CTLR1_ENPEC_Set          ((uint16_t)0x0020)
#define CTLR1_ENPEC_Reset        ((uint16_t)0xFFDF)

/* I2C ENARP mask */
#define CTLR1_ENARP_Set          ((uint16_t)0x0010)
#define CTLR1_ENARP_Reset        ((uint16_t)0xFFEF)

/* I2C NOSTRETCH mask */
#define CTLR1_NOSTRETCH_Set      ((uint16_t)0x0080)
#define CTLR1_NOSTRETCH_Reset    ((uint16_t)0xFF7F)

/* I2C registers Masks */
#define CTLR1_CLEAR_Mask         ((uint16_t)0xFBF5)

/* I2C DMAEN mask */
#define CTLR2_DMAEN_Set          ((uint16_t)0x0800)
#define CTLR2_DMAEN_Reset        ((uint16_t)0xF7FF)

/* I2C LAST mask */
#define CTLR2_LAST_Set           ((uint16_t)0x1000)
#define CTLR2_LAST_Reset         ((uint16_t)0xEFFF)

/* I2C FREQ mask */
#define CTLR2_FREQ_Reset         ((uint16_t)0xFFC0)

/* I2C ADD0 mask */
#define OADDR1_ADD0_Set          ((uint16_t)0x0001)
#define OADDR1_ADD0_Reset        ((uint16_t)0xFFFE)

/* I2C ENDUAL mask */
#define OADDR2_ENDUAL_Set        ((uint16_t)0x0001)
#define OADDR2_ENDUAL_Reset      ((uint16_t)0xFFFE)

/* I2C ADD2 mask */
#define OADDR2_ADD2_Reset        ((uint16_t)0xFF01)

/* I2C F/S mask */
#define CKCFGR_FS_Set            ((uint16_t)0x8000)

/* I2C CCR mask */
#define CKCFGR_CCR_Set           ((uint16_t)0x0FFF)

/* I2C FLAG mask */
#define FLAG_Mask                ((uint32_t)0x00FFFFFF)

/* I2C Interrupt Enable mask */
#define ITEN_Mask                ((uint32_t)0x07000000)

/*********************************************************************
 * @fn      I2C_DeInit
 *
 * @brief   Deinitializes the I2Cx peripheral registers to their default
 *        reset values.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *
 * @return  none
 */
void I2C_DeInit(I2C_TypeDef *I2Cx)
{
    if(I2Cx == I2C1)
    {
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);
    }
    else
    {
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, ENABLE);
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, DISABLE);
    }
}

/*********************************************************************
 * @fn      I2C_Init
 *
 * @brief   Initializes the I2Cx peripheral according to the specified
 *        parameters in the I2C_InitStruct.
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *          I2C_InitStruct - pointer to a I2C_InitTypeDef structure that
 *        contains the configuration information for the specified I2C peripheral.
 *
 * @return  none
 */
void I2C_Init(I2C_TypeDef *I2Cx, I2C_InitTypeDef *I2C_InitStruct)
{
    uint16_t tmpreg = 0, freqrange = 0;
    uint16_t result = 0x04;
    uint32_t pclk1 = 8000000;

    RCC_ClocksTypeDef rcc_clocks;

    tmpreg = I2Cx->CTLR2;
    tmpreg &= CTLR2_FREQ_Reset;
    RCC_GetClocksFreq(&rcc_clocks);
    pclk1 = rcc_clocks.PCLK1_Frequency;
    freqrange = (uint16_t)(pclk1 / 1000000);
    tmpreg |= freqrange;
    I2Cx->CTLR2 = tmpreg;

    I2Cx->CTLR1 &= CTLR1_PE_Reset;
    tmpreg = 0;

    if(I2C_InitStruct->I2C_ClockSpeed <= 100000)
    {
        result = (uint16_t)(pclk1 / (I2C_InitStruct->I2C_ClockSpeed << 1));

        if(result < 0x04)
        {
            result = 0x04;
        }

        tmpreg |= result;
        I2Cx->RTR = freqrange + 1;
    }
    else
    {
        if(I2C_InitStruct->I2C_DutyCycle == I2C_DutyCycle_2)
        {
            result = (uint16_t)(pclk1 / (I2C_InitStruct->I2C_ClockSpeed * 3));
        }
        else
        {
            result = (uint16_t)(pclk1 / (I2C_InitStruct->I2C_ClockSpeed * 25));
            result |= I2C_DutyCycle_16_9;
        }

        if((result & CKCFGR_CCR_Set) == 0)
        {
            result |= (uint16_t)0x0001;
        }

        tmpreg |= (uint16_t)(result | CKCFGR_FS_Set);
        I2Cx->RTR = (uint16_t)(((freqrange * (uint16_t)300) / (uint16_t)1000) + (uint16_t)1);
    }

    I2Cx->CKCFGR = tmpreg;
    I2Cx->CTLR1 |= CTLR1_PE_Set;

    tmpreg = I2Cx->CTLR1;
    tmpreg &= CTLR1_CLEAR_Mask;
    tmpreg |= (uint16_t)((uint32_t)I2C_InitStruct->I2C_Mode | I2C_InitStruct->I2C_Ack);
    I2Cx->CTLR1 = tmpreg;

    I2Cx->OADDR1 = (I2C_InitStruct->I2C_AcknowledgedAddress | I2C_InitStruct->I2C_OwnAddress1);
}

/*********************************************************************
 * @fn      I2C_StructInit
 *
 * @brief   Fills each I2C_InitStruct member with its default value.
 *
 * @param   I2C_InitStruct - pointer to an I2C_InitTypeDef structure which
 *        will be initialized.
 *
 * @return  none
 */
void I2C_StructInit(I2C_InitTypeDef *I2C_InitStruct)
{
    I2C_InitStruct->I2C_ClockSpeed = 5000;
    I2C_InitStruct->I2C_Mode = I2C_Mode_I2C;
    I2C_InitStruct->I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStruct->I2C_OwnAddress1 = 0;
    I2C_InitStruct->I2C_Ack = I2C_Ack_Disable;
    I2C_InitStruct->I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
}

/*********************************************************************
 * @fn      I2C_Cmd
 *
 * @brief   Enables or disables the specified I2C peripheral.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void I2C_Cmd(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        I2Cx->CTLR1 |= CTLR1_PE_Set;
    }
    else
    {
        I2Cx->CTLR1 &= CTLR1_PE_Reset;
    }
}

/*********************************************************************
 * @fn      I2C_DMACmd
 *
 * @brief   Enables or disables the specified I2C DMA requests.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void I2C_DMACmd(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        I2Cx->CTLR2 |= CTLR2_DMAEN_Set;
    }
    else
    {
        I2Cx->CTLR2 &= CTLR2_DMAEN_Reset;
    }
}

/*********************************************************************
 * @fn      I2C_DMALastTransferCmd
 *
 * @brief   Specifies if the next DMA transfer will be the last one.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void I2C_DMALastTransferCmd(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        I2Cx->CTLR2 |= CTLR2_LAST_Set;
    }
    else
    {
        I2Cx->CTLR2 &= CTLR2_LAST_Reset;
    }
}

/*********************************************************************
 * @fn      I2C_GenerateSTART
 *
 * @brief   Generates I2Cx communication START condition.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void I2C_GenerateSTART(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        I2Cx->CTLR1 |= CTLR1_START_Set;
    }
    else
    {
        I2Cx->CTLR1 &= CTLR1_START_Reset;
    }
}

/*********************************************************************
 * @fn      I2C_GenerateSTOP
 *
 * @brief   Generates I2Cx communication STOP condition.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void I2C_GenerateSTOP(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        I2Cx->CTLR1 |= CTLR1_STOP_Set;
    }
    else
    {
        I2Cx->CTLR1 &= CTLR1_STOP_Reset;
    }
}

/*********************************************************************
 * @fn      I2C_AcknowledgeConfig
 *
 * @brief   Enables or disables the specified I2C acknowledge feature.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void I2C_AcknowledgeConfig(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        I2Cx->CTLR1 |= CTLR1_ACK_Set;
    }
    else
    {
        I2Cx->CTLR1 &= CTLR1_ACK_Reset;
    }
}

/*********************************************************************
 * @fn      I2C_OwnAddress2Config
 *
 * @brief   Configures the specified I2C own address2.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *          Address - specifies the 7bit I2C own address2.
 *
 * @return  none
 */
void I2C_OwnAddress2Config(I2C_TypeDef *I2Cx, uint8_t Address)
{
    uint16_t tmpreg = 0;

    tmpreg = I2Cx->OADDR2;
    tmpreg &= OADDR2_ADD2_Reset;
    tmpreg |= (uint16_t)((uint16_t)Address & (uint16_t)0x00FE);
    I2Cx->OADDR2 = tmpreg;
}

/*********************************************************************
 * @fn      I2C_DualAddressCmd
 *
 * @brief   Enables or disables the specified I2C dual addressing mode.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void I2C_DualAddressCmd(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        I2Cx->OADDR2 |= OADDR2_ENDUAL_Set;
    }
    else
    {
        I2Cx->OADDR2 &= OADDR2_ENDUAL_Reset;
    }
}

/*********************************************************************
 * @fn      I2C_GeneralCallCmd
 *
 * @brief   Enables or disables the specified I2C general call feature.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void I2C_GeneralCallCmd(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        I2Cx->CTLR1 |= CTLR1_ENGC_Set;
    }
    else
    {
        I2Cx->CTLR1 &= CTLR1_ENGC_Reset;
    }
}

/*********************************************************************
 * @fn      I2C_ITConfig
 *
 * @brief   Enables or disables the specified I2C interrupts.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *          I2C_IT - specifies the I2C interrupts sources to be enabled or disabled.
 *            I2C_IT_BUF - Buffer interrupt mask.
 *            I2C_IT_EVT - Event interrupt mask.
 *            I2C_IT_ERR - Error interrupt mask.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void I2C_ITConfig(I2C_TypeDef *I2Cx, uint16_t I2C_IT, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        I2Cx->CTLR2 |= I2C_IT;
    }
    else
    {
        I2Cx->CTLR2 &= (uint16_t)~I2C_IT;
    }
}

/*********************************************************************
 * @fn      I2C_SendData
 *
 * @brief   Sends a data byte through the I2Cx peripheral.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *          Data - Byte to be transmitted.
 *
 * @return  none
 */
void I2C_SendData(I2C_TypeDef *I2Cx, uint8_t Data)
{
    I2Cx->DATAR = Data;
}

/*********************************************************************
 * @fn      I2C_ReceiveData
 *
 * @brief   Returns the most recent received data by the I2Cx peripheral.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *
 * @return  The value of the received data.
 */
uint8_t I2C_ReceiveData(I2C_TypeDef *I2Cx)
{
    return (uint8_t)I2Cx->DATAR;
}

/*********************************************************************
 * @fn      I2C_Send7bitAddress
 *
 * @brief   Transmits the address byte to select the slave device.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *          Address - specifies the slave address which will be transmitted.
 *          I2C_Direction - specifies whether the I2C device will be a
 *        Transmitter or a Receiver.
 *            I2C_Direction_Transmitter - Transmitter mode.
 *            I2C_Direction_Receiver - Receiver mode.
 *
 * @return  none
 */
void I2C_Send7bitAddress(I2C_TypeDef *I2Cx, uint8_t Address, uint8_t I2C_Direction)
{
    if(I2C_Direction != I2C_Direction_Transmitter)
    {
        Address |= OADDR1_ADD0_Set;
    }
    else
    {
        Address &= OADDR1_ADD0_Reset;
    }

    I2Cx->DATAR = Address;
}

/*********************************************************************
 * @fn      I2C_ReadRegister
 *
 * @brief   Reads the specified I2C register and returns its value.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *          I2C_Register - specifies the register to read.
 *            I2C_Register_CTLR1.
 *            I2C_Register_CTLR2.
 *            I2C_Register_OADDR1.
 *            I2C_Register_OADDR2.
 *            I2C_Register_DATAR.
 *            I2C_Register_STAR1.
 *            I2C_Register_STAR2.
 *            I2C_Register_CKCFGR.
 *            I2C_Register_RTR.
 *
 * @return  none
 */
uint16_t I2C_ReadRegister(I2C_TypeDef *I2Cx, uint8_t I2C_Register)
{
    __IO uint32_t tmp = 0;

    tmp = (uint32_t)I2Cx;
    tmp += I2C_Register;

    return (*(__IO uint16_t *)tmp);
}

/*********************************************************************
 * @fn      I2C_SoftwareResetCmd
 *
 * @brief   Enables or disables the specified I2C software reset.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void I2C_SoftwareResetCmd(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        I2Cx->CTLR1 |= CTLR1_SWRST_Set;
    }
    else
    {
        I2Cx->CTLR1 &= CTLR1_SWRST_Reset;
    }
}

/*********************************************************************
 * @fn      I2C_NACKPositionConfig
 *
 * @brief   Selects the specified I2C NACK position in master receiver mode.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *          I2C_NACKPosition - specifies the NACK position.
 *            I2C_NACKPosition_Next - indicates that the next byte will be
 *        the last received byte.
 *            I2C_NACKPosition_Current - indicates that current byte is the
 *        last received byte.
 *
 * @return  none
 */
void I2C_NACKPositionConfig(I2C_TypeDef *I2Cx, uint16_t I2C_NACKPosition)
{
    if(I2C_NACKPosition == I2C_NACKPosition_Next)
    {
        I2Cx->CTLR1 |= I2C_NACKPosition_Next;
    }
    else
    {
        I2Cx->CTLR1 &= I2C_NACKPosition_Current;
    }
}

/*********************************************************************
 * @fn      I2C_SMBusAlertConfig
 *
 * @brief   Drives the SMBusAlert pin high or low for the specified I2C.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *          I2C_SMBusAlert - specifies SMBAlert pin level.
 *            I2C_SMBusAlert_Low - SMBAlert pin driven low.
 *            I2C_SMBusAlert_High - SMBAlert pin driven high.
 *
 * @return  none
 */
void I2C_SMBusAlertConfig(I2C_TypeDef *I2Cx, uint16_t I2C_SMBusAlert)
{
    if(I2C_SMBusAlert == I2C_SMBusAlert_Low)
    {
        I2Cx->CTLR1 |= I2C_SMBusAlert_Low;
    }
    else
    {
        I2Cx->CTLR1 &= I2C_SMBusAlert_High;
    }
}

/*********************************************************************
 * @fn      I2C_TransmitPEC
 *
 * @brief   Enables or disables the specified I2C PEC transfer.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void I2C_TransmitPEC(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        I2Cx->CTLR1 |= CTLR1_PEC_Set;
    }
    else
    {
        I2Cx->CTLR1 &= CTLR1_PEC_Reset;
    }
}

/*********************************************************************
 * @fn      I2C_PECPositionConfig
 *
 * @brief   Selects the specified I2C PEC position.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *          I2C_PECPosition - specifies the PEC position.
 *            I2C_PECPosition_Next - indicates that the next byte is PEC.
 *            I2C_PECPosition_Current - indicates that current byte is PEC.
 *
 * @return  none
 */
void I2C_PECPositionConfig(I2C_TypeDef *I2Cx, uint16_t I2C_PECPosition)
{
    if(I2C_PECPosition == I2C_PECPosition_Next)
    {
        I2Cx->CTLR1 |= I2C_PECPosition_Next;
    }
    else
    {
        I2Cx->CTLR1 &= I2C_PECPosition_Current;
    }
}

/*********************************************************************
 * @fn      I2C_CalculatePEC
 *
 * @brief   Enables or disables the PEC value calculation of the transferred bytes.
 *
 * @param   I2Cx- where x can be 1 or 2 to select the I2C peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void I2C_CalculatePEC(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        I2Cx->CTLR1 |= CTLR1_ENPEC_Set;
    }
    else
    {
        I2Cx->CTLR1 &= CTLR1_ENPEC_Reset;
    }
}

/*********************************************************************
 * @fn      I2C_GetPEC
 *
 * @brief   Returns the PEC value for the specified I2C.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *
 * @return  The PEC value.
 */
uint8_t I2C_GetPEC(I2C_TypeDef *I2Cx)
{
    return ((I2Cx->STAR2) >> 8);
}

/*********************************************************************
 * @fn      I2C_ARPCmd
 *
 * @brief   Enables or disables the specified I2C ARP.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *            NewState - ENABLE or DISABLE.
 *
 * @return  The PEC value.
 */
void I2C_ARPCmd(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        I2Cx->CTLR1 |= CTLR1_ENARP_Set;
    }
    else
    {
        I2Cx->CTLR1 &= CTLR1_ENARP_Reset;
    }
}

/*********************************************************************
 * @fn      I2C_StretchClockCmd
 *
 * @brief   Enables or disables the specified I2C Clock stretching.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void I2C_StretchClockCmd(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    if(NewState == DISABLE)
    {
        I2Cx->CTLR1 |= CTLR1_NOSTRETCH_Set;
    }
    else
    {
        I2Cx->CTLR1 &= CTLR1_NOSTRETCH_Reset;
    }
}

/*********************************************************************
 * @fn      I2C_FastModeDutyCycleConfig
 *
 * @brief   Selects the specified I2C fast mode duty cycle.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *          I2C_DutyCycle - specifies the fast mode duty cycle.
 *            I2C_DutyCycle_2 - I2C fast mode Tlow/Thigh = 2.
 *            I2C_DutyCycle_16_9 - I2C fast mode Tlow/Thigh = 16/9.
 *
 * @return  none
 */
void I2C_FastModeDutyCycleConfig(I2C_TypeDef *I2Cx, uint16_t I2C_DutyCycle)
{
    if(I2C_DutyCycle != I2C_DutyCycle_16_9)
    {
        I2Cx->CKCFGR &= I2C_DutyCycle_2;
    }
    else
    {
        I2Cx->CKCFGR |= I2C_DutyCycle_16_9;
    }
}

/*********************************************************************
 * @fn      I2C_CheckEvent
 *
 * @brief   Checks whether the last I2Cx Event is equal to the one passed
 *        as parameter.
 *
 * @param   I2Cx- where x can be 1 or 2 to select the I2C peripheral.
 *          I2C_EVENT: specifies the event to be checked.
 *             I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED - EV1.
 *             I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED - EV1.
 *             I2C_EVENT_SLAVE_TRANSMITTER_SECONDADDRESS_MATCHED - EV1.
 *             I2C_EVENT_SLAVE_RECEIVER_SECONDADDRESS_MATCHED - EV1.
 *             I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED - EV1.
 *             I2C_EVENT_SLAVE_BYTE_RECEIVED - EV2.
 *             (I2C_EVENT_SLAVE_BYTE_RECEIVED | I2C_FLAG_DUALF) - EV2.
 *             (I2C_EVENT_SLAVE_BYTE_RECEIVED | I2C_FLAG_GENCALL) - EV2.
 *             I2C_EVENT_SLAVE_BYTE_TRANSMITTED - EV3.
 *             (I2C_EVENT_SLAVE_BYTE_TRANSMITTED | I2C_FLAG_DUALF) - EV3.
 *             (I2C_EVENT_SLAVE_BYTE_TRANSMITTED | I2C_FLAG_GENCALL) - EV3.
 *             I2C_EVENT_SLAVE_ACK_FAILURE - EV3_2.
 *             I2C_EVENT_SLAVE_STOP_DETECTED - EV4.
 *             I2C_EVENT_MASTER_MODE_SELECT - EV5.
 *             I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED - EV6.
 *             I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED - EV6.
 *             I2C_EVENT_MASTER_BYTE_RECEIVED - EV7.
 *             I2C_EVENT_MASTER_BYTE_TRANSMITTING - EV8.
 *             I2C_EVENT_MASTER_BYTE_TRANSMITTED - EV8_2.
 *             I2C_EVENT_MASTER_MODE_ADDRESS10 - EV9.
 *
 * @return  none
 */
ErrorStatus I2C_CheckEvent(I2C_TypeDef *I2Cx, uint32_t I2C_EVENT)
{
    uint32_t    lastevent = 0;
    uint32_t    flag1 = 0, flag2 = 0;
    ErrorStatus status = ERROR;

    flag1 = I2Cx->STAR1;
    flag2 = I2Cx->STAR2;
    flag2 = flag2 << 16;

    lastevent = (flag1 | flag2) & FLAG_Mask;

    if((lastevent & I2C_EVENT) == I2C_EVENT)
    {
        status = SUCCESS;
    }
    else
    {
        status = ERROR;
    }

    return status;
}

/*********************************************************************
 * @fn      I2C_GetLastEvent
 *
 * @brief   Returns the last I2Cx Event.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *
 * @return  none
 */
uint32_t I2C_GetLastEvent(I2C_TypeDef *I2Cx)
{
    uint32_t lastevent = 0;
    uint32_t flag1 = 0, flag2 = 0;

    flag1 = I2Cx->STAR1;
    flag2 = I2Cx->STAR2;
    flag2 = flag2 << 16;
    lastevent = (flag1 | flag2) & FLAG_Mask;

    return lastevent;
}

/*********************************************************************
 * @fn      I2C_GetFlagStatus
 *
 * @brief   Checks whether the last I2Cx Event is equal to the one passed
 *        as parameter.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *          I2C_FLAG - specifies the flag to check.
 *            I2C_FLAG_DUALF - Dual flag (Slave mode).
 *            I2C_FLAG_SMBHOST - SMBus host header (Slave mode).
 *            I2C_FLAG_SMBDEFAULT - SMBus default header (Slave mode).
 *            I2C_FLAG_GENCALL - General call header flag (Slave mode).
 *            I2C_FLAG_TRA - Transmitter/Receiver flag.
 *            I2C_FLAG_BUSY - Bus busy flag.
 *            I2C_FLAG_MSL - Master/Slave flag.
 *            I2C_FLAG_SMBALERT - SMBus Alert flag.
 *            I2C_FLAG_TIMEOUT - Timeout or Tlow error flag.
 *            I2C_FLAG_PECERR - PEC error in reception flag.
 *            I2C_FLAG_OVR - Overrun/Underrun flag (Slave mode).
 *            I2C_FLAG_AF - Acknowledge failure flag.
 *            I2C_FLAG_ARLO - Arbitration lost flag (Master mode).
 *            I2C_FLAG_BERR - Bus error flag.
 *            I2C_FLAG_TXE - Data register empty flag (Transmitter).
 *            I2C_FLAG_RXNE- Data register not empty (Receiver) flag.
 *            I2C_FLAG_STOPF - Stop detection flag (Slave mode).
 *            I2C_FLAG_ADD10 - 10-bit header sent flag (Master mode).
 *            I2C_FLAG_BTF - Byte transfer finished flag.
 *            I2C_FLAG_ADDR - Address sent flag (Master mode) "ADSL"
 *        Address matched flag (Slave mode)"ENDA".
 *            I2C_FLAG_SB - Start bit flag (Master mode).
 *
 * @return  none
 */
FlagStatus I2C_GetFlagStatus(I2C_TypeDef *I2Cx, uint32_t I2C_FLAG)
{
    FlagStatus    bitstatus = RESET;
    __IO uint32_t i2creg = 0, i2cxbase = 0;

    i2cxbase = (uint32_t)I2Cx;
    i2creg = I2C_FLAG >> 28;
    I2C_FLAG &= FLAG_Mask;

    if(i2creg != 0)
    {
        i2cxbase += 0x14;
    }
    else
    {
        I2C_FLAG = (uint32_t)(I2C_FLAG >> 16);
        i2cxbase += 0x18;
    }

    if(((*(__IO uint32_t *)i2cxbase) & I2C_FLAG) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      I2C_ClearFlag
 *
 * @brief   Clears the I2Cx's pending flags.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *          I2C_FLAG - specifies the flag to clear.
 *            I2C_FLAG_SMBALERT - SMBus Alert flag.
 *            I2C_FLAG_TIMEOUT - Timeout or Tlow error flag.
 *            I2C_FLAG_PECERR - PEC error in reception flag.
 *            I2C_FLAG_OVR - Overrun/Underrun flag (Slave mode).
 *            I2C_FLAG_AF - Acknowledge failure flag.
 *            I2C_FLAG_ARLO - Arbitration lost flag (Master mode).
 *            I2C_FLAG_BERR - Bus error flag.
 *
 * @return  none
 */
void I2C_ClearFlag(I2C_TypeDef *I2Cx, uint32_t I2C_FLAG)
{
    uint32_t flagpos = 0;

    flagpos = I2C_FLAG & FLAG_Mask;
    I2Cx->STAR1 = (uint16_t)~flagpos;
}

/*********************************************************************
 * @fn      I2C_GetITStatus
 *
 * @brief   Checks whether the specified I2C interrupt has occurred or not.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *          II2C_IT - specifies the interrupt source to check.
 *            I2C_IT_SMBALERT - SMBus Alert flag.
 *            I2C_IT_TIMEOUT - Timeout or Tlow error flag.
 *            I2C_IT_PECERR - PEC error in reception flag.
 *            I2C_IT_OVR - Overrun/Underrun flag (Slave mode).
 *            I2C_IT_AF - Acknowledge failure flag.
 *            I2C_IT_ARLO - Arbitration lost flag (Master mode).
 *            I2C_IT_BERR - Bus error flag.
 *            I2C_IT_TXE - Data register empty flag (Transmitter).
 *            I2C_IT_RXNE - Data register not empty (Receiver) flag.
 *            I2C_IT_STOPF - Stop detection flag (Slave mode).
 *            I2C_IT_ADD10 - 10-bit header sent flag (Master mode).
 *            I2C_IT_BTF - Byte transfer finished flag.
 *            I2C_IT_ADDR - Address sent flag (Master mode) "ADSL"  Address matched
 *        flag (Slave mode)"ENDAD".
 *            I2C_IT_SB - Start bit flag (Master mode).
 *
 * @return  none
 */
ITStatus I2C_GetITStatus(I2C_TypeDef *I2Cx, uint32_t I2C_IT)
{
    ITStatus bitstatus = RESET;
    uint32_t enablestatus = 0;

    enablestatus = (uint32_t)(((I2C_IT & ITEN_Mask) >> 16) & (I2Cx->CTLR2));
    I2C_IT &= FLAG_Mask;

    if(((I2Cx->STAR1 & I2C_IT) != (uint32_t)RESET) && enablestatus)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      I2C_ClearITPendingBit
 *
 * @brief   Clears the I2Cx interrupt pending bits.
 *
 * @param   I2Cx - where x can be 1 or 2 to select the I2C peripheral.
 *          I2C_IT - specifies the interrupt pending bit to clear.
 *            I2C_IT_SMBALERT - SMBus Alert interrupt.
 *            I2C_IT_TIMEOUT - Timeout or Tlow error interrupt.
 *            I2C_IT_PECERR - PEC error in reception  interrupt.
 *            I2C_IT_OVR - Overrun/Underrun interrupt (Slave mode).
 *            I2C_IT_AF - Acknowledge failure interrupt.
 *            I2C_IT_ARLO - Arbitration lost interrupt (Master mode).
 *            I2C_IT_BERR - Bus error interrupt.
 *
 * @return  none
 */
void I2C_ClearITPendingBit(I2C_TypeDef *I2Cx, uint32_t I2C_IT)
{
    uint32_t flagpos = 0;

    flagpos = I2C_IT & FLAG_Mask;
    I2Cx->STAR1 = (uint16_t)~flagpos;
}
