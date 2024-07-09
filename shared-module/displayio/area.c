// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-module/displayio/area.h"

#include "py/misc.h"

void displayio_area_copy(const displayio_area_t *src, displayio_area_t *dst) {
    dst->x1 = src->x1;
    dst->y1 = src->y1;
    dst->x2 = src->x2;
    dst->y2 = src->y2;
}

void displayio_area_scale(displayio_area_t *area, uint16_t scale) {
    area->x1 *= scale;
    area->y1 *= scale;
    area->x2 *= scale;
    area->y2 *= scale;
}

void displayio_area_shift(displayio_area_t *area, int16_t dx, int16_t dy) {
    area->x1 += dx;
    area->y1 += dy;
    area->x2 += dx;
    area->y2 += dy;
}

bool displayio_area_compute_overlap(const displayio_area_t *a,
    const displayio_area_t *b,
    displayio_area_t *overlap) {
    overlap->x1 = a->x1;
    if (b->x1 > overlap->x1) {
        overlap->x1 = b->x1;
    }
    overlap->x2 = a->x2;
    if (b->x2 < overlap->x2) {
        overlap->x2 = b->x2;
    }
    if (overlap->x1 >= overlap->x2) {
        return false;
    }
    overlap->y1 = a->y1;
    if (b->y1 > overlap->y1) {
        overlap->y1 = b->y1;
    }
    overlap->y2 = a->y2;
    if (b->y2 < overlap->y2) {
        overlap->y2 = b->y2;
    }
    if (overlap->y1 >= overlap->y2) {
        return false;
    }
    return true;
}

bool displayio_area_empty(const displayio_area_t *a) {
    return (a->x1 == a->x2) || (a->y1 == a->y2);
}

void displayio_area_canon(displayio_area_t *a) {
    if (a->x1 > a->x2) {
        int16_t t = a->x1;
        a->x1 = a->x2;
        a->x2 = t;
    }
    if (a->y1 > a->y2) {
        int16_t t = a->y1;
        a->y1 = a->y2;
        a->y2 = t;
    }
}

void displayio_area_union(const displayio_area_t *a,
    const displayio_area_t *b,
    displayio_area_t *u) {

    if (displayio_area_empty(a)) {
        displayio_area_copy(b, u);
        return;
    }
    if (displayio_area_empty(b)) {
        displayio_area_copy(a, u);
        return;
    }
    u->x1 = MIN(a->x1, b->x1);
    u->y1 = MIN(a->y1, b->y1);
    u->x2 = MAX(a->x2, b->x2);
    u->y2 = MAX(a->y2, b->y2);
}

uint16_t displayio_area_width(const displayio_area_t *area) {
    return area->x2 - area->x1;
}

uint16_t displayio_area_height(const displayio_area_t *area) {
    return area->y2 - area->y1;
}

uint32_t displayio_area_size(const displayio_area_t *area) {
    return displayio_area_width(area) * displayio_area_height(area);
}

bool displayio_area_equal(const displayio_area_t *a, const displayio_area_t *b) {
    return a->x1 == b->x1 &&
           a->y1 == b->y1 &&
           a->x2 == b->x2 &&
           a->y2 == b->y2;
}

// Original and whole must be in the same coordinate space.
void displayio_area_transform_within(bool mirror_x, bool mirror_y, bool transpose_xy,
    const displayio_area_t *original,
    const displayio_area_t *whole,
    displayio_area_t *transformed) {
    if (mirror_x) {
        transformed->x1 = whole->x1 + (whole->x2 - original->x2);
        transformed->x2 = whole->x2 - (original->x1 - whole->x1);
    } else {
        transformed->x1 = original->x1;
        transformed->x2 = original->x2;
    }
    if (mirror_y) {
        transformed->y1 = whole->y1 + (whole->y2 - original->y2);
        transformed->y2 = whole->y2 - (original->y1 - whole->y1);
    } else {
        transformed->y1 = original->y1;
        transformed->y2 = original->y2;
    }
    if (transpose_xy) {
        int16_t y1 = transformed->y1;
        int16_t y2 = transformed->y2;
        transformed->y1 = whole->y1 + (transformed->x1 - whole->x1);
        transformed->y2 = whole->y1 + (transformed->x2 - whole->x1);
        transformed->x2 = whole->x1 + (y2 - whole->y1);
        transformed->x1 = whole->x1 + (y1 - whole->y1);
    }
}
