/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Daniel Campora
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

#include <stdint.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "shared/runtime/interrupt_char.h"
#include "telnet.h"
#include "simplelink.h"
#include "modnetwork.h"
#include "modwlan.h"
#include "modsocket.h"
#include "debug.h"
#include "serverstask.h"
#include "genhdr/mpversion.h"

/******************************************************************************
 DEFINE PRIVATE CONSTANTS
 ******************************************************************************/
#define TELNET_PORT                         23
// rxRindex and rxWindex must be uint8_t and TELNET_RX_BUFFER_SIZE == 256
#define TELNET_RX_BUFFER_SIZE               256
#define TELNET_MAX_CLIENTS                  1
#define TELNET_TX_RETRIES_MAX               50
#define TELNET_WAIT_TIME_MS                 5
#define TELNET_LOGIN_RETRIES_MAX            3
#define TELNET_CYCLE_TIME_MS                (SERVERS_CYCLE_TIME_MS * 2)

/******************************************************************************
 DEFINE PRIVATE TYPES
 ******************************************************************************/
typedef enum {
    E_TELNET_RESULT_OK = 0,
    E_TELNET_RESULT_AGAIN,
    E_TELNET_RESULT_FAILED
} telnet_result_t;

typedef enum {
    E_TELNET_STE_DISABLED = 0,
    E_TELNET_STE_START,
    E_TELNET_STE_LISTEN,
    E_TELNET_STE_CONNECTED,
    E_TELNET_STE_LOGGED_IN
} telnet_state_t;

typedef enum {
    E_TELNET_STE_SUB_WELCOME,
    E_TELNET_STE_SUB_SND_USER_OPTIONS,
    E_TELNET_STE_SUB_REQ_USER,
    E_TELNET_STE_SUB_GET_USER,
    E_TELNET_STE_SUB_REQ_PASSWORD,
    E_TELNET_STE_SUB_SND_PASSWORD_OPTIONS,
    E_TELNET_STE_SUB_GET_PASSWORD,
    E_TELNET_STE_SUB_INVALID_LOGGIN,
    E_TELNET_STE_SUB_SND_REPL_OPTIONS,
    E_TELNET_STE_SUB_LOGGIN_SUCCESS
} telnet_connected_substate_t;

typedef union {
    telnet_connected_substate_t connected;
} telnet_substate_t;

typedef struct {
    uint8_t             *rxBuffer;
    uint32_t            timeout;
    telnet_state_t      state;
    telnet_substate_t   substate;
    int16_t             sd;
    int16_t             n_sd;

    // rxRindex and rxWindex must be uint8_t and TELNET_RX_BUFFER_SIZE == 256
    uint8_t             rxWindex;
    uint8_t             rxRindex;

    uint8_t             txRetries;
    uint8_t             logginRetries;
    bool                enabled;
    bool                credentialsValid;
} telnet_data_t;

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
static telnet_data_t telnet_data;
static const char* telnet_welcome_msg       = "MicroPython " MICROPY_GIT_TAG " on " MICROPY_BUILD_DATE "; " MICROPY_HW_BOARD_NAME " with " MICROPY_HW_MCU_NAME "\r\n";
static const char* telnet_request_user      = "Login as: ";
static const char* telnet_request_password  = "Password: ";
static const char* telnet_invalid_loggin    = "\r\nInvalid credentials, try again.\r\n";
static const char* telnet_loggin_success    = "\r\nLogin succeeded!\r\nType \"help()\" for more information.\r\n";
static const uint8_t telnet_options_user[]  = // IAC   WONT ECHO IAC   WONT SUPPRESS_GO_AHEAD IAC  WILL LINEMODE
                                               { 255,  252,   1, 255,  252,       3,          255, 251,   34 };
static const uint8_t telnet_options_pass[]  = // IAC   WILL ECHO IAC   WONT SUPPRESS_GO_AHEAD IAC  WILL LINEMODE
                                               { 255,  251,   1, 255,  252,       3,          255, 251,   34 };
