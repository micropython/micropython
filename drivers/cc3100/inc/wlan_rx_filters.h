/*
 * wlan_rx_filters.h - CC31xx/CC32xx Host Driver Implementation
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

#ifndef RX_FILTERS_PREPROCESSOR_CLI_IF_H_
#define RX_FILTERS_PREPROCESSOR_CLI_IF_H_



#ifdef  __cplusplus
extern "C" {
#endif


/*****************************************************************************/
/* Macro declarations                                                                        */
/*****************************************************************************/

/*!
 * \def SL_RX_FILTER_MAX_FILTERS
 *  The Max number of filters for 64 filters
 */
#define SL_RX_FILTER_MAX_FILTERS 64

/*!
 * \def SL_RX_FILTER_MAX_PRE_PREPARED_FILTERS_SETS
 * The Max number of software filters
 */
#define SL_RX_FILTER_MAX_PRE_PREPARED_FILTERS_SETS      (32)
/*!
 * \def SL_RX_FILTER_NUM_OF_FILTER_HEADER_ARGS
 *
 */
#define SL_RX_FILTER_NUM_OF_FILTER_HEADER_ARGS          (2)
/*!
 * \def SL_RX_FILTER_NUM_OF_FILTER_PAYLOAD_ARGS
 *
 */
#define SL_RX_FILTER_NUM_OF_FILTER_PAYLOAD_ARGS         (2)
/*!
 * \def SL_RX_FILTER_NUM_OF_FILTER_PAYLOAD_ARGS
 *
 */
#define SL_RX_FILTER_NUM_OF_COMBINATION_TYPE_ARGS       (2)
/*!
 * \def SL_RX_FILTER_LENGTH_OF_REGX_PATTERN_LENGTH
 *
 */
#define SL_RX_FILTER_LENGTH_OF_REGX_PATTERN_LENGTH      (32)


/*  List of possible error numbers */
#define RXFL_OK                                                (0) /*  O.K */
#define RXFL_OUTPUT_OR_INPUT_BUFFER_LENGTH_TOO_SMALL           (76) /* ! The output buffer length is smaller than required for that operation */
#define RXFL_DEPENDENT_FILTER_SOFTWARE_FILTER_NOT_FIT          (75) /* Node filter can't be child of software filter and vice_versa */
#define RXFL_DEPENDENCY_IS_NOT_PERSISTENT                      (74) /*  Dependency filter is not persistent */ 
#define RXFL_SYSTEM_STATE_NOT_SUPPORTED_FOR_THIS_FILTER        (72) /*  System state is not supported */
#define RXFL_TRIGGER_USE_REG5_TO_REG8                          (71) /*  Only counters 5 - 8 are allowed, for Tigger */
#define RXFL_TRIGGER_USE_REG1_TO_REG4                          (70) /*  Only counters 1 - 4 are allowed, for trigger */
#define RXFL_ACTION_USE_REG5_TO_REG8                           (69) /*  Only counters 5 - 8 are allowed, for action */
#define RXFL_ACTION_USE_REG1_TO_REG4                           (68) /*  Only counters 1 - 4 are allowed, for action */
#define RXFL_FIELD_SUPPORT_ONLY_EQUAL_AND_NOTEQUAL             (67) /*  Rule compare function Id is out of range */
#define RXFL_WRONG_MULTICAST_BROADCAST_ADDRESS                 (66) /*  The address should be of type mutlicast or broadcast */
#define RXFL_THE_FILTER_IS_NOT_OF_HEADER_TYPE                  (65) /*  The filter should be of header type */
#define RXFL_WRONG_COMPARE_FUNC_FOR_BROADCAST_ADDRESS          (64) /*  The compare funcion is not suitable for broadcast address */
#define RXFL_WRONG_MULTICAST_ADDRESS                           (63) /*  The address should be of muticast type */
#define RXFL_DEPENDENT_FILTER_IS_NOT_PERSISTENT                (62) /*  The dependency filter is not persistent */
#define RXFL_DEPENDENT_FILTER_IS_NOT_ENABLED                   (61) /*  The dependency filter is not enabled */
#define RXFL_FILTER_HAS_CHILDS                                 (60) /*  The filter has childs and can't be removed */
#define RXFL_CHILD_IS_ENABLED                                  (59) /*  Can't disable filter while the child is enabled */
#define RXFL_DEPENDENCY_IS_DISABLED                            (58) /*  Can't enable filetr in case its depndency filter is disabled */
#define RXFL_NUMBER_OF_CONNECTION_POINTS_EXCEEDED              (52) /*  Number of connection points exceeded */
#define RXFL_DEPENDENT_FILTER_DEPENDENCY_ACTION_IS_DROP        (51) /*  The dependent filter has Drop action, thus the filter can't be created */
#define RXFL_FILTER_DO_NOT_EXISTS                              (50) /*  The filter doesn't exists */
#define RXFL_DEPEDENCY_NOT_ON_THE_SAME_LAYER                   (49) /*  The filter and its dependency must be on the same layer */
#define RXFL_NUMBER_OF_ARGS_EXCEEDED                           (48) /*  Number of arguments excceded */
#define RXFL_ACTION_NO_REG_NUMBER                              (47) /*  Action require counter number */
#define RXFL_DEPENDENT_FILTER_LAYER_DO_NOT_FIT                 (46) /*  the filter and its dependency should be from the same layer */
#define RXFL_DEPENDENT_FILTER_SYSTEM_STATE_DO_NOT_FIT          (45) /*  The filter and its dependency system state don't fit  */
#define RXFL_DEPENDENT_FILTER_DO_NOT_EXIST_2                   (44) /*  The parent filter don't exist  */
#define RXFL_DEPENDENT_FILTER_DO_NOT_EXIST_1                   (43) /*  The parent filter is null */
#define RXFL_RULE_HEADER_ACTION_TYPE_NOT_SUPPORTED             (42) /*  The action type is not supported */
#define RXFL_RULE_HEADER_TRIGGER_COMPARE_FUNC_OUT_OF_RANGE     (41) /*  The Trigger comparision function is out of range */
#define RXFL_RULE_HEADER_TRIGGER_OUT_OF_RANGE                  (40) /*  The Trigger is out of range */
#define RXFL_RULE_HEADER_COMPARE_FUNC_OUT_OF_RANGE             (39) /*  The rule compare function is out of range */
#define RXFL_FRAME_TYPE_NOT_SUPPORTED                          (38) /*  ASCII frame type string is illegal */
#define RXFL_RULE_FIELD_ID_NOT_SUPPORTED                       (37) /*  Rule field ID is out of range */
#define RXFL_RULE_HEADER_FIELD_ID_ASCII_NOT_SUPPORTED          (36) /*  This ASCII field ID is not supported */
#define RXFL_RULE_HEADER_NOT_SUPPORTED                         (35) /*  The header rule is not supported on current release */
#define RXFL_RULE_HEADER_OUT_OF_RANGE                          (34) /*  The header rule is out of range */
#define RXFL_RULE_HEADER_COMBINATION_OPERATOR_OUT_OF_RANGE     (33) /*  Combination function Id is out of ramge */
#define RXFL_RULE_HEADER_FIELD_ID_OUT_OF_RANGE                 (32) /*  rule field Id is out of range */
#define RXFL_UPDATE_NOT_SUPPORTED                              (31) /*  Update not supported */
#define RXFL_NO_FILTERS_ARE_DEFINED                            (24) /*  No filters are defined in the system */
#define RXFL_NUMBER_OF_FILTER_EXCEEDED                         (23) /*  Number of max filters excceded */


