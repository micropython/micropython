//*****************************************************************************
//
//  i2c.h
//
//  Prototypes for the I2C Driver.
//
//  Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

#ifndef __DRIVERLIB_I2C_H__
#define __DRIVERLIB_I2C_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Defines for the API.
//
//*****************************************************************************

//*****************************************************************************
//
// Interrupt defines.
//
//*****************************************************************************
#define I2C_INT_MASTER          0x00000001
#define I2C_INT_SLAVE           0x00000002

//*****************************************************************************
//
// I2C Master commands.
//
//*****************************************************************************
#define I2C_MASTER_CMD_SINGLE_SEND                                            \
                                0x00000007
#define I2C_MASTER_CMD_SINGLE_RECEIVE                                         \
                                0x00000007
#define I2C_MASTER_CMD_BURST_SEND_START                                       \
                                0x00000003
#define I2C_MASTER_CMD_BURST_SEND_CONT                                        \
                                0x00000001
#define I2C_MASTER_CMD_BURST_SEND_FINISH                                      \
                                0x00000005
#define I2C_MASTER_CMD_BURST_SEND_STOP                                        \
                                0x00000004
#define I2C_MASTER_CMD_BURST_SEND_ERROR_STOP                                  \
                                0x00000004
#define I2C_MASTER_CMD_BURST_RECEIVE_START                                    \
                                0x0000000b
#define I2C_MASTER_CMD_BURST_RECEIVE_CONT                                     \
                                0x00000009
#define I2C_MASTER_CMD_BURST_RECEIVE_FINISH                                   \
                                0x00000005
#define I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP                               \
                                0x00000004
#define I2C_MASTER_CMD_QUICK_COMMAND                                          \
                                0x00000027
#define I2C_MASTER_CMD_HS_MASTER_CODE_SEND                                    \
                                0x00000013
#define I2C_MASTER_CMD_FIFO_SINGLE_SEND                                       \
                                0x00000046
#define I2C_MASTER_CMD_FIFO_SINGLE_RECEIVE                                    \
                                0x00000046
#define I2C_MASTER_CMD_FIFO_BURST_SEND_START                                  \
                                0x00000042
#define I2C_MASTER_CMD_FIFO_BURST_SEND_CONT                                   \
                                0x00000040
#define I2C_MASTER_CMD_FIFO_BURST_SEND_FINISH                                 \
                                0x00000044
#define I2C_MASTER_CMD_FIFO_BURST_SEND_ERROR_STOP                             \
                                0x00000004
#define I2C_MASTER_CMD_FIFO_BURST_RECEIVE_START                               \
                                0x0000004a
#define I2C_MASTER_CMD_FIFO_BURST_RECEIVE_CONT                                \
                                0x00000048
#define I2C_MASTER_CMD_FIFO_BURST_RECEIVE_FINISH                              \
                                0x00000044
#define I2C_MASTER_CMD_FIFO_BURST_RECEIVE_ERROR_STOP                          \
                                0x00000004

//*****************************************************************************
//
// I2C Master glitch filter configuration.
//
//*****************************************************************************
#define I2C_MASTER_GLITCH_FILTER_DISABLED                                     \
                                0
#define I2C_MASTER_GLITCH_FILTER_1                                            \
                                0x00010000
#define I2C_MASTER_GLITCH_FILTER_2                                            \
                                0x00020000
#define I2C_MASTER_GLITCH_FILTER_3                                            \
                                0x00030000
#define I2C_MASTER_GLITCH_FILTER_4                                            \
                                0x00040000
#define I2C_MASTER_GLITCH_FILTER_8                                            \
                                0x00050000
#define I2C_MASTER_GLITCH_FILTER_16                                           \
                                0x00060000
#define I2C_MASTER_GLITCH_FILTER_32                                           \
                                0x00070000