static const uint8_t telnet_options_repl[]  = // IAC   WILL ECHO IAC   WILL SUPPRESS_GO_AHEAD IAC  WONT LINEMODE
                                               { 255,  251,   1, 255,  251,       3,          255, 252,   34 };

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
static void telnet_wait_for_enabled (void);
static bool telnet_create_socket (void);
static void telnet_wait_for_connection (void);
static void telnet_send_and_proceed (void *data, _i16 Len, telnet_connected_substate_t next_state);
static telnet_result_t telnet_send_non_blocking (void *data, _i16 Len);
static telnet_result_t telnet_recv_text_non_blocking (void *buff, _i16 Maxlen, _i16 *rxLen);
static void telnet_process (void);
static int telnet_process_credential (char *credential, _i16 rxLen);
static void telnet_parse_input (uint8_t *str, int16_t *len);
static bool telnet_send_with_retries (int16_t sd, const void *pBuf, int16_t len);
static void telnet_reset_buffer (void);

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
void telnet_init (void) {
    // Allocate memory for the receive buffer (from the RTOS heap)
    ASSERT ((telnet_data.rxBuffer = mem_Malloc(TELNET_RX_BUFFER_SIZE)) != NULL);
    telnet_data.state = E_TELNET_STE_DISABLED;
}

void telnet_run (void) {
    _i16 rxLen;
    switch (telnet_data.state) {
        case E_TELNET_STE_DISABLED:
            telnet_wait_for_enabled();
            break;
        case E_TELNET_STE_START:
            if (wlan_is_connected() && telnet_create_socket()) {
                telnet_data.state = E_TELNET_STE_LISTEN;
            }
            break;
        case E_TELNET_STE_LISTEN:
            telnet_wait_for_connection();
            break;
        case E_TELNET_STE_CONNECTED:
            switch (telnet_data.substate.connected) {
            case E_TELNET_STE_SUB_WELCOME:
                telnet_send_and_proceed((void *)telnet_welcome_msg, strlen(telnet_welcome_msg), E_TELNET_STE_SUB_SND_USER_OPTIONS);
                break;
            case E_TELNET_STE_SUB_SND_USER_OPTIONS:
                telnet_send_and_proceed((void *)telnet_options_user, sizeof(telnet_options_user), E_TELNET_STE_SUB_REQ_USER);
                break;
            case E_TELNET_STE_SUB_REQ_USER:
                // to catch any left over characters from the previous actions
                telnet_recv_text_non_blocking(telnet_data.rxBuffer, TELNET_RX_BUFFER_SIZE, &rxLen);
                telnet_send_and_proceed((void *)telnet_request_user, strlen(telnet_request_user), E_TELNET_STE_SUB_GET_USER);
                break;
            case E_TELNET_STE_SUB_GET_USER:
                if (E_TELNET_RESULT_OK == telnet_recv_text_non_blocking(telnet_data.rxBuffer + telnet_data.rxWindex,
                                                                        TELNET_RX_BUFFER_SIZE - telnet_data.rxWindex,
                                                                        &rxLen)) {
                    int result;
                    if ((result = telnet_process_credential (servers_user, rxLen))) {
                        telnet_data.credentialsValid = result > 0 ? true : false;
                        telnet_data.substate.connected = E_TELNET_STE_SUB_REQ_PASSWORD;
                    }
                }
                break;
            case E_TELNET_STE_SUB_REQ_PASSWORD:
                telnet_send_and_proceed((void *)telnet_request_password, strlen(telnet_request_password), E_TELNET_STE_SUB_SND_PASSWORD_OPTIONS);
                break;
            case E_TELNET_STE_SUB_SND_PASSWORD_OPTIONS:
                // to catch any left over characters from the previous actions
                telnet_recv_text_non_blocking(telnet_data.rxBuffer, TELNET_RX_BUFFER_SIZE, &rxLen);
                telnet_send_and_proceed((void *)telnet_options_pass, sizeof(telnet_options_pass), E_TELNET_STE_SUB_GET_PASSWORD);
                break;
            case E_TELNET_STE_SUB_GET_PASSWORD:
                if (E_TELNET_RESULT_OK == telnet_recv_text_non_blocking(telnet_data.rxBuffer + telnet_data.rxWindex,
                                                                        TELNET_RX_BUFFER_SIZE - telnet_data.rxWindex,
                                                                        &rxLen)) {
                    int result;
                    if ((result = telnet_process_credential (servers_pass, rxLen))) {
                        if ((telnet_data.credentialsValid = telnet_data.credentialsValid && (result > 0 ? true : false))) {
                            telnet_data.substate.connected = E_TELNET_STE_SUB_SND_REPL_OPTIONS;
                        }
                        else {
                            telnet_data.substate.connected = E_TELNET_STE_SUB_INVALID_LOGGIN;
                        }
                    }
                }
                break;
            case E_TELNET_STE_SUB_INVALID_LOGGIN:
                if (E_TELNET_RESULT_OK == telnet_send_non_blocking((void *)telnet_invalid_loggin, strlen(telnet_invalid_loggin))) {
                    telnet_data.credentialsValid = true;
                    if (++telnet_data.logginRetries >= TELNET_LOGIN_RETRIES_MAX) {
                        telnet_reset();
                    }
                    else {
                        telnet_data.substate.connected = E_TELNET_STE_SUB_SND_USER_OPTIONS;
                    }
                }
                break;
            case E_TELNET_STE_SUB_SND_REPL_OPTIONS:
                telnet_send_and_proceed((void *)telnet_options_repl, sizeof(telnet_options_repl), E_TELNET_STE_SUB_LOGGIN_SUCCESS);
                break;
            case E_TELNET_STE_SUB_LOGGIN_SUCCESS:
                if (E_TELNET_RESULT_OK == telnet_send_non_blocking((void *)telnet_loggin_success, strlen(telnet_loggin_success))) {
                    // clear the current line and force the prompt
                    telnet_reset_buffer();
                    telnet_data.state= E_TELNET_STE_LOGGED_IN;
                }
            default:
                break;
            }
            break;
        case E_TELNET_STE_LOGGED_IN:
            telnet_process();
            break;
        default:
            break;
    }

    if (telnet_data.state >= E_TELNET_STE_CONNECTED) {
        if (telnet_data.timeout++ > (servers_get_timeout() / TELNET_CYCLE_TIME_MS)) {
            telnet_reset();
        }
    }
}

