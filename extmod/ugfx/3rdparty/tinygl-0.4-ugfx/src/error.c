#include <stdarg.h>
#include <assert.h>
#include "zgl.h"

void gl_fatal_error(char *format, ...)
{
  va_list ap;

  va_start(ap,format);

  fprintf(stderr,"TinyGL: fatal error: ");
  vfprintf(stderr,format,ap);
  fprintf(stderr,"\n");
  exit(1);

  va_end(ap);
}

void gl_assert(int test)
{
  assert(test);
}
