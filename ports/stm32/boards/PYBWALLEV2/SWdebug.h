#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "stdarg.h"     /* va_start() */
#include "lib/oofatfs/ff.h"
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"

#define TRACE_BUFFER_SIZE_C 1024

int init_trace(char* debug_storage);

int write_trace(char* log, ...);

int close_trace();
