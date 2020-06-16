/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright 2020 Sony Semiconductor Solutions Corporation
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

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <arch/chip/gnss.h>

#include "py/runtime.h"

#include "shared-bindings/gnss/GNSS.h"

typedef struct {
    const char* devpath;
    int fd;
} gnss_dev_t;

STATIC gnss_dev_t gnss_dev = {"/dev/gps", -1};

static unsigned long satellitesystem_type_to_bit_fields(gnss_satellitesystem_t system) {
    switch (system) {
        case SATELLITESYSTEM_GPS:
            return CXD56_GNSS_SAT_GPS;
        case SATELLITESYSTEM_GLONASS:
            return CXD56_GNSS_SAT_GLONASS;
        case SATELLITESYSTEM_SBAS:
            return CXD56_GNSS_SAT_SBAS;
        case SATELLITESYSTEM_QZSS_L1CA:
            return CXD56_GNSS_SAT_QZ_L1CA;
        case SATELLITESYSTEM_QZSS_L1S:
            return CXD56_GNSS_SAT_QZ_L1S;
        case SATELLITESYSTEM_NONE:
        default:
            return CXD56_GNSS_SAT_NONE;
    }
}

static gnss_positionfix_t fix_to_positionfix_type(uint8_t fix) {
    switch (fix) {
        case CXD56_GNSS_PVT_POSFIX_2D:
            return POSITIONFIX_2D;
        case CXD56_GNSS_PVT_POSFIX_3D:
            return POSITIONFIX_3D;
        case CXD56_GNSS_PVT_POSFIX_INVALID:
        default:
            return POSITIONFIX_INVALID;
    }
}

void common_hal_gnss_construct(gnss_obj_t *self) {
    if (gnss_dev.fd < 0) {
        gnss_dev.fd = open(gnss_dev.devpath, O_RDONLY);
        if (gnss_dev.fd < 0) {
            mp_raise_ValueError(translate("Could not initialize GNSS"));
        }
    }

    self->satellite_system = 0;
    self->fix = POSITIONFIX_INVALID;
}

void common_hal_gnss_deinit(gnss_obj_t *self) {
    if (common_hal_gnss_deinited(self)) {
        return;
    }

    close(gnss_dev.fd);
    gnss_dev.fd = -1;
}

bool common_hal_gnss_deinited(gnss_obj_t *self) {
    return gnss_dev.fd < 0;
}

void common_hal_gnss_select(gnss_obj_t *self, gnss_satellitesystem_t system) {
    unsigned long selection = self->satellite_system | satellitesystem_type_to_bit_fields(system);

    ioctl(gnss_dev.fd, CXD56_GNSS_IOCTL_SELECT_SATELLITE_SYSTEM, selection);

    self->satellite_system = selection;
}

void common_hal_gnss_deselect(gnss_obj_t *self, gnss_satellitesystem_t system) {
    unsigned long selection = self->satellite_system & ~satellitesystem_type_to_bit_fields(system);

    ioctl(gnss_dev.fd, CXD56_GNSS_IOCTL_SELECT_SATELLITE_SYSTEM, selection);

    self->satellite_system = selection;
}

void common_hal_gnss_start(gnss_obj_t *self) {
    if (self->satellite_system == 0) {
        mp_raise_ValueError(translate("Cannot start without selecting at least one satellite system."));
    }
    ioctl(gnss_dev.fd, CXD56_GNSS_IOCTL_START, CXD56_GNSS_STMOD_COLD);
}

void common_hal_gnss_stop(gnss_obj_t *self) {
    ioctl(gnss_dev.fd, CXD56_GNSS_IOCTL_STOP, 0);
}

void common_hal_gnss_update(gnss_obj_t *self) {
    struct cxd56_gnss_positiondata_s positiondata;

    read(gnss_dev.fd, &positiondata, sizeof(struct cxd56_gnss_positiondata_s));

    if (positiondata.receiver.pos_dataexist) {
        self->fix = positiondata.receiver.pos_fixmode;
        self->latitude = positiondata.receiver.latitude;
        self->longitude = positiondata.receiver.longitude;
        self->altitude = positiondata.receiver.altitude;
    }
}

mp_float_t common_hal_gnss_get_latitude(gnss_obj_t *self) {
    return (mp_float_t) self->latitude;
}

mp_float_t common_hal_gnss_get_longitude(gnss_obj_t *self) {
    return (mp_float_t) self->longitude;
}

mp_float_t common_hal_gnss_get_altitude(gnss_obj_t *self) {
    return (mp_float_t) self->altitude;
}

gnss_positionfix_t common_hal_gnss_get_fix(gnss_obj_t *self) {
    return fix_to_positionfix_type(self->fix);
}
