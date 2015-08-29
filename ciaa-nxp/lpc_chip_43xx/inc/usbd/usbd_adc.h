/***********************************************************************
* $Id:: mw_usbd_audio.h 165 2011-04-14 17:41:11Z usb10131                     $
*
* Project: USB device ROM Stack
*
* Description:
*     USB Audio Device Class Definitions.
*
***********************************************************************
*   Copyright(C) 2011, NXP Semiconductor
*   All rights reserved.
*
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
**********************************************************************/
#ifndef __AUDIO_H__
#define __AUDIO_H__


/* Audio Interface Subclass Codes */
#define AUDIO_SUBCLASS_UNDEFINED                0x00
#define AUDIO_SUBCLASS_AUDIOCONTROL             0x01
#define AUDIO_SUBCLASS_AUDIOSTREAMING           0x02
#define AUDIO_SUBCLASS_MIDISTREAMING            0x03

/* Audio Interface Protocol Codes */
#define AUDIO_PROTOCOL_UNDEFINED                0x00


/* Audio Descriptor Types */
#define AUDIO_UNDEFINED_DESCRIPTOR_TYPE         0x20
#define AUDIO_DEVICE_DESCRIPTOR_TYPE            0x21
#define AUDIO_CONFIGURATION_DESCRIPTOR_TYPE     0x22
#define AUDIO_STRING_DESCRIPTOR_TYPE            0x23
#define AUDIO_INTERFACE_DESCRIPTOR_TYPE         0x24
#define AUDIO_ENDPOINT_DESCRIPTOR_TYPE          0x25


/* Audio Control Interface Descriptor Subtypes */
#define AUDIO_CONTROL_UNDEFINED                 0x00
#define AUDIO_CONTROL_HEADER                    0x01
#define AUDIO_CONTROL_INPUT_TERMINAL            0x02
#define AUDIO_CONTROL_OUTPUT_TERMINAL           0x03
#define AUDIO_CONTROL_MIXER_UNIT                0x04
#define AUDIO_CONTROL_SELECTOR_UNIT             0x05
#define AUDIO_CONTROL_FEATURE_UNIT              0x06
#define AUDIO_CONTROL_PROCESSING_UNIT           0x07
#define AUDIO_CONTROL_EXTENSION_UNIT            0x08

/* Audio Streaming Interface Descriptor Subtypes */
#define AUDIO_STREAMING_UNDEFINED               0x00
#define AUDIO_STREAMING_GENERAL                 0x01
#define AUDIO_STREAMING_FORMAT_TYPE             0x02
#define AUDIO_STREAMING_FORMAT_SPECIFIC         0x03

/* Audio Endpoint Descriptor Subtypes */
#define AUDIO_ENDPOINT_UNDEFINED                0x00
#define AUDIO_ENDPOINT_GENERAL                  0x01


/* Audio Descriptor Sizes */
#define AUDIO_CONTROL_INTERFACE_DESC_SZ(n)      0x08+n
#define AUDIO_STREAMING_INTERFACE_DESC_SIZE     0x07
#define AUDIO_INPUT_TERMINAL_DESC_SIZE          0x0C
#define AUDIO_OUTPUT_TERMINAL_DESC_SIZE         0x09
#define AUDIO_MIXER_UNIT_DESC_SZ(p,n)           0x0A+p+n
#define AUDIO_SELECTOR_UNIT_DESC_SZ(p)          0x06+p
#define AUDIO_FEATURE_UNIT_DESC_SZ(ch,n)        0x07+(ch+1)*n
#define AUDIO_PROCESSING_UNIT_DESC_SZ(p,n,x)    0x0D+p+n+x
#define AUDIO_EXTENSION_UNIT_DESC_SZ(p,n)       0x0D+p+n
#define AUDIO_STANDARD_ENDPOINT_DESC_SIZE       0x09
#define AUDIO_STREAMING_ENDPOINT_DESC_SIZE      0x07


/* Audio Processing Unit Process Types */
#define AUDIO_UNDEFINED_PROCESS                 0x00
#define AUDIO_UP_DOWN_MIX_PROCESS               0x01
#define AUDIO_DOLBY_PROLOGIC_PROCESS            0x02
#define AUDIO_3D_STEREO_PROCESS                 0x03
#define AUDIO_REVERBERATION_PROCESS             0x04
#define AUDIO_CHORUS_PROCESS                    0x05
#define AUDIO_DYN_RANGE_COMP_PROCESS            0x06


