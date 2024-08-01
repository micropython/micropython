/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023-2024 Arduino SA
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
 *
 * OpenAMP's Python module.
 */

#if MICROPY_PY_OPENAMP

#include <stdarg.h>
#include "py/obj.h"
#include "py/nlr.h"
#include "py/runtime.h"
#include "py/mpprint.h"

#include "metal/sys.h"
#include "metal/alloc.h"
#include "metal/errno.h"
#include "metal/io.h"
#include "metal/device.h"
#include "metal/utilities.h"

#include "openamp/open_amp.h"
#include "openamp/remoteproc.h"
#include "openamp/remoteproc_loader.h"
#include "modopenamp.h"

#if !MICROPY_ENABLE_FINALISER
#error "MICROPY_PY_OPENAMP requires MICROPY_ENABLE_FINALISER"
#endif

#if MICROPY_PY_OPENAMP_RSC_TABLE_ENABLE
#define VIRTIO_DEV_ID           0xFF
#if MICROPY_PY_OPENAMP_HOST
#define VIRTIO_DEV_ROLE         RPMSG_HOST
#else
#define VIRTIO_DEV_ROLE         RPMSG_REMOTE
#endif // MICROPY_PY_OPENAMP_HOST
#define VIRTIO_DEV_FEATURES     (1 << VIRTIO_RPMSG_F_NS)

#define VRING0_ID               0   // VRING0 ID (host to remote) fixed to 0 for linux compatibility
#define VRING1_ID               1   // VRING1 ID (remote to host) fixed to 1 for linux compatibility
#if MICROPY_PY_OPENAMP_HOST
#define VRING_NOTIFY_ID         VRING0_ID
#else
#define VRING_NOTIFY_ID         VRING1_ID
#endif // MICROPY_PY_OPENAMP_HOST

#define VRING_COUNT             2
#define VRING_ALIGNMENT         32
// Note the number of buffers must be a power of 2
#define VRING_NUM_BUFFS         64

// The following config should be enough for about 128 descriptors.
// See lib/include/openamp/virtio_ring.h for the layout of vrings
// and vring_size() to calculate the vring size.
#define VRING_RX_ADDR           (METAL_SHM_ADDR)
#define VRING_TX_ADDR           (METAL_SHM_ADDR + 0x1000)
#define VRING_BUFF_ADDR         (METAL_SHM_ADDR + 0x2000)
#define VRING_BUFF_SIZE         (METAL_SHM_SIZE - 0x2000)

#if MICROPY_PY_OPENAMP_HOST
static const char openamp_trace_buf[128];
#define MICROPY_PY_OPENAMP_TRACE_BUF       ((uint32_t)openamp_trace_buf)
#define MICROPY_PY_OPENAMP_TRACE_BUF_LEN   sizeof(MICROPY_PY_OPENAMP_TRACE_BUF)
#endif // MICROPY_PY_OPENAMP_HOST

#endif // MICROPY_PY_OPENAMP_RSC_TABLE_ENABLE

#if MICROPY_PY_OPENAMP_HOST && MICROPY_PY_OPENAMP_REMOTEPROC
extern mp_obj_type_t openamp_remoteproc_type;
#endif

static struct metal_device shm_device = {
    .name = METAL_SHM_NAME,
    // The number of IO regions is fixed and must match the number and
    // layout of the remote processor's IO regions. The first region is
    // used for the vring shared memory, and the second one is used for
    // the shared resource table.
    .num_regions = METAL_MAX_DEVICE_REGIONS,
    .regions = { { 0 } },
    .node = { NULL },
    .irq_num = 0,
    .irq_info = NULL
};
static metal_phys_addr_t shm_physmap[] = { 0 };

// ###################### Virtio device class ######################
typedef struct _virtio_dev_obj_t {
    mp_obj_base_t base;
    struct rpmsg_virtio_device rvdev;
    struct rpmsg_virtio_shm_pool shm_pool;
    mp_obj_t ns_callback;
} virtio_dev_obj_t;

