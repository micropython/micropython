/* Auto-generated config file hpl_dmac_config.h */
#ifndef HPL_DMAC_CONFIG_H
#define HPL_DMAC_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <e> DMAC enable
// <i> Indicates whether dmac is enabled or not
// <id> dmac_enable
#ifndef CONF_DMAC_ENABLE
#define CONF_DMAC_ENABLE 0
#endif

// <q> Priority Level 0
// <i> Indicates whether Priority Level 0 is enabled or not
// <id> dmac_lvlen0
#ifndef CONF_DMAC_LVLEN0
#define CONF_DMAC_LVLEN0 1
#endif

// <o> Level 0 Round-Robin Arbitration
// <0=> Static arbitration scheme for channel with priority 0
// <1=> Round-robin arbitration scheme for channel with priority 0
// <i> Defines Level 0 Arbitration for DMA channels
// <id> dmac_rrlvlen0
#ifndef CONF_DMAC_RRLVLEN0
#define CONF_DMAC_RRLVLEN0 0
#endif

// <o> Level 0 Channel Priority Number <0x00-0xFF>
// <id> dmac_lvlpri0
#ifndef CONF_DMAC_LVLPRI0
#define CONF_DMAC_LVLPRI0 0
#endif
// <q> Priority Level 1
// <i> Indicates whether Priority Level 1 is enabled or not
// <id> dmac_lvlen1
#ifndef CONF_DMAC_LVLEN1
#define CONF_DMAC_LVLEN1 1
#endif

// <o> Level 1 Round-Robin Arbitration
// <0=> Static arbitration scheme for channel with priority 1
// <1=> Round-robin arbitration scheme for channel with priority 1
// <i> Defines Level 1 Arbitration for DMA channels
// <id> dmac_rrlvlen1
#ifndef CONF_DMAC_RRLVLEN1
#define CONF_DMAC_RRLVLEN1 0
#endif

// <o> Level 1 Channel Priority Number <0x00-0xFF>
// <id> dmac_lvlpri1
#ifndef CONF_DMAC_LVLPRI1
#define CONF_DMAC_LVLPRI1 0
#endif
// <q> Priority Level 2
// <i> Indicates whether Priority Level 2 is enabled or not
// <id> dmac_lvlen2
#ifndef CONF_DMAC_LVLEN2
#define CONF_DMAC_LVLEN2 1
#endif

// <o> Level 2 Round-Robin Arbitration
// <0=> Static arbitration scheme for channel with priority 2
// <1=> Round-robin arbitration scheme for channel with priority 2
// <i> Defines Level 2 Arbitration for DMA channels
// <id> dmac_rrlvlen2
#ifndef CONF_DMAC_RRLVLEN2
#define CONF_DMAC_RRLVLEN2 0
#endif

// <o> Level 2 Channel Priority Number <0x00-0xFF>
// <id> dmac_lvlpri2
#ifndef CONF_DMAC_LVLPRI2
#define CONF_DMAC_LVLPRI2 0
#endif
// <q> Priority Level 3
// <i> Indicates whether Priority Level 3 is enabled or not
// <id> dmac_lvlen3
#ifndef CONF_DMAC_LVLEN3
#define CONF_DMAC_LVLEN3 1
#endif

// <o> Level 3 Round-Robin Arbitration
// <0=> Static arbitration scheme for channel with priority 3
// <1=> Round-robin arbitration scheme for channel with priority 3
// <i> Defines Level 3 Arbitration for DMA channels
// <id> dmac_rrlvlen3
#ifndef CONF_DMAC_RRLVLEN3
#define CONF_DMAC_RRLVLEN3 0
#endif

// <o> Level 3 Channel Priority Number <0x00-0xFF>
// <id> dmac_lvlpri3
#ifndef CONF_DMAC_LVLPRI3
#define CONF_DMAC_LVLPRI3 0
#endif
// <q> Debug Run
// <i> Indicates whether Debug Run is enabled or not
// <id> dmac_dbgrun
#ifndef CONF_DMAC_DBGRUN
#define CONF_DMAC_DBGRUN 0
#endif

// <e> Channel 0 settings
// <id> dmac_channel_0_settings
#ifndef CONF_DMAC_CHANNEL_0_SETTINGS
#define CONF_DMAC_CHANNEL_0_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 0 is running in standby mode or not
// <id> dmac_runstdby_0
#ifndef CONF_DMAC_RUNSTDBY_0
#define CONF_DMAC_RUNSTDBY_0 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_0
#ifndef CONF_DMAC_TRIGACT_0
#define CONF_DMAC_TRIGACT_0 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_0
#ifndef CONF_DMAC_TRIGSRC_0
#define CONF_DMAC_TRIGSRC_0 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_0
#ifndef CONF_DMAC_LVL_0
#define CONF_DMAC_LVL_0 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_0
#ifndef CONF_DMAC_EVOE_0
#define CONF_DMAC_EVOE_0 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_0
#ifndef CONF_DMAC_EVIE_0
#define CONF_DMAC_EVIE_0 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_0
#ifndef CONF_DMAC_EVACT_0
#define CONF_DMAC_EVACT_0 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_0
#ifndef CONF_DMAC_STEPSIZE_0
#define CONF_DMAC_STEPSIZE_0 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_0
#ifndef CONF_DMAC_STEPSEL_0
#define CONF_DMAC_STEPSEL_0 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_0
#ifndef CONF_DMAC_SRCINC_0
#define CONF_DMAC_SRCINC_0 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_0
#ifndef CONF_DMAC_DSTINC_0
#define CONF_DMAC_DSTINC_0 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_0
#ifndef CONF_DMAC_BEATSIZE_0
#define CONF_DMAC_BEATSIZE_0 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_0
#ifndef CONF_DMAC_BLOCKACT_0
#define CONF_DMAC_BLOCKACT_0 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_0
#ifndef CONF_DMAC_EVOSEL_0
#define CONF_DMAC_EVOSEL_0 0
#endif
// </e>

// <e> Channel 1 settings
// <id> dmac_channel_1_settings
#ifndef CONF_DMAC_CHANNEL_1_SETTINGS
#define CONF_DMAC_CHANNEL_1_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 1 is running in standby mode or not
// <id> dmac_runstdby_1
#ifndef CONF_DMAC_RUNSTDBY_1
#define CONF_DMAC_RUNSTDBY_1 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_1
#ifndef CONF_DMAC_TRIGACT_1
#define CONF_DMAC_TRIGACT_1 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_1
#ifndef CONF_DMAC_TRIGSRC_1
#define CONF_DMAC_TRIGSRC_1 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_1
#ifndef CONF_DMAC_LVL_1
#define CONF_DMAC_LVL_1 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_1
#ifndef CONF_DMAC_EVOE_1
#define CONF_DMAC_EVOE_1 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_1
#ifndef CONF_DMAC_EVIE_1
#define CONF_DMAC_EVIE_1 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_1
#ifndef CONF_DMAC_EVACT_1
#define CONF_DMAC_EVACT_1 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_1
#ifndef CONF_DMAC_STEPSIZE_1
#define CONF_DMAC_STEPSIZE_1 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_1
#ifndef CONF_DMAC_STEPSEL_1
#define CONF_DMAC_STEPSEL_1 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_1
#ifndef CONF_DMAC_SRCINC_1
#define CONF_DMAC_SRCINC_1 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_1
#ifndef CONF_DMAC_DSTINC_1
#define CONF_DMAC_DSTINC_1 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_1
#ifndef CONF_DMAC_BEATSIZE_1
#define CONF_DMAC_BEATSIZE_1 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_1
#ifndef CONF_DMAC_BLOCKACT_1
#define CONF_DMAC_BLOCKACT_1 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_1
#ifndef CONF_DMAC_EVOSEL_1
#define CONF_DMAC_EVOSEL_1 0
#endif
// </e>

// <e> Channel 2 settings
// <id> dmac_channel_2_settings
#ifndef CONF_DMAC_CHANNEL_2_SETTINGS
#define CONF_DMAC_CHANNEL_2_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 2 is running in standby mode or not
// <id> dmac_runstdby_2
#ifndef CONF_DMAC_RUNSTDBY_2
#define CONF_DMAC_RUNSTDBY_2 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_2
#ifndef CONF_DMAC_TRIGACT_2
#define CONF_DMAC_TRIGACT_2 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_2
#ifndef CONF_DMAC_TRIGSRC_2
#define CONF_DMAC_TRIGSRC_2 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_2
#ifndef CONF_DMAC_LVL_2
#define CONF_DMAC_LVL_2 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_2
#ifndef CONF_DMAC_EVOE_2
#define CONF_DMAC_EVOE_2 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_2
#ifndef CONF_DMAC_EVIE_2
#define CONF_DMAC_EVIE_2 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_2
#ifndef CONF_DMAC_EVACT_2
#define CONF_DMAC_EVACT_2 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_2
#ifndef CONF_DMAC_STEPSIZE_2
#define CONF_DMAC_STEPSIZE_2 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_2
#ifndef CONF_DMAC_STEPSEL_2
#define CONF_DMAC_STEPSEL_2 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_2
#ifndef CONF_DMAC_SRCINC_2
#define CONF_DMAC_SRCINC_2 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_2
#ifndef CONF_DMAC_DSTINC_2
#define CONF_DMAC_DSTINC_2 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_2
#ifndef CONF_DMAC_BEATSIZE_2
#define CONF_DMAC_BEATSIZE_2 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_2
#ifndef CONF_DMAC_BLOCKACT_2
#define CONF_DMAC_BLOCKACT_2 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_2
#ifndef CONF_DMAC_EVOSEL_2
#define CONF_DMAC_EVOSEL_2 0
#endif
// </e>

