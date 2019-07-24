/*
 * Copyright 2018-2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _GENERIC_LIST_H_
#define _GENERIC_LIST_H_

/*!
 * @addtogroup GenericList
 * @{
 */

/*!*********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
********************************************************************************** */

/*! *********************************************************************************
*************************************************************************************
* Public macro definitions
*************************************************************************************
********************************************************************************** */

/*! *********************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
********************************************************************************** */
/*! @brief The list status */
typedef enum _list_status
{
    kLIST_Ok             = kStatus_Success,                   /*!< Success */
    kLIST_DuplicateError = MAKE_STATUS(kStatusGroup_LIST, 1), /*!< Duplicate Error */
    kLIST_Full           = MAKE_STATUS(kStatusGroup_LIST, 2), /*!< FULL */
    kLIST_Empty          = MAKE_STATUS(kStatusGroup_LIST, 3), /*!< Empty */
    kLIST_OrphanElement  = MAKE_STATUS(kStatusGroup_LIST, 4), /*!< Orphan Element */
} list_status_t;

/*! @brief The list structure*/
typedef struct list_tag
{
    struct list_element_tag *head; /*!< list head */
    struct list_element_tag *tail; /*!< list tail */
    uint16_t size;                 /*!< list size */
    uint16_t max;                  /*!< list max number of elements */
} list_t, *list_handle_t;

/*! @brief The list element*/
typedef struct list_element_tag
{
    struct list_element_tag *next; /*!< next list element   */
    struct list_element_tag *prev; /*!< previous list element */
    struct list_tag *list;         /*!< pointer to the list */
} list_element_t, *list_element_handle_t;

/*! *********************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
********************************************************************************** */
/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */
/*!
 * @brief Initialize the list.
 *
 * This function initialize the list.
 *
 * @param list - List handle to initialize.
 * @param max - Maximum number of elements in list. 0 for unlimited.
 */
void LIST_Init(list_handle_t list, uint32_t max);

/*!
 * @brief Gets the list that contains the given element.
 *
 *
 * @param element - Handle of the element.
 * @retval NULL if element is orphan, Handle of the list the element is inserted into.
 */
list_handle_t LIST_GetList(list_element_handle_t element);

/*!
 * @brief Links element to the head of the list.
 *
 * @param list - Handle of the list.
 * @param element - Handle of the element.
 * @retval kLIST_Full if list is full, kLIST_Ok if insertion was successful.
 */
list_status_t LIST_AddHead(list_handle_t list, list_element_handle_t element);

/*!
 * @brief Links element to the tail of the list.
 *
 * @param list - Handle of the list.
 * @param element - Handle of the element.
 * @retval kLIST_Full if list is full, kLIST_Ok if insertion was successful.
 */
list_status_t LIST_AddTail(list_handle_t list, list_element_handle_t element);

/*!
 * @brief Unlinks element from the head of the list.
 *
 * @param list - Handle of the list.
 *
 * @retval NULL if list is empty, handle of removed element(pointer) if removal was successful.
 */
list_element_handle_t LIST_RemoveHead(list_handle_t list);

/*!
 * @brief Gets head element handle.
 *
 * @param list - Handle of the list.
 *
 * @retval NULL if list is empty, handle of removed element(pointer) if removal was successful.
 */
list_element_handle_t LIST_GetHead(list_handle_t list);

/*!
 * @brief Gets next element handle for given element handle.
 *
 * @param element - Handle of the element.
 *
 * @retval NULL if list is empty, handle of removed element(pointer) if removal was successful.
 */
list_element_handle_t LIST_GetNext(list_element_handle_t element);

/*!
 * @brief Gets previous element handle for given element handle.
 *
 * @param element - Handle of the element.
 *
 * @retval NULL if list is empty, handle of removed element(pointer) if removal was successful.
 */
list_element_handle_t LIST_GetPrev(list_element_handle_t element);

/*!
 * @brief Unlinks an element from its list.
 *
 * @param element - Handle of the element.
 *
 * @retval kLIST_OrphanElement if element is not part of any list.
 * @retval kLIST_Ok if removal was successful.
 */
list_status_t LIST_RemoveElement(list_element_handle_t element);

/*!
 * @brief Links an element in the previous position relative to a given member of a list.
 *
 * @param element - Handle of the element.
 * @param newElement - New element to insert before the given member.
 *
 * @retval kLIST_OrphanElement if element is not part of any list.
 * @retval kLIST_Ok if removal was successful.
 */
list_status_t LIST_AddPrevElement(list_element_handle_t element, list_element_handle_t newElement);

/*!
 * @brief Gets the current size of a list.
 *
 * @param list - Handle of the list.
 *
 * @retval Current size of the list.
 */
uint32_t LIST_GetSize(list_handle_t list);

/*!
 * @brief Gets the number of free places in the list.
 *
 * @param list - Handle of the list.
 *
 * @retval Available size of the list.
 */
uint32_t LIST_GetAvailableSize(list_handle_t list);

/* @} */

#if defined(__cplusplus)
}
#endif
/*! @}*/
#endif /*_GENERIC_LIST_H_*/
