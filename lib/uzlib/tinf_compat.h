/* This header contains compatibility defines for the original tinf API
   and uzlib 2.x and below API. These defines are deprecated and going
   to be removed in the future, so applications should migrate to new
   uzlib API. */
#define TINF_DATA struct uzlib_uncomp

#define destSize dest_size
#define destStart dest_start
#define readSource source_read_cb
