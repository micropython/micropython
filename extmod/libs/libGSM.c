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

#include "sdkconfig.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

#include "driver/uart.h"
#include "driver/gpio.h"
#include "tcpip_adapter.h"
#include "netif/ppp/pppos.h"
#include "netif/ppp/ppp.h"
#include "netif/ppp/pppapi.h"
//#include "lwip/port/lwipopts.h"
#include "lwip/opt.h"

#include "libs/libGSM.h"
#include "py/runtime.h"
//#include "mphalport.h"
#include "py/mphal.h"

extern int MainTaskCore;

#define BUF_SIZE (1024)
#define GSM_OK_Str "OK"
#define PPPOSMUTEX_TIMEOUT 5000 / portTICK_RATE_MS

#define PPPOS_CLIENT_STACK_SIZE 1024*3

// shared variables, use mutex to access them
static uint8_t gsm_status = GSM_STATE_FIRSTINIT;
static int do_pppos_connect = 1;
static uint32_t pppos_rx_count;
static uint32_t pppos_tx_count;
static uint8_t pppos_task_started = 0;
static uint8_t gsm_rfOff = 0;

static void *New_SMS_cb = NULL;
static uint32_t SMS_check_interval = 0;
static uint8_t debug = 0;
static uint8_t doCheckSMS = 1;

// local variables
static TaskHandle_t PPPoSTaskHandle = NULL;
static QueueHandle_t pppos_mutex = NULL;
static char PPP_User[GSM_MAX_NAME_LEN] = {0};
static char PPP_Pass[GSM_MAX_NAME_LEN] = {0};
static char GSM_APN[GSM_MAX_NAME_LEN] = {0};
static int uart_num = UART_NUM_1;
static int gsm_pin_tx = UART_PIN_NO_CHANGE;
static int gsm_pin_rx = UART_PIN_NO_CHANGE;
static int gsm_pin_cts = UART_PIN_NO_CHANGE;
static int gsm_pin_rts = UART_PIN_NO_CHANGE;
static int gsm_baudrate = 115200;
static uint8_t tcpip_adapter_initialized = 0;
static uint32_t sms_timer = 0;
static bool allow_roaming = false;

static uint32_t ppp_ip = 0;
static uint32_t ppp_netmask = 0;
static uint32_t ppp_gw = 0;

// The PPP control block
static ppp_pcb *ppp = NULL;

// The PPP IP interface
struct netif ppp_netif;

static const char *TAG = "[PPPOS CLIENT]";

typedef struct
{
	char		*cmd;
	int16_t		cmdSize;
	char		*cmdResponseOnOk;
	uint16_t	timeoutMs;
	uint16_t	delayMs;
	uint8_t		skip;
}GSM_Cmd;

static GSM_Cmd cmd_AT =
{
	.cmd = "AT\r\n",
	.cmdSize = sizeof("AT\r\n")-1,
	.cmdResponseOnOk = GSM_OK_Str,
	.timeoutMs = 300,
	.delayMs = 0,
	.skip = 0,
};

static GSM_Cmd cmd_NoSMSInd =
{
	.cmd = "AT+CNMI=0,0,0,0,0\r\n",
	.cmdSize = -1,
	.cmdResponseOnOk = GSM_OK_Str,
	.timeoutMs = 1000,
	.delayMs = 0,
	.skip = 0,
};

static GSM_Cmd cmd_Reset =
{
	.cmd = "ATZ\r\n",
	.cmdSize = -1,
	.cmdResponseOnOk = GSM_OK_Str,
	.timeoutMs = 300,
	.delayMs = 0,
	.skip = 0,
};

static GSM_Cmd cmd_RFOn =
{
	.cmd = "AT+CFUN=1\r\n",
	.cmdSize = -1,
	.cmdResponseOnOk = GSM_OK_Str,
	.timeoutMs = 10000,
	.delayMs = 1000,
	.skip = 0,
};

static GSM_Cmd cmd_EchoOff =
{
	.cmd = "ATE0\r\n",
	.cmdSize = -1,
	.cmdResponseOnOk = GSM_OK_Str,
	.timeoutMs = 300,
	.delayMs = 0,
	.skip = 0,
};

static GSM_Cmd cmd_Pin =
{
	.cmd = "AT+CPIN?\r\n",
	.cmdSize = -1,
	.cmdResponseOnOk = "CPIN: READY",
	.timeoutMs = 5000,
	.delayMs = 0,
	.skip = 0,
};

static GSM_Cmd cmd_Reg =
{
	.cmd = "AT+CREG?\r\n",
	.cmdSize = -1,
	.cmdResponseOnOk = "CREG: 0,1",
	.timeoutMs = 3000,
	.delayMs = 2000,
	.skip = 0,
};

static GSM_Cmd cmd_APN =
{
	.cmd = NULL,
	.cmdSize = 0,
	.cmdResponseOnOk = GSM_OK_Str,
	.timeoutMs = 8000,
	.delayMs = 0,
	.skip = 0,
};

static GSM_Cmd cmd_Connect =
{
	.cmd = "AT+CGDATA=\"PPP\",1\r\n",
	//.cmd = "ATDT*99***1#\r\n",
	.cmdSize = -1,
	.cmdResponseOnOk = "CONNECT",
	.timeoutMs = 30000,
	.delayMs = 1000,
	.skip = 0,
};

static GSM_Cmd *GSM_Init[] =
{
		&cmd_AT,
		&cmd_Reset,
		&cmd_EchoOff,
		&cmd_RFOn,
		&cmd_Pin,
		&cmd_Reg,
		&cmd_NoSMSInd,
		&cmd_APN,
		&cmd_Connect,
};

#define GSM_InitCmdsSize  (sizeof(GSM_Init)/sizeof(GSM_Cmd *))