/* Audio Request Codes */
#define AUDIO_REQUEST_UNDEFINED                 0x00
#define AUDIO_REQUEST_SET_CUR                   0x01
#define AUDIO_REQUEST_GET_CUR                   0x81
#define AUDIO_REQUEST_SET_MIN                   0x02
#define AUDIO_REQUEST_GET_MIN                   0x82
#define AUDIO_REQUEST_SET_MAX                   0x03
#define AUDIO_REQUEST_GET_MAX                   0x83
#define AUDIO_REQUEST_SET_RES                   0x04
#define AUDIO_REQUEST_GET_RES                   0x84
#define AUDIO_REQUEST_SET_MEM                   0x05
#define AUDIO_REQUEST_GET_MEM                   0x85
#define AUDIO_REQUEST_GET_STAT                  0xFF


/* Audio Control Selector Codes */
#define AUDIO_CONTROL_UNDEFINED                 0x00    /* Common Selector */

/*  Terminal Control Selectors */
#define AUDIO_COPY_PROTECT_CONTROL              0x01

/*  Feature Unit Control Selectors */
#define AUDIO_MUTE_CONTROL                      0x01
#define AUDIO_VOLUME_CONTROL                    0x02
#define AUDIO_BASS_CONTROL                      0x03
#define AUDIO_MID_CONTROL                       0x04
#define AUDIO_TREBLE_CONTROL                    0x05
#define AUDIO_GRAPHIC_EQUALIZER_CONTROL         0x06
#define AUDIO_AUTOMATIC_GAIN_CONTROL            0x07
#define AUDIO_DELAY_CONTROL                     0x08
#define AUDIO_BASS_BOOST_CONTROL                0x09
#define AUDIO_LOUDNESS_CONTROL                  0x0A

/*  Processing Unit Control Selectors: */
#define AUDIO_ENABLE_CONTROL                    0x01    /* Common Selector */
#define AUDIO_MODE_SELECT_CONTROL               0x02    /* Common Selector */

/*  - Up/Down-mix Control Selectors */
/*      AUDIO_ENABLE_CONTROL                    0x01       Common Selector */
/*      AUDIO_MODE_SELECT_CONTROL               0x02       Common Selector */

/*  - Dolby Prologic Control Selectors */
/*      AUDIO_ENABLE_CONTROL                    0x01       Common Selector */
/*      AUDIO_MODE_SELECT_CONTROL               0x02       Common Selector */

/*  - 3D Stereo Extender Control Selectors */
/*      AUDIO_ENABLE_CONTROL                    0x01       Common Selector */
#define AUDIO_SPACIOUSNESS_CONTROL              0x02

/*  - Reverberation Control Selectors */
/*      AUDIO_ENABLE_CONTROL                    0x01       Common Selector */
#define AUDIO_REVERB_LEVEL_CONTROL              0x02
#define AUDIO_REVERB_TIME_CONTROL               0x03
#define AUDIO_REVERB_FEEDBACK_CONTROL           0x04

/*  - Chorus Control Selectors */
/*      AUDIO_ENABLE_CONTROL                    0x01       Common Selector */
#define AUDIO_CHORUS_LEVEL_CONTROL              0x02
#define AUDIO_SHORUS_RATE_CONTROL               0x03
#define AUDIO_CHORUS_DEPTH_CONTROL              0x04

/*  - Dynamic Range Compressor Control Selectors */
/*      AUDIO_ENABLE_CONTROL                    0x01       Common Selector */
#define AUDIO_COMPRESSION_RATE_CONTROL          0x02
#define AUDIO_MAX_AMPL_CONTROL                  0x03
#define AUDIO_THRESHOLD_CONTROL                 0x04
#define AUDIO_ATTACK_TIME_CONTROL               0x05
#define AUDIO_RELEASE_TIME_CONTROL              0x06

/*  Extension Unit Control Selectors */
/*      AUDIO_ENABLE_CONTROL                    0x01       Common Selector */

/*  Endpoint Control Selectors */
#define AUDIO_SAMPLING_FREQ_CONTROL             0x01
#define AUDIO_PITCH_CONTROL                     0x02


