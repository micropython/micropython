/*****************************************************************************
*
*  wlan.c  - CC3000 Host Driver Implementation.
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

//*****************************************************************************
//
//! \addtogroup wlan_api
//! @{
//
//*****************************************************************************

#include <string.h>
#include "wlan.h"
#include "hci.h"
#include "ccspi.h"
#include "socket.h"
#include "nvmem.h"
#include "security.h"
#include "evnt_handler.h"


volatile sSimplLinkInformation tSLInformation;

#define SMART_CONFIG_PROFILE_SIZE		67		// 67 = 32 (max ssid) + 32 (max key) + 1 (SSID length) + 1 (security type) + 1 (key length)

#ifndef CC3000_UNENCRYPTED_SMART_CONFIG
UINT8 key[AES128_KEY_SIZE];	
UINT8 profileArray[SMART_CONFIG_PROFILE_SIZE];
#endif //CC3000_UNENCRYPTED_SMART_CONFIG

/* patches type */
#define PATCHES_HOST_TYPE_WLAN_DRIVER   0x01
#define PATCHES_HOST_TYPE_WLAN_FW       0x02
#define PATCHES_HOST_TYPE_BOOTLOADER    0x03

#define SL_SET_SCAN_PARAMS_INTERVAL_LIST_SIZE	(16)
#define SL_SIMPLE_CONFIG_PREFIX_LENGTH (3)
#define ETH_ALEN								(6)
#define MAXIMAL_SSID_LENGTH						(32)

#define SL_PATCHES_REQUEST_DEFAULT		(0)
#define SL_PATCHES_REQUEST_FORCE_HOST	(1)
#define SL_PATCHES_REQUEST_FORCE_NONE	(2)


#define      WLAN_SEC_UNSEC (0)
#define      WLAN_SEC_WEP	(1)
#define      WLAN_SEC_WPA	(2)
#define      WLAN_SEC_WPA2	(3)


#define WLAN_SL_INIT_START_PARAMS_LEN			(1)
#define WLAN_PATCH_PARAMS_LENGTH				(8)
#define WLAN_SET_CONNECTION_POLICY_PARAMS_LEN	(12)
#define WLAN_DEL_PROFILE_PARAMS_LEN				(4)
#define WLAN_SET_MASK_PARAMS_LEN				(4)
#define WLAN_SET_SCAN_PARAMS_LEN				(100)
#define WLAN_GET_SCAN_RESULTS_PARAMS_LEN		(4)
#define WLAN_ADD_PROFILE_NOSEC_PARAM_LEN		(24)			
#define WLAN_ADD_PROFILE_WEP_PARAM_LEN			(36)
#define WLAN_ADD_PROFILE_WPA_PARAM_LEN			(44)
#define WLAN_CONNECT_PARAM_LEN					(29)
#define WLAN_SMART_CONFIG_START_PARAMS_LEN		(4)




//*****************************************************************************
//
//!  SimpleLink_Init_Start
//!
//!  @param  usPatchesAvailableAtHost  flag to indicate if patches available
//!                                    from host or from EEPROM. Due to the 
//!                                    fact the patches are burn to the EEPROM
//!                                    using the patch programmer utility, the 
//!                                    patches will be available from the EEPROM
//!                                    and not from the host.
//!
//!  @return   none
//!
//!  @brief    Send HCI_CMND_SIMPLE_LINK_START to CC3000
//
//*****************************************************************************
static void SimpleLink_Init_Start(UINT16 usPatchesAvailableAtHost)
{
	UINT8 *ptr;
	UINT8 *args;

	ptr = tSLInformation.pucTxCommandBuffer;
	args = (UINT8 *)(ptr + HEADERS_SIZE_CMD);

	UINT8_TO_STREAM(args, ((usPatchesAvailableAtHost) ? SL_PATCHES_REQUEST_FORCE_NONE : SL_PATCHES_REQUEST_DEFAULT));

	// IRQ Line asserted - send HCI_CMND_SIMPLE_LINK_START to CC3000
	hci_command_send(HCI_CMND_SIMPLE_LINK_START, ptr, WLAN_SL_INIT_START_PARAMS_LEN);

	SimpleLinkWaitEvent(HCI_CMND_SIMPLE_LINK_START, 0);
}



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