static mp_obj_t virtio_dev_deinit(mp_obj_t self_in) {
    virtio_dev_obj_t *virtio_device = MP_OBJ_TO_PTR(self_in);
    rpmsg_deinit_vdev(&virtio_device->rvdev);
    metal_finish();
    MP_STATE_PORT(virtio_device) = NULL;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(virtio_dev_deinit_obj, virtio_dev_deinit);

static const mp_rom_map_elem_t virtio_dev_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_VirtIODev) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&virtio_dev_deinit_obj) },
};
static MP_DEFINE_CONST_DICT(virtio_dev_locals_dict, virtio_dev_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    virtio_dev_type,
    MP_QSTR_VirtIODev,
    MP_TYPE_FLAG_NONE,
    locals_dict, &virtio_dev_locals_dict
    );

// ###################### RPMsg Endpoint class ######################
typedef struct _endpoint_obj_t {
    mp_obj_base_t base;
    mp_obj_t name;
    mp_obj_t callback;
    struct rpmsg_endpoint ep;
} endpoint_obj_t;

static const mp_obj_type_t endpoint_type;

static int endpoint_recv_callback(struct rpmsg_endpoint *ept, void *data, size_t len, uint32_t src, void *priv) {
    metal_log(METAL_LOG_DEBUG, "endpoint_recv_callback() message received src: %lu msg len: %d\n", src, len);
    endpoint_obj_t *self = metal_container_of(ept, endpoint_obj_t, ep);
    if (self->callback != mp_const_none) {
        mp_call_function_2(self->callback, mp_obj_new_int(src), mp_obj_new_bytearray_by_ref(len, data));
    }
    return 0;
}

static mp_obj_t endpoint_send(uint n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_src, ARG_dest, ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_src, MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = -1 } },
        { MP_QSTR_dest, MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = -1 } },
        { MP_QSTR_timeout, MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = -1 } },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 2, pos_args + 2, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    endpoint_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    if (is_rpmsg_ept_ready(&self->ep) == false) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Endpoint not ready"));
    }

    uint32_t src = self->ep.addr;
    if (args[ARG_src].u_int != -1) {
        src = args[ARG_src].u_int;
    }

    uint32_t dest = self->ep.dest_addr;
    if (args[ARG_dest].u_int != -1) {
        dest = args[ARG_dest].u_int;
    }

    mp_buffer_info_t rbuf;
    mp_get_buffer_raise(pos_args[1], &rbuf, MP_BUFFER_READ);
    metal_log(METAL_LOG_DEBUG, "endpoint_send() msg len: %d\n", rbuf.len);

    int bytes = 0;
    mp_int_t timeout = args[ARG_timeout].u_int;
    for (mp_uint_t start = mp_hal_ticks_ms(); ;) {
        bytes = rpmsg_send_offchannel_raw(&self->ep, src, dest, rbuf.buf, rbuf.len, false);
        if (bytes > 0 || timeout == 0) {
            mp_event_handle_nowait();
            break;
        }
        if (timeout > 0 && (mp_hal_ticks_ms() - start > timeout)) {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("timeout waiting for a free buffer"));
        }
        mp_event_wait_ms(1);
    }
    return mp_obj_new_int(bytes);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(endpoint_send_obj, 2, endpoint_send);

static mp_obj_t endpoint_is_ready(mp_obj_t self_in) {
    endpoint_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return is_rpmsg_ept_ready(&self->ep) ? mp_const_true : mp_const_false;
}
static MP_DEFINE_CONST_FUN_OBJ_1(endpoint_is_ready_obj, endpoint_is_ready);

static mp_obj_t endpoint_deinit(mp_obj_t self_in) {
    endpoint_obj_t *self = MP_OBJ_TO_PTR(self_in);
    rpmsg_destroy_ept(&self->ep);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(endpoint_deinit_obj, endpoint_deinit);

static mp_obj_t endpoint_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_name, ARG_callback, ARG_src, ARG_dest };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_name, MP_ARG_OBJ | MP_ARG_REQUIRED,  {.u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_callback, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_src, MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = RPMSG_ADDR_ANY } },
        { MP_QSTR_dest, MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = RPMSG_ADDR_ANY } },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    endpoint_obj_t *self = mp_obj_malloc_with_finaliser(endpoint_obj_t, &endpoint_type);
    self->name = args[ARG_name].u_obj;
    self->callback = args[ARG_callback].u_obj;

    if (MP_STATE_PORT(virtio_device) == NULL) {
        openamp_init();
    }

    if (rpmsg_create_ept(&self->ep, &MP_STATE_PORT(virtio_device)->rvdev.rdev, mp_obj_str_get_str(self->name),
        args[ARG_src].u_int, args[ARG_dest].u_int, endpoint_recv_callback, NULL) != 0) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Failed to create RPMsg endpoint"));
    }

    return MP_OBJ_FROM_PTR(self);
}

