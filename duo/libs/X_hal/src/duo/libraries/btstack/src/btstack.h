/*
 * Copyright (C) 2014 BlueKitchen GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. Any redistribution, use, or modification is done solely for
 *    personal benefit and not for any commercial purpose or for
 *    monetary gain.
 *
 * THIS SOFTWARE IS PROVIDED BY BLUEKITCHEN GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MATTHIAS
 * RINGWALD OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Please inquire about commercial licensing options at 
 * contact@bluekitchen-gmbh.com
 *
 */

/*
 *  btstack.h
 *  Convenience header to include all public APIs
 */


#ifndef __BTSTACK_H
#define __BTSTACK_H

#include "btstack_config.h"

#include "bluetooth.h"
#include "btstack.h"
#include "btstack_control.h"
#include "btstack_debug.h"
#include "btstack_event.h"
#include "btstack_defines.h"
#include "btstack_linked_list.h"
#include "btstack_memory.h"
#include "btstack_memory_pool.h"
#include "btstack_run_loop.h"
#include "btstack_util.h"
#include "gap.h"
#include "hci.h"
#include "hci_cmd.h"
#include "hci_dump.h"
#include "hci_transport.h"
#include "l2cap.h"
#include "l2cap_signaling.h"

#ifdef ENABLE_BLE
#include "ble/ad_parser.h"
#include "ble/ancs_client.h"
#include "ble/att_db.h"
#include "ble/att_db_util.h"
#include "ble/att_dispatch.h"
#include "ble/att_server.h"
#include "ble/gatt_client.h"
#include "ble/le_device_db.h"
#include "ble/sm.h"
#endif

// #ifdef HAVE_CLASSIC
#include "classic/bnep.h"
#include "classic/hfp.h"
#include "classic/hfp_ag.h"
#include "classic/hfp_hf.h"
#include "classic/hsp_ag.h"
#include "classic/hsp_hs.h"
#include "classic/pan.h"
#include "classic/btstack_link_key_db.h"
#include "classic/rfcomm.h"
#include "classic/sdp_server.h"
#include "classic/sdp_client.h"
#include "classic/sdp_query_rfcomm.h"
#include "classic/sdp_util.h"
// #endif

#endif  // __BTSTACK_H
 