void wlan_init(		tWlanCB	 	sWlanCB,
	tFWPatches sFWPatches,
	tDriverPatches sDriverPatches,
	tBootLoaderPatches sBootLoaderPatches,
	tWlanReadInteruptPin  sReadWlanInterruptPin,
	tWlanInterruptEnable  sWlanInterruptEnable,
	tWlanInterruptDisable sWlanInterruptDisable,
	tWriteWlanPin         sWriteWlanPin)
{

	tSLInformation.sFWPatches = sFWPatches;
	tSLInformation.sDriverPatches = sDriverPatches;
	tSLInformation.sBootLoaderPatches = sBootLoaderPatches;

	// init io callback
	tSLInformation.ReadWlanInterruptPin = sReadWlanInterruptPin;
	tSLInformation.WlanInterruptEnable  = sWlanInterruptEnable;
	tSLInformation.WlanInterruptDisable = sWlanInterruptDisable;
	tSLInformation.WriteWlanPin = sWriteWlanPin;

	//init asynchronous events callback
	tSLInformation.sWlanCB= sWlanCB;

	// By default TX Complete events are routed to host too
	tSLInformation.InformHostOnTxComplete = 1;
}

//*****************************************************************************
//
//!  SpiReceiveHandler
//!
//!  @param         pvBuffer - pointer to the received data buffer
//!                      The function triggers Received event/data processing
//!                 
//!  @param         Pointer to the received data
//!  @return        none
//!
//!  @brief         The function triggers Received event/data processing. It is 
//! 			          called from the SPI library to receive the data
//
//*****************************************************************************
void SpiReceiveHandler(void *pvBuffer)
{	
	tSLInformation.usEventOrDataReceived = 1;
	tSLInformation.pucReceivedData = (UINT8 *)pvBuffer;

	hci_unsolicited_event_handler();
}


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
#define TIMEOUT (500000)

int wlan_start(UINT16 usPatchesAvailableAtHost)
{

	UINT32 ulSpiIRQState;
	UINT32 wlan_timeout;

	tSLInformation.NumberOfSentPackets = 0;
	tSLInformation.NumberOfReleasedPackets = 0;
	tSLInformation.usRxEventOpcode = 0;
	tSLInformation.usNumberOfFreeBuffers = 0;
	tSLInformation.usSlBufferLength = 0;
	tSLInformation.usBufferSize = 0;
	tSLInformation.usRxDataPending = 0;
	tSLInformation.slTransmitDataError = 0;
	tSLInformation.usEventOrDataReceived = 0;
	tSLInformation.pucReceivedData = 0;

	// Allocate the memory for the RX/TX data transactions
	tSLInformation.pucTxCommandBuffer = (UINT8 *)wlan_tx_buffer;

	// init spi
	SpiOpen(SpiReceiveHandler);

	// Check the IRQ line
	ulSpiIRQState = tSLInformation.ReadWlanInterruptPin();

	// Chip enable: toggle WLAN EN line
	tSLInformation.WriteWlanPin( WLAN_ENABLE );
    
    wlan_timeout = TIMEOUT;
	if (ulSpiIRQState) {
		// wait till the IRQ line goes low
		while(tSLInformation.ReadWlanInterruptPin() != 0 && --wlan_timeout)
		{
		}
	}
	else
	{
		// wait till the IRQ line goes high and than low
		while(tSLInformation.ReadWlanInterruptPin() == 0 && --wlan_timeout)
		{
		}

        if (wlan_timeout == 0) {
            return -1;
        }

        wlan_timeout = TIMEOUT;
		while(tSLInformation.ReadWlanInterruptPin() != 0 && --wlan_timeout)
		{
		}
	}

    if (wlan_timeout ==0) {
        return -1;
    }

	SimpleLink_Init_Start(usPatchesAvailableAtHost);

	// Read Buffer's size and finish
	hci_command_send(HCI_CMND_READ_BUFFER_SIZE, tSLInformation.pucTxCommandBuffer, 0);
	SimpleLinkWaitEvent(HCI_CMND_READ_BUFFER_SIZE, 0);

    return 0;
}


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

