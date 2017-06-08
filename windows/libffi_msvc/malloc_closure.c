/*
* This file is from CPython's Modules/_ctypes/malloc_closure.c
* and has received some edits.
*/

#include <windows.h>
#include <assert.h>
#include <ffi.h>

/* 'allocate_num_pages' is dynamically adjusted starting from one
   page.  It grows by a factor of PAGE_ALLOCATION_GROWTH_RATE.  This is
   meant to handle both the common case of not needing a lot of pages,
   and the rare case of needing many of them.  Systems in general have a
   limit of how many mmap'd blocks can be open.
*/

#define PAGE_ALLOCATION_GROWTH_RATE  1.3
static size_t allocate_num_pages = 0;

/* #define MALLOC_CLOSURE_DEBUG */ /* enable for some debugging output */

/******************************************************************/

union mmaped_block {
    ffi_closure closure;
    union mmaped_block *next;
};

static union mmaped_block *free_list = 0;
static size_t _pagesize = 0;

static void more_core(void)
{
    union mmaped_block *item;
    size_t count, i;

    /* determine the pagesize */
    if (!_pagesize) {
        SYSTEM_INFO systeminfo;
        GetSystemInfo(&systeminfo);
        _pagesize = systeminfo.dwPageSize;
        if (_pagesize <= 0)
            _pagesize = 4096;
    }

    /* bump 'allocate_num_pages' */
    allocate_num_pages = 1 + (
        (size_t)(allocate_num_pages * PAGE_ALLOCATION_GROWTH_RATE));

    /* calculate the number of mmaped_blocks to allocate */
    count = (allocate_num_pages * _pagesize) / sizeof(union mmaped_block);

    /* allocate a memory block */
    item = (union mmaped_block *)VirtualAlloc(NULL,
                                           count * sizeof(union mmaped_block),
                                           MEM_COMMIT,
                                           PAGE_EXECUTE_READWRITE);
    if (item == NULL)
        return;

#ifdef MALLOC_CLOSURE_DEBUG
    printf("block at %p allocated (%ld bytes), %ld mmaped_blocks\n",
           item, (long)(allocate_num_pages * _pagesize), (long)count);
#endif
    /* put them into the free list */
    for (i = 0; i < count; ++i) {
        item->next = free_list;
        free_list = item;
        ++item;
    }
}

/******************************************************************/

/* put the item back into the free list */
void ffi_closure_free(void *p)
{
    union mmaped_block *item = (union mmaped_block *)p;
    item->next = free_list;
    free_list = item;
}

/* return one item from the free list, allocating more if needed */
void *ffi_closure_alloc(size_t size, void **code)
{
    union mmaped_block *item;
    void *result;
    assert(size == sizeof(ffi_closure));
    if (!free_list)
        more_core();
    if (!free_list)
        return NULL;
    item = free_list;
    free_list = item->next;
    result = &item->closure;
    *code = result;
    return result;
}