// <e> Channel 3 settings
// <id> dmac_channel_3_settings
#ifndef CONF_DMAC_CHANNEL_3_SETTINGS
#define CONF_DMAC_CHANNEL_3_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 3 is running in standby mode or not
// <id> dmac_runstdby_3
#ifndef CONF_DMAC_RUNSTDBY_3
#define CONF_DMAC_RUNSTDBY_3 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_3
#ifndef CONF_DMAC_TRIGACT_3
#define CONF_DMAC_TRIGACT_3 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_3
#ifndef CONF_DMAC_TRIGSRC_3
#define CONF_DMAC_TRIGSRC_3 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_3
#ifndef CONF_DMAC_LVL_3
#define CONF_DMAC_LVL_3 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_3
#ifndef CONF_DMAC_EVOE_3
#define CONF_DMAC_EVOE_3 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_3
#ifndef CONF_DMAC_EVIE_3
#define CONF_DMAC_EVIE_3 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_3
#ifndef CONF_DMAC_EVACT_3
#define CONF_DMAC_EVACT_3 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_3
#ifndef CONF_DMAC_STEPSIZE_3
#define CONF_DMAC_STEPSIZE_3 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_3
#ifndef CONF_DMAC_STEPSEL_3
#define CONF_DMAC_STEPSEL_3 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_3
#ifndef CONF_DMAC_SRCINC_3
#define CONF_DMAC_SRCINC_3 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_3
#ifndef CONF_DMAC_DSTINC_3
#define CONF_DMAC_DSTINC_3 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_3
#ifndef CONF_DMAC_BEATSIZE_3
#define CONF_DMAC_BEATSIZE_3 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_3
#ifndef CONF_DMAC_BLOCKACT_3
#define CONF_DMAC_BLOCKACT_3 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_3
#ifndef CONF_DMAC_EVOSEL_3
#define CONF_DMAC_EVOSEL_3 0
#endif
// </e>

// <e> Channel 4 settings
// <id> dmac_channel_4_settings
#ifndef CONF_DMAC_CHANNEL_4_SETTINGS
#define CONF_DMAC_CHANNEL_4_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 4 is running in standby mode or not
// <id> dmac_runstdby_4
#ifndef CONF_DMAC_RUNSTDBY_4
#define CONF_DMAC_RUNSTDBY_4 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_4
#ifndef CONF_DMAC_TRIGACT_4
#define CONF_DMAC_TRIGACT_4 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_4
#ifndef CONF_DMAC_TRIGSRC_4
#define CONF_DMAC_TRIGSRC_4 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_4
#ifndef CONF_DMAC_LVL_4
#define CONF_DMAC_LVL_4 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_4
#ifndef CONF_DMAC_EVOE_4
#define CONF_DMAC_EVOE_4 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_4
#ifndef CONF_DMAC_EVIE_4
#define CONF_DMAC_EVIE_4 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_4
#ifndef CONF_DMAC_EVACT_4
#define CONF_DMAC_EVACT_4 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_4
#ifndef CONF_DMAC_STEPSIZE_4
#define CONF_DMAC_STEPSIZE_4 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_4
#ifndef CONF_DMAC_STEPSEL_4
#define CONF_DMAC_STEPSEL_4 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_4
#ifndef CONF_DMAC_SRCINC_4
#define CONF_DMAC_SRCINC_4 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_4
#ifndef CONF_DMAC_DSTINC_4
#define CONF_DMAC_DSTINC_4 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_4
#ifndef CONF_DMAC_BEATSIZE_4
#define CONF_DMAC_BEATSIZE_4 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_4
#ifndef CONF_DMAC_BLOCKACT_4
#define CONF_DMAC_BLOCKACT_4 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_4
#ifndef CONF_DMAC_EVOSEL_4
#define CONF_DMAC_EVOSEL_4 0
#endif
// </e>

// <e> Channel 5 settings
// <id> dmac_channel_5_settings
#ifndef CONF_DMAC_CHANNEL_5_SETTINGS
#define CONF_DMAC_CHANNEL_5_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 5 is running in standby mode or not
// <id> dmac_runstdby_5
#ifndef CONF_DMAC_RUNSTDBY_5
#define CONF_DMAC_RUNSTDBY_5 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_5
#ifndef CONF_DMAC_TRIGACT_5
#define CONF_DMAC_TRIGACT_5 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_5
#ifndef CONF_DMAC_TRIGSRC_5
#define CONF_DMAC_TRIGSRC_5 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_5
#ifndef CONF_DMAC_LVL_5
#define CONF_DMAC_LVL_5 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_5
#ifndef CONF_DMAC_EVOE_5
#define CONF_DMAC_EVOE_5 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_5
#ifndef CONF_DMAC_EVIE_5
#define CONF_DMAC_EVIE_5 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_5
#ifndef CONF_DMAC_EVACT_5
#define CONF_DMAC_EVACT_5 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_5
#ifndef CONF_DMAC_STEPSIZE_5
#define CONF_DMAC_STEPSIZE_5 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_5
#ifndef CONF_DMAC_STEPSEL_5
#define CONF_DMAC_STEPSEL_5 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_5
#ifndef CONF_DMAC_SRCINC_5
#define CONF_DMAC_SRCINC_5 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_5
#ifndef CONF_DMAC_DSTINC_5
#define CONF_DMAC_DSTINC_5 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_5
#ifndef CONF_DMAC_BEATSIZE_5
#define CONF_DMAC_BEATSIZE_5 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_5
#ifndef CONF_DMAC_BLOCKACT_5
#define CONF_DMAC_BLOCKACT_5 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_5
#ifndef CONF_DMAC_EVOSEL_5
#define CONF_DMAC_EVOSEL_5 0
#endif
// </e>

// <e> Channel 6 settings
// <id> dmac_channel_6_settings
#ifndef CONF_DMAC_CHANNEL_6_SETTINGS
#define CONF_DMAC_CHANNEL_6_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 6 is running in standby mode or not
// <id> dmac_runstdby_6
#ifndef CONF_DMAC_RUNSTDBY_6
#define CONF_DMAC_RUNSTDBY_6 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_6
#ifndef CONF_DMAC_TRIGACT_6
#define CONF_DMAC_TRIGACT_6 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_6
#ifndef CONF_DMAC_TRIGSRC_6
#define CONF_DMAC_TRIGSRC_6 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_6
#ifndef CONF_DMAC_LVL_6
#define CONF_DMAC_LVL_6 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_6
#ifndef CONF_DMAC_EVOE_6
#define CONF_DMAC_EVOE_6 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_6
#ifndef CONF_DMAC_EVIE_6
#define CONF_DMAC_EVIE_6 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_6
#ifndef CONF_DMAC_EVACT_6
#define CONF_DMAC_EVACT_6 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_6
#ifndef CONF_DMAC_STEPSIZE_6
#define CONF_DMAC_STEPSIZE_6 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_6
#ifndef CONF_DMAC_STEPSEL_6
#define CONF_DMAC_STEPSEL_6 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_6
#ifndef CONF_DMAC_SRCINC_6
#define CONF_DMAC_SRCINC_6 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_6
#ifndef CONF_DMAC_DSTINC_6
#define CONF_DMAC_DSTINC_6 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_6
#ifndef CONF_DMAC_BEATSIZE_6
#define CONF_DMAC_BEATSIZE_6 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_6
#ifndef CONF_DMAC_BLOCKACT_6
#define CONF_DMAC_BLOCKACT_6 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_6
#ifndef CONF_DMAC_EVOSEL_6
#define CONF_DMAC_EVOSEL_6 0
#endif
// </e>

// <e> Channel 7 settings
// <id> dmac_channel_7_settings
#ifndef CONF_DMAC_CHANNEL_7_SETTINGS
#define CONF_DMAC_CHANNEL_7_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 7 is running in standby mode or not
// <id> dmac_runstdby_7
#ifndef CONF_DMAC_RUNSTDBY_7
#define CONF_DMAC_RUNSTDBY_7 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_7
#ifndef CONF_DMAC_TRIGACT_7
#define CONF_DMAC_TRIGACT_7 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_7
#ifndef CONF_DMAC_TRIGSRC_7
#define CONF_DMAC_TRIGSRC_7 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_7
#ifndef CONF_DMAC_LVL_7
#define CONF_DMAC_LVL_7 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_7
#ifndef CONF_DMAC_EVOE_7
#define CONF_DMAC_EVOE_7 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_7
#ifndef CONF_DMAC_EVIE_7
#define CONF_DMAC_EVIE_7 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_7
#ifndef CONF_DMAC_EVACT_7
#define CONF_DMAC_EVACT_7 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_7
#ifndef CONF_DMAC_STEPSIZE_7
#define CONF_DMAC_STEPSIZE_7 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_7
#ifndef CONF_DMAC_STEPSEL_7
#define CONF_DMAC_STEPSEL_7 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_7
#ifndef CONF_DMAC_SRCINC_7
#define CONF_DMAC_SRCINC_7 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_7
#ifndef CONF_DMAC_DSTINC_7
#define CONF_DMAC_DSTINC_7 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_7
#ifndef CONF_DMAC_BEATSIZE_7
#define CONF_DMAC_BEATSIZE_7 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_7
#ifndef CONF_DMAC_BLOCKACT_7
#define CONF_DMAC_BLOCKACT_7 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_7
#ifndef CONF_DMAC_EVOSEL_7
#define CONF_DMAC_EVOSEL_7 0
#endif
// </e>

