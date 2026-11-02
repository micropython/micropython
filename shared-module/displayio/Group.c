// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/displayio/Group.h"

#include "py/runtime.h"
#include "py/objlist.h"
#include "shared-bindings/displayio/TileGrid.h"

#if CIRCUITPY_VECTORIO || CIRCUITPY_VIDEOCORE
#include "shared-bindings/vectorio/VectorShape.h"
#endif

static void check_readonly(displayio_group_t *self) {
    if (self->readonly) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Read-only"));
    }
}

void common_hal_displayio_group_construct(displayio_group_t *self, uint32_t scale, mp_int_t x, mp_int_t y) {
    mp_obj_list_t *members = mp_obj_new_list(0, NULL);
    displayio_group_construct(self, members, scale, x, y);
}

bool common_hal_displayio_group_get_hidden(displayio_group_t *self) {
    return self->hidden;
}

void common_hal_displayio_group_set_hidden(displayio_group_t *self, bool hidden) {
    if (self->hidden == hidden) {
        return;
    }
    check_readonly(self);
    self->hidden = hidden;
    if (self->hidden_by_parent) {
        return;
    }
    for (size_t i = 0; i < self->members->len; i++) {
        mp_obj_t layer;
        layer = mp_obj_cast_to_native_base(
            self->members->items[i], &displayio_tilegrid_type);
        if (layer != MP_OBJ_NULL) {
            displayio_tilegrid_set_hidden_by_parent(layer, hidden);
            continue;
        }
        layer = mp_obj_cast_to_native_base(
            self->members->items[i], &displayio_group_type);
        if (layer != MP_OBJ_NULL) {
            displayio_group_set_hidden_by_parent(layer, hidden);
            continue;
        }
        #if CIRCUITPY_VECTORIO || CIRCUITPY_VIDEOCORE
        const vectorio_draw_protocol_t *draw_protocol = mp_proto_get(MP_QSTR_protocol_draw, self->members->items[i]);
        if (draw_protocol != NULL) {
            layer = draw_protocol->draw_get_protocol_self(self->members->items[i]);
            if (draw_protocol->draw_protocol_impl->draw_set_hidden != NULL) {
                draw_protocol->draw_protocol_impl->draw_set_hidden(layer, hidden);
            } else {
                draw_protocol->draw_protocol_impl->draw_set_dirty(layer);
            }
            continue;
        }
        #endif
    }
}

void displayio_group_set_hidden_by_parent(displayio_group_t *self, bool hidden) {
    if (self->hidden_by_parent == hidden) {
        return;
    }
    check_readonly(self);
    self->hidden_by_parent = hidden;
    // If we're already hidden, then we're done.
    if (self->hidden) {
        return;
    }
    for (size_t i = 0; i < self->members->len; i++) {
        mp_obj_t layer;
        layer = mp_obj_cast_to_native_base(
            self->members->items[i], &displayio_tilegrid_type);
        if (layer != MP_OBJ_NULL) {
            displayio_tilegrid_set_hidden_by_parent(layer, hidden);
            continue;
        }
        layer = mp_obj_cast_to_native_base(
            self->members->items[i], &displayio_group_type);
        if (layer != MP_OBJ_NULL) {
            displayio_group_set_hidden_by_parent(layer, hidden);
            continue;
        }
        #if CIRCUITPY_VECTORIO || CIRCUITPY_VIDEOCORE
        const vectorio_draw_protocol_t *draw_protocol = mp_proto_get(MP_QSTR_protocol_draw, self->members->items[i]);
        if (draw_protocol != NULL) {
            layer = draw_protocol->draw_get_protocol_self(self->members->items[i]);
            if (draw_protocol->draw_protocol_impl->draw_set_hidden != NULL) {
                draw_protocol->draw_protocol_impl->draw_set_hidden(layer, hidden);
            } else {
                draw_protocol->draw_protocol_impl->draw_set_dirty(layer);
            }
            continue;
        }
        #endif
    }
}

uint32_t common_hal_displayio_group_get_scale(displayio_group_t *self) {
    return self->scale;
}

