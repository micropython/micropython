/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "runtime0.h"
#include "runtime.h"
#include "objlist.h"

STATIC mp_obj_t mp_obj_new_list_iterator(mp_obj_list_t *list, int cur);
STATIC mp_obj_list_t *list_new(uint n);
STATIC mp_obj_t list_extend(mp_obj_t self_in, mp_obj_t arg_in);
STATIC mp_obj_t list_pop(uint n_args, const mp_obj_t *args);

// TODO: Move to mpconfig.h
#define LIST_MIN_ALLOC 4

/******************************************************************************/
/* list                                                                       */

STATIC void list_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in, mp_print_kind_t kind) {
    mp_obj_list_t *o = o_in;
    print(env, "[");
    for (int i = 0; i < o->len; i++) {
        if (i > 0) {
            print(env, ", ");
        }
        mp_obj_print_helper(print, env, o->items[i], PRINT_REPR);
    }
    print(env, "]");
}

STATIC mp_obj_t list_extend_from_iter(mp_obj_t list, mp_obj_t iterable) {
    mp_obj_t iter = mp_getiter(iterable);
    mp_obj_t item;
    while ((item = mp_iternext(iter)) != MP_OBJ_STOP_ITERATION) {
        mp_obj_list_append(list, item);
    }
    return list;
}

STATIC mp_obj_t list_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);

    switch (n_args) {
        case 0:
            // return a new, empty list
            return mp_obj_new_list(0, NULL);

        case 1:
        default: {
            // make list from iterable
            // TODO: optimize list/tuple
            mp_obj_t list = mp_obj_new_list(0, NULL);
            return list_extend_from_iter(list, args[0]);
        }
    }
}

// Don't pass MP_BINARY_OP_NOT_EQUAL here
STATIC bool list_cmp_helper(int op, mp_obj_t self_in, mp_obj_t another_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_list));
    if (!MP_OBJ_IS_TYPE(another_in, &mp_type_list)) {
        return false;
    }
    mp_obj_list_t *self = self_in;
    mp_obj_list_t *another = another_in;

    return mp_seq_cmp_objs(op, self->items, self->len, another->items, another->len);
}

STATIC mp_obj_t list_unary_op(int op, mp_obj_t self_in) {
    mp_obj_list_t *self = self_in;
    switch (op) {
        case MP_UNARY_OP_BOOL: return MP_BOOL(self->len != 0);
        case MP_UNARY_OP_LEN: return MP_OBJ_NEW_SMALL_INT(self->len);
        default: return MP_OBJ_NULL; // op not supported
    }
}

STATIC mp_obj_t list_binary_op(int op, mp_obj_t lhs, mp_obj_t rhs) {
    mp_obj_list_t *o = lhs;
    switch (op) {
        case MP_BINARY_OP_ADD: {
            if (!MP_OBJ_IS_TYPE(rhs, &mp_type_list)) {
                return MP_OBJ_NULL; // op not supported
            }
            mp_obj_list_t *p = rhs;
            mp_obj_list_t *s = list_new(o->len + p->len);
            mp_seq_cat(s->items, o->items, o->len, p->items, p->len, mp_obj_t);
            return s;
        }
        case MP_BINARY_OP_INPLACE_ADD: {
            if (!MP_OBJ_IS_TYPE(rhs, &mp_type_list)) {
                return MP_OBJ_NULL; // op not supported
            }
            list_extend(lhs, rhs);
            return o;
        }
        case MP_BINARY_OP_MULTIPLY: {
            machine_int_t n;
            if (!mp_obj_get_int_maybe(rhs, &n)) {
                return MP_OBJ_NULL; // op not supported
            }
            mp_obj_list_t *s = list_new(o->len * n);
            mp_seq_multiply(o->items, sizeof(*o->items), o->len, n, s->items);
            return s;
        }
        case MP_BINARY_OP_EQUAL:
        case MP_BINARY_OP_LESS:
        case MP_BINARY_OP_LESS_EQUAL:
        case MP_BINARY_OP_MORE:
        case MP_BINARY_OP_MORE_EQUAL:
            return MP_BOOL(list_cmp_helper(op, lhs, rhs));

        default:
            return MP_OBJ_NULL; // op not supported
    }
}