// <e> Channel 8 settings
// <id> dmac_channel_8_settings
#ifndef CONF_DMAC_CHANNEL_8_SETTINGS
#define CONF_DMAC_CHANNEL_8_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 8 is running in standby mode or not
// <id> dmac_runstdby_8
#ifndef CONF_DMAC_RUNSTDBY_8
#define CONF_DMAC_RUNSTDBY_8 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_8
#ifndef CONF_DMAC_TRIGACT_8
#define CONF_DMAC_TRIGACT_8 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_8
#ifndef CONF_DMAC_TRIGSRC_8
#define CONF_DMAC_TRIGSRC_8 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_8
#ifndef CONF_DMAC_LVL_8
#define CONF_DMAC_LVL_8 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_8
#ifndef CONF_DMAC_EVOE_8
#define CONF_DMAC_EVOE_8 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_8
#ifndef CONF_DMAC_EVIE_8
#define CONF_DMAC_EVIE_8 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_8
#ifndef CONF_DMAC_EVACT_8
#define CONF_DMAC_EVACT_8 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_8
#ifndef CONF_DMAC_STEPSIZE_8
#define CONF_DMAC_STEPSIZE_8 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_8
#ifndef CONF_DMAC_STEPSEL_8
#define CONF_DMAC_STEPSEL_8 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_8
#ifndef CONF_DMAC_SRCINC_8
#define CONF_DMAC_SRCINC_8 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_8
#ifndef CONF_DMAC_DSTINC_8
#define CONF_DMAC_DSTINC_8 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_8
#ifndef CONF_DMAC_BEATSIZE_8
#define CONF_DMAC_BEATSIZE_8 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_8
#ifndef CONF_DMAC_BLOCKACT_8
#define CONF_DMAC_BLOCKACT_8 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_8
#ifndef CONF_DMAC_EVOSEL_8
#define CONF_DMAC_EVOSEL_8 0
#endif
// </e>

// <e> Channel 9 settings
// <id> dmac_channel_9_settings
#ifndef CONF_DMAC_CHANNEL_9_SETTINGS
#define CONF_DMAC_CHANNEL_9_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 9 is running in standby mode or not
// <id> dmac_runstdby_9
#ifndef CONF_DMAC_RUNSTDBY_9
#define CONF_DMAC_RUNSTDBY_9 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_9
#ifndef CONF_DMAC_TRIGACT_9
#define CONF_DMAC_TRIGACT_9 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_9
#ifndef CONF_DMAC_TRIGSRC_9
#define CONF_DMAC_TRIGSRC_9 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_9
#ifndef CONF_DMAC_LVL_9
#define CONF_DMAC_LVL_9 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_9
#ifndef CONF_DMAC_EVOE_9
#define CONF_DMAC_EVOE_9 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_9
#ifndef CONF_DMAC_EVIE_9
#define CONF_DMAC_EVIE_9 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_9
#ifndef CONF_DMAC_EVACT_9
#define CONF_DMAC_EVACT_9 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_9
#ifndef CONF_DMAC_STEPSIZE_9
#define CONF_DMAC_STEPSIZE_9 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_9
#ifndef CONF_DMAC_STEPSEL_9
#define CONF_DMAC_STEPSEL_9 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_9
#ifndef CONF_DMAC_SRCINC_9
#define CONF_DMAC_SRCINC_9 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_9
#ifndef CONF_DMAC_DSTINC_9
#define CONF_DMAC_DSTINC_9 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_9
#ifndef CONF_DMAC_BEATSIZE_9
#define CONF_DMAC_BEATSIZE_9 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_9
#ifndef CONF_DMAC_BLOCKACT_9
#define CONF_DMAC_BLOCKACT_9 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_9
#ifndef CONF_DMAC_EVOSEL_9
#define CONF_DMAC_EVOSEL_9 0
#endif
// </e>

// <e> Channel 10 settings
// <id> dmac_channel_10_settings
#ifndef CONF_DMAC_CHANNEL_10_SETTINGS
#define CONF_DMAC_CHANNEL_10_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 10 is running in standby mode or not
// <id> dmac_runstdby_10
#ifndef CONF_DMAC_RUNSTDBY_10
#define CONF_DMAC_RUNSTDBY_10 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_10
#ifndef CONF_DMAC_TRIGACT_10
#define CONF_DMAC_TRIGACT_10 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_10
#ifndef CONF_DMAC_TRIGSRC_10
#define CONF_DMAC_TRIGSRC_10 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_10
#ifndef CONF_DMAC_LVL_10
#define CONF_DMAC_LVL_10 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_10
#ifndef CONF_DMAC_EVOE_10
#define CONF_DMAC_EVOE_10 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_10
#ifndef CONF_DMAC_EVIE_10
#define CONF_DMAC_EVIE_10 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_10
#ifndef CONF_DMAC_EVACT_10
#define CONF_DMAC_EVACT_10 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_10
#ifndef CONF_DMAC_STEPSIZE_10
#define CONF_DMAC_STEPSIZE_10 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_10
#ifndef CONF_DMAC_STEPSEL_10
#define CONF_DMAC_STEPSEL_10 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_10
#ifndef CONF_DMAC_SRCINC_10
#define CONF_DMAC_SRCINC_10 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_10
#ifndef CONF_DMAC_DSTINC_10
#define CONF_DMAC_DSTINC_10 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_10
#ifndef CONF_DMAC_BEATSIZE_10
#define CONF_DMAC_BEATSIZE_10 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_10
#ifndef CONF_DMAC_BLOCKACT_10
#define CONF_DMAC_BLOCKACT_10 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_10
#ifndef CONF_DMAC_EVOSEL_10
#define CONF_DMAC_EVOSEL_10 0
#endif
// </e>

// <e> Channel 11 settings
// <id> dmac_channel_11_settings
#ifndef CONF_DMAC_CHANNEL_11_SETTINGS
#define CONF_DMAC_CHANNEL_11_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 11 is running in standby mode or not
// <id> dmac_runstdby_11
#ifndef CONF_DMAC_RUNSTDBY_11
#define CONF_DMAC_RUNSTDBY_11 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_11
#ifndef CONF_DMAC_TRIGACT_11
#define CONF_DMAC_TRIGACT_11 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_11
#ifndef CONF_DMAC_TRIGSRC_11
#define CONF_DMAC_TRIGSRC_11 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_11
#ifndef CONF_DMAC_LVL_11
#define CONF_DMAC_LVL_11 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_11
#ifndef CONF_DMAC_EVOE_11
#define CONF_DMAC_EVOE_11 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_11
#ifndef CONF_DMAC_EVIE_11
#define CONF_DMAC_EVIE_11 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_11
#ifndef CONF_DMAC_EVACT_11
#define CONF_DMAC_EVACT_11 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_11
#ifndef CONF_DMAC_STEPSIZE_11
#define CONF_DMAC_STEPSIZE_11 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_11
#ifndef CONF_DMAC_STEPSEL_11
#define CONF_DMAC_STEPSEL_11 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_11
#ifndef CONF_DMAC_SRCINC_11
#define CONF_DMAC_SRCINC_11 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_11
#ifndef CONF_DMAC_DSTINC_11
#define CONF_DMAC_DSTINC_11 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_11
#ifndef CONF_DMAC_BEATSIZE_11
#define CONF_DMAC_BEATSIZE_11 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_11
#ifndef CONF_DMAC_BLOCKACT_11
#define CONF_DMAC_BLOCKACT_11 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_11
#ifndef CONF_DMAC_EVOSEL_11
#define CONF_DMAC_EVOSEL_11 0
#endif
// </e>