/******************************************************************************/
/* Type declarations                                                                         */
/******************************************************************************/

/*!


 * \typedef SlrxFilterID_t
 * Unique filter ID which is allocated by the system , negative number means error
 */
typedef  _i8    SlrxFilterID_t;


/*!
 * \typedef SlrxFilterCompareMask_t
 * The mask is used for the rule comparison function
 */
typedef _u8   SlrxFilterCompareMask_t;

/*!
 * \typedef SlrxFilterIdMask_t
 *  Representation of filters Id as a bit field
 *  The bit field is used to declare which filters are involved
 *  in operation. Number of filter can be up to 128 filters. i.e. 128 bits are needed.
 *  On the current release, up to 64 filters can be defined.
 */
typedef _u8   SlrxFilterIdMask_t[128/8];

/*!
 * \typedef SlrxFilterPrePreparedFilters_t
 *  Describes the supported software filter sets,
 */
typedef _u8  SlrxFilterPrePreparedFilters_t;
#define SL_ARP_AUTO_REPLY_PRE_PREPARED_FILTERS       (0)
#define SL_MULTICASTSIPV4_DROP_PREPREPARED_FILTERS   (1)
#define SL_MULTICASTSIPV6_DROP_PREPREPARED_FILTERS   (2)
#define SL_MULTICASTSWIFI_DROP_PREPREPARED_FILTERS   (3)



/*!
 * \typedef SlrxFilterPrePreparedFiltersMask_t
 *  Describes the supported software filter sets,
 *  each bit represents different software filter set
 *  The filter sets are defined at SlrxFilterPrePreparedFilters_t
 */
typedef _u8   SlrxFilterPrePreparedFiltersMask_t[SL_RX_FILTER_MAX_PRE_PREPARED_FILTERS_SETS/8];


/*! \typedef SlrxFilterRegxPattern_t
 * The struct contains the regular expression pattern which is used in case of payload rule.
 * Not supported in the current release
 */
typedef struct SlrxFilterRegxPattern_t
{
    _u8 x[SL_RX_FILTER_LENGTH_OF_REGX_PATTERN_LENGTH];
}SlrxFilterRegxPattern_t;


/*! \typedef SlrxFilterAsciiArg_t
 * The buffer is used to provide container for ASCII argument, which may be used in case of HEADER rule.
 * example for ASCII argument can be : IP = 256.0.67.1
 */
