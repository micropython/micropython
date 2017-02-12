/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2017 Damien P. George
 * Copyright (c) 2014, 2017 Paul Sokolovsky
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

#include <string.h>

#include "py/nlr.h"
#include "py/runtime0.h"
#include "py/runtime.h"
#include "py/objint.h"

#if MICROPY_PY_COLLECTIONS

#define BLOCKLEN 4
#define CENTER ((BLOCKLEN - 1) / 2)

#define NEEDS_TRIM(deque, maxlen) ((size_t)(maxlen) < (size_t)(deque->len))

#ifndef NDEBUG
#define MARK_END(link)  link = NULL;
#define CHECK_END(link) assert(link == NULL);
#define CHECK_NOT_END(link) assert(link != NULL);
#else
#define MARK_END(link)
#define CHECK_END(link)
#define CHECK_NOT_END(link)
#endif

#define MAXFREEBLOCKS (BLOCKLEN / 4)

typedef struct BLOCK {
    struct BLOCK *leftlink;
    mp_obj_t data[BLOCKLEN];
    struct BLOCK *rightlink;
} block_t;

typedef struct _mp_obj_deque_t {
    mp_obj_base_t base;
    block_t *leftblock;
    block_t *rightblock;
    mp_int_t leftindex;         /* 0 <= leftindex < BLOCKLEN */
    mp_int_t rightindex;        /* 0 <= rightindex < BLOCKLEN */
    mp_int_t len;
    mp_int_t maxlen;
} mp_obj_deque_t;

typedef struct _mp_obj_deque_it_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    block_t *b;
    mp_int_t counter;
    mp_int_t index;
} mp_obj_deque_it_t;

STATIC size_t numfreeblocks = 0;
STATIC block_t *freeblocks[MAXFREEBLOCKS];

STATIC block_t *newblock(void) {
    block_t *b = NULL;
    if (numfreeblocks) {
        numfreeblocks--;
        return freeblocks[numfreeblocks];
    }
    b = m_new(block_t, 1);
    if (b == NULL) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_MemoryError, "out of memory"));
    } 
    return b;
}

STATIC void freeblock(block_t *b) {
    if (numfreeblocks < MAXFREEBLOCKS) {
        freeblocks[numfreeblocks] = b;
        numfreeblocks++;
    } else {
        m_del(block_t, b, 1);
    }
}

STATIC mp_obj_t deque_pop_internal(mp_obj_deque_t *deque) {
    block_t *prevblock;
    if (deque->len == 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_IndexError, "pop from empty deque"));
    }
    mp_obj_t item = deque->rightblock->data[deque->rightindex];
    deque->rightindex--;
    deque->len--;
    if (deque->rightindex < 0) {
        if (deque->len) {
            prevblock = deque->rightblock->leftlink;
            assert(deque->leftblock != deque->rightblock);
            freeblock(deque->rightblock);
            CHECK_NOT_END(prevblock);
            MARK_END(prevblock->rightlink);
            deque->rightblock = prevblock;
            deque->rightindex = BLOCKLEN - 1;
        } else {
            assert(deque->leftblock == deque->rightblock);
            assert(deque->leftindex == deque->rightindex+1);
            /* re-center instead of freeing a block */
            deque->leftindex = CENTER + 1;
            deque->rightindex = CENTER;
        }
    }
    return item;
}

STATIC mp_obj_t deque_popleft_internal(mp_obj_deque_t *deque) {
    block_t *prevblock = NULL;
    if (deque->len == 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_IndexError, "pop from empty deque"));
    }
    assert(deque->leftblock != NULL);
    mp_obj_t item = deque->leftblock->data[deque->leftindex];
    deque->leftindex++;
    deque->len--;
    if (deque->leftindex == BLOCKLEN) {
        if (deque->len) {
            assert(deque->leftblock != deque->rightblock);
            prevblock = deque->leftblock->rightlink;
            freeblock(deque->leftblock);
            CHECK_NOT_END(prevblock);
            MARK_END(prevblock->leftlink);
            deque->leftblock = prevblock;
            deque->leftindex = 0;
        } else {
            assert(deque->leftblock == deque->rightblock);
            assert(deque->leftindex == deque->rightindex+1);
            /* re-center instead of freeing a block */
            deque->leftindex = CENTER + 1;
            deque->rightindex = CENTER;
        }
    }
    return item;
}

