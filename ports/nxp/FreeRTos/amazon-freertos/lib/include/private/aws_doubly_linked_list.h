/*
 * Amazon FreeRTOS
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */


/**
 * @file aws_doubly_linked_list.h
 * @brief Doubly Linked List implementation.
 *
 * A generic implementation of circular Doubly Linked List which consists of a
 * list head and some list entries (zero in case of an empty list).
 *
 * To start with, a structure of type Link_t should be embedded in the structure
 * which is to be organized as doubly linked list.
 * @code
 * typedef struct UserStruct
 * {
 *     uint32_t ulField1;
 *     uint32_t ulField2;
 *     Link_t xLink;
 * } UserStruct_t;
 * @endcode
 *
 * A List head should then be defined and initialized.
 * @code
 * Link_t xListHead;
 * listINIT_HEAD( &xListHead );
 * @endcode
 *
 * listADD can then be used to add nodes to the list.
 * @code
 * listADD( &( xListHead ), &( pxUserStruct->xLink ) );
 * @endcode
 *
 * listFOR_EACH can be used for traversing the list.
 * @code
 * Link_t *pxLink;
 * UserStruct_t *pxUserStruct;
 * listFOR_EACH( pxLink, &( xListHead ) )
 * {
 *      pxUserStruct = listCONTAINER( pxLink, UserStruct_t, xLink );
 * }
 * @endcode
 *
 * listFOR_EACH_SAFE should be used if you want to perform destructive operations
 * (like free) on nodes while traversing the list.
 * @code
 * Link_t *pxLink, *pxTempLink;
 * UserStruct_t *pxUserStruct;
 * listFOR_EACH( pxLink, pxTempLink, &( xListHead ) )
 * {
 *      pxUserStruct = listCONTAINER( pxLink, UserStruct_t, xLink );
 *      free( pxUserStruct );
 * }
 * @endcode
 */

#ifndef _AWS_DOUBLY_LINKED_LIST_H_
#define _AWS_DOUBLY_LINKED_LIST_H_

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Struct embedded in any struct to make it a doubly linked
 * list.
 *
 * pxNext in the head points to the first node in the list and pxPrev
 * in the head points to the last node in the list. In case of empty
 * list, both pxPrev and pxNext in the head point to the head node itself.
 */
typedef struct Link
{
    struct Link * pxPrev; /**< Pointer to the previous node. */
    struct Link * pxNext; /**< Pointer to the next node. */
} Link_t;

/**
 * @brief Initializes the given list head to an empty list.
 *
 * @param[in] pxHead The given list head to initialize.
 */
#define listINIT_HEAD( pxHead )          \
    {                                    \
        ( pxHead )->pxPrev = ( pxHead ); \
        ( pxHead )->pxNext = ( pxHead ); \
    }

/**
 * @brief Adds the given new node to the given list.
 *
 * @param[in] pxHead The head of the given list.
 * @param[in] pxLink The given new node to be added to the given
 * list.
 */
#define listADD( pxHead, pxLink )                     \
    {                                                 \
        Link_t * pxPrevLink = ( pxHead );             \
        Link_t * pxNextLink = ( ( pxHead )->pxNext ); \
                                                      \
        ( pxLink )->pxNext = pxNextLink;              \
        pxNextLink->pxPrev = ( pxLink );              \
        pxPrevLink->pxNext = ( pxLink );              \
        ( pxLink )->pxPrev = ( pxPrevLink );          \
    }

/**
 * @brief Removes the given node from the list it is part of.
 *
 * If the given node is not a part of any list (i.e. next and previous
 * nodes are NULL), nothing happens.
 *
 * @param[in] pxLink The given node to remove from the list.
 */
#define listREMOVE( pxLink )                                            \
    {                                                                   \
        /* If the link is part of a list, remove it from the list. */   \
        if( ( pxLink )->pxNext != NULL && ( pxLink )->pxPrev != NULL )  \
        {                                                               \
            ( pxLink )->pxPrev->pxNext = ( pxLink )->pxNext;            \
            ( pxLink )->pxNext->pxPrev = ( pxLink )->pxPrev;            \
        }                                                               \
                                                                        \
        /* Make sure that this link is not part of any list anymore. */ \
        ( pxLink )->pxPrev = NULL;                                      \
        ( pxLink )->pxNext = NULL;                                      \
    }