bool displayio_group_get_previous_area(displayio_group_t *self, displayio_area_t *area) {
    bool first = true;
    for (size_t i = 0; i < self->members->len; i++) {
        mp_obj_t layer;
        displayio_area_t layer_area;
        layer = mp_obj_cast_to_native_base(
            self->members->items[i], &displayio_tilegrid_type);
        if (layer != MP_OBJ_NULL) {
            if (!displayio_tilegrid_get_previous_area(layer, &layer_area)) {
                continue;
            }
        } else {
            layer = mp_obj_cast_to_native_base(
                self->members->items[i], &displayio_group_type);
            if (layer != MP_OBJ_NULL) {
                if (!displayio_group_get_previous_area(layer, &layer_area)) {
                    continue;
                }
            }
        }
        if (first) {
            displayio_area_copy(&layer_area, area);
            first = false;
        } else {
            displayio_area_union(area, &layer_area, area);
        }
    }
    if (self->item_removed) {
        if (first) {
            displayio_area_copy(&self->dirty_area, area);
            first = false;
        } else {
            displayio_area_union(area, &self->dirty_area, area);
        }
    }
    return !first;
}

static void _update_child_transforms(displayio_group_t *self) {
    // Draw protocol layers get NULL while inactive. TileGrids and nested
    // Groups always get the (zeroed when inactive) transform, since it also
    // tracks their group membership.
    #if CIRCUITPY_VECTORIO || CIRCUITPY_VIDEOCORE
    displayio_buffer_transform_t *draw_transform =
        self->active ? &self->absolute_transform : NULL;
    #endif
    for (size_t i = 0; i < self->members->len; i++) {
        mp_obj_t layer;
        #if CIRCUITPY_VECTORIO || CIRCUITPY_VIDEOCORE
        const vectorio_draw_protocol_t *draw_protocol = mp_proto_get(MP_QSTR_protocol_draw, self->members->items[i]);
        if (draw_protocol != NULL) {
            layer = draw_protocol->draw_get_protocol_self(self->members->items[i]);
            draw_protocol->draw_protocol_impl->draw_update_transform(layer, draw_transform);
            continue;
        }
        #endif
        layer = mp_obj_cast_to_native_base(
            self->members->items[i], &displayio_tilegrid_type);
        if (layer != MP_OBJ_NULL) {
            displayio_tilegrid_update_transform(layer, &self->absolute_transform);
            continue;
        }
        layer = mp_obj_cast_to_native_base(
            self->members->items[i], &displayio_group_type);
        if (layer != MP_OBJ_NULL) {
            displayio_group_update_transform(layer, &self->absolute_transform);
            continue;
        }
    }
}

void displayio_group_update_transform(displayio_group_t *self,
    const displayio_buffer_transform_t *parent_transform) {
    self->active = parent_transform != NULL && parent_transform->scale != 0;
    if (self->active) {
        int16_t x = self->x;
        int16_t y = self->y;
        if (parent_transform->transpose_xy) {
            x = y;
            y = self->x;
        }
        self->absolute_transform.x = parent_transform->x + parent_transform->dx * x;
        self->absolute_transform.y = parent_transform->y + parent_transform->dy * y;
        self->absolute_transform.dx = parent_transform->dx * self->scale;
        self->absolute_transform.dy = parent_transform->dy * self->scale;
        self->absolute_transform.transpose_xy = parent_transform->transpose_xy;
        self->absolute_transform.mirror_x = parent_transform->mirror_x;
        self->absolute_transform.mirror_y = parent_transform->mirror_y;

        self->absolute_transform.scale = parent_transform->scale * self->scale;
    } else {
        // Clear the transform so inactive layers can't act on a stale one.
        displayio_buffer_transform_t no_transform = {};
        self->absolute_transform = no_transform;
    }
    _update_child_transforms(self);
}

void common_hal_displayio_group_set_scale(displayio_group_t *self, uint32_t scale) {
    if (self->scale == scale) {
        return;
    }
    check_readonly(self);
    uint8_t parent_scale = self->absolute_transform.scale / self->scale;
    self->absolute_transform.dx = self->absolute_transform.dx / self->scale * scale;
    self->absolute_transform.dy = self->absolute_transform.dy / self->scale * scale;
    self->absolute_transform.scale = parent_scale * scale;
    self->scale = scale;
    _update_child_transforms(self);
}

mp_int_t common_hal_displayio_group_get_x(displayio_group_t *self) {
    return self->x;
}

void common_hal_displayio_group_set_x(displayio_group_t *self, mp_int_t x) {
    if (self->x == x) {
        return;
    }
    check_readonly(self);
    if (self->absolute_transform.transpose_xy) {
        int16_t dy = self->absolute_transform.dy / self->scale;
        self->absolute_transform.y += dy * (x - self->x);
    } else {
        int16_t dx = self->absolute_transform.dx / self->scale;
        self->absolute_transform.x += dx * (x - self->x);
    }

    self->x = x;
    _update_child_transforms(self);
}

