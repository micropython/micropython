/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/**
 * This file implements a simple in-RAM key database for BLE host security
 * material and CCCDs.  As this database is only ble_store_ramd in RAM, its
 * contents are lost when the application terminates.
 */

#include <inttypes.h>
#include <string.h>

#include <py/runtime.h>
#include <py/builtin.h>
#include <extmod/vfs.h>
#include <py/stream.h>

#include "sysinit/sysinit.h"
#include "syscfg/syscfg.h"
#include "host/ble_hs.h"

#include "py/mpconfig.h"
#include "extmod/modbluetooth.h"


const mp_obj_type_t ble_store_type;

typedef struct _ble_store_obj_t {
    mp_obj_base_t base;

    struct ble_store_value_sec ble_store_our_secs[MYNEWT_VAL(BLE_STORE_MAX_BONDS)];
    uint8_t ble_store_num_our_secs;

    struct ble_store_value_sec ble_store_peer_secs[MYNEWT_VAL(BLE_STORE_MAX_BONDS)];
    uint8_t ble_store_num_peer_secs;

    struct ble_store_value_cccd ble_store_cccds[MYNEWT_VAL(BLE_STORE_MAX_CCCDS)];
    uint8_t ble_store_num_cccds;

} ble_store_obj_t;

STATIC ble_store_obj_t ble_store_obj = {
    .base = {&ble_store_type}
};

#ifdef MICROPY_PY_BLUETOOTH_BOND_FILE
#if MICROPY_PY_UJSON && MICROPY_PY_UBINASCII
static void ble_store_load();
static bool bluetooth_ble_store_dump_scheduled = false;
STATIC mp_obj_t bluetooth_ble_store_dump(mp_obj_t none_in);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bluetooth_ble_store_dump_obj, bluetooth_ble_store_dump);
#else
#error MICROPY_PY_BLUETOOTH_BOND_FILE requires MICROPY_PY_UJSON and MICROPY_PY_UBINASCII
#endif
#endif

/*****************************************************************************
 * $sec                                                                      *
 *****************************************************************************/

static void
ble_store_mpy_print_value_sec(const struct ble_store_value_sec *sec) {
    if (sec->ltk_present) {
        BLE_HS_LOG(DEBUG, "ediv=%u rand=0x%08x%08x authenticated=%d ltk=",
            sec->ediv, (uint32_t)sec->rand_num, (uint32_t)(sec->rand_num >> 32), sec->authenticated);
        ble_hs_log_flat_buf(sec->ltk, 16);
        BLE_HS_LOG(DEBUG, " ");
    }
    if (sec->irk_present) {
        BLE_HS_LOG(DEBUG, "irk=");
        ble_hs_log_flat_buf(sec->irk, 16);
        BLE_HS_LOG(DEBUG, " ");
    }
    if (sec->csrk_present) {
        BLE_HS_LOG(DEBUG, "csrk=");
        ble_hs_log_flat_buf(sec->csrk, 16);
        BLE_HS_LOG(DEBUG, " ");
    }

    BLE_HS_LOG(DEBUG, "\n");
}

static void
ble_store_mpy_print_key_sec(const struct ble_store_key_sec *key_sec) {
    if (ble_addr_cmp(&key_sec->peer_addr, BLE_ADDR_ANY)) {
        BLE_HS_LOG(DEBUG, "peer_addr_type=%d peer_addr=",
            key_sec->peer_addr.type);
        ble_hs_log_flat_buf(key_sec->peer_addr.val, 6);
        BLE_HS_LOG(DEBUG, " ");
    }
    if (key_sec->ediv_rand_present) {
        BLE_HS_LOG(DEBUG, "ediv=0x%02x rand=0x%08x%08x ",
            key_sec->ediv, (uint32_t)key_sec->rand_num, (uint32_t)(key_sec->rand_num >> 32));
    }
}

static int
ble_store_mpy_find_sec(const struct ble_store_key_sec *key_sec,
                       const struct ble_store_value_sec *value_secs,
                       int num_value_secs)
{
    const struct ble_store_value_sec *cur;
    int skipped;
    int i;

    skipped = 0;

    for (i = 0; i < num_value_secs; i++) {
        cur = value_secs + i;

        if (ble_addr_cmp(&key_sec->peer_addr, BLE_ADDR_ANY)) {
            if (ble_addr_cmp(&cur->peer_addr, &key_sec->peer_addr)) {
                continue;
            }
        }

        if (key_sec->ediv_rand_present) {
            if (cur->ediv != key_sec->ediv) {
                continue;
            }

            if (cur->rand_num != key_sec->rand_num) {
                continue;
            }
        }

        if (key_sec->idx > skipped) {
            skipped++;
            continue;
        }

        return i;
    }

    return -1;
}