typedef _u8 SlrxFilterAsciiArg_t;


/*! \typedef SlrxFilterBinaryArg_t
 * The buffer provides container for binary argument, which  may be used in case of HEADER rule
 */
typedef _u8   SlrxFilterBinaryArg_t ;


/*! \typedef SlrxFilterActionArg_t
 *  Provides container for the filter action argument.
 *  for example: in case action is to send automatic response , the argument is the template to be used for the automatic response.
 *
 *
 */
typedef  _u8 SlrxFilterActionArg_t ;



/*! \typedef SlrxFilterOffset_t
 * The offset relative to the packet payload start location.
 * Not supported on current release
 */
typedef _u32   SlrxFilterOffset_t;



/*! \typedef SlrxFilterRuleType_t
 * Enumerates the different filter types.
 * On the current release only HEADER and COMBINATION are supported.
 */
typedef _u8 SlrxFilterRuleType_t;
/* possible values for SlrxFilterRuleType_t */
#define HEADER                    (0)
#define COMBINATION               (1)
#define EXACT_PATTERN             (2)
#define LIKELIHOOD_PATTERN        (3)
#define ALWAYS_TRUE               (4)
#define NUM_OF_FILTER_TYPES       (5)


/*! \typedef SlrxFilterFlags_t
 * Bit field which sets the behaviour of the RX filter
 *
 */

#define RX_FILTER_BINARY          (0x1)
#define RX_FILTER_PERSISTENT      (0x8)
#define RX_FILTER_ENABLE          (0x10)

typedef union SlrxFilterFlags_t
{

    /* struct
    { */
        /*!
         *  The filter argument can be set as binary argument or ASCII arguments.
         *  When the bit is on the argument are binary.
         */
        /* _u8 Binary: 1; */
        /*!
         * 
         */
        /* _u8 AutoSort : 1; */
        /*!
         * 
         */
        /* _u8 AutoFaultDetect : 1; */
        /*!
         *  When the bit is on it means the the node is enabled .
        */
        /* _u8 Enabled : 1; */
        /* _u8 padding : 3; */
    /*
    };*/

    _u8 IntRepresentation;

}SlrxFilterFlags_t;

/*! \typedef SlrxFilterCompareFunction_t
 * Used as comparison function for the header type arguments
 *
 */
typedef _u8 SlrxFilterCompareFunction_t;
/* Possible values for SlrxFilterCompareFunction_t */
#define COMPARE_FUNC_IN_BETWEEN                 (0)
#define COMPARE_FUNC_EQUAL                      (1)
#define COMPARE_FUNC_NOT_EQUAL_TO               (2)
#define COMPARE_FUNC_NOT_IN_BETWEEN             (3)
#define COMPARE_FUNC_NUM_OF_FILTER_COMPARE_FUNC (4)

/*! \typedef SlrxFilterCompareFunction_t
 * Used as comparison function for the header type arguments
 *
 */
typedef _u8 SlrxTriggerCompareFunction_t;
/* Possible values for SlrxTriggerCompareFunction_t */
#define TRIGGER_COMPARE_FUNC_EQUAL              (0)
/*   arg1 == protocolVal ,not supported in current release */
#define TRIGGER_COMPARE_FUNC_NOT_EQUAL_TO       (1)
/*   arg1 == protocolVal */
#define TRIGGER_COMPARE_FUNC_SMALLER_THAN       (2)
/*   arg1 == protocolVal */
#define TRIGGER_COMPARE_FUNC_BIGGER_THAN        (3)
/* definition */
#define TRIGGER_COMPARE_FUNC_NUM_OF_FILTER_COMPARE_FUNC (4)


/*! \typedef SlrxFilterHdrField_t
 * Provides list of possible header types which may be defined as part of the rule
 *
 */
typedef _u8 SlrxFilterHdrField_t;
/* Possible values for SlrxFilterHdrField_t */
#define NULL_FIELD_ID_TYPE           (0)
/* 802.11 control\data\management */
#define FRAME_TYPE_FIELD             (1)
/*  802.11 beacon\probe\.. */
#define FRAME_SUBTYPE_FIELD          (2)
 /*  802.11 bssid type */
#define BSSID_FIELD                  (3)
 /* */
#define MAC_SRC_ADDRESS_FIELD        (4)
 /* */
#define MAC_DST_ADDRESS_FIELD        (5)
/* */
#define FRAME_LENGTH_FIELD           (6)
/* */
#define PROTOCOL_TYPE_FIELD          (7)
 /* */
#define IP_VERSION_FIELD             (8)
 /* TCP / UDP */
#define IP_PROTOCOL_FIELD            (9)
 /* */
#define IPV4_SRC_ADRRESS_FIELD       (10)
/* */
#define IPV4_DST_ADDRESS_FIELD       (11)
/* */
#define IPV6_SRC_ADRRESS_FIELD       (12)
/* */
#define IPV6_DST_ADDRESS_FIELD       (13)
 /* */