void telnet_tx_strn (const char *str, int len) {
    if (telnet_data.n_sd > 0 && telnet_data.state == E_TELNET_STE_LOGGED_IN && len > 0) {
        telnet_send_with_retries(telnet_data.n_sd, str, len);
    }
}

bool telnet_rx_any (void) {
    return (telnet_data.n_sd > 0) ? (telnet_data.rxRindex != telnet_data.rxWindex &&
            telnet_data.state == E_TELNET_STE_LOGGED_IN) : false;
}

int telnet_rx_char (void) {
    int rx_char = -1;
    if (telnet_data.rxRindex != telnet_data.rxWindex) {
        // rxRindex must be uint8_t and TELNET_RX_BUFFER_SIZE == 256 so that it wraps around automatically
        rx_char = (int)telnet_data.rxBuffer[telnet_data.rxRindex++];
    }
    return rx_char;
}

void telnet_enable (void) {
    telnet_data.enabled = true;
}

void telnet_disable (void) {
    telnet_reset();
    telnet_data.enabled = false;
    telnet_data.state = E_TELNET_STE_DISABLED;
}

void telnet_reset (void) {
    // close the connection and start all over again
    servers_close_socket(&telnet_data.n_sd);
    servers_close_socket(&telnet_data.sd);
    telnet_data.state = E_TELNET_STE_START;
}

/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/
static void telnet_wait_for_enabled (void) {
    // Init telnet's data
    telnet_data.n_sd = -1;
    telnet_data.sd   = -1;

    // Check if the telnet service has been enabled
    if (telnet_data.enabled) {
        telnet_data.state = E_TELNET_STE_START;
    }
}

