/**
  ******************************************************************************
  * @file    usbh_cdc.h
  * @author  MCD Application Team
  * @version V3.0.0
  * @date    18-February-2014
  * @brief   This file contains all the prototypes for the usbh_cdc.c
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive  ----------------------------------------------*/
#ifndef __USBH_CDC_CORE_H
#define __USBH_CDC_CORE_H

/* Includes ------------------------------------------------------------------*/
#include "usbh_core.h"


/** @addtogroup USBH_LIB
* @{
*/

/** @addtogroup USBH_CLASS
* @{
*/

/** @addtogroup USBH_CDC_CLASS
* @{
*/

/** @defgroup USBH_CDC_CORE
* @brief This file is the Header file for USBH_CDC_CORE.c
* @{
*/ 




/*Communication Class codes*/
#define USB_CDC_CLASS                                           0x02
#define COMMUNICATION_INTERFACE_CLASS_CODE                      0x02

/*Data Interface Class Codes*/
#define DATA_INTERFACE_CLASS_CODE                               0x0A

/*Communcation sub class codes*/
#define RESERVED                                                0x00
#define DIRECT_LINE_CONTROL_MODEL                               0x01
#define ABSTRACT_CONTROL_MODEL                                  0x02
#define TELEPHONE_CONTROL_MODEL                                 0x03
#define MULTICHANNEL_CONTROL_MODEL                              0x04   
#define CAPI_CONTROL_MODEL                                      0x05
#define ETHERNET_NETWORKING_CONTROL_MODEL                       0x06
#define ATM_NETWORKING_CONTROL_MODEL                            0x07


/*Communication Interface Class Control Protocol Codes*/
#define NO_CLASS_SPECIFIC_PROTOCOL_CODE                         0x00
#define COMMON_AT_COMMAND                                       0x01
#define VENDOR_SPECIFIC                                         0xFF


#define CS_INTERFACE                                            0x24
#define CDC_PAGE_SIZE_64                                        0x40

/*Class-Specific Request Codes*/
#define CDC_SEND_ENCAPSULATED_COMMAND                           0x00
#define CDC_GET_ENCAPSULATED_RESPONSE                           0x01
#define CDC_SET_COMM_FEATURE                                    0x02
#define CDC_GET_COMM_FEATURE                                    0x03
#define CDC_CLEAR_COMM_FEATURE                                  0x04

#define CDC_SET_AUX_LINE_STATE                                  0x10
#define CDC_SET_HOOK_STATE                                      0x11
#define CDC_PULSE_SETUP                                         0x12
#define CDC_SEND_PULSE                                          0x13
#define CDC_SET_PULSE_TIME                                      0x14
#define CDC_RING_AUX_JACK                                       0x15

#define CDC_SET_LINE_CODING                                     0x20
#define CDC_GET_LINE_CODING                                     0x21
#define CDC_SET_CONTROL_LINE_STATE                              0x22
#define CDC_SEND_BREAK                                          0x23

#define CDC_SET_RINGER_PARMS                                    0x30
#define CDC_GET_RINGER_PARMS                                    0x31
#define CDC_SET_OPERATION_PARMS                                 0x32
#define CDC_GET_OPERATION_PARMS                                 0x33  
#define CDC_SET_LINE_PARMS                                      0x34
#define CDC_GET_LINE_PARMS                                      0x35
#define CDC_DIAL_DIGITS                                         0x36
#define CDC_SET_UNIT_PARAMETER                                  0x37  
#define CDC_GET_UNIT_PARAMETER                                  0x38
#define CDC_CLEAR_UNIT_PARAMETER                                0x39
#define CDC_GET_PROFILE                                         0x3A

#define CDC_SET_ETHERNET_MULTICAST_FILTERS                      0x40
#define CDC_SET_ETHERNET_POWER_MANAGEMENT_PATTERN FILTER        0x41
#define CDC_GET_ETHERNET_POWER_MANAGEMENT_PATTERN FILTER        0x42
#define CDC_SET_ETHERNET_PACKET_FILTER                          0x43
#define CDC_GET_ETHERNET_STATISTIC                              0x44

#define CDC_SET_ATM_DATA_FORMAT                                 0x50  
#define CDC_GET_ATM_DEVICE_STATISTICS                           0x51
#define CDC_SET_ATM_DEFAULT_VC                                  0x52
#define CDC_GET_ATM_VC_STATISTICS                               0x53