void wlan_stop(void)
{
	// Chip disable
	tSLInformation.WriteWlanPin( WLAN_DISABLE );

	// Wait till IRQ line goes high...
	while(tSLInformation.ReadWlanInterruptPin() == 0)
	{
	}

	// Free the used by WLAN Driver memory
	if (tSLInformation.pucTxCommandBuffer)
	{
		tSLInformation.pucTxCommandBuffer = 0;
	}

	SpiClose();
}


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
INT32 wlan_connect(UINT32 ulSecType, CHAR *ssid, INT32 ssid_len,
	UINT8 *bssid, UINT8 *key, INT32 key_len)
{
	INT32 ret;
	UINT8 *ptr;
	UINT8 *args;
	UINT8 bssid_zero[] = {0, 0, 0, 0, 0, 0};

	ret  	= EFAIL;
	ptr  	= tSLInformation.pucTxCommandBuffer;
	args 	= (ptr + HEADERS_SIZE_CMD);

	// Fill in command buffer
	args = UINT32_TO_STREAM(args, 0x0000001c);
	args = UINT32_TO_STREAM(args, ssid_len);
	args = UINT32_TO_STREAM(args, ulSecType);
	args = UINT32_TO_STREAM(args, 0x00000010 + ssid_len);
	args = UINT32_TO_STREAM(args, key_len);
	args = UINT16_TO_STREAM(args, 0);

	// padding shall be zeroed
	if(bssid)
	{
		ARRAY_TO_STREAM(args, bssid, ETH_ALEN);
	}
	else
	{
		ARRAY_TO_STREAM(args, bssid_zero, ETH_ALEN);
	}

	ARRAY_TO_STREAM(args, ssid, ssid_len);

	if(key_len && key)
	{
		ARRAY_TO_STREAM(args, key, key_len);
	}

	// Initiate a HCI command
	hci_command_send(HCI_CMND_WLAN_CONNECT, ptr, WLAN_CONNECT_PARAM_LEN + 
		ssid_len + key_len - 1);

	// Wait for command complete event
	SimpleLinkWaitEvent(HCI_CMND_WLAN_CONNECT, &ret);
	CC3000_EXPORT(errno) = ret;

	return(ret);
}
#else
INT32 wlan_connect(CHAR *ssid, INT32 ssid_len)
{
	INT32 ret;
	UINT8 *ptr;
	UINT8 *args;
	UINT8 bssid_zero[] = {0, 0, 0, 0, 0, 0};

	ret  	= EFAIL;
	ptr  	= tSLInformation.pucTxCommandBuffer;
	args 	= (ptr + HEADERS_SIZE_CMD);

	// Fill in command buffer
	args = UINT32_TO_STREAM(args, 0x0000001c);
	args = UINT32_TO_STREAM(args, ssid_len);
	args = UINT32_TO_STREAM(args, 0);
	args = UINT32_TO_STREAM(args, 0x00000010 + ssid_len);
	args = UINT32_TO_STREAM(args, 0);
	args = UINT16_TO_STREAM(args, 0);

	// padding shall be zeroed
	ARRAY_TO_STREAM(args, bssid_zero, ETH_ALEN);
	ARRAY_TO_STREAM(args, ssid, ssid_len);

	// Initiate a HCI command
	hci_command_send(HCI_CMND_WLAN_CONNECT, ptr, WLAN_CONNECT_PARAM_LEN + 
		ssid_len  - 1);

	// Wait for command complete event
	SimpleLinkWaitEvent(HCI_CMND_WLAN_CONNECT, &ret);
	CC3000_EXPORT(errno) = ret;

	return(ret);
}
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

