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
 *  btstack_linked_list.h
 */

#ifndef __BTSTACK_LINKED_LIST_H
#define __BTSTACK_LINKED_LIST_H

#if defined __cplusplus
extern "C" {
#endif
	
typedef struct btstack_linked_item {
    struct btstack_linked_item *next; // <-- next element in list, or NULL
} btstack_linked_item_t;

typedef btstack_linked_item_t * btstack_linked_list_t;

typedef struct {
	int advance_on_next;
    btstack_linked_item_t * prev;	// points to the item before the current one
    btstack_linked_item_t * curr;	// points to the current item (to detect item removal)
} btstack_linked_list_iterator_t;


// test if list is empty
int                     btstack_linked_list_empty(btstack_linked_list_t * list);
// add item to list as first element
void                    btstack_linked_list_add(btstack_linked_list_t * list, btstack_linked_item_t *item);       
// add item to list as last element
void                    btstack_linked_list_add_tail(btstack_linked_list_t * list, btstack_linked_item_t *item); 
// remove item from list
int                     btstack_linked_list_remove(btstack_linked_list_t * list, btstack_linked_item_t *item); 
// find the last item in the list
btstack_linked_item_t * btstack_linked_list_get_last_item(btstack_linked_list_t * list);   

/**
 * @brief Counts number of items in list
 * @returns number of items in list
 */
int btstack_linked_list_count(btstack_linked_list_t * list);

//
// iterator for linked lists. allows to remove current element.
// robust against removal of current element by btstack_linked_list_remove.
//
void            btstack_linked_list_iterator_init(btstack_linked_list_iterator_t * it, btstack_linked_list_t * list);
int             btstack_linked_list_iterator_has_next(btstack_linked_list_iterator_t * it);
btstack_linked_item_t * btstack_linked_list_iterator_next(btstack_linked_list_iterator_t * it);
void            btstack_linked_list_iterator_remove(btstack_linked_list_iterator_t * it);

void test_linked_list(void);

#if defined __cplusplus
}
#endif

#endif // __BTSTACK_LINKED_LIST_H