#define SRC_PORT_FIELD               (14)
 /* */
#define DST_PORT_FIELD               (15)
 /* Definition */
#define NUM_OF_FIELD_NAME_FIELD      (16)

/*! \union SlrxFilterHeaderArg_t
 * The structure holds the header ARGS which are used in case of HDR rule.
 */
/* -- 36 bytes */
typedef union SlrxFilterHeaderArg_t
{
    /*----------------------------- Large size ---------------------------------*/
    /*! buffer for binary arguments, number of argument may be up to SL_RX_FILTER_NUM_OF_FILTER_HEADER_ARGS
     * example : IPV6 16 bytes, 39 characters
     * ipv6 Ascii address: 2001:0db8:3c4d:0015:0000:0000:abcd:ef12
     */

    SlrxFilterBinaryArg_t RxFilterDB16BytesRuleArgs[SL_RX_FILTER_NUM_OF_FILTER_HEADER_ARGS][16 ]; /*  Binary Values for comparition */
    /*! buffer for ASCII arguments, number of argument may be up to SL_RX_FILTER_NUM_OF_FILTER_HEADER_ARGS
     * example : IPV6 16 bytes, 39 characters
     * ipv6 Ascii address: 2001:0db8:3c4d:0015:0000:0000:abcd:ef12
     * Ascii format for ipV6 is not supported
     */
    /*----------------------------- Medium size ---------------------------------*/
    /*! buffer for binary arguments, number of argument may be up to SL_RX_FILTER_NUM_OF_FILTER_HEADER_ARGS
     * MAC address: 6 bytes, 17 chars
     */
    SlrxFilterBinaryArg_t RxFilterDB6BytesRuleArgs[SL_RX_FILTER_NUM_OF_FILTER_HEADER_ARGS][6]; /*  Binary Values for comparition */
    /*!
     * ! buffer for ASCII arguments, number of argument may be up to SL_RX_FILTER_NUM_OF_FILTER_HEADER_ARGS
     *  IP address: 4 bytes, 15 chars
     *  2 bytes are added for padding
     */
    SlrxFilterAsciiArg_t RxFilterDB18BytesAsciiRuleArgs[SL_RX_FILTER_NUM_OF_FILTER_HEADER_ARGS][18]; /* Ascii Values for comparison */
    /*----------------------------- Small size ---------------------------------*/
    /*! buffer for binary arguments, number of argument may be up to SL_RX_FILTER_NUM_OF_FILTER_HEADER_ARGS
     *  IP address: 4 bytes, 15 chars
     *  Port: 2 bytes, chars: 5 chars
     */
    SlrxFilterBinaryArg_t RxFilterDB4BytesRuleArgs[SL_RX_FILTER_NUM_OF_FILTER_HEADER_ARGS][4]; /*  Binary Values for comparison */
    /*! buffer for ASCII arguments, number of argument may be up to SL_RX_FILTER_NUM_OF_FILTER_HEADER_ARGS
     * Port: 2 bytes, chars: 5 chars
     */
    SlrxFilterAsciiArg_t RxFilterDB5BytesRuleAsciiArgs[SL_RX_FILTER_NUM_OF_FILTER_HEADER_ARGS][5]; /* Ascii Values for comparison */
    /*----------------------------- 1 byte size ---------------------------------*/
    /*! buffer for binary arguments, number of argument may be up to SL_RX_FILTER_NUM_OF_FILTER_HEADER_ARGS
     */
    SlrxFilterBinaryArg_t RxFilterDB1BytesRuleArgs[SL_RX_FILTER_NUM_OF_FILTER_HEADER_ARGS][1]; /*  Binary Values for comparison */
}SlrxFilterHeaderArg_t;



/*! \typedef SlrxFilterRuleHeaderArgsAndMask_t
 * Structure which defines the Header Args and mask
 */
/* -- 52 bytes */
typedef struct SlrxFilterRuleHeaderArgsAndMask_t
{
    /*! Argument for the comparison function */
    /* -- 36 byte */
    SlrxFilterHeaderArg_t RuleHeaderArgs;

    /*! the mask is used in order to enable partial comparison,
     *  Use the 0xFFFFFFFF in case you don't want to use mask
     */
    /* -- 16 bytes */
    SlrxFilterCompareMask_t RuleHeaderArgsMask[16];

}SlrxFilterRuleHeaderArgsAndMask_t;

/*! \typedef SlrxFilterHeaderType_t
 * Structure which defines the Header rule
 * The header rule defines compare function on the protocol header
 * For example destMacAddre is between ( 12:6::78:77,  12:6::78:90 )
 *
 */
