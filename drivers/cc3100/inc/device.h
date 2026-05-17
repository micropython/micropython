/*
 * device.h - CC31xx/CC32xx Host Driver Implementation
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

#ifndef __DEVICE_H__
#define __DEVICE_H__



#ifdef __cplusplus
extern "C" {
#endif



/*!

    \addtogroup device
    @{

*/

/*****************************************************************************/
/* Macro declarations                                                        */
/*****************************************************************************/
     
     
/* SL internal Error codes */
         
/* Receive this error in case there are no resources to issue the command
   If possible, increase the number of MAX_CUNCURENT_ACTIONS (result in memory increase)
   If not, try again later */
#define SL_POOL_IS_EMPTY (-2000)
         
/* Receive this error in case a given length for RX buffer was too small. 
   Receive payload was bigger than the given buffer size. Therefore, payload is cut according to receive size 
   Recommend to increase buffer size */
#define SL_ESMALLBUF     (-2001)

/* Receive this error in case zero length is supplied to a "get" API
   Recommend to supply length according to requested information (view options defines for help) */
#define SL_EZEROLEN      (-2002)

/* User supplied invalid parameter */
#define SL_INVALPARAM    (-2003)


/* Failed to open interface  */
#define SL_BAD_INTERFACE    (-2004)

/* End of SL internal Error codes */



/*****************************************************************************/
/* Errors returned from the general error async event                        */
/*****************************************************************************/

/* Send types */
typedef enum
{
    SL_ERR_SENDER_HEALTH_MON,
    SL_ERR_SENDER_CLI_UART,
    SL_ERR_SENDER_SUPPLICANT,
    SL_ERR_SENDER_NETWORK_STACK,
    SL_ERR_SENDER_WLAN_DRV_IF,
    SL_ERR_SENDER_WILINK,
    SL_ERR_SENDER_INIT_APP,
    SL_ERR_SENDER_NETX,
    SL_ERR_SENDER_HOST_APD,
    SL_ERR_SENDER_MDNS,
    SL_ERR_SENDER_HTTP_SERVER,
    SL_ERR_SENDER_DHCP_SERVER,
    SL_ERR_SENDER_DHCP_CLIENT,
    SL_ERR_DISPATCHER,
    SL_ERR_NUM_SENDER_LAST=0xFF
}SlErrorSender_e; 


/* Error codes */
#define SL_ERROR_STATIC_ADDR_SUBNET_ERROR                   (-60)  /* network stack error*/
#define SL_ERROR_ILLEGAL_CHANNEL                            (-61)  /* supplicant error */
#define SL_ERROR_SUPPLICANT_ERROR                           (-72)  /* init error code */
#define SL_ERROR_HOSTAPD_INIT_FAIL                          (-73)  /* init error code */
#define SL_ERROR_HOSTAPD_INIT_IF_FAIL                       (-74)  /* init error code */
#define SL_ERROR_WLAN_DRV_INIT_FAIL                         (-75)  /* init error code */
#define SL_ERROR_WLAN_DRV_START_FAIL                        (-76)  /* wlan start error */
#define SL_ERROR_FS_FILE_TABLE_LOAD_FAILED                  (-77)  /* init file system failed */
#define SL_ERROR_PREFERRED_NETWORKS_FILE_LOAD_FAILED        (-78)  /* init file system failed */
#define SL_ERROR_HOSTAPD_BSSID_VALIDATION_ERROR             (-79)  /* Ap configurations BSSID error */
#define SL_ERROR_HOSTAPD_FAILED_TO_SETUP_INTERFACE          (-80)  /* Ap configurations interface error */
#define SL_ERROR_MDNS_ENABLE_FAIL                           (-81)  /* mDNS enable failed                */
#define SL_ERROR_HTTP_SERVER_ENABLE_FAILED                  (-82)  /* HTTP server enable failed         */
#define SL_ERROR_DHCP_SERVER_ENABLE_FAILED                  (-83)  /* DHCP server enable failed         */
#define SL_ERROR_PREFERRED_NETWORK_LIST_FULL                (-93)  /* supplicant error  */
#define SL_ERROR_PREFERRED_NETWORKS_FILE_WRITE_FAILED       (-94)  /* supplicant error  */
#define SL_ERROR_DHCP_CLIENT_RENEW_FAILED                   (-100) /* DHCP client error */
/* WLAN Connection management status */
#define SL_ERROR_CON_MGMT_STATUS_UNSPECIFIED                (-102)  
#define SL_ERROR_CON_MGMT_STATUS_AUTH_REJECT                (-103)  
#define SL_ERROR_CON_MGMT_STATUS_ASSOC_REJECT               (-104)  
#define SL_ERROR_CON_MGMT_STATUS_SECURITY_FAILURE           (-105)  
#define SL_ERROR_CON_MGMT_STATUS_AP_DEAUTHENTICATE          (-106)  
#define SL_ERROR_CON_MGMT_STATUS_AP_DISASSOCIATE            (-107)  
#define SL_ERROR_CON_MGMT_STATUS_ROAMING_TRIGGER            (-108)  
#define SL_ERROR_CON_MGMT_STATUS_DISCONNECT_DURING_CONNECT  (-109)  
#define SL_ERROR_CON_MGMT_STATUS_SG_RESELECT                (-110)  
#define SL_ERROR_CON_MGMT_STATUS_ROC_FAILURE                (-111)  
#define SL_ERROR_CON_MGMT_STATUS_MIC_FAILURE                (-112)  
/* end of WLAN connection management error statuses */
#define SL_ERROR_WAKELOCK_ERROR_PREFIX                      (-115)  /* Wake lock expired */
#define SL_ERROR_LENGTH_ERROR_PREFIX                        (-116)  /* Uart header length error */
#define SL_ERROR_MDNS_CREATE_FAIL                           (-121)  /* mDNS create failed */
#define SL_ERROR_GENERAL_ERROR                              (-127)



