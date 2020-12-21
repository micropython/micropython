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
#include <stdio.h>

#include "py/runtime.h"
#include "lib/timeutils/timeutils.h"
#include "lib/oofatfs/ff.h"
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "rom_map.h"
#include "prcm.h"
#include "pybrtc.h"
#include "ftp.h"
#include "simplelink.h"
#include "modnetwork.h"
#include "modwlan.h"
#include "modusocket.h"
#include "debug.h"
#include "serverstask.h"
#include "fifo.h"
#include "socketfifo.h"
#include "updater.h"
#include "moduos.h"

/******************************************************************************
 DEFINE PRIVATE CONSTANTS
 ******************************************************************************/
#define FTP_CMD_PORT                        21
#define FTP_ACTIVE_DATA_PORT                20
#define FTP_PASIVE_DATA_PORT                2024
#define FTP_BUFFER_SIZE                     512
#define FTP_TX_RETRIES_MAX                  25
#define FTP_CMD_SIZE_MAX                    6
#define FTP_CMD_CLIENTS_MAX                 1
#define FTP_DATA_CLIENTS_MAX                1
#define FTP_MAX_PARAM_SIZE                  (MICROPY_ALLOC_PATH_MAX + 1)
#define FTP_UNIX_TIME_20000101              946684800
#define FTP_UNIX_TIME_20150101              1420070400
#define FTP_UNIX_SECONDS_180_DAYS           15552000
#define FTP_DATA_TIMEOUT_MS                 5000            // 5 seconds
#define FTP_SOCKETFIFO_ELEMENTS_MAX         4
#define FTP_CYCLE_TIME_MS                   (SERVERS_CYCLE_TIME_MS * 2)

/******************************************************************************
 DEFINE PRIVATE TYPES
 ******************************************************************************/
typedef enum {
    E_FTP_RESULT_OK = 0,
    E_FTP_RESULT_CONTINUE,
    E_FTP_RESULT_FAILED
} ftp_result_t;

typedef enum {
    E_FTP_STE_DISABLED = 0,
    E_FTP_STE_START,
    E_FTP_STE_READY,
    E_FTP_STE_END_TRANSFER,
    E_FTP_STE_CONTINUE_LISTING,
    E_FTP_STE_CONTINUE_FILE_TX,
    E_FTP_STE_CONTINUE_FILE_RX
} ftp_state_t;

typedef enum {
    E_FTP_STE_SUB_DISCONNECTED = 0,
    E_FTP_STE_SUB_LISTEN_FOR_DATA,
    E_FTP_STE_SUB_DATA_CONNECTED
} ftp_substate_t;

typedef struct {
    bool            uservalid : 1;
    bool            passvalid : 1;
} ftp_loggin_t;

typedef enum {
    E_FTP_NOTHING_OPEN = 0,
    E_FTP_FILE_OPEN,
    E_FTP_DIR_OPEN
} ftp_e_open_t;

typedef enum {
    E_FTP_CLOSE_NONE = 0,
    E_FTP_CLOSE_DATA,
    E_FTP_CLOSE_CMD_AND_DATA,
} ftp_e_closesocket_t;

typedef struct {
    uint8_t             *dBuffer;
    uint32_t            ctimeout;
    union {
        FF_DIR          dp;
        FIL             fp;
    };
    int16_t             lc_sd;
    int16_t             ld_sd;
    int16_t             c_sd;
    int16_t             d_sd;
    int16_t             dtimeout;
    uint16_t            volcount;
    uint8_t             state;
    uint8_t             substate;
    uint8_t             txRetries;
    uint8_t             logginRetries;
    ftp_loggin_t        loggin;
    uint8_t             e_open;
    bool                closechild;
    bool                enabled;
    bool                special_file;
    bool                listroot;
} ftp_data_t;

typedef struct {
    char * cmd;
} ftp_cmd_t;

typedef struct {
    char * month;
} ftp_month_t;

typedef enum {
    E_FTP_CMD_NOT_SUPPORTED = -1,
    E_FTP_CMD_FEAT = 0,
    E_FTP_CMD_SYST,
    E_FTP_CMD_CDUP,
    E_FTP_CMD_CWD,
    E_FTP_CMD_PWD,
    E_FTP_CMD_XPWD,
    E_FTP_CMD_SIZE,
    E_FTP_CMD_MDTM,
    E_FTP_CMD_TYPE,
    E_FTP_CMD_USER,
    E_FTP_CMD_PASS,
    E_FTP_CMD_PASV,
    E_FTP_CMD_LIST,
    E_FTP_CMD_RETR,
    E_FTP_CMD_STOR,
    E_FTP_CMD_DELE,
    E_FTP_CMD_RMD,
    E_FTP_CMD_MKD,
    E_FTP_CMD_RNFR,
    E_FTP_CMD_RNTO,
    E_FTP_CMD_NOOP,
    E_FTP_CMD_QUIT,
    E_FTP_NUM_FTP_CMDS
} ftp_cmd_index_t;

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
static ftp_data_t ftp_data;
static char *ftp_path;
static char *ftp_scratch_buffer;
static char *ftp_cmd_buffer;
static const ftp_cmd_t ftp_cmd_table[] = { { "FEAT" }, { "SYST" }, { "CDUP" }, { "CWD"  },
                                           { "PWD"  }, { "XPWD" }, { "SIZE" }, { "MDTM" },
                                           { "TYPE" }, { "USER" }, { "PASS" }, { "PASV" },
                                           { "LIST" }, { "RETR" }, { "STOR" }, { "DELE" },
                                           { "RMD"  }, { "MKD"  }, { "RNFR" }, { "RNTO" },
                                           { "NOOP" }, { "QUIT" } };