STATIC int deque_append_internal(mp_obj_deque_t *deque, mp_obj_t item, size_t maxlen) {
    if (deque->rightindex == BLOCKLEN - 1) {
        block_t *b = NULL;
        if ((b = newblock()) == NULL) {
            return -1;
        }
        b->leftlink = deque->rightblock;
        CHECK_END(deque->rightblock->rightlink);
        deque->rightblock->rightlink = b;
        deque->rightblock = b;
        MARK_END(b->rightlink);
        deque->rightindex = -1;
    }
    deque->len++;
    deque->rightindex++;
    deque->rightblock->data[deque->rightindex] = item;
    if (NEEDS_TRIM(deque, maxlen)) {
        deque_popleft_internal(deque);
    }
    return 0;
}

STATIC int deque_appendleft_internal(mp_obj_deque_t *deque, mp_obj_t item, size_t maxlen) {
    if (deque->leftindex == 0) {
        block_t *b = newblock();
        if (b == NULL) {
            return -1;
        }
        b->rightlink = deque->leftblock;
        CHECK_END(deque->leftblock->leftlink);
        deque->leftblock->leftlink = b;
        deque->leftblock = b;
        MARK_END(b->leftlink);
        deque->leftindex = BLOCKLEN;
    }
    deque->len++;
    deque->leftindex--;
    deque->leftblock->data[deque->leftindex] = item;
    if (NEEDS_TRIM(deque, maxlen)) {
        deque_pop_internal(deque);
    }
    return 0;
}

STATIC int _deque_rotate(mp_obj_deque_t *deque, mp_int_t n) {
    block_t *b = NULL;
    block_t *leftblock = deque->leftblock;
    block_t *rightblock = deque->rightblock;
    mp_int_t leftindex = deque->leftindex;
    mp_int_t rightindex = deque->rightindex;
    mp_int_t len=deque->len, halflen=len>>1;
    int rv = -1;
    if (len <= 1) {
        return 0;
    } 
    if (n > halflen || n < -halflen) {
        n %= len;
        if (n > halflen) {
            n -= len;
        } else if (n < -halflen) {
            n += len;
        }
    }
    assert(len > 1);
    assert(-halflen <= n && n <= halflen);
    while (n > 0) {
        if (leftindex == 0) {
            if (b == NULL) {
                b = newblock();
                if (b == NULL) {
                    goto done;
                }
            }
            b->rightlink = leftblock;
            CHECK_END(leftblock->leftlink);
            leftblock->leftlink = b;
            leftblock = b;
            MARK_END(b->leftlink);
            leftindex = BLOCKLEN;
            b = NULL;
        }
        assert(leftindex > 0);
        {
            mp_obj_t *src, *dest;
            mp_int_t m = n;
            if (m > rightindex + 1) {
                m = rightindex + 1; 
            }
            if (m > leftindex) {
                m = leftindex;
            }
            assert (m > 0 && m <= len);
            rightindex -= m;
            leftindex -= m;
            src = &rightblock->data[rightindex + 1];
            dest = &leftblock->data[leftindex];
            n -= m;
            do {
                *(dest++) = *(src++);
            } while (--m);
        }
        if (rightindex < 0) {
            assert(leftblock != rightblock);
            assert(b == NULL);
            b = rightblock;
            CHECK_NOT_END(rightblock->leftlink);
            rightblock = rightblock->leftlink;
            MARK_END(rightblock->rightlink);
            rightindex = BLOCKLEN - 1;
        }
    }
    while (n < 0) {
        if (rightindex == BLOCKLEN - 1) {
            if (b == NULL) {
                b = newblock();
                if (b == NULL) {
                    goto done;
                }
            }
            b->leftlink = rightblock;
            CHECK_END(rightblock->rightlink);
            rightblock->rightlink = b;
            rightblock = b;
            MARK_END(b->rightlink);
            rightindex = -1;
            b = NULL;
        }
        assert (rightindex < BLOCKLEN - 1);
        {
            mp_obj_t *src, *dest;
            mp_int_t m = -n;
            if (m > BLOCKLEN - leftindex) {
                m = BLOCKLEN - leftindex;
            }        
            if (m > BLOCKLEN - 1 - rightindex) {
                m = BLOCKLEN - 1 - rightindex;
            }
            assert (m > 0 && m <= len);
            src = &leftblock->data[leftindex];
            dest = &rightblock->data[rightindex + 1];
            leftindex += m;
            rightindex += m;
            n += m;
            do {
                *(dest++) = *(src++);
            } while (--m);
        }
        if (leftindex == BLOCKLEN) {
            assert(leftblock != rightblock);
            assert(b == NULL);
            b = leftblock;
            CHECK_NOT_END(leftblock->rightlink);
            leftblock = leftblock->rightlink;
            MARK_END(leftblock->leftlink);
            leftindex = 0;
        }
    }
    rv = 0;
done:
    if (b != NULL) {
        freeblock(b); 
    }
    deque->leftblock = leftblock;
    deque->rightblock = rightblock;
    deque->leftindex = leftindex;
    deque->rightindex = rightindex;
    return rv;
}