INT32 wlan_disconnect()
{
	INT32 ret;
	UINT8 *ptr;

	ret = EFAIL;
	ptr = tSLInformation.pucTxCommandBuffer;

	hci_command_send(HCI_CMND_WLAN_DISCONNECT, ptr, 0);

	// Wait for command complete event
	SimpleLinkWaitEvent(HCI_CMND_WLAN_DISCONNECT, &ret);
	CC3000_EXPORT(errno) = ret;

	return(ret);
}

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

INT32 wlan_ioctl_set_connection_policy(UINT32 should_connect_to_open_ap, 
	UINT32 ulShouldUseFastConnect,
	UINT32 ulUseProfiles)
{
	INT32 ret;
	UINT8 *ptr;
	UINT8 *args;

	ret = EFAIL;
	ptr = tSLInformation.pucTxCommandBuffer;
	args = (UINT8 *)(ptr + HEADERS_SIZE_CMD);

	// Fill in HCI packet structure
	args = UINT32_TO_STREAM(args, should_connect_to_open_ap);
	args = UINT32_TO_STREAM(args, ulShouldUseFastConnect);
	args = UINT32_TO_STREAM(args, ulUseProfiles);

	// Initiate a HCI command
	hci_command_send(HCI_CMND_WLAN_IOCTL_SET_CONNECTION_POLICY,
		ptr, WLAN_SET_CONNECTION_POLICY_PARAMS_LEN);

	// Wait for command complete event
	SimpleLinkWaitEvent(HCI_CMND_WLAN_IOCTL_SET_CONNECTION_POLICY, &ret);

	return(ret);
}

//*****************************************************************************
//
//!  wlan_add_profile
//!
//!  @param    ulSecType  WLAN_SEC_UNSEC,WLAN_SEC_WEP,WLAN_SEC_WPA,WLAN_SEC_WPA2
//!  @param    ucSsid    ssid  SSID up to 32 bytes
//!  @param    ulSsidLen ssid length
//!  @param    ucBssid   bssid  6 bytes
//!  @param    ulPriority ulPriority profile priority. Lowest priority:0.
//!			   Important Note: Smartconfig process (in unencrypted mode) 
//!			   stores the profile internally with priority 1, so changing  
//!			   priorities when adding new profiles should be done with extra care
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