/* Audio Format Specific Control Selectors */

/*  MPEG Control Selectors */
#define AUDIO_MPEG_CONTROL_UNDEFINED            0x00
#define AUDIO_MPEG_DUAL_CHANNEL_CONTROL         0x01
#define AUDIO_MPEG_SECOND_STEREO_CONTROL        0x02
#define AUDIO_MPEG_MULTILINGUAL_CONTROL         0x03
#define AUDIO_MPEG_DYN_RANGE_CONTROL            0x04
#define AUDIO_MPEG_SCALING_CONTROL              0x05
#define AUDIO_MPEG_HILO_SCALING_CONTROL         0x06

/*  AC-3 Control Selectors */
#define AUDIO_AC3_CONTROL_UNDEFINED             0x00
#define AUDIO_AC3_MODE_CONTROL                  0x01
#define AUDIO_AC3_DYN_RANGE_CONTROL             0x02
#define AUDIO_AC3_SCALING_CONTROL               0x03
#define AUDIO_AC3_HILO_SCALING_CONTROL          0x04


/* Audio Format Types */
#define AUDIO_FORMAT_TYPE_UNDEFINED             0x00
#define AUDIO_FORMAT_TYPE_I                     0x01
#define AUDIO_FORMAT_TYPE_II                    0x02
#define AUDIO_FORMAT_TYPE_III                   0x03


/* Audio Format Type Descriptor Sizes */
#define AUDIO_FORMAT_TYPE_I_DESC_SZ(n)          0x08+(n*3)
#define AUDIO_FORMAT_TYPE_II_DESC_SZ(n)         0x09+(n*3)
#define AUDIO_FORMAT_TYPE_III_DESC_SZ(n)        0x08+(n*3)
#define AUDIO_FORMAT_MPEG_DESC_SIZE             0x09
#define AUDIO_FORMAT_AC3_DESC_SIZE              0x0A


/* Audio Data Format Codes */

/*  Audio Data Format Type I Codes */
#define AUDIO_FORMAT_TYPE_I_UNDEFINED           0x0000
#define AUDIO_FORMAT_PCM                        0x0001
#define AUDIO_FORMAT_PCM8                       0x0002
#define AUDIO_FORMAT_IEEE_FLOAT                 0x0003
#define AUDIO_FORMAT_ALAW                       0x0004
#define AUDIO_FORMAT_MULAW                      0x0005

/*  Audio Data Format Type II Codes */
#define AUDIO_FORMAT_TYPE_II_UNDEFINED          0x1000
#define AUDIO_FORMAT_MPEG                       0x1001
#define AUDIO_FORMAT_AC3                        0x1002

/*  Audio Data Format Type III Codes */
#define AUDIO_FORMAT_TYPE_III_UNDEFINED         0x2000
#define AUDIO_FORMAT_IEC1937_AC3                0x2001
#define AUDIO_FORMAT_IEC1937_MPEG1_L1           0x2002
#define AUDIO_FORMAT_IEC1937_MPEG1_L2_3         0x2003
#define AUDIO_FORMAT_IEC1937_MPEG2_NOEXT        0x2003
#define AUDIO_FORMAT_IEC1937_MPEG2_EXT          0x2004
#define AUDIO_FORMAT_IEC1937_MPEG2_L1_LS        0x2005
#define AUDIO_FORMAT_IEC1937_MPEG2_L2_3         0x2006


/* Predefined Audio Channel Configuration Bits */
#define AUDIO_CHANNEL_M                         0x0000  /* Mono */
#define AUDIO_CHANNEL_L                         0x0001  /* Left Front */
#define AUDIO_CHANNEL_R                         0x0002  /* Right Front */
#define AUDIO_CHANNEL_C                         0x0004  /* Center Front */
#define AUDIO_CHANNEL_LFE                       0x0008  /* Low Freq. Enhance. */
#define AUDIO_CHANNEL_LS                        0x0010  /* Left Surround */
#define AUDIO_CHANNEL_RS                        0x0020  /* Right Surround */
#define AUDIO_CHANNEL_LC                        0x0040  /* Left of Center */
#define AUDIO_CHANNEL_RC                        0x0080  /* Right of Center */
#define AUDIO_CHANNEL_S                         0x0100  /* Surround */
#define AUDIO_CHANNEL_SL                        0x0200  /* Side Left */
#define AUDIO_CHANNEL_SR                        0x0400  /* Side Right */
#define AUDIO_CHANNEL_T                         0x0800  /* Top */