// PPP status callback
//--------------------------------------------------------------
static void ppp_status_cb(ppp_pcb *pcb, int err_code, void *ctx)
{
	struct netif *pppif = ppp_netif(pcb);
	LWIP_UNUSED_ARG(ctx);

	switch(err_code) {
		case PPPERR_NONE: {
			if (debug) {
				ESP_LOGI(TAG,"status_cb: Connected");
				#if PPP_IPV4_SUPPORT
				ESP_LOGI(TAG,"   ipaddr    = %s", ipaddr_ntoa(&pppif->ip_addr));
				ESP_LOGI(TAG,"   gateway   = %s", ipaddr_ntoa(&pppif->gw));
				ESP_LOGI(TAG,"   netmask   = %s", ipaddr_ntoa(&pppif->netmask));
				#endif

				#if PPP_IPV6_SUPPORT
				ESP_LOGI(TAG,"   ip6addr   = %s", ip6addr_ntoa(netif_ip6_addr(pppif, 0)));
				#endif
			}
			xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
            ppp_ip = pppif->ip_addr.u_addr.ip4.addr;
            ppp_netmask = pppif->netmask.u_addr.ip4.addr;
            ppp_gw = pppif->gw.u_addr.ip4.addr;
			gsm_status = GSM_STATE_CONNECTED;
			xSemaphoreGive(pppos_mutex);
			break;
		}
		case PPPERR_PARAM: {
			if (debug) {
				ESP_LOGE(TAG,"status_cb: Invalid parameter");
			}
			break;
		}
		case PPPERR_OPEN: {
			if (debug) {
				ESP_LOGE(TAG,"status_cb: Unable to open PPP session");
			}
			break;
		}
		case PPPERR_DEVICE: {
			if (debug) {
				ESP_LOGE(TAG,"status_cb: Invalid I/O device for PPP");
			}
			break;
		}
		case PPPERR_ALLOC: {
			if (debug) {
				ESP_LOGE(TAG,"status_cb: Unable to allocate resources");
			}
			break;
		}
		case PPPERR_USER: {
			/* ppp_free(); -- can be called here */
			if (debug) {
				ESP_LOGW(TAG,"status_cb: User interrupt (disconnected)");
			}
			xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
            ppp_ip = 0;
            ppp_netmask = 0;
            ppp_gw = 0;
			gsm_status = GSM_STATE_DISCONNECTED;
			xSemaphoreGive(pppos_mutex);
			break;
		}
		case PPPERR_CONNECT: {
			if (debug) {
				ESP_LOGE(TAG,"status_cb: Connection lost");
			}
			xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
            ppp_ip = 0;
            ppp_netmask = 0;
            ppp_gw = 0;
			gsm_status = GSM_STATE_DISCONNECTED;
			xSemaphoreGive(pppos_mutex);
			break;
		}
		case PPPERR_AUTHFAIL: {
			if (debug) {
				ESP_LOGE(TAG,"status_cb: Failed authentication challenge");
			}
			break;
		}
		case PPPERR_PROTOCOL: {
			if (debug) {
				ESP_LOGE(TAG,"status_cb: Failed to meet protocol");
			}
			break;
		}
		case PPPERR_PEERDEAD: {
			if (debug) {
				ESP_LOGE(TAG,"status_cb: Connection timeout");
			}
			break;
		}
		case PPPERR_IDLETIMEOUT: {
			if (debug) {
				ESP_LOGE(TAG,"status_cb: Idle Timeout");
			}
			break;
		}
		case PPPERR_CONNECTTIME: {
			if (debug) {
				ESP_LOGE(TAG,"status_cb: Max connect time reached");
			}
			break;
		}
		case PPPERR_LOOPBACK: {
			if (debug) {
				ESP_LOGE(TAG,"status_cb: Loopback detected");
			}
			break;
		}
		default: {
			if (debug) {
				ESP_LOGE(TAG,"status_cb: Unknown error code %d", err_code);
			}
			break;
		}
	}
}

// === Handle sending data to GSM modem ===
//------------------------------------------------------------------------------
static u32_t ppp_output_callback(ppp_pcb *pcb, u8_t *data, u32_t len, void *ctx)
{
	uint32_t ret = uart_write_bytes(uart_num, (const char*)data, len);
    uart_wait_tx_done(uart_num, 10 / portTICK_RATE_MS);
    if (ret > 0) {
		xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
    	pppos_rx_count += ret;
		xSemaphoreGive(pppos_mutex);
    }
    return ret;
}

//---------------------------------------------------------
static void infoCommand(char *cmd, int cmdSize, char *info)
{
	char buf[cmdSize+2];
	memset(buf, 0, cmdSize+2);

	for (int i=0; i<cmdSize;i++) {
		if ((cmd[i] != 0x00) && ((cmd[i] < 0x20) || (cmd[i] > 0x7F))) buf[i] = '.';
		else buf[i] = cmd[i];
		if (buf[i] == '\0') break;
	}
	ESP_LOGI(TAG,"%s [%s]", info, buf);
}