STATIC bool valid_index(mp_int_t i, mp_int_t limit) {
    /* The cast to size_t lets us use just a single comparison
       to check whether i is in the range: 0 <= i < limit */
    return (size_t) i < (size_t) limit;
}

STATIC int deque_del_item(mp_obj_deque_t *deque, mp_int_t i) {
    int rv;
    assert (i >= 0 && i < deque->len);
    if (_deque_rotate(deque, -i)) {
        return -1;
    }
    deque_popleft_internal(deque);
    rv = _deque_rotate(deque, i);
    return rv;
}

STATIC int deque_ass_item(mp_obj_deque_t *deque, mp_int_t i, mp_obj_t v) {
    block_t *b;
    mp_int_t n, len=deque->len, halflen=(len+1)>>1, index=i;
    if (!valid_index(i, len)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_IndexError, "deque index out of range"));
    }
    if (v == MP_OBJ_NULL) {
        return deque_del_item(deque, i);
    }
    i += deque->leftindex;
    n = (mp_int_t)((size_t) i / BLOCKLEN);
    i = (mp_int_t)((size_t) i % BLOCKLEN);
    if (index <= halflen) {
        b = deque->leftblock;
        n++;
        while (--n) {
            b = b->rightlink;
        }
    } else {
        n = (mp_int_t)(((size_t)(deque->leftindex + deque->len - 1)) / BLOCKLEN - n);
        b = deque->rightblock;
        n++;
        while (--n) {
            b = b->leftlink;
        }
    }
    mp_obj_t old_value = b->data[i];
    (void)old_value;
    b->data[i] = v;
    return 0;
}

STATIC mp_obj_t deque_item(mp_obj_deque_t *deque, mp_int_t i) {
    block_t *b;
    mp_int_t n, index=i;
    if (!valid_index(i, deque->len)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_IndexError, "deque index out of range"));
    }
    if (i == 0) {
        i = deque->leftindex;
        b = deque->leftblock;
    } else if (i == deque->len - 1) {
        i = deque->rightindex;
        b = deque->rightblock;
    } else {
        i += deque->leftindex;
        n = (mp_int_t)((size_t) i / BLOCKLEN);
        i = (mp_int_t)((size_t) i % BLOCKLEN);
        if (index < deque->len >> 1) {
            b = deque->leftblock;
            n++;
            while (--n) {
                b = b->rightlink;
            }
        } else {
            n = (mp_int_t)(((size_t)(deque->leftindex + deque->len - 1)) / BLOCKLEN - n);
            b = deque->rightblock;
            n++;
            while (--n) {
                b = b->leftlink;
            }
        }
    }

    mp_obj_t item = b->data[i];
    return item;
}

