/**
 * \file
 *
 * \brief List declaration.
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

#ifndef _UTILS_LIST_H_INCLUDED
#define _UTILS_LIST_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup doc_driver_hal_utils_list
 *
 * @{
 */

#include <compiler.h>

/**
 * \brief List element type
 */
struct list_element {
	struct list_element *next;
};

/**
 * \brief List head type
 */
struct list_descriptor {
	struct list_element *head;
};

/**
 * \brief Reset list
 *
 * \param[in] list The pointer to a list descriptor
 */
static inline void list_reset(struct list_descriptor *const list)
{
	list->head = NULL;
}

/**
 * \brief Retrieve list head
 *
 * \param[in] list The pointer to a list descriptor
 *
 * \return A pointer to the head of the given list or NULL if the list is
 * empty
 */
static inline void *list_get_head(const struct list_descriptor *const list)
{
	return (void *)list->head;
}

/**
 * \brief Retrieve next list head
 *
 * \param[in] list The pointer to a list element
 *
 * \return A pointer to the next list element or NULL if there is not next
 * element
 */
static inline void *list_get_next_element(const void *const element)
{
	return element ? ((struct list_element *)element)->next : NULL;
}

/**
 * \brief Insert an element as list head
 *
 * \param[in] list The pointer to a list element
 * \param[in] element An element to insert to the given list
 */
void list_insert_as_head(struct list_descriptor *const list, void *const element);

/**
 * \brief Insert an element after the given list element
 *
 * \param[in] after An element to insert after
 * \param[in] element Element to insert to the given list
 */
void list_insert_after(void *const after, void *const element);

/**
 * \brief Insert an element at list end
 *
 * \param[in] after An element to insert after
 * \param[in] element Element to insert to the given list
 */
void list_insert_at_end(struct list_descriptor *const list, void *const element);

/**
 * \brief Check whether an element belongs to a list
 *
 * \param[in] list The pointer to a list
 * \param[in] element An element to check
 *
 * \return The result of checking
 * \retval true If the given element is an element of the given list
 * \retval false Otherwise
 */
bool is_list_element(const struct list_descriptor *const list, const void *const element);

/**
 * \brief Removes list head
 *
 * This function removes the list head and sets the next element after the list
 * head as a new list head.
 *
 * \param[in] list The pointer to a list
 *
 * \return The pointer to the new list head of NULL if the list head is NULL
 */
void *list_remove_head(struct list_descriptor *const list);

/**
 * \brief Removes the list element
 *
 * \param[in] list The pointer to a list
 * \param[in] element An element to remove
 *
 * \return The result of element removing
 * \retval true The given element is removed from the given list
 * \retval false The given element is not an element of the given list
 */
bool list_delete_element(struct list_descriptor *const list, const void *const element);

/**@}*/

#ifdef __cplusplus
}
#endif
#endif /* _UTILS_LIST_H_INCLUDED */