//-------------------------------------------------------------------------------------------------------------------------------------
static int atCmd_waitResponse(char * cmd, char *resp, char * resp1, int cmdSize, int timeout, char **response, int size, char *cmddata)
{
	char data[256] = {'\0'};
    int len, res = 1, tot = 0, timeoutCnt = 0;
	size_t blen = 0;

	// ** Send command to GSM
	vTaskDelay(100 / portTICK_PERIOD_MS);
	uart_flush(uart_num);

	if (cmd != NULL) {
		if (cmdSize == -1) cmdSize = strlen(cmd);
		if (debug) {
			infoCommand(cmd, cmdSize, "AT COMMAND:");
		}
		uart_write_bytes(uart_num, (const char*)cmd, cmdSize);
		uart_wait_tx_done(uart_num, 100 / portTICK_RATE_MS);
	}

	if (response != NULL) {
		// === Read GSM response into buffer ===
		char *pbuf = *response;
		// wait for first response data
		while (blen == 0) {
			uart_get_buffered_data_len(uart_num, &blen);
			vTaskDelay(10 / portTICK_PERIOD_MS);
			timeoutCnt += 10;
			if (timeoutCnt > timeout) break;
		}
		len = uart_read_bytes(uart_num, (uint8_t*)data, 256, 50 / portTICK_RATE_MS);
		// Add response to buffer
		while (len > 0) {
			if ((tot+len) >= size) {
				// Need to expand the buffer
				char *ptemp = realloc(pbuf, size+512);
				if (ptemp == NULL) {
					if (debug) {
						ESP_LOGE(TAG,"AT RESPONSE (to buffer): Error reallocating buffer of size %d", size+512);
					}
					// Ignore any new data sent by modem
					while (len > 0) {
						len = uart_read_bytes(uart_num, (uint8_t*)data, 256, 100 / portTICK_RATE_MS);
					}
					return tot; // return success with received bytes
				}
				else if (debug) {
					ESP_LOGD(TAG,"AT RESPONSE (to buffer): buffer reallocated, new size: %d", size+512);
				}
				size += 512;
				pbuf = ptemp;
			}
			memcpy(pbuf+tot, data, len);	// append response to the buffer
			tot += len;						// increase total received count
			pbuf[tot] = '\0';				// terminate string
			if (resp != NULL) {
				// Check terminating string
				if (strstr(pbuf, resp)) {
					if (debug) {
						ESP_LOGI(TAG,"RESPONSE terminator detected");
					}
					if (cmddata) {
						if (debug) {
							ESP_LOGI(TAG,"Sending data");
						}
						vTaskDelay(10 / portTICK_PERIOD_MS);
						// Send data after response
						uart_write_bytes(uart_num, (const char*)cmddata, strlen(cmddata));
						uart_wait_tx_done(uart_num, 1000 / portTICK_RATE_MS);
						// Read the response after the data was sent
						resp = NULL;
						// wait for first response data
						timeoutCnt = 0;
						blen = 0;
						while (blen == 0) {
							uart_get_buffered_data_len(uart_num, &blen);
							vTaskDelay(10 / portTICK_PERIOD_MS);
							timeoutCnt += 10;
							if (timeoutCnt > timeout) break;
						}
						len = uart_read_bytes(uart_num, (uint8_t*)data, 256, 50 / portTICK_RATE_MS);
						continue;
					}
					// Ignore any new data sent by modem
					while (len > 0) {
						len = uart_read_bytes(uart_num, (uint8_t*)data, 256, 100 / portTICK_RATE_MS);
					}
					break;
				}
			}
			len = uart_read_bytes(uart_num, (uint8_t*)data, 256, 100 / portTICK_RATE_MS);
		}
		*response = pbuf;
		if (debug) {
			ESP_LOGI(TAG,"AT RESPONSE (to buffer): len=%d", tot);
		}
		return tot;
	}

    // === Receive response to temporary buffer, wait for and check the response ===
	char sresp[256] = {'\0'};
	int idx = 0;
	while(1)
	{
		memset(data, 0, 256);
		len = 0;
		len = uart_read_bytes(uart_num, (uint8_t*)data, 256, 10 / portTICK_RATE_MS);
		if (len > 0) {
			for (int i=0; i<len;i++) {
				if (idx < 256) {
					if ((data[i] >= 0x20) && (data[i] < 0x80)) sresp[idx++] = data[i];
					else sresp[idx++] = 0x2e;
				}
			}
			tot += len;
		}
		else {
			if (tot > 0) {
				// Check the response
				if (strstr(sresp, resp) != NULL) {
					if (debug) {
						ESP_LOGI(TAG,"AT RESPONSE: [%s]", sresp);
					}
					break;
				}
				else {
					if (resp1 != NULL) {
						if (strstr(sresp, resp1) != NULL) {
							if (debug) {
								ESP_LOGI(TAG,"AT RESPONSE (1): [%s]", sresp);
							}
							res = 2;
							break;
						}
					}
					// no match
					if (debug) {
						ESP_LOGI(TAG,"AT BAD RESPONSE: [%s]", sresp);
					}
					res = 0;
					break;
				}
			}
		}

		timeoutCnt += 10;
		if (timeoutCnt > timeout) {
			// timeout
			if (debug) {
				ESP_LOGE(TAG,"AT: TIMEOUT");
			}
			res = 0;
			break;
		}
	}

	return res;
}

//------------------------------------
static void _disconnect(uint8_t rfOff)
{
	int res = atCmd_waitResponse("AT\r\n", GSM_OK_Str, NULL, 4, 1000, NULL, 0, NULL);
	if (res == 1) {
		if (rfOff) {
			cmd_Reg.timeoutMs = 10000;
			res = atCmd_waitResponse("AT+CFUN=4\r\n", GSM_OK_Str, NULL, 11, 10000, NULL, 0, NULL); // disable RF function
		}
		return;
	}

	if (debug) {
		ESP_LOGI(TAG,"ONLINE, DISCONNECTING...");
	}
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	uart_flush(uart_num);
	uart_write_bytes(uart_num, "+++", 3);
    uart_wait_tx_done(uart_num, 10 / portTICK_RATE_MS);
	vTaskDelay(1100 / portTICK_PERIOD_MS);

	int n = 0;
	res = atCmd_waitResponse("ATH\r\n", GSM_OK_Str, "NO CARRIER", 5, 3000, NULL, 0, NULL);
	while (res == 0) {
		n++;
		if (n > 10) {
			if (debug) {
				ESP_LOGI(TAG,"STILL CONNECTED.");
			}
			n = 0;
			vTaskDelay(1000 / portTICK_PERIOD_MS);
			uart_flush(uart_num);
			uart_write_bytes(uart_num, "+++", 3);
		    uart_wait_tx_done(uart_num, 10 / portTICK_RATE_MS);
			vTaskDelay(1000 / portTICK_PERIOD_MS);
		}
		vTaskDelay(100 / portTICK_PERIOD_MS);
		res = atCmd_waitResponse("ATH\r\n", GSM_OK_Str, "NO CARRIER", 5, 3000, NULL, 0, NULL);
	}
	vTaskDelay(100 / portTICK_PERIOD_MS);
	if (rfOff) {
		cmd_Reg.timeoutMs = 10000;
		res = atCmd_waitResponse("AT+CFUN=4\r\n", GSM_OK_Str, NULL, 11, 3000, NULL, 0, NULL);
	}
	if (debug) {
		ESP_LOGI(TAG,"DISCONNECTED.");
	}
}

//----------------------------
static void enableAllInitCmd()
{
	for (int idx = 0; idx < GSM_InitCmdsSize; idx++) {
		GSM_Init[idx]->skip = 0;
	}
}

//--------------------
static void checkSMS()
{
	if ((New_SMS_cb) && (SMS_check_interval > 0) && (doCheckSMS)) {
		// Check for new SMS and schedule MicroPython callback function
		uint8_t dbg = debug;
		debug = 0;
		if (sms_timer > SMS_check_interval) {
			sms_timer = 0;
			SMS_indexes indexes;
			int nmsg = smsCount(SMS_LIST_NEW, &indexes, SMS_SORT_NONE);
			if (nmsg > 0) {
				if (nmsg > 100) nmsg = 100;
				// Create a string containing SMS indexes
				char sidx[4];
				char *sindexes = calloc(nmsg*3, 1);
				if (sindexes) {
					for (int i=0; i<nmsg; i++) {
						sprintf(sidx, "%d;", indexes.idx[i]);
						strcat(sindexes, sidx);
					}
					mp_sched_schedule(New_SMS_cb, MP_OBJ_FROM_PTR(sindexes));
					free(sindexes);
				}
			}
		}
		else sms_timer += 100;
		debug = dbg;
	}
}

/*
 * PPPoS TASK
 * Handles GSM initialization, disconnects and GSM modem responses
 */
