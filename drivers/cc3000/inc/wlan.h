/*****************************************************************************
*
*  wlan.h  - CC3000 Host Driver Implementation.
*  Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/
#ifndef __CC3000_WLAN_H__
#define	__CC3000_WLAN_H__

#include "cc3000_common.h"

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef	__cplusplus
extern "C" {
#endif

#define      WLAN_SEC_UNSEC (0)
#define      WLAN_SEC_WEP	(1)
#define      WLAN_SEC_WPA	(2)
#define      WLAN_SEC_WPA2	(3)
//*****************************************************************************
//
//! \addtogroup wlan_api
//! @{
//
//*****************************************************************************


//*****************************************************************************
//
//!  wlan_init
//!
//!  @param  sWlanCB   Asynchronous events callback.  
//!                    0 no event call back.
//!                  -call back parameters:
//!                   1) event_type: HCI_EVNT_WLAN_UNSOL_CONNECT connect event,
//!                     HCI_EVNT_WLAN_UNSOL_DISCONNECT disconnect event,
//!                     HCI_EVNT_WLAN_ASYNC_SIMPLE_CONFIG_DONE config done,
//!                     HCI_EVNT_WLAN_UNSOL_DHCP dhcp report, 
//!                     HCI_EVNT_WLAN_ASYNC_PING_REPORT ping report OR 
//!                     HCI_EVNT_WLAN_KEEPALIVE keepalive.
//!                   2) data: pointer to extra data that received by the event
//!                     (NULL no data).
//!                   3) length: data length.
//!                  -Events with extra data:
//!                     HCI_EVNT_WLAN_UNSOL_DHCP: 4 bytes IP, 4 bytes Mask, 
//!                     4 bytes default gateway, 4 bytes DHCP server and 4 bytes
//!                     for DNS server.
//!                     HCI_EVNT_WLAN_ASYNC_PING_REPORT: 4 bytes Packets sent, 
//!                     4 bytes Packets received, 4 bytes Min round time, 
//!                     4 bytes Max round time and 4 bytes for Avg round time.
//!
//!  @param    sFWPatches  0 no patch or pointer to FW patches 
//!  @param    sDriverPatches  0 no patch or pointer to driver patches
//!  @param    sBootLoaderPatches  0 no patch or pointer to bootloader patches
//!  @param    sReadWlanInterruptPin    init callback. the callback read wlan 
//!            interrupt status.
//!  @param    sWlanInterruptEnable   init callback. the callback enable wlan 
//!            interrupt.
//!  @param    sWlanInterruptDisable   init callback. the callback disable wlan
//!            interrupt.
//!  @param    sWriteWlanPin      init callback. the callback write value 
//!            to device pin.  
//!
//!  @return   none
//!
//!  @sa       wlan_set_event_mask , wlan_start , wlan_stop 
//!
//!  @brief    Initialize wlan driver
//!
//!  @warning This function must be called before ANY other wlan driver function
//
//*****************************************************************************
extern void wlan_init(		tWlanCB	 	sWlanCB,
	   			tFWPatches sFWPatches,
	   			tDriverPatches sDriverPatches,
	   			tBootLoaderPatches sBootLoaderPatches,
                tWlanReadInteruptPin  sReadWlanInterruptPin,
                tWlanInterruptEnable  sWlanInterruptEnable,
                tWlanInterruptDisable sWlanInterruptDisable,
                tWriteWlanPin         sWriteWlanPin);



//*****************************************************************************
//
//!  wlan_start
//!
//!  @param   usPatchesAvailableAtHost -  flag to indicate if patches available
//!                                    from host or from EEPROM. Due to the 
//!                                    fact the patches are burn to the EEPROM
//!                                    using the patch programmer utility, the 
//!                                    patches will be available from the EEPROM
//!                                    and not from the host.
//!
//!  @return        none
//!
//!  @brief        Start WLAN device. This function asserts the enable pin of 
//!                the device (WLAN_EN), starting the HW initialization process.
//!                The function blocked until device Initialization is completed.
//!                Function also configure patches (FW, driver or bootloader) 
//!                and calls appropriate device callbacks.
//!
//!  @Note          Prior calling the function wlan_init shall be called.
//!  @Warning       This function must be called after wlan_init and before any 
//!                 other wlan API
//!  @sa            wlan_init , wlan_stop
//!
//
//*****************************************************************************
extern int wlan_start(UINT16 usPatchesAvailableAtHost);

//*****************************************************************************
//
//!  wlan_stop
//!
//!  @param         none
//!
//!  @return        none
//!
//!  @brief         Stop WLAN device by putting it into reset state.
//!
//!  @sa            wlan_start
//
//*****************************************************************************
extern void wlan_stop(void);

//*****************************************************************************
//
//!  wlan_connect
//!
//!  @param    sec_type   security options:
//!               WLAN_SEC_UNSEC, 
//!               WLAN_SEC_WEP (ASCII support only),
//!               WLAN_SEC_WPA or WLAN_SEC_WPA2
//!  @param    ssid       up to 32 bytes and is ASCII SSID of the AP
//!  @param    ssid_len   length of the SSID
//!  @param    bssid      6 bytes specified the AP bssid
//!  @param    key        up to 32 bytes specified the AP security key
//!  @param    key_len    key length 
//!
//!  @return     On success, zero is returned. On error, negative is returned. 
//!              Note that even though a zero is returned on success to trigger
//!              connection operation, it does not mean that CCC3000 is already
//!              connected. An asynchronous "Connected" event is generated when 
//!              actual association process finishes and CC3000 is connected to
//!              the AP. If DHCP is set, An asynchronous "DHCP" event is 
//!              generated when DHCP process is finish.
//!              
//!
//!  @brief      Connect to AP
//!  @warning    Please Note that when connection to AP configured with security
//!              type WEP, please confirm that the key is set as ASCII and not
//!              as HEX.
//!  @sa         wlan_disconnect
//
//*****************************************************************************
#ifndef CC3000_TINY_DRIVER
extern INT32 wlan_connect(UINT32 ulSecType, CHAR *ssid, INT32 ssid_len,
                        UINT8 *bssid, UINT8 *key, INT32 key_len);
#else
extern INT32 wlan_connect(CHAR *ssid, INT32 ssid_len);

#endif

//*****************************************************************************
//
//!  wlan_disconnect
//!
//!  @return    0 disconnected done, other CC3000 already disconnected            
//!
//!  @brief      Disconnect connection from AP. 
//!
//!  @sa         wlan_connect
//
//*****************************************************************************

extern INT32 wlan_disconnect(void);

//*****************************************************************************
//
//!  wlan_add_profile
//!
//!  @param    ulSecType  WLAN_SEC_UNSEC,WLAN_SEC_WEP,WLAN_SEC_WPA,WLAN_SEC_WPA2
//!  @param    ucSsid    ssid  SSID up to 32 bytes
//!  @param    ulSsidLen ssid length
//!  @param    ucBssid   bssid  6 bytes
//!  @param    ulPriority ulPriority profile priority. Lowest priority:0.
//!  @param    ulPairwiseCipher_Or_TxKeyLen  key length for WEP security
//!  @param    ulGroupCipher_TxKeyIndex  key index
//!  @param    ulKeyMgmt        KEY management 
//!  @param    ucPf_OrKey       security key
//!  @param    ulPassPhraseLen  security key length for WPA\WPA2
//!
//!  @return    On success, index (1-7) of the stored profile is returned.         
//!				On error, -1 is returned.        
//!
//!  @brief     When auto start is enabled, the device connects to
//!             station from the profiles table. Up to 7 profiles are supported. 
//!             If several profiles configured the device choose the highest 
//!             priority profile, within each priority group, device will choose 
//!             profile based on security policy, signal strength, etc 
//!             parameters. All the profiles are stored in CC3000 NVMEM.
//!
//!  @sa        wlan_ioctl_del_profile 
//
//*****************************************************************************

extern INT32 wlan_add_profile(UINT32 ulSecType, UINT8* ucSsid,
										 UINT32 ulSsidLen, 
										 UINT8 *ucBssid,
                                         UINT32 ulPriority,
                                         UINT32 ulPairwiseCipher_Or_Key,
                                         UINT32 ulGroupCipher_TxKeyLen,
                                         UINT32 ulKeyMgmt,
                                         UINT8* ucPf_OrKey,
                                         UINT32 ulPassPhraseLen);



//*****************************************************************************
//
//!  wlan_ioctl_del_profile
//!
//!  @param    index   number of profile to delete
//!
//!  @return    On success, zero is returned. On error, -1 is returned        
//!
//!  @brief     Delete WLAN profile 
//!
//!  @Note      In order to delete all stored profile, set index to 255.
//!
//!  @sa        wlan_add_profile 
//
//*****************************************************************************
extern INT32 wlan_ioctl_del_profile(UINT32 ulIndex);

//*****************************************************************************
//
//!  wlan_set_event_mask
//!
//!  @param    mask   mask option:
//!       HCI_EVNT_WLAN_UNSOL_CONNECT connect event
//!       HCI_EVNT_WLAN_UNSOL_DISCONNECT disconnect event
//!       HCI_EVNT_WLAN_ASYNC_SIMPLE_CONFIG_DONE  smart config done
//!       HCI_EVNT_WLAN_UNSOL_INIT init done
//!       HCI_EVNT_WLAN_UNSOL_DHCP dhcp event report
//!       HCI_EVNT_WLAN_ASYNC_PING_REPORT ping report
//!       HCI_EVNT_WLAN_KEEPALIVE keepalive
//!       HCI_EVNT_WLAN_TX_COMPLETE - disable information on end of transmission
//!   	  Saved: no.
//!
//!  @return    On success, zero is returned. On error, -1 is returned        
//!
//!  @brief    Mask event according to bit mask. In case that event is 
//!            masked (1), the device will not send the masked event to host. 
//
//*****************************************************************************
extern INT32 wlan_set_event_mask(UINT32 ulMask);

//*****************************************************************************
//
//!  wlan_ioctl_statusget
//!
//!  @param none 
//!
//!  @return    WLAN_STATUS_DISCONNECTED, WLAN_STATUS_SCANING, 
//!             STATUS_CONNECTING or WLAN_STATUS_CONNECTED      
//!
//!  @brief    get wlan status: disconnected, scanning, connecting or connected
//
//*****************************************************************************
extern INT32 wlan_ioctl_statusget(void);


//*****************************************************************************
//
//!  wlan_ioctl_set_connection_policy
//!
//!  @param    should_connect_to_open_ap  enable(1), disable(0) connect to any 
//!            available AP. This parameter corresponds to the configuration of 
//!            item # 3 in the brief description.
//!  @param    should_use_fast_connect enable(1), disable(0). if enabled, tries 
//!            to connect to the last connected AP. This parameter corresponds 
//!            to the configuration of item # 1 in the brief description.
//!  @param    auto_start enable(1), disable(0) auto connect 
//!            after reset and periodically reconnect if needed. This 
//!       	   configuration configures option 2 in the above description.
//!
//!  @return     On success, zero is returned. On error, -1 is returned        
//!
//!  @brief      When auto is enabled, the device tries to connect according 
//!              the following policy:
//!              1) If fast connect is enabled and last connection is valid, 
//!                 the device will try to connect to it without the scanning 
//!                 procedure (fast). The last connection will be marked as
//!                 invalid, due to adding/removing profile. 
//!              2) If profile exists, the device will try to connect it 
//!                 (Up to seven profiles).
//!              3) If fast and profiles are not found, and open mode is
//!                 enabled, the device will try to connect to any AP.
//!              * Note that the policy settings are stored in the CC3000 NVMEM.
//!
//!  @sa         wlan_add_profile , wlan_ioctl_del_profile 
//
//*****************************************************************************
extern INT32 wlan_ioctl_set_connection_policy(
                                        UINT32 should_connect_to_open_ap,
                                        UINT32 should_use_fast_connect,
                                        UINT32 ulUseProfiles);

//*****************************************************************************
//
//!  wlan_ioctl_get_scan_results
//!
//!  @param[in]    scan_timeout   parameter not supported
//!  @param[out]   ucResults  scan result (_wlan_full_scan_results_args_t)
//!
//!  @return    On success, zero is returned. On error, -1 is returned        
//!
//!  @brief    Gets entry from scan result table.
//!            The scan results are returned one by one, and each entry 
//!            represents a single AP found in the area. The following is a 
//!            format of the scan result: 
//!        	 - 4 Bytes: number of networks found
//!          - 4 Bytes: The status of the scan: 0 - aged results,
//!                     1 - results valid, 2 - no results
//!          - 42 bytes: Result entry, where the bytes are arranged as  follows:
//!              
//!          				- 1 bit isValid - is result valid or not
//!         				- 7 bits rssi - RSSI value;	 
//!                 - 2 bits: securityMode - security mode of the AP:
//!                           0 - Open, 1 - WEP, 2 WPA, 3 WPA2
//!         				- 6 bits: SSID name length
//!         				- 2 bytes: the time at which the entry has entered into 
//!                            scans result table
//!         				- 32 bytes: SSID name
//!                 - 6 bytes:	BSSID 
//!
//!  @Note      scan_timeout, is not supported on this version.
//!
//!  @sa        wlan_ioctl_set_scan_params 
//
//*****************************************************************************


extern INT32 wlan_ioctl_get_scan_results(UINT32 ulScanTimeout,
                                       UINT8 *ucResults);

//*****************************************************************************
//
//!  wlan_ioctl_set_scan_params
//!
//!  @param    uiEnable - start/stop application scan: 
//!            1 = start scan with default interval value of 10 min. 
//!            in order to set a different scan interval value apply the value 
//!            in milliseconds. minimum 1 second. 0=stop). Wlan reset
//!           (wlan_stop() wlan_start()) is needed when changing scan interval
//!            value. Saved: No
//!  @param   uiMinDwellTime   minimum dwell time value to be used for each 
//!           channel, in milliseconds. Saved: yes
//!           Recommended Value: 100 (Default: 20)
//!  @param   uiMaxDwellTime    maximum dwell time value to be used for each
//!           channel, in milliseconds. Saved: yes
//!           Recommended Value: 100 (Default: 30)
//!  @param   uiNumOfProbeRequests  max probe request between dwell time. 
//!           Saved: yes. Recommended Value: 5 (Default:2)
//!  @param   uiChannelMask  bitwise, up to 13 channels (0x1fff). 
//!           Saved: yes. Default: 0x7ff
//!  @param   uiRSSIThreshold   RSSI threshold. Saved: yes (Default: -80)
//!  @param   uiSNRThreshold    NSR threshold. Saved: yes (Default: 0)
//!  @param   uiDefaultTxPower  probe Tx power. Saved: yes (Default: 205)
//!  @param   aiIntervalList    pointer to array with 16 entries (16 channels) 
//!           each entry (UINT32) holds timeout between periodic scan 
//!           (connection scan) - in milliseconds. Saved: yes. Default 2000ms.
//!
//!  @return    On success, zero is returned. On error, -1 is returned        
//!
//!  @brief    start and stop scan procedure. Set scan parameters. 
//!
//!  @Note     uiDefaultTxPower, is not supported on this version.
//!
//!  @sa        wlan_ioctl_get_scan_results 
//
//*****************************************************************************
extern INT32 wlan_ioctl_set_scan_params(UINT32 uiEnable, UINT32 
											 uiMinDwellTime,UINT32 uiMaxDwellTime,
										   UINT32 uiNumOfProbeRequests,
											 UINT32 uiChannelMask,
										   INT32 iRSSIThreshold,UINT32 uiSNRThreshold,
										   UINT32 uiDefaultTxPower, 
											 UINT32 *aiIntervalList);

                                           
//*****************************************************************************
//
//!  wlan_smart_config_start
//!
//!  @param    algoEncryptedFlag indicates whether the information is encrypted
//!
//!  @return   On success, zero is returned. On error, -1 is returned        
//!
//!  @brief   Start to acquire device profile. The device acquire its own 
//!           profile, if profile message is found. The acquired AP information
//!           is stored in CC3000 EEPROM only in case AES128 encryption is used.
//!           In case AES128 encryption is not used, a profile is created by 
//!           CC3000 internally.
//!
//!  @Note    An asynchronous event - Smart Config Done will be generated as soon
//!           as the process finishes successfully.
//!
//!  @sa      wlan_smart_config_set_prefix , wlan_smart_config_stop
//
//*****************************************************************************                                        
extern INT32 wlan_smart_config_start(UINT32 algoEncryptedFlag);


//*****************************************************************************
//
//!  wlan_smart_config_stop
//!
//!  @param    algoEncryptedFlag indicates whether the information is encrypted
//!
//!  @return   On success, zero is returned. On error, -1 is returned        
//!
//!  @brief   Stop the acquire profile procedure 
//!
//!  @sa      wlan_smart_config_start , wlan_smart_config_set_prefix
//
//*****************************************************************************
extern INT32 wlan_smart_config_stop(void);

//*****************************************************************************
//
//!  wlan_smart_config_set_prefix
//!
//!  @param   newPrefix  3 bytes identify the SSID prefix for the Smart Config. 
//!
//!  @return   On success, zero is returned. On error, -1 is returned        
//!
//!  @brief   Configure station ssid prefix. The prefix is used internally 
//!           in CC3000. It should always be TTT.
//!
//!  @Note    The prefix is stored in CC3000 NVMEM
//!
//!  @sa      wlan_smart_config_start , wlan_smart_config_stop
//
//*****************************************************************************
extern INT32 wlan_smart_config_set_prefix(CHAR* cNewPrefix);

//*****************************************************************************
//
//!  wlan_smart_config_process
//!
//!  @param   none 
//!
//!  @return   On success, zero is returned. On error, -1 is returned        
//!
//!  @brief   process the acquired data and store it as a profile. The acquired 
//!           AP information is stored in CC3000 EEPROM encrypted.
//!           The encrypted data is decrypted and stored as a profile.
//!           behavior is as defined by connection policy.
//
//*****************************************************************************
extern INT32 wlan_smart_config_process(void);

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************



//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef	__cplusplus
}
#endif // __cplusplus

#endif	// __CC3000_WLAN_H__