// <e> Channel 12 settings
// <id> dmac_channel_12_settings
#ifndef CONF_DMAC_CHANNEL_12_SETTINGS
#define CONF_DMAC_CHANNEL_12_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 12 is running in standby mode or not
// <id> dmac_runstdby_12
#ifndef CONF_DMAC_RUNSTDBY_12
#define CONF_DMAC_RUNSTDBY_12 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_12
#ifndef CONF_DMAC_TRIGACT_12
#define CONF_DMAC_TRIGACT_12 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_12
#ifndef CONF_DMAC_TRIGSRC_12
#define CONF_DMAC_TRIGSRC_12 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_12
#ifndef CONF_DMAC_LVL_12
#define CONF_DMAC_LVL_12 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_12
#ifndef CONF_DMAC_EVOE_12
#define CONF_DMAC_EVOE_12 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_12
#ifndef CONF_DMAC_EVIE_12
#define CONF_DMAC_EVIE_12 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_12
#ifndef CONF_DMAC_EVACT_12
#define CONF_DMAC_EVACT_12 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_12
#ifndef CONF_DMAC_STEPSIZE_12
#define CONF_DMAC_STEPSIZE_12 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_12
#ifndef CONF_DMAC_STEPSEL_12
#define CONF_DMAC_STEPSEL_12 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_12
#ifndef CONF_DMAC_SRCINC_12
#define CONF_DMAC_SRCINC_12 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_12
#ifndef CONF_DMAC_DSTINC_12
#define CONF_DMAC_DSTINC_12 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_12
#ifndef CONF_DMAC_BEATSIZE_12
#define CONF_DMAC_BEATSIZE_12 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_12
#ifndef CONF_DMAC_BLOCKACT_12
#define CONF_DMAC_BLOCKACT_12 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_12
#ifndef CONF_DMAC_EVOSEL_12
#define CONF_DMAC_EVOSEL_12 0
#endif
// </e>

// <e> Channel 13 settings
// <id> dmac_channel_13_settings
#ifndef CONF_DMAC_CHANNEL_13_SETTINGS
#define CONF_DMAC_CHANNEL_13_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 13 is running in standby mode or not
// <id> dmac_runstdby_13
#ifndef CONF_DMAC_RUNSTDBY_13
#define CONF_DMAC_RUNSTDBY_13 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_13
#ifndef CONF_DMAC_TRIGACT_13
#define CONF_DMAC_TRIGACT_13 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_13
#ifndef CONF_DMAC_TRIGSRC_13
#define CONF_DMAC_TRIGSRC_13 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_13
#ifndef CONF_DMAC_LVL_13
#define CONF_DMAC_LVL_13 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_13
#ifndef CONF_DMAC_EVOE_13
#define CONF_DMAC_EVOE_13 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_13
#ifndef CONF_DMAC_EVIE_13
#define CONF_DMAC_EVIE_13 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_13
#ifndef CONF_DMAC_EVACT_13
#define CONF_DMAC_EVACT_13 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_13
#ifndef CONF_DMAC_STEPSIZE_13
#define CONF_DMAC_STEPSIZE_13 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_13
#ifndef CONF_DMAC_STEPSEL_13
#define CONF_DMAC_STEPSEL_13 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_13
#ifndef CONF_DMAC_SRCINC_13
#define CONF_DMAC_SRCINC_13 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_13
#ifndef CONF_DMAC_DSTINC_13
#define CONF_DMAC_DSTINC_13 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_13
#ifndef CONF_DMAC_BEATSIZE_13
#define CONF_DMAC_BEATSIZE_13 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_13
#ifndef CONF_DMAC_BLOCKACT_13
#define CONF_DMAC_BLOCKACT_13 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_13
#ifndef CONF_DMAC_EVOSEL_13
#define CONF_DMAC_EVOSEL_13 0
#endif
// </e>

// <e> Channel 14 settings
// <id> dmac_channel_14_settings
#ifndef CONF_DMAC_CHANNEL_14_SETTINGS
#define CONF_DMAC_CHANNEL_14_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 14 is running in standby mode or not
// <id> dmac_runstdby_14
#ifndef CONF_DMAC_RUNSTDBY_14
#define CONF_DMAC_RUNSTDBY_14 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_14
#ifndef CONF_DMAC_TRIGACT_14
#define CONF_DMAC_TRIGACT_14 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_14
#ifndef CONF_DMAC_TRIGSRC_14
#define CONF_DMAC_TRIGSRC_14 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_14
#ifndef CONF_DMAC_LVL_14
#define CONF_DMAC_LVL_14 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_14
#ifndef CONF_DMAC_EVOE_14
#define CONF_DMAC_EVOE_14 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_14
#ifndef CONF_DMAC_EVIE_14
#define CONF_DMAC_EVIE_14 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_14
#ifndef CONF_DMAC_EVACT_14
#define CONF_DMAC_EVACT_14 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_14
#ifndef CONF_DMAC_STEPSIZE_14
#define CONF_DMAC_STEPSIZE_14 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_14
#ifndef CONF_DMAC_STEPSEL_14
#define CONF_DMAC_STEPSEL_14 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_14
#ifndef CONF_DMAC_SRCINC_14
#define CONF_DMAC_SRCINC_14 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_14
#ifndef CONF_DMAC_DSTINC_14
#define CONF_DMAC_DSTINC_14 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_14
#ifndef CONF_DMAC_BEATSIZE_14
#define CONF_DMAC_BEATSIZE_14 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_14
#ifndef CONF_DMAC_BLOCKACT_14
#define CONF_DMAC_BLOCKACT_14 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_14
#ifndef CONF_DMAC_EVOSEL_14
#define CONF_DMAC_EVOSEL_14 0
#endif
// </e>

// <e> Channel 15 settings
// <id> dmac_channel_15_settings
#ifndef CONF_DMAC_CHANNEL_15_SETTINGS
#define CONF_DMAC_CHANNEL_15_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 15 is running in standby mode or not
// <id> dmac_runstdby_15
#ifndef CONF_DMAC_RUNSTDBY_15
#define CONF_DMAC_RUNSTDBY_15 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_15
#ifndef CONF_DMAC_TRIGACT_15
#define CONF_DMAC_TRIGACT_15 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_15
#ifndef CONF_DMAC_TRIGSRC_15
#define CONF_DMAC_TRIGSRC_15 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_15
#ifndef CONF_DMAC_LVL_15
#define CONF_DMAC_LVL_15 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_15
#ifndef CONF_DMAC_EVOE_15
#define CONF_DMAC_EVOE_15 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_15
#ifndef CONF_DMAC_EVIE_15
#define CONF_DMAC_EVIE_15 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_15
#ifndef CONF_DMAC_EVACT_15
#define CONF_DMAC_EVACT_15 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_15
#ifndef CONF_DMAC_STEPSIZE_15
#define CONF_DMAC_STEPSIZE_15 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_15
#ifndef CONF_DMAC_STEPSEL_15
#define CONF_DMAC_STEPSEL_15 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_15
#ifndef CONF_DMAC_SRCINC_15
#define CONF_DMAC_SRCINC_15 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_15
#ifndef CONF_DMAC_DSTINC_15
#define CONF_DMAC_DSTINC_15 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_15
#ifndef CONF_DMAC_BEATSIZE_15
#define CONF_DMAC_BEATSIZE_15 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_15
#ifndef CONF_DMAC_BLOCKACT_15
#define CONF_DMAC_BLOCKACT_15 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_15
#ifndef CONF_DMAC_EVOSEL_15
#define CONF_DMAC_EVOSEL_15 0
#endif
// </e>

// <e> Channel 16 settings
// <id> dmac_channel_16_settings
#ifndef CONF_DMAC_CHANNEL_16_SETTINGS
#define CONF_DMAC_CHANNEL_16_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 16 is running in standby mode or not
// <id> dmac_runstdby_16
#ifndef CONF_DMAC_RUNSTDBY_16
#define CONF_DMAC_RUNSTDBY_16 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_16
#ifndef CONF_DMAC_TRIGACT_16
#define CONF_DMAC_TRIGACT_16 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_16
#ifndef CONF_DMAC_TRIGSRC_16
#define CONF_DMAC_TRIGSRC_16 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_16
#ifndef CONF_DMAC_LVL_16
#define CONF_DMAC_LVL_16 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_16
#ifndef CONF_DMAC_EVOE_16
#define CONF_DMAC_EVOE_16 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_16
#ifndef CONF_DMAC_EVIE_16
#define CONF_DMAC_EVIE_16 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_16
#ifndef CONF_DMAC_EVACT_16
#define CONF_DMAC_EVACT_16 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_16
#ifndef CONF_DMAC_STEPSIZE_16
#define CONF_DMAC_STEPSIZE_16 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_16
#ifndef CONF_DMAC_STEPSEL_16
#define CONF_DMAC_STEPSEL_16 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_16
#ifndef CONF_DMAC_SRCINC_16
#define CONF_DMAC_SRCINC_16 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_16
#ifndef CONF_DMAC_DSTINC_16
#define CONF_DMAC_DSTINC_16 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_16
#ifndef CONF_DMAC_BEATSIZE_16
#define CONF_DMAC_BEATSIZE_16 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_16
#ifndef CONF_DMAC_BLOCKACT_16
#define CONF_DMAC_BLOCKACT_16 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_16
#ifndef CONF_DMAC_EVOSEL_16
#define CONF_DMAC_EVOSEL_16 0
#endif
// </e>