//-----------------------------
static void pppos_client_task()
{
	xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
	pppos_task_started = 1;
    ppp_ip = 0;
    ppp_netmask = 0;
    ppp_gw = 0;
	xSemaphoreGive(pppos_mutex);

	char PPP_ApnATReq[strlen(GSM_APN)+24];
    // Allocate receive buffer
    char* data = (char*) malloc(BUF_SIZE);
    if (data == NULL) {
		if (debug) {
			ESP_LOGE(TAG,"Failed to allocate data buffer.");
		}
    	goto exit;
    }

    uart_hw_flowcontrol_t flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    // Initialize the UART pins
    if (gpio_set_direction(gsm_pin_tx, GPIO_MODE_OUTPUT)) goto exit;
	if (gpio_set_direction(gsm_pin_rx, GPIO_MODE_INPUT)) goto exit;
	if (gpio_set_pull_mode(gsm_pin_rx, GPIO_PULLUP_ONLY)) goto exit;
	if ((gsm_pin_rts >=0) && (gsm_pin_cts >= 0)) {
	    if (gpio_set_direction(gsm_pin_rts, GPIO_MODE_OUTPUT)) goto exit;
		if (gpio_set_direction(gsm_pin_cts, GPIO_MODE_INPUT)) goto exit;
		if (gpio_set_pull_mode(gsm_pin_cts, GPIO_PULLUP_ONLY)) goto exit;
		flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS;
	}
	else if (gsm_pin_rts >=0) {
	    if (gpio_set_direction(gsm_pin_rts, GPIO_MODE_OUTPUT)) goto exit;
		flow_ctrl = UART_HW_FLOWCTRL_RTS;
	}
	else if (gsm_pin_cts >= 0) {
		if (gpio_set_direction(gsm_pin_cts, GPIO_MODE_INPUT)) goto exit;
		if (gpio_set_pull_mode(gsm_pin_cts, GPIO_PULLUP_ONLY)) goto exit;
		flow_ctrl = UART_HW_FLOWCTRL_CTS;
	}

	uart_config_t uart_config = {
			.baud_rate = gsm_baudrate,
			.data_bits = UART_DATA_8_BITS,
			.parity = UART_PARITY_DISABLE,
			.stop_bits = UART_STOP_BITS_1,
			.flow_ctrl = flow_ctrl
	};
	if (flow_ctrl & UART_HW_FLOWCTRL_RTS) uart_config.rx_flow_ctrl_thresh = UART_FIFO_LEN/2;

	// Configure UART parameters
	if (uart_param_config(uart_num, &uart_config)) goto exit;
	// Set UART pins(TX, RX, RTS, CTS)
	if (uart_set_pin(uart_num, gsm_pin_tx, gsm_pin_rx, gsm_pin_rts, gsm_pin_cts)) goto exit;
	if (uart_driver_install(uart_num, BUF_SIZE * 2, BUF_SIZE * 2, 0, NULL, 0)) goto exit;

	// Set APN from config
	sprintf(PPP_ApnATReq, "AT+CGDCONT=1,\"IP\",\"%s\"\r\n", GSM_APN);
	cmd_APN.cmd = PPP_ApnATReq;
	cmd_APN.cmdSize = strlen(PPP_ApnATReq);

	_disconnect(1); // Disconnect if connected

	xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
    pppos_tx_count = 0;
    pppos_rx_count = 0;
	gsm_status = GSM_STATE_FIRSTINIT;
	xSemaphoreGive(pppos_mutex);

	enableAllInitCmd();

	while(1)
	{
		if (debug) {
			ESP_LOGI(TAG,"GSM initialization start");
		}
		vTaskDelay(500 / portTICK_PERIOD_MS);

		if (do_pppos_connect <= 0) {
			cmd_Connect.skip = 1;
			cmd_APN.skip = 1;
		}
		int gsmCmdIter = 0;
		int nfail = 0;
		int cmd_res = 0;

		// ===== GSM Initialization loop =========================================================================
		while(gsmCmdIter < GSM_InitCmdsSize)
		{
			if (GSM_Init[gsmCmdIter]->skip) {
				if (debug) {
					infoCommand(GSM_Init[gsmCmdIter]->cmd, GSM_Init[gsmCmdIter]->cmdSize, "Skip command:");
				}
				gsmCmdIter++;
				continue;
			}
			if ((GSM_Init[gsmCmdIter] == &cmd_Reg) && (allow_roaming))
	            cmd_res = atCmd_waitResponse(GSM_Init[gsmCmdIter]->cmd, GSM_Init[gsmCmdIter]->cmdResponseOnOk, "CREG: 0,5", GSM_Init[gsmCmdIter]->cmdSize, GSM_Init[gsmCmdIter]->timeoutMs, NULL, 0, NULL);
			else
			    cmd_res = atCmd_waitResponse(GSM_Init[gsmCmdIter]->cmd, GSM_Init[gsmCmdIter]->cmdResponseOnOk, NULL, GSM_Init[gsmCmdIter]->cmdSize, GSM_Init[gsmCmdIter]->timeoutMs, NULL, 0, NULL);
			if (cmd_res == 0) {
				// * No response or not as expected, start from first initialization command
				if (debug) {
					ESP_LOGW(TAG,"Wrong response, restarting...");
				}

				if (++nfail > 20) goto exit;

				vTaskDelay(3000 / portTICK_PERIOD_MS);
				gsmCmdIter = 0;
				continue;
			}

            if ((GSM_Init[gsmCmdIter] == &cmd_Reg) && (allow_roaming)) {
                if (cmd_res == 2) {
                    if (debug) {
                        ESP_LOGW(TAG,"Connected in roaming");
                    }
                }
            }

            if (GSM_Init[gsmCmdIter]->delayMs > 0) vTaskDelay(GSM_Init[gsmCmdIter]->delayMs / portTICK_PERIOD_MS);
			GSM_Init[gsmCmdIter]->skip = 1;
			if (GSM_Init[gsmCmdIter] == &cmd_Reg) GSM_Init[gsmCmdIter]->delayMs = 0;
			// Next command
			gsmCmdIter++;
		}
		// =======================================================================================================

		if (debug) {
			ESP_LOGI(TAG,"GSM initialized.");
		}

		// === GSM is now initiated ===
		xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
		if (gsm_status == GSM_STATE_FIRSTINIT) {
			// ** After first successful initialization create PPP control block
			xSemaphoreGive(pppos_mutex);
			ppp = pppapi_pppos_create(&ppp_netif,
					ppp_output_callback, ppp_status_cb, NULL);

			if (ppp == NULL) {
				if (debug) {
					ESP_LOGE(TAG, "Error initializing PPPoS");
				}
				break; // end task
			}
			if (debug) {
				ESP_LOGI(TAG, "PPPoS control block created");
			}
		}
		else {
			gsm_status = GSM_STATE_IDLE;
			xSemaphoreGive(pppos_mutex);
		}

		int gstat = 0;
		if (do_pppos_connect <= 0) {
			// === Connection to the Internet was not requested, stay in idle mode ===
			if (debug) {
				ESP_LOGI(TAG, "PPPoS IDLE mode");
			}
			xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
			gsm_status = GSM_STATE_IDLE;
			xSemaphoreGive(pppos_mutex);
			// === Wait for connect request ===
			gstat = 0;
			while (gstat == 0) {
				vTaskDelay(100 / portTICK_PERIOD_MS);
				xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
				gstat = do_pppos_connect;
				checkSMS();
				xSemaphoreGive(pppos_mutex);
			}
			if (gstat < 0) break;  // terminate task
			gsmCmdIter = 0;
			enableAllInitCmd();
			cmd_Connect.skip = 0;
			cmd_APN.skip = 0;
			xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
			do_pppos_connect = 1;
			xSemaphoreGive(pppos_mutex);
			if (debug) {
				mp_hal_stdout_tx_str("\n");
				ESP_LOGI(TAG, "Connect requested.");
			}
			continue;
		}
		if (gstat < 0) break;  // terminate task

		// === Connect to the Internet ===========================
		pppapi_set_default(ppp);
		pppapi_set_auth(ppp, PPPAUTHTYPE_PAP, PPP_User, PPP_Pass);
		//pppapi_set_auth(ppp, PPPAUTHTYPE_NONE, PPP_User, PPP_Pass);

		pppapi_connect(ppp, 0);
		// =======================================================
		gstat = 1;

		// ===== LOOP: Handle GSM modem responses & disconnects =====
		while(1) {
			xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
			if (do_pppos_connect <= 0) {
				// === Disconnect was requested ===
				int end_task = do_pppos_connect;
				do_pppos_connect = 1;
				xSemaphoreGive(pppos_mutex);
				if (debug) {
					mp_hal_stdout_tx_str("\n");
					ESP_LOGI(TAG, "Disconnect requested.");
				}

				pppapi_close(ppp, 0);
				gstat = 1;
				while (gsm_status != GSM_STATE_DISCONNECTED) {
					// Handle data received from GSM
					memset(data, 0, BUF_SIZE);
					int len = uart_read_bytes(uart_num, (uint8_t*)data, BUF_SIZE, 30 / portTICK_RATE_MS);
					if (len > 0)	{
						pppos_input_tcpip(ppp, (u8_t*)data, len);
						xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
					    pppos_tx_count += len;
						xSemaphoreGive(pppos_mutex);
					}
					xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
					gstat = gsm_status;
					xSemaphoreGive(pppos_mutex);
				}
				vTaskDelay(1000 / portTICK_PERIOD_MS);

				xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
				uint8_t rfoff = gsm_rfOff;
				xSemaphoreGive(pppos_mutex);
				_disconnect(rfoff); // Disconnect GSM if still connected

				if (debug) {
					ESP_LOGI(TAG, "Disconnected.");
				}

				gsmCmdIter = 0;
				enableAllInitCmd();
				xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
				gsm_status = GSM_STATE_IDLE;
				do_pppos_connect = 0;
				xSemaphoreGive(pppos_mutex);

				if (end_task < 0) goto exit;

				// === Wait for reconnect request ===
				gstat = 0;
				while (gstat == 0) {
					vTaskDelay(100 / portTICK_PERIOD_MS);
					xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
					gstat = do_pppos_connect;
					checkSMS();
					xSemaphoreGive(pppos_mutex);
				}
				if (gstat < 0) break;  // terminate task
				if (debug) {
					mp_hal_stdout_tx_str("\n");
					ESP_LOGI(TAG, "Reconnect requested.");
				}
				break;
			}

			// === Check if disconnected ==============
			if (gsm_status == GSM_STATE_DISCONNECTED) {
				gsm_status = GSM_STATE_IDLE;
				xSemaphoreGive(pppos_mutex);
				if (debug) {
					mp_hal_stdout_tx_str("\n");
					ESP_LOGE(TAG, "Disconnected, trying again...");
				}

				pppapi_close(ppp, 0);

				enableAllInitCmd();
				gsmCmdIter = 0;
				vTaskDelay(5000 / portTICK_PERIOD_MS);
				// Initialize the GSM modem again
				break;
			}
			else xSemaphoreGive(pppos_mutex);

			// === Handle data received from GSM ================================================
			memset(data, 0, BUF_SIZE);
			int len = uart_read_bytes(uart_num, (uint8_t*)data, BUF_SIZE, 30 / portTICK_RATE_MS);
			if (len > 0)	{
				pppos_input_tcpip(ppp, (u8_t*)data, len);
				xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
			    pppos_tx_count += len;
				xSemaphoreGive(pppos_mutex);
			}
			// ==================================================================================

		}  // Handle GSM modem responses & disconnects loop
		if (gstat < 0) break;  // terminate task
	}  // main task loop

exit:
	// Terminate GSM task
	xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
	if (data) free(data);	// free data buffer
	if (ppp) ppp_free(ppp);	// free PPP control block

	pppos_task_started = 0;
	gsm_status = GSM_STATE_FIRSTINIT;
	uart_driver_delete(uart_num);
	xSemaphoreGive(pppos_mutex);

	if (debug) {
		ESP_LOGE(TAG, "PPPoS TASK TERMINATED");
	}
	vTaskDelete(NULL);
}