#define SL_DEVICE_GENERAL_CONFIGURATION           (1)
#define SL_DEVICE_GENERAL_CONFIGURATION_DATE_TIME (11)
#define SL_DEVICE_GENERAL_VERSION                 (12)
#define SL_DEVICE_STATUS                          (2)

/*
    Declare the different event group classifications
    The SimpleLink device send asynchronous events. Each event has a group 
    classification according to its nature.
*/
/* SL_EVENT_CLASS_WLAN connection user events */
#define SL_WLAN_CONNECT_EVENT                     (1)
#define SL_WLAN_DISCONNECT_EVENT                  (2)
/* WLAN Smart Config user events */
#define SL_WLAN_SMART_CONFIG_COMPLETE_EVENT       (3)
#define SL_WLAN_SMART_CONFIG_STOP_EVENT           (4)
/* WLAN AP user events */
#define SL_WLAN_STA_CONNECTED_EVENT               (5)
#define SL_WLAN_STA_DISCONNECTED_EVENT            (6)
/* WLAN P2P user events */
#define SL_WLAN_P2P_DEV_FOUND_EVENT               (7)
#define    SL_WLAN_P2P_NEG_REQ_RECEIVED_EVENT     (8)
#define SL_WLAN_CONNECTION_FAILED_EVENT           (9)
/* SL_EVENT_CLASS_DEVICE user events */
#define SL_DEVICE_FATAL_ERROR_EVENT               (1)
#define SL_DEVICE_ABORT_ERROR_EVENT               (2)

/* SL_EVENT_CLASS_BSD user events */               
#define    SL_SOCKET_TX_FAILED_EVENT              (1) 
#define SL_SOCKET_ASYNC_EVENT                     (2)
/* SL_EVENT_CLASS_NETAPP user events */   
#define    SL_NETAPP_IPV4_IPACQUIRED_EVENT        (1)
#define    SL_NETAPP_IPV6_IPACQUIRED_EVENT        (2)
#define SL_NETAPP_IP_LEASED_EVENT                 (3)
#define SL_NETAPP_IP_RELEASED_EVENT               (4)

/* Server Events */
#define SL_NETAPP_HTTPGETTOKENVALUE_EVENT          (1)
#define SL_NETAPP_HTTPPOSTTOKENVALUE_EVENT         (2)


/*
    Declare the different event group classifications for sl_DevGet
    for getting status indications
 */

