/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __MODSIM_H__
#define __MODSIM_H__

#ifdef __cplusplus
extern "C" {
#endif

#define QL_SIM_MCC_LEN      4   /**  Length of the MCC. */
#define QL_SIM_MNC_MAX      4   /**  Maximum length of the MNC. */
#define QL_SIM_PLMN_NUM_MAX 24  /**  Maximum number of PLMN data sets. */

typedef enum 
{
	QL_SIM_SUCCESS,
	QL_SIM_GENERIC_FAILURE,
}QL_SIM_ERROR_CODE;



typedef struct 
{
    uint8_t mcc[QL_SIM_MCC_LEN];  /**< MCC value in ASCII characters.*/
    uint8_t mnc[QL_SIM_MNC_MAX];  /**< MNC value in ASCII characters.*/
}QL_SIM_PLMN_INFO;  

typedef struct 
{
    unsigned int          preferred_operator_list_num;                    /**< Must be set to the number of elements in preferred_operator_list. */
    QL_SIM_PLMN_INFO    preferred_operator_list[QL_SIM_PLMN_NUM_MAX];   /**< Preferred operator list. */
}QL_SIM_PREFERRED_OPERATOR_LIST;    /* Message */

#define QL_SIM_PIN_LEN_MAX  16   /**  Maximum length of PIN data. */ 
typedef struct 
{
    uint8_t                     pin_value[QL_SIM_PIN_LEN_MAX];  /*  Value of the PIN */
}QL_SIM_VERIFY_PIN_INFO;  

typedef struct 
{
    uint8_t                     old_pin_value[QL_SIM_PIN_LEN_MAX];  /**< Value of the old PIN as a sequence of ASCII characters. */
    uint8_t                     new_pin_value[QL_SIM_PIN_LEN_MAX];  /**< Value of the new PIN as a sequence of ASCII characters. */
}QL_SIM_CHANGE_PIN_INFO; 

typedef struct 
{
    uint8_t                     puk_value[QL_SIM_PIN_LEN_MAX];      /**< Value of the PUK as a sequence of ASCII characters. */
    uint8_t                     new_pin_value[QL_SIM_PIN_LEN_MAX];  /**< Value of the new PIN as a sequence of ASCII characters. */
}QL_SIM_UNBLOCK_PIN_INFO;  

typedef enum 
{
    QL_SIM_CARD_TYPE_UNKNOWN  = 0,    /**<  Unidentified card type.  */
    QL_SIM_CARD_TYPE_ICC      = 1,    /**<  Card of SIM or RUIM type.  */
    QL_SIM_CARD_TYPE_UICC     = 2,    /**<  Card of USIM or CSIM type.  */
}QL_SIM_CARD_TYPE;  
	
typedef enum 
{
   QL_SIM_STATUS_NOT_INSERTED,
   QL_SIM_STATUS_READY,
   QL_SIM_STATUS_SIM_PIN,
   QL_SIM_STATUS_SIM_PUK,
   QL_SIM_STATUS_PH_SIM_LOCK_PIN,
   QL_SIM_STATUS_PH_SIM_LOCK_PUK,
   QL_SIM_STATUS_PH_FSIM_PIN,
   QL_SIM_STATUS_PH_FSIM_PUK,
   QL_SIM_STATUS_SIM_PIN2,
   QL_SIM_STATUS_SIM_PUK2,
   QL_SIM_STATUS_PH_NET_PIN,
   QL_SIM_STATUS_PH_NET_PUK,
   QL_SIM_STATUS_PH_NET_SUB_PIN,
   QL_SIM_STATUS_PH_NET_SUB_PUK,
   QL_SIM_STATUS_PH_SP_PIN,
   QL_SIM_STATUS_PH_SP_PUK,
   QL_SIM_STATUS_PH_CORP_PIN,
   QL_SIM_STATUS_PH_CORP_PUK,
   QL_SIM_STATUS_BUSY,
   QL_SIM_STATUS_BLOCKED,
   QL_SIM_STATUS_UNKNOWN
}QL_SIM_STATUS;    /**< Card state. */
	
typedef struct 
{
    uint8_t               pin1_num_retries;       /**<   Number of PIN 1 retries. */
    uint8_t               puk1_num_retries;       /**<   Number of PUK 1 retries. */
    uint8_t               pin2_num_retries;       /**<   Number of PIN 2 retries. */
    uint8_t               puk2_num_retries;       /**<   Number of PUK 2 retries. */
}QL_SIM_CARD_PIN_INFO; 



typedef struct 
{
    QL_SIM_CARD_TYPE      	card_type; // SIM card type
    QL_SIM_STATUS     		card_state;  //SIM card state
    QL_SIM_CARD_PIN_INFO  	card_pin_info; // PIN info
}QL_SIM_CARD_INFO; 

typedef enum 
{
    QL_SIM_FILE_TYPE_UNKNOWN       = 0, /**<  Unknown file type  */
    QL_SIM_FILE_TYPE_TRANSPARENT   = 1, /**< File structure consisting of a sequence of bytes.  */
    QL_SIM_FILE_TYPE_CYCLIC        = 2,	/**< File structure consisting of a sequence of records, each containing the same fixed size in 
                                             chronological order. Once all the records have been used, the oldest data is overwritten.  */
    QL_SIM_FILE_TYPE_LINEAR_FIXED  = 3, /**< File structure consisting of a sequence of records, each containing the same fixed size.  */
}QL_SIM_FILE_TYPE;

typedef enum 
{
    QL_SIM_FILE_ACCESS_TYPE_ALWAYS      =0,
    QL_SIM_FILE_ACCESS_TYPE_CHV1        =1,
    QL_SIM_FILE_ACCESS_TYPE_CHV2        =2,
    QL_SIM_FILE_ACCESS_TYPE_ADM         =3,
}QL_SIM_FILE_ACCESS_TYPE;
	
typedef struct 
{
    QL_SIM_FILE_ACCESS_TYPE read_access;
    QL_SIM_FILE_ACCESS_TYPE update_access;
}QL_SIM_FILE_ACCESS_INFO;

typedef enum 
{
    QL_SIM_FILE_STATUS_INVALID      =0,
    QL_SIM_FILE_STATUS_EFFECTIVE    =1,
}QL_SIM_FILE_STATUS;


typedef struct 
{
    unsigned int                id;
    QL_SIM_FILE_TYPE            type;    /**<   File type: */
    QL_SIM_FILE_ACCESS_INFO     access;    /**<   File access conditions: */
    QL_SIM_FILE_STATUS          status;    /**<   File status: */
    unsigned int                size;      /**<   Size of transparent files.*/
    unsigned int                record_len;    /**<   Size of each cyclic or linear fixed file record.*/
    unsigned int                record_count;   /**<   Number of cyclic or linear fixed file records.*/
}QL_SIM_FILE_INFO; 

typedef struct 
{
    int sw1;
    int sw2;
}QL_SIM_FILE_OPERATION_RET;

#define QL_SIM_DATA_LEN_MAX     255
typedef struct 
{
    unsigned int    data_len;                        /**< Must be set to the number of elements in data. */
    uint8_t     data[QL_SIM_DATA_LEN_MAX];       /**< Data retrieved from the card. */
}QL_SIM_CARD_FILE_DATA; 

typedef enum
{
    QL_SIM_PHONE_BOOK_STORAGE_DC,    /**< 0 - ME dialed calls list */
    QL_SIM_PHONE_BOOK_STORAGE_EN,    /**< 1 - SIM (or ME) emergency number  */
    QL_SIM_PHONE_BOOK_STORAGE_FD,    /**< 2 - SIM fix dialing-phone book    */
    QL_SIM_PHONE_BOOK_STORAGE_LD,    /**< 3 - SIM last-dialing-phone book   */
    QL_SIM_PHONE_BOOK_STORAGE_MC,    /**< 4 - ME missed (unanswered) calls list */
    QL_SIM_PHONE_BOOK_STORAGE_ME,    /**< 5 - Mobile equipment phonebook    */
    QL_SIM_PHONE_BOOK_STORAGE_MT,    /**< 6 -  */
    QL_SIM_PHONE_BOOK_STORAGE_ON,    /**< 7 - SIM own numbers (MSISDNs) list */
    QL_SIM_PHONE_BOOK_STORAGE_RC,    /**< 8 - ME received calls list */
    QL_SIM_PHONE_BOOK_STORAGE_SM,    /**< 9 - SIM phonebook */
    QL_SIM_PHONE_BOOK_STORAGE_AP,    /**< 10 -  */
    QL_SIM_PHONE_BOOK_STORAGE_MBDN,  /**< 11 -  */
    QL_SIM_PHONE_BOOK_STORAGE_MN,    /**< 12 -  */
    QL_SIM_PHONE_BOOK_STORAGE_SDN,   /**< 13 -  */
    QL_SIM_PHONE_BOOK_STORAGE_ICI,   /**< 14 -  */
    QL_SIM_PHONE_BOOK_STORAGE_OCI,   /**< 15 -  */
}QL_SIM_PHONE_BOOK_STORAGE;

#define QL_SIM_PHONE_BOOK_RECORDS_MAX_COUNT 20

typedef struct 
{ // when write, if phonenum is empty, it means to delete this item specified by index  
    int          index;  // the record index in phone book
    uint8_t      username[32];   //  username
    uint8_t      phonenum[24];   //  Phone number, it can include '+'*/
}QL_SIM_PHONE_BOOK_RECORD_INFO;


typedef struct 
{
    int record_count;  //the count of record
    QL_SIM_PHONE_BOOK_RECORD_INFO record[QL_SIM_PHONE_BOOK_RECORDS_MAX_COUNT]; // the list of record
}QL_SIM_PHONE_BOOK_RECORDS_INFO;



#ifdef __cplusplus
}
#endif

#endif