//=================================================================================================================================
int ppposInit(int tx, int rx, int rts, int cts, int bdr, char *user, char *pass, char *apn, uint8_t wait, int doconn, bool roaming)
{
	if (pppos_mutex != NULL) xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
	do_pppos_connect = doconn;
	allow_roaming = roaming;
	int gstat = 0;
	int task_s = pppos_task_started;
	if (pppos_mutex != NULL) xSemaphoreGive(pppos_mutex);

	if (task_s == 0) {
		// PPPoS task not running
		gsm_pin_tx = tx;
		gsm_pin_rx = rx;
		gsm_pin_cts = cts;
		gsm_pin_rts = rts;
		gsm_baudrate = bdr;
		strncpy(PPP_User, user, GSM_MAX_NAME_LEN);
		strncpy(PPP_Pass, pass, GSM_MAX_NAME_LEN);
		strncpy(GSM_APN, apn, GSM_MAX_NAME_LEN);

		if (pppos_mutex == NULL) pppos_mutex = xSemaphoreCreateMutex();
		if (pppos_mutex == NULL) return -1;

		if (tcpip_adapter_initialized == 0) {
			tcpip_adapter_init();
			tcpip_adapter_initialized = 1;
		}
		#if CONFIG_MICROPY_USE_BOTH_CORES
		xTaskCreate(&pppos_client_task, "GSM_PPPoS", PPPOS_CLIENT_STACK_SIZE, NULL, CONFIG_MICROPY_TASK_PRIORITY, &PPPoSTaskHandle);
		#else
		// Select GSM task core
		int task_core = 0;

		xTaskCreatePinnedToCore(&pppos_client_task, "GSM_PPPoS", PPPOS_CLIENT_STACK_SIZE, NULL, CONFIG_MICROPY_TASK_PRIORITY, &PPPoSTaskHandle, task_core);
		#endif
		if (PPPoSTaskHandle == NULL) return -2;

		while (task_s == 0) {
			vTaskDelay(10 / portTICK_RATE_MS);
			xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
			task_s = pppos_task_started;
			xSemaphoreGive(pppos_mutex);
		}
	}

	if (wait == 0) return 0;

	// Wait until ready
	gstat = 0;
	while ((gstat != GSM_STATE_IDLE) && (gstat != GSM_STATE_CONNECTED)) {
		vTaskDelay(10 / portTICK_RATE_MS);
		xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
		gstat = gsm_status;
		task_s = pppos_task_started;
		xSemaphoreGive(pppos_mutex);
		if (task_s == 0) return -3;
	}

	return 0;
}

