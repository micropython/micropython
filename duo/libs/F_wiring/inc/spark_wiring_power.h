/**
 ******************************************************************************
 * @file    spark_wiring_power.h
 * @author  Mohit Bhoite
 * @version V1.0.0
 * @date    11-August-2015
 * @brief   Header for spark_wiring_power.cpp module
 ******************************************************************************
  Copyright (c) 2013-2015 Particle Industries, Inc.  All rights reserved.
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */

#include "spark_wiring.h"
#include "spark_wiring_i2c.h"
#include "spark_wiring_platform.h"

#ifndef __SPARK_WIRING_POWER_H
#define __SPARK_WIRING_POWER_H

//Default PMIC (BQ24195) I2C address
#define PMIC_ADDRESS							0x6B

// Register address definitions
#define INPUT_SOURCE_REGISTER					0x00
#define POWERON_CONFIG_REGISTER					0x01
#define CHARGE_CURRENT_CONTROL_REGISTER			0x02
#define PRECHARGE_CURRENT_CONTROL_REGISTER		0x03
#define CHARGE_VOLTAGE_CONTROL_REGISTER			0x04
#define CHARGE_TIMER_CONTROL_REGISTER			0x05
#define THERMAL_REG_CONTROL_REGISTER			0x06
#define MISC_CONTROL_REGISTER					0x07
#define SYSTEM_STATUS_REGISTER					0x08
#define FAULT_REGISTER							0x09
#define PMIC_VERSION_REGISTER					0x0A



class PMIC {

	public:
	    PMIC();
		bool begin();
		byte getVersion();
		byte getSystemStatus();
		byte getFault();

		// Input source control register
		byte readInputSourceRegister(void);
		bool enableBuck(void);
		bool disableBuck(void);
		bool setInputCurrentLimit(uint16_t current);
		byte getInputCurrentLimit(void);
		bool setInputVoltageLimit(uint16_t voltage);
		byte getInputVoltageLimit(void);

		// Power ON configuration register
		bool enableCharging(void);
		bool disableCharging(void);
		bool enableOTG(void);
		bool disableOTG(void);
		bool resetWatchdog(void);
		bool setMinimumSystemVoltage(uint16_t voltage);
		uint16_t getMinimumSystemVoltage();
		byte readPowerONRegister(void);

		// Charge current control register
		bool setChargeCurrent(bool bit7, bool bit6, bool bit5, bool bit4, bool bit3, bool bit2);
		byte getChargeCurrent(void);

		//PreCharge/ Termination Current Control Register
		bool setPreChargeCurrent();
		byte getPreChargeCurrent();
		bool setTermChargeCurrent();
		byte getTermChargeCurrent();

		//Charge Voltage Control Register
		bool setChargeVoltage(uint16_t voltage);
		byte getChargeVoltage();

		//CHARGE_TIMER_CONTROL_REGISTER
		byte readChargeTermRegister();
		bool disableWatchdog(void);
		bool setWatchdog(byte time);



		//Thermal Regulation Control Register
		bool setThermalRegulation();
		byte getThermalRegulation();

		//Misc Operation Control Register
		byte readOpControlRegister();
		bool enableDPDM(void);
		bool disableDPDM(void);
		bool enableBATFET(void);
		bool disableBATFET(void);
		bool safetyTimer(); //slow/ normal

		bool enableChargeFaultINT();
		bool disableChargeFaultINT();
		bool enableBatFaultINT();
		bool disableBatFaultINT();

		//System Status Register
		byte getVbusStat();
		byte getChargingStat();
		bool getDPMStat();
		bool isPowerGood(void);
		bool isHot(void);
		bool getVsysStat();

		//Fault Register
		bool isWatchdogFault();
		byte getChargeFault();
		bool isBatFault();
		byte getNTCFault();





	private:

		byte readRegister(byte startAddress);
		void writeRegister(byte address, byte DATA);
};

#endif /* __SPARK_WIRING_POWER_H */

