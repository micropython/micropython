//*****************************************************************************
// stdcmd.h
//
// Defines standard SD Card commands for CC3200 SDHOST module.
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

#ifndef __STDCMD_H__
#define __STDCMD_H__

//*****************************************************************************
// Standard MMC/SD Card Commands
//*****************************************************************************
#define CMD_GO_IDLE_STATE    SDHOST_CMD_0
#define CMD_SEND_IF_COND     SDHOST_CMD_8|SDHOST_RESP_LEN_48
#define CMD_SEND_CSD         SDHOST_CMD_9|SDHOST_RESP_LEN_136
#define CMD_APP_CMD          SDHOST_CMD_55|SDHOST_RESP_LEN_48
#define CMD_SD_SEND_OP_COND  SDHOST_CMD_41|SDHOST_RESP_LEN_48
#define CMD_SEND_OP_COND     SDHOST_CMD_1|SDHOST_RESP_LEN_48
#define CMD_READ_SINGLE_BLK  SDHOST_CMD_17|SDHOST_RD_CMD|SDHOST_RESP_LEN_48
#define CMD_READ_MULTI_BLK   SDHOST_CMD_18|SDHOST_RD_CMD|SDHOST_RESP_LEN_48|SDHOST_MULTI_BLK
#define CMD_WRITE_SINGLE_BLK SDHOST_CMD_24|SDHOST_WR_CMD|SDHOST_RESP_LEN_48
#define CMD_WRITE_MULTI_BLK  SDHOST_CMD_25|SDHOST_WR_CMD|SDHOST_RESP_LEN_48|SDHOST_MULTI_BLK
#define CMD_SELECT_CARD      SDHOST_CMD_7|SDHOST_RESP_LEN_48B
#define CMD_DESELECT_CARD    SDHOST_CMD_7
#define CMD_STOP_TRANS       SDHOST_CMD_12|SDHOST_RESP_LEN_48B
#define CMD_SET_BLK_CNT      SDHOST_CMD_23|SDHOST_RESP_LEN_48
#define CMD_ALL_SEND_CID     SDHOST_CMD_2|SDHOST_RESP_LEN_136
#define CMD_SEND_REL_ADDR    SDHOST_CMD_3|SDHOST_RESP_LEN_48

#endif //__STDCMD_H__