//*****************************************************************************
//
// I2C Master error status.
//
//*****************************************************************************
#define I2C_MASTER_ERR_NONE     0
#define I2C_MASTER_ERR_ADDR_ACK 0x00000004
#define I2C_MASTER_ERR_DATA_ACK 0x00000008
#define I2C_MASTER_ERR_ARB_LOST 0x00000010
#define I2C_MASTER_ERR_CLK_TOUT 0x00000080

//*****************************************************************************
//
// I2C Slave action requests
//
//*****************************************************************************
#define I2C_SLAVE_ACT_NONE      0
#define I2C_SLAVE_ACT_RREQ      0x00000001  // Master has sent data
#define I2C_SLAVE_ACT_TREQ      0x00000002  // Master has requested data
#define I2C_SLAVE_ACT_RREQ_FBR  0x00000005  // Master has sent first byte
#define I2C_SLAVE_ACT_OWN2SEL   0x00000008  // Master requested secondary slave
#define I2C_SLAVE_ACT_QCMD      0x00000010  // Master has sent a Quick Command
#define I2C_SLAVE_ACT_QCMD_DATA 0x00000020  // Master Quick Command value

//*****************************************************************************
//
// Miscellaneous I2C driver definitions.
//
//*****************************************************************************
#define I2C_MASTER_MAX_RETRIES  1000        // Number of retries

//*****************************************************************************
//
// I2C Master interrupts.
//
//*****************************************************************************
#define I2C_MASTER_INT_RX_FIFO_FULL                                           \
                                0x00000800  // RX FIFO Full Interrupt
#define I2C_MASTER_INT_TX_FIFO_EMPTY                                          \
                                0x00000400  // TX FIFO Empty Interrupt
#define I2C_MASTER_INT_RX_FIFO_REQ                                            \
                                0x00000200  // RX FIFO Request Interrupt
#define I2C_MASTER_INT_TX_FIFO_REQ                                            \
                                0x00000100  // TX FIFO Request Interrupt
#define I2C_MASTER_INT_ARB_LOST                                               \
                                0x00000080  // Arb Lost Interrupt
#define I2C_MASTER_INT_STOP     0x00000040  // Stop Condition Interrupt
#define I2C_MASTER_INT_START    0x00000020  // Start Condition Interrupt
#define I2C_MASTER_INT_NACK     0x00000010  // Addr/Data NACK Interrupt
#define I2C_MASTER_INT_TX_DMA_DONE                                            \
                                0x00000008  // TX DMA Complete Interrupt
#define I2C_MASTER_INT_RX_DMA_DONE                                            \
                                0x00000004  // RX DMA Complete Interrupt
#define I2C_MASTER_INT_TIMEOUT  0x00000002  // Clock Timeout Interrupt
#define I2C_MASTER_INT_DATA     0x00000001  // Data Interrupt

//*****************************************************************************
//
// I2C Slave interrupts.
//
//*****************************************************************************
#define I2C_SLAVE_INT_RX_FIFO_FULL                                            \
                                0x00000100  // RX FIFO Full Interrupt
#define I2C_SLAVE_INT_TX_FIFO_EMPTY                                           \
                                0x00000080  // TX FIFO Empty Interrupt
#define I2C_SLAVE_INT_RX_FIFO_REQ                                             \
                                0x00000040  // RX FIFO Request Interrupt
#define I2C_SLAVE_INT_TX_FIFO_REQ                                             \
                                0x00000020  // TX FIFO Request Interrupt
#define I2C_SLAVE_INT_TX_DMA_DONE                                             \
                                0x00000010  // TX DMA Complete Interrupt
#define I2C_SLAVE_INT_RX_DMA_DONE                                             \
                                0x00000008  // RX DMA Complete Interrupt
#define I2C_SLAVE_INT_STOP      0x00000004  // Stop Condition Interrupt
#define I2C_SLAVE_INT_START     0x00000002  // Start Condition Interrupt
#define I2C_SLAVE_INT_DATA      0x00000001  // Data Interrupt