/* wValue for SetControlLineState*/
#define CDC_ACTIVATE_CARRIER_SIGNAL_RTS                         0x0002
#define CDC_DEACTIVATE_CARRIER_SIGNAL_RTS                       0x0000
#define CDC_ACTIVATE_SIGNAL_DTR                                 0x0001
#define CDC_DEACTIVATE_SIGNAL_DTR                               0x0000

#define LINE_CODING_STRUCTURE_SIZE                              0x07
/**
  * @}
  */ 

/** @defgroup USBH_CDC_CORE_Exported_Types
* @{
*/ 

/* States for CDC State Machine */
typedef enum
{
  CDC_IDLE= 0,
  CDC_SEND_DATA,
  CDC_SEND_DATA_WAIT,
  CDC_RECEIVE_DATA,
  CDC_RECEIVE_DATA_WAIT,  
}
CDC_DataStateTypeDef;

typedef enum
{
  CDC_IDLE_STATE= 0,
  CDC_SET_LINE_CODING_STATE,  
  CDC_GET_LAST_LINE_CODING_STATE,    
  CDC_TRANSFER_DATA, 
  CDC_ERROR_STATE,  
}
CDC_StateTypeDef;


/*Line coding structure*/
typedef union _CDC_LineCodingStructure
{
  uint8_t Array[LINE_CODING_STRUCTURE_SIZE];
  
  struct
  {
    
    uint32_t             dwDTERate;     /*Data terminal rate, in bits per second*/
    uint8_t              bCharFormat;   /*Stop bits
    0 - 1 Stop bit
    1 - 1.5 Stop bits
    2 - 2 Stop bits*/
    uint8_t              bParityType;   /* Parity
    0 - None
    1 - Odd
    2 - Even
    3 - Mark
    4 - Space*/
    uint8_t                bDataBits;     /* Data bits (5, 6, 7, 8 or 16). */
  }b;
}
CDC_LineCodingTypeDef;