/* Feature Unit Control Bits */
#define AUDIO_CONTROL_MUTE                      0x0001
#define AUDIO_CONTROL_VOLUME                    0x0002
#define AUDIO_CONTROL_BASS                      0x0004
#define AUDIO_CONTROL_MID                       0x0008
#define AUDIO_CONTROL_TREBLE                    0x0010
#define AUDIO_CONTROL_GRAPHIC_EQUALIZER         0x0020
#define AUDIO_CONTROL_AUTOMATIC_GAIN            0x0040
#define AUDIO_CONTROL_DEALY                     0x0080
#define AUDIO_CONTROL_BASS_BOOST                0x0100
#define AUDIO_CONTROL_LOUDNESS                  0x0200

/* Processing Unit Control Bits: */
#define AUDIO_CONTROL_ENABLE                    0x0001  /* Common Bit */
#define AUDIO_CONTROL_MODE_SELECT               0x0002  /* Common Bit */

/* - Up/Down-mix Control Bits */
/*      AUDIO_CONTROL_ENABLE                    0x0001     Common Bit */
/*      AUDIO_CONTROL_MODE_SELECT               0x0002     Common Bit */

/* - Dolby Prologic Control Bits */
/*      AUDIO_CONTROL_ENABLE                    0x0001     Common Bit */
/*      AUDIO_CONTROL_MODE_SELECT               0x0002     Common Bit */

/* - 3D Stereo Extender Control Bits */
/*      AUDIO_CONTROL_ENABLE                    0x0001     Common Bit */
#define AUDIO_CONTROL_SPACIOUSNESS              0x0002

/* - Reverberation Control Bits */
/*      AUDIO_CONTROL_ENABLE                    0x0001     Common Bit */
#define AUDIO_CONTROL_REVERB_TYPE               0x0002
#define AUDIO_CONTROL_REVERB_LEVEL              0x0004
#define AUDIO_CONTROL_REVERB_TIME               0x0008
#define AUDIO_CONTROL_REVERB_FEEDBACK           0x0010

/* - Chorus Control Bits */
/*      AUDIO_CONTROL_ENABLE                    0x0001     Common Bit */
#define AUDIO_CONTROL_CHORUS_LEVEL              0x0002
#define AUDIO_CONTROL_SHORUS_RATE               0x0004
#define AUDIO_CONTROL_CHORUS_DEPTH              0x0008

/* - Dynamic Range Compressor Control Bits */
/*      AUDIO_CONTROL_ENABLE                    0x0001     Common Bit */
#define AUDIO_CONTROL_COMPRESSION_RATE          0x0002
#define AUDIO_CONTROL_MAX_AMPL                  0x0004
#define AUDIO_CONTROL_THRESHOLD                 0x0008
#define AUDIO_CONTROL_ATTACK_TIME               0x0010
#define AUDIO_CONTROL_RELEASE_TIME              0x0020

/* Extension Unit Control Bits */
/*      AUDIO_CONTROL_ENABLE                    0x0001     Common Bit */

/* Endpoint Control Bits */
#define AUDIO_CONTROL_SAMPLING_FREQ             0x01
#define AUDIO_CONTROL_PITCH                     0x02
#define AUDIO_MAX_PACKETS_ONLY                  0x80


/* Audio Terminal Types */

/*  USB Terminal Types */
#define AUDIO_TERMINAL_USB_UNDEFINED            0x0100
#define AUDIO_TERMINAL_USB_STREAMING            0x0101
#define AUDIO_TERMINAL_USB_VENDOR_SPECIFIC      0x01FF

/*  Input Terminal Types */
#define AUDIO_TERMINAL_INPUT_UNDEFINED          0x0200
#define AUDIO_TERMINAL_MICROPHONE               0x0201
#define AUDIO_TERMINAL_DESKTOP_MICROPHONE       0x0202
#define AUDIO_TERMINAL_PERSONAL_MICROPHONE      0x0203
#define AUDIO_TERMINAL_OMNI_DIR_MICROPHONE      0x0204
#define AUDIO_TERMINAL_MICROPHONE_ARRAY         0x0205
#define AUDIO_TERMINAL_PROCESSING_MIC_ARRAY     0x0206