//================
int ppposConnect()
{
	if (pppos_mutex == NULL) return -1;
	xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
	do_pppos_connect = 1;
	int gstat = gsm_status;
	int task_s = pppos_task_started;
	xSemaphoreGive(pppos_mutex);

	if (task_s == 0) return -2;
	if (gstat == GSM_STATE_CONNECTED) return 0;

	int tmo = 0;
	while (gstat != 1) {
		vTaskDelay(10 / portTICK_RATE_MS);
		xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
		gstat = gsm_status;
		task_s = pppos_task_started;
		xSemaphoreGive(pppos_mutex);
		if (task_s == 0) return -2;
        tmo++;
        if (tmo > 800) return -1;
	}

	return 0;
}

//===================================================
int ppposDisconnect(uint8_t end_task, uint8_t rfoff)
{
	if (pppos_mutex == NULL) return -1;

	xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
	int gstat = gsm_status;
	int task_s = pppos_task_started;
	xSemaphoreGive(pppos_mutex);

	if (task_s == 0) return 0;

	if ((gstat == GSM_STATE_IDLE) && (end_task == 0)) return 0;

	vTaskDelay(2000 / portTICK_RATE_MS);
	xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
	if (end_task) do_pppos_connect = -1;
	else do_pppos_connect = 0;
	gsm_rfOff = rfoff;
	xSemaphoreGive(pppos_mutex);

	gstat = 0;
    int tmo = 0;
	while ((gstat == 0) && (task_s != 0)) {
		xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
		gstat = do_pppos_connect;
		task_s = pppos_task_started;
		xSemaphoreGive(pppos_mutex);
		vTaskDelay(10 / portTICK_RATE_MS);
        tmo++;
        if (tmo > 800) return -1;
	}
	if (task_s == 0) return 0;

	tmo = 0;
	while ((gstat != 0) && (task_s != 0)) {
		vTaskDelay(100 / portTICK_RATE_MS);
		xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
		gstat = do_pppos_connect;
		task_s = pppos_task_started;
		xSemaphoreGive(pppos_mutex);
        tmo++;
        if (tmo > 800) return -1;
	}
	return 0;
}

//============================================================
int ppposStatus(uint32_t *ip, uint32_t *netmask, uint32_t *gw)
{
	if (pppos_mutex == NULL) return GSM_STATE_FIRSTINIT;

	xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
	int gstat = gsm_status;
    if (ip) *ip = ppp_ip;
    if (netmask) *netmask = ppp_netmask;
    if (gw) *gw = ppp_gw;
	xSemaphoreGive(pppos_mutex);

	return gstat;
}

//========================================================
void getRxTxCount(uint32_t *rx, uint32_t *tx, uint8_t rst)
{
	if (pppos_mutex == NULL) {
		*rx = 0;
		*tx = 0;
	}

	xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
	*rx = pppos_rx_count;
	*tx = pppos_tx_count;
	if (rst) {
		pppos_rx_count = 0;
		pppos_tx_count = 0;
	}
	xSemaphoreGive(pppos_mutex);
}

//===================
void resetRxTxCount()
{
	if (pppos_mutex == NULL) return;
	xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
	pppos_rx_count = 0;
	pppos_tx_count = 0;
	xSemaphoreGive(pppos_mutex);
}

//=============
int gsm_RFOff()
{
	if (pppos_mutex == NULL) return 1;
	xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
	int gstat = gsm_status;
	xSemaphoreGive(pppos_mutex);

	if (gstat != GSM_STATE_IDLE) return 0;

	uint8_t f = 1;
	char buf[64] = {'\0'};
	char *pbuf = buf;
	int res = atCmd_waitResponse("AT+CFUN?\r\n", NULL, NULL, -1, 2000, &pbuf, 63, NULL);
	if (res > 0) {
		if (strstr(buf, "+CFUN: 4")) f = 0;
	}

	if (f) {
		cmd_Reg.timeoutMs = 500;
		return atCmd_waitResponse("AT+CFUN=4\r\n", GSM_OK_Str, NULL, 11, 10000, NULL, 0, NULL); // disable RF function
	}
	return 1;
}

//============
int gsm_RFOn()
{
	if (pppos_mutex == NULL) return 1;
	xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
	int gstat = gsm_status;
	xSemaphoreGive(pppos_mutex);

	if (gstat != GSM_STATE_IDLE) return 0;

	uint8_t f = 1;
	char buf[64] = {'\0'};
	char *pbuf = buf;
	int res = atCmd_waitResponse("AT+CFUN?\r\n", NULL, NULL, -1, 2000, &pbuf, 63, NULL);
	if (res > 0) {
		if (strstr(buf, "+CFUN: 1")) f = 0;
	}

	if (f) {
		cmd_Reg.timeoutMs = 0;
		return atCmd_waitResponse("AT+CFUN=1\r\n", GSM_OK_Str, NULL, 11, 10000, NULL, 0, NULL); // disable RF function
	}
	return 1;
}