/* Header Functional Descriptor
--------------------------------------------------------------------------------
Offset|  field              | Size  |    Value   |   Description                    
------|---------------------|-------|------------|------------------------------
0     |  bFunctionLength    | 1     |   number   |  Size of this descriptor.
1     |  bDescriptorType    | 1     |   Constant |  CS_INTERFACE (0x24)
2     |  bDescriptorSubtype | 1     |   Constant |  Identifier (ID) of functional 
      |                     |       |            | descriptor.
3     |  bcdCDC             | 2     |            |
      |                     |       |   Number   | USB Class Definitions for 
      |                     |       |            | Communication Devices Specification 
      |                     |       |            | release number in binary-coded 
      |                     |       |            | decimal
------|---------------------|-------|------------|------------------------------
*/
typedef struct _FunctionalDescriptorHeader
{
  uint8_t     bLength;            /*Size of this descriptor.*/
  uint8_t     bDescriptorType;    /*CS_INTERFACE (0x24)*/
  uint8_t     bDescriptorSubType; /* Header functional descriptor subtype as*/
  uint16_t    bcdCDC;             /* USB Class Definitions for Communication
                                    Devices Specification release number in
                                  binary-coded decimal. */
}
CDC_HeaderFuncDesc_TypeDef;
/* Call Management Functional Descriptor
--------------------------------------------------------------------------------
Offset|  field              | Size  |    Value   |   Description                    
------|---------------------|-------|------------|------------------------------
0     |  bFunctionLength    | 1     |   number   |  Size of this descriptor.
1     |  bDescriptorType    | 1     |   Constant |  CS_INTERFACE (0x24)
2     |  bDescriptorSubtype | 1     |   Constant |  Call Management functional 
      |                     |       |            |  descriptor subtype.
3     |  bmCapabilities     | 1     |   Bitmap   | The capabilities that this configuration
      |                     |       |            | supports:
      |                     |       |            | D7..D2: RESERVED (Reset to zero)
      |                     |       |            | D1: 0 - Device sends/receives call
      |                     |       |            | management information only over
      |                     |       |            | the Communication Class
      |                     |       |            | interface.
      |                     |       |            | 1 - Device can send/receive call
      |                     \       |            | management information over a
      |                     |       |            | Data Class interface.
      |                     |       |            | D0: 0 - Device does not handle call
      |                     |       |            | management itself.
      |                     |       |            | 1 - Device handles call
      |                     |       |            | management itself.
      |                     |       |            | The previous bits, in combination, identify
      |                     |       |            | which call management scenario is used. If bit
      |                     |       |            | D0 is reset to 0, then the value of bit D1 is
      |                     |       |            | ignored. In this case, bit D1 is reset to zero for
      |                     |       |            | future compatibility.
4     | bDataInterface      | 1     | Number     | Interface number of Data Class interface
      |                     |       |            | optionally used for call management.        
------|---------------------|-------|------------|------------------------------
*/
typedef struct _CallMgmtFunctionalDescriptor
{
  uint8_t    bLength;            /*Size of this functional descriptor, in bytes.*/
  uint8_t    bDescriptorType;    /*CS_INTERFACE (0x24)*/
  uint8_t    bDescriptorSubType; /* Call Management functional descriptor subtype*/
  uint8_t    bmCapabilities;      /* bmCapabilities: D0+D1 */
  uint8_t    bDataInterface;      /*bDataInterface: 1*/
}
CDC_CallMgmtFuncDesc_TypeDef;
/* Abstract Control Management Functional Descriptor
--------------------------------------------------------------------------------
Offset|  field              | Size  |    Value   |   Description                    
------|---------------------|-------|------------|------------------------------
0     |  bFunctionLength    | 1     |   number   |  Size of functional descriptor, in bytes.
1     |  bDescriptorType    | 1     |   Constant |  CS_INTERFACE (0x24)
2     |  bDescriptorSubtype | 1     |   Constant |  Abstract Control Management 
      |                     |       |            |  functional  descriptor subtype.
3     |  bmCapabilities     | 1     |   Bitmap   | The capabilities that this configuration
      |                     |       |            | supports ((A bit value of zero means that the
      |                     |       |            | request is not supported.) )
                                                   D7..D4: RESERVED (Reset to zero)
      |                     |       |            | D3: 1 - Device supports the notification
      |                     |       |            | Network_Connection.
      |                     |       |            | D2: 1 - Device supports the request
      |                     |       |            | Send_Break
      |                     |       |            | D1: 1 - Device supports the request
      |                     \       |            | combination of Set_Line_Coding,
      |                     |       |            | Set_Control_Line_State, Get_Line_Coding, and the
                                                   notification Serial_State.
      |                     |       |            | D0: 1 - Device supports the request
      |                     |       |            | combination of Set_Comm_Feature,
      |                     |       |            | Clear_Comm_Feature, and Get_Comm_Feature.
      |                     |       |            | The previous bits, in combination, identify
      |                     |       |            | which requests/notifications are supported by
      |                     |       |            | a Communication Class interface with the
      |                     |       |            |   SubClass code of Abstract Control Model.
------|---------------------|-------|------------|------------------------------
*/
typedef struct _AbstractCntrlMgmtFunctionalDescriptor
{
  uint8_t    bLength;            /*Size of this functional descriptor, in bytes.*/
  uint8_t    bDescriptorType;    /*CS_INTERFACE (0x24)*/
  uint8_t    bDescriptorSubType; /* Abstract Control Management functional
                                  descriptor subtype*/
  uint8_t    bmCapabilities;      /* The capabilities that this configuration supports */
}
CDC_AbstCntrlMgmtFuncDesc_TypeDef;
/* Union Functional Descriptor
--------------------------------------------------------------------------------
Offset|  field              | Size  |    Value   |   Description                    
------|---------------------|-------|------------|------------------------------
0     |  bFunctionLength    | 1     |   number   |  Size of this descriptor.
1     |  bDescriptorType    | 1     |   Constant |  CS_INTERFACE (0x24)
2     |  bDescriptorSubtype | 1     |   Constant |  Union functional 
      |                     |       |            |  descriptor subtype.
3     |  bMasterInterface   | 1     |   Constant | The interface number of the 
      |                     |       |            | Communication or Data Class interface
4     | bSlaveInterface0    | 1     | Number     | nterface number of first slave or associated
      |                     |       |            | interface in the union.        
------|---------------------|-------|------------|------------------------------
*/
typedef struct _UnionFunctionalDescriptor
{
  uint8_t    bLength;            /*Size of this functional descriptor, in bytes*/
  uint8_t    bDescriptorType;    /*CS_INTERFACE (0x24)*/
  uint8_t    bDescriptorSubType; /* Union functional descriptor SubType*/
  uint8_t    bMasterInterface;   /* The interface number of the Communication or
                                 Data Class interface,*/
  uint8_t    bSlaveInterface0;   /*Interface number of first slave*/
}
CDC_UnionFuncDesc_TypeDef;

