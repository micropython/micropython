/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _HAL_EFUSE_H_
#define _HAL_EFUSE_H_

_LONG_CALL_ROM_ extern VOID HalEFUSEPowerSwitch8195AROM(IN u8 bWrite, IN u8 PwrState, IN u8 L25OutVoltage);
_LONG_CALL_ extern u32 HALEFUSEOneByteReadROM(IN	u32 CtrlSetting, IN	u16 Addr, OUT u8 *Data, IN u8 L25OutVoltage);
_LONG_CALL_ extern u32 HALEFUSEOneByteWriteROM(IN	u32 CtrlSetting, IN	u16 Addr, IN u8 Data, IN u8 L25OutVoltage);

#define EFUSERead8     HALEFUSEOneByteReadROM
#define EFUSEWrite8    HALEFUSEOneByteWriteROM

#define L25EOUTVOLTAGE	7
#endif