// <e> Channel 17 settings
// <id> dmac_channel_17_settings
#ifndef CONF_DMAC_CHANNEL_17_SETTINGS
#define CONF_DMAC_CHANNEL_17_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 17 is running in standby mode or not
// <id> dmac_runstdby_17
#ifndef CONF_DMAC_RUNSTDBY_17
#define CONF_DMAC_RUNSTDBY_17 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_17
#ifndef CONF_DMAC_TRIGACT_17
#define CONF_DMAC_TRIGACT_17 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_17
#ifndef CONF_DMAC_TRIGSRC_17
#define CONF_DMAC_TRIGSRC_17 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_17
#ifndef CONF_DMAC_LVL_17
#define CONF_DMAC_LVL_17 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_17
#ifndef CONF_DMAC_EVOE_17
#define CONF_DMAC_EVOE_17 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_17
#ifndef CONF_DMAC_EVIE_17
#define CONF_DMAC_EVIE_17 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_17
#ifndef CONF_DMAC_EVACT_17
#define CONF_DMAC_EVACT_17 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_17
#ifndef CONF_DMAC_STEPSIZE_17
#define CONF_DMAC_STEPSIZE_17 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_17
#ifndef CONF_DMAC_STEPSEL_17
#define CONF_DMAC_STEPSEL_17 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_17
#ifndef CONF_DMAC_SRCINC_17
#define CONF_DMAC_SRCINC_17 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_17
#ifndef CONF_DMAC_DSTINC_17
#define CONF_DMAC_DSTINC_17 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_17
#ifndef CONF_DMAC_BEATSIZE_17
#define CONF_DMAC_BEATSIZE_17 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_17
#ifndef CONF_DMAC_BLOCKACT_17
#define CONF_DMAC_BLOCKACT_17 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_17
#ifndef CONF_DMAC_EVOSEL_17
#define CONF_DMAC_EVOSEL_17 0
#endif
// </e>

// <e> Channel 18 settings
// <id> dmac_channel_18_settings
#ifndef CONF_DMAC_CHANNEL_18_SETTINGS
#define CONF_DMAC_CHANNEL_18_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 18 is running in standby mode or not
// <id> dmac_runstdby_18
#ifndef CONF_DMAC_RUNSTDBY_18
#define CONF_DMAC_RUNSTDBY_18 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_18
#ifndef CONF_DMAC_TRIGACT_18
#define CONF_DMAC_TRIGACT_18 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_18
#ifndef CONF_DMAC_TRIGSRC_18
#define CONF_DMAC_TRIGSRC_18 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_18
#ifndef CONF_DMAC_LVL_18
#define CONF_DMAC_LVL_18 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_18
#ifndef CONF_DMAC_EVOE_18
#define CONF_DMAC_EVOE_18 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_18
#ifndef CONF_DMAC_EVIE_18
#define CONF_DMAC_EVIE_18 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_18
#ifndef CONF_DMAC_EVACT_18
#define CONF_DMAC_EVACT_18 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_18
#ifndef CONF_DMAC_STEPSIZE_18
#define CONF_DMAC_STEPSIZE_18 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_18
#ifndef CONF_DMAC_STEPSEL_18
#define CONF_DMAC_STEPSEL_18 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_18
#ifndef CONF_DMAC_SRCINC_18
#define CONF_DMAC_SRCINC_18 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_18
#ifndef CONF_DMAC_DSTINC_18
#define CONF_DMAC_DSTINC_18 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_18
#ifndef CONF_DMAC_BEATSIZE_18
#define CONF_DMAC_BEATSIZE_18 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_18
#ifndef CONF_DMAC_BLOCKACT_18
#define CONF_DMAC_BLOCKACT_18 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_18
#ifndef CONF_DMAC_EVOSEL_18
#define CONF_DMAC_EVOSEL_18 0
#endif
// </e>

// <e> Channel 19 settings
// <id> dmac_channel_19_settings
#ifndef CONF_DMAC_CHANNEL_19_SETTINGS
#define CONF_DMAC_CHANNEL_19_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 19 is running in standby mode or not
// <id> dmac_runstdby_19
#ifndef CONF_DMAC_RUNSTDBY_19
#define CONF_DMAC_RUNSTDBY_19 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_19
#ifndef CONF_DMAC_TRIGACT_19
#define CONF_DMAC_TRIGACT_19 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_19
#ifndef CONF_DMAC_TRIGSRC_19
#define CONF_DMAC_TRIGSRC_19 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_19
#ifndef CONF_DMAC_LVL_19
#define CONF_DMAC_LVL_19 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_19
#ifndef CONF_DMAC_EVOE_19
#define CONF_DMAC_EVOE_19 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_19
#ifndef CONF_DMAC_EVIE_19
#define CONF_DMAC_EVIE_19 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_19
#ifndef CONF_DMAC_EVACT_19
#define CONF_DMAC_EVACT_19 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_19
#ifndef CONF_DMAC_STEPSIZE_19
#define CONF_DMAC_STEPSIZE_19 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_19
#ifndef CONF_DMAC_STEPSEL_19
#define CONF_DMAC_STEPSEL_19 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_19
#ifndef CONF_DMAC_SRCINC_19
#define CONF_DMAC_SRCINC_19 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_19
#ifndef CONF_DMAC_DSTINC_19
#define CONF_DMAC_DSTINC_19 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_19
#ifndef CONF_DMAC_BEATSIZE_19
#define CONF_DMAC_BEATSIZE_19 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_19
#ifndef CONF_DMAC_BLOCKACT_19
#define CONF_DMAC_BLOCKACT_19 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_19
#ifndef CONF_DMAC_EVOSEL_19
#define CONF_DMAC_EVOSEL_19 0
#endif
// </e>

// <e> Channel 20 settings
// <id> dmac_channel_20_settings
#ifndef CONF_DMAC_CHANNEL_20_SETTINGS
#define CONF_DMAC_CHANNEL_20_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 20 is running in standby mode or not
// <id> dmac_runstdby_20
#ifndef CONF_DMAC_RUNSTDBY_20
#define CONF_DMAC_RUNSTDBY_20 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_20
#ifndef CONF_DMAC_TRIGACT_20
#define CONF_DMAC_TRIGACT_20 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_20
#ifndef CONF_DMAC_TRIGSRC_20
#define CONF_DMAC_TRIGSRC_20 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_20
#ifndef CONF_DMAC_LVL_20
#define CONF_DMAC_LVL_20 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_20
#ifndef CONF_DMAC_EVOE_20
#define CONF_DMAC_EVOE_20 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_20
#ifndef CONF_DMAC_EVIE_20
#define CONF_DMAC_EVIE_20 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_20
#ifndef CONF_DMAC_EVACT_20
#define CONF_DMAC_EVACT_20 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_20
#ifndef CONF_DMAC_STEPSIZE_20
#define CONF_DMAC_STEPSIZE_20 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_20
#ifndef CONF_DMAC_STEPSEL_20
#define CONF_DMAC_STEPSEL_20 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_20
#ifndef CONF_DMAC_SRCINC_20
#define CONF_DMAC_SRCINC_20 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_20
#ifndef CONF_DMAC_DSTINC_20
#define CONF_DMAC_DSTINC_20 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_20
#ifndef CONF_DMAC_BEATSIZE_20
#define CONF_DMAC_BEATSIZE_20 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_20
#ifndef CONF_DMAC_BLOCKACT_20
#define CONF_DMAC_BLOCKACT_20 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_20
#ifndef CONF_DMAC_EVOSEL_20
#define CONF_DMAC_EVOSEL_20 0
#endif
// </e>

// <e> Channel 21 settings
// <id> dmac_channel_21_settings
#ifndef CONF_DMAC_CHANNEL_21_SETTINGS
#define CONF_DMAC_CHANNEL_21_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 21 is running in standby mode or not
// <id> dmac_runstdby_21
#ifndef CONF_DMAC_RUNSTDBY_21
#define CONF_DMAC_RUNSTDBY_21 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_21
#ifndef CONF_DMAC_TRIGACT_21
#define CONF_DMAC_TRIGACT_21 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_21
#ifndef CONF_DMAC_TRIGSRC_21
#define CONF_DMAC_TRIGSRC_21 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_21
#ifndef CONF_DMAC_LVL_21
#define CONF_DMAC_LVL_21 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_21
#ifndef CONF_DMAC_EVOE_21
#define CONF_DMAC_EVOE_21 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_21
#ifndef CONF_DMAC_EVIE_21
#define CONF_DMAC_EVIE_21 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_21
#ifndef CONF_DMAC_EVACT_21
#define CONF_DMAC_EVACT_21 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_21
#ifndef CONF_DMAC_STEPSIZE_21
#define CONF_DMAC_STEPSIZE_21 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_21
#ifndef CONF_DMAC_STEPSEL_21
#define CONF_DMAC_STEPSEL_21 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_21
#ifndef CONF_DMAC_SRCINC_21
#define CONF_DMAC_SRCINC_21 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_21
#ifndef CONF_DMAC_DSTINC_21
#define CONF_DMAC_DSTINC_21 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_21
#ifndef CONF_DMAC_BEATSIZE_21
#define CONF_DMAC_BEATSIZE_21 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_21
#ifndef CONF_DMAC_BLOCKACT_21
#define CONF_DMAC_BLOCKACT_21 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_21
#ifndef CONF_DMAC_EVOSEL_21
#define CONF_DMAC_EVOSEL_21 0
#endif
// </e>