static int
ble_store_mpy_read_our_sec(const struct ble_store_key_sec *key_sec,
    struct ble_store_value_sec *value_sec) {
    int idx;

    idx = ble_store_mpy_find_sec(key_sec, ble_store_obj.ble_store_our_secs,
        ble_store_obj.ble_store_num_our_secs);
    if (idx == -1) {
        return BLE_HS_ENOENT;
    }

    *value_sec = ble_store_obj.ble_store_our_secs[idx];
    return 0;
}

static int
ble_store_mpy_write_our_sec(const struct ble_store_value_sec *value_sec) {
    struct ble_store_key_sec key_sec;
    int idx;

    BLE_HS_LOG(DEBUG, "persisting our sec; ");
    ble_store_mpy_print_value_sec(value_sec);

    ble_store_key_from_value_sec(&key_sec, value_sec);
    idx = ble_store_mpy_find_sec(&key_sec, ble_store_obj.ble_store_our_secs,
        ble_store_obj.ble_store_num_our_secs);
    if (idx == -1) {
        if (ble_store_obj.ble_store_num_our_secs >= MYNEWT_VAL(BLE_STORE_MAX_BONDS)) {
            // Store is full, remove oldest entry
            for (int i = 1; i < ble_store_obj.ble_store_num_our_secs; i++) {
                ble_store_obj.ble_store_our_secs[i - 1] = ble_store_obj.ble_store_our_secs[i];
            }
            idx = ble_store_obj.ble_store_num_our_secs - 1;
        } else {
            idx = ble_store_obj.ble_store_num_our_secs;
            ble_store_obj.ble_store_num_our_secs++;
        }
    }

    ble_store_obj.ble_store_our_secs[idx] = *value_sec;
    return 0;
}

static int
ble_store_mpy_delete_obj(void *values, int value_size, int idx,
    uint8_t *num_values) {
    uint8_t *dst;
    uint8_t *src;
    int move_count;

    (*num_values)--;
    if (idx < *num_values) {
        dst = values;
        dst += idx * value_size;
        src = dst + value_size;

        move_count = *num_values - idx;
        memmove(dst, src, move_count);
    }

    return 0;
}