/* -- 56 byte */
typedef struct SlrxFilterHeaderType_t
{
    /*! see :: SlrxFilterRuleHeaderArgsAndMask_t */
    /* -- 52 bytes */ 
    SlrxFilterRuleHeaderArgsAndMask_t RuleHeaderArgsAndMask;

    /*! Packet HDR field which will be compared to the argument */
    /* -- 1 byte */
    SlrxFilterHdrField_t RuleHeaderfield;

    /* -- 1 byte */
    /*! type of the comparison function
     * see :: SlrxFilterCompareFunction_t
     */
    SlrxFilterCompareFunction_t RuleCompareFunc;

    /*! padding */
    /* -- 2 bytes */
    _u8 RulePadding[2];

}SlrxFilterHeaderType_t;

/*! \typedef SlrxFilterPayloadType_t
 *  Structure which defines payload rule.
 *  Not supported on current release.
 */
/* -- 40 byte  */
typedef struct SlrxFilterPayloadType_t
{
    /*! Not supported on current release */
    /* -- 32 byte */
    SlrxFilterRegxPattern_t RegxPattern;
    /*! Not supported on current release */
    /* -- 4 byte */
    SlrxFilterOffset_t LowerOffset;
    /*! Not supported on current release */
    /* -- 4 byte */
    SlrxFilterOffset_t UpperOffset;
}SlrxFilterPayloadType_t;

/*! \typedef SlrxFilterCombinationTypeOperator_t
 * Enumerate the optional operators for the combination type
 * filterID1 is located in  the first arg , filterId2 is the second arg,see ::SlrxFilterCombinationType_t.CombinationFilterId
 */
typedef _u8 SlrxFilterCombinationTypeOperator_t;
/* Possible values for SlrxFilterCombinationTypeOperator_t */
/*! !filterID1 */
#define COMBINED_FUNC_NOT     (0)
/*! filterID1 && filterID2 */
#define COMBINED_FUNC_AND     (1)
/*! filterID1 && filterID2 */
#define COMBINED_FUNC_OR      (2)

/*! \typedef SlrxFilterCombinationType_t
 * Defines the structure which define the combination type filter
 * The combined filter enable to make operation on one or two filter,
 * for example !filterId1 or and(filterId2,filterId3).
 *
 */
/* -- 4  byte */
typedef struct SlrxFilterCombinationType_t
{
    /* ! combination operator */
    /*  -- 1 byte */
    SlrxFilterCombinationTypeOperator_t CombinationTypeOperator;
    /* ! filterID, may be one or two depends on the combination operator type */
    /*  -- 2  byte */
    SlrxFilterID_t CombinationFilterId[SL_RX_FILTER_NUM_OF_COMBINATION_TYPE_ARGS];
    /* ! Padding */
    /*  -- 1  byte */
    _u8 Padding;
}SlrxFilterCombinationType_t;


/*! \typedef SlrxFilterRule_t
 * Rule structure composed of behavioral flags and the filter rule definitions
 *
 */
/*  -- 56  byte */
typedef union SlrxFilterRule_t
{
    /* ! Header type rule , see explanation on the ::SlrxFilterHeaderType_t structure */
    /*  -- 56 byte */
    SlrxFilterHeaderType_t HeaderType;
    /* ! Payload rule, not supported in current release */
    /*  -- 40 byte */
    SlrxFilterPayloadType_t PayLoadHeaderType; /* future for exact pattern or like hood pattern */
    /* ! Combined type rule , see explanation in ::SlrxFilterCombinationType_t structure */
    /*  -- 4  byte */
    SlrxFilterCombinationType_t CombinationType;
}SlrxFilterRule_t;

/*! \typedef SlrxFilterTriggerRoles_t
 * Bit field which represents the roleId possible values
 * In the current release only Station/AP roles are supported.
 */
#define RX_FILTER_ROLE_AP                            (1)
#define RX_FILTER_ROLE_STA                           (2)
#define RX_FILTER_ROLE_PROMISCUOUS                   (4)
#define RX_FILTER_ROLE_NULL                          (0)

typedef union SlrxFilterTriggerRoles_t
{
/*     struct                                                            */
/*     {                                                                 */
/*         _u8 RoleAP :1;                                      */
/*         _u8 RoleStation :1;                                 */
      /* The filter is activated only in Promiscuous mode          */
/*         _u8 PromiscuousMode :1;                             */
/*         _u8 RoleReserved :5;                                */
/*     };                                                                */
    /* ! Bit fiels of the Filter role    */   
    _u8 IntRepresentation;

}SlrxFilterTriggerRoles_t;

/*! \typedef SlrxFilterTriggerConnectionStates_t
 * Bit field representing the possible values of the When section of the rule
 *
 */
#define RX_FILTER_CONNECTION_STATE_STA_CONNECTED     (1)
#define RX_FILTER_CONNECTION_STATE_STA_NOT_CONNECTED (2)
#define RX_FILTER_CONNECTION_STATE_STA_HAS_IP        (4)
#define RX_FILTER_CONNECTION_STATE_STA_HAS_NO_IP     (8)

