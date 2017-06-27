#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <errno.h>
#include <reent.h>
/*
volatile void* heap_start = (void*) 0x02000000;
volatile void* heap_end = (void*) 0x02000000;

void* malloc ( size_t m )
{
    heap_start-= m;
    return (heap_start);
}

void* realloc ( void* ptr, size_t m )
{
    int* newPtr = malloc ( m );
    int* c = ptr;
    void* cpyPtr = newPtr;
    
    size_t k = 0;
    for ( ; k< m; k+=sizeof(int) )
    {
        *newPtr = *c;
        newPtr++;
        c++;
    }
    
    return cpyPtr;
}

void free ( void* m )
{
    // todo *sing*
}

*/

extern unsigned int heap_end;
unsigned int prev_heap_end;

/* Forward prototypes.  */
int     _system     _PARAMS ((const char *));
int     _rename     _PARAMS ((const char *, const char *));
int     isatty      _PARAMS ((int));
clock_t _times      _PARAMS ((struct tms *));
int     _gettimeofday   _PARAMS ((struct timeval *, struct timezone *));
void    _raise      _PARAMS ((void));
int     _unlink     _PARAMS ((void));
int     _link       _PARAMS ((void));
int     _stat       _PARAMS ((const char *, struct stat *));
int     _fstat      _PARAMS ((int, struct stat *));
caddr_t _sbrk       _PARAMS ((int));
int     _getpid     _PARAMS ((int));
int     _kill       _PARAMS ((int, int));
void    _exit       _PARAMS ((int));
int     _close      _PARAMS ((int));
int     _open       _PARAMS ((const char *, int, ...));
int     _write      _PARAMS ((int, char *, int));
int     _lseek      _PARAMS ((int, int, int));
int     _read       _PARAMS ((int, char *, int));
void    initialise_monitor_handles _PARAMS ((void));

//static int
//remap_handle (int fh)
//{
    //return 0;
//}

int _write ( int fd, char* ptr, int size )
{
    pios_uart_write ( ptr, size );
}

#if 1
void
initialise_monitor_handles (void)
{
}

//static int
//get_errno (void)
//{
    //return(0);
//}

//static int
//error (int result)
//{
  //errno = get_errno ();
  //return result;
//}


int
_read (int file,
       char * ptr,
       int len)
{
  return len;
}


int
_lseek (int file,
    int ptr,
    int dir)
{
    return 0;
}

int
_open (const char * path,
       int          flags,
       ...)
{
    return 0;
}


int
_close (int file)
{
    return 0;
}

void
_exit (int n)
{
    printf("Called Exit, restarting\n");
    
    main (0,0 );
}

int
_kill (int n, int m)
{
    return(0);
}

int
_getpid (int n)
{
  return 1;
  n = n;
}


caddr_t
_sbrk (int incr)
{
    prev_heap_end = heap_end;
    heap_end += incr;
    printf ("_SBRK ( %d ), HEAP_END: 0x%08x, STACK_TOP: 0x%08x\n", incr, heap_end, getSP());
    return (caddr_t) prev_heap_end;
}

int
_fstat (int file, struct stat * st)
{
  return 0;
}

int _stat (const char *fname, struct stat *st)
{
  return 0;
}

int
_link (void)
{
  return -1;
}

int
_unlink (void)
{
  return -1;
}

void
_raise (void)
{
  return;
}

int
_gettimeofday (struct timeval * tp, struct timezone * tzp)
{
    if(tp)
    {
        tp->tv_sec = 10;
        tp->tv_usec = 0;
    }
    if (tzp)
    {
        tzp->tz_minuteswest = 0;
        tzp->tz_dsttime = 0;
    }
    return 0;
}

clock_t
_times (struct tms * tp)
{
    clock_t timeval;

    timeval = 10;
    if (tp)
    {
        tp->tms_utime  = timeval;   /* user time */
        tp->tms_stime  = 0; /* system time */
        tp->tms_cutime = 0; /* user time, children */
        tp->tms_cstime = 0; /* system time, children */
    }
    return timeval;
};


int
_isatty (int fd)
{
  return 1;
  fd = fd;
}

int
_system (const char *s)
{
  if (s == NULL)
    return 0;
  errno = ENOSYS;
  return -1;
}

int
_rename (const char * oldpath, const char * newpath)
{
  errno = ENOSYS;
  return -1;
}
#endif