static bool telnet_create_socket (void) {
    SlSockNonblocking_t nonBlockingOption;
    SlSockAddrIn_t sServerAddress;
    _i16 result;

    // Open a socket for telnet
    ASSERT ((telnet_data.sd = sl_Socket(SL_AF_INET, SL_SOCK_STREAM, SL_IPPROTO_TCP)) > 0);
    if (telnet_data.sd > 0) {
        // add the socket to the network administration
        modusocket_socket_add(telnet_data.sd, false);

        // Enable non-blocking mode
        nonBlockingOption.NonblockingEnabled = 1;
        ASSERT ((result = sl_SetSockOpt(telnet_data.sd, SL_SOL_SOCKET, SL_SO_NONBLOCKING, &nonBlockingOption, sizeof(nonBlockingOption))) == SL_SOC_OK);

        // Bind the socket to a port number
        sServerAddress.sin_family = SL_AF_INET;
        sServerAddress.sin_addr.s_addr = SL_INADDR_ANY;
        sServerAddress.sin_port = sl_Htons(TELNET_PORT);

        ASSERT ((result |= sl_Bind(telnet_data.sd, (const SlSockAddr_t *)&sServerAddress, sizeof(sServerAddress))) == SL_SOC_OK);

        // Start listening
        ASSERT ((result |= sl_Listen (telnet_data.sd, TELNET_MAX_CLIENTS)) == SL_SOC_OK);

        if (result == SL_SOC_OK) {
            return true;
        }
        servers_close_socket(&telnet_data.sd);
    }

    return false;
}

static void telnet_wait_for_connection (void) {
    SlSocklen_t  in_addrSize;
    SlSockAddrIn_t sClientAddress;

    // accepts a connection from a TCP client, if there is any, otherwise returns SL_EAGAIN
    telnet_data.n_sd = sl_Accept(telnet_data.sd, (SlSockAddr_t *)&sClientAddress, (SlSocklen_t *)&in_addrSize);
    if (telnet_data.n_sd == SL_EAGAIN) {
        return;
    }
    else {
        if (telnet_data.n_sd <= 0) {
            // error
            telnet_reset();
            return;
        }

        // close the listening socket, we don't need it anymore
        servers_close_socket(&telnet_data.sd);

        // add the new socket to the network administration
        modusocket_socket_add(telnet_data.n_sd, false);

        // client connected, so go on
        telnet_data.rxWindex = 0;
        telnet_data.rxRindex = 0;
        telnet_data.txRetries = 0;

        telnet_data.state = E_TELNET_STE_CONNECTED;
        telnet_data.substate.connected = E_TELNET_STE_SUB_WELCOME;
        telnet_data.credentialsValid = true;
        telnet_data.logginRetries = 0;
        telnet_data.timeout = 0;
    }
}

static void telnet_send_and_proceed (void *data, _i16 Len, telnet_connected_substate_t next_state) {
    if (E_TELNET_RESULT_OK == telnet_send_non_blocking(data, Len)) {
        telnet_data.substate.connected = next_state;
    }
}

static telnet_result_t telnet_send_non_blocking (void *data, _i16 Len) {
    int16_t result = sl_Send(telnet_data.n_sd, data, Len, 0);

    if (result > 0) {
        telnet_data.txRetries = 0;
        return E_TELNET_RESULT_OK;
    }
    else if ((TELNET_TX_RETRIES_MAX >= ++telnet_data.txRetries) && (result == SL_EAGAIN)) {
        return E_TELNET_RESULT_AGAIN;
    }
    else {
        // error
        telnet_reset();
        return E_TELNET_RESULT_FAILED;
    }
}

static telnet_result_t telnet_recv_text_non_blocking (void *buff, _i16 Maxlen, _i16 *rxLen) {
    *rxLen = sl_Recv(telnet_data.n_sd, buff, Maxlen, 0);
    // if there's data received, parse it
    if (*rxLen > 0) {
        telnet_data.timeout = 0;
        telnet_parse_input (buff, rxLen);
        if (*rxLen > 0) {
            return E_TELNET_RESULT_OK;
        }
    }
    else if (*rxLen != SL_EAGAIN) {
        // error
        telnet_reset();
        return E_TELNET_RESULT_FAILED;
    }
    return E_TELNET_RESULT_AGAIN;
}

