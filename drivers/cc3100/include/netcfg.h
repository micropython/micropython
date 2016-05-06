/*
 * netcfg.h - CC31xx/CC32xx Host Driver Implementation
 *
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
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
*/

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/
#include "simplelink.h"

    
#ifndef __NETCFG_H__
#define __NETCFG_H__


#ifdef    __cplusplus
extern "C" {
#endif



/*!

    \addtogroup netcfg
    @{

*/


/*****************************************************************************/
/* Macro declarations                                                        */
/*****************************************************************************/

#define SL_MAC_ADDR_LEN                          (6)
#define SL_IPV4_VAL(add_3,add_2,add_1,add_0)     ((((_u32)add_3 << 24) & 0xFF000000) | (((_u32)add_2 << 16) & 0xFF0000) | (((_u32)add_1 << 8) & 0xFF00) | ((_u32)add_0 & 0xFF) )
#define SL_IPV4_BYTE(val,index)                  ( (val >> (index*8)) & 0xFF )

#define IPCONFIG_MODE_DISABLE_IPV4               (0)
#define IPCONFIG_MODE_ENABLE_IPV4                (1)

/*****************************************************************************/
/* Structure/Enum declarations                                               */
/*****************************************************************************/
typedef enum
{
    SL_MAC_ADDRESS_SET          = 1,
    SL_MAC_ADDRESS_GET          = 2,          
    SL_IPV4_STA_P2P_CL_GET_INFO           = 3,
    SL_IPV4_STA_P2P_CL_DHCP_ENABLE        = 4,
    SL_IPV4_STA_P2P_CL_STATIC_ENABLE      = 5,
    SL_IPV4_AP_P2P_GO_GET_INFO            = 6,
    SL_IPV4_AP_P2P_GO_STATIC_ENABLE       = 7,
    SL_SET_HOST_RX_AGGR                   = 8,
    MAX_SETTINGS = 0xFF
}Sl_NetCfg_e;


typedef struct
{
    _u32  ipV4;
    _u32  ipV4Mask;
    _u32  ipV4Gateway;
    _u32  ipV4DnsServer;
}SlNetCfgIpV4Args_t;


/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/

/*!
    \brief     Internal function for setting network configurations

    \return    On success, zero is returned. On error, -1 is 
               returned
     
    \param[in] ConfigId   configuration id
    \param[in] ConfigOpt  configurations option
    \param[in] ConfigLen  configurations len
    \param[in] pValues    configurations values
     
    \sa         
    \note 
    \warning  

    \par   Examples:
    \code
        SL_MAC_ADDRESS_SET:
    
        Setting MAC address to the Device.
        The new MAC address will override the default MAC address and it be saved in the FileSystem.
        Requires restarting the device for updating this setting.
    
        _u8 MAC_Address[6];
        MAC_Address[0] = 0x8;
        MAC_Address[1] = 0x0;
        MAC_Address[2] = 0x28;
        MAC_Address[3] = 0x22;
        MAC_Address[4] = 0x69;
        MAC_Address[5] = 0x31;
        sl_NetCfgSet(SL_MAC_ADDRESS_SET,1,SL_MAC_ADDR_LEN,(_u8 *)newMacAddress);
        sl_Stop(0);
        sl_Start(NULL,NULL,NULL);
    \endcode 

    \code 
        SL_IPV4_STA_P2P_CL_STATIC_ENABLE:

        Setting a static IP address to the device working in STA mode or P2P client.
        The IP address will be stored in the FileSystem.
        In order to disable the static IP and get the address assigned from DHCP one should use SL_STA_P2P_CL_IPV4_DHCP_SET

        SlNetCfgIpV4Args_t ipV4;
        ipV4.ipV4          = (_u32)SL_IPV4_VAL(10,1,1,201);            // _u32 IP address 
        ipV4.ipV4Mask      = (_u32)SL_IPV4_VAL(255,255,255,0);         // _u32 Subnet mask for this STA/P2P
        ipV4.ipV4Gateway   = (_u32)SL_IPV4_VAL(10,1,1,1);              // _u32 Default gateway address
        ipV4.ipV4DnsServer = (_u32)SL_IPV4_VAL(8,16,32,64);            // _u32 DNS server address

        sl_NetCfgSet(SL_IPV4_STA_P2P_CL_STATIC_ENABLE,IPCONFIG_MODE_ENABLE_IPV4,sizeof(SlNetCfgIpV4Args_t),(_u8 *)&ipV4); 
        sl_Stop(0);
        sl_Start(NULL,NULL,NULL);
    \endcode

    \code 
        SL_IPV4_STA_P2P_CL_DHCP_ENABLE:

        Setting IP address by DHCP to FileSystem using WLAN sta mode or P2P client.
                This should be done once if using Serial Flash.
                This is the system's default mode for acquiring an IP address after WLAN connection.
        _u8 val = 1;
        sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,IPCONFIG_MODE_ENABLE_IPV4,1,&val);
        sl_Stop(0);
        sl_Start(NULL,NULL,NULL);
    \endcode

    \code       
        SL_IPV4_AP_P2P_GO_STATIC_ENABLE:

        Setting a static IP address to the device working in AP mode or P2P go.
        The IP address will be stored in the FileSystem. Requires restart.
                                                                     
        SlNetCfgIpV4Args_t ipV4;
        ipV4.ipV4          = (_u32)SL_IPV4_VAL(10,1,1,201);            // _u32 IP address 
        ipV4.ipV4Mask      = (_u32)SL_IPV4_VAL(255,255,255,0);         // _u32 Subnet mask for this AP/P2P
        ipV4.ipV4Gateway   = (_u32)SL_IPV4_VAL(10,1,1,1);              // _u32 Default gateway address
        ipV4.ipV4DnsServer = (_u32)SL_IPV4_VAL(8,16,32,64);            // _u32 DNS server address

        sl_NetCfgSet(SL_IPV4_AP_P2P_GO_STATIC_ENABLE,IPCONFIG_MODE_ENABLE_IPV4,sizeof(SlNetCfgIpV4Args_t),(_u8 *)&ipV4);
        sl_Stop(0);
        sl_Start(NULL,NULL,NULL);
    \endcode

   
*/
#if _SL_INCLUDE_FUNC(sl_NetCfgSet)
_i32 sl_NetCfgSet(const _u8 ConfigId,const _u8 ConfigOpt,const _u8 ConfigLen,const _u8 *pValues);
#endif


/*!
    \brief     Internal function for getting network configurations

    \return    On success, zero is returned. On error, -1 is 
               returned
   
    \param[in] ConfigId      configuration id

    \param[out] pConfigOpt   Get configurations option 

    \param[out] pConfigLen   The length of the allocated memory as input, when the
                                        function complete, the value of this parameter would be
                             the len that actually read from the device.\n 
                                        If the device return length that is longer from the input 
                                        value, the function will cut the end of the returned structure
                                        and will return ESMALLBUF

    \param[out] pValues - get configurations values

    \sa         
    \note 
    \warning     
    \par   Examples:
    \code
       SL_MAC_ADDRESS_GET:
        
       Get the device MAC address.
       The returned MAC address is taken from FileSystem first. If the MAC address was not set by SL_MAC_ADDRESS_SET, the default MAC address
       is retrieved from HW.
       
       _u8 macAddressVal[SL_MAC_ADDR_LEN];
       _u8 macAddressLen = SL_MAC_ADDR_LEN;
       sl_NetCfgGet(SL_MAC_ADDRESS_GET,NULL,&macAddressLen,(_u8 *)macAddressVal);
    
    \endcode

    \code
        SL_IPV4_STA_P2P_CL_GET_INFO:
       
        Get IP address from WLAN station or P2P client. A DHCP flag is returned to indicate if the IP address is static or from DHCP. 
 
        _u8 len = sizeof(SlNetCfgIpV4Args_t);
        _u8 dhcpIsOn = 0;
        SlNetCfgIpV4Args_t ipV4 = {0};
        sl_NetCfgGet(SL_IPV4_STA_P2P_CL_GET_INFO,&dhcpIsOn,&len,(_u8 *)&ipV4);
                                          
        printf("DHCP is %s IP %d.%d.%d.%d MASK %d.%d.%d.%d GW %d.%d.%d.%d DNS %d.%d.%d.%d\n",        
                (dhcpIsOn > 0) ? "ON" : "OFF",                                                           
                SL_IPV4_BYTE(ipV4.ipV4,3),SL_IPV4_BYTE(ipV4.ipV4,2),SL_IPV4_BYTE(ipV4.ipV4,1),SL_IPV4_BYTE(ipV4.ipV4,0), 
                SL_IPV4_BYTE(ipV4.ipV4Mask,3),SL_IPV4_BYTE(ipV4.ipV4Mask,2),SL_IPV4_BYTE(ipV4.ipV4Mask,1),SL_IPV4_BYTE(ipV4.ipV4Mask,0),         
                SL_IPV4_BYTE(ipV4.ipV4Gateway,3),SL_IPV4_BYTE(ipV4.ipV4Gateway,2),SL_IPV4_BYTE(ipV4.ipV4Gateway,1),SL_IPV4_BYTE(ipV4.ipV4Gateway,0),                 
                SL_IPV4_BYTE(ipV4.ipV4DnsServer,3),SL_IPV4_BYTE(ipV4.ipV4DnsServer,2),SL_IPV4_BYTE(ipV4.ipV4DnsServer,1),SL_IPV4_BYTE(ipV4.ipV4DnsServer,0));

    \endcode

    \code
        SL_IPV4_AP_P2P_GO_GET_INFO:
       
        Get static IP address for AP or P2P go.   
 
        _u8 len = sizeof(SlNetCfgIpV4Args_t);
        _u8 dhcpIsOn = 0; // this flag is meaningless on AP/P2P go.
        SlNetCfgIpV4Args_t ipV4 = {0};
        sl_NetCfgGet(SL_IPV4_AP_P2P_GO_GET_INFO,&dhcpIsOn,&len,(_u8 *)&ipV4);
                                          
        printf("IP %d.%d.%d.%d MASK %d.%d.%d.%d GW %d.%d.%d.%d DNS %d.%d.%d.%d\n",                                                             
                SL_IPV4_BYTE(ipV4.ipV4,3),SL_IPV4_BYTE(ipV4.ipV4,2),SL_IPV4_BYTE(ipV4.ipV4,1),SL_IPV4_BYTE(ipV4.ipV4,0), 
                SL_IPV4_BYTE(ipV4.ipV4Mask,3),SL_IPV4_BYTE(ipV4.ipV4Mask,2),SL_IPV4_BYTE(ipV4.ipV4Mask,1),SL_IPV4_BYTE(ipV4.ipV4Mask,0),         
                SL_IPV4_BYTE(ipV4.ipV4Gateway,3),SL_IPV4_BYTE(ipV4.ipV4Gateway,2),SL_IPV4_BYTE(ipV4.ipV4Gateway,1),SL_IPV4_BYTE(ipV4.ipV4Gateway,0),                 
                SL_IPV4_BYTE(ipV4.ipV4DnsServer,3),SL_IPV4_BYTE(ipV4.ipV4DnsServer,2),SL_IPV4_BYTE(ipV4.ipV4DnsServer,1),SL_IPV4_BYTE(ipV4.ipV4DnsServer,0));

    \endcode

   
*/
#if _SL_INCLUDE_FUNC(sl_NetCfgGet)
_i32 sl_NetCfgGet(const _u8 ConfigId ,_u8 *pConfigOpt, _u8 *pConfigLen, _u8 *pValues);
#endif

/*!

 Close the Doxygen group.
 @}

 */


#ifdef  __cplusplus
}
#endif /*  __cplusplus */

#endif    /*  __NETCFG_H__ */