static const ftp_month_t ftp_month[] = { { "Jan" }, { "Feb" }, { "Mar" }, { "Apr" },
                                         { "May" }, { "Jun" }, { "Jul" }, { "Ago" },
                                         { "Sep" }, { "Oct" }, { "Nov" }, { "Dec" } };

static SocketFifoElement_t ftp_fifoelements[FTP_SOCKETFIFO_ELEMENTS_MAX];
static FIFO_t ftp_socketfifo;

/******************************************************************************
 DEFINE VFS WRAPPER FUNCTIONS
 ******************************************************************************/

// These wrapper functions are used so that the FTP server can access the
// mounted FATFS devices directly without going through the costly mp_vfs_XXX
// functions.  The latter may raise exceptions and we would then need to wrap
// all calls in an nlr handler.  The wrapper functions below assume that there
// are only FATFS filesystems mounted.

STATIC FATFS *lookup_path(const TCHAR **path) {
    mp_vfs_mount_t *fs = mp_vfs_lookup_path(*path, path);
    if (fs == MP_VFS_NONE || fs == MP_VFS_ROOT) {
        return NULL;
    }
    // here we assume that the mounted device is FATFS
    return &((fs_user_mount_t*)MP_OBJ_TO_PTR(fs->obj))->fatfs;
}

STATIC FRESULT f_open_helper(FIL *fp, const TCHAR *path, BYTE mode) {
    FATFS *fs = lookup_path(&path);
    if (fs == NULL) {
        return FR_NO_PATH;
    }
    return f_open(fs, fp, path, mode);
}

STATIC FRESULT f_opendir_helper(FF_DIR *dp, const TCHAR *path) {
    FATFS *fs = lookup_path(&path);
    if (fs == NULL) {
        return FR_NO_PATH;
    }
    return f_opendir(fs, dp, path);
}

STATIC FRESULT f_stat_helper(const TCHAR *path, FILINFO *fno) {
    FATFS *fs = lookup_path(&path);
    if (fs == NULL) {
        return FR_NO_PATH;
    }
    return f_stat(fs, path, fno);
}

STATIC FRESULT f_mkdir_helper(const TCHAR *path) {
    FATFS *fs = lookup_path(&path);
    if (fs == NULL) {
        return FR_NO_PATH;
    }
    return f_mkdir(fs, path);
}

STATIC FRESULT f_unlink_helper(const TCHAR *path) {
    FATFS *fs = lookup_path(&path);
    if (fs == NULL) {
        return FR_NO_PATH;
    }
    return f_unlink(fs, path);
}

STATIC FRESULT f_rename_helper(const TCHAR *path_old, const TCHAR *path_new) {
    FATFS *fs_old = lookup_path(&path_old);
    if (fs_old == NULL) {
        return FR_NO_PATH;
    }
    FATFS *fs_new = lookup_path(&path_new);
    if (fs_new == NULL) {
        return FR_NO_PATH;
    }
    if (fs_old != fs_new) {
        return FR_NO_PATH;
    }
    return f_rename(fs_new, path_old, path_new);
}

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
static void ftp_wait_for_enabled (void);
static bool ftp_create_listening_socket (_i16 *sd, _u16 port, _u8 backlog);
static ftp_result_t ftp_wait_for_connection (_i16 l_sd, _i16 *n_sd);
static ftp_result_t ftp_send_non_blocking (_i16 sd, void *data, _i16 Len);
static void ftp_send_reply (_u16 status, char *message);
static void ftp_send_data (_u32 datasize);
static void ftp_send_from_fifo (void);
static ftp_result_t ftp_recv_non_blocking (_i16 sd, void *buff, _i16 Maxlen, _i32 *rxLen);
static void ftp_process_cmd (void);
static void ftp_close_files (void);
static void ftp_close_filesystem_on_error (void);
static void ftp_close_cmd_data (void);
static ftp_cmd_index_t ftp_pop_command (char **str);
static void ftp_pop_param (char **str, char *param);
static int ftp_print_eplf_item (char *dest, uint32_t destsize, FILINFO *fno);
static int ftp_print_eplf_drive (char *dest, uint32_t destsize, const char *name);
static bool ftp_open_file (const char *path, int mode);
static ftp_result_t ftp_read_file (char *filebuf, uint32_t desiredsize, uint32_t *actualsize);
static ftp_result_t ftp_write_file (char *filebuf, uint32_t size);
static ftp_result_t ftp_open_dir_for_listing (const char *path);
static ftp_result_t ftp_list_dir (char *list, uint32_t maxlistsize, uint32_t *listsize);
static void ftp_open_child (char *pwd, char *dir);
static void ftp_close_child (char *pwd);
static void ftp_return_to_previous_path (char *pwd, char *dir);

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
void ftp_init (void) {
    // Allocate memory for the data buffer, and the file system structs (from the RTOS heap)
    ASSERT ((ftp_data.dBuffer = mem_Malloc(FTP_BUFFER_SIZE)) != NULL);
    ASSERT ((ftp_path = mem_Malloc(FTP_MAX_PARAM_SIZE)) != NULL);
    ASSERT ((ftp_scratch_buffer = mem_Malloc(FTP_MAX_PARAM_SIZE)) != NULL);
    ASSERT ((ftp_cmd_buffer = mem_Malloc(FTP_MAX_PARAM_SIZE + FTP_CMD_SIZE_MAX)) != NULL);
    SOCKETFIFO_Init (&ftp_socketfifo, (void *)ftp_fifoelements, FTP_SOCKETFIFO_ELEMENTS_MAX);
    ftp_data.c_sd  = -1;
    ftp_data.d_sd  = -1;
    ftp_data.lc_sd = -1;
    ftp_data.ld_sd = -1;
    ftp_data.e_open = E_FTP_NOTHING_OPEN;
    ftp_data.state = E_FTP_STE_DISABLED;
    ftp_data.substate = E_FTP_STE_SUB_DISCONNECTED;
    ftp_data.special_file = false;
    ftp_data.volcount = 0;
}