static void telnet_process (void) {
    _i16 rxLen;
    _i16 maxLen = (telnet_data.rxWindex >= telnet_data.rxRindex) ? (TELNET_RX_BUFFER_SIZE - telnet_data.rxWindex) :
                                                                   ((telnet_data.rxRindex - telnet_data.rxWindex) - 1);
    // to avoid an overrun
    maxLen = (telnet_data.rxRindex == 0) ? (maxLen - 1) : maxLen;

    if (maxLen > 0) {
        if (E_TELNET_RESULT_OK == telnet_recv_text_non_blocking(&telnet_data.rxBuffer[telnet_data.rxWindex], maxLen, &rxLen)) {
            // rxWindex must be uint8_t and TELNET_RX_BUFFER_SIZE == 256 so that it wraps around automatically
            telnet_data.rxWindex = telnet_data.rxWindex + rxLen;
        }
    }
}

static int telnet_process_credential (char *credential, _i16 rxLen) {
    telnet_data.rxWindex += rxLen;
    if (telnet_data.rxWindex >= SERVERS_USER_PASS_LEN_MAX) {
        telnet_data.rxWindex = SERVERS_USER_PASS_LEN_MAX;
    }

    uint8_t *p = telnet_data.rxBuffer + SERVERS_USER_PASS_LEN_MAX;
    // if a '\r' is found, or the length exceeds the max username length
    if ((p = memchr(telnet_data.rxBuffer, '\r', telnet_data.rxWindex)) || (telnet_data.rxWindex >= SERVERS_USER_PASS_LEN_MAX)) {
        uint8_t len = p - telnet_data.rxBuffer;

        telnet_data.rxWindex = 0;
        if ((len > 0) && (memcmp(credential, telnet_data.rxBuffer, MAX(len, strlen(credential))) == 0)) {
            return 1;
        }
        return -1;
    }
    return 0;
}

static void telnet_parse_input (uint8_t *str, int16_t *len) {
    int16_t b_len = *len;
    uint8_t *b_str = str;

    for (uint8_t *_str = b_str; _str < b_str + b_len; ) {
        if (*_str <= 127) {
            if (telnet_data.state == E_TELNET_STE_LOGGED_IN && *_str == mp_interrupt_char) {
                // raise a keyboard exception
                mp_sched_keyboard_interrupt();
                (*len)--;
                _str++;
            }
            else if (*_str > 0) {
                *str++ = *_str++;
            }
            else {
                _str++;
                *len -= 1;
            }
        }
        else {
            // in case we have received an incomplete telnet option, unlikely, but possible
            _str += MIN(3, *len);
            *len -= MIN(3, *len);
        }
    }
}

static bool telnet_send_with_retries (int16_t sd, const void *pBuf, int16_t len) {
    int32_t retries = 0;
    uint32_t delay = TELNET_WAIT_TIME_MS;
    // only if we are not within interrupt context and interrupts are enabled
    if ((HAL_NVIC_INT_CTRL_REG & HAL_VECTACTIVE_MASK) == 0 && query_irq() == IRQ_STATE_ENABLED) {
        do {
            _i16 result = sl_Send(sd, pBuf, len, 0);
            if (result > 0) {
                return true;
            }
            else if (SL_EAGAIN != result) {
                return false;
            }
            // start with the default delay and increment it on each retry
            mp_hal_delay_ms(delay++);
        } while (++retries <= TELNET_TX_RETRIES_MAX);
    }
    return false;
}

static void telnet_reset_buffer (void) {
    // erase any characters present in the current line
    memset (telnet_data.rxBuffer, '\b', TELNET_RX_BUFFER_SIZE / 2);
    telnet_data.rxWindex = TELNET_RX_BUFFER_SIZE / 2;
    // fake an "enter" key pressed to display the prompt
    telnet_data.rxBuffer[telnet_data.rxWindex++] = '\r';
}