/*
// List of Registers

//-----------------------------------------------------------------------------
// Input source control register
//-----------------------------------------------------------------------------

REG00
BIT
7 : 0:Enable Buck regulator 1:disable buck regulator (powered only from a LiPo)
--- input volatge limit. this is used to determine if USB source is overloaded
6 : VINDPM[3] 640mV | offset is 3.88V, Range is 3.88 to 5.08
5 : VINDPM[2] 320mV	| enabling bits 3 to 6 adds the volatges to 3.88 base value
4 : VINDPM[1] 160mV	| Default is 4.36 (0110)
3 : VINDPM[0] 80mV	|
--- input current limit
2 : INLIM[2]  000: 100mA, 001: 150mA, 010: 500mA,	| Default: 100mA when OTG pin is LOW and
1 : INLIM[1]  011: 900mA, 100: 1.2A,   101: 1.5A 	| 500mA when OTG pin is HIGH
0 : INLIM[0]  110: 2.0A,  111: 3.0A   				| when charging port detected, 1.5A


//-----------------------------------------------------------------------------
// Power ON configuration register
//-----------------------------------------------------------------------------

REG01
BIT
7: 0: keep current settings 1:reset register to default
6: 0: normal 1: reset I2C watchdog timer
--- charger config
5: CHG_CONFIG[1]	| 00: Disable charging, 01: Charge battery(default)
4: CHG_CONFIG[0]	| 10/11: OTG
--- minimum system voltage limit. this is the minimum output feeding the gsm module
3: SYS_MIN[2] 0.4V	| offset is 3.0V, Range 3.0V to 3.7V
2: SYS_MIN[1] 0.2V	| enabling bits 1 to 3 adds the volatges to 3.0 base value
1: SYS_MIN[0] 0.1V 	| default is 3.5V (101)
0: Reserved

//-----------------------------------------------------------------------------
// Charge current control register
//-----------------------------------------------------------------------------

REG02
BIT
7: ICHG[5] 2048mA	| offset is 512mA
6: ICHG[4] 1024mA	| Range: 512 to 4544mA (BQ24195)
5: ICHG[3] 512mA	| Range: 512 to 2496mA (BQ24195L)
4: ICHG[2] 256mA	| Default: 2048mA (011000) = 512mA+1024mA+512mA
3: ICHG[1] 128mA	| enabling bits 2 to 7 adds the current to 512mA base value
2: ICHG[0] 64mA		|
1: Reserved (should always be 0)
0: FORCE_20PCT (fill this description)

//-----------------------------------------------------------------------------
//PreCharge/ Termination Current Control Register
//-----------------------------------------------------------------------------

REG03
BIT
--- precharge current limit
7: IPRECHG[3]	1024mA	| offset is 128mA
6: IPRECHG[2] 	512mA	| Range: 128 mA to 2048 mA
5: IPRECHG[1] 	256mA	| Default: 256 mA (0001) = 128mA+128mA
4: IPRECHG[0] 	128mA	| enabling bits 4 to 7, adds the current to 128mA base value
--- termination current limit
3: ITERM[3]		1024mA	| offset is 128mA
2: ITERM[2]		512mA	| Range: 128 mA to 2048 mA
1: ITERM[1]		256mA	| Default: 256 mA (0001) = 128mA+128mA
0: ITERM[0] 	128mA	| enabling bits 0 to 3, adds the current to 128mA base value

//-----------------------------------------------------------------------------
//Charge Voltage Control Register
//-----------------------------------------------------------------------------

REG04
BIT
--- Charge Voltage Limit
7: VREG[5]	512mV	| offset os 3.504V
6: VREG[4]	256mV	| Range: 3.504V to 4.400V
5: VREG[3] 	128mV	| Default: 4.208V (101100) = 3.504V+512mV+128mV+64mV
4: VREG[2]	64mV 	| enabling bits 2 to 7, adds the voltage to 3.504V base value
3: VREG[1] 	32mV 	|
2: VREG[0]	16mV 	|
--- Battery Precharge to Fast Charge Threshold
1: BATLOWV 	0:2.8V, 1:3.0V Default:3.0V(1)
--- Battery Recharge Threshold (below battery regulation voltage)
0: VRECHG 	0:100mV, 1:300mV Default:100mV(0)

//-----------------------------------------------------------------------------
//Charge Termination/Timer Control Register
//-----------------------------------------------------------------------------

REG05
BIT
--- Charging Termination Enable
7: EN_TERM 		0:Disable
				1:Enable
				Default: Enable Termination (1)
--- Termination Indicator Threshold
6: TERM_STAT 	0:Match ITERM,
				1:STAT pin high before actual termination when charge current below 800 mA
				Default Match ITERM (0)
--- I2C Watchdog Timer Setting
5: WATCHDOG[1] 	| 00: disable timer, 01: 40seconds
4: WATCHDOG[0] 	| 10: 80 seconds, 11: 160 seconds. Default: 40s(01)
--- Charging Safety Timer Enable
3: EN_TIMER		0:Disable
				1:Enable
				Default:Enable(1)
--- Fast Charge Timer Setting
2: CHG_TIMER[1]	| 00: 5hrs, 01: 8hrs, 10: 12hrs, 11: 20hrs
1: CHG_TIMER[0]	| Default: 8hrs(01)
0: Reserved

//-----------------------------------------------------------------------------
//Thermal Regulation Control Register
//-----------------------------------------------------------------------------

REG06
BIT
7: Reserved (should always be 0)
6: Reserved (should always be 0)
5: Reserved (should always be 0)
4: Reserved (should always be 0)
3: Reserved (should always be 0)
2: Reserved (should always be 0)
--- Thermal Regulation Threshold
1: TREG[1]	| 00: 60°C, 01: 80°C, 10:100°C, 11: 120°C
0: TREG[0]	| Default: 120°C (11)

//-----------------------------------------------------------------------------
//Misc Operation Control Register
//-----------------------------------------------------------------------------

REG07
BIT
--- Force DPDM detection
7: DPDM_EN 	0: Not in D+/D– detection,
			1: Force D+/D– detection.
			Default: (0)
--- Safety Timer Setting during Input DPM and Thermal Regulation
6: TMR2X_EN 0: Safety timer not slowed by 2X during input DPM or thermal regulation
			1: Safety timer slowed by 2X during input DPM or thermal regulation
			Default: (1)
--- Force BATFET Off (this essentially disconnects the battery from the system)
5: BATFET_Disable 	0: Allow Q4 turn on
					1: Turn off Q4
					Default: (0)
4: 0 – Reserved. Must write "0"
3: 1 – Reserved. Must write "1"
2: 0 – Reserved. Must write "0"
1: INT_MASK[1]		0: No INT during CHRG_FAULT
					1: INT onCHRG_FAULT
					Default(1)
0: INT_MASK[0]		0: No INT during BAT_FAULT
					1: INT on BAT_FAULT
					Default(1)


//-----------------------------------------------------------------------------
//System Status Register
//-----------------------------------------------------------------------------
//NOTE: This is a read-only register

REG08
BIT
--- VBUS status
7: VBUS_STAT[1]	| 00: Unknown (no input, or DPDM detection incomplete), 01: USB host
6: VBUS_STAT[0]	| 10: Adapter port, 11: OTG
--- Charging status
5: CHRG_STAT[1] | 00: Not Charging,  01: Pre-charge (<VBATLOWV)
4: CHRG_STAT[0] | 10: Fast Charging, 11: Charge termination done
3: DPM_STAT		0: Not DPM
				1: VINDPM or IINDPM
2: PG_STAT		0: Power NO Good :(
				1: Power Good :)
1: THERM_STAT	0: Normal
				1: In Thermal Regulation (HOT)
0: VSYS_STAT	0: Not in VSYSMIN regulation (BAT > VSYSMIN)
				1: In VSYSMIN regulation (BAT < VSYSMIN)

//-----------------------------------------------------------------------------
//Fault Register
//-----------------------------------------------------------------------------
//NOTE: This is a read-only register

REG09
BIT
7: WATCHDOG_FAULT	0: Normal
					1: watchdog timer expired
6: Reserved
--- Charge fault status
5: CHRG_FAULT[1]	| 00: Normal, 01: Input fault (VBUS OVP or VBAT < VBUS < 3.8 V)
4: CHRG_FAULT[0]	| 10: Thermal shutdown, 11: charge safetly timer expiration
--- Battery fault status
3: BAT_FAULT		0: Normal
					1: BATOVP battery over threshold
--- NTC thermistor fault status
2: NTC_FAULT[2]		| 000: Normal
1: NTC_FAULT[1]		| 101: Cold
0: NTC_FAULT[0]		| 110: Hot

//-----------------------------------------------------------------------------
//Vender / Part / Revision Status Register
//-----------------------------------------------------------------------------
//NOTE: This is a read-only register

REG0A  (reset = 00100011, or 0x23)
BIT
7: Reserved
6: Reserved
--- Device configuration
5: PN[2]		1
4: PN[1]		0
3: PN[0]		0
2: TS_PROFILE	0
1: DEV_REG[0]  	1
0: DEV_REG[0]	1

*/