STATIC void deque_reverse_internal(mp_obj_deque_t *deque) {
    block_t *leftblock = deque->leftblock;
    block_t *rightblock = deque->rightblock;
    mp_int_t leftindex = deque->leftindex;
    mp_int_t rightindex = deque->rightindex;
    mp_int_t n = deque->len >> 1;
    mp_obj_t tmp;
    n++;
    while (--n) {
        /* Validate that pointers haven't met in the middle */
        assert(leftblock != rightblock || leftindex < rightindex);
        CHECK_NOT_END(leftblock);
        CHECK_NOT_END(rightblock);
        /* Swap */
        tmp = leftblock->data[leftindex];
        leftblock->data[leftindex] = rightblock->data[rightindex];
        rightblock->data[rightindex] = tmp;
        /* Advance left block/index pair */
        leftindex++;
        if (leftindex == BLOCKLEN) {
            leftblock = leftblock->rightlink;
            leftindex = 0;
        }
        /* Step backwards with the right block/index pair */
        rightindex--;
        if (rightindex < 0) {
            rightblock = rightblock->leftlink;
            rightindex = BLOCKLEN - 1;
        }
    }
}

STATIC void deque_insert_internal(mp_obj_deque_t *deque, mp_int_t index, mp_obj_t value) {
    mp_int_t n = deque->len;
    if (deque->maxlen == deque->len) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_IndexError, "deque already at its maximum size"));
    }
    if (index >= n) {
        deque_append_internal(deque, value, deque->maxlen);
        return;
    }
    if (index <= -n || index == 0) {
        deque_appendleft_internal(deque, value, deque->maxlen);
        return;
    }
    if (_deque_rotate(deque, -index)) {
        return;
    }
    mp_int_t rv = -1;
    if (index < 0) {
        rv = deque_append_internal(deque, value, deque->maxlen);
    } else {
        rv = deque_appendleft_internal(deque, value, deque->maxlen);
    }
    if(rv == -1) {
        return;
    }
    if (_deque_rotate(deque, index)) {
        return;
    }
}

STATIC void deque_extend_from_iter(mp_obj_deque_t *deque, mp_obj_t iterable, bool left_to_right) {
    mp_obj_t iter = mp_getiter(iterable);
    mp_obj_t item;
    while ((item = mp_iternext(iter)) != MP_OBJ_STOP_ITERATION) {
        if(left_to_right) {
            deque_append_internal(deque, item, deque->maxlen);
        } else {
            deque_appendleft_internal(deque, item, deque->maxlen);
        }
    }
}

STATIC mp_obj_t deque_it_iternext(mp_obj_t self_in) {
    mp_obj_deque_it_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->counter == 0) {
        return MP_OBJ_STOP_ITERATION;
    }
    mp_obj_t item = self->b->data[self->index];
    self->index++;
    self->counter--;
    if (self->index == BLOCKLEN && self->counter > 0) {
        assert(self->b->rightlink != NULL);
        self->b = self->b->rightlink;
        self->index = 0;
    }
    return item;
}

STATIC mp_obj_t mp_obj_new_deque_iterator(mp_obj_t self_in) {
    mp_obj_deque_it_t *o = m_new_obj(mp_obj_deque_it_t);
    mp_obj_deque_t *deque = MP_OBJ_TO_PTR(self_in);
    o->base.type = &mp_type_polymorph_iter;
    o->iternext = deque_it_iternext;
    o->b = deque->leftblock;
    o->index = deque->leftindex;
    o->counter = deque->len;
    return MP_OBJ_FROM_PTR(o);
}

