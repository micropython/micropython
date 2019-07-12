/**
  ******************************************************************************
  * @file    usbh_mtp_ptp.h
  * @author  MCD Application Team
  * @version V3.0.0
  * @date    18-February-2014
  * @brief   Header file for usbh_mtp_ptp.c
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
#ifndef __USBH_MTP_PTP_H__
#define __USBH_MTP_PTP_H__

/* Includes ------------------------------------------------------------------*/
#include "usbh_core.h"

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_CLASS
  * @{
  */

/** @addtogroup USBH_MTP_PTP_CLASS
  * @{
  */
  
/** @defgroup USBH_MTP_PTP
  * @brief This file is the Header file for usbh_mtp_ptp.c
  * @{
  */ 


/* Operation Codes */

/* PTP v1.0 operation codes */
#define PTP_OC_Undefined                0x1000
#define PTP_OC_GetDeviceInfo            0x1001
#define PTP_OC_OpenSession              0x1002
#define PTP_OC_CloseSession             0x1003
#define PTP_OC_GetStorageIDs            0x1004
#define PTP_OC_GetStorageInfo           0x1005
#define PTP_OC_GetNumObjects            0x1006
#define PTP_OC_GetObjectHandles         0x1007
#define PTP_OC_GetObjectInfo            0x1008
#define PTP_OC_GetObject                0x1009
#define PTP_OC_GetThumb                 0x100A
#define PTP_OC_DeleteObject             0x100B
#define PTP_OC_SendObjectInfo           0x100C
#define PTP_OC_SendObject               0x100D
#define PTP_OC_InitiateCapture          0x100E
#define PTP_OC_FormatStore              0x100F
#define PTP_OC_ResetDevice              0x1010
#define PTP_OC_SelfTest                 0x1011
#define PTP_OC_SetObjectProtection      0x1012
#define PTP_OC_PowerDown                0x1013
#define PTP_OC_GetDevicePropDesc        0x1014
#define PTP_OC_GetDevicePropValue       0x1015
#define PTP_OC_SetDevicePropValue       0x1016
#define PTP_OC_ResetDevicePropValue     0x1017
#define PTP_OC_TerminateOpenCapture     0x1018
#define PTP_OC_MoveObject               0x1019
#define PTP_OC_CopyObject               0x101A
#define PTP_OC_GetPartialObject         0x101B
#define PTP_OC_InitiateOpenCapture      0x101C
    
/* PTP v1.1 operation codes */
#define PTP_OC_StartEnumHandles		0x101D
#define PTP_OC_EnumHandles		0x101E
#define PTP_OC_StopEnumHandles		0x101F
#define PTP_OC_GetVendorExtensionMaps	0x1020
#define PTP_OC_GetVendorDeviceInfo	0x1021
#define PTP_OC_GetResizedImageObject	0x1022
#define PTP_OC_GetFilesystemManifest	0x1023
#define PTP_OC_GetStreamInfo		0x1024
#define PTP_OC_GetStream		0x1025

    /* Microsoft / MTP extension codes */
#define PTP_OC_GetObjectPropsSupported	 0x9801
#define PTP_OC_GetObjectPropDesc	 0x9802
#define PTP_OC_GetObjectPropValue	 0x9803
#define PTP_OC_SetObjectPropValue	 0x9804
#define PTP_OC_GetObjPropList		 0x9805
#define PTP_OC_SetObjPropList		 0x9806
#define PTP_OC_GetInterdependendPropdesc 0x9807
#define PTP_OC_SendObjectPropList	 0x9808
#define PTP_OC_GetObjectReferences	 0x9810
#define PTP_OC_SetObjectReferences	 0x9811
#define PTP_OC_UpdateDeviceFirmware	 0x9812
#define PTP_OC_Skip			 0x9820

    
/* Response Codes */

/* PTP v1.0 response codes */
#define PTP_RC_Undefined                0x2000
#define PTP_RC_OK                       0x2001
#define PTP_RC_GeneralError             0x2002
#define PTP_RC_SessionNotOpen           0x2003
#define PTP_RC_InvalidTransactionID	0x2004
#define PTP_RC_OperationNotSupported    0x2005
#define PTP_RC_ParameterNotSupported    0x2006
#define PTP_RC_IncompleteTransfer       0x2007
#define PTP_RC_InvalidStorageId         0x2008
#define PTP_RC_InvalidObjectHandle      0x2009
#define PTP_RC_DevicePropNotSupported   0x200A
#define PTP_RC_InvalidObjectFormatCode  0x200B
#define PTP_RC_StoreFull                0x200C
#define PTP_RC_ObjectWriteProtected     0x200D
#define PTP_RC_StoreReadOnly            0x200E
#define PTP_RC_AccessDenied             0x200F
#define PTP_RC_NoThumbnailPresent       0x2010
#define PTP_RC_SelfTestFailed           0x2011
#define PTP_RC_PartialDeletion          0x2012
#define PTP_RC_StoreNotAvailable        0x2013
#define PTP_RC_SpecificationByFormatUnsupported         0x2014
#define PTP_RC_NoValidObjectInfo        0x2015
#define PTP_RC_InvalidCodeFormat        0x2016
#define PTP_RC_UnknownVendorCode        0x2017
#define PTP_RC_CaptureAlreadyTerminated 0x2018
#define PTP_RC_DeviceBusy               0x2019
#define PTP_RC_InvalidParentObject      0x201A
#define PTP_RC_InvalidDevicePropFormat  0x201B
#define PTP_RC_InvalidDevicePropValue   0x201C
#define PTP_RC_InvalidParameter         0x201D
#define PTP_RC_SessionAlreadyOpened     0x201E
#define PTP_RC_TransactionCanceled      0x201F
#define PTP_RC_SpecificationOfDestinationUnsupported       0x2020
/* PTP v1.1 response codes */
#define PTP_RC_InvalidEnumHandle	0x2021
#define PTP_RC_NoStreamEnabled		0x2022
#define PTP_RC_InvalidDataSet		0x2023
    
/* USB container types */

#define PTP_USB_CONTAINER_UNDEFINED	 0x0000
#define PTP_USB_CONTAINER_COMMAND	 0x0001
#define PTP_USB_CONTAINER_DATA		 0x0002
#define PTP_USB_CONTAINER_RESPONSE	 0x0003
#define PTP_USB_CONTAINER_EVENT		 0x0004

/* PTP/IP definitions */
#define PTPIP_INIT_COMMAND_REQUEST	1
#define PTPIP_INIT_COMMAND_ACK		2
#define PTPIP_INIT_EVENT_REQUEST	3
#define PTPIP_INIT_EVENT_ACK		4
#define PTPIP_INIT_FAIL			5
#define PTPIP_CMD_REQUEST		6
#define PTPIP_CMD_RESPONSE		7
#define PTPIP_EVENT			8
#define PTPIP_START_DATA_PACKET		9
#define PTPIP_DATA_PACKET		10
#define PTPIP_CANCEL_TRANSACTION	11
#define PTPIP_END_DATA_PACKET		12
#define PTPIP_PING			13
#define PTPIP_PONG			14

/* Transaction data phase description */
#define PTP_DP_NODATA		0x0000	/* no data phase */
#define PTP_DP_SENDDATA		0x0001	/* sending data */
#define PTP_DP_GETDATA		0x0002	/* receiving data */
#define PTP_DP_DATA_MASK	0x00ff	/* data phase mask */
    
/** @defgroup USBH_MTP_PTP_Exported_Types
  * @{
  */ 

typedef enum 
{
  PTP_REQ_IDLE = 0,   
  PTP_REQ_SEND = 1,  
  PTP_REQ_WAIT,   
  PTP_REQ_ERROR,         
} 
PTP_RequestStateTypeDef;  

typedef enum 
{
  PTP_IDLE = 0,   
  PTP_OP_REQUEST_STATE,  
  PTP_OP_REQUEST_WAIT_STATE,   
  PTP_DATA_OUT_PHASE_STATE,
  PTP_DATA_OUT_PHASE_WAIT_STATE,  
  PTP_DATA_IN_PHASE_STATE,
  PTP_DATA_IN_PHASE_WAIT_STATE,  
  PTP_RESPONSE_STATE, 
  PTP_RESPONSE_WAIT_STATE,  
  PTP_ERROR,         
} 
PTP_ProcessStateTypeDef;  

/* PTP request/response/event general PTP container (transport independent) */
typedef struct 
{
	uint16_t Code;
	uint32_t SessionID;
	uint32_t Transaction_ID;
	/* params  may be of any type of size less or equal to uint32_t */
	uint32_t Param1;
	uint32_t Param2;
	uint32_t Param3;
	/* events can only have three parameters */
	uint32_t Param4;
	uint32_t Param5;
	/* the number of meaningfull parameters */
	uint8_t	 Nparam;
}
PTP_ContainerTypedef; 

#define PTP_USB_BULK_HS_MAX_PACKET_LEN_WRITE	1024
#define PTP_USB_BULK_HS_MAX_PACKET_LEN_READ     1024
#define PTP_USB_BULK_HDR_LEN		       (2*sizeof(uint32_t)+2*sizeof(uint16_t))
#define PTP_USB_BULK_PAYLOAD_LEN_WRITE	       (PTP_USB_BULK_HS_MAX_PACKET_LEN_WRITE-PTP_USB_BULK_HDR_LEN)
#define PTP_USB_BULK_PAYLOAD_LEN_READ	       (PTP_USB_BULK_HS_MAX_PACKET_LEN_READ-PTP_USB_BULK_HDR_LEN)
#define PTP_USB_BULK_REQ_LEN	               (PTP_USB_BULK_HDR_LEN+5*sizeof(uint32_t))
#define PTP_USB_BULK_REQ_RESP_MAX_LEN           63

typedef struct 
{
  uint32_t length;
  uint16_t type;
  uint16_t code;
  uint32_t trans_id;
  uint32_t param1;
  uint32_t param2;
  uint32_t param3;
  uint32_t param4;
  uint32_t param5;
}
PTP_RespContainerTypedef;


typedef struct 
{
  uint32_t length;
  uint16_t type;
  uint16_t code;
  uint32_t trans_id;
  uint32_t param1;
  uint32_t param2;
  uint32_t param3;
  uint32_t param4;
  uint32_t param5;
}
PTP_OpContainerTypedef;

typedef struct 
{
  uint32_t length;
  uint16_t type;
  uint16_t code;
  uint32_t trans_id;
  union {
    struct {
      uint32_t param1;
      uint32_t param2;
      uint32_t param3;
      uint32_t param4;
      uint32_t param5;
    } params;  
    uint8_t  data[PTP_USB_BULK_PAYLOAD_LEN_READ];
  }payload;
}
PTP_DataContainerTypedef;

/* PTP USB Asynchronous Event Interrupt Data Format */
typedef struct 
{
  uint32_t length;
  uint16_t type;
  uint16_t code;
  uint32_t trans_id;
  uint32_t param1;
  uint32_t param2;
  uint32_t param3;
}
PTP_EventContainerTypedef;

/* Structure for PTP Transport process */
typedef struct
{
  PTP_ProcessStateTypeDef      state;
  PTP_RequestStateTypeDef      req_state;
  PTP_OpContainerTypedef       op_container;    
  PTP_DataContainerTypedef     data_container;  
  PTP_RespContainerTypedef     resp_container;    

  /* ptp transaction ID */
  uint32_t	transaction_id;
  
  /* ptp session ID */
  uint32_t	session_id; 
  
  /* device flags */
  uint32_t	flags;  

  /****** PTP transfer control *******/ 
  
  /* Data pointer */
  uint8_t	*data_ptr;  

  /* Data length */
  int32_t	data_length;
  
  /* Data length */
  uint32_t	data_packet;
  
  /* Data length */
  uint32_t	iteration;  
  
  /* Packet Index */
  uint32_t   data_packet_counter;
  
  /****** Object transfer control *******/
  
  /* object pointer */
  uint8_t	*object_ptr;

}
PTP_HandleTypeDef;

/* DeviceInfo data offset */
#define PTP_di_StandardVersion		 0
#define PTP_di_VendorExtensionID	 2
#define PTP_di_VendorExtensionVersion	 6
#define PTP_di_VendorExtensionDesc	 8
#define PTP_di_FunctionalMode		 8
#define PTP_di_OperationsSupported	10

/* Max info items size */
#define PTP_SUPPORTED_OPERATIONS_NBR    100
#define PTP_SUPPORTED_EVENTS_NBR        100
#define PTP_SUPPORTED_PROPRIETIES_NBR   100
#define PTP_CAPTURE_FORMATS_NBR         100
#define PTP_IMAGE_FORMATS_NBR           100
#define PTP_MAX_STR_SIZE                255
/* PTP device info structure */
typedef struct
{
  uint16_t StandardVersion;
  uint32_t VendorExtensionID;
  uint16_t VendorExtensionVersion;
  uint8_t  VendorExtensionDesc[PTP_MAX_STR_SIZE];
  uint16_t FunctionalMode;
  uint32_t OperationsSupported_len;
  uint16_t OperationsSupported[PTP_SUPPORTED_OPERATIONS_NBR];
  uint32_t EventsSupported_len;
  uint16_t EventsSupported[PTP_SUPPORTED_EVENTS_NBR];
  uint32_t DevicePropertiesSupported_len;
  uint16_t DevicePropertiesSupported[PTP_SUPPORTED_PROPRIETIES_NBR];
  uint32_t CaptureFormats_len;
  uint16_t CaptureFormats[PTP_CAPTURE_FORMATS_NBR];
  uint32_t ImageFormats_len;
  uint16_t ImageFormats[PTP_IMAGE_FORMATS_NBR];
  uint8_t  Manufacturer[PTP_MAX_STR_SIZE];
  uint8_t  Model[PTP_MAX_STR_SIZE];
  uint8_t  DeviceVersion[PTP_MAX_STR_SIZE];
  uint8_t  SerialNumber[PTP_MAX_STR_SIZE];
}
PTP_DeviceInfoTypedef;

#define PTP_MAX_STORAGE_UNITS_NBR      5
/* PTP storageIDs structute (returned by GetStorageIDs) */
typedef struct
{
  uint32_t n;
  uint32_t Storage [PTP_MAX_STORAGE_UNITS_NBR];
}
PTP_StorageIDsTypedef;

/* PTP StorageInfo structure (returned by GetStorageInfo) */

#define PTP_si_StorageType		 0
#define PTP_si_FilesystemType		 2
#define PTP_si_AccessCapability		 4
#define PTP_si_MaxCapability		 6
#define PTP_si_FreeSpaceInBytes		14
#define PTP_si_FreeSpaceInImages	22
#define PTP_si_StorageDescription	26


/* PTP Storage Types */

#define PTP_ST_Undefined			0x0000
#define PTP_ST_FixedROM				0x0001
#define PTP_ST_RemovableROM			0x0002
#define PTP_ST_FixedRAM				0x0003
#define PTP_ST_RemovableRAM			0x0004

/* PTP FilesystemType Values */

#define PTP_FST_Undefined			0x0000
#define PTP_FST_GenericFlat			0x0001
#define PTP_FST_GenericHierarchical		0x0002
#define PTP_FST_DCF				0x0003

/* PTP StorageInfo AccessCapability Values */

#define PTP_AC_ReadWrite			0x0000
#define PTP_AC_ReadOnly				0x0001
#define PTP_AC_ReadOnly_with_Object_Deletion	0x0002

typedef struct
{
  uint16_t StorageType;
  uint16_t FilesystemType;
  uint16_t AccessCapability;
  uint64_t MaxCapability;
  uint64_t FreeSpaceInBytes;
  uint32_t FreeSpaceInImages;
  uint8_t  StorageDescription[PTP_MAX_STR_SIZE];
  uint8_t  VolumeLabel[PTP_MAX_STR_SIZE];
}
PTP_StorageInfoTypedef;

/* PTP Object Format Codes */

/* ancillary formats */
#define PTP_OFC_Undefined			0x3000
#define PTP_OFC_Defined				0x3800
#define PTP_OFC_Association			0x3001
#define PTP_OFC_Script				0x3002
#define PTP_OFC_Executable			0x3003
#define PTP_OFC_Text				0x3004
#define PTP_OFC_HTML				0x3005
#define PTP_OFC_DPOF				0x3006
#define PTP_OFC_AIFF	 			0x3007
#define PTP_OFC_WAV				0x3008
#define PTP_OFC_MP3				0x3009
#define PTP_OFC_AVI				0x300A
#define PTP_OFC_MPEG				0x300B
#define PTP_OFC_ASF				0x300C
#define PTP_OFC_QT				0x300D /* guessing */
/* image formats */
#define PTP_OFC_EXIF_JPEG			0x3801
#define PTP_OFC_TIFF_EP				0x3802
#define PTP_OFC_FlashPix			0x3803
#define PTP_OFC_BMP				0x3804
#define PTP_OFC_CIFF				0x3805
#define PTP_OFC_Undefined_0x3806		0x3806
#define PTP_OFC_GIF				0x3807
#define PTP_OFC_JFIF				0x3808
#define PTP_OFC_PCD				0x3809
#define PTP_OFC_PICT				0x380A
#define PTP_OFC_PNG				0x380B
#define PTP_OFC_Undefined_0x380C		0x380C
#define PTP_OFC_TIFF				0x380D
#define PTP_OFC_TIFF_IT				0x380E
#define PTP_OFC_JP2				0x380F
#define PTP_OFC_JPX				0x3810
/* ptp v1.1 has only DNG new */
#define PTP_OFC_DNG				0x3811

/* MTP extensions */
#define PTP_OFC_MTP_MediaCard			0xb211
#define PTP_OFC_MTP_MediaCardGroup		0xb212
#define PTP_OFC_MTP_Encounter			0xb213
#define PTP_OFC_MTP_EncounterBox		0xb214
#define PTP_OFC_MTP_M4A				0xb215
#define PTP_OFC_MTP_ZUNEUNDEFINED		0xb217 /* Unknown file type */
#define PTP_OFC_MTP_Firmware			0xb802
#define PTP_OFC_MTP_WindowsImageFormat		0xb881
#define PTP_OFC_MTP_UndefinedAudio		0xb900
#define PTP_OFC_MTP_WMA				0xb901
#define PTP_OFC_MTP_OGG				0xb902
#define PTP_OFC_MTP_AAC				0xb903
#define PTP_OFC_MTP_AudibleCodec		0xb904
#define PTP_OFC_MTP_FLAC			0xb906
#define PTP_OFC_MTP_SamsungPlaylist		0xb909
#define PTP_OFC_MTP_UndefinedVideo		0xb980
#define PTP_OFC_MTP_WMV				0xb981
#define PTP_OFC_MTP_MP4				0xb982
#define PTP_OFC_MTP_MP2				0xb983
#define PTP_OFC_MTP_3GP				0xb984
#define PTP_OFC_MTP_UndefinedCollection		0xba00
#define PTP_OFC_MTP_AbstractMultimediaAlbum	0xba01
#define PTP_OFC_MTP_AbstractImageAlbum		0xba02
#define PTP_OFC_MTP_AbstractAudioAlbum		0xba03
#define PTP_OFC_MTP_AbstractVideoAlbum		0xba04
#define PTP_OFC_MTP_AbstractAudioVideoPlaylist	0xba05
#define PTP_OFC_MTP_AbstractContactGroup	0xba06
#define PTP_OFC_MTP_AbstractMessageFolder	0xba07
#define PTP_OFC_MTP_AbstractChapteredProduction	0xba08
#define PTP_OFC_MTP_AbstractAudioPlaylist	0xba09
#define PTP_OFC_MTP_AbstractVideoPlaylist	0xba0a
#define PTP_OFC_MTP_AbstractMediacast		0xba0b
#define PTP_OFC_MTP_WPLPlaylist			0xba10
#define PTP_OFC_MTP_M3UPlaylist			0xba11
#define PTP_OFC_MTP_MPLPlaylist			0xba12
#define PTP_OFC_MTP_ASXPlaylist			0xba13
#define PTP_OFC_MTP_PLSPlaylist			0xba14
#define PTP_OFC_MTP_UndefinedDocument		0xba80
#define PTP_OFC_MTP_AbstractDocument		0xba81
#define PTP_OFC_MTP_XMLDocument			0xba82
#define PTP_OFC_MTP_MSWordDocument		0xba83
#define PTP_OFC_MTP_MHTCompiledHTMLDocument	0xba84
#define PTP_OFC_MTP_MSExcelSpreadsheetXLS	0xba85
#define PTP_OFC_MTP_MSPowerpointPresentationPPT	0xba86
#define PTP_OFC_MTP_UndefinedMessage		0xbb00
#define PTP_OFC_MTP_AbstractMessage		0xbb01
#define PTP_OFC_MTP_UndefinedContact		0xbb80
#define PTP_OFC_MTP_AbstractContact		0xbb81
#define PTP_OFC_MTP_vCard2			0xbb82
#define PTP_OFC_MTP_vCard3			0xbb83
#define PTP_OFC_MTP_UndefinedCalendarItem	0xbe00
#define PTP_OFC_MTP_AbstractCalendarItem	0xbe01
#define PTP_OFC_MTP_vCalendar1			0xbe02
#define PTP_OFC_MTP_vCalendar2			0xbe03
#define PTP_OFC_MTP_UndefinedWindowsExecutable	0xbe80
#define PTP_OFC_MTP_MediaCast			0xbe81
#define PTP_OFC_MTP_Section			0xbe82

/* MTP specific Object Properties */
#define PTP_OPC_StorageID				0xDC01
#define PTP_OPC_ObjectFormat				0xDC02
#define PTP_OPC_ProtectionStatus			0xDC03
#define PTP_OPC_ObjectSize				0xDC04
#define PTP_OPC_AssociationType				0xDC05
#define PTP_OPC_AssociationDesc				0xDC06
#define PTP_OPC_ObjectFileName				0xDC07
#define PTP_OPC_DateCreated				0xDC08
#define PTP_OPC_DateModified				0xDC09
#define PTP_OPC_Keywords				0xDC0A
#define PTP_OPC_ParentObject				0xDC0B
#define PTP_OPC_AllowedFolderContents			0xDC0C
#define PTP_OPC_Hidden					0xDC0D
#define PTP_OPC_SystemObject				0xDC0E
#define PTP_OPC_PersistantUniqueObjectIdentifier	0xDC41
#define PTP_OPC_SyncID					0xDC42
#define PTP_OPC_PropertyBag				0xDC43
#define PTP_OPC_Name					0xDC44
#define PTP_OPC_CreatedBy				0xDC45
#define PTP_OPC_Artist					0xDC46
#define PTP_OPC_DateAuthored				0xDC47
#define PTP_OPC_Description				0xDC48
#define PTP_OPC_URLReference				0xDC49
#define PTP_OPC_LanguageLocale				0xDC4A
#define PTP_OPC_CopyrightInformation			0xDC4B
#define PTP_OPC_Source					0xDC4C
#define PTP_OPC_OriginLocation				0xDC4D
#define PTP_OPC_DateAdded				0xDC4E
#define PTP_OPC_NonConsumable				0xDC4F
#define PTP_OPC_CorruptOrUnplayable			0xDC50
#define PTP_OPC_ProducerSerialNumber			0xDC51
#define PTP_OPC_RepresentativeSampleFormat		0xDC81
#define PTP_OPC_RepresentativeSampleSize		0xDC82
#define PTP_OPC_RepresentativeSampleHeight		0xDC83
#define PTP_OPC_RepresentativeSampleWidth		0xDC84
#define PTP_OPC_RepresentativeSampleDuration		0xDC85
#define PTP_OPC_RepresentativeSampleData		0xDC86
#define PTP_OPC_Width					0xDC87
#define PTP_OPC_Height					0xDC88
#define PTP_OPC_Duration				0xDC89
#define PTP_OPC_Rating					0xDC8A
#define PTP_OPC_Track					0xDC8B
#define PTP_OPC_Genre					0xDC8C
#define PTP_OPC_Credits					0xDC8D
#define PTP_OPC_Lyrics					0xDC8E
#define PTP_OPC_SubscriptionContentID			0xDC8F
#define PTP_OPC_ProducedBy				0xDC90
#define PTP_OPC_UseCount				0xDC91
#define PTP_OPC_SkipCount				0xDC92
#define PTP_OPC_LastAccessed				0xDC93
#define PTP_OPC_ParentalRating				0xDC94
#define PTP_OPC_MetaGenre				0xDC95
#define PTP_OPC_Composer				0xDC96
#define PTP_OPC_EffectiveRating				0xDC97
#define PTP_OPC_Subtitle				0xDC98
#define PTP_OPC_OriginalReleaseDate			0xDC99
#define PTP_OPC_AlbumName				0xDC9A
#define PTP_OPC_AlbumArtist				0xDC9B
#define PTP_OPC_Mood					0xDC9C
#define PTP_OPC_DRMStatus				0xDC9D
#define PTP_OPC_SubDescription				0xDC9E
#define PTP_OPC_IsCropped				0xDCD1
#define PTP_OPC_IsColorCorrected			0xDCD2
#define PTP_OPC_ImageBitDepth				0xDCD3
#define PTP_OPC_Fnumber					0xDCD4
#define PTP_OPC_ExposureTime				0xDCD5
#define PTP_OPC_ExposureIndex				0xDCD6
#define PTP_OPC_DisplayName				0xDCE0
#define PTP_OPC_BodyText				0xDCE1
#define PTP_OPC_Subject					0xDCE2
#define PTP_OPC_Priority				0xDCE3
#define PTP_OPC_GivenName				0xDD00
#define PTP_OPC_MiddleNames				0xDD01
#define PTP_OPC_FamilyName				0xDD02
#define PTP_OPC_Prefix					0xDD03
#define PTP_OPC_Suffix					0xDD04
#define PTP_OPC_PhoneticGivenName			0xDD05
#define PTP_OPC_PhoneticFamilyName			0xDD06
#define PTP_OPC_EmailPrimary				0xDD07
#define PTP_OPC_EmailPersonal1				0xDD08
#define PTP_OPC_EmailPersonal2				0xDD09
#define PTP_OPC_EmailBusiness1				0xDD0A
#define PTP_OPC_EmailBusiness2				0xDD0B
#define PTP_OPC_EmailOthers				0xDD0C
#define PTP_OPC_PhoneNumberPrimary			0xDD0D
#define PTP_OPC_PhoneNumberPersonal			0xDD0E
#define PTP_OPC_PhoneNumberPersonal2			0xDD0F
#define PTP_OPC_PhoneNumberBusiness			0xDD10
#define PTP_OPC_PhoneNumberBusiness2			0xDD11
#define PTP_OPC_PhoneNumberMobile			0xDD12
#define PTP_OPC_PhoneNumberMobile2			0xDD13
#define PTP_OPC_FaxNumberPrimary			0xDD14
#define PTP_OPC_FaxNumberPersonal			0xDD15
#define PTP_OPC_FaxNumberBusiness			0xDD16
#define PTP_OPC_PagerNumber				0xDD17
#define PTP_OPC_PhoneNumberOthers			0xDD18
#define PTP_OPC_PrimaryWebAddress			0xDD19
#define PTP_OPC_PersonalWebAddress			0xDD1A
#define PTP_OPC_BusinessWebAddress			0xDD1B
#define PTP_OPC_InstantMessengerAddress			0xDD1C
#define PTP_OPC_InstantMessengerAddress2		0xDD1D
#define PTP_OPC_InstantMessengerAddress3		0xDD1E
#define PTP_OPC_PostalAddressPersonalFull		0xDD1F
#define PTP_OPC_PostalAddressPersonalFullLine1		0xDD20
#define PTP_OPC_PostalAddressPersonalFullLine2		0xDD21
#define PTP_OPC_PostalAddressPersonalFullCity		0xDD22
#define PTP_OPC_PostalAddressPersonalFullRegion		0xDD23
#define PTP_OPC_PostalAddressPersonalFullPostalCode	0xDD24
#define PTP_OPC_PostalAddressPersonalFullCountry	0xDD25
#define PTP_OPC_PostalAddressBusinessFull		0xDD26
#define PTP_OPC_PostalAddressBusinessLine1		0xDD27
#define PTP_OPC_PostalAddressBusinessLine2		0xDD28
#define PTP_OPC_PostalAddressBusinessCity		0xDD29
#define PTP_OPC_PostalAddressBusinessRegion		0xDD2A
#define PTP_OPC_PostalAddressBusinessPostalCode		0xDD2B
#define PTP_OPC_PostalAddressBusinessCountry		0xDD2C
#define PTP_OPC_PostalAddressOtherFull			0xDD2D
#define PTP_OPC_PostalAddressOtherLine1			0xDD2E
#define PTP_OPC_PostalAddressOtherLine2			0xDD2F
#define PTP_OPC_PostalAddressOtherCity			0xDD30
#define PTP_OPC_PostalAddressOtherRegion		0xDD31
#define PTP_OPC_PostalAddressOtherPostalCode		0xDD32
#define PTP_OPC_PostalAddressOtherCountry		0xDD33
#define PTP_OPC_OrganizationName			0xDD34
#define PTP_OPC_PhoneticOrganizationName		0xDD35
#define PTP_OPC_Role					0xDD36
#define PTP_OPC_Birthdate				0xDD37
#define PTP_OPC_MessageTo				0xDD40
#define PTP_OPC_MessageCC				0xDD41
#define PTP_OPC_MessageBCC				0xDD42
#define PTP_OPC_MessageRead				0xDD43
#define PTP_OPC_MessageReceivedTime			0xDD44
#define PTP_OPC_MessageSender				0xDD45
#define PTP_OPC_ActivityBeginTime			0xDD50
#define PTP_OPC_ActivityEndTime				0xDD51
#define PTP_OPC_ActivityLocation			0xDD52
#define PTP_OPC_ActivityRequiredAttendees		0xDD54
#define PTP_OPC_ActivityOptionalAttendees		0xDD55
#define PTP_OPC_ActivityResources			0xDD56
#define PTP_OPC_ActivityAccepted			0xDD57
#define PTP_OPC_Owner					0xDD5D
#define PTP_OPC_Editor					0xDD5E
#define PTP_OPC_Webmaster				0xDD5F
#define PTP_OPC_URLSource				0xDD60
#define PTP_OPC_URLDestination				0xDD61
#define PTP_OPC_TimeBookmark				0xDD62
#define PTP_OPC_ObjectBookmark				0xDD63
#define PTP_OPC_ByteBookmark				0xDD64
#define PTP_OPC_LastBuildDate				0xDD70
#define PTP_OPC_TimetoLive				0xDD71
#define PTP_OPC_MediaGUID				0xDD72
#define PTP_OPC_TotalBitRate				0xDE91
#define PTP_OPC_BitRateType				0xDE92
#define PTP_OPC_SampleRate				0xDE93
#define PTP_OPC_NumberOfChannels			0xDE94
#define PTP_OPC_AudioBitDepth				0xDE95
#define PTP_OPC_ScanDepth				0xDE97
#define PTP_OPC_AudioWAVECodec				0xDE99
#define PTP_OPC_AudioBitRate				0xDE9A
#define PTP_OPC_VideoFourCCCodec			0xDE9B
#define PTP_OPC_VideoBitRate				0xDE9C
#define PTP_OPC_FramesPerThousandSeconds		0xDE9D
#define PTP_OPC_KeyFrameDistance			0xDE9E
#define PTP_OPC_BufferSize				0xDE9F
#define PTP_OPC_EncodingQuality				0xDEA0
#define PTP_OPC_EncodingProfile				0xDEA1
#define PTP_OPC_BuyFlag					0xD901

/* WiFi Provisioning MTP Extension property codes */
#define PTP_OPC_WirelessConfigurationFile		0xB104


/* PTP Association Types */
#define PTP_AT_Undefined			0x0000
#define PTP_AT_GenericFolder			0x0001
#define PTP_AT_Album				0x0002
#define PTP_AT_TimeSequence			0x0003
#define PTP_AT_HorizontalPanoramic		0x0004
#define PTP_AT_VerticalPanoramic		0x0005
#define PTP_AT_2DPanoramic			0x0006
#define PTP_AT_AncillaryData			0x0007

#define PTP_MAX_HANDLER_NBR                      0x255
typedef struct 
{
  uint32_t n;
  uint32_t Handler[PTP_MAX_HANDLER_NBR];
}
PTP_ObjectHandlesTypedef;


#define PTP_oi_StorageID		 0
#define PTP_oi_ObjectFormat		 4
#define PTP_oi_ProtectionStatus		 6
#define PTP_oi_ObjectCompressedSize	 8
#define PTP_oi_ThumbFormat		12
#define PTP_oi_ThumbCompressedSize	14
#define PTP_oi_ThumbPixWidth		18
#define PTP_oi_ThumbPixHeight		22
#define PTP_oi_ImagePixWidth		26
#define PTP_oi_ImagePixHeight		30
#define PTP_oi_ImageBitDepth		34
#define PTP_oi_ParentObject		38
#define PTP_oi_AssociationType		42
#define PTP_oi_AssociationDesc		44
#define PTP_oi_SequenceNumber		48
#define PTP_oi_filenamelen		52
#define PTP_oi_Filename			53

typedef struct 
{
  uint32_t StorageID;
  uint16_t ObjectFormat;
  uint16_t ProtectionStatus;
  /* In the regular objectinfo this is 32bit, but we keep the general object size here
     that also arrives via other methods and so use 64bit */
  uint64_t ObjectCompressedSize;
  uint16_t ThumbFormat;
  uint32_t ThumbCompressedSize;
  uint32_t ThumbPixWidth;
  uint32_t ThumbPixHeight;
  uint32_t ImagePixWidth;
  uint32_t ImagePixHeight;
  uint32_t ImageBitDepth;
  uint32_t ParentObject;
  uint16_t AssociationType;
  uint32_t AssociationDesc;
  uint32_t SequenceNumber;
  uint8_t  Filename[PTP_MAX_STR_SIZE];
  uint32_t CaptureDate;
  uint32_t ModificationDate;
  uint8_t  Keywords[PTP_MAX_STR_SIZE];
}
PTP_ObjectInfoTypedef;

/* Object Property Describing Dataset (DevicePropDesc) */

typedef union  _PTP_PropertyValueTypedef
{
	char		str[PTP_MAX_STR_SIZE];
	uint8_t		u8;
	int8_t		i8;
	uint16_t	u16;
	int16_t		i16;
	uint32_t	u32;
	int32_t		i32;
	uint64_t	u64;
	int64_t		i64;

	struct array {
		uint32_t	count;
		union _PTP_PropertyValueTypedef *v;
	} a;
}PTP_PropertyValueTypedef;

typedef struct 
{
	PTP_PropertyValueTypedef 	MinimumValue;
	PTP_PropertyValueTypedef 	MaximumValue;
	PTP_PropertyValueTypedef 	StepSize;
}
PTP_PropDescRangeFormTypedef;

/* Property Describing Dataset, Enum Form */

typedef struct 
{
	uint16_t		 NumberOfValues;
	PTP_PropertyValueTypedef SupportedValue[PTP_SUPPORTED_PROPRIETIES_NBR];
}
PTP_PropDescEnumFormTypedef;

/* (MTP) Object Property pack/unpack */
#define PTP_opd_ObjectPropertyCode	0
#define PTP_opd_DataType		2
#define PTP_opd_GetSet			4
#define PTP_opd_FactoryDefaultValue	5

typedef struct
{
	uint16_t		ObjectPropertyCode;
	uint16_t		DataType;
	uint8_t			GetSet;
	PTP_PropertyValueTypedef FactoryDefaultValue;
	uint32_t		GroupCode;
	uint8_t			FormFlag;
	union	{
		PTP_PropDescEnumFormTypedef	Enum;
		PTP_PropDescRangeFormTypedef	Range;
	} FORM;
}
PTP_ObjectPropDescTypeDef;

/* Metadata lists for MTP operations */
typedef struct
{
	uint16_t 	 	property;
	uint16_t 	 	datatype;
	uint32_t 	 	ObjectHandle;
	PTP_PropertyValueTypedef propval;
}
MTP_PropertiesTypedef;


/* Device Property Form Flag */

#define PTP_DPFF_None			0x00
#define PTP_DPFF_Range			0x01
#define PTP_DPFF_Enumeration		0x02

/* Object Property Codes used by MTP (first 3 are same as DPFF codes) */
#define PTP_OPFF_None			0x00
#define PTP_OPFF_Range			0x01
#define PTP_OPFF_Enumeration		0x02
#define PTP_OPFF_DateTime		0x03
#define PTP_OPFF_FixedLengthArray	0x04
#define PTP_OPFF_RegularExpression	0x05
#define PTP_OPFF_ByteArray		0x06
#define PTP_OPFF_LongString		0xFF

/* Device Property pack/unpack */

#define PTP_dpd_DevicePropertyCode	0
#define PTP_dpd_DataType		2
#define PTP_dpd_GetSet			4
#define PTP_dpd_FactoryDefaultValue	5

/* Device Property Describing Dataset (DevicePropDesc) */

typedef struct 
{
	uint16_t		DevicePropertyCode;
	uint16_t		DataType;
	uint8_t			GetSet;
	PTP_PropertyValueTypedef FactoryDefaultValue;
	PTP_PropertyValueTypedef CurrentValue;
	uint8_t			FormFlag;
	union	{
		PTP_PropDescEnumFormTypedef	Enum;
		PTP_PropDescRangeFormTypedef	Range;
	} FORM;
}
PTP_DevicePropDescTypdef;

/* DataType Codes */

#define PTP_DTC_UNDEF		0x0000
#define PTP_DTC_INT8		0x0001
#define PTP_DTC_UINT8		0x0002
#define PTP_DTC_INT16		0x0003
#define PTP_DTC_UINT16		0x0004
#define PTP_DTC_INT32		0x0005
#define PTP_DTC_UINT32		0x0006
#define PTP_DTC_INT64		0x0007
#define PTP_DTC_UINT64		0x0008
#define PTP_DTC_INT128		0x0009
#define PTP_DTC_UINT128		0x000A

#define PTP_DTC_ARRAY_MASK	0x4000

#define PTP_DTC_AINT8		(PTP_DTC_ARRAY_MASK | PTP_DTC_INT8)
#define PTP_DTC_AUINT8		(PTP_DTC_ARRAY_MASK | PTP_DTC_UINT8)
#define PTP_DTC_AINT16		(PTP_DTC_ARRAY_MASK | PTP_DTC_INT16)
#define PTP_DTC_AUINT16		(PTP_DTC_ARRAY_MASK | PTP_DTC_UINT16)
#define PTP_DTC_AINT32		(PTP_DTC_ARRAY_MASK | PTP_DTC_INT32)
#define PTP_DTC_AUINT32		(PTP_DTC_ARRAY_MASK | PTP_DTC_UINT32)
#define PTP_DTC_AINT64		(PTP_DTC_ARRAY_MASK | PTP_DTC_INT64)
#define PTP_DTC_AUINT64		(PTP_DTC_ARRAY_MASK | PTP_DTC_UINT64)
#define PTP_DTC_AINT128		(PTP_DTC_ARRAY_MASK | PTP_DTC_INT128)
#define PTP_DTC_AUINT128	(PTP_DTC_ARRAY_MASK | PTP_DTC_UINT128)

#define PTP_DTC_STR		0xFFFF

/* PTP Event Codes */

#define PTP_EC_Undefined		0x4000
#define PTP_EC_CancelTransaction	0x4001
#define PTP_EC_ObjectAdded		0x4002
#define PTP_EC_ObjectRemoved		0x4003
#define PTP_EC_StoreAdded		0x4004
#define PTP_EC_StoreRemoved		0x4005
#define PTP_EC_DevicePropChanged	0x4006
#define PTP_EC_ObjectInfoChanged	0x4007
#define PTP_EC_DeviceInfoChanged	0x4008
#define PTP_EC_RequestObjectTransfer	0x4009
#define PTP_EC_StoreFull		0x400A
#define PTP_EC_DeviceReset		0x400B
#define PTP_EC_StorageInfoChanged	0x400C
#define PTP_EC_CaptureComplete		0x400D
#define PTP_EC_UnreportedStatus		0x400E


/**
  * @}
  */ 

/** @defgroup USBH_MTP_PTP_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_MTP_PTP_Exported_Variables
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBH_MTP_PTP_Exported_FunctionsPrototype
  * @{
  */ 
USBH_StatusTypeDef USBH_PTP_Init(USBH_HandleTypeDef *phost);
USBH_StatusTypeDef USBH_PTP_Process (USBH_HandleTypeDef *phost);

USBH_StatusTypeDef USBH_PTP_SendRequest (USBH_HandleTypeDef *phost, PTP_ContainerTypedef  *req);
USBH_StatusTypeDef USBH_PTP_GetResponse (USBH_HandleTypeDef *phost, PTP_ContainerTypedef  *req);

USBH_StatusTypeDef USBH_PTP_OpenSession (USBH_HandleTypeDef *phost, uint32_t session);
USBH_StatusTypeDef USBH_PTP_GetDeviceInfo (USBH_HandleTypeDef *phost, PTP_DeviceInfoTypedef *dev_info);
USBH_StatusTypeDef USBH_PTP_GetStorageIds (USBH_HandleTypeDef *phost, PTP_StorageIDsTypedef *storage_ids);

USBH_StatusTypeDef USBH_PTP_GetStorageInfo (USBH_HandleTypeDef *phost, 
                                            uint32_t storage_id, 
                                            PTP_StorageInfoTypedef *storage_info);

USBH_StatusTypeDef USBH_PTP_GetNumObjects (USBH_HandleTypeDef *phost,
                                           uint32_t storage_id, 
                                           uint32_t objectformatcode, 
                                           uint32_t associationOH,
                                           uint32_t* numobs);

USBH_StatusTypeDef USBH_PTP_GetObjectHandles (USBH_HandleTypeDef *phost, 
                                           uint32_t storage_id, 
                                           uint32_t objectformatcode, 
                                           uint32_t associationOH,
                                           PTP_ObjectHandlesTypedef* objecthandles);

USBH_StatusTypeDef USBH_PTP_GetObjectInfo (USBH_HandleTypeDef *phost, 
                                           uint32_t handle, 
                                           PTP_ObjectInfoTypedef* objectinfo);

USBH_StatusTypeDef USBH_PTP_DeleteObject (USBH_HandleTypeDef *phost,
                                          uint32_t handle,
                                          uint32_t objectformatcode);

USBH_StatusTypeDef USBH_PTP_GetObject (USBH_HandleTypeDef *phost, 
                                           uint32_t handle, 
                                           uint8_t *object);

USBH_StatusTypeDef USBH_PTP_GetPartialObject(USBH_HandleTypeDef *phost, 
                                           uint32_t handle, 
                                           uint32_t offset,
                                           uint32_t maxbytes, uint8_t *object,
                                           uint32_t *len);

USBH_StatusTypeDef USBH_PTP_GetObjectPropsSupported (USBH_HandleTypeDef *phost,
                                                     uint16_t ofc,
                                                     uint32_t *propnum, 
                                                     uint16_t *props);

USBH_StatusTypeDef USBH_PTP_GetObjectPropDesc (USBH_HandleTypeDef *phost,
                                                uint16_t opc, 
                                                uint16_t ofc, 
                                                PTP_ObjectPropDescTypeDef *opd);

USBH_StatusTypeDef USBH_PTP_GetObjectPropList (USBH_HandleTypeDef *phost,
                                                uint32_t handle, 
                                                MTP_PropertiesTypedef *pprops, 
                                                uint32_t *nrofprops);

USBH_StatusTypeDef USBH_PTP_SendObject (USBH_HandleTypeDef *phost, 
                                           uint32_t handle, 
                                           uint8_t *object,
                                           uint32_t size);

USBH_StatusTypeDef USBH_PTP_GetDevicePropDesc (USBH_HandleTypeDef *phost,
                                                uint16_t propcode, 
			                      PTP_DevicePropDescTypdef* devicepropertydesc);

/**
  * @}
  */ 

#endif  //__USBH_MTP_PTP_H__


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

