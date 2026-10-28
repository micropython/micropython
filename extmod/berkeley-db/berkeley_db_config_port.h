// Berkeley-db configuration.

#define __DBINTERFACE_PRIVATE 1
#define mpool_error printf
#define abort abort_
#define virt_fd_t void *

// berkeley-db 1.xx uses the legacy sys/cdefs.h macros (__P, __BEGIN_DECLS,
// __END_DECLS) which some toolchains no longer provide (eg the newlib used
// by ESP-IDF v6). db.h includes this file (as "berkeley-db/config.h") after
// <sys/cdefs.h> and before its first use of these macros, so provide them
// here when missing. Toolchains that do define them are unaffected.
#ifndef __BEGIN_DECLS
#define __BEGIN_DECLS
#endif
#ifndef __END_DECLS
#define __END_DECLS
#endif
#ifndef __P
#define __P(protos) protos
#endif

#ifdef MICROPY_BERKELEY_DB_DEFPSIZE
#define DEFPSIZE MICROPY_BERKELEY_DB_DEFPSIZE
#endif

#ifdef MICROPY_BERKELEY_DB_MINCACHE
#define MINCACHE MICROPY_BERKELEY_DB_MINCACHE
#endif

__attribute__((noreturn)) void abort_(void);