// ==== SMS Functions ==========================================================================

//--------------------
static int sms_ready()
{
	if (ppposStatus(NULL, NULL, NULL) != GSM_STATE_IDLE) return 0;
	int ret = 0;

	xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
	doCheckSMS = 0;
	xSemaphoreGive(pppos_mutex);

	int res = atCmd_waitResponse("AT+CFUN?\r\n", "+CFUN: 1", NULL, -1, 1000, NULL, 0, NULL);
	if (res != 1) goto exit;

	res = atCmd_waitResponse("AT+CMGF=1\r\n", GSM_OK_Str, NULL, -1, 1000, NULL, 0, NULL);
	if (res != 1) goto exit;
	ret = 1;

	//res = atCmd_waitResponse("AT+CPMS=\"SM\"\r\n", GSM_OK_Str, NULL, -1, 1000, NULL, 0, NULL);
	//if (res != 1) goto exit;
exit:
	xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
	doCheckSMS = 1;
	xSemaphoreGive(pppos_mutex);

	return ret;
}

//---------------------------------------
time_t sms_time(char * msg_time, int *tz)
{
	if (strlen(msg_time) >= 20) {
		// Convert message time to time structure
		int hh,mm,ss,yy,mn,dd, tz;
		struct tm tm;
		sscanf(msg_time, "%u/%u/%u,%u:%u:%u%d", &yy, &mn, &dd, &hh, &mm, &ss, &tz);
		tm.tm_hour = hh;
		tm.tm_min = mm;
		tm.tm_sec = ss;
		tm.tm_year = yy+100;
		tm.tm_mon = mn-1;
		tm.tm_mday = dd;
		if (tz) tz = tz/4;	// time zone info
		return mktime(&tm);	// Linux time
	}
	return 0;
}

// Parse message in buffer to message structure
//---------------------------------------------------------------
static int getSMS(char *msgstart, SMS_Msg *msg, uint8_t msgalloc)
{
	char *msgidx = msgstart;
	// Clear message structure
	memset(msg, 0, sizeof(SMS_Msg));

	// Get message info
	char *pend = strstr(msgidx, "\r\n");
	if (pend == NULL) return 0;

	int len = pend-msgidx;
	char hdr[len+4];
	char buf[32];

	memset(hdr, 0, len+4);
	memcpy(hdr, msgidx, len);
	hdr[len] = '\0';

	if (msgalloc) {
		msgidx = pend + 2;
		// Allocate message body buffer and copy the data
		len = strlen(msgidx);
		msg->msg = (char *)calloc(len+1, 1);
		if (msg->msg) {
			memcpy(msg->msg, msgidx, len);
			msg->msg[len] = '\0';
		}
	}

	// Parse message info
	msgidx = hdr;
	pend = strstr(hdr, ",\"");
	int i = 1;
	while (pend != NULL) {
		len = pend-msgidx;
		if ((len < 32) && (len > 0)) {
			memset(buf, 0, 32);
			strncpy(buf, msgidx, len);
			buf[len] = '\0';
			if (buf[len-1] == '"') buf[len-1] = '\0';

			if (i == 1) {
				msg->idx = (int)strtol(buf, NULL, 0);	// message index
			}
			else if (i == 2) strcpy(msg->stat, buf);			// message status
			else if (i == 3) strcpy(msg->from, buf);			// phone number of message sender
			else if (i == 5) strcpy(msg->time, buf);			// the time when the message was sent
		}
		i++;
		msgidx = pend + 2;
		pend = strstr(msgidx, ",\"");
		if (pend == NULL) pend = strstr(msgidx, "\"");
	}

	msg->time_value = sms_time(msg->time, &msg->tz);

	return 1;
}

// Get message index and time
//-----------------------------------------------------
static int getSMSindex(char *msgstart, time_t *msgtime)
{
	char *msgidx = msgstart;
	// Get message info
	char *pend = strstr(msgidx, "\r\n");
	if (pend == NULL) return 0;

	int len = pend-msgidx;
	char hdr[len+4];
	char buf[32];
	char msg_time[32] = {'\0'};
	int msg_idx = 0;

	memcpy(hdr, msgidx, len);
	hdr[len] = '\0';

	// Parse message info
	msgidx = hdr;
	pend = strstr(hdr, ",\"");
	int i = 1;
	while (pend != NULL) {
		len = pend-msgidx;
		if ((len < 32) && (len > 0)) {
			memset(buf, 0, 32);
			memcpy(buf, msgidx, len);
			buf[len] = '\0';
			if (buf[len-1] == '"') buf[len-1] = '\0';

			if (i == 1) msg_idx = (int)strtol(buf, NULL, 0);	// message index
			else if (i == 5) strcpy(msg_time, buf);				// the time when the message was sent
		}
		i++;
		msgidx = pend + 2;
		// find next entry
		pend = strstr(msgidx, ",\"");
		if (pend == NULL) pend = strstr(msgidx, "\"");
	}

	*msgtime = sms_time(msg_time, NULL);

	return msg_idx;
}

