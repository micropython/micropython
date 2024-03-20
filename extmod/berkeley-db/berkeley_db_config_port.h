// Berkeley-db configuration.

#define __DBINTERFACE_PRIVATE 1
#define mpool_error printf
#define abort abort_
#define virt_fd_t void*

#ifdef MICROPY_BERKELEY_DB_DEFPSIZE
#define DEFPSIZE MICROPY_BERKELEY_DB_DEFPSIZE
#endif

#ifdef MICROPY_BERKELEY_DB_MINCACHE
#define MINCACHE MICROPY_BERKELEY_DB_MINCACHE
#endif

__attribute__((noreturn)) void abort_(void);