STATIC void deque_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind) {
    kind = PRINT_REPR;
    mp_obj_deque_t *o = MP_OBJ_TO_PTR(o_in);
    (void)o;
    vstr_t vstr;
    mp_print_t pr;
    vstr_init_print(&vstr, 10, &pr);
    mp_print_str(&pr, "deque([");
    for (mp_int_t i = 0; i < o->len; i++) {
        if (i > 0) {
            mp_print_str(&pr, ", ");
        }
        mp_obj_print_helper(&pr, deque_item(o, i), kind);
    }
    if(o->maxlen >= 0) {
        mp_printf(&pr, "], maxlen=%ld)", o->maxlen);
    } else {
        mp_print_str(&pr, "])");
    }
    print->print_strn(print->data, (const char*)vstr.buf, vstr.len);

    if ((MICROPY_PY_UJSON && kind == PRINT_JSON)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
            "%s is not JSON serializable", vstr.buf));
    }
}

STATIC mp_obj_t deque_new(mp_obj_t iterable, mp_int_t maxlen) {
    mp_obj_deque_t *o = m_new_obj(mp_obj_deque_t);
    o->base.type = &mp_type_deque;
    o->len = 0;
    block_t *b = NULL;
    if ((b = newblock()) == NULL) {
        return mp_const_none;
    }
    MARK_END(b->leftlink);
    MARK_END(b->rightlink);
    o->leftblock = b;
    o->rightblock = b;
    o->leftindex = CENTER + 1;
    o->rightindex = CENTER;
    o->maxlen = maxlen;
    if(iterable != MP_OBJ_NULL) {
        deque_extend_from_iter(o, iterable, true);
    }
    return MP_OBJ_FROM_PTR(o);
}

STATIC mp_obj_t deque_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    (void)type_in;
    mp_arg_check_num(n_args, n_kw, 0, 2, true);
    mp_map_t kw_args;
    const mp_arg_t deque_args[] = {
        { MP_QSTR_iterable, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_maxlen, MP_ARG_INT, {.u_int = -1} },
    };
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    mp_arg_val_t args[MP_ARRAY_SIZE(deque_args)];
    mp_arg_parse_all(n_args, all_args, &kw_args, MP_ARRAY_SIZE(args), deque_args, args);
    return deque_new(args[0].u_obj, args[1].u_int);
}

STATIC mp_obj_t deque_append(mp_obj_t self_in, mp_obj_t arg_in) {
    mp_obj_deque_t *self = MP_OBJ_TO_PTR(self_in);
    deque_append_internal(self, arg_in, self->maxlen);
    return mp_const_none;
}

STATIC mp_obj_t deque_appendleft(mp_obj_t self_in, mp_obj_t arg_in) {
    mp_obj_deque_t *self = MP_OBJ_TO_PTR(self_in);
    deque_appendleft_internal(self, arg_in, self->maxlen);
    return mp_const_none;
}

STATIC mp_obj_t deque_extend(mp_obj_t self_in, mp_obj_t arg_in) {
    mp_obj_deque_t *self = MP_OBJ_TO_PTR(self_in);
    deque_extend_from_iter(self, arg_in, true);
    return mp_const_none;
}

STATIC mp_obj_t deque_extendleft(mp_obj_t self_in, mp_obj_t arg_in) {
    mp_obj_deque_t *self = MP_OBJ_TO_PTR(self_in);
    deque_extend_from_iter(self, arg_in, false);
    return mp_const_none;
}

STATIC mp_obj_t deque_clear(mp_obj_t self_in) {
    mp_obj_t iter = mp_getiter(self_in);
    mp_obj_t item;
    while ((item = mp_iternext(iter)) != MP_OBJ_STOP_ITERATION) {
        deque_del_item(MP_OBJ_TO_PTR(self_in), 0);
    }
    return mp_const_none;
}

STATIC mp_obj_t deque_copy(mp_obj_t self_in) {
    mp_obj_deque_t *self = MP_OBJ_TO_PTR(self_in);
    return deque_new(self_in, self->maxlen);
}