void ftp_run (void) {
    switch (ftp_data.state) {
        case E_FTP_STE_DISABLED:
            ftp_wait_for_enabled();
            break;
        case E_FTP_STE_START:
            if (wlan_is_connected() && ftp_create_listening_socket(&ftp_data.lc_sd, FTP_CMD_PORT, FTP_CMD_CLIENTS_MAX - 1)) {
                ftp_data.state = E_FTP_STE_READY;
            }
            break;
        case E_FTP_STE_READY:
            if (ftp_data.c_sd < 0 && ftp_data.substate == E_FTP_STE_SUB_DISCONNECTED) {
                if (E_FTP_RESULT_OK == ftp_wait_for_connection(ftp_data.lc_sd, &ftp_data.c_sd)) {
                    ftp_data.txRetries = 0;
                    ftp_data.logginRetries = 0;
                    ftp_data.ctimeout = 0;
                    ftp_data.loggin.uservalid = false;
                    ftp_data.loggin.passvalid = false;
                    strcpy (ftp_path, "/");
                    ftp_send_reply (220, "MicroPython FTP Server");
                    break;
                }
            }
            if (SOCKETFIFO_IsEmpty()) {
                if (ftp_data.c_sd > 0 && ftp_data.substate != E_FTP_STE_SUB_LISTEN_FOR_DATA) {
                    ftp_process_cmd();
                    if (ftp_data.state != E_FTP_STE_READY) {
                        break;
                    }
                }
            }
            break;
        case E_FTP_STE_END_TRANSFER:
            break;
        case E_FTP_STE_CONTINUE_LISTING:
            // go on with listing only if the transmit buffer is empty
            if (SOCKETFIFO_IsEmpty()) {
                uint32_t listsize;
                ftp_list_dir((char *)ftp_data.dBuffer, FTP_BUFFER_SIZE, &listsize);
                if (listsize > 0) {
                    ftp_send_data(listsize);
                } else {
                    ftp_send_reply(226, NULL);
                    ftp_data.state = E_FTP_STE_END_TRANSFER;
                }
                ftp_data.ctimeout = 0;
            }
            break;
        case E_FTP_STE_CONTINUE_FILE_TX:
            // read the next block from the file only if the previous one has been sent
            if (SOCKETFIFO_IsEmpty()) {
                uint32_t readsize;
                ftp_result_t result;
                ftp_data.ctimeout = 0;
                result = ftp_read_file ((char *)ftp_data.dBuffer, FTP_BUFFER_SIZE, &readsize);
                if (result == E_FTP_RESULT_FAILED) {
                    ftp_send_reply(451, NULL);
                    ftp_data.state = E_FTP_STE_END_TRANSFER;
                }
                else {
                    if (readsize > 0) {
                        ftp_send_data(readsize);
                    }
                    if (result == E_FTP_RESULT_OK) {
                        ftp_send_reply(226, NULL);
                        ftp_data.state = E_FTP_STE_END_TRANSFER;
                    }
                }
            }
            break;
        case E_FTP_STE_CONTINUE_FILE_RX:
            if (SOCKETFIFO_IsEmpty()) {
                _i32 len;
                ftp_result_t result;
                if (E_FTP_RESULT_OK == (result = ftp_recv_non_blocking(ftp_data.d_sd, ftp_data.dBuffer, FTP_BUFFER_SIZE, &len))) {
                    ftp_data.dtimeout = 0;
                    ftp_data.ctimeout = 0;
                    // its a software update
                    if (ftp_data.special_file) {
                        if (updater_write(ftp_data.dBuffer, len)) {
                            break;
                        }
                    }
                    // user file being received
                    else if (E_FTP_RESULT_OK == ftp_write_file ((char *)ftp_data.dBuffer, len)) {
                        break;
                    }
                    ftp_send_reply(451, NULL);
                    ftp_data.state = E_FTP_STE_END_TRANSFER;
                }
                else if (result == E_FTP_RESULT_CONTINUE) {
                    if (ftp_data.dtimeout++ > FTP_DATA_TIMEOUT_MS / FTP_CYCLE_TIME_MS) {
                        ftp_close_files();
                        ftp_send_reply(426, NULL);
                        ftp_data.state = E_FTP_STE_END_TRANSFER;
                    }
                }
                else {
                    if (ftp_data.special_file) {
                        ftp_data.special_file = false;
                        updater_finnish();
                    }
                    ftp_close_files();
                    ftp_send_reply(226, NULL);
                    ftp_data.state = E_FTP_STE_END_TRANSFER;
                }
            }
            break;
        default:
            break;
    }

    switch (ftp_data.substate) {
    case E_FTP_STE_SUB_DISCONNECTED:
        break;
    case E_FTP_STE_SUB_LISTEN_FOR_DATA:
        if (E_FTP_RESULT_OK == ftp_wait_for_connection(ftp_data.ld_sd, &ftp_data.d_sd)) {
            ftp_data.dtimeout = 0;
            ftp_data.substate = E_FTP_STE_SUB_DATA_CONNECTED;
        }
        else if (ftp_data.dtimeout++ > FTP_DATA_TIMEOUT_MS / FTP_CYCLE_TIME_MS) {
            ftp_data.dtimeout = 0;
            // close the listening socket
            servers_close_socket(&ftp_data.ld_sd);
            ftp_data.substate = E_FTP_STE_SUB_DISCONNECTED;
        }
        break;
    case E_FTP_STE_SUB_DATA_CONNECTED:
        if (ftp_data.state == E_FTP_STE_READY && ftp_data.dtimeout++ > FTP_DATA_TIMEOUT_MS / FTP_CYCLE_TIME_MS) {
            // close the listening and the data socket
            servers_close_socket(&ftp_data.ld_sd);
            servers_close_socket(&ftp_data.d_sd);
            ftp_close_filesystem_on_error ();
            ftp_data.substate = E_FTP_STE_SUB_DISCONNECTED;
        }
        break;
    default:
        break;
    }

    // send data pending in the queue
    ftp_send_from_fifo();

    // check the state of the data sockets
    if (ftp_data.d_sd < 0 && (ftp_data.state > E_FTP_STE_READY)) {
        ftp_data.substate = E_FTP_STE_SUB_DISCONNECTED;
        ftp_data.state = E_FTP_STE_READY;
    }
}