mp_int_t common_hal_displayio_group_get_y(displayio_group_t *self) {
    return self->y;
}

void common_hal_displayio_group_set_y(displayio_group_t *self, mp_int_t y) {
    if (self->y == y) {
        return;
    }
    check_readonly(self);
    if (self->absolute_transform.transpose_xy) {
        int8_t dx = self->absolute_transform.dx / self->scale;
        self->absolute_transform.x += dx * (y - self->y);
    } else {
        int8_t dy = self->absolute_transform.dy / self->scale;
        self->absolute_transform.y += dy * (y - self->y);
    }
    self->y = y;
    _update_child_transforms(self);
}

static void _add_layer(displayio_group_t *self, mp_obj_t layer) {
    check_readonly(self);
    #if CIRCUITPY_VECTORIO || CIRCUITPY_VIDEOCORE
    const vectorio_draw_protocol_t *draw_protocol = mp_proto_get(MP_QSTR_protocol_draw, layer);
    if (draw_protocol != NULL) {
        mp_obj_t protocol_self = draw_protocol->draw_get_protocol_self(layer);
        if (draw_protocol->draw_protocol_impl->draw_set_in_group(protocol_self, true)) {
            mp_raise_ValueError(MP_ERROR_TEXT("Layer already in a group"));
        }
        draw_protocol->draw_protocol_impl->draw_update_transform(
            protocol_self, self->active ? &self->absolute_transform : NULL);
        return;
    }
    #endif
    mp_obj_t native_layer = mp_obj_cast_to_native_base(layer, &displayio_tilegrid_type);
    if (native_layer != MP_OBJ_NULL) {
        displayio_tilegrid_t *tilegrid = native_layer;
        if (tilegrid->in_group) {
            mp_raise_ValueError(MP_ERROR_TEXT("Layer already in a group"));
        } else {
            tilegrid->in_group = true;
        }
        displayio_tilegrid_update_transform(tilegrid, &self->absolute_transform);
        displayio_tilegrid_set_hidden_by_parent(
            tilegrid, self->hidden || self->hidden_by_parent);
        return;
    }
    native_layer = mp_obj_cast_to_native_base(layer, &displayio_group_type);
    if (native_layer != MP_OBJ_NULL) {
        displayio_group_t *group = native_layer;
        if (group->in_group) {
            mp_raise_ValueError(MP_ERROR_TEXT("Layer already in a group"));
        } else {
            group->in_group = true;
        }
        displayio_group_update_transform(group, &self->absolute_transform);
        displayio_group_set_hidden_by_parent(
            group, self->hidden || self->hidden_by_parent);
        return;
    }
    mp_raise_ValueError(MP_ERROR_TEXT("Layer must be a Group or TileGrid subclass"));
}

static void _remove_layer(displayio_group_t *self, size_t index) {
    check_readonly(self);
    mp_obj_t layer;
    displayio_area_t layer_area;
    bool rendered_last_frame = false;
    #if CIRCUITPY_VECTORIO || CIRCUITPY_VIDEOCORE
    const vectorio_draw_protocol_t *draw_protocol = mp_proto_get(MP_QSTR_protocol_draw, self->members->items[index]);
    if (draw_protocol != NULL) {
        layer = draw_protocol->draw_get_protocol_self(self->members->items[index]);
        bool has_dirty_area = draw_protocol->draw_protocol_impl->draw_get_dirty_area(layer, &layer_area);
        rendered_last_frame = has_dirty_area;
        draw_protocol->draw_protocol_impl->draw_update_transform(layer, NULL);
        draw_protocol->draw_protocol_impl->draw_set_in_group(layer, false);
    }
    #endif
    layer = mp_obj_cast_to_native_base(
        self->members->items[index], &displayio_tilegrid_type);
    if (layer != MP_OBJ_NULL) {
        displayio_tilegrid_t *tilegrid = layer;
        tilegrid->in_group = false;
        rendered_last_frame = displayio_tilegrid_get_previous_area(tilegrid, &layer_area);
        displayio_tilegrid_update_transform(tilegrid, NULL);
    }
    layer = mp_obj_cast_to_native_base(
        self->members->items[index], &displayio_group_type);
    if (layer != MP_OBJ_NULL) {
        displayio_group_t *group = layer;
        group->in_group = false;
        rendered_last_frame = displayio_group_get_previous_area(group, &layer_area);
        displayio_group_update_transform(group, NULL);
    }
    if (!rendered_last_frame) {
        return;
    }
    if (!self->item_removed) {
        displayio_area_copy(&layer_area, &self->dirty_area);
    } else {
        displayio_area_union(&self->dirty_area, &layer_area, &self->dirty_area);
    }
    self->item_removed = true;
}