// <e> Channel 22 settings
// <id> dmac_channel_22_settings
#ifndef CONF_DMAC_CHANNEL_22_SETTINGS
#define CONF_DMAC_CHANNEL_22_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 22 is running in standby mode or not
// <id> dmac_runstdby_22
#ifndef CONF_DMAC_RUNSTDBY_22
#define CONF_DMAC_RUNSTDBY_22 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_22
#ifndef CONF_DMAC_TRIGACT_22
#define CONF_DMAC_TRIGACT_22 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_22
#ifndef CONF_DMAC_TRIGSRC_22
#define CONF_DMAC_TRIGSRC_22 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_22
#ifndef CONF_DMAC_LVL_22
#define CONF_DMAC_LVL_22 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_22
#ifndef CONF_DMAC_EVOE_22
#define CONF_DMAC_EVOE_22 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_22
#ifndef CONF_DMAC_EVIE_22
#define CONF_DMAC_EVIE_22 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_22
#ifndef CONF_DMAC_EVACT_22
#define CONF_DMAC_EVACT_22 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_22
#ifndef CONF_DMAC_STEPSIZE_22
#define CONF_DMAC_STEPSIZE_22 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_22
#ifndef CONF_DMAC_STEPSEL_22
#define CONF_DMAC_STEPSEL_22 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_22
#ifndef CONF_DMAC_SRCINC_22
#define CONF_DMAC_SRCINC_22 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_22
#ifndef CONF_DMAC_DSTINC_22
#define CONF_DMAC_DSTINC_22 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_22
#ifndef CONF_DMAC_BEATSIZE_22
#define CONF_DMAC_BEATSIZE_22 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_22
#ifndef CONF_DMAC_BLOCKACT_22
#define CONF_DMAC_BLOCKACT_22 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_22
#ifndef CONF_DMAC_EVOSEL_22
#define CONF_DMAC_EVOSEL_22 0
#endif
// </e>

// <e> Channel 23 settings
// <id> dmac_channel_23_settings
#ifndef CONF_DMAC_CHANNEL_23_SETTINGS
#define CONF_DMAC_CHANNEL_23_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 23 is running in standby mode or not
// <id> dmac_runstdby_23
#ifndef CONF_DMAC_RUNSTDBY_23
#define CONF_DMAC_RUNSTDBY_23 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_23
#ifndef CONF_DMAC_TRIGACT_23
#define CONF_DMAC_TRIGACT_23 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_23
#ifndef CONF_DMAC_TRIGSRC_23
#define CONF_DMAC_TRIGSRC_23 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_23
#ifndef CONF_DMAC_LVL_23
#define CONF_DMAC_LVL_23 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_23
#ifndef CONF_DMAC_EVOE_23
#define CONF_DMAC_EVOE_23 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_23
#ifndef CONF_DMAC_EVIE_23
#define CONF_DMAC_EVIE_23 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_23
#ifndef CONF_DMAC_EVACT_23
#define CONF_DMAC_EVACT_23 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_23
#ifndef CONF_DMAC_STEPSIZE_23
#define CONF_DMAC_STEPSIZE_23 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_23
#ifndef CONF_DMAC_STEPSEL_23
#define CONF_DMAC_STEPSEL_23 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_23
#ifndef CONF_DMAC_SRCINC_23
#define CONF_DMAC_SRCINC_23 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_23
#ifndef CONF_DMAC_DSTINC_23
#define CONF_DMAC_DSTINC_23 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_23
#ifndef CONF_DMAC_BEATSIZE_23
#define CONF_DMAC_BEATSIZE_23 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_23
#ifndef CONF_DMAC_BLOCKACT_23
#define CONF_DMAC_BLOCKACT_23 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_23
#ifndef CONF_DMAC_EVOSEL_23
#define CONF_DMAC_EVOSEL_23 0
#endif
// </e>

// <e> Channel 24 settings
// <id> dmac_channel_24_settings
#ifndef CONF_DMAC_CHANNEL_24_SETTINGS
#define CONF_DMAC_CHANNEL_24_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 24 is running in standby mode or not
// <id> dmac_runstdby_24
#ifndef CONF_DMAC_RUNSTDBY_24
#define CONF_DMAC_RUNSTDBY_24 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_24
#ifndef CONF_DMAC_TRIGACT_24
#define CONF_DMAC_TRIGACT_24 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_24
#ifndef CONF_DMAC_TRIGSRC_24
#define CONF_DMAC_TRIGSRC_24 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_24
#ifndef CONF_DMAC_LVL_24
#define CONF_DMAC_LVL_24 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_24
#ifndef CONF_DMAC_EVOE_24
#define CONF_DMAC_EVOE_24 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_24
#ifndef CONF_DMAC_EVIE_24
#define CONF_DMAC_EVIE_24 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_24
#ifndef CONF_DMAC_EVACT_24
#define CONF_DMAC_EVACT_24 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_24
#ifndef CONF_DMAC_STEPSIZE_24
#define CONF_DMAC_STEPSIZE_24 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_24
#ifndef CONF_DMAC_STEPSEL_24
#define CONF_DMAC_STEPSEL_24 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_24
#ifndef CONF_DMAC_SRCINC_24
#define CONF_DMAC_SRCINC_24 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_24
#ifndef CONF_DMAC_DSTINC_24
#define CONF_DMAC_DSTINC_24 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_24
#ifndef CONF_DMAC_BEATSIZE_24
#define CONF_DMAC_BEATSIZE_24 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_24
#ifndef CONF_DMAC_BLOCKACT_24
#define CONF_DMAC_BLOCKACT_24 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_24
#ifndef CONF_DMAC_EVOSEL_24
#define CONF_DMAC_EVOSEL_24 0
#endif
// </e>

// <e> Channel 25 settings
// <id> dmac_channel_25_settings
#ifndef CONF_DMAC_CHANNEL_25_SETTINGS
#define CONF_DMAC_CHANNEL_25_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 25 is running in standby mode or not
// <id> dmac_runstdby_25
#ifndef CONF_DMAC_RUNSTDBY_25
#define CONF_DMAC_RUNSTDBY_25 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_25
#ifndef CONF_DMAC_TRIGACT_25
#define CONF_DMAC_TRIGACT_25 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_25
#ifndef CONF_DMAC_TRIGSRC_25
#define CONF_DMAC_TRIGSRC_25 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_25
#ifndef CONF_DMAC_LVL_25
#define CONF_DMAC_LVL_25 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_25
#ifndef CONF_DMAC_EVOE_25
#define CONF_DMAC_EVOE_25 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_25
#ifndef CONF_DMAC_EVIE_25
#define CONF_DMAC_EVIE_25 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_25
#ifndef CONF_DMAC_EVACT_25
#define CONF_DMAC_EVACT_25 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_25
#ifndef CONF_DMAC_STEPSIZE_25
#define CONF_DMAC_STEPSIZE_25 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_25
#ifndef CONF_DMAC_STEPSEL_25
#define CONF_DMAC_STEPSEL_25 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_25
#ifndef CONF_DMAC_SRCINC_25
#define CONF_DMAC_SRCINC_25 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_25
#ifndef CONF_DMAC_DSTINC_25
#define CONF_DMAC_DSTINC_25 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_25
#ifndef CONF_DMAC_BEATSIZE_25
#define CONF_DMAC_BEATSIZE_25 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_25
#ifndef CONF_DMAC_BLOCKACT_25
#define CONF_DMAC_BLOCKACT_25 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_25
#ifndef CONF_DMAC_EVOSEL_25
#define CONF_DMAC_EVOSEL_25 0
#endif
// </e>

// <e> Channel 26 settings
// <id> dmac_channel_26_settings
#ifndef CONF_DMAC_CHANNEL_26_SETTINGS
#define CONF_DMAC_CHANNEL_26_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 26 is running in standby mode or not
// <id> dmac_runstdby_26
#ifndef CONF_DMAC_RUNSTDBY_26
#define CONF_DMAC_RUNSTDBY_26 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_26
#ifndef CONF_DMAC_TRIGACT_26
#define CONF_DMAC_TRIGACT_26 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_26
#ifndef CONF_DMAC_TRIGSRC_26
#define CONF_DMAC_TRIGSRC_26 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_26
#ifndef CONF_DMAC_LVL_26
#define CONF_DMAC_LVL_26 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_26
#ifndef CONF_DMAC_EVOE_26
#define CONF_DMAC_EVOE_26 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_26
#ifndef CONF_DMAC_EVIE_26
#define CONF_DMAC_EVIE_26 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_26
#ifndef CONF_DMAC_EVACT_26
#define CONF_DMAC_EVACT_26 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_26
#ifndef CONF_DMAC_STEPSIZE_26
#define CONF_DMAC_STEPSIZE_26 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_26
#ifndef CONF_DMAC_STEPSEL_26
#define CONF_DMAC_STEPSEL_26 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_26
#ifndef CONF_DMAC_SRCINC_26
#define CONF_DMAC_SRCINC_26 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_26
#ifndef CONF_DMAC_DSTINC_26
#define CONF_DMAC_DSTINC_26 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_26
#ifndef CONF_DMAC_BEATSIZE_26
#define CONF_DMAC_BEATSIZE_26 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_26
#ifndef CONF_DMAC_BLOCKACT_26
#define CONF_DMAC_BLOCKACT_26 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_26
#ifndef CONF_DMAC_EVOSEL_26
#define CONF_DMAC_EVOSEL_26 0
#endif
// </e>