typedef union SlrxFilterTriggerConnectionStates_t
{
/*     struct */
/*     {      */
/*         _u8 RoleStationWiFiConnected :1;  */
/*         _u8 RoleStationWiFiDisconneted:1; */
/*         _u8 RoleStationWiFiHasIp:1;       */
/*         _u8 RoleStationWiFiHasNoIp:1;     */
/*         _u8 RoleStationWiFiSocketOpened:1; */
/*         _u8 RoleStationWiFiSocketclosed:1; */
/*     }; */
/*  */
    /* ! */
    _u8 IntRepresentation;
        
}SlrxFilterTriggerConnectionStates_t;

/*! \typedef SlrxFilterDBTriggerArg_t
 * Provides container for entering the filter 'when' argument.
 * The current release support 'When rules' which has no arguments.
 * For example :
 *  When connect to specific AP -- the AP bssid is the argument.
 *
 */
typedef _u32  SlrxFilterDBTriggerArg_t;



/*! \typedef  SlrxFilterCounterId_t
 * the counter ID we have 4 counters
 */
typedef _u8 SlrxFilterCounterId_t;
/* Possible values for SlrxFilterCounterId_t */
#define NO_TRIGGER                                  (0)
#define RX_FILTER_COUNTER1                          (1)
#define RX_FILTER_COUNTER2                          (2)
#define RX_FILTER_COUNTER3                          (3)
#define RX_FILTER_COUNTER4                          (4)
#define RX_FILTER_COUNTER5                          (5)
#define RX_FILTER_COUNTER6                          (6)
#define RX_FILTER_COUNTER7                          (7)
#define RX_FILTER_COUNTER8                          (8)
#define MAX_RX_FILTER_COUNTER                       (9)



/*! \typedef SlrxFilterActionArgs_t
 * Possible value for filter action args
 *
 */

typedef _u8  SlrxFilterActionArgs_t;
/* Possible values for SlrxFilterActionArgs_t */
#define ACTION_ARG_REG_1_4                          (0)
    /* ! Can be use as counter */
#define ACTION_ARG_TEMPLATE                         (1)
    /* ! Can be use as counter */
#define ACTION_ARG_EVENT                            (2)

/* ! GPIO number */
#define ACTION_ARG_GPIO                             (4)
/*!
 * \def SL_RX_FILTER_NUM_OF_BYTES_FOR_ACTIONS_ARGS
 *
 */
#define SL_RX_FILTER_NUM_OF_BYTES_FOR_ACTIONS_ARGS  (5)




/*! \typedef SlrxFilterTrigger_t
 * The filter trigger, determine when the filter is triggered,
 * The filter is triggered in the following condition :\n
 * 1. The filter parent is triggered\n
 * 2. The requested connection type exists, i.e. wlan_connect\n
 * 3. The filter role is the same as the system role\n
 *
 */
/*  --  12 byte */
typedef struct SlrxFilterTrigger_t
{
    /*! The parent filter ID, this is the way to build filter tree. */
    /*  NULL           value means  tree root.
    */
    /*  --  1 byte */
    SlrxFilterID_t ParentFilterID;
    /* ! See ::SlrxFilterCounterId_t explanation */
    /*  -- 1 byte */
    SlrxFilterCounterId_t Trigger;
    /* ! See :: SlrxFilterTriggerConnectionStates_t */
    /*  -- 1 byte */
    SlrxFilterTriggerConnectionStates_t TriggerArgConnectionState;
    /* ! See ::SlrxFilterTriggerRoles_t */
    /*  -- 1 byte */
    SlrxFilterTriggerRoles_t TriggerArgRoleStatus;
    /* ! The Trigger arguments are in the same order as the Trigger bit field order. */
    /*  --  4 byte */
    SlrxFilterDBTriggerArg_t TriggerArg;
    /** The compare function which will be operate for each bit that is turned on in the ::SlrxFilterTrigger_t.Trigger field,
     * for example , in case the second bit in the Trigger function is on the second function in the list will be executed.
     *
    */
    /*  --  1 byte */
    SlrxTriggerCompareFunction_t TriggerCompareFunction;

    /* ! padding */
    /*  --  3 byte */
    _u8 Padding[3];
} SlrxFilterTrigger_t;

/*! \typedef SlrxFilterActionType_t
 *  The actions are executed only if the filter is matched,\n
 *  In case of false match the packet is transfered to the HOST. \n
 *  The action is composed of bit field structure,
 *  up to 2 actions can be defined per filter.
 *
 */
#define RX_FILTER_ACTION_NULL               (0x0)
#define RX_FILTER_ACTION_DROP               (0x1)
#define RX_FILTER_ACTION_GPIO               (0x2)
#define RX_FILTER_ACTION_ON_REG_INCREASE    (0x4)
#define RX_FILTER_ACTION_ON_REG_DECREASE    (0x8)
#define RX_FILTER_ACTION_ON_REG_RESET       (0x10)
#define RX_FILTER_ACTION_SEND_TEMPLATE      (0x20) /* unsupported */
#define RX_FILTER_ACTION_EVENT_TO_HOST      (0x40) /* unsupported */