STATIC mp_obj_t deque_count(mp_obj_t self_in, mp_obj_t x) {
    mp_obj_deque_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t count = 0;
    for (mp_int_t i = 0; i < self->len; i++) {
        mp_obj_t y = deque_item(self, i);
        if (y == x) {
            count++;
        }
    }
    return mp_obj_new_int_from_ull(count);
}

STATIC mp_obj_t deque_index(size_t n_args, const mp_obj_t *args) {
    mp_obj_deque_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_obj_t x = args[1];
    mp_int_t start = ( n_args > 2 ? mp_obj_get_int(args[2]) : 0 );
    mp_int_t stop = ( n_args > 3 ? mp_obj_get_int(args[3]) : self->len );
    for (mp_int_t i = start; i < stop; i++) {
        mp_obj_t y = deque_item(self, i);
        if (y == x) {
            return mp_obj_new_int_from_ull(i);
        }
    }
    vstr_t vstr;
    mp_print_t pr;
    vstr_init_print(&vstr, 4, &pr);
    mp_obj_print_helper(&pr, x, PRINT_REPR);
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "%s is not in deque", vstr.buf)); 
}

STATIC mp_obj_t deque_insert(mp_obj_t self_in, mp_obj_t idx, mp_obj_t obj) {
    mp_obj_deque_t *self = MP_OBJ_TO_PTR(self_in);
    deque_insert_internal(self, mp_obj_get_int(idx), obj);
    return mp_const_none;
}

STATIC mp_obj_t deque_pop(mp_obj_t self_in) {
    mp_obj_deque_t *self = MP_OBJ_TO_PTR(self_in);
    return deque_pop_internal(self);
}

STATIC mp_obj_t deque_popleft(mp_obj_t self_in) {
    mp_obj_deque_t *self = MP_OBJ_TO_PTR(self_in);
    return deque_popleft_internal(self);
}

STATIC mp_obj_t deque_remove(mp_obj_t self_in, mp_obj_t value) {
    mp_obj_deque_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t args[2] = {self_in, value};
    deque_del_item(self, mp_obj_get_int(deque_index(2, args)));
    return mp_const_none;
}

STATIC mp_obj_t deque_reverse(mp_obj_t self_in) {
    mp_obj_deque_t *self = MP_OBJ_TO_PTR(self_in);
    deque_reverse_internal(self);
    return mp_const_none;
}

STATIC mp_obj_t deque_rotate(mp_obj_t self_in, mp_obj_t arg_in) {
    mp_obj_deque_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t n = mp_obj_get_int(arg_in);
    _deque_rotate(self, n);
    return mp_const_none;
}

STATIC void deque_attr(mp_obj_t obj, qstr attr, mp_obj_t *dest) {
    if (dest[0] == MP_OBJ_NULL) {
        mp_obj_type_t *type = mp_obj_get_type(obj);
        mp_map_t *locals_map = &type->locals_dict->map;
        mp_map_elem_t *elem = mp_map_lookup(locals_map, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP);
        if (elem != NULL) {
            mp_convert_member_lookup(obj, type, elem->value, dest);
        } else {
            if(attr == MP_QSTR_maxlen) {
                mp_obj_deque_t *self = MP_OBJ_TO_PTR(obj);
                dest[0] = mp_const_none;
                if(self->maxlen > 0) {
                    dest[0] = mp_obj_new_int_from_ll(self->maxlen);
                }
                return;
            }
        }
    }
}

STATIC mp_obj_t deque_binary_op(mp_uint_t op, mp_obj_t lhs, mp_obj_t rhs) {
    //mp_obj_deque_t *o = MP_OBJ_TO_PTR(lhs);
    switch (op) {
        case MP_BINARY_OP_ADD: {
            if (!MP_OBJ_IS_TYPE(rhs, &mp_type_deque)) {
                return MP_OBJ_NULL; // op not supported
            }
            mp_obj_t l =  deque_new(mp_getiter(lhs), -1);
            deque_extend(l, mp_getiter(rhs));
            return l;
        }
        case MP_BINARY_OP_INPLACE_ADD: {
            deque_extend(lhs, rhs);
            return lhs;
        }
        default:
            return MP_OBJ_NULL; // op not supported
    }
}