void ftp_enable (void) {
    ftp_data.enabled = true;
}

void ftp_disable (void) {
    ftp_reset();
    ftp_data.enabled = false;
    ftp_data.state = E_FTP_STE_DISABLED;
}

void ftp_reset (void) {
    // close all connections and start all over again
    servers_close_socket(&ftp_data.lc_sd);
    servers_close_socket(&ftp_data.ld_sd);
    ftp_close_cmd_data();
    ftp_data.state = E_FTP_STE_START;
    ftp_data.substate = E_FTP_STE_SUB_DISCONNECTED;
    ftp_data.volcount = 0;
    SOCKETFIFO_Flush();
}

/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/
static void ftp_wait_for_enabled (void) {
    // Check if the telnet service has been enabled
    if (ftp_data.enabled) {
        ftp_data.state = E_FTP_STE_START;
    }
}

static bool ftp_create_listening_socket (_i16 *sd, _u16 port, _u8 backlog) {
    SlSockNonblocking_t nonBlockingOption;
    SlSockAddrIn_t sServerAddress;
    _i16 _sd;
    _i16 result;

    // Open a socket for ftp data listen
    ASSERT ((*sd = sl_Socket(SL_AF_INET, SL_SOCK_STREAM, SL_IPPROTO_IP)) > 0);
    _sd = *sd;

    if (_sd > 0) {
        // add the new socket to the network administration
        modusocket_socket_add(_sd, false);

        // Enable non-blocking mode
        nonBlockingOption.NonblockingEnabled = 1;
        ASSERT ((result = sl_SetSockOpt(_sd, SL_SOL_SOCKET, SL_SO_NONBLOCKING, &nonBlockingOption, sizeof(nonBlockingOption))) == SL_SOC_OK);

        // Bind the socket to a port number
        sServerAddress.sin_family = SL_AF_INET;
        sServerAddress.sin_addr.s_addr = SL_INADDR_ANY;
        sServerAddress.sin_port = sl_Htons(port);

        ASSERT ((result |= sl_Bind(_sd, (const SlSockAddr_t *)&sServerAddress, sizeof(sServerAddress))) == SL_SOC_OK);

        // Start listening
        ASSERT ((result |= sl_Listen (_sd, backlog)) == SL_SOC_OK);

        if (result == SL_SOC_OK) {
            return true;
        }
        servers_close_socket(sd);
    }
    return false;
}

static ftp_result_t ftp_wait_for_connection (_i16 l_sd, _i16 *n_sd) {
    SlSockAddrIn_t sClientAddress;
    SlSocklen_t  in_addrSize;

    // accepts a connection from a TCP client, if there is any, otherwise returns SL_EAGAIN
    *n_sd = sl_Accept(l_sd, (SlSockAddr_t *)&sClientAddress, (SlSocklen_t *)&in_addrSize);
    _i16 _sd = *n_sd;
    if (_sd == SL_EAGAIN) {
        return E_FTP_RESULT_CONTINUE;
    }
    else if (_sd < 0) {
        // error
        ftp_reset();
        return E_FTP_RESULT_FAILED;
    }

    // add the new socket to the network administration
    modusocket_socket_add(_sd, false);

    // client connected, so go on
    return E_FTP_RESULT_OK;
}

static ftp_result_t ftp_send_non_blocking (_i16 sd, void *data, _i16 Len) {
    int16_t result = sl_Send(sd, data, Len, 0);

    if (result > 0) {
        ftp_data.txRetries = 0;
        return E_FTP_RESULT_OK;
    }
    else if ((FTP_TX_RETRIES_MAX >= ++ftp_data.txRetries) && (result == SL_EAGAIN)) {
        return E_FTP_RESULT_CONTINUE;
    }
    else {
        // error
        ftp_reset();
        return E_FTP_RESULT_FAILED;
    }
}

static void ftp_send_reply (_u16 status, char *message) {
    SocketFifoElement_t fifoelement;
    if (!message) {
        message = "";
    }
    snprintf((char *)ftp_cmd_buffer, 4, "%u", status);
    strcat ((char *)ftp_cmd_buffer, " ");
    strcat ((char *)ftp_cmd_buffer, message);
    strcat ((char *)ftp_cmd_buffer, "\r\n");
    fifoelement.sd = &ftp_data.c_sd;
    fifoelement.datasize = strlen((char *)ftp_cmd_buffer);
    fifoelement.data = mem_Malloc(fifoelement.datasize);
    if (status == 221) {
        fifoelement.closesockets = E_FTP_CLOSE_CMD_AND_DATA;
    }
    else if (status == 426 || status == 451 || status == 550) {
        fifoelement.closesockets = E_FTP_CLOSE_DATA;
    }
    else {
        fifoelement.closesockets = E_FTP_CLOSE_NONE;
    }
    fifoelement.freedata = true;
    if (fifoelement.data) {
        memcpy (fifoelement.data, ftp_cmd_buffer, fifoelement.datasize);
        if (!SOCKETFIFO_Push (&fifoelement)) {
            mem_Free(fifoelement.data);
        }
    }
}