/* Events list to mask/unmask*/
#define SL_EVENT_CLASS_GLOBAL                     (0)
#define SL_EVENT_CLASS_DEVICE                     (1)
#define SL_EVENT_CLASS_WLAN                       (2)
#define SL_EVENT_CLASS_BSD                        (3)
#define SL_EVENT_CLASS_NETAPP                     (4)
#define SL_EVENT_CLASS_NETCFG                     (5)
#define SL_EVENT_CLASS_FS                         (6)

  
/******************  DEVICE CLASS status ****************/
#define EVENT_DROPPED_DEVICE_ASYNC_GENERAL_ERROR          (0x00000001L)
#define STATUS_DEVICE_SMART_CONFIG_ACTIVE                 (0x80000000L)
  
/******************  WLAN CLASS status ****************/
#define EVENT_DROPPED_WLAN_WLANASYNCONNECTEDRESPONSE      (0x00000001L)
#define EVENT_DROPPED_WLAN_WLANASYNCDISCONNECTEDRESPONSE  (0x00000002L)
#define EVENT_DROPPED_WLAN_STA_CONNECTED                  (0x00000004L)
#define EVENT_DROPPED_WLAN_STA_DISCONNECTED               (0x00000008L)
#define STATUS_WLAN_STA_CONNECTED                         (0x80000000L)
                      
/******************  NETAPP CLASS status ****************/
#define EVENT_DROPPED_NETAPP_IPACQUIRED                   (0x00000001L)
#define EVENT_DROPPED_NETAPP_IPACQUIRED_V6                (0x00000002L)
#define EVENT_DROPPED_NETAPP_IP_LEASED                    (0x00000004L)
#define EVENT_DROPPED_NETAPP_IP_RELEASED                  (0x00000008L)
                      
/******************  BSD CLASS status ****************/
#define EVENT_DROPPED_SOCKET_TXFAILEDASYNCRESPONSE        (0x00000001L)
  
/******************  FS CLASS  ****************/
  


/*****************************************************************************/
/* Structure/Enum declarations                                               */
/*****************************************************************************/

#define ROLE_UNKNOWN_ERR                      (-1)

#ifdef SL_IF_TYPE_UART
typedef struct  
{
    _u32             BaudRate;
    _u8              FlowControlEnable;
    _u8              CommPort;
} SlUartIfParams_t;
#endif

typedef struct
{
    _u32               ChipId;
    _u32               FwVersion[4];
    _u8                PhyVersion[4];
}_SlPartialVersion;

typedef struct
{
    _SlPartialVersion ChipFwAndPhyVersion;
    _u32               NwpVersion[4];
    _u16               RomVersion;
    _u16               Padding;
}SlVersionFull;


typedef struct
{
    _u32 				AbortType;
    _u32				AbortData;
}sl_DeviceReportAbort;


typedef struct
{
    _i8                status;
    SlErrorSender_e        sender;
}sl_DeviceReport;

typedef union
{
  sl_DeviceReport           deviceEvent; 
  sl_DeviceReportAbort		deviceReport;  
} _SlDeviceEventData_u;

typedef struct
{
   _u32                 Event;
   _SlDeviceEventData_u EventData;
} SlDeviceEvent_t;

typedef struct  
{
       /* time */
    _u32                sl_tm_sec;
    _u32                sl_tm_min;
    _u32                sl_tm_hour;
       /* date */
    _u32                sl_tm_day; /* 1-31 */
    _u32                sl_tm_mon; /* 1-12 */
    _u32                sl_tm_year; /*  YYYY 4 digits  */
    _u32                sl_tm_week_day; /* not required */
    _u32                sl_tm_year_day; /* not required */ 
    _u32                reserved[3];  
}SlDateTime_t;


/******************************************************************************/
/* Type declarations                                                          */
/******************************************************************************/
typedef void (*P_INIT_CALLBACK)(_u32 Status);

/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/