//*****************************************************************************
//
// I2C Slave FIFO configuration macros.
//
//*****************************************************************************
#define I2C_SLAVE_TX_FIFO_ENABLE                                              \
                                0x00000002
#define I2C_SLAVE_RX_FIFO_ENABLE                                              \
                                0x00000004

//*****************************************************************************
//
// I2C FIFO configuration macros.
//
//*****************************************************************************
#define I2C_FIFO_CFG_TX_MASTER  0x00000000
#define I2C_FIFO_CFG_TX_SLAVE   0x00008000
#define I2C_FIFO_CFG_RX_MASTER  0x00000000
#define I2C_FIFO_CFG_RX_SLAVE   0x80000000
#define I2C_FIFO_CFG_TX_MASTER_DMA                                            \
                                0x00002000
#define I2C_FIFO_CFG_TX_SLAVE_DMA                                             \
                                0x0000a000
#define I2C_FIFO_CFG_RX_MASTER_DMA                                            \
                                0x20000000
#define I2C_FIFO_CFG_RX_SLAVE_DMA                                             \
                                0xa0000000
#define I2C_FIFO_CFG_TX_NO_TRIG 0x00000000
#define I2C_FIFO_CFG_TX_TRIG_1  0x00000001
#define I2C_FIFO_CFG_TX_TRIG_2  0x00000002
#define I2C_FIFO_CFG_TX_TRIG_3  0x00000003
#define I2C_FIFO_CFG_TX_TRIG_4  0x00000004
#define I2C_FIFO_CFG_TX_TRIG_5  0x00000005
#define I2C_FIFO_CFG_TX_TRIG_6  0x00000006
#define I2C_FIFO_CFG_TX_TRIG_7  0x00000007
#define I2C_FIFO_CFG_TX_TRIG_8  0x00000008
#define I2C_FIFO_CFG_RX_NO_TRIG 0x00000000
#define I2C_FIFO_CFG_RX_TRIG_1  0x00010000
#define I2C_FIFO_CFG_RX_TRIG_2  0x00020000
#define I2C_FIFO_CFG_RX_TRIG_3  0x00030000
#define I2C_FIFO_CFG_RX_TRIG_4  0x00040000
#define I2C_FIFO_CFG_RX_TRIG_5  0x00050000
#define I2C_FIFO_CFG_RX_TRIG_6  0x00060000
#define I2C_FIFO_CFG_RX_TRIG_7  0x00070000
#define I2C_FIFO_CFG_RX_TRIG_8  0x00080000

//*****************************************************************************
//
// I2C FIFO status.
//
//*****************************************************************************
#define I2C_FIFO_RX_BELOW_TRIG_LEVEL                                          \
                                0x00040000
#define I2C_FIFO_RX_FULL        0x00020000
#define I2C_FIFO_RX_EMPTY       0x00010000
#define I2C_FIFO_TX_BELOW_TRIG_LEVEL                                          \
                                0x00000004
#define I2C_FIFO_TX_FULL        0x00000002
#define I2C_FIFO_TX_EMPTY       0x00000001

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************
extern void I2CIntRegister(uint32_t ui32Base, void(pfnHandler)(void));
extern void I2CIntUnregister(uint32_t ui32Base);
extern void I2CTxFIFOConfigSet(uint32_t ui32Base, uint32_t ui32Config);
extern void I2CTxFIFOFlush(uint32_t ui32Base);
extern void I2CRxFIFOConfigSet(uint32_t ui32Base, uint32_t ui32Config);
extern void I2CRxFIFOFlush(uint32_t ui32Base);
extern uint32_t I2CFIFOStatus(uint32_t ui32Base);
extern void I2CFIFODataPut(uint32_t ui32Base, uint8_t ui8Data);
extern uint32_t I2CFIFODataPutNonBlocking(uint32_t ui32Base,
                                          uint8_t ui8Data);
extern uint32_t I2CFIFODataGet(uint32_t ui32Base);
extern uint32_t I2CFIFODataGetNonBlocking(uint32_t ui32Base,
                                          uint8_t *pui8Data);
