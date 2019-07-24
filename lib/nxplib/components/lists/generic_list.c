/*
 * Copyright 2018-2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
********************************************************************************** */
#include "fsl_common.h"
#include "generic_list.h"

static list_status_t LIST_Scan(list_handle_t list, list_element_handle_t newElement)
{
    list_element_handle_t element = list->head;

    while (element != NULL)
    {
        if (element == newElement)
        {
            return kLIST_DuplicateError;
        }
        element = element->next;
    }
    return kLIST_Ok;
}

/*! *********************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
********************************************************************************** */
/*! *********************************************************************************
 * \brief     Initialises the list descriptor.
 *
 * \param[in] list - LIST_ handle to init.
 *            max - Maximum number of elements in list. 0 for unlimited.
 *
 * \return void.
 *
 * \pre
 *
 * \post
 *
 * \remarks
 *
 ********************************************************************************** */
void LIST_Init(list_handle_t list, uint32_t max)
{
    list->head = NULL;
    list->tail = NULL;
    list->max  = (uint16_t)max;
    list->size = 0;
}

/*! *********************************************************************************
 * \brief     Gets the list that contains the given element.
 *
 * \param[in] element - Handle of the element.
 *
 * \return NULL if element is orphan.
 *         Handle of the list the element is inserted into.
 *
 * \pre
 *
 * \post
 *
 * \remarks
 *
 ********************************************************************************** */
list_handle_t LIST_GetList(list_element_handle_t element)
{
    return element->list;
}

/*! *********************************************************************************
 * \brief     Links element to the tail of the list.
 *
 * \param[in] list - ID of list to insert into.
 *            element - element to add
 *
 * \return kLIST_Full if list is full.
 *         kLIST_Ok if insertion was successful.
 *
 * \pre
 *
 * \post
 *
 * \remarks
 *
 ********************************************************************************** */
list_status_t LIST_AddTail(list_handle_t list, list_element_handle_t element)
{
    uint32_t regPrimask = DisableGlobalIRQ();

    if ((list->max != 0U) && (list->max == list->size))
    {
        EnableGlobalIRQ(regPrimask);
        return kLIST_Full;
    }

    if (kLIST_DuplicateError == LIST_Scan(list, element))
    {
        EnableGlobalIRQ(regPrimask);
        return kLIST_DuplicateError;
    }

    if (list->size == 0U)
    {
        list->head = element;
    }
    else
    {
        list->tail->next = element;
    }
    element->prev = list->tail;
    element->next = NULL;
    element->list = list;
    list->tail    = element;
    list->size++;

    EnableGlobalIRQ(regPrimask);
    return kLIST_Ok;
}

/*! *********************************************************************************
 * \brief     Links element to the head of the list.
 *
 * \param[in] list - ID of list to insert into.
 *            element - element to add
 *
 * \return kLIST_Full if list is full.
 *         kLIST_Ok if insertion was successful.
 *
 * \pre
 *
 * \post
 *
 * \remarks
 *
 ********************************************************************************** */
list_status_t LIST_AddHead(list_handle_t list, list_element_handle_t element)
{
    uint32_t regPrimask = DisableGlobalIRQ();

    if ((list->max != 0U) && (list->max == list->size))
    {
        EnableGlobalIRQ(regPrimask);
        return kLIST_Full;
    }

    if (kLIST_DuplicateError == LIST_Scan(list, element))
    {
        EnableGlobalIRQ(regPrimask);
        return kLIST_DuplicateError;
    }

    if (list->size == 0U)
    {
        list->tail = element;
    }
    else
    {
        list->head->prev = element;
    }
    element->next = list->head;
    element->prev = NULL;
    element->list = list;
    list->head    = element;
    list->size++;

    EnableGlobalIRQ(regPrimask);
    return kLIST_Ok;
}

/*! *********************************************************************************
 * \brief     Unlinks element from the head of the list.
 *
 * \param[in] list - ID of list to remove from.
 *
 * \return NULL if list is empty.
 *         ID of removed element(pointer) if removal was successful.
 *
 * \pre
 *
 * \post
 *
 * \remarks
 *
 ********************************************************************************** */
list_element_handle_t LIST_RemoveHead(list_handle_t list)
{
    list_element_handle_t element;

    uint32_t regPrimask = DisableGlobalIRQ();

    if ((NULL == list) || (list->size == 0U))
    {
        EnableGlobalIRQ(regPrimask);
        return NULL; /*LIST_ is empty*/
    }

    element = list->head;
    list->size--;
    if (list->size == 0U)
    {
        list->tail = NULL;
    }
    else
    {
        element->next->prev = NULL;
    }
    list->head    = element->next; /*Is NULL if element is head*/
    element->list = NULL;

    EnableGlobalIRQ(regPrimask);
    return element;
}