#ifndef CC3000_TINY_DRIVER
INT32 wlan_add_profile(UINT32 ulSecType, 
	UINT8* ucSsid,
	UINT32 ulSsidLen, 
	UINT8 *ucBssid,
	UINT32 ulPriority,
	UINT32 ulPairwiseCipher_Or_TxKeyLen,
	UINT32 ulGroupCipher_TxKeyIndex,
	UINT32 ulKeyMgmt,
	UINT8* ucPf_OrKey,
	UINT32 ulPassPhraseLen)
{
	UINT16 arg_len=0;
	INT32 ret;
	UINT8 *ptr;
	INT32 i = 0;
	UINT8 *args;
	UINT8 bssid_zero[] = {0, 0, 0, 0, 0, 0};

	ptr = tSLInformation.pucTxCommandBuffer;
	args = (ptr + HEADERS_SIZE_CMD);

	args = UINT32_TO_STREAM(args, ulSecType);

	// Setup arguments in accordance with the security type
	switch (ulSecType)
	{
		//OPEN
	case WLAN_SEC_UNSEC:
		{
			args = UINT32_TO_STREAM(args, 0x00000014);
			args = UINT32_TO_STREAM(args, ulSsidLen);
			args = UINT16_TO_STREAM(args, 0);
			if(ucBssid)
			{
				ARRAY_TO_STREAM(args, ucBssid, ETH_ALEN);
			}
			else
			{
				ARRAY_TO_STREAM(args, bssid_zero, ETH_ALEN);
			}
			args = UINT32_TO_STREAM(args, ulPriority);
			ARRAY_TO_STREAM(args, ucSsid, ulSsidLen);

			arg_len = WLAN_ADD_PROFILE_NOSEC_PARAM_LEN + ulSsidLen;
		}
		break;

		//WEP
	case WLAN_SEC_WEP:
		{
			args = UINT32_TO_STREAM(args, 0x00000020);
			args = UINT32_TO_STREAM(args, ulSsidLen);
			args = UINT16_TO_STREAM(args, 0);
			if(ucBssid)
			{
				ARRAY_TO_STREAM(args, ucBssid, ETH_ALEN);
			}
			else
			{
				ARRAY_TO_STREAM(args, bssid_zero, ETH_ALEN);
			}
			args = UINT32_TO_STREAM(args, ulPriority);
			args = UINT32_TO_STREAM(args, 0x0000000C + ulSsidLen);
			args = UINT32_TO_STREAM(args, ulPairwiseCipher_Or_TxKeyLen);
			args = UINT32_TO_STREAM(args, ulGroupCipher_TxKeyIndex);
			ARRAY_TO_STREAM(args, ucSsid, ulSsidLen);

			for(i = 0; i < 4; i++)
			{
				UINT8 *p = &ucPf_OrKey[i * ulPairwiseCipher_Or_TxKeyLen];

				ARRAY_TO_STREAM(args, p, ulPairwiseCipher_Or_TxKeyLen);
			}		

			arg_len = WLAN_ADD_PROFILE_WEP_PARAM_LEN + ulSsidLen + 
				ulPairwiseCipher_Or_TxKeyLen * 4;

		}
		break;

		//WPA
		//WPA2
	case WLAN_SEC_WPA:
	case WLAN_SEC_WPA2:
		{
			args = UINT32_TO_STREAM(args, 0x00000028);
			args = UINT32_TO_STREAM(args, ulSsidLen);
			args = UINT16_TO_STREAM(args, 0);
			if(ucBssid)
			{
				ARRAY_TO_STREAM(args, ucBssid, ETH_ALEN);
			}
			else
			{
				ARRAY_TO_STREAM(args, bssid_zero, ETH_ALEN);
			}
			args = UINT32_TO_STREAM(args, ulPriority);
			args = UINT32_TO_STREAM(args, ulPairwiseCipher_Or_TxKeyLen);
			args = UINT32_TO_STREAM(args, ulGroupCipher_TxKeyIndex);
			args = UINT32_TO_STREAM(args, ulKeyMgmt);
			args = UINT32_TO_STREAM(args, 0x00000008 + ulSsidLen);
			args = UINT32_TO_STREAM(args, ulPassPhraseLen);
			ARRAY_TO_STREAM(args, ucSsid, ulSsidLen);
			ARRAY_TO_STREAM(args, ucPf_OrKey, ulPassPhraseLen);

			arg_len = WLAN_ADD_PROFILE_WPA_PARAM_LEN + ulSsidLen + ulPassPhraseLen;
		}

		break;
	}    

	// Initiate a HCI command
	hci_command_send(HCI_CMND_WLAN_IOCTL_ADD_PROFILE,
		ptr, arg_len);

	// Wait for command complete event
	SimpleLinkWaitEvent(HCI_CMND_WLAN_IOCTL_ADD_PROFILE, &ret);

	return(ret);
}
#else
INT32 wlan_add_profile(UINT32 ulSecType, 
	UINT8* ucSsid,
	UINT32 ulSsidLen, 
	UINT8 *ucBssid,
	UINT32 ulPriority,
	UINT32 ulPairwiseCipher_Or_TxKeyLen,
	UINT32 ulGroupCipher_TxKeyIndex,
	UINT32 ulKeyMgmt,
	UINT8* ucPf_OrKey,
	UINT32 ulPassPhraseLen)
{
	return -1;
}
#endif

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