STATIC mp_obj_t list_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    if (value == MP_OBJ_NULL) {
        // delete
#if MICROPY_PY_SLICE
        if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
            mp_obj_list_t *self = self_in;
            mp_bound_slice_t slice;
            if (!mp_seq_get_fast_slice_indexes(self->len, index, &slice)) {
                assert(0);
            }

            int len_adj = slice.start - slice.stop;
            //printf("Len adj: %d\n", len_adj);
            assert(len_adj <= 0);
            mp_seq_replace_slice_no_grow(self->items, self->len, slice.start, slice.stop, self->items/*NULL*/, 0, mp_obj_t);
            // Clear "freed" elements at the end of list
            mp_seq_clear(self->items, self->len + len_adj, self->len, sizeof(*self->items));
            self->len += len_adj;
            return mp_const_none;
        }
#endif
        mp_obj_t args[2] = {self_in, index};
        list_pop(2, args);
        return mp_const_none;
    } else if (value == MP_OBJ_SENTINEL) {
        // load
        mp_obj_list_t *self = self_in;
#if MICROPY_PY_SLICE
        if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
            mp_bound_slice_t slice;
            if (!mp_seq_get_fast_slice_indexes(self->len, index, &slice)) {
                return mp_seq_extract_slice(self->len, self->items, &slice);
            }
            mp_obj_list_t *res = list_new(slice.stop - slice.start);
            mp_seq_copy(res->items, self->items + slice.start, res->len, mp_obj_t);
            return res;
        }
#endif
        uint index_val = mp_get_index(self->base.type, self->len, index, false);
        return self->items[index_val];
    } else {
#if MICROPY_PY_SLICE
        if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
            mp_obj_list_t *self = self_in;
            assert(MP_OBJ_IS_TYPE(value, &mp_type_list));
            mp_obj_list_t *slice = value;
            mp_bound_slice_t slice_out;
            if (!mp_seq_get_fast_slice_indexes(self->len, index, &slice_out)) {
                assert(0);
            }
            int len_adj = slice->len - (slice_out.stop - slice_out.start);
            //printf("Len adj: %d\n", len_adj);
            if (len_adj > 0) {
                if (self->len + len_adj > self->alloc) {
                    // TODO: Might optimize memory copies here by checking if block can
                    // be grown inplace or not
                    self->items = m_renew(mp_obj_t, self->items, self->alloc, self->len + len_adj);
                    self->alloc = self->len + len_adj;
                }
                mp_seq_replace_slice_grow_inplace(self->items, self->len,
                    slice_out.start, slice_out.stop, slice->items, slice->len, len_adj, mp_obj_t);
            } else {
                mp_seq_replace_slice_no_grow(self->items, self->len,
                    slice_out.start, slice_out.stop, slice->items, slice->len, mp_obj_t);
                // Clear "freed" elements at the end of list
                mp_seq_clear(self->items, self->len + len_adj, self->len, sizeof(*self->items));
                // TODO: apply allocation policy re: alloc_size
            }
            self->len += len_adj;
            return mp_const_none;
        }
#endif
        mp_obj_list_store(self_in, index, value);
        return mp_const_none;
    }
}

STATIC mp_obj_t list_getiter(mp_obj_t o_in) {
    return mp_obj_new_list_iterator(o_in, 0);
}

mp_obj_t mp_obj_list_append(mp_obj_t self_in, mp_obj_t arg) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_list));
    mp_obj_list_t *self = self_in;
    if (self->len >= self->alloc) {
        self->items = m_renew(mp_obj_t, self->items, self->alloc, self->alloc * 2);
        self->alloc *= 2;
        mp_seq_clear(self->items, self->len + 1, self->alloc, sizeof(*self->items));
    }
    self->items[self->len++] = arg;
    return mp_const_none; // return None, as per CPython
}

