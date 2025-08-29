/*
 * This file is part of the MicroPython project, https://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025-2026 Alessandro Gatti
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

#include "py/mphal.h"

#if MICROPY_PY_GPIO

#ifndef __linux__
#error "Platform does not support GPIO access"
#endif

#if MICROPY_PY_GPIO_IRQ && !MICROPY_PY_THREAD
#error "GPIO IRQ support needs threading to work"
#endif
#if MICROPY_PY_GPIO_DYNAMIC_ALLOCATION && !MICROPY_PY_THREAD
#error "GPIO dynamic pin allocation needs threading to work"
#endif

#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#if MICROPY_PY_GPIO_IRQ
#include <pthread.h>
#include <signal.h>
#include <sys/epoll.h>
#endif
#if MICROPY_PY_GPIO_DYNAMIC_ALLOCATION
#include <unistd.h>
#include <sys/inotify.h>
#endif

#include <linux/gpio.h>

#include "py/gc.h"
#include "py/misc.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/objint.h"
#include "py/objstr.h"
#include "py/qstr.h"
#include "py/runtime.h"

#include "pin.h"

#if MICROPY_DEBUG_VERBOSE
#define DEBUG_PRINT (1)
#define DEBUG_printf DEBUG_printf
#else
#define DEBUG_printf(...) (void)0
#endif

static const char *GPIO_DEVICES_ROOT = "/sys/bus/gpio/devices/";

///////////////////////////////////////////////////////////////////////////////

typedef struct _gpio_port_t {
    mp_obj_t path;
    mp_obj_t name;
    mp_obj_t label;
    mp_obj_t *pins;
    int fd;
    uint32_t lines;
    #if MICROPY_PY_GPIO_DYNAMIC_ALLOCATION
    int watch_descriptor;
    #endif
} gpio_port_t;

#if MICROPY_PY_GPIO_IRQ

static void *gpio_epoll_thread(void *arg);

static pthread_t gpio_epoll_thread_id = 0;
static int gpio_epoll_fd = -1;
static volatile bool gpio_epoll_thread_stop = false;

#endif

#if MICROPY_PY_GPIO_DYNAMIC_ALLOCATION

static void *gpio_inotify_thread(void *arg);

static pthread_t gpio_inotify_thread_id = 0;
static int gpio_inotify_fd = -1;
static volatile bool gpio_inotify_thread_stop = false;

#endif

static void gpio_ioctl(int fd, int call, void *payload) {
    MP_THREAD_GIL_EXIT();
    int result = ioctl(fd, call, payload); \
    MP_THREAD_GIL_ENTER();
    if (result < 0) {
        mp_raise_OSError(errno);
    }
}

static void fd_cleanup_callback(void *fd) {
    MP_THREAD_GIL_EXIT();
    close((intptr_t)fd);
    MP_THREAD_GIL_ENTER();
}

// Thanks to the existence of symbolic links, any character device on the
// machine can be referenced by an infinite number of names.  However,
// character devices can still be referenced by their device identifier.  Too
// bad a device identifier *is not unique* across devices of different classes
// so that won't save you from errors, and sending the right ioctl to the wrong
// device may or may not be harmful.
//
// Still, the "/sys/bus/gpio/devices" directory will contain the names of
// available GPIO character devices, with each entry in there being a link to
// another directory with text files containing more information on the target
// device.  To keep things manageable and third party dependency free (as in,
// not making libudev a dependency), the given port name (not the full path!)
// is checked for existence as "/sys/bus/gpio/devices/<portname>/dev", and if
// there's a match, then it is opened as "/dev/<portname>".  This introduces a
// potential race condition if what is reported in procfs and what is reported
// in devtmpfs happens to change after procfs is evaluated and before devtmpfs
// is looked into.  Security-wise, I guess said race condition could only be
// exploitable if an attacker can manipulate entries in both "/sys" and "/dev";
// in that case, the game may already be lost to begin with.  Probably not much
// can be done on our end to mitigate that.
//
// For more mundane security mitigations, though, characters in the port name
// are restricted to be in the ranges "a-z", "A-Z", "0-9".  The lack of
// slashes, tildes, and other characters should be good enough for stopping
// unwanted directory traversals.  If it does not, then patches are welcome.
//
// This method for resolving character devices' effective path, despite its
// limits, makes handling dynamic GPIO ports disconnections much easier.  An
// inotify change watch can be placed on "/sys/bus/gpio/devices" and then the
// ports dictionary can be updated to reflect the new devices set whenever the
// inotify callback is invoked.

static mp_obj_t resolve_gpio_device_name(mp_obj_t device) {
    mp_obj_t name = mp_const_none;

    if (mp_obj_is_int(device)) {
        mp_int_t device_node_id = mp_obj_get_int(device);
        // Artificially limit the number of GPIO ports to 255 for now.
        if (device_node_id < 0 || device_node_id > 255) {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid device number"));
        }

        char path[sizeof("gpiochip") + 3] = {};
        snprintf(path, sizeof(path), "gpiochip%d", (uint8_t)device_node_id);
        name = mp_obj_new_str_from_cstr(path);
    }

    if (mp_obj_is_str(device)) {
        name = mp_obj_is_qstr(device) ? mp_obj_str_intern_checked(device) : device;
    }

    if (name == mp_const_none) {
        mp_raise_TypeError(MP_ERROR_TEXT("invalid port type"));
    }

    size_t length = 0;
    const char *raw_name = mp_obj_str_get_data(name, &length);
    for (size_t index = 0; index < length; index++) {
        char character = raw_name[index];
        // "Be aware: These unichar_is* functions are actually ASCII-only!"
        if (!unichar_isalpha(character) && !unichar_isdigit(character)) {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid port name"));
        }
    }

    vstr_t port_path;
    vstr_init(&port_path, sizeof(GPIO_DEVICES_ROOT) - 1 + length + sizeof("/dev") - 1);
    vstr_add_str(&port_path, GPIO_DEVICES_ROOT);
    vstr_add_strn(&port_path, raw_name, length);
    vstr_add_str(&port_path, "/dev");
    const char *path = vstr_null_terminated_str(&port_path);

    MP_THREAD_GIL_EXIT();
    int fd = open(path, O_RDONLY | O_CLOEXEC);
    MP_THREAD_GIL_ENTER();
    if (fd < 0) {
        return mp_const_none;
    }
    MP_THREAD_GIL_EXIT();
    close(fd);
    MP_THREAD_GIL_ENTER();

    vstr_init(&port_path, sizeof("/dev/") - 1 + length);
    vstr_add_str(&port_path, "/dev/");
    vstr_add_strn(&port_path, raw_name, length);
    path = vstr_null_terminated_str(&port_path);

    MP_THREAD_GIL_EXIT();
    fd = open(path, O_RDWR | O_CLOEXEC);
    close(fd);
    MP_THREAD_GIL_ENTER();
    if (fd < 0) {
        return mp_const_none;
    }

    // Probably duplication is not needed...
    return mp_obj_new_str_from_cstr(path);
}

#if MICROPY_PY_GPIO_IRQ

static void gpio_epoll_init(void) {
    mp_set_init(&MP_STATE_PORT(epoll_pins), 0);

    MP_THREAD_GIL_EXIT();
    gpio_epoll_fd = epoll_create1(0);
    MP_THREAD_GIL_ENTER();
    if (gpio_epoll_fd < 0) {
        mp_raise_OSError(errno);
    }

    MP_THREAD_GIL_EXIT();
    int result = pthread_create(&gpio_epoll_thread_id, NULL, &gpio_epoll_thread, (void *)&gpio_epoll_thread_stop);
    MP_THREAD_GIL_ENTER();
    if (result != 0) {
        mp_raise_OSError(errno);
    }
}

static void gpio_epoll_deinit(void) {
    gpio_epoll_thread_stop = true;
    MP_THREAD_GIL_EXIT();
    close(gpio_epoll_fd);
    MP_THREAD_GIL_ENTER();
    gpio_epoll_fd = -1;
    MP_THREAD_GIL_EXIT();
    int result = pthread_cancel(gpio_epoll_thread_id);
    MP_THREAD_GIL_ENTER();
    assert(result >= 0 && "pthread_cancel failed to cancel the epoll thread");
    MP_THREAD_GIL_EXIT();
    result = pthread_join(gpio_epoll_thread_id, NULL);
    MP_THREAD_GIL_ENTER();
    assert(result >= 0 && "pthread_join failed to wait until the epoll thread exited");
    (void)result;
    gpio_epoll_thread_id = 0;
    // The pins set can be safely cleared without being in a critical section.
    mp_set_clear(&MP_STATE_PORT(epoll_pins));
    m_del(struct epoll_event, MP_STATE_PORT(epoll_events), MICROPY_PY_GPIO_IRQ_QUEUE_SIZE);
}

// epoll() descriptor set management functions.

static void gpio_epoll_undo_addition(void *pin) {
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    mp_obj_t removed_object = mp_set_lookup(&MP_STATE_PORT(epoll_pins), MP_OBJ_FROM_PTR(pin), MP_MAP_LOOKUP_REMOVE_IF_FOUND);
    MICROPY_END_ATOMIC_SECTION(atomic_state);
    assert(removed_object != MP_OBJ_NULL && "Nothing to undo?");
    (void)removed_object;
}

static void gpio_epoll_add_pin(mp_obj_t pin_in) {
    machine_pin_obj_t *pin = MP_OBJ_TO_PTR(pin_in);

    assert((pin->fd >= 0) && "Adding a pin with an invalid descriptor to the epoll list");

    memset(&pin->event, 0x00, sizeof(struct epoll_event));
    // Only report edge-trigger events (we are just interested in whether the
    // pin changed state, nothing more).
    pin->event.events = EPOLLET;
    pin->event.data.ptr = pin;

    mp_set_t *epoll_pins = &MP_STATE_PORT(epoll_pins);

    int result = 0;
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();

    // There's no direct way to just query a set whether an element is stored
    // in there or not, so we traverse the items table instead.

    bool element_already_added = false;

    for (size_t index = 0; index < epoll_pins->alloc; index++) {
        mp_obj_t element = epoll_pins->table[index];
        if (element == pin_in) {
            element_already_added = true;
            break;
        }
    }

    assert((!element_already_added || (element_already_added && (pin->event.events != 0 && pin->event.data.ptr != 0))) && "epoll set went out of sync");

    if (!element_already_added) {
        // Adding the file descriptor to an epoll group descriptor may fail,
        // and if it does then we have to clean up the epoll pins set.
        // Updating the set may also incur on a faillible memory reallocation
        // too.  It's easier to undo a set addition rather than dealing with
        // epoll once more.
        //
        // TODO: are NLR jump callbacks safe to be fully enclosed inside a
        //       critical section?

        bool force_cleanup = false;
        MP_DEFINE_NLR_JUMP_CALLBACK_FUNCTION_1(ctx, gpio_epoll_undo_addition, (void *)pin);
        nlr_push_jump_callback(&ctx.callback, mp_call_function_1_from_nlr_jump_callback);

        // mp_set_lookup may raise when calling mp_set_rehash on low memory
        mp_set_lookup(epoll_pins, pin_in, MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
        MP_THREAD_GIL_EXIT();
        result = epoll_ctl(gpio_epoll_fd, EPOLL_CTL_ADD, pin->fd, &pin->event);
        MP_THREAD_GIL_ENTER();
        if (result < 0) {
            force_cleanup = true;
        }

        nlr_pop_jump_callback(force_cleanup);
    }

    MICROPY_END_ATOMIC_SECTION(atomic_state);

    if (result < 0) {
        mp_raise_OSError(-result);
    }
}

static void gpio_epoll_remove_pin(mp_obj_t pin_in) {
    // This may be invoked during GPIO subsystem deinitialisation, so if the
    // epoll thread should stop leave things alone (the group descriptor is
    // probably already closed by now).
    if (gpio_epoll_thread_stop) {
        return;
    }

    machine_pin_obj_t *pin = MP_OBJ_TO_PTR(pin_in);

    assert((pin->fd >= 0) && "Removing a pin with an invalid descriptor from the epoll group");
    memset(&pin->event, 0x00, sizeof(struct epoll_event));

    int result = 0;
    bool element_found = false;
    mp_set_t *epoll_pins = &MP_STATE_PORT(epoll_pins);
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();

    for (size_t index = 0; index < epoll_pins->alloc; index++) {
        mp_obj_t element = epoll_pins->table[index];
        if (element == pin_in) {
            element_found = true;
            break;
        }
    }

    if (element_found) {
        // Removing an item from a set will always succeed, so the only check here
        // is whether the removal of the pin's file descriptor from the epoll
        // descriptors group succeeded or not.

        MP_THREAD_GIL_EXIT();
        result = epoll_ctl(gpio_epoll_fd, EPOLL_CTL_DEL, pin->fd, NULL);
        MP_THREAD_GIL_ENTER();
        if (result >= 0) {
            mp_obj_t removed_object = mp_set_lookup(&MP_STATE_PORT(epoll_pins), MP_OBJ_FROM_PTR(pin), MP_MAP_LOOKUP_REMOVE_IF_FOUND);
            pin->callback = mp_const_none;
            assert(removed_object != MP_OBJ_NULL && "Pin descriptor not found in epoll group");
            (void)removed_object;
        }
    }

    MICROPY_END_ATOMIC_SECTION(atomic_state);

    if (result < 0) {
        mp_raise_OSError(-result);
    }
}

#endif

#if MICROPY_PY_GPIO_DYNAMIC_ALLOCATION

static void gpio_inotify_init(void) {
    MP_THREAD_GIL_EXIT();
    gpio_inotify_fd = inotify_init1(IN_CLOEXEC);
    MP_THREAD_GIL_ENTER();
    if (gpio_inotify_fd < 0) {
        mp_raise_OSError(errno);
    }

    MP_THREAD_GIL_EXIT();
    int result = pthread_create(&gpio_inotify_thread_id, NULL, &gpio_inotify_thread, (void *)&gpio_inotify_thread_stop);
    MP_THREAD_GIL_ENTER();
    if (result != 0) {
        mp_raise_OSError(errno);
    }
}

static void gpio_inotify_deinit(void) {
    gpio_inotify_thread_stop = true;
    MP_THREAD_GIL_EXIT();
    close(gpio_inotify_fd);
    MP_THREAD_GIL_ENTER();
    gpio_inotify_fd = -1;
    MP_THREAD_GIL_EXIT();
    int result = pthread_cancel(gpio_inotify_thread_id);
    MP_THREAD_GIL_ENTER();
    assert(result >= 0 && "pthread_cancel failed to cancel the inotify thread");
    MP_THREAD_GIL_EXIT();
    result = pthread_join(gpio_inotify_thread_id, NULL);
    MP_THREAD_GIL_ENTER();
    assert(result >= 0 && "pthread_join failed to wait until the inotify exited");
    (void)result;
    gpio_inotify_thread_id = 0;
    m_del(struct inotify_event, MP_STATE_PORT(inotify_events), MICROPY_PY_GPIO_ALLOCATION_QUEUE_SIZE);
}

#endif

// This expects to receive an already resolved port name!
static gpio_port_t *gpio_add_port(mp_obj_t path) {
    mp_map_t *ports = &MP_STATE_PORT(ports);
    assert(ports && "Ports dictionary must not be NULL");
    mp_map_elem_t *slot = mp_map_lookup(ports, path, MP_MAP_LOOKUP);
    if (slot) {
        return (gpio_port_t *)MP_OBJ_TO_PTR(slot->value);
    }

    MP_THREAD_GIL_EXIT();
    mp_int_t fd = open(mp_obj_str_get_str(path), O_RDWR | O_CLOEXEC);
    MP_THREAD_GIL_ENTER();
    if (fd < 0) {
        mp_raise_OSError(errno);
    }

    #if MICROPY_PY_GPIO_DYNAMIC_ALLOCATION
    int watch_descriptor = -1;
    #endif

    gpio_port_t *port;
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        struct gpiochip_info info;
        memset(&info, 0x00, sizeof(info));
        gpio_ioctl(fd, GPIO_GET_CHIPINFO_IOCTL, &info);

        mp_obj_t *pins = NULL;
        if (info.lines > 0) {
            MP_STATIC_ASSERT(MP_OBJ_NULL == 0);
            pins = m_new0(mp_obj_t, info.lines);
        }

        port = m_new(gpio_port_t, 1);
        port->path = path;
        port->name = mp_obj_new_str_from_cstr(info.name);
        port->label = mp_obj_new_str_from_cstr(info.label);
        port->pins = pins;
        port->fd = fd;
        port->lines = info.lines;

        #if MICROPY_PY_GPIO_DYNAMIC_ALLOCATION
        // TODO: See if an inotify watch on the /dev entry is sufficient, otherwise
        //       rebuild a sysfs path out of this.
        watch_descriptor = inotify_add_watch(gpio_inotify_fd, mp_obj_str_get_str(path), IN_DELETE | IN_DELETE_SELF);
        if (watch_descriptor < 0) {
            mp_raise_OSError(errno);
        }
        port->watch_descriptor = watch_descriptor;
        #endif

        slot = mp_map_lookup(ports, path, MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
        assert(slot && "No slot to fill was returned on port add");
        slot->value = MP_OBJ_FROM_PTR(port);

        nlr_pop();
    } else {
        #if MICROPY_PY_GPIO_DYNAMIC_ALLOCATION
        if (watch_descriptor != -1) {
            inotify_rm_watch(gpio_inotify_fd, watch_descriptor);
        }
        #endif
        close(fd);

        // Re-raise the exception.
        nlr_jump(nlr.ret_val);
    }

    return port;
}

// This expects to receive an already resolved port name!
static mp_obj_t gpio_find_pin(mp_obj_t port_key, uint32_t pin) {
    mp_map_t *ports = &MP_STATE_PORT(ports);
    mp_map_elem_t *slot = mp_map_lookup(ports, port_key, MP_MAP_LOOKUP);
    if (!slot || slot->value == MP_OBJ_NULL) {
        return MP_OBJ_NULL;
    }
    gpio_port_t *port = (gpio_port_t *)MP_OBJ_TO_PTR(slot->value);
    if (pin >= port->lines) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid pin number"));
    }
    return port->pins[pin];
}

static void gpio_evict_pin(mp_obj_t pin_in) {
    #if MICROPY_PY_GPIO_IRQ
    gpio_epoll_remove_pin(pin_in);
    #endif

    machine_pin_obj_t *pin = MP_OBJ_TO_PTR(pin_in);
    mp_map_t *ports = &MP_STATE_PORT(ports);
    mp_map_elem_t *slot = mp_map_lookup(ports, pin->port, MP_MAP_LOOKUP);
    if (!slot) {
        // The port this pin belongs to may have already been evicted during
        // GPIO support deinitialisation or by a pin object finaliser.
        return;
    }

    gpio_port_t *port = MP_OBJ_TO_PTR(slot->value);
    assert((port == (gpio_port_t *)MP_OBJ_TO_PTR(pin->port)) && "Pin and port went out of sync");
    assert((pin->number < port->lines) && "Pin and port went out of sync");
    assert(port->pins[pin->number] == pin_in && "Pin objects mismatch");
    port->pins[pin->number] = MP_OBJ_NULL;

    bool empty_port = true;
    for (size_t index = 0; index < port->lines; index++) {
        if (port->pins[index] != MP_OBJ_NULL) {
            empty_port = false;
            break;
        }
    }

    if (!empty_port) {
        return;
    }

    slot = mp_map_lookup(ports, pin->port, MP_MAP_LOOKUP_REMOVE_IF_FOUND);
    assert(slot && "Port disappeared from map");
    MP_THREAD_GIL_EXIT();
    close(port->fd);
    MP_THREAD_GIL_ENTER();
    port->fd = -1;
    m_del(mp_obj_t, port->pins, port->lines);
    slot->value = MP_OBJ_NULL;
    m_del(gpio_port_t, port, 1);
}

static uint32_t gpio_get_pin_number(mp_obj_t number_in) {
    assert(mp_obj_is_int(number_in) && "Invalid pin number type");

    uint32_t number = 0;

    // If mp_obj_int_to_bytes would signal whether the number was truncated or
    // not, this NLR shouldn't be needed.  For this case a more appropriate
    // error message is required rather than "integer out of range".

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_int_to_bytes(number_in, sizeof(uint32_t), (byte *)&number,
            #if MP_ENDIANNESS_BIG
            true,
            #else
            false,
            #endif
            false, true);
        nlr_pop();
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid pin number"));
    }

    return number;
}

static void gpio_fill_line_config_struct(struct gpio_v2_line_config *config, uint32_t pin, mp_obj_t mode, mp_obj_t pull, mp_obj_t value) {
    assert(config && "GPIO config structrure must not be NULL");

    memset(config, 0x00, sizeof(struct gpio_v2_line_config));

    // Checks for mode and pull could be made simpler by just checking whether
    // the value bit is in the combined OR of all valid options, and then use
    // the value directly.  Initialisation will fail anyway but you won't know
    // why, so we have to do this instead.

    if (mode != mp_const_none) {
        switch (mp_obj_get_int(mode)) {
            case GPIO_V2_LINE_FLAG_INPUT:
                config->flags |= GPIO_V2_LINE_FLAG_INPUT;
                break;

            case GPIO_V2_LINE_FLAG_OUTPUT:
                config->flags |= GPIO_V2_LINE_FLAG_OUTPUT;
                break;

            default:
                mp_raise_ValueError(MP_ERROR_TEXT("invalid pin mode"));
                break;
        }
    }

    if (pull != MP_OBJ_NEW_SMALL_INT(-1)) {
        switch (mp_obj_get_int(pull)) {
            case GPIO_V2_LINE_FLAG_BIAS_PULL_UP:
                config->flags |= GPIO_V2_LINE_FLAG_BIAS_PULL_UP;
                break;

            case GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN:
                config->flags |= GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN;
                break;

            default:
                mp_raise_ValueError(MP_ERROR_TEXT("invalid pull mode"));
                break;
        }
    }

    if (value != MP_OBJ_NULL) {
        config->num_attrs = 1;
        config->attrs[0].attr.id = GPIO_V2_LINE_ATTR_ID_OUTPUT_VALUES;
        config->attrs[0].attr.values = (mp_obj_is_true(value) ? 1U : 0U);
        config->attrs[0].mask = 1U;
    }
}

static int gpio_set_state_inner(int fd, mp_int_t state) {
    struct gpio_v2_line_values values;
    memset(&values, 0x00, sizeof(values));
    values.mask = 1U;
    values.bits = state != 0 ? 1U : 0U;
    MP_THREAD_GIL_EXIT();
    int result = ioctl(fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &values);
    MP_THREAD_GIL_ENTER();
    return result;
}

static int gpio_get_state_inner(int fd, mp_int_t *state) {
    assert(state && "GPIO state value pointer cannot be NULL");

    struct gpio_v2_line_values values;
    memset(&values, 0x00, sizeof(values));
    values.mask = 1U;
    values.bits = 1U;
    MP_THREAD_GIL_EXIT();
    int result = ioctl(fd, GPIO_V2_LINE_GET_VALUES_IOCTL, &values);
    MP_THREAD_GIL_ENTER();
    if (result >= 0) {
        *state = (values.bits & 1U) ? 1 : 0;
    }
    return result;
}

static void gpio_set_state(mp_obj_t self_in, mp_int_t state) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (gpio_set_state_inner(self->fd, state) < 0) {
        mp_raise_OSError(errno);
    }
}

static bool gpio_get_state(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t state;
    if (gpio_get_state_inner(self->fd, &state) < 0) {
        mp_raise_OSError(errno);
    }
    return !!state;
}

///////////////////////////////////////////////////////////////////////////////

static mp_obj_t init_helper(uint32_t pin_number, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_mode, ARG_pull, ARG_value, ARG_port, ARG_consumer };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode,                      MP_ARG_OBJ, { .u_obj = mp_const_none                        }},
        { MP_QSTR_pull,                      MP_ARG_OBJ, { .u_obj = MP_OBJ_NEW_SMALL_INT(-1)             }},
        { MP_QSTR_value,    MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL                          }},
        { MP_QSTR_port,     MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = MP_OBJ_NEW_SMALL_INT(0)              }},
        { MP_QSTR_consumer, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = MP_OBJ_NEW_QSTR(MP_QSTR_MicroPython) }},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    struct gpio_v2_line_config config;
    gpio_fill_line_config_struct(&config, pin_number, args[ARG_mode].u_obj, args[ARG_pull].u_obj, args[ARG_value].u_obj);

    mp_obj_t port_path = resolve_gpio_device_name(args[ARG_port].u_obj);
    if (port_path == mp_const_none) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid port name"));
    }

    mp_obj_t existing_pin = gpio_find_pin(port_path, pin_number);
    if (existing_pin != MP_OBJ_NULL) {
        machine_pin_obj_t *pin = MP_OBJ_TO_PTR(existing_pin);

        // Reconfigure the pin with the new parameters.
        // WARNING: If an IRQ callback was attached before this point, it will
        //          stay bound to the pin instance.
        if (pin->fd >= 0) {
            gpio_ioctl(pin->fd, GPIO_V2_LINE_SET_CONFIG_IOCTL, &config);
            return existing_pin;
        }

        gpio_evict_pin(existing_pin);
    }

    if (!mp_obj_is_str(args[ARG_consumer].u_obj)) {
        mp_raise_TypeError(MP_ERROR_TEXT("invalid consumer"));
    }
    const char *consumer;
    size_t consumer_length;
    if (mp_obj_is_qstr(args[ARG_consumer].u_obj)) {
        consumer = (const char *)qstr_data(MP_OBJ_QSTR_VALUE(args[ARG_consumer].u_obj), &consumer_length);
    } else {
        consumer = mp_obj_str_get_data(args[ARG_consumer].u_obj, &consumer_length);
    }
    if (consumer_length >= GPIO_MAX_NAME_SIZE) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid consumer"));
    }

    // If the port is already known then it will just return the old instance.
    gpio_port_t *port = gpio_add_port(port_path);

    if (pin_number >= port->lines) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid pin number"));
    }

    struct gpio_v2_line_request request;
    memset(&request, 0x00, sizeof(request));
    request.offsets[0] = pin_number;
    request.config = config;
    request.num_lines = 1;
    strcpy(request.consumer, consumer);

    gpio_ioctl(port->fd, GPIO_V2_GET_LINE_IOCTL, &request);

    MP_DEFINE_NLR_JUMP_CALLBACK_FUNCTION_1(ctx, fd_cleanup_callback, (void *)(intptr_t)request.fd);
    nlr_push_jump_callback(&ctx.callback, mp_call_function_1_from_nlr_jump_callback);

    machine_pin_obj_t *self = mp_obj_malloc_with_finaliser(machine_pin_obj_t, &machine_pin_type);
    self->port = MP_OBJ_FROM_PTR(port);
    self->number = pin_number;
    self->fd = request.fd;

    #if MICROPY_PY_GPIO_IRQ
    self->callback = mp_const_none;
    self->hard_callback = false;
    memset(&self->event, 0x00, sizeof(struct epoll_event));
    #endif

    mp_obj_t self_out = MP_OBJ_FROM_PTR(self);

    if (port->pins[pin_number] != MP_OBJ_NULL) {
        machine_pin_obj_t *old_self = MP_OBJ_TO_PTR(port->pins[pin_number]);
        gpio_evict_pin(port->pins[pin_number]);
        port->pins[pin_number] = MP_OBJ_NULL;
        MP_THREAD_GIL_EXIT();
        close(old_self->fd);
        MP_THREAD_GIL_ENTER();
    }
    port->pins[pin_number] = self_out;
    nlr_pop_jump_callback(false);

    return self_out;
}

mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    if (mp_obj_is_type(args[0], &machine_pin_type)) {
        return args[0];
    }

    if (mp_obj_is_int(args[0])) {
        uint32_t number = gpio_get_pin_number(args[0]);
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        return init_helper(number, n_args - 1, args + 1, &kw_args);
    }

    mp_raise_TypeError(MP_ERROR_TEXT("invalid pin type"));
}

static void machine_pin_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    const machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    const gpio_port_t *port = MP_OBJ_TO_PTR(self->port);
    mp_printf(print, "Pin(%s, %d)", mp_obj_str_get_str(port->name), self->number);
}

static mp_obj_t machine_pin_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (n_args == 0) {
        return MP_OBJ_NEW_SMALL_INT(gpio_get_state(self));
    }
    gpio_set_state(self, mp_obj_is_true(args[0]));
    return mp_const_none;
}

static mp_uint_t pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->fd < 0) {
        if (errcode != NULL) {
            *errcode = MP_EPIPE;
        }
        return -1;
    }

    switch (request) {
        case MP_PIN_READ: {
            mp_int_t state = 0;
            int result = gpio_get_state_inner(self->fd, &state);
            if (errcode != NULL) {
                *errcode = result < 0 ? result : 0;
            }
            return result < 0 ? -1 : state;
        }

        case MP_PIN_WRITE: {
            int result = gpio_set_state_inner(self->fd, arg != 0 ? 1 : 0);
            if (errcode != NULL) {
                *errcode = result;
            }
            return result < 0 ? -1 : 0;
        }

        default:
            return -1;
    }
}

static mp_obj_t machine_pin_off(mp_obj_t self_in) {
    gpio_set_state(self_in, 0);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_off_obj, machine_pin_off);

static mp_obj_t machine_pin_on(mp_obj_t self_in) {
    gpio_set_state(self_in, 1);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_on_obj, machine_pin_on);

static mp_obj_t machine_pin_toggle(mp_obj_t self_in) {
    gpio_set_state(self_in, gpio_get_state(self_in) ? 0 : 1);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_toggle_obj, machine_pin_toggle);

mp_obj_t machine_pin_del(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    #if MICROPY_PY_GPIO_IRQ
    gpio_epoll_remove_pin(self_in);
    #endif
    MP_THREAD_GIL_EXIT();
    close(self->fd);
    MP_THREAD_GIL_ENTER();
    self->fd = -1;
    gpio_evict_pin(self_in);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_del_obj, machine_pin_del);

static mp_obj_t machine_pin_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return init_helper(gpio_get_pin_number(args[0]), n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_init_obj, 1, machine_pin_init);

static mp_obj_t machine_pin_available(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(self->fd != -1);
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_available_obj, machine_pin_available);

mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args) {
    return machine_pin_call(args[0], n_args - 1, 0, args + 1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_value_obj, 1, 2, machine_pin_value);

#if MICROPY_PY_GPIO_IRQ

static mp_obj_t machine_pin_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_handler, ARG_trigger, ARG_hard };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_handler, MP_ARG_OBJ,  { .u_obj = mp_const_none                                                  } },
        { MP_QSTR_trigger, MP_ARG_INT,  { .u_int = GPIO_V2_LINE_FLAG_EDGE_RISING | GPIO_V2_LINE_FLAG_EDGE_FALLING } },
        { MP_QSTR_hard,    MP_ARG_BOOL, { .u_bool = false                                                         } },
    };

    machine_pin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (n_args > 1 || kw_args->used != 0) {
        mp_uint_t trigger = args[ARG_trigger].u_int;
        if ((trigger & ~(GPIO_V2_LINE_FLAG_EDGE_RISING | GPIO_V2_LINE_FLAG_EDGE_FALLING)) != 0) {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid trigger"));
        }

        gpio_epoll_remove_pin(pos_args[0]);
        if (args[ARG_handler].u_obj != mp_const_none) {
            self->callback = args[ARG_handler].u_obj;
            self->hard_callback = args[ARG_hard].u_bool;
            gpio_epoll_add_pin(pos_args[0]);
        }
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_irq_obj, 1, machine_pin_irq);
#endif

static mp_obj_tuple_t *gpio_port_enumerate_lines(mp_obj_t port_name) {
    assert(port_name && "Port name is NULL");

    mp_obj_t device_name = resolve_gpio_device_name(port_name);
    if (device_name == mp_const_none) {
        return MP_OBJ_NULL;
    }

    int fd;
    MP_THREAD_GIL_EXIT();
    fd = open(mp_obj_str_get_str(device_name), O_RDWR | O_CLOEXEC);
    MP_THREAD_GIL_ENTER();
    if (fd < 0) {
        return MP_OBJ_NULL;
    }

    mp_obj_tuple_t *port = mp_obj_new_tuple(2, NULL);
    bool success = true;
    uint32_t lines = 0;

    {
        struct gpiochip_info chip_info;
        memset(&chip_info, 0x00, sizeof(chip_info));
        MP_THREAD_GIL_EXIT();
        int result = ioctl(fd, GPIO_GET_CHIPINFO_IOCTL, &chip_info);
        MP_THREAD_GIL_ENTER();
        if (result < 0) {
            success = false;
            goto done;
        }
        lines = chip_info.lines;
    }

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_tuple_t *pins = MP_OBJ_TO_PTR(mp_obj_new_tuple((size_t)lines, NULL));
        struct gpio_v2_line_info line_info;
        // name, consumer, available
        mp_obj_t line_objects[3] = {};
        for (uint32_t index = 0; index < lines; index++) {
            memset((void *)&line_info, 0x00, sizeof(line_info));
            line_info.offset = index;
            gpio_ioctl(fd, GPIO_V2_GET_LINEINFO_IOCTL, &line_info);
            line_objects[0] = mp_obj_new_str_from_cstr(line_info.name);
            line_objects[1] = mp_obj_new_str_from_cstr(line_info.consumer);
            line_objects[2] = mp_obj_new_bool((line_info.flags & GPIO_V2_LINE_FLAG_USED) == 0);
            pins->items[index] = mp_obj_new_tuple(3, (const void *)&line_objects);
        }
        port->items[0] = port_name;
        port->items[1] = MP_OBJ_FROM_PTR(pins);

        nlr_pop();
    } else {
        success = false;
    }

done:
    close(fd);

    return success ? port : MP_OBJ_NULL;
}

static mp_obj_t machine_pin_enumerate(void) {
    DIR *devices_directory = NULL;

    MP_THREAD_GIL_EXIT();
    devices_directory = opendir(GPIO_DEVICES_ROOT);
    MP_THREAD_GIL_ENTER();
    if (!devices_directory) {
        if (errno == ENOENT) {
            // GPIO support is not enabled in the running kernel?
            return mp_obj_new_tuple(0, NULL);
        } else {
            mp_raise_OSError(errno);
        }
    }

    bool failed = false;
    mp_obj_t ports = mp_obj_new_list(0, NULL);
    struct dirent *device_entry = NULL;
    do {
        errno = 0;
        MP_THREAD_GIL_EXIT();
        device_entry = readdir(devices_directory);
        MP_THREAD_GIL_ENTER();
        if (device_entry == NULL) {
            failed = errno != 0;
            break;
        }
        const char *name = device_entry->d_name;
        char *end = NULL;
        if (memcmp(name, "gpiochip", sizeof("gpiochip") - 1) == 0) {
            name += sizeof("gpiochip") - 1;
            long index = strtol(name, &end, 10);
            if ((index == LONG_MIN || index == LONG_MAX) && errno == ERANGE) {
                continue;
            }
            if (*end != '\0') {
                continue;
            }
            mp_obj_t port = gpio_port_enumerate_lines(mp_obj_new_str_from_cstr(device_entry->d_name));
            if (port != MP_OBJ_NULL) {
                mp_obj_list_append(ports, MP_OBJ_FROM_PTR(port));
            }
        }
    } while (device_entry != NULL);

    closedir(devices_directory);
    if (failed) {
        mp_raise_OSError(errno);
    }

    // TODO: Is there a better way to achieve this?
    return mp_obj_new_tuple(((mp_obj_list_t *)MP_OBJ_TO_PTR(ports))->len, ((mp_obj_list_t *)MP_OBJ_TO_PTR(ports))->items);
}
static MP_DEFINE_CONST_FUN_OBJ_0(machine_pin_enumerate_obj, machine_pin_enumerate);

///////////////////////////////////////////////////////////////////////////////

#if MICROPY_PY_GPIO_IRQ
static void *gpio_epoll_thread(void *arg) {
    // Keep a reference to the stop variable.
    volatile bool *stop = (volatile bool *)arg;

    assert(MP_STATE_PORT(epoll_events) != NULL && "GPIO epoll thread started at the wrong time?");

    for (;;) {
        int events = epoll_wait(gpio_epoll_fd, MP_STATE_PORT(epoll_events), MICROPY_PY_GPIO_IRQ_QUEUE_SIZE, -1);
        if (stop) {
            goto done;
        }

        if (events < 0) {
            // This is not correct but the code isn't in its final form yet.
            continue;
        }

        for (int event_index = 0; event_index < events; event_index++) {
            struct epoll_event *event = ((struct epoll_event *)(&MP_STATE_PORT(epoll_events))[event_index]);
            assert(((event->events & EPOLLET) == EPOLLET) && "Event not marked as edge triggered");
            // TODO: Is this actually correct?  In theory this should work if
            //       all modifications to the pin object involve setting the
            //       relevant pointers to MP_OBJ_NULL, but then what happens if
            //       the IRQ handlers are collected?  This needs more thought.
            mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
            mp_obj_t lookup_result = mp_set_lookup(&MP_STATE_PORT(epoll_pins), MP_OBJ_FROM_PTR(event->data.ptr), MP_MAP_LOOKUP);
            MICROPY_END_ATOMIC_SECTION(atomic_state);
            if (lookup_result == MP_OBJ_NULL) {
                continue;
            }
            machine_pin_obj_t *pin = MP_OBJ_TO_PTR(event->data.ptr);
            assert((pin->callback != MP_OBJ_NULL) && "Event for a tracked pin with no callback");
            if (pin->hard_callback) {
                #if MICROPY_ENABLE_SCHEDULER
                mp_sched_lock();
                #endif
                gc_lock();
                // TODO: Is this the correct way to call a Python function from
                //       a different thread even if the scheduler and GC are
                //       locked?
                nlr_buf_t nlr;
                if (nlr_push(&nlr) == 0) {
                    mp_call_function_1(pin->callback, MP_OBJ_FROM_PTR(pin));
                    nlr_pop();
                } else {
                    mp_printf(MICROPY_ERROR_PRINTER, "Uncaught exception in Pin IRQ callback\n");
                    mp_obj_print_exception(MICROPY_ERROR_PRINTER, MP_OBJ_FROM_PTR(nlr.ret_val));
                }
                gc_unlock();
                #if MICROPY_ENABLE_SCHEDULER
                mp_sched_unlock();
                #endif
            } else {
                mp_sched_schedule(pin->callback, MP_OBJ_FROM_PTR(pin));
            }
        }
    }

done:
    return NULL;
}
#endif

#if MICROPY_PY_GPIO_DYNAMIC_ALLOCATION
static void *gpio_inotify_thread(void *arg) {
    // Keep a reference to the stop variable.
    volatile bool *stop = (volatile bool *)arg;

    for (;;) {
        ssize_t read_count = read(gpio_inotify_fd, MP_STATE_PORT(inotify_events), MICROPY_PY_GPIO_ALLOCATION_QUEUE_SIZE * sizeof(struct inotify_event));
        if (stop) {
            goto done;
        }

        if (read_count < 0) {
            // This is not correct but the code isn't in its final form yet.
            continue;
        }

        const char *current_pointer = MP_STATE_PORT(inotify_events);
        for (;;) {
            const struct inotify_event *event = (const struct inotify_event *)current_pointer;
            mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
            // Find which port is involved with the removal event.
            mp_map_t *ports = &MP_STATE_PORT(ports);
            if (ports->table != NULL) {
                for (size_t port_index = 0; port_index < ports->alloc; port_index++) {
                    if (!mp_map_slot_is_filled(ports, port_index)) {
                        continue;
                    }
                    mp_map_elem_t slot = ports->table[port_index];
                    gpio_port_t *port = MP_OBJ_TO_PTR(slot.value);
                    if (port->watch_descriptor != event->wd) {
                        continue;
                    }
                    // This port was removed.  Evict all pins so the last one
                    // will deallocate the port as well.
                    for (size_t pin = 0; pin < port->lines; pin++) {
                        if (port->pins[pin] != MP_OBJ_NULL) {
                            gpio_evict_pin(port->pins[pin]);
                        }
                    }
                    // Remove the watch.
                    inotify_rm_watch(gpio_inotify_fd, event->wd);
                    break;
                }
            }
            MICROPY_END_ATOMIC_SECTION(atomic_state);
            current_pointer += sizeof(struct inotify_event) + event->len;
            if (((ptrdiff_t)current_pointer - (ptrdiff_t)MP_STATE_PORT(inotify_events)) >= read_count) {
                break;
            }
        }
    }

done:
    return NULL;
}
#endif

void mp_pin_init(void) {
    mp_map_init(&MP_STATE_PORT(ports), 0);

    #if MICROPY_PY_GPIO_IRQ
    MP_STATE_PORT(epoll_events) = m_new0(struct epoll_event, MICROPY_PY_GPIO_IRQ_QUEUE_SIZE);
    #endif
    #if MICROPY_PY_GPIO_DYNAMIC_ALLOCATION
    // The documentation suggests to align the buffer using the structure size
    // as the alignment boundary, to improve performance.  In this case we can
    // make the trade-off to wait a millisecond or so later that a GPIO port
    // went away.  Even if something happens between the port disappearance
    // and the inotify report, any ioctl on the port's pin file descriptors
    // (or on the port's descriptor itself) would fail anyway, and this should
    // be accounted for anyway.
    MP_STATE_PORT(inotify_events) = m_new(struct inotify_event, MICROPY_PY_GPIO_ALLOCATION_QUEUE_SIZE);
    #endif

    #if MICROPY_PY_GPIO_IRQ
    gpio_epoll_init();
    #endif
    #if MICROPY_PY_GPIO_DYNAMIC_ALLOCATION
    gpio_inotify_init();
    #endif
}

void mp_pin_deinit(void) {
    #if MICROPY_PY_GPIO_DYNAMIC_ALLOCATION
    gpio_inotify_deinit();
    #endif
    #if MICROPY_PY_GPIO_IRQ
    gpio_epoll_deinit();
    #endif

    mp_map_t *ports = &MP_STATE_PORT(ports);
    if (ports->table == NULL) {
        return;
    }

    for (size_t port_index = 0; port_index < ports->alloc; port_index++) {
        if (!mp_map_slot_is_filled(ports, port_index)) {
            continue;
        }

        mp_map_elem_t slot = ports->table[port_index];
        gpio_port_t *port = MP_OBJ_TO_PTR(slot.value);
        close(port->fd);
        if (port->lines > 0) {
            for (size_t pin = 0; pin < port->lines; pin++) {
                if (port->pins[pin] != MP_OBJ_NULL) {
                    machine_pin_del(port->pins[pin]);
                    port->pins[pin] = MP_OBJ_NULL;
                }
            }
            m_del(mp_obj_t, port->pins, port->lines);
            port->pins = NULL;
        }
        assert(port->pins == NULL && "GPIO port count went out of sync");
        m_del_obj(gpio_port_t, port);
        slot.value = MP_OBJ_NULL;
    }
    mp_map_clear(ports);

    // Root pointers will be freed by the OS after this point.  For the Unix
    // port, once the main interpreter loop exits there's no recovery unlike
    // on QEMU or other microcontroller-based ports where the board would just
    // reboot and restart things up.
}

static const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    // instance methods

    { MP_ROM_QSTR(MP_QSTR___del__),     MP_ROM_PTR(&machine_pin_del_obj)             },

    { MP_ROM_QSTR(MP_QSTR_init),        MP_ROM_PTR(&machine_pin_init_obj)            },

    { MP_ROM_QSTR(MP_QSTR_value),       MP_ROM_PTR(&machine_pin_value_obj)           },
    { MP_ROM_QSTR(MP_QSTR_off),         MP_ROM_PTR(&machine_pin_off_obj)             },
    { MP_ROM_QSTR(MP_QSTR_on),          MP_ROM_PTR(&machine_pin_on_obj)              },
    { MP_ROM_QSTR(MP_QSTR_toggle),      MP_ROM_PTR(&machine_pin_toggle_obj)          },

    #if MICROPY_PY_GPIO_IRQ
    { MP_ROM_QSTR(MP_QSTR_irq),         MP_ROM_PTR(&machine_pin_irq_obj)             },
    #endif

    { MP_ROM_QSTR(MP_QSTR_close),       MP_ROM_PTR(&machine_pin_del_obj)             },
    { MP_ROM_QSTR(MP_QSTR_available),   MP_ROM_PTR(&machine_pin_available_obj)       },
    { MP_ROM_QSTR(MP_QSTR___enter__),   MP_ROM_PTR(&mp_identity_obj)                 },
    { MP_ROM_QSTR(MP_QSTR___exit__),    MP_ROM_PTR(&machine_pin_del_obj)             },

    { MP_ROM_QSTR(MP_QSTR_enumerate),   MP_ROM_PTR(&machine_pin_enumerate_obj)       },

    // class constants

    { MP_ROM_QSTR(MP_QSTR_IN),          MP_ROM_INT(GPIO_V2_LINE_FLAG_INPUT)          },
    { MP_ROM_QSTR(MP_QSTR_OUT),         MP_ROM_INT(GPIO_V2_LINE_FLAG_OUTPUT)         },
    { MP_ROM_QSTR(MP_QSTR_OPEN_DRAIN),  MP_ROM_INT(GPIO_V2_LINE_FLAG_OPEN_DRAIN)     },
    { MP_ROM_QSTR(MP_QSTR_PULL_UP),     MP_ROM_INT(GPIO_V2_LINE_FLAG_BIAS_PULL_UP)   },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN),   MP_ROM_INT(GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN) },

    #if MICROPY_PY_GPIO_IRQ
    { MP_ROM_QSTR(MP_QSTR_IRQ_RISING),  MP_ROM_INT(GPIO_V2_LINE_FLAG_EDGE_RISING)    },
    { MP_ROM_QSTR(MP_QSTR_IRQ_FALLING), MP_ROM_INT(GPIO_V2_LINE_FLAG_EDGE_FALLING)   },
    #endif
};

static MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

static const mp_pin_p_t machine_pin_obj_protocol = {
    .ioctl = pin_ioctl,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_pin_type,
    MP_QSTR_Pin,
    MP_TYPE_FLAG_NONE,
    make_new, mp_pin_make_new,
    print, machine_pin_obj_print,
    call, machine_pin_call,
    protocol, &machine_pin_obj_protocol,
    locals_dict, &machine_pin_locals_dict
    );

MP_REGISTER_ROOT_POINTER(mp_map_t ports);

#if MICROPY_PY_GPIO_IRQ
MP_REGISTER_ROOT_POINTER(void *epoll_events);
MP_REGISTER_ROOT_POINTER(mp_set_t epoll_pins);
#endif

#if MICROPY_PY_GPIO_DYNAMIC_ALLOCATION
MP_REGISTER_ROOT_POINTER(void *inotify_events);
#endif

#endif