static void ftp_send_data (_u32 datasize) {
    SocketFifoElement_t fifoelement;

    fifoelement.data = ftp_data.dBuffer;
    fifoelement.datasize = datasize;
    fifoelement.sd = &ftp_data.d_sd;
    fifoelement.closesockets = E_FTP_CLOSE_NONE;
    fifoelement.freedata = false;
    SOCKETFIFO_Push (&fifoelement);
}

static void ftp_send_from_fifo (void) {
    SocketFifoElement_t fifoelement;
    if (SOCKETFIFO_Peek (&fifoelement)) {
        _i16 _sd = *fifoelement.sd;
        if (_sd > 0) {
            if (E_FTP_RESULT_OK == ftp_send_non_blocking (_sd, fifoelement.data, fifoelement.datasize)) {
                SOCKETFIFO_Pop (&fifoelement);
                if (fifoelement.closesockets != E_FTP_CLOSE_NONE) {
                    servers_close_socket(&ftp_data.d_sd);
                    if (fifoelement.closesockets == E_FTP_CLOSE_CMD_AND_DATA) {
                        servers_close_socket(&ftp_data.ld_sd);
                        // this one is the command socket
                        servers_close_socket(fifoelement.sd);
                        ftp_data.substate = E_FTP_STE_SUB_DISCONNECTED;
                    }
                    ftp_close_filesystem_on_error();
                }
                if (fifoelement.freedata) {
                    mem_Free(fifoelement.data);
                }
            }
        }
        // socket closed, remove from the queue
        else {
            SOCKETFIFO_Pop (&fifoelement);
            if (fifoelement.freedata) {
                mem_Free(fifoelement.data);
            }
        }
    }
    else if (ftp_data.state == E_FTP_STE_END_TRANSFER && (ftp_data.d_sd > 0)) {
        // close the listening and the data sockets
        servers_close_socket(&ftp_data.ld_sd);
        servers_close_socket(&ftp_data.d_sd);
        if (ftp_data.special_file) {
            ftp_data.special_file = false;
        }
    }
}

static ftp_result_t ftp_recv_non_blocking (_i16 sd, void *buff, _i16 Maxlen, _i32 *rxLen) {
    *rxLen = sl_Recv(sd, buff, Maxlen, 0);

    if (*rxLen > 0) {
        return E_FTP_RESULT_OK;
    }
    else if (*rxLen != SL_EAGAIN) {
        // error
        return E_FTP_RESULT_FAILED;
    }
    return E_FTP_RESULT_CONTINUE;
}

static void ftp_get_param_and_open_child (char **bufptr) {
    ftp_pop_param (bufptr, ftp_scratch_buffer);
    ftp_open_child (ftp_path, ftp_scratch_buffer);
    ftp_data.closechild = true;
}

