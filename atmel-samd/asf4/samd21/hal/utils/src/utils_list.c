/**
 * \file
 *
 * \brief List functionality implementation.
 *
 * Copyright (C) 2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include <utils_list.h>
#include <utils_assert.h>

/**
 * \brief Check whether element belongs to list
 */
bool is_list_element(const struct list_descriptor *const list, const void *const element)
{
	struct list_element *it;
	for (it = list->head; it; it = it->next) {
		if (it == element) {
			return true;
		}
	}

	return false;
}

/**
 * \brief Insert an element as list head
 */
void list_insert_as_head(struct list_descriptor *const list, void *const element)
{
	ASSERT(!is_list_element(list, element));

	((struct list_element *)element)->next = list->head;
	list->head                             = (struct list_element *)element;
}

/**
 * \brief Insert an element after the given list element
 */
void list_insert_after(void *const after, void *const element)
{
	((struct list_element *)element)->next = ((struct list_element *)after)->next;
	((struct list_element *)after)->next   = (struct list_element *)element;
}

/**
 * \brief Insert an element at list end
 */
void list_insert_at_end(struct list_descriptor *const list, void *const element)
{
	struct list_element *it = list->head;

	ASSERT(!is_list_element(list, element));

	if (!list->head) {
		list->head                             = (struct list_element *)element;
		((struct list_element *)element)->next = NULL;
		return;
	}

	while (it->next) {
		it = it->next;
	}
	it->next                               = (struct list_element *)element;
	((struct list_element *)element)->next = NULL;
}

/**
 * \brief Removes list head
 */
void *list_remove_head(struct list_descriptor *const list)
{
	if (list->head) {
		struct list_element *tmp = list->head;

		list->head = list->head->next;
		return (void *)tmp;
	}

	return NULL;
}

/**
 * \brief Removes list element
 */
bool list_delete_element(struct list_descriptor *const list, const void *const element)
{
	if (!element) {
		return false;
	}

	if (list->head == element) {
		list->head = list->head->next;
		return true;
	} else {
		struct list_element *it = list->head;

		while (it && it->next != element) {
			it = it->next;
		}
		if (it) {
			it->next = ((struct list_element *)element)->next;
			return true;
		}
	}

	return false;
}

//@}