static const mp_rom_map_elem_t endpoint_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_Endpoint) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&endpoint_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&endpoint_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_ready), MP_ROM_PTR(&endpoint_is_ready_obj) },
};
static MP_DEFINE_CONST_DICT(endpoint_locals_dict, endpoint_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    endpoint_type,
    MP_QSTR_Endpoint,
    MP_TYPE_FLAG_NONE,
    make_new, endpoint_make_new,
    locals_dict, &endpoint_locals_dict
    );

// ###################### openamp module ######################
void openamp_remoteproc_notified(mp_sched_node_t *node) {
    (void)node;
    rproc_virtio_notified(MP_STATE_PORT(virtio_device)->rvdev.vdev, VRING_NOTIFY_ID);
}

static void openamp_ns_callback(struct rpmsg_device *rdev, const char *name, uint32_t dest) {
    metal_log(METAL_LOG_DEBUG, "rpmsg_new_service_callback() new service request name: %s dest %lu\n", name, dest);
    // The remote processor advertises its presence to the host by sending
    // the Name Service (NS) announcement containing the name of the channel.
    virtio_dev_obj_t *virtio_device = metal_container_of(rdev, virtio_dev_obj_t, rvdev);
    if (virtio_device->ns_callback != mp_const_none) {
        mp_call_function_2(virtio_device->ns_callback, mp_obj_new_int(dest), mp_obj_new_str_from_cstr(name));
    }
}

#if MICROPY_PY_OPENAMP_HOST && MICROPY_PY_OPENAMP_RSC_TABLE_ENABLE
// The shared resource table must be initialized manually by the host here,
// because it's not located in the data region, so the startup code doesn't
// know about it.
static void openamp_rsc_table_init(openamp_rsc_table_t *rsc_table) {
    memset(rsc_table, 0, METAL_RSC_SIZE);

    rsc_table->version = 1;
    rsc_table->num = MP_ARRAY_SIZE(rsc_table->offset);
    rsc_table->offset[0] = offsetof(openamp_rsc_table_t, vdev);
    #if MICROPY_PY_OPENAMP_TRACE_BUF_ENABLE
    rsc_table->offset[1] = offsetof(openamp_rsc_table_t, trace);
    #endif
    rsc_table->vdev = (struct fw_rsc_vdev) {
        RSC_VDEV, VIRTIO_ID_RPMSG, 0, VIRTIO_DEV_FEATURES, 0, 0, 0, VRING_COUNT, {0, 0}
    };
    rsc_table->vring0 = (struct fw_rsc_vdev_vring) {
        VRING_TX_ADDR, VRING_ALIGNMENT, VRING_NUM_BUFFS, VRING0_ID, 0
    };
    rsc_table->vring1 = (struct fw_rsc_vdev_vring) {
        VRING_RX_ADDR, VRING_ALIGNMENT, VRING_NUM_BUFFS, VRING1_ID, 0
    };
    #if MICROPY_PY_OPENAMP_TRACE_BUF_ENABLE
    rsc_table->trace = (struct fw_rsc_trace) {
        RSC_TRACE, MICROPY_PY_OPENAMP_TRACE_BUF, MICROPY_PY_OPENAMP_TRACE_BUF_LEN, 0, "trace_buf"
    };
    #endif
    #ifdef VIRTIO_USE_DCACHE
    // Flush resource table.
    metal_cache_flush((uint32_t *)rsc_table, sizeof(openamp_rsc_table_t));
    #endif
}
#endif // MICROPY_PY_OPENAMP_HOST && MICROPY_PY_OPENAMP_RSC_TABLE_ENABLE