typedef struct _USBH_CDCInterfaceDesc
{
  CDC_HeaderFuncDesc_TypeDef           CDC_HeaderFuncDesc;
  CDC_CallMgmtFuncDesc_TypeDef         CDC_CallMgmtFuncDesc;
  CDC_AbstCntrlMgmtFuncDesc_TypeDef    CDC_AbstCntrlMgmtFuncDesc;
  CDC_UnionFuncDesc_TypeDef            CDC_UnionFuncDesc;  
}
CDC_InterfaceDesc_Typedef;


/* Structure for CDC process */
typedef struct
{
  uint8_t              NotifPipe; 
  uint8_t              NotifEp;
  uint8_t              buff[8];
  uint16_t             NotifEpSize;
}
CDC_CommItfTypedef ;

typedef struct
{
  uint8_t              InPipe; 
  uint8_t              OutPipe;
  uint8_t              OutEp;
  uint8_t              InEp;
  uint8_t              buff[8];
  uint16_t             OutEpSize;
  uint16_t             InEpSize;  
}
CDC_DataItfTypedef ;

/* Structure for CDC process */
typedef struct _CDC_Process
{
  CDC_CommItfTypedef                CommItf;
  CDC_DataItfTypedef                DataItf;
  uint8_t                           *pTxData;
  uint8_t                           *pRxData; 
  uint32_t                           TxDataLength;
  uint32_t                           RxDataLength;  
  CDC_InterfaceDesc_Typedef         CDC_Desc;
  CDC_LineCodingTypeDef             LineCoding;
  CDC_LineCodingTypeDef             *pUserLineCoding;  
  CDC_StateTypeDef                  state;
  CDC_DataStateTypeDef              data_tx_state;
  CDC_DataStateTypeDef              data_rx_state; 
  uint8_t                           Rx_Poll;
}
CDC_HandleTypeDef;

/**
* @}
*/ 

/** @defgroup USBH_CDC_CORE_Exported_Defines
* @{
*/ 

/**
* @}
*/ 

/** @defgroup USBH_CDC_CORE_Exported_Macros
* @{
*/ 
/**
* @}
*/ 

/** @defgroup USBH_CDC_CORE_Exported_Variables
* @{
*/ 
extern USBH_ClassTypeDef  CDC_Class;
#define USBH_CDC_CLASS    &CDC_Class

/**
* @}
*/ 

/** @defgroup USBH_CDC_CORE_Exported_FunctionsPrototype
* @{
*/ 

USBH_StatusTypeDef  USBH_CDC_SetLineCoding(USBH_HandleTypeDef *phost, 
                                           CDC_LineCodingTypeDef *linecoding);

USBH_StatusTypeDef  USBH_CDC_GetLineCoding(USBH_HandleTypeDef *phost, 
                                           CDC_LineCodingTypeDef *linecoding);

USBH_StatusTypeDef  USBH_CDC_Transmit(USBH_HandleTypeDef *phost, 
                                      uint8_t *pbuff, 
                                      uint32_t length);

USBH_StatusTypeDef  USBH_CDC_Receive(USBH_HandleTypeDef *phost, 
                                     uint8_t *pbuff, 
                                     uint32_t length);


uint16_t            USBH_CDC_GetLastReceivedDataSize(USBH_HandleTypeDef *phost);

USBH_StatusTypeDef  USBH_CDC_Stop(USBH_HandleTypeDef *phost);

void USBH_CDC_LineCodingChanged(USBH_HandleTypeDef *phost);

void USBH_CDC_TransmitCallback(USBH_HandleTypeDef *phost);

void USBH_CDC_ReceiveCallback(USBH_HandleTypeDef *phost);

/**
* @}
*/ 


#endif /* __USBH_CDC_CORE_H */

/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/ 
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