STATIC mp_obj_t list_extend(mp_obj_t self_in, mp_obj_t arg_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_list));
    if (MP_OBJ_IS_TYPE(arg_in, &mp_type_list)) {
        mp_obj_list_t *self = self_in;
        mp_obj_list_t *arg = arg_in;

        if (self->len + arg->len > self->alloc) {
            // TODO: use alloc policy for "4"
            self->items = m_renew(mp_obj_t, self->items, self->alloc, self->len + arg->len + 4);
            self->alloc = self->len + arg->len + 4;
            mp_seq_clear(self->items, self->len + arg->len, self->alloc, sizeof(*self->items));
        }

        memcpy(self->items + self->len, arg->items, sizeof(mp_obj_t) * arg->len);
        self->len += arg->len;
    } else {
        list_extend_from_iter(self_in, arg_in);
    }
    return mp_const_none; // return None, as per CPython
}

STATIC mp_obj_t list_pop(uint n_args, const mp_obj_t *args) {
    assert(1 <= n_args && n_args <= 2);
    assert(MP_OBJ_IS_TYPE(args[0], &mp_type_list));
    mp_obj_list_t *self = args[0];
    if (self->len == 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_IndexError, "pop from empty list"));
    }
    uint index = mp_get_index(self->base.type, self->len, n_args == 1 ? MP_OBJ_NEW_SMALL_INT(-1) : args[1], false);
    mp_obj_t ret = self->items[index];
    self->len -= 1;
    memcpy(self->items + index, self->items + index + 1, (self->len - index) * sizeof(mp_obj_t));
    // Clear stale pointer from slot which just got freed to prevent GC issues
    self->items[self->len] = MP_OBJ_NULL;
    if (self->alloc > LIST_MIN_ALLOC && self->alloc > 2 * self->len) {
        self->items = m_renew(mp_obj_t, self->items, self->alloc, self->alloc/2);
        self->alloc /= 2;
    }
    return ret;
}

// TODO make this conform to CPython's definition of sort

// Non-Recursive QuickSort
#define INSERTION_SORT_SIZE       (8)

STATIC mp_obj_t get_value(mp_obj_t *data, int position, mp_obj_t key_fn) {
    mp_obj_t value;
    if (key_fn == NULL) {
        value = data[position];
    } else {
        value = mp_call_function_1(key_fn, data[position]);
    }
    return value;
}

STATIC bool compare(mp_obj_t *data, int position, mp_obj_t value, int op, mp_obj_t key_fn) { 
    mp_obj_t cmp_value = get_value(data, position, key_fn);
    
    return mp_binary_op(op, cmp_value, value) == mp_const_true;
}

STATIC bool compare_by_pos(mp_obj_t *data, int pos1, int pos2, int op, mp_obj_t key_fn) {
    mp_obj_t val1 = get_value(data, pos1, key_fn);
    mp_obj_t val2 = get_value(data, pos2, key_fn);

    return mp_binary_op(op, val1, val2) == mp_const_true;
}

STATIC void swap_data(mp_obj_t *data, int pos1, int pos2) {
    mp_obj_t val = data[pos1];
    data[pos1] = data[pos2];
    data[pos2] = val;
}

STATIC uint32_t log2_uint32(uint32_t val) {
    const uint32_t b[5] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000};
    const uint32_t S[5] = {1, 2, 4, 8, 16};
    
    uint32_t result = 0;
    
    for(int i = 4; i >= 0; i--) 
    {
      if (val & b[i])
      {
        val >>= S[i];
        result |= S[i];
      } 
    }
    
    return result;
}