/**
 * @brief Given the head of a list, checks if the list is empty.
 *
 * @param[in] pxHead The head of the given list.
 */
#define listIS_EMPTY( pxHead )    ( ( ( pxHead ) == NULL ) || ( ( pxHead )->pxNext == ( pxHead ) ) )

/**
 * @brief Removes the first node from the given list and returns it.
 *
 * Removes the first node from the given list and assigns it to the
 * pxLink parameter. If the list is empty, it assigns NULL to the
 * pxLink.
 *
 * @param[in] pxHead The head of the list from which to remove the
 * first node.
 * @param[out] pxLink The output parameter to receive the removed
 * node.
 */
#define listPOP( pxHead, pxLink )                                           \
    {                                                                       \
        if( listIS_EMPTY( ( pxHead ) ) )                                    \
        {                                                                   \
            ( pxLink ) = NULL;                                              \
        }                                                                   \
        else                                                                \
        {                                                                   \
            ( pxLink ) = ( pxHead )->pxNext;                                \
            /* If the link is part of a list, remove it from the list. */   \
            if( ( pxLink )->pxNext != NULL && ( pxLink )->pxPrev != NULL )  \
            {                                                               \
                ( pxLink )->pxPrev->pxNext = ( pxLink )->pxNext;            \
                ( pxLink )->pxNext->pxPrev = ( pxLink )->pxPrev;            \
            }                                                               \
                                                                            \
            /* Make sure that this link is not part of any list anymore. */ \
            ( pxLink )->pxPrev = NULL;                                      \
            ( pxLink )->pxNext = NULL;                                      \
        }                                                                   \
    }

/**
 * @brief Merges a list into a given list.
 *
 * @param[in] pxHeadResultList The head of the given list into which the
 * other list should be merged.
 * @param[in] pxHeadListToMerge The head of the list to be merged into the
 * given list.
 */
#define listMERGE( pxHeadResultList, pxHeadListToMerge )                                     \
    {                                                                                        \
        if( !listIS_EMPTY( ( pxHeadListToMerge ) ) )                                         \
        {                                                                                    \
            /* Setup links between last node of listToMerge and first node of resultList. */ \
            ( pxHeadListToMerge )->pxPrev->pxNext = ( pxHeadResultList )->pxNext;            \
            ( pxHeadResultList )->pxNext->pxPrev = ( pxHeadListToMerge )->pxPrev;            \
                                                                                             \
            /* Setup links between first node of listToMerge and the head of resultList. */  \
            ( pxHeadListToMerge )->pxNext->pxPrev = ( pxHeadResultList );                    \
            ( pxHeadResultList )->pxNext = ( pxHeadListToMerge )->pxNext;                    \
            /* Empty the merged list. */                                                     \
            listINIT_HEAD( ( pxHeadListToMerge ) );                                          \
        }                                                                                    \
    }

/**
 * @brief Helper macro to iterate over a list. pxLink contains the link node
 * in each iteration.
 */
#define listFOR_EACH( pxLink, pxHead )    \
    for( ( pxLink ) = ( pxHead )->pxNext; \
         ( pxLink ) != ( pxHead );        \
         ( pxLink ) = ( pxLink )->pxNext )

/**
 * @brief Helper macro to iterate over a list. It is safe to destroy/free the
 * nodes while iterating. pxLink contains the link node in each iteration.
 */
#define listFOR_EACH_SAFE( pxLink, pxTempLink, pxHead )                        \
    for( ( pxLink ) = ( pxHead )->pxNext, ( pxTempLink ) = ( pxLink )->pxNext; \
         ( pxLink ) != ( pxHead );                                             \
         ( pxLink ) = ( pxTempLink ), ( pxTempLink ) = ( pxLink )->pxNext )

/**
 * @brief Given the pointer to the link member (of type Link_t) in a struct,
 * extracts the pointer to the containing struct.
 *
 * @param[in] pxLink The pointer to the link member.
 * @param[in] type The type of the containing struct.
 * @param[in] member Name of the link member in the containing struct.
 */
#define listCONTAINER( pxLink, type, member )    ( ( type * ) ( ( uint8_t * ) ( pxLink ) - ( uint8_t * ) ( &( ( type * ) 0 )->member ) ) )

#endif /* _AWS_DOUBLY_LINKED_LIST_H_ */
