/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef _HAL_UTIL_H_
#define _HAL_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */
struct LIST_HEADER {
	struct LIST_HEADER *Next, *Prev;
};

typedef struct  LIST_HEADER     _LIST;

//#define RTL_LIST_HEAD_INIT(name) { &(name), &(name) }

#define RTL_INIT_LIST_HEAD(ptr) do { \
	(ptr)->Next = (ptr); (ptr)->Prev = (ptr); \
} while (0)


/*
 * Insert a new entry between two known consecutive entries. 
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
 static __inline__ VOID 
 __List_Add(
    IN  struct LIST_HEADER * New,
    IN  struct LIST_HEADER * Prev,
    IN  struct LIST_HEADER * Next
)
{
	Next->Prev = New;
	New->Next = Next;
	New->Prev = Prev;
	Prev->Next = New;
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
 static __inline__ VOID
 __List_Del(
    IN  struct LIST_HEADER * Prev,
    IN  struct LIST_HEADER * Next
 )
{
	Next->Prev = Prev;
	Prev->Next = Next;
}

/**
 * ListDel - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty on entry does not return true after this, the entry is in an undefined state.
 */
static __inline__ VOID 
ListDel(
    IN  struct LIST_HEADER *Entry
)
{
	__List_Del(Entry->Prev, Entry->Next);
}

/**
 * ListDelInit - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static  __inline__ VOID
ListDelInit(
    IN  struct LIST_HEADER *Entry
)
{
	__List_Del(Entry->Prev, Entry->Next);
	RTL_INIT_LIST_HEAD(Entry);

}

/**
 * ListEmpty - tests whether a list is empty
 * @head: the list to test.
 */
static __inline__ u32 
ListEmpty(
    IN  struct LIST_HEADER *Head
)
{
	return Head->Next == Head;
}

/**
 * ListSplice - join two lists
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static __inline__ VOID 
ListSplice(
    IN  struct LIST_HEADER *List,
    IN  struct LIST_HEADER *Head
)
{
	struct LIST_HEADER *First = List->Next;

	if (First != List) {
		struct LIST_HEADER *Last = List->Prev;
		struct LIST_HEADER *At = Head->Next;

		First->Prev = Head;
		Head->Next = First;

		Last->Next = At;
		At->Prev = Last;
	}
}

static __inline__ VOID 
ListAdd(
    IN  struct LIST_HEADER *New, 
    IN  struct LIST_HEADER *head
)
{
	__List_Add(New, head, head->Next);
}


static __inline__ VOID 
ListAddTail(
    IN  struct LIST_HEADER *New, 
    IN  struct LIST_HEADER *head
)
{
	__List_Add(New, head->Prev, head);
}

static __inline VOID 
RtlInitListhead(
    IN  _LIST *list
)
{
    RTL_INIT_LIST_HEAD(list);
}


/*
For the following list_xxx operations, 
caller must guarantee the atomic context.
Otherwise, there will be racing condition.
*/
static __inline u32	
RtlIsListEmpty(
    IN  _LIST *phead
)
{

	if (ListEmpty(phead))
		return _TRUE;
	else
		return _FALSE;
	
}

static __inline VOID 
RtlListInsertHead(
    IN  _LIST *plist,
    IN  _LIST *phead
)
{
	ListAdd(plist, phead);
}

static __inline VOID 
RtlListInsertTail(
    IN  _LIST *plist,
    IN  _LIST *phead
)
{
	ListAddTail(plist, phead);	
}


static __inline _LIST
*RtlListGetNext(
    IN  _LIST *plist
)
{
	return plist->Next;
}

static __inline VOID 
RtlListDelete(
    IN _LIST *plist
)
{
	ListDelInit(plist);
}

#define RTL_LIST_CONTAINOR(ptr, type, member) \
        ((type *)((char *)(ptr)-(SIZE_T)(&((type *)0)->member)))
        
#ifndef CONTAINER_OF
#define CONTAINER_OF(ptr, type, member) \
        ((type *)((char *)(ptr)-(SIZE_T)(&((type *)0)->member)))
#endif
/*
#define list_entry(ptr, type, member) \
 		CONTAINER_OF(ptr, type, member)

#define list_first_entry(ptr, type, member) \
        list_entry((ptr)->Next, type, member) 		

#define list_next_entry(pos, member, type) \
        list_entry((pos)->member.Next, type, member)

#define list_for_each_entry(pos, head, member, type) \
        for (pos = list_first_entry(head, type, member); \
             &pos->member != (head); \
             pos = list_next_entry(pos, member, type))
#define list_for_each(pos, head) \
        for (pos = (head)->Next; pos != (head); pos = pos->Next)
*/

#ifndef BIT
	#define BIT(x)	( 1 << (x))
#endif

#ifdef __cplusplus
}
#endif

#endif //_HAL_UTIL_H_