// <e> Channel 27 settings
// <id> dmac_channel_27_settings
#ifndef CONF_DMAC_CHANNEL_27_SETTINGS
#define CONF_DMAC_CHANNEL_27_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 27 is running in standby mode or not
// <id> dmac_runstdby_27
#ifndef CONF_DMAC_RUNSTDBY_27
#define CONF_DMAC_RUNSTDBY_27 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_27
#ifndef CONF_DMAC_TRIGACT_27
#define CONF_DMAC_TRIGACT_27 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_27
#ifndef CONF_DMAC_TRIGSRC_27
#define CONF_DMAC_TRIGSRC_27 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_27
#ifndef CONF_DMAC_LVL_27
#define CONF_DMAC_LVL_27 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_27
#ifndef CONF_DMAC_EVOE_27
#define CONF_DMAC_EVOE_27 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_27
#ifndef CONF_DMAC_EVIE_27
#define CONF_DMAC_EVIE_27 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_27
#ifndef CONF_DMAC_EVACT_27
#define CONF_DMAC_EVACT_27 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_27
#ifndef CONF_DMAC_STEPSIZE_27
#define CONF_DMAC_STEPSIZE_27 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_27
#ifndef CONF_DMAC_STEPSEL_27
#define CONF_DMAC_STEPSEL_27 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_27
#ifndef CONF_DMAC_SRCINC_27
#define CONF_DMAC_SRCINC_27 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_27
#ifndef CONF_DMAC_DSTINC_27
#define CONF_DMAC_DSTINC_27 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_27
#ifndef CONF_DMAC_BEATSIZE_27
#define CONF_DMAC_BEATSIZE_27 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_27
#ifndef CONF_DMAC_BLOCKACT_27
#define CONF_DMAC_BLOCKACT_27 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_27
#ifndef CONF_DMAC_EVOSEL_27
#define CONF_DMAC_EVOSEL_27 0
#endif
// </e>

// <e> Channel 28 settings
// <id> dmac_channel_28_settings
#ifndef CONF_DMAC_CHANNEL_28_SETTINGS
#define CONF_DMAC_CHANNEL_28_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 28 is running in standby mode or not
// <id> dmac_runstdby_28
#ifndef CONF_DMAC_RUNSTDBY_28
#define CONF_DMAC_RUNSTDBY_28 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_28
#ifndef CONF_DMAC_TRIGACT_28
#define CONF_DMAC_TRIGACT_28 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_28
#ifndef CONF_DMAC_TRIGSRC_28
#define CONF_DMAC_TRIGSRC_28 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_28
#ifndef CONF_DMAC_LVL_28
#define CONF_DMAC_LVL_28 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_28
#ifndef CONF_DMAC_EVOE_28
#define CONF_DMAC_EVOE_28 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_28
#ifndef CONF_DMAC_EVIE_28
#define CONF_DMAC_EVIE_28 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_28
#ifndef CONF_DMAC_EVACT_28
#define CONF_DMAC_EVACT_28 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_28
#ifndef CONF_DMAC_STEPSIZE_28
#define CONF_DMAC_STEPSIZE_28 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_28
#ifndef CONF_DMAC_STEPSEL_28
#define CONF_DMAC_STEPSEL_28 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_28
#ifndef CONF_DMAC_SRCINC_28
#define CONF_DMAC_SRCINC_28 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_28
#ifndef CONF_DMAC_DSTINC_28
#define CONF_DMAC_DSTINC_28 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_28
#ifndef CONF_DMAC_BEATSIZE_28
#define CONF_DMAC_BEATSIZE_28 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_28
#ifndef CONF_DMAC_BLOCKACT_28
#define CONF_DMAC_BLOCKACT_28 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_28
#ifndef CONF_DMAC_EVOSEL_28
#define CONF_DMAC_EVOSEL_28 0
#endif
// </e>

// <e> Channel 29 settings
// <id> dmac_channel_29_settings
#ifndef CONF_DMAC_CHANNEL_29_SETTINGS
#define CONF_DMAC_CHANNEL_29_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 29 is running in standby mode or not
// <id> dmac_runstdby_29
#ifndef CONF_DMAC_RUNSTDBY_29
#define CONF_DMAC_RUNSTDBY_29 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_29
#ifndef CONF_DMAC_TRIGACT_29
#define CONF_DMAC_TRIGACT_29 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_29
#ifndef CONF_DMAC_TRIGSRC_29
#define CONF_DMAC_TRIGSRC_29 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_29
#ifndef CONF_DMAC_LVL_29
#define CONF_DMAC_LVL_29 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_29
#ifndef CONF_DMAC_EVOE_29
#define CONF_DMAC_EVOE_29 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_29
#ifndef CONF_DMAC_EVIE_29
#define CONF_DMAC_EVIE_29 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_29
#ifndef CONF_DMAC_EVACT_29
#define CONF_DMAC_EVACT_29 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_29
#ifndef CONF_DMAC_STEPSIZE_29
#define CONF_DMAC_STEPSIZE_29 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_29
#ifndef CONF_DMAC_STEPSEL_29
#define CONF_DMAC_STEPSEL_29 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_29
#ifndef CONF_DMAC_SRCINC_29
#define CONF_DMAC_SRCINC_29 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_29
#ifndef CONF_DMAC_DSTINC_29
#define CONF_DMAC_DSTINC_29 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_29
#ifndef CONF_DMAC_BEATSIZE_29
#define CONF_DMAC_BEATSIZE_29 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_29
#ifndef CONF_DMAC_BLOCKACT_29
#define CONF_DMAC_BLOCKACT_29 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_29
#ifndef CONF_DMAC_EVOSEL_29
#define CONF_DMAC_EVOSEL_29 0
#endif
// </e>

// <e> Channel 30 settings
// <id> dmac_channel_30_settings
#ifndef CONF_DMAC_CHANNEL_30_SETTINGS
#define CONF_DMAC_CHANNEL_30_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 30 is running in standby mode or not
// <id> dmac_runstdby_30
#ifndef CONF_DMAC_RUNSTDBY_30
#define CONF_DMAC_RUNSTDBY_30 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_30
#ifndef CONF_DMAC_TRIGACT_30
#define CONF_DMAC_TRIGACT_30 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_30
#ifndef CONF_DMAC_TRIGSRC_30
#define CONF_DMAC_TRIGSRC_30 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_30
#ifndef CONF_DMAC_LVL_30
#define CONF_DMAC_LVL_30 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_30
#ifndef CONF_DMAC_EVOE_30
#define CONF_DMAC_EVOE_30 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_30
#ifndef CONF_DMAC_EVIE_30
#define CONF_DMAC_EVIE_30 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_30
#ifndef CONF_DMAC_EVACT_30
#define CONF_DMAC_EVACT_30 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_30
#ifndef CONF_DMAC_STEPSIZE_30
#define CONF_DMAC_STEPSIZE_30 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_30
#ifndef CONF_DMAC_STEPSEL_30
#define CONF_DMAC_STEPSEL_30 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_30
#ifndef CONF_DMAC_SRCINC_30
#define CONF_DMAC_SRCINC_30 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_30
#ifndef CONF_DMAC_DSTINC_30
#define CONF_DMAC_DSTINC_30 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_30
#ifndef CONF_DMAC_BEATSIZE_30
#define CONF_DMAC_BEATSIZE_30 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_30
#ifndef CONF_DMAC_BLOCKACT_30
#define CONF_DMAC_BLOCKACT_30 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_30
#ifndef CONF_DMAC_EVOSEL_30
#define CONF_DMAC_EVOSEL_30 0
#endif
// </e>

// <e> Channel 31 settings
// <id> dmac_channel_31_settings
#ifndef CONF_DMAC_CHANNEL_31_SETTINGS
#define CONF_DMAC_CHANNEL_31_SETTINGS 0
#endif

// <q> Channel Run in Standby
// <i> Indicates whether channel 31 is running in standby mode or not
// <id> dmac_runstdby_31
#ifndef CONF_DMAC_RUNSTDBY_31
#define CONF_DMAC_RUNSTDBY_31 0
#endif

// <o> Trigger action
// <0=> One trigger required for each block transfer
// <2=> One trigger required for each beat transfer
// <3=> One trigger required for each transaction
// <i> Defines the trigger action used for a transfer
// <id> dmac_trigact_31
#ifndef CONF_DMAC_TRIGACT_31
#define CONF_DMAC_TRIGACT_31 0
#endif