extern void I2CMasterBurstLengthSet(uint32_t ui32Base,
                                    uint8_t ui8Length);
extern uint32_t I2CMasterBurstCountGet(uint32_t ui32Base);
extern void I2CMasterGlitchFilterConfigSet(uint32_t ui32Base,
                                           uint32_t ui32Config);
extern void I2CSlaveFIFOEnable(uint32_t ui32Base, uint32_t ui32Config);
extern void I2CSlaveFIFODisable(uint32_t ui32Base);
extern bool I2CMasterBusBusy(uint32_t ui32Base);
extern bool I2CMasterBusy(uint32_t ui32Base);
extern void I2CMasterControl(uint32_t ui32Base, uint32_t ui32Cmd);
extern uint32_t I2CMasterDataGet(uint32_t ui32Base);
extern void I2CMasterDataPut(uint32_t ui32Base, uint8_t ui8Data);
extern void I2CMasterDisable(uint32_t ui32Base);
extern void I2CMasterEnable(uint32_t ui32Base);
extern uint32_t I2CMasterErr(uint32_t ui32Base);
extern void I2CMasterInitExpClk(uint32_t ui32Base, uint32_t ui32I2CClk,
                                bool bFast);
extern void I2CMasterIntClear(uint32_t ui32Base);
extern void I2CMasterIntDisable(uint32_t ui32Base);
extern void I2CMasterIntEnable(uint32_t ui32Base);
extern bool I2CMasterIntStatus(uint32_t ui32Base, bool bMasked);
extern void I2CMasterIntEnableEx(uint32_t ui32Base,
                                 uint32_t ui32IntFlags);
extern void I2CMasterIntDisableEx(uint32_t ui32Base,
                                  uint32_t ui32IntFlags);
extern uint32_t I2CMasterIntStatusEx(uint32_t ui32Base,
                                       bool bMasked);
extern void I2CMasterIntClearEx(uint32_t ui32Base,
                                uint32_t ui32IntFlags);
extern void I2CMasterTimeoutSet(uint32_t ui32Base, uint32_t ui32Value);
extern void I2CSlaveACKOverride(uint32_t ui32Base, bool bEnable);
extern void I2CSlaveACKValueSet(uint32_t ui32Base, bool bACK);
extern uint32_t I2CMasterLineStateGet(uint32_t ui32Base);
extern void I2CMasterSlaveAddrSet(uint32_t ui32Base,
                                  uint8_t ui8SlaveAddr,
                                  bool bReceive);
extern uint32_t I2CSlaveDataGet(uint32_t ui32Base);
extern void I2CSlaveDataPut(uint32_t ui32Base, uint8_t ui8Data);
extern void I2CSlaveDisable(uint32_t ui32Base);
extern void I2CSlaveEnable(uint32_t ui32Base);
extern void I2CSlaveInit(uint32_t ui32Base, uint8_t ui8SlaveAddr);
extern void I2CSlaveAddressSet(uint32_t ui32Base, uint8_t ui8AddrNum,
                                 uint8_t ui8SlaveAddr);
extern void I2CSlaveIntClear(uint32_t ui32Base);
extern void I2CSlaveIntDisable(uint32_t ui32Base);
extern void I2CSlaveIntEnable(uint32_t ui32Base);
extern void I2CSlaveIntClearEx(uint32_t ui32Base, uint32_t ui32IntFlags);
extern void I2CSlaveIntDisableEx(uint32_t ui32Base,
                                 uint32_t ui32IntFlags);
extern void I2CSlaveIntEnableEx(uint32_t ui32Base, uint32_t ui32IntFlags);
extern bool I2CSlaveIntStatus(uint32_t ui32Base, bool bMasked);
extern uint32_t I2CSlaveIntStatusEx(uint32_t ui32Base,
                                      bool bMasked);
extern uint32_t I2CSlaveStatus(uint32_t ui32Base);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __DRIVERLIB_I2C_H__