INT32 wlan_ioctl_del_profile(UINT32 ulIndex)
{
	INT32 ret;
	UINT8 *ptr;
	UINT8 *args;

	ptr = tSLInformation.pucTxCommandBuffer;
	args = (UINT8 *)(ptr + HEADERS_SIZE_CMD);

	// Fill in HCI packet structure
	args = UINT32_TO_STREAM(args, ulIndex);
	ret = EFAIL;

	// Initiate a HCI command
	hci_command_send(HCI_CMND_WLAN_IOCTL_DEL_PROFILE,
		ptr, WLAN_DEL_PROFILE_PARAMS_LEN);

	// Wait for command complete event
	SimpleLinkWaitEvent(HCI_CMND_WLAN_IOCTL_DEL_PROFILE, &ret);

	return(ret);
}

//*****************************************************************************
//
//!  wlan_ioctl_get_scan_results
//!
//!  @param[in]    scan_timeout   parameter not supported
//!  @param[out]   ucResults  scan results (_wlan_full_scan_results_args_t)
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

#ifndef CC3000_TINY_DRIVER
INT32 wlan_ioctl_get_scan_results(UINT32 ulScanTimeout,
	UINT8 *ucResults)
{
	UINT8 *ptr;
	UINT8 *args;

	ptr = tSLInformation.pucTxCommandBuffer;
	args = (ptr + HEADERS_SIZE_CMD);

	// Fill in temporary command buffer
	args = UINT32_TO_STREAM(args, ulScanTimeout);

	// Initiate a HCI command
	hci_command_send(HCI_CMND_WLAN_IOCTL_GET_SCAN_RESULTS,
		ptr, WLAN_GET_SCAN_RESULTS_PARAMS_LEN);

	// Wait for command complete event
	SimpleLinkWaitEvent(HCI_CMND_WLAN_IOCTL_GET_SCAN_RESULTS, ucResults);

	return(0);
}
#endif

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
//!           (connection scan) - in millisecond. Saved: yes. Default 2000ms.
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

#ifndef CC3000_TINY_DRIVER
INT32 wlan_ioctl_set_scan_params(UINT32 uiEnable, UINT32 uiMinDwellTime,
	UINT32 uiMaxDwellTime,
	UINT32 uiNumOfProbeRequests,
	UINT32 uiChannelMask,INT32 iRSSIThreshold,
	UINT32 uiSNRThreshold,
	UINT32 uiDefaultTxPower,
	UINT32 *aiIntervalList)
{
	UINT32  uiRes;
	UINT8 *ptr;
	UINT8 *args;

	ptr = tSLInformation.pucTxCommandBuffer;
	args = (ptr + HEADERS_SIZE_CMD);

	// Fill in temporary command buffer
	args = UINT32_TO_STREAM(args, 36);
	args = UINT32_TO_STREAM(args, uiEnable);
	args = UINT32_TO_STREAM(args, uiMinDwellTime);
	args = UINT32_TO_STREAM(args, uiMaxDwellTime);
	args = UINT32_TO_STREAM(args, uiNumOfProbeRequests);
	args = UINT32_TO_STREAM(args, uiChannelMask);
	args = UINT32_TO_STREAM(args, iRSSIThreshold);
	args = UINT32_TO_STREAM(args, uiSNRThreshold);
	args = UINT32_TO_STREAM(args, uiDefaultTxPower);
	ARRAY_TO_STREAM(args, aiIntervalList, sizeof(UINT32) * 
		SL_SET_SCAN_PARAMS_INTERVAL_LIST_SIZE);

	// Initiate a HCI command
	hci_command_send(HCI_CMND_WLAN_IOCTL_SET_SCANPARAM,
		ptr, WLAN_SET_SCAN_PARAMS_LEN);

	// Wait for command complete event
	SimpleLinkWaitEvent(HCI_CMND_WLAN_IOCTL_SET_SCANPARAM, &uiRes);

	return(uiRes);
}
#endif

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