static void ftp_process_cmd (void) {
    _i32 len;
    char *bufptr = (char *)ftp_cmd_buffer;
    ftp_result_t result;
    FRESULT fres;
    FILINFO fno;

    ftp_data.closechild = false;
    // also use the reply buffer to receive new commands
    if (E_FTP_RESULT_OK == (result = ftp_recv_non_blocking(ftp_data.c_sd, ftp_cmd_buffer, FTP_MAX_PARAM_SIZE + FTP_CMD_SIZE_MAX, &len))) {
        // bufptr is moved as commands are being popped
        ftp_cmd_index_t cmd = ftp_pop_command(&bufptr);
        if (!ftp_data.loggin.passvalid && (cmd != E_FTP_CMD_USER && cmd != E_FTP_CMD_PASS && cmd != E_FTP_CMD_QUIT && cmd != E_FTP_CMD_FEAT)) {
            ftp_send_reply(332, NULL);
            return;
        }
        switch (cmd) {
        case E_FTP_CMD_FEAT:
            ftp_send_reply(211, "no-features");
            break;
        case E_FTP_CMD_SYST:
            ftp_send_reply(215, "UNIX Type: L8");
            break;
        case E_FTP_CMD_CDUP:
            ftp_close_child(ftp_path);
            ftp_send_reply(250, NULL);
            break;
        case E_FTP_CMD_CWD:
            {
                fres = FR_NO_PATH;
                ftp_pop_param (&bufptr, ftp_scratch_buffer);
                ftp_open_child (ftp_path, ftp_scratch_buffer);
                if ((ftp_path[0] == '/' && ftp_path[1] == '\0') || ((fres = f_opendir_helper (&ftp_data.dp, ftp_path)) == FR_OK)) {
                    if (fres == FR_OK) {
                        f_closedir(&ftp_data.dp);
                    }
                    ftp_send_reply(250, NULL);
                }
                else {
                    ftp_close_child (ftp_path);
                    ftp_send_reply(550, NULL);
                }
            }
            break;
        case E_FTP_CMD_PWD:
        case E_FTP_CMD_XPWD:
            snprintf((char *)ftp_data.dBuffer, FTP_BUFFER_SIZE, "\"%s\"", ftp_path);
            ftp_send_reply(257, (char *)ftp_data.dBuffer);
            break;
        case E_FTP_CMD_SIZE:
            {
                ftp_get_param_and_open_child (&bufptr);
                if (FR_OK == f_stat_helper(ftp_path, &fno)) {
                    // send the size
                    snprintf((char *)ftp_data.dBuffer, FTP_BUFFER_SIZE, "%u", (_u32)fno.fsize);
                    ftp_send_reply(213, (char *)ftp_data.dBuffer);
                }
                else {
                    ftp_send_reply(550, NULL);
                }
            }
            break;
        case E_FTP_CMD_MDTM:
            ftp_get_param_and_open_child (&bufptr);
            if (FR_OK == f_stat_helper(ftp_path, &fno)) {
                // send the last modified time
                snprintf((char *)ftp_data.dBuffer, FTP_BUFFER_SIZE, "%u%02u%02u%02u%02u%02u",
                         1980 + ((fno.fdate >> 9) & 0x7f), (fno.fdate >> 5) & 0x0f,
                         fno.fdate & 0x1f, (fno.ftime >> 11) & 0x1f,
                         (fno.ftime >> 5) & 0x3f, 2 * (fno.ftime & 0x1f));
                ftp_send_reply(213, (char *)ftp_data.dBuffer);
            }
            else {
                ftp_send_reply(550, NULL);
            }
            break;
        case E_FTP_CMD_TYPE:
            ftp_send_reply(200, NULL);
            break;
        case E_FTP_CMD_USER:
            ftp_pop_param (&bufptr, ftp_scratch_buffer);
            if (!memcmp(ftp_scratch_buffer, servers_user, MAX(strlen(ftp_scratch_buffer), strlen(servers_user)))) {
                ftp_data.loggin.uservalid = true && (strlen(servers_user) == strlen(ftp_scratch_buffer));
            }
            ftp_send_reply(331, NULL);
            break;
        case E_FTP_CMD_PASS:
            ftp_pop_param (&bufptr, ftp_scratch_buffer);
            if (!memcmp(ftp_scratch_buffer, servers_pass, MAX(strlen(ftp_scratch_buffer), strlen(servers_pass))) &&
                    ftp_data.loggin.uservalid) {
                ftp_data.loggin.passvalid = true && (strlen(servers_pass) == strlen(ftp_scratch_buffer));
                if (ftp_data.loggin.passvalid) {
                    ftp_send_reply(230, NULL);
                    break;
                }
            }
            ftp_send_reply(530, NULL);
            break;
        case E_FTP_CMD_PASV:
            {
                // some servers (e.g. google chrome) send PASV several times very quickly
                servers_close_socket(&ftp_data.d_sd);
                ftp_data.substate = E_FTP_STE_SUB_DISCONNECTED;
                bool socketcreated = true;
                if (ftp_data.ld_sd < 0) {
                    socketcreated = ftp_create_listening_socket(&ftp_data.ld_sd, FTP_PASIVE_DATA_PORT, FTP_DATA_CLIENTS_MAX - 1);
                }
                if (socketcreated) {
                    uint32_t ip;
                    uint8_t *pip = (uint8_t *)&ip;
                    ftp_data.dtimeout = 0;
                    wlan_get_ip(&ip);
                    snprintf((char *)ftp_data.dBuffer, FTP_BUFFER_SIZE, "(%u,%u,%u,%u,%u,%u)",
                             pip[3], pip[2], pip[1], pip[0], (FTP_PASIVE_DATA_PORT >> 8), (FTP_PASIVE_DATA_PORT & 0xFF));
                    ftp_data.substate = E_FTP_STE_SUB_LISTEN_FOR_DATA;
                    ftp_send_reply(227, (char *)ftp_data.dBuffer);
                }
                else {
                    ftp_send_reply(425, NULL);
                }
            }
            break;
        case E_FTP_CMD_LIST:
            if (ftp_open_dir_for_listing(ftp_path) == E_FTP_RESULT_CONTINUE) {
                ftp_data.state = E_FTP_STE_CONTINUE_LISTING;
                ftp_send_reply(150, NULL);
            }
            else {
                ftp_send_reply(550, NULL);
            }
            break;
        case E_FTP_CMD_RETR:
            ftp_get_param_and_open_child (&bufptr);
            if (ftp_open_file (ftp_path, FA_READ)) {
                ftp_data.state = E_FTP_STE_CONTINUE_FILE_TX;
                ftp_send_reply(150, NULL);
            }
            else {
                ftp_data.state = E_FTP_STE_END_TRANSFER;
                ftp_send_reply(550, NULL);
            }
            break;
        case E_FTP_CMD_STOR:
            ftp_get_param_and_open_child (&bufptr);
            // first check if a software update is being requested
            if (updater_check_path (ftp_path)) {
                if (updater_start()) {
                    ftp_data.special_file = true;
                    ftp_data.state = E_FTP_STE_CONTINUE_FILE_RX;
                    ftp_send_reply(150, NULL);
                }
                else {
                    // to unlock the updater
                    updater_finnish();
                    ftp_data.state = E_FTP_STE_END_TRANSFER;
                    ftp_send_reply(550, NULL);
                }
            }
            else {
                if (ftp_open_file (ftp_path, FA_WRITE | FA_CREATE_ALWAYS)) {
                    ftp_data.state = E_FTP_STE_CONTINUE_FILE_RX;
                    ftp_send_reply(150, NULL);
                }
                else {
                    ftp_data.state = E_FTP_STE_END_TRANSFER;
                    ftp_send_reply(550, NULL);
                }
            }
            break;
        case E_FTP_CMD_DELE:
        case E_FTP_CMD_RMD:
            ftp_get_param_and_open_child (&bufptr);
            if (FR_OK == f_unlink_helper(ftp_path)) {
                ftp_send_reply(250, NULL);
            }
            else {
                ftp_send_reply(550, NULL);
            }
            break;
        case E_FTP_CMD_MKD:
            ftp_get_param_and_open_child (&bufptr);
            if (FR_OK == f_mkdir_helper(ftp_path)) {
                ftp_send_reply(250, NULL);
            }
            else {
                ftp_send_reply(550, NULL);
            }
            break;
        case E_FTP_CMD_RNFR:
            ftp_get_param_and_open_child (&bufptr);
            if (FR_OK == f_stat_helper(ftp_path, &fno)) {
                ftp_send_reply(350, NULL);
                // save the current path
                strcpy ((char *)ftp_data.dBuffer, ftp_path);
            }
            else {
                ftp_send_reply(550, NULL);
            }
            break;
        case E_FTP_CMD_RNTO:
            ftp_get_param_and_open_child (&bufptr);
            // old path was saved in the data buffer
            if (FR_OK == (fres = f_rename_helper((char *)ftp_data.dBuffer, ftp_path))) {
                ftp_send_reply(250, NULL);
            }
            else {
                ftp_send_reply(550, NULL);
            }
            break;
        case E_FTP_CMD_NOOP:
            ftp_send_reply(200, NULL);
            break;
        case E_FTP_CMD_QUIT:
            ftp_send_reply(221, NULL);
            break;
        default:
            // command not implemented
            ftp_send_reply(502, NULL);
            break;
        }

        if (ftp_data.closechild) {
            ftp_return_to_previous_path(ftp_path, ftp_scratch_buffer);
        }
    }
    else if (result == E_FTP_RESULT_CONTINUE) {
        if (ftp_data.ctimeout++ > (servers_get_timeout() / FTP_CYCLE_TIME_MS)) {
            ftp_send_reply(221, NULL);
        }
    }
    else {
        ftp_close_cmd_data ();
    }
}