/*! *********************************************************************************
 * \brief     Gets head element ID.
 *
 * \param[in] list - ID of list.
 *
 * \return NULL if list is empty.
 *         ID of head element if list is not empty.
 *
 * \pre
 *
 * \post
 *
 * \remarks
 *
 ********************************************************************************** */
list_element_handle_t LIST_GetHead(list_handle_t list)
{
    return list->head;
}

/*! *********************************************************************************
 * \brief     Gets next element ID.
 *
 * \param[in] element - ID of the element.
 *
 * \return NULL if element is tail.
 *         ID of next element if exists.
 *
 * \pre
 *
 * \post
 *
 * \remarks
 *
 ********************************************************************************** */
list_element_handle_t LIST_GetNext(list_element_handle_t element)
{
    return element->next;
}

/*! *********************************************************************************
 * \brief     Gets previous element ID.
 *
 * \param[in] element - ID of the element.
 *
 * \return NULL if element is head.
 *         ID of previous element if exists.
 *
 * \pre
 *
 * \post
 *
 * \remarks
 *
 ********************************************************************************** */
list_element_handle_t LIST_GetPrev(list_element_handle_t element)
{
    return element->prev;
}

/*! *********************************************************************************
 * \brief     Unlinks an element from its list.
 *
 * \param[in] element - ID of the element to remove.
 *
 * \return kLIST_OrphanElement if element is not part of any list.
 *         kLIST_Ok if removal was successful.
 *
 * \pre
 *
 * \post
 *
 * \remarks
 *
 ********************************************************************************** */
list_status_t LIST_RemoveElement(list_element_handle_t element)
{
    if (element->list == NULL)
    {
        return kLIST_OrphanElement; /*Element was previusly removed or never added*/
    }

    uint32_t regPrimask = DisableGlobalIRQ();

    if (element->prev == NULL) /*Element is head or solo*/
    {
        element->list->head = element->next; /*is null if solo*/
    }
    if (element->next == NULL) /*Element is tail or solo*/
    {
        element->list->tail = element->prev; /*is null if solo*/
    }
    if (element->prev != NULL) /*Element is not head*/
    {
        element->prev->next = element->next;
    }
    if (element->next != NULL) /*Element is not tail*/
    {
        element->next->prev = element->prev;
    }
    element->list->size--;
    element->list = NULL;

    EnableGlobalIRQ(regPrimask);
    return kLIST_Ok;
}

/*! *********************************************************************************
 * \brief     Links an element in the previous position relative to a given member
 *            of a list.
 *
 * \param[in] element - ID of a member of a list.
 *            newElement - new element to insert before the given member.
 *
 * \return kLIST_OrphanElement if element is not part of any list.
 *         kLIST_Full if list is full.
 *         kLIST_Ok if insertion was successful.
 *
 * \pre
 *
 * \post
 *
 * \remarks
 *
 ********************************************************************************** */
list_status_t LIST_AddPrevElement(list_element_handle_t element, list_element_handle_t newElement)
{
    if (element->list == NULL)
    {
        return kLIST_OrphanElement; /*Element was previusly removed or never added*/
    }
    uint32_t regPrimask = DisableGlobalIRQ();

    if ((element->list->max != 0U) && (element->list->max == element->list->size))
    {
        EnableGlobalIRQ(regPrimask);
        return kLIST_Full;
    }

    if (kLIST_DuplicateError == LIST_Scan(element->list, newElement))
    {
        EnableGlobalIRQ(regPrimask);
        return kLIST_DuplicateError;
    }

    if (element->prev == NULL) /*Element is list head*/
    {
        element->list->head = newElement;
    }
    else
    {
        element->prev->next = newElement;
    }
    newElement->list = element->list;
    element->list->size++;
    newElement->next = element;
    newElement->prev = element->prev;
    element->prev    = newElement;

    EnableGlobalIRQ(regPrimask);
    return kLIST_Ok;
}

/*! *********************************************************************************
 * \brief     Gets the current size of a list.
 *
 * \param[in] list - ID of the list.
 *
 * \return Current size of the list.
 *
 * \pre
 *
 * \post
 *
 * \remarks
 *
 ********************************************************************************** */
uint32_t LIST_GetSize(list_handle_t list)
{
    return list->size;
}

/*! *********************************************************************************
 * \brief     Gets the number of free places in the list.
 *
 * \param[in] list - ID of the list.
 *
 * \return Available size of the list.
 *
 * \pre
 *
 * \post
 *
 * \remarks
 *
 ********************************************************************************** */
uint32_t LIST_GetAvailableSize(list_handle_t list)
{
    return ((uint32_t)list->max - (uint32_t)list->size);
}
