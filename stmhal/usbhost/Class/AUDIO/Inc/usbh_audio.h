/**
  ******************************************************************************
  * @file    usbh_audio.h
  * @author  MCD Application Team
  * @version V3.0.0
  * @date    18-February-2014
  * @brief   This file contains all the prototypes for the usbh_audio.c
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
#ifndef __USBH_AUDIO_H
#define __USBH_AUDIO_H

/* Includes ------------------------------------------------------------------*/
#include "usbh_core.h"
 
/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_CLASS
  * @{
  */

/** @addtogroup USBH_AUDIO_CLASS
  * @{
  */
  
/** @defgroup USBH_AUDIO_CORE
  * @brief This file is the Header file for usbh_audio.c
  * @{
  */ 


/** @defgroup USBH_AUDIO_CORE_Exported_Types
  * @{
  */ 

/* States for AUDIO State Machine */
typedef enum
{
  AUDIO_INIT = 0, 
  AUDIO_IDLE,   
  AUDIO_CS_REQUESTS,
  AUDIO_SET_DEFAULT_FEATURE_UNIT,
  AUDIO_SET_INTERFACE,
  AUDIO_SET_STREAMING_INTERFACE,
  AUDIO_SET_CUR1,
  AUDIO_GET_MIN,
  AUDIO_GET_MAX,
  AUDIO_GET_RES,
  AUDIO_GET_CUR1,
  AUDIO_SET_CUR2,
  AUDIO_GET_CUR2,
  AUDIO_SET_CUR3,
  AUDIO_SET_INTERFACE0,
  AUDIO_SET_INTERFACE1,
  AUDIO_SET_INTERFACE2,
  AUDIO_ISOC_OUT,        
  AUDIO_ISOC_IN,  
  AUDIO_ISOC_POLL,
  AUDIO_ERROR,  
}
AUDIO_StateTypeDef;

typedef enum
{
  AUDIO_REQ_INIT = 1,
  AUDIO_REQ_IDLE, 
  AUDIO_REQ_SET_DEFAULT_IN_INTERFACE,
  AUDIO_REQ_SET_DEFAULT_OUT_INTERFACE,  
  AUDIO_REQ_SET_IN_INTERFACE,
  AUDIO_REQ_SET_OUT_INTERFACE,
  AUDIO_REQ_CS_REQUESTS,
}
AUDIO_ReqStateTypeDef;

typedef enum
{
  AUDIO_REQ_SET_VOLUME = 1,
  AUDIO_REQ_SET_MUTE,  
  AUDIO_REQ_GET_CURR_VOLUME,
  AUDIO_REQ_GET_MIN_VOLUME,
  AUDIO_REQ_GET_MAX_VOLUME, 
  AUDIO_REQ_GET_VOLUME, 
  AUDIO_REQ_GET_RESOLUTION,
  AUDIO_REQ_CS_IDLE,
}
AUDIO_CSReqStateTypeDef;

typedef enum
{
 AUDIO_PLAYBACK_INIT = 1,
 AUDIO_PLAYBACK_SET_EP,
 AUDIO_PLAYBACK_SET_EP_FREQ,
 AUDIO_PLAYBACK_PLAY,  
 AUDIO_PLAYBACK_IDLE,
}
AUDIO_PlayStateTypeDef;

typedef enum
{
 VOLUME_UP = 1,
 VOLUME_DOWN = 2,   
}
AUDIO_VolumeCtrlTypeDef;

typedef enum
{
 AUDIO_CONTROL_INIT = 1,
 AUDIO_CONTROL_CHANGE,
 AUDIO_CONTROL_IDLE,
 AUDIO_CONTROL_VOLUME_UP,
 AUDIO_CONTROL_VOLUME_DOWN,   
}
AUDIO_ControlStateTypeDef;


typedef enum
{
 AUDIO_DATA_START_OUT = 1,  
 AUDIO_DATA_OUT,
}
AUDIO_ProcessingTypeDef;

/* Structure for AUDIO process */
typedef struct
{
  uint8_t   Channels;
  uint8_t   Bits;
  uint32_t  SampleRate;
} 
AUDIO_FormatTypeDef;

typedef struct
{
  uint8_t              Ep;
  uint16_t             EpSize;
  uint8_t              AltSettings;
  uint8_t              interface;
  uint8_t              valid; 
  uint16_t             Poll;     
}
AUDIO_STREAMING_IN_HandleTypeDef;

typedef struct
{
  uint8_t              Ep;
  uint16_t             EpSize;
  uint8_t              AltSettings;
  uint8_t              interface;
  uint8_t              valid;  
  uint16_t             Poll;       
}
AUDIO_STREAMING_OUT_HandleTypeDef;


typedef struct
{
  uint8_t              mute;
  uint32_t             volumeMin;
  uint32_t             volumeMax;
  uint32_t             volume;
  uint32_t             resolution;  
} 
AUDIO_ControlAttributeTypeDef;

typedef struct
{

  uint8_t              Ep;
  uint16_t             EpSize; 
  uint8_t              interface; 
  uint8_t              AltSettings;
  uint8_t              supported;    

  uint8_t              Pipe;  
  uint8_t              Poll; 
  uint32_t             timer ; 
  
  uint8_t              asociated_as; 
  uint8_t              asociated_mixer; 
  uint8_t              asociated_selector; 
  uint8_t              asociated_feature; 
  uint8_t              asociated_terminal;
  uint8_t              asociated_channels;
  
  uint32_t             frequency; 
  uint8_t              *buf;
  uint8_t              *cbuf; 
  uint32_t             partial_ptr; 

  uint32_t             global_ptr;  
  uint16_t             frame_length;  
  uint32_t             total_length; 
  
  AUDIO_ControlAttributeTypeDef attribute;  
}
AUDIO_InterfaceStreamPropTypeDef;

typedef struct
{

  uint8_t              Ep;
  uint16_t             EpSize; 
  uint8_t              interface; 
  uint8_t              supported; 
  
  uint8_t              Pipe;  
  uint8_t              Poll; 
  uint32_t             timer ; 
}
AUDIO_InterfaceControlPropTypeDef;


#define AUDIO_MAX_AUDIO_STD_INTERFACE      0x05
#define AUDIO_MAX_FREQ_SUPPORTED           0x05
#define AUDIO_MAX_STREAMING_INTERFACE      0x05
#define AUDIO_MAX_NUM_IN_TERMINAL          0x04
#define AUDIO_MAX_NUM_OUT_TERMINAL         0x04
#define AUDIO_MAX_NUM_FEATURE_UNIT         0x04
#define AUDIO_MAX_NUM_MIXER_UNIT           0x04
#define AUDIO_MAX_NUM_SELECTOR_UNIT        0x04

#define HEADPHONE_SUPPORTED                0x01
#define MICROPHONE_SUPPORTED               0x02
#define HEADSET_SUPPORTED                  0x03


/*Class-Specific AS(Audio Streaming) Interface Descriptor*/
typedef struct
{
  uint8_t bLength; 
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bTerminalLink;
  uint8_t bDelay;
  uint8_t wFormatTag[2];
}
AUDIO_ASGeneralDescTypeDef;

/*Class-Specific AS(Audio Streaming) Format Type Descriptor*/
typedef struct
{
  uint8_t bLength;    /*At to be deside*/
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bFormatType;
  uint8_t bNrChannels;
  uint8_t bSubframeSize;
  uint8_t bBitResolution;
  uint8_t bSamFreqType;
  uint8_t tSamFreq[][3];
}
AUDIO_ASFormatTypeDescTypeDef;

/*Class-Specific AS(Audio Streaming) Interface Descriptor*/
typedef struct
{
  AUDIO_ASGeneralDescTypeDef      *GeneralDesc;
  AUDIO_ASFormatTypeDescTypeDef   *FormatTypeDesc;
}
AUDIO_ASDescTypeDef;

/* 4.3.2  Class-Specific AC Interface Descriptor */

typedef struct
{
  uint8_t  bLength;                 
  uint8_t  bDescriptorType;         
  uint8_t  bDescriptorSubtype;      
  uint8_t  bcdADC[2];                  
  uint8_t  wTotalLength[2];            
  uint8_t  bInCollection;           
  uint8_t  baInterfaceNr[];         
} 
AUDIO_HeaderDescTypeDef;

/* 4.3.2.1 Input Terminal Descriptor */
typedef struct 
{
  uint8_t  bLength;                 
  uint8_t  bDescriptorType;         
  uint8_t  bDescriptorSubtype;      
  uint8_t  bTerminalID;             
  uint8_t  wTerminalType[2];          
  uint8_t  bAssocTerminal;          
  uint8_t  bNrChannels;             
  uint8_t  wChannelConfig[2];
  uint8_t  iChannelNames;
  uint8_t  iTerminal;
}
AUDIO_ITDescTypeDef;

/* 4.3.2.2 Output Terminal Descriptor */
typedef struct 
{
  uint8_t  bLength;                 
  uint8_t  bDescriptorType;         
  uint8_t  bDescriptorSubtype;      
  uint8_t  bTerminalID;             
  uint8_t  wTerminalType[2];          
  uint8_t  bAssocTerminal;          
  uint8_t  bSourceID;               
  uint8_t  iTerminal;
}
AUDIO_OTDescTypeDef;

/* 4.3.2.3 Feature Descriptor */
typedef struct
{                                                             
  uint8_t  bLength;                                        
  uint8_t  bDescriptorType;                                
  uint8_t  bDescriptorSubtype;                             
  uint8_t  bUnitID;                                        
  uint8_t  bSourceID;                                      
  uint8_t  bControlSize;                                   
  uint8_t bmaControls[][2];                                                                 
}
AUDIO_FeatureDescTypeDef;


/* 4.3.2.3 Feature Descriptor */
typedef struct
{                                                             
  uint8_t  bLength;                                        
  uint8_t  bDescriptorType;                                
  uint8_t  bDescriptorSubtype;                             
  uint8_t  bUnitID; 
  uint8_t  bNrInPins; 
  uint8_t  bSourceID0; 
  uint8_t  bSourceID1;  
  uint8_t  bNrChannels;    
  uint8_t  bmChannelsConfig[2];                                   
  uint8_t iChannelsNames;    
  uint8_t bmaControls;   
  uint8_t iMixer;   
}
AUDIO_MixerDescTypeDef;



/* 4.3.2.3 Feature Descriptor */
typedef struct
{                                                             
  uint8_t  bLength;                                        
  uint8_t  bDescriptorType;                                
  uint8_t  bDescriptorSubtype;                             
  uint8_t  bUnitID;                                        
  uint8_t  bNrInPins; 
  uint8_t  bSourceID0;  
  uint8_t  iSelector;                                                                   
}
AUDIO_SelectorDescTypeDef;

/*Class-Specific AC(Audio Control) Interface Descriptor*/
typedef struct
{
  AUDIO_HeaderDescTypeDef   *HeaderDesc;
  AUDIO_ITDescTypeDef       *InputTerminalDesc [AUDIO_MAX_NUM_IN_TERMINAL];
  AUDIO_OTDescTypeDef       *OutputTerminalDesc[AUDIO_MAX_NUM_OUT_TERMINAL];
  AUDIO_FeatureDescTypeDef  *FeatureUnitDesc   [AUDIO_MAX_NUM_FEATURE_UNIT];
  AUDIO_MixerDescTypeDef    *MixerUnitDesc     [AUDIO_MAX_NUM_MIXER_UNIT];
  AUDIO_SelectorDescTypeDef *SelectorUnitDesc  [AUDIO_MAX_NUM_SELECTOR_UNIT];
}
AUDIO_ACDescTypeDef;

/*Class-Specific AC : Global descriptor*/

typedef struct
{
  AUDIO_ACDescTypeDef   cs_desc; /* Only one control descriptor*/
  AUDIO_ASDescTypeDef   as_desc[AUDIO_MAX_STREAMING_INTERFACE];

  uint16_t ASNum; 
  uint16_t InputTerminalNum;   
  uint16_t OutputTerminalNum;  
  uint16_t FeatureUnitNum;    
  uint16_t SelectorUnitNum;    
  uint16_t MixerUnitNum;    
}
AUDIO_ClassSpecificDescTypedef;


typedef struct _AUDIO_Process
{
  AUDIO_ReqStateTypeDef              req_state;
  AUDIO_CSReqStateTypeDef            cs_req_state;  
  AUDIO_PlayStateTypeDef             play_state;  
  AUDIO_ControlStateTypeDef          control_state; 
  AUDIO_ProcessingTypeDef            processing_state;
  
  AUDIO_STREAMING_IN_HandleTypeDef   stream_in[AUDIO_MAX_AUDIO_STD_INTERFACE];
  AUDIO_STREAMING_OUT_HandleTypeDef  stream_out[AUDIO_MAX_AUDIO_STD_INTERFACE];
  AUDIO_ClassSpecificDescTypedef     class_desc;
  
  AUDIO_InterfaceStreamPropTypeDef   headphone;
  AUDIO_InterfaceStreamPropTypeDef   microphone; 
  AUDIO_InterfaceControlPropTypeDef  control;
  uint16_t                            mem [8];  
  uint8_t                            temp_feature;   
  uint8_t                            temp_channels;      
}
AUDIO_HandleTypeDef;

/**
  * @}
  */ 

/** @defgroup USBH_AUDIO_CORE_Exported_Defines
  * @{
  */ 


/*Audio Interface Subclass Codes*/
#define AC_CLASS                        0x01

/* A.2 Audio Interface Subclass Codes */
#define USB_SUBCLASS_AUDIOCONTROL	0x01
#define USB_SUBCLASS_AUDIOSTREAMING	0x02
#define USB_SUBCLASS_MIDISTREAMING	0x03 

#define USB_DESC_TYPE_CS_INTERFACE      0x24
#define USB_DESC_TYPE_CS_ENDPOINT       0x25

/* A.5 Audio Class-Specific AC Interface Descriptor Subtypes */
#define UAC_HEADER			0x01
#define UAC_INPUT_TERMINAL		0x02
#define UAC_OUTPUT_TERMINAL		0x03
#define UAC_MIXER_UNIT			0x04
#define UAC_SELECTOR_UNIT		0x05
#define UAC_FEATURE_UNIT		0x06
#define UAC_PROCESSING_UNIT		0x07
#define UAC_EXTENSION_UNIT		0x08

/*Audio Class-Specific Endpoint Descriptor Subtypes*/
#define  EP_CONTROL_UNDEFINED             0x00
#define  SAMPLING_FREQ_CONTROL            0x01
#define  PITCH_CONTROL                    0x02

/*Feature unit control selector*/
#define FU_CONTROL_UNDEFINED                        0x00
#define MUTE_CONTROL                                0x01
#define VOLUME_CONTROL                              0x02
#define BASS_CONTROL                                0x03
#define MID_CONTROL                                 0x04
#define TREBLE_CONTROL                              0x05
#define GRAPHIC_EQUALIZER_CONTROL                   0x06
#define AUTOMATIC_GAIN_CONTROL                      0x07
#define DELAY_CONTROL                               0x08
#define BASS_BOOST_CONTROL                          0x09
#define LOUDNESS_CONTROL                            0x0A

/*Terminal control selector*/
#define TE_CONTROL_UNDEFINED                        0x00
#define COPY_PROTECT_CONTROL                        0x01


/* A.6 Audio Class-Specific AS Interface Descriptor Subtypes */
#define UAC_AS_GENERAL			0x01
#define UAC_FORMAT_TYPE			0x02
#define UAC_FORMAT_SPECIFIC		0x03

/* A.8 Audio Class-Specific Endpoint Descriptor Subtypes */
#define UAC_EP_GENERAL			0x01

/* A.9 Audio Class-Specific Request Codes */
#define UAC_SET_			0x00
#define UAC_GET_			0x80

#define UAC__CUR			0x1
#define UAC__MIN			0x2
#define UAC__MAX			0x3
#define UAC__RES			0x4
#define UAC__MEM			0x5

#define UAC_SET_CUR			(UAC_SET_ | UAC__CUR)
#define UAC_GET_CUR			(UAC_GET_ | UAC__CUR)
#define UAC_SET_MIN			(UAC_SET_ | UAC__MIN)
#define UAC_GET_MIN			(UAC_GET_ | UAC__MIN)
#define UAC_SET_MAX			(UAC_SET_ | UAC__MAX)
#define UAC_GET_MAX			(UAC_GET_ | UAC__MAX)
#define UAC_SET_RES			(UAC_SET_ | UAC__RES)
#define UAC_GET_RES			(UAC_GET_ | UAC__RES)
#define UAC_SET_MEM			(UAC_SET_ | UAC__MEM)
#define UAC_GET_MEM			(UAC_GET_ | UAC__MEM)

#define UAC_GET_STAT			0xff

/* MIDI - A.1 MS Class-Specific Interface Descriptor Subtypes */
#define UAC_MS_HEADER			0x01
#define UAC_MIDI_IN_JACK		0x02
#define UAC_MIDI_OUT_JACK		0x03

/* MIDI - A.1 MS Class-Specific Endpoint Descriptor Subtypes */
#define UAC_MS_GENERAL			0x01

/* Terminals - 2.1 USB Terminal Types */
#define UAC_TERMINAL_UNDEFINED		0x100
#define UAC_TERMINAL_STREAMING		0x101
#define UAC_TERMINAL_VENDOR_SPEC	0x1FF
    
/**
  * @}
  */ 

/** @defgroup USBH_AUDIO_CORE_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_AUDIO_CORE_Exported_Variables
  * @{
  */ 
extern USBH_ClassTypeDef  AUDIO_Class;
#define USBH_AUDIO_CLASS    &AUDIO_Class
/**
  * @}
  */ 

/** @defgroup USBH_AUDIO_CORE_Exported_FunctionsPrototype
  * @{
  */ 
USBH_StatusTypeDef USBH_AUDIO_SetFrequency (USBH_HandleTypeDef *phost, 
                                            uint16_t sample_rate,
                                            uint8_t  channel_num,
                                            uint8_t data_width);
  
USBH_StatusTypeDef USBH_AUDIO_Play (USBH_HandleTypeDef *phost, uint8_t *buf, uint32_t length);
USBH_StatusTypeDef USBH_AUDIO_Stop (USBH_HandleTypeDef *phost);
USBH_StatusTypeDef USBH_AUDIO_Suspend (USBH_HandleTypeDef *phost);
USBH_StatusTypeDef USBH_AUDIO_Resume (USBH_HandleTypeDef *phost);
USBH_StatusTypeDef USBH_AUDIO_SetVolume (USBH_HandleTypeDef *phost, AUDIO_VolumeCtrlTypeDef volume_ctl);
USBH_StatusTypeDef USBH_AUDIO_ChangeOutBuffer (USBH_HandleTypeDef *phost, uint8_t *buf);
int32_t            USBH_AUDIO_GetOutOffset (USBH_HandleTypeDef *phost);

void        USBH_AUDIO_FrequencySet(USBH_HandleTypeDef *phost);
/**
  * @}
  */ 


#endif /* __USBH_AUDIO_H */

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