/*!
    \brief Start the SimpleLink device
    
    This function initialize the communication interface, set the enable pin 
    of the device, and call to the init complete callback.

    \param[in]      pIfHdl              Opened Interface Object. In case the interface 
                                        must be opened outside the SimpleLink Driver, the
                                        user might give the handler to be used in \n
                                        any access of the communication interface with the 
                                        device (UART/SPI). \n
                                        The SimpleLink driver will open an interface port
                                        only if this parameter is null! \n
    \param[in]      pDevName            The name of the device to open. Could be used when 
                                        the pIfHdl is null, to transfer information to the 
                                        open interface function \n
                                        This pointer could be used to pass additional information to
                                        sl_IfOpen in case it is required (e.g. UART com port name)
    \param[in]      pInitCallBack       Pointer to function that would be called
                                        on completion of the initialization process.\n
                                        If this parameter is NULL the function is 
                                        blocked until the device initialization 
                                        is completed, otherwise the function returns 
                                        immediately.

    \return         Returns the current active role (STA/AP/P2P) or an error code:
                    - ROLE_STA, ROLE_AP, ROLE_P2P in case of success, 
                      otherwise in failure one of the following is return:
                    - ROLE_STA_ERR  (Failure to load MAC/PHY in STA role)
                    - ROLE_AP_ERR  (Failure to load MAC/PHY in AP role)
                    - ROLE_P2P_ERR  (Failure to load MAC/PHY in P2P role)

    
     \sa             sl_Stop

    \note           belongs to \ref basic_api

    \warning        This function must be called before any other SimpleLink API is used, or after sl_Stop is called for reinit the device
    \par            Example:
    \code            
                   An example for open interface without callback routine. The interface name and handler are
                   handled by the sl_IfOpen routine: 

                    if( sl_Start(NULL, NULL, NULL) < 0 )
                   {
                       LOG("Error opening interface to device\n"); 
                   }
    \endcode
*/
#if _SL_INCLUDE_FUNC(sl_Start)
_i16 sl_Start(const void* pIfHdl, _i8*  pDevName, const P_INIT_CALLBACK pInitCallBack);
#endif

/*!
    \brief Stop the SimpleLink device

    This function clears the enable pin of the device, closes the communication \n
    interface and invokes the stop complete callback 

    \param[in]      timeout                       Stop timeout in msec. Should be used to give the device time to finish \n 
                                                  any transmission/reception that is not completed when the function was called. \n
                    Additional options:
                    - 0                             Enter to hibernate immediately \n
                    - 0xFFFF                        Host waits for device's response before \n
                                                    hibernating, without timeout protection \n      
                    - 0 < Timeout[msec] < 0xFFFF    Host waits for device's response before \n
                                                    hibernating, with a defined timeout protection \n
                                                    This timeout defines the max time to wait. The NWP \n
                                                    response can be sent earlier than this timeout.

    \return         On success, zero is returned. On error, -1 is returned     
 
    \sa             sl_Start

    \note           This API will shutdown the device and invoke the "i/f close" function regardless \n
                    if it was opened implicitly or explicitly. \n
                    It is up to the platform interface library to properly handle interface close \n
                    routine \n
                    belongs to \ref basic_api \n
    \warning     
*/
#if _SL_INCLUDE_FUNC(sl_Stop)
_i16 sl_Stop(const _u16 timeout);
#endif


/*!
    \brief     Internal function for setting device configurations

    \return    On success, zero is returned. On error, -1 is 
               returned
   
    \param[in] DeviceSetId   configuration id
    \param[in] Option         configurations option
    \param[in] ConfigLen     configurations len
    \param[in] pValues         configurations values

    \sa         
    \note 
    \warning     
    \par   Examples:
    \code
         Setting device time and date example:

         SlDateTime_t dateTime= {0};
         dateTime.sl_tm_day =   (_u32)23;          // Day of month (DD format) range 1-31
         dateTime.sl_tm_mon =   (_u32)6;           // Month (MM format) in the range of 1-12 
         dateTime.sl_tm_year =  (_u32)2014;        // Year (YYYY format) 
         dateTime.sl_tm_hour =  (_u32)17;          // Hours in the range of 0-23
         dateTime.sl_tm_min =   (_u32)55;          // Minutes in the range of 0-59
         dateTime.sl_tm_sec =   (_u32)22;          // Seconds in the range of  0-59
         sl_DevSet(SL_DEVICE_GENERAL_CONFIGURATION,
                   SL_DEVICE_GENERAL_CONFIGURATION_DATE_TIME,
                   sizeof(SlDateTime_t),
                   (_u8 *)(&dateTime));

    \endcode
*/
#if _SL_INCLUDE_FUNC(sl_DevSet)
_i32 sl_DevSet(const _u8 DeviceSetId ,const _u8 Option,const _u8 ConfigLen,const _u8 *pValues);
#endif

