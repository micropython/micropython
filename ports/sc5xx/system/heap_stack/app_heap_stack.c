
/*
** app_heap_stack.c generated on February 13, 2019 at 13:43:37.
**
** Copyright (C) 2019 Analog Devices Inc., All Rights Reserved.
**
** This file is generated automatically based upon the options selected in the
** Heap and Stack Configuration Editor. Changes to the heap and stack
** configuration should be made by changing the appropriate options rather than
** editing this file.
**
*/

#include <stdint.h>

#define __ADI_B  (1u)
#define __ADI_kB (1024u)
#define __ADI_MB (1024u * 1024u)

#define __ADI_SYSTEM_HEAP_SIZE (192u)
#define __ADI_SYSTEM_HEAP_SIZE_UNITS __ADI_MB
#define __ADI_SYSTEM_HEAP_ALIGN (4u)
#define __ADI_SYSTEM_STACK_SIZE (32u)
#define __ADI_SYSTEM_STACK_SIZE_UNITS __ADI_kB
#define __ADI_SYSTEM_STACK_ALIGN (64u)
#define __ADI_SUPERVISOR_STACK_SIZE (4u)
#define __ADI_SUPERVISOR_STACK_SIZE_UNITS __ADI_kB
#define __ADI_SUPERVISOR_STACK_ALIGN (64u)
#define __ADI_FIQ_STACK_SIZE (4u)
#define __ADI_FIQ_STACK_SIZE_UNITS __ADI_kB
#define __ADI_FIQ_STACK_ALIGN (64u)
#define __ADI_IRQ_STACK_SIZE (4u)
#define __ADI_IRQ_STACK_SIZE_UNITS __ADI_kB
#define __ADI_IRQ_STACK_ALIGN (64u)
#define __ADI_ABORT_STACK_SIZE (4u)
#define __ADI_ABORT_STACK_SIZE_UNITS __ADI_kB
#define __ADI_ABORT_STACK_ALIGN (64u)
#define __ADI_UNDEFINED_STACK_SIZE (4u)
#define __ADI_UNDEFINED_STACK_SIZE_UNITS __ADI_kB
#define __ADI_UNDEFINED_STACK_ALIGN (64u)

/* Size of system heap. */
uint8_t   __adi_heap_object[__ADI_SYSTEM_HEAP_SIZE * __ADI_SYSTEM_HEAP_SIZE_UNITS]
             __attribute__ ((aligned (__ADI_SYSTEM_HEAP_ALIGN))) 
             __attribute__ ((section (".heap,\"aw\",%nobits //")));

/* Size of system stack. */
uint8_t   __adi_stack_sys_object[__ADI_SYSTEM_STACK_SIZE * __ADI_SYSTEM_STACK_SIZE_UNITS]
             __attribute__ ((aligned (__ADI_SYSTEM_STACK_ALIGN))) 
             __attribute__ ((section (".stack_sys,\"aw\",%nobits //")));

/* Size of supervisor stack. */
uint8_t   __adi_stack_sup_object[__ADI_SUPERVISOR_STACK_SIZE * __ADI_SUPERVISOR_STACK_SIZE_UNITS]
             __attribute__ ((aligned (__ADI_SUPERVISOR_STACK_ALIGN))) 
             __attribute__ ((section (".stack_sup,\"aw\",%nobits //")));

/* Size of FIQ stack. */
uint8_t   __adi_stack_fiq_object[__ADI_FIQ_STACK_SIZE * __ADI_FIQ_STACK_SIZE_UNITS]
             __attribute__ ((aligned (__ADI_FIQ_STACK_ALIGN))) 
             __attribute__ ((section (".stack_fiq,\"aw\",%nobits //")));

/* Size of IRQ stack. */
uint8_t   __adi_stack_irq_object[__ADI_IRQ_STACK_SIZE * __ADI_IRQ_STACK_SIZE_UNITS]
             __attribute__ ((aligned (__ADI_IRQ_STACK_ALIGN))) 
             __attribute__ ((section (".stack_irq,\"aw\",%nobits //")));

/* Size of abort stack. */
uint8_t   __adi_stack_abort_object[__ADI_ABORT_STACK_SIZE * __ADI_ABORT_STACK_SIZE_UNITS]
             __attribute__ ((aligned (__ADI_ABORT_STACK_ALIGN))) 
             __attribute__ ((section (".stack_abort,\"aw\",%nobits //")));

/* Size of undefined stack. */
uint8_t   __adi_stack_undef_object[__ADI_UNDEFINED_STACK_SIZE * __ADI_UNDEFINED_STACK_SIZE_UNITS]
             __attribute__ ((aligned (__ADI_UNDEFINED_STACK_ALIGN))) 
             __attribute__ ((section (".stack_undef,\"aw\",%nobits //")));