STATIC void mp_quicksort(mp_obj_t *head, mp_obj_t *tail, mp_obj_t key_fn, bool reversed) {
    int op = reversed ? MP_BINARY_OP_LESS : MP_BINARY_OP_MORE;
    
    int low;
    int high;
    int mid;
    int left;
    int right;

    int stack_top = 0;
    int length = (tail - head + 1);

    assert(length >= 0);
    
    mp_obj_t separate_value;
    mp_obj_t separate_value_for_compare;    // value after key_fn
    mp_obj_t *data = head;
    
    int stack_size = (log2_uint32(length / (INSERTION_SORT_SIZE/2) )) * 2 + 4;
    int *stack = m_malloc( stack_size * sizeof(int) );

    // We need sort from first point to last point.
    stack[stack_top++] = 0;
    stack[stack_top++] = length - 1;

    // Repeat until all sorting is done (i.e., the sort position stack is empty).
    while (stack_top > 0) {

        // Get the starting/ending positions of the aray to be sorted.
        high = stack[--stack_top];
        low  = stack[--stack_top];

        // If the number of items to sort is small or if there isn't enough space on the sort stack
        // use an insertion sort to sort this array.
        // For a array length of 8 or less this is faster than partitioning.
        if ( ((high - low) <= INSERTION_SORT_SIZE) ||
             (stack_top >= (stack_size - 4)) ) {

            for ( right =  (low + 1); right <= high; right ++ ) {
                // Get the item to insert.
                separate_value = get_value(data, right, NULL);
                separate_value_for_compare = get_value(data, right, key_fn);
                left = right;
                
                // Find the location to insert it by moving up all values larger than item.
                while ( (left > low) &&
                        compare(data, left - 1, separate_value_for_compare, op, key_fn) ) {
                    data[left] = data[left - 1];
                    left--;
                }
                
                data[left] = separate_value;
            }
        } else {
            // PartitionSort

            // The point here is to separate the array into 2 arrays, and any data
            // in one array must <= any data of another array. To reduce the loop
            // number and test time, the lengths of these 2 arrays should be as close
            // as possible

            // First, sort the array so that 1st, 2nd, and last will be in order.
            // To have more chance to get 2 arrayes with close lengths, here we choose
            // 3 data at the original positions First, Middle, and Last.

            // Swap the 2nd and middle data.
            mid = (low + high) >> 1;
            swap_data(data, low + 1, mid);

            // Sort 1st and last.
            if ( compare_by_pos(data, low, high, op, key_fn)) {
                swap_data(data, low, high);
            }

            // Sort 2nd and last.
            if ( compare_by_pos(data, low + 1, high, op, key_fn)) {
                swap_data(data, low + 1, high);
            }

            // Sort 1st and 2nd.
            if ( compare_by_pos(data, low, low + 1, op, key_fn)) {
                swap_data(data, low, low + 1);
            }
           
            // At this point, 1st, 2nd, and last data are in order.
            // (Note: here we only sort 3 data, it does not means the 1st data is the
            // least one of the array. The 1st one is only the least one of these 3 data.)

            // Separate the array into 2 arrays, the value to separate them is the
            // 2nd data.
            left = low + 1;
            right = high;
            separate_value = get_value(data, left, NULL); 
            separate_value_for_compare = get_value(data, left, key_fn);

            while (1) {
                // Search the whole range forward to find the first position whose value is
                // greater than or equal to the SeparateValue, and assign the position into
                // 'left'. Since Last data >= SeparateValue, we are guaranteed to have
                // one valid position.
                while ( !compare(data, ++left, separate_value_for_compare, op, key_fn) ) {
                };

                // Search the whole range backword to find the first position whose value is
                // less than or equal to the SeparateValue, and assign it into 'right'.
                // We can at least find 2 (i.e., 1st and 2nd).
                while ( compare(data, --right, separate_value_for_compare, op, key_fn) ) {
                };

                if (left < right) {
                    // since data at 'left' is larger than or equal to data at 'right', swap them
                    // to make sure smaller value is before the larger value.
                    swap_data(data, left, right);
                } else {
                    // Finished searching the whole array.
                    break;
                }
            }

            // At this point, all values before (and at) right should <= SeparateValue, and
            // all values after it should >= SeparateValue.

            // Swap the 2nd position and right so that the data at right will
            // be the largest value from low to right, and it also will be the smallest
            // value from right to high.
            data[low + 1] = data[right];
            data[right] = separate_value;


            // Therefore, we can separate the array into 2 arrays. First is from low to right - 1,
            // and the second is from right + 1 to high. right itself needs not be included
            // in these 2 arrays.

            // Always sort the shorter segment first to saves stack space.
              if ( (right - low) < (high - right) ) {
                stack[stack_top++] = right + 1;
                stack[stack_top++] = high;
                stack[stack_top++] = low;
                stack[stack_top++] = right - 1;
            } else {
                stack[stack_top++] = low;
                stack[stack_top++] = right - 1;
                stack[stack_top++] = right + 1;
                stack[stack_top++] = high;
            }
        }
    }
    
    m_free(stack, stack_size * sizeof(int));
}