INT32 wlan_set_event_mask(UINT32 ulMask)
{
	INT32 ret;
	UINT8 *ptr;
	UINT8 *args;


	if ((ulMask & HCI_EVNT_WLAN_TX_COMPLETE) == HCI_EVNT_WLAN_TX_COMPLETE)
	{
		tSLInformation.InformHostOnTxComplete = 0;

		// Since an event is a virtual event - i.e. it is not coming from CC3000
		// there is no need to send anything to the device if it was an only event
		if (ulMask == HCI_EVNT_WLAN_TX_COMPLETE)
		{
			return 0;
		}

		ulMask &= ~HCI_EVNT_WLAN_TX_COMPLETE;
		ulMask |= HCI_EVNT_WLAN_UNSOL_BASE;
	}
	else
	{
		tSLInformation.InformHostOnTxComplete = 1;
	}

	ret = EFAIL;
	ptr = tSLInformation.pucTxCommandBuffer;
	args = (UINT8 *)(ptr + HEADERS_SIZE_CMD);

	// Fill in HCI packet structure
	args = UINT32_TO_STREAM(args, ulMask);

	// Initiate a HCI command
	hci_command_send(HCI_CMND_EVENT_MASK,
		ptr, WLAN_SET_MASK_PARAMS_LEN);

	// Wait for command complete event
	SimpleLinkWaitEvent(HCI_CMND_EVENT_MASK, &ret);

	return(ret);
}

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

#ifndef CC3000_TINY_DRIVER
INT32 wlan_ioctl_statusget(void)
{
	INT32 ret;
	UINT8 *ptr;

	ret = EFAIL;
	ptr = tSLInformation.pucTxCommandBuffer;

	hci_command_send(HCI_CMND_WLAN_IOCTL_STATUSGET,
		ptr, 0);

	// Wait for command complete event
	SimpleLinkWaitEvent(HCI_CMND_WLAN_IOCTL_STATUSGET, &ret);

	return(ret);    
}
#endif

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

INT32 wlan_smart_config_start(UINT32 algoEncryptedFlag)
{
	INT32 ret;
	UINT8 *ptr;
	UINT8 *args;

	ret = EFAIL;
	ptr = tSLInformation.pucTxCommandBuffer;
	args = (UINT8 *)(ptr + HEADERS_SIZE_CMD);

	// Fill in HCI packet structure
	args = UINT32_TO_STREAM(args, algoEncryptedFlag);
	ret = EFAIL;

	hci_command_send(HCI_CMND_WLAN_IOCTL_SIMPLE_CONFIG_START, ptr, 
		WLAN_SMART_CONFIG_START_PARAMS_LEN);

	// Wait for command complete event
	SimpleLinkWaitEvent(HCI_CMND_WLAN_IOCTL_SIMPLE_CONFIG_START, &ret);

	return(ret);    
}

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

INT32 wlan_smart_config_stop(void)
{
	INT32 ret;
	UINT8 *ptr;

	ret = EFAIL;
	ptr = tSLInformation.pucTxCommandBuffer;

	hci_command_send(HCI_CMND_WLAN_IOCTL_SIMPLE_CONFIG_STOP, ptr, 0);

	// Wait for command complete event
	SimpleLinkWaitEvent(HCI_CMND_WLAN_IOCTL_SIMPLE_CONFIG_STOP, &ret);

	return(ret);    
}

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

