/*
 * This file is part of the MicroPython ESP32 project, https://github.com/loboris/MicroPython_ESP32_psRAM_LoBo
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 LoBo (https://github.com/loboris)
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

#ifndef _LIBGSM_H_
#define _LIBGSM_H_

#include "sdkconfig.h"

#include <time.h>
#include <stdint.h>

#define GSM_STATE_DISCONNECTED	0
#define GSM_STATE_CONNECTED		1
#define GSM_STATE_IDLE			89
#define GSM_STATE_FIRSTINIT		98
#define GSM_MAX_NAME_LEN		32

#define SMS_SORT_NONE	0
#define SMS_SORT_ASC	1
#define SMS_SORT_DESC	2

#define SMS_LIST_ALL	0
#define SMS_LIST_NEW	1
#define SMS_LIST_OLD	2
#define SMS_LIST_ALL_STR	"AT+CMGL=\"ALL\"\r\n"
#define SMS_LIST_NEW_STR	"AT+CMGL=\"REC UNREAD\"\r\n"
#define SMS_LIST_OLD_STR	"AT+CMGL=\"REC READ\"\r\n"

typedef struct
{
	int		idx;
	char	*msg;
	char	stat[32];
	char	from[32];
	char	time[32];
	time_t	time_value;
	int		tz;
}SMS_Msg;

typedef struct
{
	uint8_t	idx[32];
	time_t	time[32];
}SMS_indexes;


/*
 * Create GSM/PPPoS task if not already created
 * Initialize GSM and connect to Internet
 * Handle all PPPoS requests
 * Disconnect/Reconnect from/to Internet on user request
 * If 'wait' = 1, wait until connected
 * If 'doconn' = 0, only initialize the task, do not connect to Internet
 */
//=================================================================================================================================
int ppposInit(int tx, int rx, int rts, int cts, int bdr, char *user, char *pass, char *apn, uint8_t wait, int doconn, bool roaming);

/*
 * Disconnect from Internet
 * If 'end_task' = 1 also terminate GSM/PPPoS task
 * If 'rfoff' = 1, turns off GSM RF section to preserve power
 * If already disconnected, this function does nothing
 */
//===================================================
int ppposDisconnect(uint8_t end_task, uint8_t rfoff);

/*
 * Connect from Internet
 * If already connected, this function does nothing
 */
//=================
int ppposConnect();

/*
 * Get transmitted and received bytes count
 * If 'rst' = 1, resets the counters
 */
//=========================================================
void getRxTxCount(uint32_t *rx, uint32_t *tx, uint8_t rst);

/*
 * Resets transmitted and received bytes counters
 */
//====================
void resetRxTxCount();

/*
 * Get GSM/Task status
 *
 * Result:
 * GSM_STATE_DISCONNECTED	(0)		Disconnected from Internet
 * GSM_STATE_CONNECTED		(1)		Connected to Internet
 * GSM_STATE_IDLE			(89)	Disconnected from Internet, Task idle, waiting for reconnect request
 * GSM_STATE_FIRSTINIT		(98)	Task started, initializing PPPoS
 */
//============================================================
int ppposStatus(uint32_t *ip, uint32_t *netmask, uint32_t *gw);

/*
 * Turn GSM RF Off
 */
//==============
int gsm_RFOff();

/*
 * Turn GSM RF On
 */
//=============
int gsm_RFOn();

/*
 * Send SMS
 *
 * Params:
 *   smsnum:	Pointer to phone number in international format (+<counry_code><gsm number>)
 *      msg:	Pointer to message text
 */
//==================================
int smsSend(char *smsnum, char *msg);

/*
 * Get messages list
 *
 * Params:
 *  rd_status:	check all, unread or read messages
 *    sms_idx:	return sms at index in msg
 *        msg:	SMS message structure pointer
 *    indexes:	pointer to indexes of the detected message
 *       sort:	sort the indexes
 */
//====================================================================================================
int getMessagesList(uint8_t rd_status, int sms_idx, SMS_Msg *msg, SMS_indexes *indexes, uint8_t sort);

/*
 * return number of messages of given type
 * and, optionally the indexes of all new messages
 */
//=============================================================
int smsCount(uint8_t type, SMS_indexes *indexes, uint8_t sort);

/*
 * Delete the message at GSM message index 'idx'
 */
//=====================
int smsDelete(int idx);

//==============================================
int setSMS_cb(void *cb_func, uint32_t interval);

//=========================
void setDebug(uint8_t dbg);

//=====================================================================================
int at_Cmd(char *cmd, char* resp, char **buffer, int buf_size, int tmo, char *cmddata);

#endif