/*!
    \brief      Internal function for getting device configurations
    \return     On success, zero is returned. On error, -1 is 
                returned
    \param[in]  DeviceGetId   configuration id - example SL_DEVICE_STATUS
    \param[out] pOption   Get configurations option, example for get status options
                            - SL_EVENT_CLASS_GLOBAL
                            - SL_EVENT_CLASS_DEVICE
                            - SL_EVENT_CLASS_WLAN  
                            - SL_EVENT_CLASS_BSD   
                            - SL_EVENT_CLASS_NETAPP
                            - SL_EVENT_CLASS_NETCFG
                            - SL_EVENT_CLASS_FS 
    \param[out] pConfigLen   The length of the allocated memory as input, when the
                             function complete, the value of this parameter would be
                             the len that actually read from the device.\n 
                             If the device return length that is longer from the input 
                             value, the function will cut the end of the returned structure
                             and will return SL_ESMALLBUF
    \param[out] pValues      Get configurations values
    \sa         
    \note 
    \warning  
    \par        Examples:
    \code
     Example for getting WLAN class status:
     _u32 statusWlan;
     _u8 pConfigOpt;
     _u8 pConfigLen;
     pConfigOpt = SL_EVENT_CLASS_WLAN;
     pConfigLen = sizeof(_u32);
     sl_DevGet(SL_DEVICE_STATUS,&pConfigOpt,&pConfigLen,(_u8 *)(&statusWlan));
     Example for getting version:
     SlVersionFull ver;
     pConfigLen = sizeof(ver);
     pConfigOpt = SL_DEVICE_GENERAL_VERSION;
     sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION,&pConfigOpt,&pConfigLen,(_u8 *)(&ver));
     printf("CHIP %d\nMAC 31.%d.%d.%d.%d\nPHY %d.%d.%d.%d\nNWP %d.%d.%d.%d\nROM %d\nHOST %d.%d.%d.%d\n",
             ver.ChipFwAndPhyVersion.ChipId,
             ver.ChipFwAndPhyVersion.FwVersion[0],ver.ChipFwAndPhyVersion.FwVersion[1],
             ver.ChipFwAndPhyVersion.FwVersion[2],ver.ChipFwAndPhyVersion.FwVersion[3],
             ver.ChipFwAndPhyVersion.PhyVersion[0],ver.ChipFwAndPhyVersion.PhyVersion[1],
             ver.ChipFwAndPhyVersion.PhyVersion[2],ver.ChipFwAndPhyVersion.PhyVersion[3],
             ver.NwpVersion[0],ver.NwpVersion[1],ver.NwpVersion[2],ver.NwpVersion[3],
             ver.RomVersion,
             SL_MAJOR_VERSION_NUM,SL_MINOR_VERSION_NUM,SL_VERSION_NUM,SL_SUB_VERSION_NUM);

     \endcode
     \code
         Getting Device time and date example:
         
         SlDateTime_t dateTime =  {0};  
         _i8 configLen = sizeof(SlDateTime_t); 
         _i8 configOpt = SL_DEVICE_GENERAL_CONFIGURATION_DATE_TIME;
         sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION,&configOpt, &configLen,(_u8 *)(&dateTime)); 
         
         printf("Day %d,Mon %d,Year %d,Hour %,Min %d,Sec %d\n",dateTime.sl_tm_day,dateTime.sl_tm_mon,dateTime.sl_tm_year
                 dateTime.sl_tm_hour,dateTime.sl_tm_min,dateTime.sl_tm_sec);
     \endcode
*/
#if _SL_INCLUDE_FUNC(sl_DevGet)
_i32 sl_DevGet(const _u8 DeviceGetId,_u8 *pOption,_u8 *pConfigLen, _u8 *pValues);
#endif


