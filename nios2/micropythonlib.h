#include <stdint.h>
#include <stdio.h>
#include <string.h>


#ifdef __cplusplus
extern "C" {
#endif

/* initialize */
void init_mp();
/* run a python string */
void do_pystr(const char *src);
/* de-initialize */
void deinit_mp();

/* initialize, run string, and de-initialize */
void run_pystr(const char *src);

#ifdef __cplusplus
} /* extern "C" */
#endif