STATIC mp_obj_t deque_unary_op(mp_uint_t op, mp_obj_t self_in) {
    mp_obj_deque_t *self = MP_OBJ_TO_PTR(self_in);
    switch (op) {
        case MP_UNARY_OP_BOOL: return mp_obj_new_bool(self->len != 0);
        case MP_UNARY_OP_LEN: return MP_OBJ_NEW_SMALL_INT(self->len);
        default: return MP_OBJ_NULL; // op not supported
    }
}

STATIC mp_obj_t deque_subscr(mp_obj_t self_in, mp_obj_t index_in, mp_obj_t value) {
    mp_obj_deque_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t index = mp_obj_get_int(index_in);
    mp_int_t queue_index = ( (index < 0) ? (self->len + index) : index );
    if (value == MP_OBJ_NULL) {
        // delete
        deque_del_item(self, queue_index);
        return mp_const_none;
    } else if (value == MP_OBJ_SENTINEL) {
        // load
        return deque_item(self, queue_index);
    } else {
        // store
        deque_ass_item(self, queue_index, value);
        return mp_const_none;
    }
}

STATIC mp_obj_t deque_getiter(mp_obj_t o_in) {
    return mp_obj_new_deque_iterator(o_in);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(deque_append_obj, deque_append);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(deque_appendleft_obj, deque_appendleft);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(deque_extend_obj, deque_extend);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(deque_extendleft_obj, deque_extendleft);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(deque_clear_obj, deque_clear);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(deque_copy_obj, deque_copy);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(deque_count_obj, deque_count);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(deque_index_obj, 2, 4, deque_index);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(deque_insert_obj, deque_insert);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(deque_pop_obj, deque_pop);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(deque_popleft_obj, deque_popleft);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(deque_remove_obj, deque_remove);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(deque_reverse_obj, deque_reverse);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(deque_rotate_obj, deque_rotate);

STATIC const mp_rom_map_elem_t deque_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_append), MP_ROM_PTR(&deque_append_obj) },
    { MP_ROM_QSTR(MP_QSTR_appendleft), MP_ROM_PTR(&deque_appendleft_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&deque_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_copy), MP_ROM_PTR(&deque_copy_obj) },
    { MP_ROM_QSTR(MP_QSTR_count), MP_ROM_PTR(&deque_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_extend), MP_ROM_PTR(&deque_extend_obj) },
    { MP_ROM_QSTR(MP_QSTR_extendleft), MP_ROM_PTR(&deque_extendleft_obj) },
    { MP_ROM_QSTR(MP_QSTR_index), MP_ROM_PTR(&deque_index_obj) },
    { MP_ROM_QSTR(MP_QSTR_insert), MP_ROM_PTR(&deque_insert_obj) },
    { MP_ROM_QSTR(MP_QSTR_pop), MP_ROM_PTR(&deque_pop_obj) },
    { MP_ROM_QSTR(MP_QSTR_popleft), MP_ROM_PTR(&deque_popleft_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&deque_remove_obj) },
    { MP_ROM_QSTR(MP_QSTR_reverse), MP_ROM_PTR(&deque_reverse_obj) },
    { MP_ROM_QSTR(MP_QSTR_rotate), MP_ROM_PTR(&deque_rotate_obj) },
};

STATIC MP_DEFINE_CONST_DICT(deque_locals_dict, deque_locals_dict_table);

const mp_obj_type_t mp_type_deque = {
    { &mp_type_type },
    .name = MP_QSTR_deque,
    .print = deque_print,
    .make_new = deque_make_new,
    .attr = deque_attr,
    .binary_op = deque_binary_op,
    .unary_op = deque_unary_op,
    .subscr = deque_subscr,
    .getiter = deque_getiter,
    .locals_dict = (mp_obj_dict_t*)&deque_locals_dict,
};

#endif