mp_obj_t mp_obj_list_sort(uint n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    assert(n_args >= 1);
    assert(MP_OBJ_IS_TYPE(args[0], &mp_type_list));
    if (n_args > 1) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError,
                                          "list.sort takes no positional arguments"));
    }
    mp_obj_list_t *self = args[0];
    if (self->len > 1) {
        mp_map_elem_t *keyfun = mp_map_lookup(kwargs, MP_OBJ_NEW_QSTR(MP_QSTR_key), MP_MAP_LOOKUP);
        mp_map_elem_t *reverse = mp_map_lookup(kwargs, MP_OBJ_NEW_QSTR(MP_QSTR_reverse), MP_MAP_LOOKUP);
        mp_quicksort(self->items, self->items + self->len - 1,
                     keyfun ? keyfun->value : NULL,
                     reverse && reverse->value ? mp_obj_is_true(reverse->value) : false);
    }
    return mp_const_none; // return None, as per CPython
}

STATIC mp_obj_t list_clear(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_list));
    mp_obj_list_t *self = self_in;
    self->len = 0;
    self->items = m_renew(mp_obj_t, self->items, self->alloc, LIST_MIN_ALLOC);
    self->alloc = LIST_MIN_ALLOC;
    mp_seq_clear(self->items, 0, self->alloc, sizeof(*self->items));
    return mp_const_none;
}

STATIC mp_obj_t list_copy(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_list));
    mp_obj_list_t *self = self_in;
    return mp_obj_new_list(self->len, self->items);
}

STATIC mp_obj_t list_count(mp_obj_t self_in, mp_obj_t value) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_list));
    mp_obj_list_t *self = self_in;
    return mp_seq_count_obj(self->items, self->len, value);
}

STATIC mp_obj_t list_index(uint n_args, const mp_obj_t *args) {
    assert(2 <= n_args && n_args <= 4);
    assert(MP_OBJ_IS_TYPE(args[0], &mp_type_list));
    mp_obj_list_t *self = args[0];
    return mp_seq_index_obj(self->items, self->len, n_args, args);
}

STATIC mp_obj_t list_insert(mp_obj_t self_in, mp_obj_t idx, mp_obj_t obj) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_list));
    mp_obj_list_t *self = self_in;
    // insert has its own strange index logic
    int index = MP_OBJ_SMALL_INT_VALUE(idx);
    if (index < 0) {
         index += self->len;
    }
    if (index < 0) {
         index = 0;
    }
    if (index > self->len) {
         index = self->len;
    }

    mp_obj_list_append(self_in, mp_const_none);

    for (int i = self->len-1; i > index; i--) {
         self->items[i] = self->items[i-1];
    }
    self->items[index] = obj;

    return mp_const_none;
}

STATIC mp_obj_t list_remove(mp_obj_t self_in, mp_obj_t value) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_list));
    mp_obj_t args[] = {self_in, value};
    args[1] = list_index(2, args);
    list_pop(2, args);

    return mp_const_none;
}