typedef union SlrxFilterActionType_t
{
/*     struct */
/*     { */
         /* ! No action to execute the packet is dropped,drop is always on leaf. */
         /* ! If not dropped ,The packet is passed to the next filter or in case it is the last filter to the host */
/*         _u8 ActionDrop : 1; */
         /* ! Not Supported in the current release */
/*         _u8 ActionGpio : 1; */
         /*! action can increase counter registers.
          *  1 = Increase
          *  2 = decrease
          *  3 = reset
          */
/*         _u8 ActionOnREGIncrease : 1; */
/*         _u8 ActionOnREGDecrease : 1; */
/*         _u8 ActionOnREGReset : 1;    */

         /* ! Not Supported in the current release */
/*         _u8 ActionSendTemplate : 1; */
         /* ! Not Supported in the current release */
/*         _u8 ActionEventToHost: 1;   */
/*         _u8 padding: 1;             */
/*     }; */

    _u8 IntRepresentation;

}SlrxFilterActionType_t;

/*! \typedef SlrxFilterAction_t
 * Several actions can be defined,\n
 * The action is executed in case the filter rule is matched.
 */
/*  -- 8 byte   */
typedef struct SlrxFilterAction_t
{
    /*  -- 1 byte */
    /* ! Determine which actions are supported */
    SlrxFilterActionType_t ActionType;
    /* ! Buffer for the action arguments */
    /**
     * <b>location 0</b> - The counter to increase
     *     In case the action is of type "increase" the arg will contain the counter number,
     *     The counter number values are as in  ::SlrxFilterCounterId_t.\n
     * <b>location 1</b> - The template arg.\n
     * <b>location 2</b> - The event arg.\n
     *
     */
    /*  -- 5 byte */
    SlrxFilterActionArg_t ActionArg[SL_RX_FILTER_NUM_OF_BYTES_FOR_ACTIONS_ARGS];

    /* ! Padding */
    /* - 2 Bytes */
    _u8 Padding[2];

} SlrxFilterAction_t;


/*! \struct _WlanRxFilterOperationCommandBuff_t
 *  The structure is used for the interface HOST NWP.\n
 *  The supported operation : \n
 *   ::ENABLE_DISABLE_RX_FILTER,\n
 *   ::REMOVE_RX_FILTER,\n
 *
 */
/*  20 bytes */
typedef struct _WlanRxFilterOperationCommandBuff_t
{
    /*  -- 16 bytes */
    SlrxFilterIdMask_t FilterIdMask;
    /*  4 bytes */
    _u8 Padding[4];
}_WlanRxFilterOperationCommandBuff_t;



/* -- 56 bytes */
typedef struct _WlanRxFilterUpdateArgsCommandBuff_t
{
    /*  -- 1 bytes */
    _u8  FilterId;

    /*  -- 1 bytes */
    /* ! the args representation */
    _u8 BinaryRepresentation;

    /*  -- 52 byte */
    SlrxFilterRuleHeaderArgsAndMask_t FilterRuleHeaderArgsAndMask;

    /* -- 2 bytes */
    _u8 Padding[2];
}_WlanRxFilterUpdateArgsCommandBuff_t;


/*! \typedef _WlanRxFilterRetrieveEnableStatusCommandResponseBuff_t
 *  The structure is used for the interface HOST NWP.\n
 *
 */
/* -- 16 bytes */
typedef struct _WlanRxFilterRetrieveEnableStatusCommandResponseBuff_t
{

    /* ! the filter set bit map */
    /*  -- 16 bytes             */
    SlrxFilterIdMask_t FilterIdMask;

}_WlanRxFilterRetrieveEnableStatusCommandResponseBuff_t;


/*! \struct _WlanRxFilterPrePreparedFiltersCommandBuff_t
 *  The function enables to perform operations on pre-prepared filters
 *
 */
typedef struct _WlanRxFilterPrePreparedFiltersCommandBuff_t
{
    /* ! the filter set bit map */
    /*  -- 4 bytes */
    SlrxFilterPrePreparedFiltersMask_t  FilterPrePreparedFiltersMask;

}_WlanRxFilterPrePreparedFiltersCommandBuff_t;


/*! \typedef sl_protocol_WlanRxFilterPrePreparedFiltersCommandResponseBuff_t
 *
 */
/*-- 4 bytes  */
typedef struct _WlanRxFilterPrePreparedFiltersCommandResponseBuff_t
{
    /* -- 4 bytes */
    /* ! the filter set bit map */
    SlrxFilterPrePreparedFiltersMask_t  FilterPrePreparedFiltersMask;

}_WlanRxFilterPrePreparedFiltersCommandResponseBuff_t;



typedef _u8 SLrxFilterOperation_t;
#define SL_ENABLE_DISABLE_RX_FILTER                         (0)
#define SL_REMOVE_RX_FILTER                                 (1)
#define SL_STORE_RX_FILTERS                                 (2)
#define SL_UPDATE_RX_FILTER_ARGS                            (3)
#define SL_FILTER_RETRIEVE_ENABLE_STATE                     (4)
#define SL_FILTER_PRE_PREPARED_RETRIEVE_CREATE_REMOVE_STATE (5)
#define SL_FILTER_PRE_PREPARED_SET_CREATE_REMOVE_STATE      (6)