void common_hal_displayio_group_insert(displayio_group_t *self, size_t index, mp_obj_t layer) {
    _add_layer(self, layer);
    mp_obj_list_insert(self->members, index, layer);
}

mp_obj_t common_hal_displayio_group_pop(displayio_group_t *self, size_t index) {
    _remove_layer(self, index);
    return mp_obj_list_pop(self->members, index);
}

mp_int_t common_hal_displayio_group_index(displayio_group_t *self, mp_obj_t layer) {
    mp_obj_t args[] = {self->members, layer};
    mp_obj_t *index = mp_seq_index_obj(
        self->members->items, self->members->len, 2, args);
    return MP_OBJ_SMALL_INT_VALUE(index);
}

size_t common_hal_displayio_group_get_len(displayio_group_t *self) {
    return self->members->len;
}

mp_obj_t common_hal_displayio_group_get(displayio_group_t *self, size_t index) {
    return self->members->items[index];
}

void common_hal_displayio_group_set(displayio_group_t *self, size_t index, mp_obj_t layer) {
    _add_layer(self, layer);
    _remove_layer(self, index);
    mp_obj_list_store(self->members, MP_OBJ_NEW_SMALL_INT(index), layer);
}

void displayio_group_construct(displayio_group_t *self, mp_obj_list_t *members, uint32_t scale, mp_int_t x, mp_int_t y) {
    self->x = x;
    self->y = y;
    self->members = members;
    self->item_removed = false;
    self->scale = scale;
    self->in_group = false;
    self->active = false;
    self->readonly = false;
}

bool displayio_group_fill_area(displayio_group_t *self, const _displayio_colorspace_t *colorspace, const displayio_area_t *area, uint32_t *mask, uint32_t *buffer) {
    // Track if any of the layers finishes filling in the given area. We can ignore any remaining
    // layers at that point.
    if (self->hidden == false) {
        for (int32_t i = self->members->len - 1; i >= 0; i--) {
            mp_obj_t layer;
            #if CIRCUITPY_VECTORIO || CIRCUITPY_VIDEOCORE
            const vectorio_draw_protocol_t *draw_protocol = mp_proto_get(MP_QSTR_protocol_draw, self->members->items[i]);
            if (draw_protocol != NULL) {
                layer = draw_protocol->draw_get_protocol_self(self->members->items[i]);
                if (draw_protocol->draw_protocol_impl->draw_fill_area(layer, colorspace, area, mask, buffer)) {
                    return true;
                }
                continue;
            }
            #endif
            layer = mp_obj_cast_to_native_base(
                self->members->items[i], &displayio_tilegrid_type);
            if (layer != MP_OBJ_NULL) {
                if (displayio_tilegrid_fill_area(layer, colorspace, area, mask, buffer)) {
                    return true;
                }
                continue;
            }
            layer = mp_obj_cast_to_native_base(
                self->members->items[i], &displayio_group_type);
            if (layer != MP_OBJ_NULL) {
                if (displayio_group_fill_area(layer, colorspace, area, mask, buffer)) {
                    return true;
                }
                continue;
            }
        }
    }
    return false;
}

void displayio_group_finish_refresh(displayio_group_t *self) {
    self->item_removed = false;
    for (int32_t i = self->members->len - 1; i >= 0; i--) {
        mp_obj_t layer;
        #if CIRCUITPY_VECTORIO || CIRCUITPY_VIDEOCORE
        const vectorio_draw_protocol_t *draw_protocol = mp_proto_get(MP_QSTR_protocol_draw, self->members->items[i]);
        if (draw_protocol != NULL) {
            layer = draw_protocol->draw_get_protocol_self(self->members->items[i]);
            draw_protocol->draw_protocol_impl->draw_finish_refresh(layer);
            continue;
        }
        #endif
        layer = mp_obj_cast_to_native_base(
            self->members->items[i], &displayio_tilegrid_type);
        if (layer != MP_OBJ_NULL) {
            displayio_tilegrid_finish_refresh(layer);
            continue;
        }
        layer = mp_obj_cast_to_native_base(
            self->members->items[i], &displayio_group_type);
        if (layer != MP_OBJ_NULL) {
            displayio_group_finish_refresh(layer);
            continue;
        }
    }
}