static int
ble_store_mpy_delete_sec(const struct ble_store_key_sec *key_sec,
                         struct ble_store_value_sec *value_secs,
                         uint8_t *num_value_secs)
{
    int idx;
    int rc;

    idx = ble_store_mpy_find_sec(key_sec, value_secs, *num_value_secs);
    if (idx == -1) {
        return BLE_HS_ENOENT;
    }

    rc = ble_store_mpy_delete_obj(value_secs, sizeof *value_secs, idx,
                                  num_value_secs);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

static int
ble_store_mpy_delete_our_sec(const struct ble_store_key_sec *key_sec) {
    int rc;

    rc = ble_store_mpy_delete_sec(key_sec, ble_store_obj.ble_store_our_secs,
        &ble_store_obj.ble_store_num_our_secs);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

static int
ble_store_mpy_delete_peer_sec(const struct ble_store_key_sec *key_sec) {
    int rc;

    rc = ble_store_mpy_delete_sec(key_sec, ble_store_obj.ble_store_peer_secs,
        &ble_store_obj.ble_store_num_peer_secs);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

static int
ble_store_mpy_read_peer_sec(const struct ble_store_key_sec *key_sec,
    struct ble_store_value_sec *value_sec) {
    int idx;

    idx = ble_store_mpy_find_sec(key_sec, ble_store_obj.ble_store_peer_secs,
        ble_store_obj.ble_store_num_peer_secs);
    if (idx == -1) {
        return BLE_HS_ENOENT;
    }

    *value_sec = ble_store_obj.ble_store_peer_secs[idx];
    return 0;
}

static int
ble_store_mpy_write_peer_sec(const struct ble_store_value_sec *value_sec) {
    struct ble_store_key_sec key_sec;
    int idx;

    BLE_HS_LOG(DEBUG, "persisting peer sec; ");
    ble_store_mpy_print_value_sec(value_sec);

    ble_store_key_from_value_sec(&key_sec, value_sec);
    idx = ble_store_mpy_find_sec(&key_sec, ble_store_obj.ble_store_peer_secs,
        ble_store_obj.ble_store_num_peer_secs);
    if (idx == -1) {
        if (ble_store_obj.ble_store_num_peer_secs >= MYNEWT_VAL(BLE_STORE_MAX_BONDS)) {
            // Store is full, remove oldest entry
            for (int i = 1; i < ble_store_obj.ble_store_num_peer_secs; i++) {
                ble_store_obj.ble_store_peer_secs[i - 1] = ble_store_obj.ble_store_peer_secs[i];
            }
            idx = ble_store_obj.ble_store_num_peer_secs - 1;
        } else {
            idx = ble_store_obj.ble_store_num_peer_secs;
            ble_store_obj.ble_store_num_peer_secs++;
        }
    }

    ble_store_obj.ble_store_peer_secs[idx] = *value_sec;
    return 0;
}

/*****************************************************************************
 * $cccd                                                                     *
 *****************************************************************************/

static int
ble_store_mpy_find_cccd(const struct ble_store_key_cccd *key) {
    struct ble_store_value_cccd *cccd;
    int skipped;
    int i;

    skipped = 0;
    for (i = 0; i < ble_store_obj.ble_store_num_cccds; i++) {
        cccd = ble_store_obj.ble_store_cccds + i;

        if (ble_addr_cmp(&key->peer_addr, BLE_ADDR_ANY)) {
            if (ble_addr_cmp(&cccd->peer_addr, &key->peer_addr)) {
                continue;
            }
        }

        if (key->chr_val_handle != 0) {
            if (cccd->chr_val_handle != key->chr_val_handle) {
                continue;
            }
        }

        if (key->idx > skipped) {
            skipped++;
            continue;
        }

        return i;
    }

    return -1;
}

static int
ble_store_mpy_delete_cccd(const struct ble_store_key_cccd *key_cccd) {
    int idx;
    int rc;

    idx = ble_store_mpy_find_cccd(key_cccd);
    if (idx == -1) {
        return BLE_HS_ENOENT;
    }

    rc = ble_store_mpy_delete_obj(ble_store_obj.ble_store_cccds,
        sizeof *ble_store_obj.ble_store_cccds,
        idx,
        &ble_store_obj.ble_store_num_cccds);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

static int
ble_store_mpy_read_cccd(const struct ble_store_key_cccd *key_cccd,
    struct ble_store_value_cccd *value_cccd) {
    int idx;

    idx = ble_store_mpy_find_cccd(key_cccd);
    if (idx == -1) {
        return BLE_HS_ENOENT;
    }

    *value_cccd = ble_store_obj.ble_store_cccds[idx];
    return 0;
}

static int
ble_store_mpy_write_cccd(const struct ble_store_value_cccd *value_cccd) {
    struct ble_store_key_cccd key_cccd;
    int idx;

    ble_store_key_from_value_cccd(&key_cccd, value_cccd);
    idx = ble_store_mpy_find_cccd(&key_cccd);
    if (idx == -1) {
        if (ble_store_obj.ble_store_num_cccds >= MYNEWT_VAL(BLE_STORE_MAX_BONDS)) {
            // Store is full, remove oldest entry
            for (int i = 1; i < ble_store_obj.ble_store_num_cccds; i++) {
                ble_store_obj.ble_store_cccds[i - 1] = ble_store_obj.ble_store_cccds[i];
            }
            idx = ble_store_obj.ble_store_num_cccds - 1;
        } else {
            idx = ble_store_obj.ble_store_num_cccds;
            ble_store_obj.ble_store_num_cccds++;
        }
    }

    ble_store_obj.ble_store_cccds[idx] = *value_cccd;
    return 0;
}

/*****************************************************************************
 * $api                                                                      *
 *****************************************************************************/

/**
 * Searches the database for an object matching the specified criteria.
 *
 * @return                      0 if a key was found; else BLE_HS_ENOENT.
 */
int
ble_store_mpy_read(int obj_type, const union ble_store_key *key,
    union ble_store_value *value) {
    int rc;

    switch (obj_type) {
        case BLE_STORE_OBJ_TYPE_PEER_SEC:
            /* An encryption procedure (bonding) is being attempted.  The nimble
             * stack is asking us to look in our key database for a long-term key
             * corresponding to the specified ediv and random number.
             *
             * Perform a key lookup and populate the context object with the
             * result.  The nimble stack will use this key if this function returns
             * success.
             */
            BLE_HS_LOG(DEBUG, "looking up peer sec; ");
            ble_store_mpy_print_key_sec(&key->sec);
            BLE_HS_LOG(DEBUG, "\n");
            rc = ble_store_mpy_read_peer_sec(&key->sec, &value->sec);
            return rc;

        case BLE_STORE_OBJ_TYPE_OUR_SEC:
            BLE_HS_LOG(DEBUG, "looking up our sec; ");
            ble_store_mpy_print_key_sec(&key->sec);
            BLE_HS_LOG(DEBUG, "\n");
            rc = ble_store_mpy_read_our_sec(&key->sec, &value->sec);
            return rc;

        case BLE_STORE_OBJ_TYPE_CCCD:
            rc = ble_store_mpy_read_cccd(&key->cccd, &value->cccd);
            return rc;

        default:
            return BLE_HS_ENOTSUP;
    }
}

/**
 * Adds the specified object to the database.
 *
 * @return                      0 on success; BLE_HS_ESTORE_CAP if the database
 *                                  is full.
 */
int
ble_store_mpy_write(int obj_type, const union ble_store_value *val) {
    int rc;

    switch (obj_type) {
        case BLE_STORE_OBJ_TYPE_PEER_SEC:
            rc = ble_store_mpy_write_peer_sec(&val->sec);
            break;

        case BLE_STORE_OBJ_TYPE_OUR_SEC:
            rc = ble_store_mpy_write_our_sec(&val->sec);
            break;

        case BLE_STORE_OBJ_TYPE_CCCD:
            rc = ble_store_mpy_write_cccd(&val->cccd);
            break;

        default:
            rc = BLE_HS_ENOTSUP;
    }

    #ifdef MICROPY_PY_BLUETOOTH_BOND_FILE
    if (rc == 0) {
        if (!bluetooth_ble_store_dump_scheduled) {
            bluetooth_ble_store_dump_scheduled = true;
            mp_sched_schedule(MP_OBJ_FROM_PTR(&bluetooth_ble_store_dump_obj), mp_const_none);
        }
    }
    #endif
    return rc;
}

int
ble_store_mpy_delete(int obj_type, const union ble_store_key *key) {
    int rc;

    switch (obj_type) {
        case BLE_STORE_OBJ_TYPE_PEER_SEC:
            rc = ble_store_mpy_delete_peer_sec(&key->sec);
            break;

        case BLE_STORE_OBJ_TYPE_OUR_SEC:
            rc = ble_store_mpy_delete_our_sec(&key->sec);
            break;

        case BLE_STORE_OBJ_TYPE_CCCD:
            rc = ble_store_mpy_delete_cccd(&key->cccd);
            break;

        default:
            rc = BLE_HS_ENOTSUP;
    }

    #ifdef MICROPY_PY_BLUETOOTH_BOND_FILE
    if (rc == 0) {
        if (!bluetooth_ble_store_dump_scheduled) {
            bluetooth_ble_store_dump_scheduled = true;
            mp_sched_schedule(MP_OBJ_FROM_PTR(&bluetooth_ble_store_dump_obj), mp_const_none);
        }
    }
    #endif
    return rc;

}

void
ble_store_ram_init(void) {
    /* Ensure this function only gets called by sysinit. */
    SYSINIT_ASSERT_ACTIVE();

    ble_hs_cfg.store_read_cb = ble_store_mpy_read;
    ble_hs_cfg.store_write_cb = ble_store_mpy_write;
    ble_hs_cfg.store_delete_cb = ble_store_mpy_delete;

    /* Re-initialize BSS values in case of unit tests. */
    ble_store_obj.ble_store_num_our_secs = 0;
    ble_store_obj.ble_store_num_peer_secs = 0;
    ble_store_obj.ble_store_num_cccds = 0;

    #ifdef MICROPY_PY_BLUETOOTH_BOND_FILE
    ble_store_load();
    #endif
}


/*****************************************************************************
 * $micropython interface                                                    *
 *****************************************************************************/


#ifdef MICROPY_PY_BLUETOOTH_BOND_FILE
#if MICROPY_PY_UJSON && MICROPY_PY_UBINASCII

extern const mp_obj_module_t mp_module_ujson;
extern const mp_obj_module_t mp_module_ubinascii;


static mp_obj_t parse_json_hexkey(mp_obj_t hexstring) {
    mp_obj_t unhexlify_fn[3];
    mp_load_method(MP_OBJ_FROM_PTR(&mp_module_ubinascii), MP_QSTR_unhexlify, unhexlify_fn);
    unhexlify_fn[2] = hexstring;
    return mp_call_method_n_kw(1, 0, unhexlify_fn);
}

static void parse_json_value_sec(mp_obj_t value_sec_dict, struct ble_store_value_sec *loaded) {
    mp_map_t *map = mp_obj_dict_get_map(value_sec_dict);

    mp_obj_t parsed_peer_addr = parse_json_hexkey(mp_obj_dict_get(value_sec_dict, MP_OBJ_NEW_QSTR(MP_QSTR_peer_addr)));
    const char *peer_addr = mp_obj_str_get_str(parsed_peer_addr);
    strncpy((char *)loaded->peer_addr.val, peer_addr, sizeof(loaded->peer_addr.val));
    loaded->peer_addr.type = mp_obj_get_int(mp_obj_dict_get(value_sec_dict, MP_OBJ_NEW_QSTR(MP_QSTR_peer_addr_type)));

    if (mp_map_lookup(map, MP_OBJ_NEW_QSTR(MP_QSTR_ediv), MP_MAP_LOOKUP) != NULL) {
        loaded->key_size = mp_obj_get_int(mp_obj_dict_get(value_sec_dict, MP_OBJ_NEW_QSTR(MP_QSTR_key_size)));
        loaded->ediv = mp_obj_get_int(mp_obj_dict_get(value_sec_dict, MP_OBJ_NEW_QSTR(MP_QSTR_ediv)));
        // loaded->rand_num = mp_obj_to_64bit_int(mp_obj_dict_get(value_sec_dict, MP_OBJ_NEW_QSTR(MP_QSTR_rand)), false);
        loaded->rand_num = mp_obj_get_int(mp_obj_dict_get(value_sec_dict, MP_OBJ_NEW_QSTR(MP_QSTR_rand)));
        loaded->authenticated = mp_obj_get_int(mp_obj_dict_get(value_sec_dict, MP_OBJ_NEW_QSTR(MP_QSTR_authenticated)));
        loaded->sc = mp_obj_get_int(mp_obj_dict_get(value_sec_dict, MP_OBJ_NEW_QSTR(MP_QSTR_sc)));

        mp_obj_t parsed_ltk = parse_json_hexkey(mp_obj_dict_get(value_sec_dict, MP_OBJ_NEW_QSTR(MP_QSTR_ltk)));
        const char *ltk = mp_obj_str_get_str(parsed_ltk);
        strncpy((char *)loaded->ltk, ltk, sizeof(loaded->ltk));
        loaded->ltk_present = 1;
    }

    if (mp_map_lookup(map, MP_OBJ_NEW_QSTR(MP_QSTR_irk), MP_MAP_LOOKUP) != NULL) {
        mp_obj_t parsed_irk = parse_json_hexkey(mp_obj_dict_get(value_sec_dict, MP_OBJ_NEW_QSTR(MP_QSTR_irk)));
        const char *irk = mp_obj_str_get_str(parsed_irk);
        strncpy((char *)loaded->irk, irk, sizeof(loaded->irk));
        loaded->irk_present = 1;
    }

    if (mp_map_lookup(map, MP_OBJ_NEW_QSTR(MP_QSTR_csrk), MP_MAP_LOOKUP) != NULL) {
        mp_obj_t parsed_csrk = parse_json_hexkey(mp_obj_dict_get(value_sec_dict, MP_OBJ_NEW_QSTR(MP_QSTR_csrk)));
        const char *csrk = mp_obj_str_get_str(parsed_csrk);
        strncpy((char *)loaded->csrk, csrk, sizeof(loaded->csrk));
        loaded->csrk_present = 1;
    }
}

static void parse_json_value_cccd(mp_obj_t value_cccd_dict, struct ble_store_value_cccd *loaded) {

    mp_obj_t parsed_peer_addr = parse_json_hexkey(mp_obj_dict_get(value_cccd_dict, MP_OBJ_NEW_QSTR(MP_QSTR_peer_addr)));
    const char *peer_addr = mp_obj_str_get_str(parsed_peer_addr);
    strncpy((char *)loaded->peer_addr.val, peer_addr, sizeof(loaded->peer_addr.val));
    loaded->peer_addr.type = mp_obj_get_int(mp_obj_dict_get(value_cccd_dict, MP_OBJ_NEW_QSTR(MP_QSTR_peer_addr_type)));

    loaded->chr_val_handle = mp_obj_get_int(mp_obj_dict_get(value_cccd_dict, MP_OBJ_NEW_QSTR(MP_QSTR_chr_val_handle)));
    loaded->flags = mp_obj_get_int(mp_obj_dict_get(value_cccd_dict, MP_OBJ_NEW_QSTR(MP_QSTR_flags)));
    loaded->value_changed = mp_obj_get_int(mp_obj_dict_get(value_cccd_dict, MP_OBJ_NEW_QSTR(MP_QSTR_value_changed)));
}

static void ble_store_load() {
    if (mp_import_stat(MICROPY_PY_BLUETOOTH_BOND_FILE) == MP_IMPORT_STAT_FILE) {

        mp_obj_t mp_path = mp_obj_new_str(MICROPY_PY_BLUETOOTH_BOND_FILE, strlen(MICROPY_PY_BLUETOOTH_BOND_FILE));
        mp_obj_t file = mp_builtin_open(1, &mp_path, (mp_map_t *)&mp_const_empty_map);

        mp_obj_t load_fn[3];
        mp_load_method(MP_OBJ_FROM_PTR(&mp_module_ujson), MP_QSTR_load, load_fn);
        load_fn[2] = file;
        mp_obj_t loaded = mp_call_method_n_kw(1, 0, load_fn);

        mp_obj_iter_buf_t iter_buf;
        mp_obj_t iterable;
        mp_obj_t entry_obj;

        mp_obj_t our_secs_list = mp_obj_dict_get(loaded, MP_OBJ_NEW_QSTR(MP_QSTR_our_secs));
        iterable = mp_getiter(our_secs_list, &iter_buf);
        uint8_t idx = 0;
        while ((entry_obj = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            parse_json_value_sec(entry_obj, &ble_store_obj.ble_store_our_secs[idx++]);
            if (idx == MYNEWT_VAL(BLE_STORE_MAX_BONDS)) {
                break;
            }
        }
        ble_store_obj.ble_store_num_our_secs = idx;

        mp_obj_t our_peer_secs = mp_obj_dict_get(loaded, MP_OBJ_NEW_QSTR(MP_QSTR_peer_secs));
        iterable = mp_getiter(our_peer_secs, &iter_buf);
        idx = 0;
        while ((entry_obj = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            parse_json_value_sec(entry_obj, &ble_store_obj.ble_store_peer_secs[idx++]);
            if (idx == MYNEWT_VAL(BLE_STORE_MAX_BONDS)) {
                break;
            }
        }
        ble_store_obj.ble_store_num_peer_secs = idx;

        mp_obj_t cccds_list = mp_obj_dict_get(loaded, MP_OBJ_NEW_QSTR(MP_QSTR_cccds));
        iterable = mp_getiter(cccds_list, &iter_buf);
        idx = 0;
        while ((entry_obj = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            parse_json_value_cccd(entry_obj, &ble_store_obj.ble_store_cccds[idx++]);
            if (idx == MYNEWT_VAL(BLE_STORE_MAX_CCCDS)) {
                break;
            }
        }
        ble_store_obj.ble_store_num_cccds = idx;
    }
}

STATIC mp_obj_t bluetooth_ble_store_dump(mp_obj_t none_in) {
    (void)none_in;
    bluetooth_ble_store_dump_scheduled = false;
    // This is always executing in schedule context.
    MICROPY_PY_BLUETOOTH_ENTER

    mp_obj_t args[] = {
        mp_obj_new_str(MICROPY_PY_BLUETOOTH_BOND_FILE, strlen(MICROPY_PY_BLUETOOTH_BOND_FILE)),
        MP_ROM_QSTR(MP_QSTR_w)
    };
    mp_obj_t file = mp_builtin_open(2, args, (mp_map_t *)&mp_const_empty_map);

    mp_obj_t dump_fn[4];
    mp_load_method(MP_OBJ_FROM_PTR(&mp_module_ujson), MP_QSTR_dump, dump_fn);
    dump_fn[2] = MP_OBJ_FROM_PTR(&ble_store_obj);
    dump_fn[3] = file;
    mp_call_method_n_kw(2, 0, dump_fn);

    mp_stream_close(file);
    MICROPY_PY_BLUETOOTH_EXIT

    return mp_const_none;
}

STATIC void _ble_array_print(const mp_print_t *print, const uint8_t *data, size_t len) {
    mp_printf(print, "\"");
    for (size_t i = 0; i < len; i++) {
        mp_printf(print, "%02x", data[i]);
    }
    mp_printf(print, "\"");
}

STATIC void ble_value_sec_print(const mp_print_t *print, struct ble_store_value_sec *sec, mp_print_kind_t kind) {
    bool preceding = false;

    if (kind != PRINT_JSON) {
        mp_printf(print, "BLE_SEC(");
    } else {
        mp_printf(print, "{");
    }

    mp_printf(print, "\"peer_addr\":");
    _ble_array_print(print, sec->peer_addr.val, 6);
    mp_printf(print, ", \"peer_addr_type\":%u", sec->peer_addr.type);

    if (sec->ltk_present) {
        mp_printf(print, ", \"key_size\":%u, \"ediv\":%u, \"rand\":%lu, \"authenticated\":%d, \"sc\":%d, \"ltk\":",
            sec->key_size, sec->ediv, sec->rand_num, sec->authenticated, sec->sc);
        _ble_array_print(print, sec->ltk, 16);
        preceding = true;
    }
    if (sec->irk_present) {
        if (preceding) {
            mp_printf(print, ", ");
            preceding = false;
        }
        mp_printf(print, "\"irk\":");
        _ble_array_print(print, sec->irk, 16);
        preceding = true;
    }
    if (sec->csrk_present) {
        if (preceding) {
            mp_printf(print, ", ");
            preceding = false;
        }
        mp_printf(print, "\"csrk\":");
        _ble_array_print(print, sec->csrk, 16);
    }

    if (kind != PRINT_JSON) {
        mp_printf(print, ")\n");
    } else {
        mp_printf(print, "}");
    }
}

STATIC void ble_value_cccd_print(const mp_print_t *print, struct ble_store_value_cccd *cccd_val, mp_print_kind_t kind) {
    if (kind != PRINT_JSON) {
        mp_printf(print, "BLE_CCCD(");
    } else {
        mp_printf(print, "{");
    }

    mp_printf(print, "\"peer_addr\":");
    _ble_array_print(print, cccd_val->peer_addr.val, 6);


    mp_printf(print, ", \"peer_addr_type\":%u, \"chr_val_handle\":%u, \"flags\":%u, \"value_changed\":%u",
        cccd_val->peer_addr.type, cccd_val->chr_val_handle, cccd_val->flags, cccd_val->value_changed);

    if (kind != PRINT_JSON) {
        mp_printf(print, ")\n");
    } else {
        mp_printf(print, "}");
    }
}

STATIC void ble_store_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    ble_store_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (kind != PRINT_JSON) {
        mp_printf(print, "BLE_SEC(");
    } else {
        mp_printf(print, "{");
    }

    mp_printf(print, "\"our_secs\":[");
    for (uint8_t i = 0; i < self->ble_store_num_our_secs; i++) {
        ble_value_sec_print(print, &self->ble_store_our_secs[i], kind);
        if (i < self->ble_store_num_our_secs - 1) {
            mp_printf(print, ", ");
        }
    }


    mp_printf(print, "], \"peer_secs\":[");
    for (uint8_t i = 0; i < self->ble_store_num_peer_secs; i++) {
        ble_value_sec_print(print, &self->ble_store_peer_secs[i], kind);
        if (i < self->ble_store_num_peer_secs - 1) {
            mp_printf(print, ", ");
        }
    }

    mp_printf(print, "], \"cccds\":[");
    for (uint8_t i = 0; i < self->ble_store_num_cccds; i++) {
        ble_value_cccd_print(print, &self->ble_store_cccds[i], kind);
        if (i < self->ble_store_num_cccds - 1) {
            mp_printf(print, ", ");
        }
    }
    mp_printf(print, "]");

    if (kind != PRINT_JSON) {
        mp_printf(print, ")\n");
    } else {
        mp_printf(print, "}");
    }
}

const mp_obj_type_t ble_store_type = {
    { &mp_type_type },
    .name = MP_QSTR_BLE_SEC,
    .print = ble_store_print,
};

#endif
#endif
