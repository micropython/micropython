/*
 * Copyright (C) 2017 BlueKitchen GmbH
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
 *
 * THIS SOFTWARE IS PROVIDED BY MATTHIAS RINGWALD AND CONTRIBUTORS
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
 */

/*
 *  btstack_tlv_mpy.h
 *
 *  Implementation for BTstack's Tag Value Length Persistent Storage implementations
 *  using in-memory storage (RAM & malloc) and append-only log files on disc
 */

#ifndef btstack_tlv_mpy_H
#define btstack_tlv_mpy_H

#include <stdint.h>
#include <stdio.h>
#include <extmod/vfs.h>
#include "btstack_tlv.h"
#include "btstack_linked_list.h"

#if defined __cplusplus
extern "C" {
#endif

typedef struct {
	btstack_linked_list_t entry_list;
	const char * db_path;
	mp_obj_t file;
} btstack_tlv_mpy_t;

/**
 * Init Tag Length Value Store
 * @param context btstack_tlv_mpy_t 
 * @param db_path on disc
 */
const btstack_tlv_t * btstack_tlv_mpy_init_instance(btstack_tlv_mpy_t * context, const char * db_path);

#if defined __cplusplus
}
#endif
#endif // btstack_tlv_mpy_H