/*!
    \brief          Set asynchronous event mask
    
    Mask asynchronous events from the device. Masked events do not
    generate asynchronous messages from the device. 
    By default - all events are active
    
    \param[in]      EventClass          The classification groups that the 
                                        mask is referred to. Need to be one of
                                        the following:
                                        - SL_EVENT_CLASS_GLOBAL
                                        - SL_EVENT_CLASS_DEVICE
                                        - SL_EVENT_CLASS_WLAN  
                                        - SL_EVENT_CLASS_BSD   
                                        - SL_EVENT_CLASS_NETAPP
                                        - SL_EVENT_CLASS_NETCFG
                                        - SL_EVENT_CLASS_FS 


    \param[in]      Mask               Event Mask bitmap. Valid mask are (per group):
                                        - SL_EVENT_CLASS_WLAN user events
                                          - SL_WLAN_CONNECT_EVENT                     
                                          - SL_WLAN_DISCONNECT_EVENT
                                        - SL_EVENT_CLASS_DEVICE user events
                                          - SL_DEVICE_FATAL_ERROR_EVENT
                                        - SL_EVENT_CLASS_BSD user events
                                          - SL_SOCKET_TX_FAILED_EVENT     
                                          - SL_SOCKET_ASYNC_EVENT 
                                        - SL_EVENT_CLASS_NETAPP user events
                                          - SL_NETAPP_IPV4_IPACQUIRED_EVENT 
                                          - SL_NETAPP_IPV6_IPACQUIRED_EVENT
     
    \return         On success, zero is returned. On error, -1 is returned       

    \sa             sl_EventMaskGet

    \note           belongs to \ref ext_api
     
    \warning     
    \par           Example:
    \code          

    An example of masking connection/disconnection async events from WLAN class:
                   sl_EventMaskSet(SL_EVENT_CLASS_WLAN, (SL_WLAN_CONNECT_EVENT | SL_WLAN_DISCONNECT_EVENT) );
    
    \endcode
*/
#if _SL_INCLUDE_FUNC(sl_EventMaskSet)
_i16 sl_EventMaskSet(const _u8 EventClass ,const _u32 Mask);
#endif

/*!
    \brief Get current event mask of the device
    
    return the events bit mask from the device. In case that event is 
    masked, the device is not sending this event.  
    
    \param[in]      EventClass          The classification groups that the 
                                        mask is referred to. Need to be one of
                                        the following:
                                        - SL_EVENT_CLASS_GLOBAL
                                        - SL_EVENT_CLASS_DEVICE
                                        - SL_EVENT_CLASS_WLAN  
                                        - SL_EVENT_CLASS_BSD   
                                        - SL_EVENT_CLASS_NETAPP
                                        - SL_EVENT_CLASS_NETCFG
                                        - SL_EVENT_CLASS_FS 

    \param[out]      pMask              Pointer to Mask bitmap where the 
                                        value should be stored. Bitmasks are the same as in \ref sl_EventMaskSet
     
    \return         On success, zero is returned. On error, -1 is returned       
     
    \sa             sl_EventMaskSet         

    \note           belongs to \ref ext_api

    \warning     
    \par           Example:
    \code          

    An example of getting an event mask for WLAN class
    _u32 maskWlan;
                   sl_StatusGet(SL_EVENT_CLASS_WLAN,&maskWlan);

   \endcode
*/
#if _SL_INCLUDE_FUNC(sl_EventMaskGet)
_i16 sl_EventMaskGet(const _u8 EventClass,_u32 *pMask);
#endif


/*!
    \brief the simple link task entry

    \Param
    This function must be called from the main loop or from dedicated thread in
    the following cases:
        - Non-Os Platform - should be called from the mail loop
        - Multi Threaded Platform when the user does not implement the external spawn functions - 
           should be called from dedicated thread allocated to the simplelink driver.
           In this mode the function never return.
    
    \return         None
    
    \sa             sl_Stop

    \note           belongs to \ref basic_api

    \warning        This function must be called from a thread that is start running before 
                    any call to other simple link API
*/
#if _SL_INCLUDE_FUNC(sl_Task)
void sl_Task(void);
#endif


/*!
    \brief Setting the internal uart mode 

    \param[in]      pUartParams          Pointer to the uart configuration parameter set: 
                                         baudrate     - up to 711 Kbps
                                         flow control - enable/disable 
                                         comm port    - the comm port number
    
    \return         On success zero is returned, otherwise - Failed.   
    
    \sa             sl_Stop

    \note           belongs to \ref basic_api

    \warning        This function must consider the host uart capability
*/
#ifdef SL_IF_TYPE_UART
#if _SL_INCLUDE_FUNC(sl_UartSetMode)
_i16 sl_UartSetMode(const SlUartIfParams_t* pUartParams);
#endif
#endif

/*!

 Close the Doxygen group.
 @}

 */


#ifdef  __cplusplus
}
#endif /*  __cplusplus */

#endif  /*  __DEVICE_H__ */