INT32 wlan_smart_config_set_prefix(CHAR* cNewPrefix)
{
	INT32 ret;
	UINT8 *ptr;
	UINT8 *args;

	ret = EFAIL;
	ptr = tSLInformation.pucTxCommandBuffer;
	args = (ptr + HEADERS_SIZE_CMD);

	if (cNewPrefix == NULL)
		return ret;
	else	// with the new Smart Config, prefix must be TTT
	{
		*cNewPrefix = 'T';
		*(cNewPrefix + 1) = 'T';
		*(cNewPrefix + 2) = 'T';
	}

	ARRAY_TO_STREAM(args, cNewPrefix, SL_SIMPLE_CONFIG_PREFIX_LENGTH);

	hci_command_send(HCI_CMND_WLAN_IOCTL_SIMPLE_CONFIG_SET_PREFIX, ptr, 
		SL_SIMPLE_CONFIG_PREFIX_LENGTH);

	// Wait for command complete event
	SimpleLinkWaitEvent(HCI_CMND_WLAN_IOCTL_SIMPLE_CONFIG_SET_PREFIX, &ret);

	return(ret);    
}

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


#ifndef CC3000_UNENCRYPTED_SMART_CONFIG
INT32 wlan_smart_config_process()
{
	INT32	returnValue;
	UINT32 ssidLen, keyLen;
	UINT8 *decKeyPtr;
	UINT8 *ssidPtr;

	// read the key from EEPROM - fileID 12
	returnValue = aes_read_key(key);

	if (returnValue != 0)
		return returnValue;

	// read the received data from fileID #13 and parse it according to the followings:
	// 1) SSID LEN - not encrypted
	// 2) SSID - not encrypted
	// 3) KEY LEN - not encrypted. always 32 bytes long
	// 4) Security type - not encrypted
	// 5) KEY - encrypted together with true key length as the first byte in KEY
	//	 to elaborate, there are two corner cases:
	//		1) the KEY is 32 bytes long. In this case, the first byte does not represent KEY length
	//		2) the KEY is 31 bytes long. In this case, the first byte represent KEY length and equals 31
	returnValue = nvmem_read(NVMEM_SHARED_MEM_FILEID, SMART_CONFIG_PROFILE_SIZE, 0, profileArray);

	if (returnValue != 0)
		return returnValue;

	ssidPtr = &profileArray[1];

	ssidLen = profileArray[0];

	decKeyPtr = &profileArray[profileArray[0] + 3];

	aes_decrypt(decKeyPtr, key);
	if (profileArray[profileArray[0] + 1] > 16)
		aes_decrypt((UINT8 *)(decKeyPtr + 16), key);

	if (*(UINT8 *)(decKeyPtr +31) != 0)
	{
		if (*decKeyPtr == 31)
		{
			keyLen = 31;
			decKeyPtr++;
		}
		else
		{
			keyLen = 32;
		}
	}
	else
	{
		keyLen = *decKeyPtr;
		decKeyPtr++;
	}

	// add a profile
	switch (profileArray[profileArray[0] + 2])
	{
	case WLAN_SEC_UNSEC://None
		{
			returnValue = wlan_add_profile(profileArray[profileArray[0] + 2], 	// security type
				ssidPtr,		 					// SSID
				ssidLen, 							// SSID length
				NULL, 							// BSSID
				1,								// Priority
				0, 0, 0, 0, 0);

			break;
		}

	case WLAN_SEC_WEP://WEP
		{
			returnValue = wlan_add_profile(profileArray[profileArray[0] + 2], 	// security type
				ssidPtr, 							// SSID
				ssidLen, 							// SSID length
				NULL, 							// BSSID
				1,								// Priority
				keyLen,							// KEY length
				0, 								// KEY index
				0,
				decKeyPtr,						// KEY
				0);

			break;
		}

	case WLAN_SEC_WPA://WPA
	case WLAN_SEC_WPA2://WPA2
		{
			returnValue = wlan_add_profile(WLAN_SEC_WPA2, 	// security type
				ssidPtr,
				ssidLen,
				NULL, 							// BSSID
				1,								// Priority
				0x18,							// PairwiseCipher
				0x1e, 							// GroupCipher
				2,								// KEY management
				decKeyPtr,						// KEY
				keyLen);							// KEY length

			break;
		}
	}

	return returnValue;
}
#endif //CC3000_UNENCRYPTED_SMART_CONFIG		

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