STATIC mp_obj_t list_reverse(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_list));
    mp_obj_list_t *self = self_in;

    int len = self->len;
    for (int i = 0; i < len/2; i++) {
         mp_obj_t *a = self->items[i];
         self->items[i] = self->items[len-i-1];
         self->items[len-i-1] = a;
    }

    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(list_append_obj, mp_obj_list_append);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(list_extend_obj, list_extend);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(list_clear_obj, list_clear);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(list_copy_obj, list_copy);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(list_count_obj, list_count);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(list_index_obj, 2, 4, list_index);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(list_insert_obj, list_insert);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(list_pop_obj, 1, 2, list_pop);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(list_remove_obj, list_remove);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(list_reverse_obj, list_reverse);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(list_sort_obj, 0, mp_obj_list_sort);

STATIC const mp_map_elem_t list_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_append), (mp_obj_t)&list_append_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_clear), (mp_obj_t)&list_clear_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_copy), (mp_obj_t)&list_copy_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_count), (mp_obj_t)&list_count_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_extend), (mp_obj_t)&list_extend_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_index), (mp_obj_t)&list_index_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_insert), (mp_obj_t)&list_insert_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pop), (mp_obj_t)&list_pop_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_remove), (mp_obj_t)&list_remove_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_reverse), (mp_obj_t)&list_reverse_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sort), (mp_obj_t)&list_sort_obj },
};

STATIC MP_DEFINE_CONST_DICT(list_locals_dict, list_locals_dict_table);

const mp_obj_type_t mp_type_list = {
    { &mp_type_type },
    .name = MP_QSTR_list,
    .print = list_print,
    .make_new = list_make_new,
    .unary_op = list_unary_op,
    .binary_op = list_binary_op,
    .subscr = list_subscr,
    .getiter = list_getiter,
    .locals_dict = (mp_obj_t)&list_locals_dict,
};

void mp_obj_list_init(mp_obj_list_t *o, uint n) {
    o->base.type = &mp_type_list;
    o->alloc = n < LIST_MIN_ALLOC ? LIST_MIN_ALLOC : n;
    o->len = n;
    o->items = m_new(mp_obj_t, o->alloc);
    mp_seq_clear(o->items, n, o->alloc, sizeof(*o->items));
}

STATIC mp_obj_list_t *list_new(uint n) {
    mp_obj_list_t *o = m_new_obj(mp_obj_list_t);
    mp_obj_list_init(o, n);
    return o;
}

mp_obj_t mp_obj_new_list(uint n, mp_obj_t *items) {
    mp_obj_list_t *o = list_new(n);
    if (items != NULL) {
        for (int i = 0; i < n; i++) {
            o->items[i] = items[i];
        }
    }
    return o;
}

void mp_obj_list_get(mp_obj_t self_in, uint *len, mp_obj_t **items) {
    mp_obj_list_t *self = self_in;
    *len = self->len;
    *items = self->items;
}

void mp_obj_list_set_len(mp_obj_t self_in, uint len) {
    // trust that the caller knows what it's doing
    // TODO realloc if len got much smaller than alloc
    mp_obj_list_t *self = self_in;
    self->len = len;
}

void mp_obj_list_store(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    mp_obj_list_t *self = self_in;
    uint i = mp_get_index(self->base.type, self->len, index, false);
    self->items[i] = value;
}

/******************************************************************************/
/* list iterator                                                              */

typedef struct _mp_obj_list_it_t {
    mp_obj_base_t base;
    mp_obj_list_t *list;
    machine_uint_t cur;
} mp_obj_list_it_t;

mp_obj_t list_it_iternext(mp_obj_t self_in) {
    mp_obj_list_it_t *self = self_in;
    if (self->cur < self->list->len) {
        mp_obj_t o_out = self->list->items[self->cur];
        self->cur += 1;
        return o_out;
    } else {
        return MP_OBJ_STOP_ITERATION;
    }
}

STATIC const mp_obj_type_t mp_type_list_it = {
    { &mp_type_type },
    .name = MP_QSTR_iterator,
    .getiter = mp_identity,
    .iternext = list_it_iternext,
};

mp_obj_t mp_obj_new_list_iterator(mp_obj_list_t *list, int cur) {
    mp_obj_list_it_t *o = m_new_obj(mp_obj_list_it_t);
    o->base.type = &mp_type_list_it;
    o->list = list;
    o->cur = cur;
    return o;
}