//--------------------------------------------------------------------------------------------------------
static int checkMessages(uint8_t rd_status, int sms_idx, SMS_Msg *msg, SMS_indexes *indexes, uint8_t sort)
{
    int timeoutCnt = 0;
	size_t blen = 0;

	// ** Send command to GSM
	vTaskDelay(100 / portTICK_PERIOD_MS);
	uart_flush(uart_num);

	if (rd_status == SMS_LIST_NEW) uart_write_bytes(uart_num, SMS_LIST_NEW_STR, strlen(SMS_LIST_NEW_STR));
	else if (rd_status == SMS_LIST_OLD) uart_write_bytes(uart_num, SMS_LIST_OLD_STR, strlen(SMS_LIST_OLD_STR));
	else uart_write_bytes(uart_num, SMS_LIST_ALL_STR, strlen(SMS_LIST_ALL_STR));

	uart_wait_tx_done(uart_num, 100 / portTICK_RATE_MS);

	// ** Read GSM response
	// wait for first response data
	while (blen == 0) {
		uart_get_buffered_data_len(uart_num, &blen);
		vTaskDelay(10 / portTICK_PERIOD_MS);
		timeoutCnt += 10;
		if (timeoutCnt > 1000) {
			if (debug) {
				ESP_LOGE(TAG,"Check SMS, no response (timeout)");
			}
			return 0;
		}
	}

	if (indexes !=NULL) memset(indexes, 0, sizeof(SMS_indexes));

	char *rbuffer = calloc(1024, 1);
	if (rbuffer == NULL) {
		if (debug) {
			ESP_LOGE(TAG,"Check SMS, Error allocating receive buffer");
		}
		return 0;
	}

	int len, buflen = 0, nmsg = 0;
	uint8_t idx_found = 0;
	char *msgstart = rbuffer;
	char *msgend = NULL;
	char *bufptr = rbuffer;
	while (1) {
		len = uart_read_bytes(uart_num, (uint8_t*)bufptr, 1023-buflen, 50 / portTICK_RATE_MS);
		if (len == 0) break;
		buflen += len;
		bufptr += len;
		*bufptr = '\0';

		// Check message start string
		msgstart = strstr(rbuffer, "+CMGL: ");
		if (msgstart) msgend = strstr(msgstart, "\r\n\r\n");

		while ((msgstart) && (msgend)) {
			*msgend = '\0';
			// We have the whole message in the buffer
			nmsg++;
			if ((indexes !=NULL) && (nmsg < 33)) indexes->idx[nmsg-1] = getSMSindex(msgstart+7, &indexes->time[nmsg-1]);
			if ((sms_idx == nmsg) && (msg != NULL)) {
				getSMS(msgstart+7, msg, 1);
				// Ignore remaining data from module
				while (len > 0) {
					len = uart_read_bytes(uart_num, (uint8_t*)rbuffer, 1023, 50 / portTICK_RATE_MS);
				}
				// and return
				idx_found = 1;
				break;
			}

			// Delete the message
			memmove(rbuffer, msgend+4, buflen - (msgend-rbuffer+4));
			buflen -= (msgend-rbuffer+4);
			bufptr = rbuffer+buflen;
			*bufptr = '\0';

			// Check message start string
			msgend = NULL;
			msgstart = strstr(rbuffer, "+CMGL: ");
			if (msgstart) msgend = strstr(msgstart, "\r\n\r\n");
		}
	}

	free(rbuffer);

	if ((msg != NULL) && (idx_found == 0)) return 0;

	if ((nmsg > 0) && (indexes != NULL) && (sort != SMS_SORT_NONE)) {
		// Sort messages
    	bool f;
    	int temp;
    	time_t tempt;
		for (int i=0; i<nmsg; ++i) {
		    for (int j=i+1; j<nmsg; ++j) {
		    	if (sort == SMS_SORT_ASC) f = (indexes->time[i] > indexes->time[j]);
		    	else f = (indexes->time[i] < indexes->time[j]);
		        if (f) {
		            temp = indexes->idx[i];
		            tempt = indexes->time[i];
		            indexes->idx[i] = indexes->idx[j];
		            indexes->time[i] = indexes->time[j];
		            indexes->idx[j] = temp;
		            indexes->time[j] = tempt;
		        }
		    }
		}
	}
	return nmsg;
}

//==================================
int smsSend(char *smsnum, char *msg)
{
	if (sms_ready() == 0) return 0;

	xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
	doCheckSMS = 0;
	xSemaphoreGive(pppos_mutex);

	char *msgbuf = NULL;
	int res = 0;
	char buf[64];
	int len = strlen(msg);

	sprintf(buf, "AT+CMGS=\"%s\"\r\n", smsnum);
	res = atCmd_waitResponse(buf, "> ", NULL, -1, 1000, NULL, 0, NULL);
	if (res != 1) {
		atCmd_waitResponse("\x1B", GSM_OK_Str, NULL, 1, 1000, NULL, 0, NULL);
		res = 0;
		goto exit;
	}

	msgbuf = malloc(len+2);
	if (msgbuf == NULL) {
		res = 0;
		goto exit;
	}

	sprintf(msgbuf, "%s\x1A", msg);
	res = atCmd_waitResponse(msgbuf, "+CMGS: ", "ERROR", len+1, 40000, NULL, 0, NULL);
	if (res != 1) {
		res = atCmd_waitResponse("\x1B", GSM_OK_Str, NULL, 1, 1000, NULL, 0, NULL);
		res = 0;
	}
exit:
	xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
	doCheckSMS = 1;
	xSemaphoreGive(pppos_mutex);

	if (msgbuf) free(msgbuf);
	return res;
}

//============================================================
int smsCount(uint8_t type, SMS_indexes *indexes, uint8_t sort)
{
	if (sms_ready() == 0) return -1;

	xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
	doCheckSMS = 0;
	xSemaphoreGive(pppos_mutex);

	int res = checkMessages(type, 0, NULL, indexes, sort);

	xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
	doCheckSMS = 1;
	xSemaphoreGive(pppos_mutex);

	return res;
}

//===================================================================================================
int getMessagesList(uint8_t rd_status, int sms_idx, SMS_Msg *msg, SMS_indexes *indexes, uint8_t sort)
{
	if (sms_ready() == 0) return -1;

	xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
	doCheckSMS = 0;
	xSemaphoreGive(pppos_mutex);

	int res = checkMessages(rd_status, sms_idx, msg, indexes, sort);

	xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
	doCheckSMS = 1;
	xSemaphoreGive(pppos_mutex);

	return res;
}

//====================
int smsDelete(int idx)
{
	if (sms_ready() == 0) return 0;

	char buf[64];

	xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
	doCheckSMS = 0;
	xSemaphoreGive(pppos_mutex);

	sprintf(buf,"AT+CMGD=%d\r\n", idx);

	xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
	doCheckSMS = 1;
	xSemaphoreGive(pppos_mutex);

	return atCmd_waitResponse(buf, GSM_OK_Str, NULL, -1, 5000, NULL, 0, NULL);
}

//=============================================
int setSMS_cb(void *cb_func, uint32_t interval)
{
	if (pppos_mutex == NULL) return 0;
	xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
	New_SMS_cb = cb_func;
	SMS_check_interval = interval;
	xSemaphoreGive(pppos_mutex);
	return 1;
}

//========================
void setDebug(uint8_t dbg)
{
	if (pppos_mutex != NULL) xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
	debug = dbg;
	if (debug) esp_log_level_set(TAG, ESP_LOG_DEBUG);
	else esp_log_level_set(TAG, ESP_LOG_NONE);
	if (pppos_mutex != NULL) xSemaphoreGive(pppos_mutex);
}

//=======================================*============================================
int at_Cmd(char *cmd, char* resp, char **buffer, int buf_size, int tmo, char *cmddata)
{
	if (ppposStatus(NULL, NULL, NULL) != GSM_STATE_IDLE) return 0;
	xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);

	int res = atCmd_waitResponse(cmd, resp, NULL, -1, tmo, buffer, buf_size, cmddata);

	xSemaphoreGive(pppos_mutex);
	return res;
}