static displayio_area_t *group_get_refresh_areas_impl(displayio_group_t *self, displayio_area_t *tail) {
    if (self->item_removed) {
        self->dirty_area.next = tail;
        tail = &self->dirty_area;
    }

    for (int32_t i = self->members->len - 1; i >= 0; i--) {
        mp_obj_t layer;
        #if CIRCUITPY_VECTORIO || CIRCUITPY_VIDEOCORE
        const vectorio_draw_protocol_t *draw_protocol = mp_proto_get(MP_QSTR_protocol_draw, self->members->items[i]);
        if (draw_protocol != NULL) {
            layer = draw_protocol->draw_get_protocol_self(self->members->items[i]);
            tail = draw_protocol->draw_protocol_impl->draw_get_refresh_areas(layer, tail);
            continue;
        }
        #endif
        layer = mp_obj_cast_to_native_base(
            self->members->items[i], &displayio_tilegrid_type);
        if (layer != MP_OBJ_NULL) {
            if (!displayio_tilegrid_get_rendered_hidden(layer)) {
                tail = displayio_tilegrid_get_refresh_areas(layer, tail);
            }
            continue;
        }
        layer = mp_obj_cast_to_native_base(
            self->members->items[i], &displayio_group_type);
        if (layer != MP_OBJ_NULL) {
            tail = group_get_refresh_areas_impl(layer, tail);
            continue;
        }
    }

    return tail;
}

// Filter the dirty-rect list by containment: unlink every area that is fully contained
// in another area in the list (a changing text label is the common case - the group
// dirties the whole label region via item_removed plus one area per glyph, and the
// glyph areas all sit inside the label region). Dropping a covered area is lossless:
// the same pixels are refreshed by the survivor, in fewer transfers.
//
// The nodes are OWNED BY THE ITEMS (TileGrid reuses dirty_area as its tile-space dirty
// accumulator between frames, vectorio keeps its current_area, ...), so their
// coordinates must never be modified here. The ONLY mutation is the .next relink,
// which is safe because every provider unconditionally reassigns .next at link time on
// every get_refresh_areas call and never reads it back.
//
// Nodes at and past `tail` belong to the caller, so the sweep uses `tail` as its end
// sentinel and never looks past it (every in-tree caller passes tail == NULL).
//
// One pass suffices: coordinates never change and unlinking preserves the relative
// order of the remaining nodes, so every pair of surviving nodes is examined. Equal
// areas satisfy the containment test in both directions; testing b-inside-a FIRST
// keeps the earlier node and drops the later one - the two tests must stay an else-if
// chain or equal areas would drop BOTH nodes and lose the area entirely.
static displayio_area_t *filter_out_redundant_areas(displayio_area_t *head,
    const displayio_area_t *tail) {
    displayio_area_t *a_prev = NULL;
    displayio_area_t *a = head;
    while (a != tail) {
        // The casts from the const .next field are safe: every node before `tail` is a
        // mutable item-owned struct; only the field's declared type is pointer-to-const.
        displayio_area_t *b_prev = a;
        displayio_area_t *b = (displayio_area_t *)a->next;
        bool a_dropped = false;
        while (b != tail) {
            if (b->x1 >= a->x1 && b->y1 >= a->y1 && b->x2 <= a->x2 && b->y2 <= a->y2) {
                // b is contained in a (equal areas land here): unlink b.
                b_prev->next = b->next;
                b = (displayio_area_t *)b_prev->next;
            } else if (a->x1 >= b->x1 && a->y1 >= b->y1 && a->x2 <= b->x2 && a->y2 <= b->y2) {
                // a is strictly contained in the later b: unlink a and move on.
                if (a_prev == NULL) {
                    head = (displayio_area_t *)a->next;
                } else {
                    a_prev->next = a->next;
                }
                a_dropped = true;
                break;
            } else {
                b_prev = b;
                b = (displayio_area_t *)b->next;
            }
        }
        if (a_dropped) {
            a = (a_prev == NULL) ? head : (displayio_area_t *)a_prev->next;
        } else {
            a_prev = a;
            a = (displayio_area_t *)a->next;
        }
    }
    return head;
}

// Public entry point: displays call this once on their root group, so the whole tree's
// list is filtered in one place and no display type needs its own copy of the logic.
displayio_area_t *displayio_group_get_refresh_areas(displayio_group_t *self, displayio_area_t *tail) {
    displayio_area_t *areas = group_get_refresh_areas_impl(self, tail);
    return filter_out_redundant_areas(areas, tail);
}