static void ftp_close_files (void) {
    if (ftp_data.e_open == E_FTP_FILE_OPEN) {
        f_close(&ftp_data.fp);
    }
    else if (ftp_data.e_open == E_FTP_DIR_OPEN) {
        f_closedir(&ftp_data.dp);
    }
    ftp_data.e_open = E_FTP_NOTHING_OPEN;
}

static void ftp_close_filesystem_on_error (void) {
    ftp_close_files();
    if (ftp_data.special_file) {
        updater_finnish ();
        ftp_data.special_file = false;
    }
}

static void ftp_close_cmd_data (void) {
    servers_close_socket(&ftp_data.c_sd);
    servers_close_socket(&ftp_data.d_sd);
    ftp_close_filesystem_on_error ();
}

static void stoupper (char *str) {
    while (str && *str != '\0') {
        *str = (char)unichar_toupper((int)(*str));
        str++;
    }
}

static ftp_cmd_index_t ftp_pop_command (char **str) {
    char _cmd[FTP_CMD_SIZE_MAX];
    ftp_pop_param (str, _cmd);
    stoupper (_cmd);
    for (ftp_cmd_index_t i = 0; i < E_FTP_NUM_FTP_CMDS; i++) {
        if (!strcmp (_cmd, ftp_cmd_table[i].cmd)) {
            // move one step further to skip the space
            (*str)++;
            return i;
        }
    }
    return E_FTP_CMD_NOT_SUPPORTED;
}

static void ftp_pop_param (char **str, char *param) {
    while (**str != ' ' && **str != '\r' && **str != '\n' && **str != '\0') {
        *param++ = **str;
        (*str)++;
    }
    *param = '\0';
}

static int ftp_print_eplf_item (char *dest, uint32_t destsize, FILINFO *fno) {

    char *type = (fno->fattrib & AM_DIR) ? "d" : "-";
    uint32_t tseconds;
    uint mindex = (((fno->fdate >> 5) & 0x0f) > 0) ? (((fno->fdate >> 5) & 0x0f) - 1) : 0;
    uint day = ((fno->fdate & 0x1f) > 0) ? (fno->fdate & 0x1f) : 1;
    uint fseconds = timeutils_seconds_since_2000(1980 + ((fno->fdate >> 9) & 0x7f),
                                                        (fno->fdate >> 5) & 0x0f,
                                                        fno->fdate & 0x1f,
                                                        (fno->ftime >> 11) & 0x1f,
                                                        (fno->ftime >> 5) & 0x3f,
                                                        2 * (fno->ftime & 0x1f));
    tseconds = pyb_rtc_get_seconds();
    if (FTP_UNIX_SECONDS_180_DAYS < tseconds - fseconds) {
        return snprintf(dest, destsize, "%srw-rw-r--   1 root  root %9u %s %2u %5u %s\r\n",
                        type, (_u32)fno->fsize, ftp_month[mindex].month, day,
                        1980 + ((fno->fdate >> 9) & 0x7f), fno->fname);
    }
    else {
        return snprintf(dest, destsize, "%srw-rw-r--   1 root  root %9u %s %2u %02u:%02u %s\r\n",
                        type, (_u32)fno->fsize, ftp_month[mindex].month, day,
                        (fno->ftime >> 11) & 0x1f, (fno->ftime >> 5) & 0x3f, fno->fname);
    }
}