// <o> Trigger source
// <0x00=> Only software/event triggers
// <0x01=> RTC Time Stamp Trigger
// <0x02=> DSU Debug Communication Channel 0 Trigger
// <0x03=> DSU Debug Communication Channel 1 Trigger
// <0x04=> SERCOM0 RX Trigger
// <0x05=> SERCOM0 TX Trigger
// <0x06=> SERCOM1 RX Trigger
// <0x07=> SERCOM1 TX Trigger
// <0x08=> SERCOM2 RX Trigger
// <0x09=> SERCOM2 TX Trigger
// <0x0A=> SERCOM3 RX Trigger
// <0x0B=> SERCOM3 TX Trigger
// <0x0C=> SERCOM4 RX Trigger
// <0x0D=> SERCOM4 TX Trigger
// <0x0E=> SERCOM5 RX Trigger
// <0x0F=> SERCOM5 TX Trigger
// <0x10=> SERCOM6 RX Trigger
// <0x11=> SERCOM6 TX Trigger
// <0x12=> SERCOM7 RX Trigger
// <0x13=> SERCOM7 TX Trigger
// <0x14=> CAN0 DEBUG Trigger
// <0x15=> CAN1 DEBUG Trigger
// <0x16=> TCC0 Overflow Trigger Trigger
// <0x17=> TCC0 Match/Compare 0 Trigger Trigger
// <0x18=> TCC0 Match/Compare 1 Trigger Trigger
// <0x19=> TCC0 Match/Compare 2 Trigger Trigger
// <0x1A=> TCC0 Match/Compare 3 Trigger Trigger
// <0x1B=> TCC0 Match/Compare 4 Trigger Trigger
// <0x1C=> TCC0 Match/Compare 5 Trigger Trigger
// <0x1D=> TCC1 Overflow Trigger Trigger
// <0x1E=> TCC1 Match/Compare 0 Trigger Trigger
// <0x1F=> TCC1 Match/Compare 1 Trigger Trigger
// <0x20=> TCC1 Match/Compare 2 Trigger Trigger
// <0x21=> TCC1 Match/Compare 3 Trigger Trigger
// <0x22=> TCC2 Overflow Trigger Trigger
// <0x23=> TCC2 Match/Compare 0 Trigger Trigger
// <0x24=> TCC2 Match/Compare 1 Trigger Trigger
// <0x25=> TCC2 Match/Compare 2 Trigger Trigger
// <0x26=> TCC3 Overflow Trigger Trigger
// <0x27=> TCC3 Match/Compare 0 Trigger Trigger
// <0x28=> TCC3 Match/Compare 1 Trigger Trigger
// <0x29=> TCC4 Overflow Trigger Trigger
// <0x2A=> TCC4 Match/Compare 0 Trigger Trigger
// <0x2B=> TCC4 Match/Compare 1 Trigger Trigger
// <0x2C=> TC0 Overflow Trigger
// <0x2D=> TC0 Match/Compare 0 Trigger
// <0x2E=> TC0 Match/Compare 1 Trigger
// <0x2F=> TC1 Overflow Trigger
// <0x30=> TC1 Match/Compare 0 Trigger
// <0x31=> TC1 Match/Compare 1 Trigger
// <0x32=> TC2 Overflow Trigger
// <0x33=> TC2 Match/Compare 0 Trigger
// <0x34=> TC2 Match/Compare 1 Trigger
// <0x35=> TC3 Overflow Trigger
// <0x36=> TC3 Match/Compare 0 Trigger
// <0x37=> TC3 Match/Compare 1 Trigger
// <0x38=> TC4 Overflow Trigger
// <0x39=> TC4 Match/Compare 0 Trigger
// <0x3A=> TC4 Match/Compare 1 Trigger
// <0x3B=> TC5 Overflow Trigger
// <0x3C=> TC5 Match/Compare 0 Trigger
// <0x3D=> TC5 Match/Compare 1 Trigger
// <0x3E=> TC6 Overflow Trigger
// <0x3F=> TC6 Match/Compare 0 Trigger
// <0x40=> TC6 Match/Compare 1 Trigger
// <0x41=> TC7 Overflow Trigger
// <0x42=> TC7 Match/Compare 0 Trigger
// <0x43=> TC7 Match/Compare 1 Trigger
// <0x44=> ADC0 Result Ready Trigger
// <0x45=> ADC0 Sequencing Trigger
// <0x46=> ADC1 Result Ready Trigger
// <0x47=> ADC1 Sequencing Trigger
// <0x48=> DAC Empty 0 Trigger
// <0x49=> DAC Empty 1 Trigger
// <0x4A=> DAC Result Ready 0 Trigger
// <0x4B=> DAC Result Ready 1 Trigger
// <0x4C=> I2S Rx 0 Trigger
// <0x4D=> I2S Rx 1 Trigger
// <0x4E=> I2S Tx 0 Trigger
// <0x4F=> I2S Tx 1 Trigger
// <0x50=> PCC RX Trigger
// <0x51=> AES Write Trigger
// <0x52=> AES Read Trigger
// <0x53=> QSPI Rx Trigger
// <0x54=> QSPI Tx Trigger
// <i> Defines the peripheral trigger which is source of the transfer
// <id> dmac_trifsrc_31
#ifndef CONF_DMAC_TRIGSRC_31
#define CONF_DMAC_TRIGSRC_31 0
#endif

// <o> Channel Arbitration Level
// <0=> Channel priority 0
// <1=> Channel priority 1
// <2=> Channel priority 2
// <3=> Channel priority 3
// <i> Defines the arbitration level for this channel
// <id> dmac_lvl_31
#ifndef CONF_DMAC_LVL_31
#define CONF_DMAC_LVL_31 0
#endif

// <q> Channel Event Output
// <i> Indicates whether channel event generation is enabled or not
// <id> dmac_evoe_31
#ifndef CONF_DMAC_EVOE_31
#define CONF_DMAC_EVOE_31 0
#endif

// <q> Channel Event Input
// <i> Indicates whether channel event reception is enabled or not
// <id> dmac_evie_31
#ifndef CONF_DMAC_EVIE_31
#define CONF_DMAC_EVIE_31 0
#endif

// <o> Event Input Action
// <0=> No action
// <1=> Normal transfer and conditional transfer on strobe trigger
// <2=> Conditional transfer trigger
// <3=> Conditional block transfer
// <4=> Channel suspend operation
// <5=> Channel resume operation
// <6=> Skip next block suspend action
// <i> Defines the event input action
// <id> dmac_evact_31
#ifndef CONF_DMAC_EVACT_31
#define CONF_DMAC_EVACT_31 0
#endif

// <o> Address Increment Step Size
// <0=> Next ADDR = ADDR + (BEATSIZE + 1) * 1
// <1=> Next ADDR = ADDR + (BEATSIZE + 1) * 2
// <2=> Next ADDR = ADDR + (BEATSIZE + 1) * 4
// <3=> Next ADDR = ADDR + (BEATSIZE + 1) * 8
// <4=> Next ADDR = ADDR + (BEATSIZE + 1) * 16
// <5=> Next ADDR = ADDR + (BEATSIZE + 1) * 32
// <6=> Next ADDR = ADDR + (BEATSIZE + 1) * 64
// <7=> Next ADDR = ADDR + (BEATSIZE + 1) * 128
// <i> Defines the address increment step size, applies to source or destination address
// <id> dmac_stepsize_31
#ifndef CONF_DMAC_STEPSIZE_31
#define CONF_DMAC_STEPSIZE_31 0
#endif

// <o> Step Selection
// <0=> Step size settings apply to the destination address
// <1=> Step size settings apply to the source address
// <i> Defines whether source or destination addresses are using the step size settings
// <id> dmac_stepsel_31
#ifndef CONF_DMAC_STEPSEL_31
#define CONF_DMAC_STEPSEL_31 0
#endif

// <q> Source Address Increment
// <i> Indicates whether the source address incrementation is enabled or not
// <id> dmac_srcinc_31
#ifndef CONF_DMAC_SRCINC_31
#define CONF_DMAC_SRCINC_31 0
#endif

// <q> Destination Address Increment
// <i> Indicates whether the destination address incrementation is enabled or not
// <id> dmac_dstinc_31
#ifndef CONF_DMAC_DSTINC_31
#define CONF_DMAC_DSTINC_31 0
#endif

// <o> Beat Size
// <0=> 8-bit bus transfer
// <1=> 16-bit bus transfer
// <2=> 32-bit bus transfer
// <i> Defines the size of one beat
// <id> dmac_beatsize_31
#ifndef CONF_DMAC_BEATSIZE_31
#define CONF_DMAC_BEATSIZE_31 0
#endif

// <o> Block Action
// <0=> Channel will be disabled if it is the last block transfer in the transaction
// <1=> Channel will be disabled if it is the last block transfer in the transaction and block interrupt
// <2=> Channel suspend operation is complete
// <3=> Both channel suspend operation and block interrupt
// <i> Defines the the DMAC should take after a block transfer has completed
// <id> dmac_blockact_31
#ifndef CONF_DMAC_BLOCKACT_31
#define CONF_DMAC_BLOCKACT_31 0
#endif

// <o> Event Output Selection
// <0=> Event generation disabled
// <1=> Event strobe when block transfer complete
// <3=> Event strobe when beat transfer complete
// <i> Defines the event output selection
// <id> dmac_evosel_31
#ifndef CONF_DMAC_EVOSEL_31
#define CONF_DMAC_EVOSEL_31 0
#endif
// </e>

// </e>

// <<< end of configuration section >>>

#endif // HPL_DMAC_CONFIG_H
