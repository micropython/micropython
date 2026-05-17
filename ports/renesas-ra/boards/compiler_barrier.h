// we need to provide a definition of __COMPILER_BARRIER() defined in cmsis/cmsis_gcc.h V5.4.1 for FSP v4.4.0
#ifndef   __COMPILER_BARRIER
  #define __COMPILER_BARRIER()                   __ASM volatile ("" ::: "memory")
#endif