static mp_obj_t openamp_new_service_callback(mp_obj_t ns_callback) {
    if (MP_STATE_PORT(virtio_device) == NULL) {
        openamp_init();
    }
    if (ns_callback != mp_const_none && !mp_obj_is_callable(ns_callback)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid callback"));
    }
    MP_STATE_PORT(virtio_device)->ns_callback = ns_callback;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(openamp_new_service_callback_obj, openamp_new_service_callback);

void openamp_metal_log_handler(enum metal_log_level level, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    mp_vprintf(&mp_plat_print, fmt, args);
    va_end(args);
}

void openamp_init(void) {
    if (MP_STATE_PORT(virtio_device) != NULL) {
        // Already initialized.
        return;
    }

    struct metal_device *device;
    struct metal_init_params metal_params = { 0 };

    #if METAL_LOG_HANDLER_ENABLE
    // If logging is enabled, set the default log level and handler before
    // calling metal_init, to allow ports to override them in metal_sys_init.
    metal_params.log_level = METAL_LOG_DEBUG;
    metal_params.log_handler = openamp_metal_log_handler;
    #endif

    // Initialize libmetal.
    metal_init(&metal_params);

    // Initialize the shared resource table.
    openamp_rsc_table_t *rsc_table = METAL_RSC_ADDR;
    #if MICROPY_PY_OPENAMP_HOST
    openamp_rsc_table_init(rsc_table);
    #endif // MICROPY_PY_OPENAMP_HOST

    if (metal_register_generic_device(&shm_device) != 0) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Failed to register metal device"));
    }

    if (metal_device_open("generic", METAL_SHM_NAME, &device) != 0) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Failed to open metal device"));
    }

    // Initialize shared memory IO region.
    metal_io_init(&device->regions[0], (void *)METAL_SHM_ADDR, (void *)shm_physmap, METAL_SHM_SIZE, -1U, 0, NULL);
    struct metal_io_region *shm_io = metal_device_io_region(device, 0);
    if (shm_io == NULL) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Failed to initialize device io region"));
    }

    // Initialize resource table IO region.
    metal_io_init(&device->regions[1], (void *)rsc_table, (void *)rsc_table, sizeof(*rsc_table), -1U, 0, NULL);
    struct metal_io_region *rsc_io = metal_device_io_region(device, 1);
    if (rsc_io == NULL) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Failed to initialize device io region"));
    }

    // Create virtio device.
    struct virtio_device *vdev = rproc_virtio_create_vdev(VIRTIO_DEV_ROLE, VIRTIO_DEV_ID,
        &rsc_table->vdev, rsc_io, NULL, metal_rproc_notify, NULL);
    if (vdev == NULL) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Failed to create virtio device"));
    }

    // Initialize vrings.
    struct fw_rsc_vdev_vring *vring_rsc = &rsc_table->vring0;
    for (int i = 0; i < VRING_COUNT; i++, vring_rsc++) {
        if (rproc_virtio_init_vring(vdev, vring_rsc->notifyid, vring_rsc->notifyid,
            (void *)vring_rsc->da, shm_io, vring_rsc->num, vring_rsc->align) != 0) {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Failed to initialize vrings"));
        }
    }

    virtio_dev_obj_t *virtio_device = mp_obj_malloc_with_finaliser(virtio_dev_obj_t, &virtio_dev_type);
    virtio_device->ns_callback = mp_const_none;

    // The remote processor detects that the virtio device is ready by polling
    // the status field in the resource table.
    rpmsg_virtio_init_shm_pool(&virtio_device->shm_pool, (void *)VRING_BUFF_ADDR, (size_t)VRING_BUFF_SIZE);

    rpmsg_init_vdev(&virtio_device->rvdev, vdev, openamp_ns_callback, shm_io, &virtio_device->shm_pool);
    MP_STATE_PORT(virtio_device) = virtio_device;
}

static const mp_rom_map_elem_t globals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_openamp) },
    { MP_ROM_QSTR(MP_QSTR_ENDPOINT_ADDR_ANY), MP_ROM_INT(RPMSG_ADDR_ANY) },
    { MP_ROM_QSTR(MP_QSTR_new_service_callback), MP_ROM_PTR(&openamp_new_service_callback_obj) },
    { MP_ROM_QSTR(MP_QSTR_Endpoint), MP_ROM_PTR(&endpoint_type) },
    #if MICROPY_PY_OPENAMP_HOST && MICROPY_PY_OPENAMP_REMOTEPROC
    { MP_ROM_QSTR(MP_QSTR_RemoteProc), MP_ROM_PTR(&openamp_remoteproc_type) },
    #endif
};
static MP_DEFINE_CONST_DICT(globals_dict, globals_dict_table);

const mp_obj_module_t openamp_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_t)&globals_dict,
};

MP_REGISTER_ROOT_POINTER(struct _virtio_dev_obj_t *virtio_device);
MP_REGISTER_MODULE(MP_QSTR_openamp, openamp_module);

#endif // MICROPY_PY_OPENAMP
