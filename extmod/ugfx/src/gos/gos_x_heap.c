/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "../../gfx.h"

#if GOS_NEED_X_HEAP

#include <string.h>				// Prototype for memcpy()


#if GFX_OS_HEAP_SIZE == 0
	#include <stdlib.h>				// Prototype for malloc(), realloc() and free()

	void _gosHeapInit(void) {
	}
	void *gfxAlloc(size_t sz) {
		return malloc(sz);
	}

	void *gfxRealloc(void *ptr, size_t oldsz, size_t newsz) {
		(void) oldsz;
		return realloc(ptr, newsz);
	}

	void gfxFree(void *ptr) {
		free(ptr);
	}

#else

	// Slot structure - user memory follows
	typedef struct memslot {
		struct memslot *next;		// The next memslot
		size_t			sz;			// Includes the size of this memslot.
		} memslot;

	// Free Slot - immediately follows the memslot structure
	typedef struct freeslot {
		memslot *nextfree;			// The next free slot
	} freeslot;

	#define GetSlotSize(sz)		((((sz) + (sizeof(freeslot) - 1)) & ~(sizeof(freeslot) - 1)) + sizeof(memslot))
	#define NextFree(pslot)		((freeslot *)Slot2Ptr(pslot))->nextfree
	#define Ptr2Slot(p)			((memslot *)(p) - 1)
	#define Slot2Ptr(pslot)		((pslot)+1)

	static memslot *			firstSlot;
	static memslot *			lastSlot;
	static memslot *			freeSlots;
	static char					heap[GFX_OS_HEAP_SIZE];

	void _gosHeapInit(void) {
		lastSlot = 0;
		gfxAddHeapBlock(heap, GFX_OS_HEAP_SIZE);
	}

	void gfxAddHeapBlock(void *ptr, size_t sz) {
		if (sz < sizeof(memslot)+sizeof(freeslot))
			return;

		if (lastSlot)
			lastSlot->next = (memslot *)ptr;
		else
			firstSlot = lastSlot = freeSlots = (memslot *)ptr;

		lastSlot->next = 0;
		lastSlot->sz = sz;
		NextFree(lastSlot) = 0;
	}

	void *gfxAlloc(size_t sz) {
		register memslot *prev, *p, *new;

		if (!sz) return 0;
		sz = GetSlotSize(sz);
		for (prev = 0, p = freeSlots; p != 0; prev = p, p = NextFree(p)) {
			// Loop till we have a block big enough
			if (p->sz < sz)
				continue;
			// Can we save some memory by splitting this block?
			if (p->sz >= sz + sizeof(memslot)+sizeof(freeslot)) {
				new = (memslot *)((char *)p + sz);
				new->next = p->next;
				p->next = new;
				new->sz = p->sz - sz;
				p->sz = sz;
				if (lastSlot == p)
					lastSlot = new;
				NextFree(new) = NextFree(p);
				NextFree(p) = new;
			}
			// Remove it from the free list
			if (prev)
				NextFree(prev) = NextFree(p);
			else
				freeSlots = NextFree(p);
			// Return the result found
			return Slot2Ptr(p);
		}
		// No slots large enough
		return 0;
	}

	void *gfxRealloc(void *ptr, size_t oldsz, size_t sz) {
		register memslot *prev, *p, *new;
		(void) oldsz;

		if (!ptr)
			return gfxAlloc(sz);
		if (!sz) {
			gfxFree(ptr);
			return 0;
		}

		p = Ptr2Slot(ptr);
		sz = GetSlotSize(sz);

		// If the next slot is free (and contiguous) merge it into this one
		if ((char *)p + p->sz == (char *)p->next) {
			for (prev = 0, new = freeSlots; new != 0; prev = new, new = NextFree(new)) {
				if (new == p->next) {
					p->next = new->next;
					p->sz += new->sz;
					if (prev)
						NextFree(prev) = NextFree(new);
					else
						freeSlots = NextFree(new);
					if (lastSlot == new)
						lastSlot = p;
					break;
				}
			}
		}

		// If this block is large enough we are nearly done
		if (sz < p->sz) {
			// Can we save some memory by splitting this block?
			if (p->sz >= sz + sizeof(memslot)+sizeof(freeslot)) {
				new = (memslot *)((char *)p + sz);
				new->next = p->next;
				p->next = new;
				new->sz = p->sz - sz;
				p->sz = sz;
				if (lastSlot == p)
					lastSlot = new;
				NextFree(new) = freeSlots;
				freeSlots = new;
			}
			return Slot2Ptr(p);
		}

		// We need to do this the hard way
		new = gfxAlloc(sz);
		if (new)
			return 0;
		memcpy(new, ptr, p->sz - sizeof(memslot));
		gfxFree(ptr);
		return new;
	}

	void gfxFree(void *ptr) {
		register memslot *prev, *p, *new;

		if (!ptr)
			return;

		p = Ptr2Slot(ptr);

		// If the next slot is free (and contiguous) merge it into this one
		if ((char *)p + p->sz == (char *)p->next) {
			for (prev = 0, new = freeSlots; new != 0; prev = new, new = NextFree(new)) {
				if (new == p->next) {
					p->next = new->next;
					p->sz += new->sz;
					if (prev)
						NextFree(prev) = NextFree(new);
					else
						freeSlots = NextFree(new);
					if (lastSlot == new)
						lastSlot = p;
					break;
				}
			}
		}

		// Add it into the free chain
		NextFree(p) = freeSlots;
		freeSlots = p;
	}
#endif

#endif /* GOS_NEED_X_HEAP */

#if GFX_EMULATE_MALLOC
	#include <stdlib.h>

	void* malloc(size_t size) {
		return gfxAlloc(size);
	}
	void free(void *ptr) {
		gfxFree(ptr);
	}
#endif