static int ftp_print_eplf_drive (char *dest, uint32_t destsize, const char *name) {
    timeutils_struct_time_t tm;
    uint32_t tseconds;
    char *type = "d";

    timeutils_seconds_since_2000_to_struct_time((FTP_UNIX_TIME_20150101 - FTP_UNIX_TIME_20000101), &tm);

    tseconds = pyb_rtc_get_seconds();
    if (FTP_UNIX_SECONDS_180_DAYS < tseconds - (FTP_UNIX_TIME_20150101 - FTP_UNIX_TIME_20000101)) {
        return snprintf(dest, destsize, "%srw-rw-r--   1 root  root %9u %s %2u %5u %s\r\n",
                        type, 0, ftp_month[(tm.tm_mon - 1)].month, tm.tm_mday, tm.tm_year, name);
    }
    else {
        return snprintf(dest, destsize, "%srw-rw-r--   1 root  root %9u %s %2u %02u:%02u %s\r\n",
                        type, 0, ftp_month[(tm.tm_mon - 1)].month, tm.tm_mday, tm.tm_hour, tm.tm_min, name);
    }
}

static bool ftp_open_file (const char *path, int mode) {
    FRESULT res = f_open_helper(&ftp_data.fp, path, mode);
    if (res != FR_OK) {
        return false;
    }
    ftp_data.e_open = E_FTP_FILE_OPEN;
    return true;
}

static ftp_result_t ftp_read_file (char *filebuf, uint32_t desiredsize, uint32_t *actualsize) {
    ftp_result_t result = E_FTP_RESULT_CONTINUE;
    FRESULT res = f_read(&ftp_data.fp, filebuf, desiredsize, (UINT *)actualsize);
    if (res != FR_OK) {
        ftp_close_files();
        result = E_FTP_RESULT_FAILED;
        *actualsize = 0;
    }
    else if (*actualsize < desiredsize) {
        ftp_close_files();
        result = E_FTP_RESULT_OK;
    }
    return result;
}

static ftp_result_t ftp_write_file (char *filebuf, uint32_t size) {
    ftp_result_t result = E_FTP_RESULT_FAILED;
    uint32_t actualsize;
    FRESULT res = f_write(&ftp_data.fp, filebuf, size, (UINT *)&actualsize);
    if ((actualsize == size) && (FR_OK == res)) {
        result = E_FTP_RESULT_OK;
    }
    else {
        ftp_close_files();
    }
    return result;
}

static ftp_result_t ftp_open_dir_for_listing (const char *path) {
    // "hack" to detect the root directory
    if (path[0] == '/' && path[1] == '\0') {
        ftp_data.listroot = true;
    } else {
        FRESULT res;
        res = f_opendir_helper(&ftp_data.dp, path); /* Open the directory */
        if (res != FR_OK) {
            return E_FTP_RESULT_FAILED;
        }
        ftp_data.e_open = E_FTP_DIR_OPEN;
        ftp_data.listroot = false;
    }
    return E_FTP_RESULT_CONTINUE;
}

static ftp_result_t ftp_list_dir (char *list, uint32_t maxlistsize, uint32_t *listsize) {
    uint next = 0;
    uint listcount = 0;
    FRESULT res;
    ftp_result_t result = E_FTP_RESULT_CONTINUE;
    FILINFO fno;
#if _USE_LFN
    // read up to 2 directory items
    while (listcount < 2) {
#else
    // read up to 4 directory items
    while (listcount < 4) {
#endif
        if (ftp_data.listroot) {
            // root directory "hack"
            mp_vfs_mount_t *vfs = MP_STATE_VM(vfs_mount_table);
            int i = ftp_data.volcount;
            while (vfs != NULL && i != 0) {
                vfs = vfs->next;
                i -= 1;
            }
            if (vfs == NULL) {
                if (!next) {
                    // no volume found this time, we are done
                    ftp_data.volcount = 0;
                }
                break;
            } else {
                next += ftp_print_eplf_drive((list + next), (maxlistsize - next), vfs->str + 1);
            }
            ftp_data.volcount++;
        } else {
            // a "normal" directory
            res = f_readdir(&ftp_data.dp, &fno);                                                       /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) {
                result = E_FTP_RESULT_OK;
                break;                                                                                 /* Break on error or end of dp */
            }
            if (fno.fname[0] == '.' && fno.fname[1] == 0) continue;                                    /* Ignore . entry */
            if (fno.fname[0] == '.' && fno.fname[1] == '.' && fno.fname[2] == 0) continue;             /* Ignore .. entry */

            // add the entry to the list
            next += ftp_print_eplf_item((list + next), (maxlistsize - next), &fno);
        }
        listcount++;
    }
    if (result == E_FTP_RESULT_OK) {
        ftp_close_files();
    }
    *listsize = next;
    return result;
}

static void ftp_open_child (char *pwd, char *dir) {
    if (dir[0] == '/') {
        strcpy (pwd, dir);
    } else {
        if (strlen(pwd) > 1) {
            strcat (pwd, "/");
        }
        strcat (pwd, dir);
    }

    uint len = strlen(pwd);
    if ((len > 1) && (pwd[len - 1] == '/')) {
        pwd[len - 1] = '\0';
    }
}

static void ftp_close_child (char *pwd) {
    uint len = strlen(pwd);
    while (len && (pwd[len] != '/')) {
        len--;
    }
    if (len == 0) {
        strcpy (pwd, "/");
    } else {
        pwd[len] = '\0';
    }
}

static void ftp_return_to_previous_path (char *pwd, char *dir) {
    uint newlen = strlen(pwd) - strlen(dir);
    if ((newlen > 2) && (pwd[newlen - 1] == '/')) {
        pwd[newlen - 1] = '\0';
    }
    else {
        if (newlen == 0) {
            strcpy (pwd, "/");
        } else {
            pwd[newlen] = '\0';
        }
    }
}