/*  Bit manipulation for 8 bit */
#define ISBITSET8(x,i) ((x[i>>3] & (0x80>>(i&7)))!=0) /* < Is bit set, 8 bit unsigned numbers = x , location = i */
#define SETBIT8(x,i) x[i>>3]|=(0x80>>(i&7)); /* < Set bit,8 bit unsigned numbers = x , location = i */
#define CLEARBIT8(x,i) x[i>>3]&=(0x80>>(i&7))^0xFF; /* < Clear bit,8 bit unsigned numbers = x , location = i */


/*********************************************************************************************/
/* Function prototypes                                                                       */
/*********************************************************************************************/

/*!

    \addtogroup wlan
    @{

*/


/*!
  \brief Adds new filter rule to the system

  \param[in]    RuleType     The rule type
  \param[in]    FilterFlags  Flags which set the type of header rule Args and sets the persistent flag
  \param[in]    pRule        Determine the filter rule logic
  \param[in]    pTrigger     Determine when the rule is triggered also sets rule parent.
  \param[in]    pAction      Sets the action to be executed in case the match functions pass
  \param[out]   pFilterId    The filterId which was created

  \return       On success, zero is returned. Otherwise error code is returned
 */
#if _SL_INCLUDE_FUNC(sl_WlanRxFilterAdd)
SlrxFilterID_t sl_WlanRxFilterAdd(    SlrxFilterRuleType_t                 RuleType,
                                    SlrxFilterFlags_t                     FilterFlags,
                                    const SlrxFilterRule_t* const         Rule,
                                    const SlrxFilterTrigger_t* const     Trigger,
                                    const SlrxFilterAction_t* const     Action,
                                    SlrxFilterID_t*                     pFilterId);

#endif





/*!
  \brief   Sets parameters to Rx filters

 \param[in] RxFilterOperation
 possible operations :
    - SL_ENABLE_DISABLE_RX_FILTER - Enables\disables filter in a filter list
    - SL_REMOVE_RX_FILTER - Removes filter from memory ( to remove from flash call SL_STORE_RX_FILTERS after this command)
    - SL_STORE_RX_FILTERS - Save the filters for persistent
    - SL_UPDATE_RX_FILTER_ARGS - Update the arguments of existing filter
    - SL_FILTER_PRE_PREPARED_SET_CREATE_REMOVE_STATE - Change the default creation of the pre-prepared filters

  \param[in] pInputBuffer options:
   The buffer input is _WlanRxFilterOperationCommandBuff_t:
     - SL_ENABLE_DISABLE_RX_FILTER
    - SL_REMOVE_RX_FILTER
    - SL_STORE_RX_FILTERS
    The buffer input is _WlanRxFilterUpdateArgsCommandBuff_t:
     - SL_UPDATE_RX_FILTER_ARGS
   The buffer input is _WlanRxFilterPrePreparedFiltersCommandBuff_t:
     - SL_FILTER_PRE_PREPARED_SET_CREATE_REMOVE_STATE

  \param[in] InputbufferLength The length in byte of the input buffer

  \return    On success, zero is returned. Otherwise error code is returned
 */

#if _SL_INCLUDE_FUNC(sl_WlanRxFilterSet)
_i16 sl_WlanRxFilterSet(  const SLrxFilterOperation_t RxFilterOperation,
                          const _u8*  const pInputBuffer,
                         _u16 InputbufferLength);
#endif

/*!
  \brief    Gets parameters of Rx filters

 \param[in] RxFilterOperation
 possible operations :
    - SL_FILTER_RETRIEVE_ENABLE_STATE - Retrieves the enable disable status
    - SL_FILTER_PRE_PREPARED_RETRIEVE_CREATE_REMOVE_STATE - Retrieves the pre-prepared filters creation status

 \param[in] pOutputBuffer
   The buffer input is _WlanRxFilterRetrieveEnableStatusCommandResponseBuff_t:
     - SL_FILTER_RETRIEVE_ENABLE_STATE
   The buffer input is _WlanRxFilterPrePreparedFiltersCommandResponseBuff_t:
     - SL_FILTER_PRE_PREPARED_RETRIEVE_CREATE_REMOVE_STATE

  \param[in] OutputbufferLength   The length in byte of the output buffer

   \return   On success, zero is returned. Otherwise error code is returned
*/

#if _SL_INCLUDE_FUNC(sl_WlanRxFilterGet)
_i16 sl_WlanRxFilterGet(const SLrxFilterOperation_t RxFilterOperation,
                        _u8*  pOutputBuffer,
                        _u16  OutputbufferLength);
#endif


/*!

 Close the Doxygen group.
 @}

 */

#ifdef  __cplusplus
}
#endif /*  __cplusplus */

#endif /* RX_FILTERS_PREPROCESSOR_CLI_IF_H_ */


