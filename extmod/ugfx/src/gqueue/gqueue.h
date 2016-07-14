/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gqueue/gqueue.h
 * @brief   GQUEUE header file.
 *
 * @addtogroup GQUEUE
 *
 * @brief	Module which provides queue management (only internally used)
 *
 * @details	There are 3 types of queues:
 * 			<ul><li><b>Asynchronous Queues (ASync) </b> - Queue operations never block</li>
 * 				<li><b>Get Synchronous Queues (GSync) </b> - Queue Get operations block until something is placed in the Queue</li>
 * 				<li><b>Put Synchronous Queues (PSync)</b> - Queue Put operations block until the element is removed from the Queue</li>
 * 				<li><b>Fully Synchronous Queues (FSync)</b> - Queue GET and Put operations block</li>
 * 			</ul>
 * 			We need 4 types of queues even though fully synchronous queues support all operations including asynchronous
 * 			operations because fully synchronous queues have the highest storage requirements. The other queue types are
 * 			optimizations. Efficiency IS important to use (particularly RAM efficiency).
 * 			In practice we only implement ASync, GSync and FSync queues as PSync queues are of dubious value.
 * 			<br>
 * 			We also define GDataBuffer which is a data buffer that supports being queued.
 * @{
 */

#ifndef _GQUEUE_H
#define _GQUEUE_H

#if GFX_USE_GQUEUE || defined(__DOXYGEN__)

/**
 * @brief	A queue item
 * @{
 */
typedef struct gfxQueueASyncItem {
	struct gfxQueueASyncItem	*next;
} gfxQueueASyncItem, gfxQueueGSyncItem;

typedef struct gfxQueueFSyncItem {
	struct gfxQueueFSyncItem	*next;
	gfxSem						sem;
} gfxQueueFSyncItem;
/** @} */

/**
 * @brief	A queue
 * @{
 */
typedef struct gfxQueueASync {
	gfxQueueASyncItem	*head;
	gfxQueueASyncItem	*tail;
} gfxQueueASync;

typedef struct gfxQueueGSync {
	gfxQueueGSyncItem	*head;
	gfxQueueGSyncItem	*tail;
	gfxSem				sem;
} gfxQueueGSync;

typedef struct gfxQueueFSync {
	gfxQueueFSyncItem	*head;
	gfxQueueFSyncItem	*tail;
	gfxSem				sem;
} gfxQueueFSync;
/** @} */

/**
 * @brief	A Data Buffer Queue
 * @note	This structure is followed immediately by the data itself.
 * 			When allocating the buffers for the data put this structure
 * 			at the beginning of the buffer.
 */
typedef struct GDataBuffer {
	gfxQueueGSyncItem	next;		// @< Used for queueing the buffers
	size_t				size;		// @< The size of the buffer area following this structure (in bytes)
	size_t				len;		// @< The length of the data in the buffer area (in bytes)
} GDataBuffer;

/*===========================================================================*/
/* Function declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name	Initialisation functions
 * @brief	Initialise a queue.
 *
 * @param[in]	pqueue	A pointer to the queue
 *
 * @note	Whilst queues are normally FIFO, a GFX queue also supports push and pop operations.
 * 			A pop operation is the same as normal get from the queue but a push places the item
 * 			at the head of the queue instead of the tail (as a put would).
 *
 * @api
 * @{
 */
void gfxQueueASyncInit(gfxQueueASync *pqueue);
void gfxQueueGSyncInit(gfxQueueGSync *pqueue);
void gfxQueueFSyncInit(gfxQueueFSync *pqueue);
/** @} */

/**
 * @name	Deinitialisation functions
 * @brief	De-Initialise a queue.
 *
 * @param[in]	pqueue	A pointer to the queue
 *
 * @api
 * @{
 */
#define gfxQueueASyncDeinit(pqueue)
void gfxQueueGSyncDeinit(gfxQueueGSync *pqueue);
void gfxQueueFSyncDeinit(gfxQueueFSync *pqueue);
/** @} */

/**
 * @name	Get() Functions
 * @brief	Get an item from the head of the queue (and remove it from the queue).
 * @return	NULL if the timeout expires before an item is available
 *
 * @param[in]	pqueue	A pointer to the queue
 * @param[in]	ms		The maxmimum time to wait for an item. For ASync queues this parameter is
 * 						not specified as TIME_IMMEDIATE is assumed.
 *
 * @note		The routines ending in "I" are interrupt/system/iclass level routines.
 *
 * @api
 * @{
 */
gfxQueueASyncItem *gfxQueueASyncGet(gfxQueueASync *pqueue);
gfxQueueASyncItem *gfxQueueASyncGetI(gfxQueueASync *pqueue);
gfxQueueGSyncItem *gfxQueueGSyncGet(gfxQueueGSync *pqueue, delaytime_t ms);
gfxQueueGSyncItem *gfxQueueGSyncGetI(gfxQueueGSync *pqueue);
gfxQueueFSyncItem *gfxQueueFSyncGet(gfxQueueFSync *pqueue, delaytime_t ms);
/** @} */

/**
 * @name	Put() Functions
 * @brief	Put an item on the end of the queue.
 * @return	none for ASync and GSync queues; For FSync queues - FALSE on timeout, otherwise TRUE
 *
 * @param[in]	pqueue	A pointer to the queue
 * @param[in]	pitem	A pointer to the queue item
 * @param[in]	ms		The maxmimum time to wait for an item to be removed from the queue (only for FSync queues)
 *
 * @note		FSync: Use a delay time of TIME_IMMEDIATE if you don't want to wait until the
 * 				item is removed from the queue. Note that even if the timeout occurs - the item
 * 				remains in the queue.
 * @note		The routines ending in "I" are interrupt/system/iclass level routines.
 *
 * @api
 * @{
 */
void gfxQueueASyncPut(gfxQueueASync *pqueue, gfxQueueASyncItem *pitem);
void gfxQueueASyncPutI(gfxQueueASync *pqueue, gfxQueueASyncItem *pitem);
void gfxQueueGSyncPut(gfxQueueGSync *pqueue, gfxQueueGSyncItem *pitem);
void gfxQueueGSyncPutI(gfxQueueGSync *pqueue, gfxQueueGSyncItem *pitem);
bool_t gfxQueueFSyncPut(gfxQueueFSync *pqueue, gfxQueueFSyncItem *pitem, delaytime_t ms);
/** @} */

/**
 * @name	Pop() Functions
 * @brief	Pop an item from the head of the queue (and remove it from the queue).
 * @details	This is exactly the same as the Get operation above.
 *
 * @api
 * @{
 */
#define gfxQueueASyncPop(pqueue)			gfxQueueASyncGet(pqueue)
#define gfxQueueASyncPopI(pqueue)			gfxQueueASyncGetI(pqueue)
#define gfxQueueGSyncPop(pqueue, ms)		gfxQueueGSyncGet(pqueue, ms)
#define gfxQueueFSyncPop(pqueue, ms)		gfxQueueFSyncGet(pqueue, ms)
/** @} */

/**
 * @name	Push() Functions
 * @brief	Push an item into the start of the queue.
 * @return	none for ASync and GSync queues; For FSync queues - FALSE on timeout, otherwise TRUE
 *
 * @param[in]	pqueue	A pointer to the queue
 * @param[in]	pitem	A pointer to the queue item
 * @param[in]	ms		The maxmimum time to wait for an item to be popped (only for FSync queues)
 *
 * @note		FSync: Use a delay time of TIME_IMMEDIATE if you don't want to wait until the
 * 				item is removed from the queue. Note that even if the timeout occurs - the item
 * 				remains in the queue.
 * @note		The routines ending in "I" are interrupt/system/iclass level routines.
 *
 * @api
 * @{
 */
void gfxQueueASyncPush(gfxQueueASync *pqueue, gfxQueueASyncItem *pitem);
void gfxQueueASyncPushI(gfxQueueASync *pqueue, gfxQueueASyncItem *pitem);
void gfxQueueGSyncPush(gfxQueueGSync *pqueue, gfxQueueGSyncItem *pitem);
void gfxQueueGSyncPushI(gfxQueueGSync *pqueue, gfxQueueGSyncItem *pitem);
bool_t gfxQueueFSyncPush(gfxQueueFSync *pqueue, gfxQueueFSyncItem *pitem, delaytime_t ms);
/** @} */

/**
 * @name	Insert() Functions
 * @brief	Insert an item on the queue after the specified item.
 * @return	none for ASync and GSync queues; For FSync queues - FALSE on timeout, otherwise TRUE
 *
 * @param[in]	pqueue	A pointer to the queue
 * @param[in]	pitem	A pointer to the queue item
 * @param[in]	pafter	A pointer to the queue item this new item must be inserted after. If NULL or
 * 							pafter can't be found in the queue, it puts the new item at the end of the queue.
 * @param[in]	ms		The maxmimum time to wait for an item to be removed from the queue (only for FSync queues)
 *
 * @note		FSync: Use a delay time of TIME_IMMEDIATE if you don't want to wait until the
 * 				item is removed from the queue. Note that even if the timeout occurs - the item
 * 				remains in the queue.
 * @note		The routines ending in "I" are interrupt/system/iclass level routines.
 *
 * @api
 * @{
 */
void gfxQueueASyncInsert(gfxQueueASync *pqueue, gfxQueueASyncItem *pitem, gfxQueueASyncItem *pafter);
void gfxQueueASyncInsertI(gfxQueueASync *pqueue, gfxQueueASyncItem *pitem, gfxQueueASyncItem *pafter);
void gfxQueueGSyncInsert(gfxQueueGSync *pqueue, gfxQueueGSyncItem *pitem, gfxQueueASyncItem *pafter);
void gfxQueueGSyncInsertI(gfxQueueGSync *pqueue, gfxQueueGSyncItem *pitem, gfxQueueASyncItem *pafter);
bool_t gfxQueueFSyncInsert(gfxQueueFSync *pqueue, gfxQueueFSyncItem *pitem, gfxQueueASyncItem *pafter, delaytime_t ms);
/** @} */

/**
 * @name	Remove() Functions
 * @brief	Remove an item from the queue.
 * @note	Removes the specified item from the queue where-ever it is in the queue
 *
 * @param[in]	pqueue	A pointer to the queue
 * @param[in]	pitem	A pointer to the queue item
 *
 * @note	If the item isn't in the queue the routine just returns.
 * @note	If a process is waiting on the Put/Push operation for the item, that process
 * 			will be signaled.
 * @note	The routines ending in "I" are interrupt/system/iclass level routines.
 *
 * @api
 * @{
 */
void gfxQueueASyncRemove(gfxQueueASync *pqueue, gfxQueueASyncItem *pitem);
void gfxQueueASyncRemoveI(gfxQueueASync *pqueue, gfxQueueASyncItem *pitem);
void gfxQueueGSyncRemove(gfxQueueGSync *pqueue, gfxQueueGSyncItem *pitem);
void gfxQueueGSyncRemoveI(gfxQueueGSync *pqueue, gfxQueueGSyncItem *pitem);
void gfxQueueFSyncRemove(gfxQueueFSync *pqueue, gfxQueueFSyncItem *pitem);
/** @} */

/**
 * @name	isEmpty() Functions
 * @brief	Is the queue empty?
 * @return	TRUE if the queue is empty
 *
 * @param[in]	pqueue	A pointer to the queue
 *
 * @note	The routines ending in "I" are interrupt/system/iclass level routines.
 *
 * @api
 * @{
 */
#define gfxQueueASyncIsEmpty(pqueue)		((pqueue)->head == 0)
#define gfxQueueASyncIsEmptyI(pqueue)		((pqueue)->head == 0)
#define gfxQueueGSyncIsEmpty(pqueue)		((pqueue)->head == 0)
#define gfxQueueGSyncIsEmptyI(pqueue)		((pqueue)->head == 0)
#define gfxQueueFSyncIsEmpty(pqueue)		((pqueue)->head == 0)
#define gfxQueueFSyncIsEmptyI(pqueue)		((pqueue)->head == 0)
/** @} */

/**
 * @name	IsInQueue() Functions
 * @brief	Is an item in the queue?
 * @return	TRUE if the item is in the queue?
 *
 * @param[in]	pqueue	A pointer to the queue
 * @param[in]	pitem	A pointer to the queue item
 *
 * @note	This operation may be expensive.
 * @note	The routines ending in "I" are interrupt/system/iclass level routines.
 *
 * @api
 * @{
 */
bool_t gfxQueueASyncIsIn(gfxQueueASync *pqueue, const gfxQueueASyncItem *pitem);
bool_t gfxQueueASyncIsInI(gfxQueueASync *pqueue, const gfxQueueASyncItem *pitem);
bool_t gfxQueueGSyncIsIn(gfxQueueGSync *pqueue, const gfxQueueGSyncItem *pitem);
bool_t gfxQueueGSyncIsInI(gfxQueueGSync *pqueue, const gfxQueueGSyncItem *pitem);
bool_t gfxQueueFSyncIsIn(gfxQueueFSync *pqueue, const gfxQueueFSyncItem *pitem);
bool_t gfxQueueFSyncIsInI(gfxQueueFSync *pqueue, const gfxQueueFSyncItem *pitem);
/** @} */

/**
 * @name	Peek() Functions
 * @brief	Get the first item from the head of the queue but do not remove it from the queue.
 * @return	NULL if no item is available.
 *
 * @param[in]	pqueue	A pointer to the queue
 *
 * @note	This call does not block.
 * @note	This can be used as the first call to iterate all the elements in the queue.
 * @note	As that item is still on the queue, it should be treated as read-only. It could
 * 			also be removed from the queue at any time by another thread (thereby altering the
 * 			queue item).
 * @note	The routines ending in "I" are interrupt/system/iclass level routines.
 *
 * @api
 * @{
 */
#define gfxQueueASyncPeek(pqueue)	((const gfxQueueASyncItem *)((pqueue)->head))
#define gfxQueueASyncPeekI(pqueue)	((const gfxQueueASyncItem *)((pqueue)->head))
#define gfxQueueGSyncPeek(pqueue)	((const gfxQueueGSyncItem *)((pqueue)->head))
#define gfxQueueGSyncPeekI(pqueue)	((const gfxQueueGSyncItem *)((pqueue)->head))
#define gfxQueueFSyncPeek(pqueue)	((const gfxQueueFSyncItem *)((pqueue)->head))
#define gfxQueueFSyncPeekI(pqueue)	((const gfxQueueFSyncItem *)((pqueue)->head))
/** @} */

/**
 * @name	Next() Functions
 * @brief	Get the next item in the queue (but do not remove it from the queue).
 * @return	NULL if no item is available.
 *
 * @param[in]	pitem	The previous item in the queue
 *
 * @note	This call does not block.
 * @note	This can be used as subsequent calls to iterate all the elements in the queue.
 * @note	As that item is still on the queue, it should be treated as read-only. It could
 * 			also be removed from the queue at any time by another thread (thereby altering the
 * 			queue item).
 * @note	The routines ending in "I" are interrupt/system/iclass level routines.
 *
 * @api
 * @{
 */
#define gfxQueueASyncNext(pitem)	((const gfxQueueASyncItem *)((pitem)->next))
#define gfxQueueASyncNextI(pitem)	((const gfxQueueASyncItem *)((pitem)->next))
#define gfxQueueGSyncNext(pitem)	((const gfxQueueGSyncItem *)((pitem)->next))
#define gfxQueueGSyncNextI(pitem)	((const gfxQueueGSyncItem *)((pitem)->next))
#define gfxQueueFSyncNext(pitem)	((const gfxQueueFSyncItem *)((pitem)->next))
#define gfxQueueFSyncNextI(pitem)	((const gfxQueueFSyncItem *)((pitem)->next))
/** @} */

/**
 * @name		BufferAlloc() Functions
 * @brief		Allocate some buffers and put them on the free list
 * @return		TRUE is it succeeded. FALSE on allocation failure.
 *
 * @param[in] num	The number of buffers to allocate
 * @param[in] size	The size (in bytes) of each buffer
 *
 * @api
 * @{
 */
bool_t gfxBufferAlloc(unsigned num, size_t size);
/** @} */

/**
 * @name		BufferIsAvailable() Functions
 * @brief		Is there one or more buffers currently available on the free list
 * @return		TRUE if there are buffers in the free list
 *
 * @api
 * @{
 */
bool_t gfxBufferIsAvailable(void);
/** @} */

/**
 * @name		BufferGet() Functions
 * @brief		Get a buffer from the free list
 * @return		A GDataBuffer pointer or NULL if the timeout is exceeded
 *
 * @param[in] ms	The maximum amount of time in milliseconds to wait for a buffer if one is not available.
 *
 * @api
 * @{
 */
GDataBuffer *gfxBufferGet(delaytime_t ms);
GDataBuffer *gfxBufferGetI(void);
/** @} */

/**
 * @name		BufferRelease() Functions
 * @brief		Release a buffer back to the free list
 *
 * @param[in] pd		The buffer to put (back) on the free-list.
 *
 * @note		This call should be used to return any buffers that were taken from
 * 				the free-list once they have been finished with. It can also be used
 * 				to put new buffers onto the free-list. Just make sure the "size" field
 * 				of the GDataBuffer structure has been filled in first.
 *
 * @api
 * @{
 */
void gfxBufferRelease(GDataBuffer *pd);
void gfxBufferReleaseI(GDataBuffer *pd);
/** @} */


#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GQUEUE */
#endif /* _GQUEUE_H */
/** @} */