/*  Output Terminal Types */
#define AUDIO_TERMINAL_OUTPUT_UNDEFINED         0x0300
#define AUDIO_TERMINAL_SPEAKER                  0x0301
#define AUDIO_TERMINAL_HEADPHONES               0x0302
#define AUDIO_TERMINAL_HEAD_MOUNTED_AUDIO       0x0303
#define AUDIO_TERMINAL_DESKTOP_SPEAKER          0x0304
#define AUDIO_TERMINAL_ROOM_SPEAKER             0x0305
#define AUDIO_TERMINAL_COMMUNICATION_SPEAKER    0x0306
#define AUDIO_TERMINAL_LOW_FREQ_SPEAKER         0x0307

/*  Bi-directional Terminal Types */
#define AUDIO_TERMINAL_BIDIRECTIONAL_UNDEFINED  0x0400
#define AUDIO_TERMINAL_HANDSET                  0x0401
#define AUDIO_TERMINAL_HEAD_MOUNTED_HANDSET     0x0402
#define AUDIO_TERMINAL_SPEAKERPHONE             0x0403
#define AUDIO_TERMINAL_SPEAKERPHONE_ECHOSUPRESS 0x0404
#define AUDIO_TERMINAL_SPEAKERPHONE_ECHOCANCEL  0x0405

/*  Telephony Terminal Types */
#define AUDIO_TERMINAL_TELEPHONY_UNDEFINED      0x0500
#define AUDIO_TERMINAL_PHONE_LINE               0x0501
#define AUDIO_TERMINAL_TELEPHONE                0x0502
#define AUDIO_TERMINAL_DOWN_LINE_PHONE          0x0503

/*  External Terminal Types */
#define AUDIO_TERMINAL_EXTERNAL_UNDEFINED       0x0600
#define AUDIO_TERMINAL_ANALOG_CONNECTOR         0x0601
#define AUDIO_TERMINAL_DIGITAL_AUDIO_INTERFACE  0x0602
#define AUDIO_TERMINAL_LINE_CONNECTOR           0x0603
#define AUDIO_TERMINAL_LEGACY_AUDIO_CONNECTOR   0x0604
#define AUDIO_TERMINAL_SPDIF_INTERFACE          0x0605
#define AUDIO_TERMINAL_1394_DA_STREAM           0x0606
#define AUDIO_TERMINAL_1394_DA_STREAM_TRACK     0x0607

/*  Embedded Function Terminal Types */
#define AUDIO_TERMINAL_EMBEDDED_UNDEFINED       0x0700
#define AUDIO_TERMINAL_CALIBRATION_NOISE        0x0701
#define AUDIO_TERMINAL_EQUALIZATION_NOISE       0x0702
#define AUDIO_TERMINAL_CD_PLAYER                0x0703
#define AUDIO_TERMINAL_DAT                      0x0704
#define AUDIO_TERMINAL_DCC                      0x0705
#define AUDIO_TERMINAL_MINI_DISK                0x0706
#define AUDIO_TERMINAL_ANALOG_TAPE              0x0707
#define AUDIO_TERMINAL_PHONOGRAPH               0x0708
#define AUDIO_TERMINAL_VCR_AUDIO                0x0709
#define AUDIO_TERMINAL_VIDEO_DISC_AUDIO         0x070A
#define AUDIO_TERMINAL_DVD_AUDIO                0x070B
#define AUDIO_TERMINAL_TV_TUNER_AUDIO           0x070C
#define AUDIO_TERMINAL_SATELLITE_RECEIVER_AUDIO 0x070D
#define AUDIO_TERMINAL_CABLE_TUNER_AUDIO        0x070E
#define AUDIO_TERMINAL_DSS_AUDIO                0x070F
#define AUDIO_TERMINAL_RADIO_RECEIVER           0x0710
#define AUDIO_TERMINAL_RADIO_TRANSMITTER        0x0711
#define AUDIO_TERMINAL_MULTI_TRACK_RECORDER     0x0712
#define AUDIO_TERMINAL_SYNTHESIZER              0x0713


#endif  /* __AUDIO_H__ */
