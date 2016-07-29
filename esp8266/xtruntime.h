/*
 * xtruntime.h  --  general C definitions for single-threaded run-time
 *
 * Copyright (c) 2002-2008 Tensilica Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef XTRUNTIME_H
#define XTRUNTIME_H

#include <xtensa/config/core.h>
#include <xtensa/config/specreg.h>

#ifndef XTSTR
#define _XTSTR(x)	# x
#define XTSTR(x)	_XTSTR(x)
#endif

#define _xtos_set_execption_handler _xtos_set_exception_handler	/* backward compatibility */
#define _xtos_set_saved_intenable	_xtos_ints_on	/* backward compatibility */
#define _xtos_clear_saved_intenable	_xtos_ints_off	/* backward compatibility */

#if !defined(_ASMLANGUAGE) && !defined(__ASSEMBLER__)

#ifdef __cplusplus
extern "C" {
#endif

/*typedef void (_xtos_timerdelta_func)(int);*/
#ifdef __cplusplus
typedef void (_xtos_handler_func)(...);
#else
typedef void (_xtos_handler_func)();
#endif
typedef _xtos_handler_func *_xtos_handler;

/*
 *  unsigned XTOS_SET_INTLEVEL(int intlevel);
 *  This macro sets the current interrupt level.
 *  The 'intlevel' parameter must be a constant.
 *  This macro returns a 32-bit value that must be passed to
 *  XTOS_RESTORE_INTLEVEL() to restore the previous interrupt level.
 *  XTOS_RESTORE_JUST_INTLEVEL() also does this, but in XEA2 configs
 *  it restores only PS.INTLEVEL rather than the entire PS register
 *  and thus is slower.
 */
#if !XCHAL_HAVE_INTERRUPTS
# define XTOS_SET_INTLEVEL(intlevel)		0
# define XTOS_SET_MIN_INTLEVEL(intlevel)	0
# define XTOS_RESTORE_INTLEVEL(restoreval)
# define XTOS_RESTORE_JUST_INTLEVEL(restoreval)
#elif XCHAL_HAVE_XEA2
/*  In XEA2, we can simply safely set PS.INTLEVEL directly:  */
/*  NOTE: these asm macros don't modify memory, but they are marked
 *  as such to act as memory access barriers to the compiler because
 *  these macros are sometimes used to delineate critical sections;
 *  function calls are natural barriers (the compiler does not know
 *  whether a function modifies memory) unless declared to be inlined.  */
# define XTOS_SET_INTLEVEL(intlevel)		({ unsigned __tmp; \
			__asm__ __volatile__(	"rsil	%0, " XTSTR(intlevel) "\n" \
						: "=a" (__tmp) : : "memory" ); \
			__tmp;})
# define XTOS_SET_MIN_INTLEVEL(intlevel)		({ unsigned __tmp, __tmp2, __tmp3; \
			__asm__ __volatile__(	"rsr	%0, " XTSTR(PS) "\n"	/* get old (current) PS.INTLEVEL */ \
						"movi	%2, " XTSTR(intlevel) "\n" \
						"extui	%1, %0, 0, 4\n"	/* keep only INTLEVEL bits of parameter */ \
						"blt	%2, %1, 1f\n" \
						"rsil	%0, " XTSTR(intlevel) "\n" \
						"1:\n" \
						: "=a" (__tmp), "=&a" (__tmp2), "=&a" (__tmp3) : : "memory" ); \
			__tmp;})
# define XTOS_RESTORE_INTLEVEL(restoreval)	do{ unsigned __tmp = (restoreval); \
			__asm__ __volatile__(	"wsr	%0, " XTSTR(PS) " ; rsync\n" \
						: : "a" (__tmp) : "memory" ); \
			}while(0)
# define XTOS_RESTORE_JUST_INTLEVEL(restoreval)	_xtos_set_intlevel(restoreval)
#else
/*  In XEA1, we have to rely on INTENABLE register virtualization:  */
extern unsigned		_xtos_set_vpri( unsigned vpri );
extern unsigned		_xtos_vpri_enabled;	/* current virtual priority */
# define XTOS_SET_INTLEVEL(intlevel)		_xtos_set_vpri(~XCHAL_INTLEVEL_ANDBELOW_MASK(intlevel))
# define XTOS_SET_MIN_INTLEVEL(intlevel)	_xtos_set_vpri(_xtos_vpri_enabled & ~XCHAL_INTLEVEL_ANDBELOW_MASK(intlevel))
# define XTOS_RESTORE_INTLEVEL(restoreval)	_xtos_set_vpri(restoreval)
# define XTOS_RESTORE_JUST_INTLEVEL(restoreval)	_xtos_set_vpri(restoreval)
#endif

/*
 *  The following macros build upon the above.  They are generally used
 *  instead of invoking the SET_INTLEVEL and SET_MIN_INTLEVEL macros directly.
 *  They all return a value that can be used with XTOS_RESTORE_INTLEVEL()
 *  or _xtos_restore_intlevel() or _xtos_restore_just_intlevel() to restore
 *  the effective interrupt level to what it was before the macro was invoked.
 *  In XEA2, the DISABLE macros are much faster than the MASK macros
 *  (in all configs, DISABLE sets the effective interrupt level, whereas MASK
 *  makes ensures the effective interrupt level is at least the level given
 *  without lowering it; in XEA2 with INTENABLE virtualization, these macros
 *  affect PS.INTLEVEL only, not the virtual priority, so DISABLE has partial
 *  MASK semantics).
 *
 *  A typical critical section sequence might be:
 *	unsigned rval = XTOS_DISABLE_EXCM_INTERRUPTS;
 *	... critical section ...
 *	XTOS_RESTORE_INTLEVEL(rval);
 */
/*  Enable all interrupts (those activated with _xtos_ints_on()):  */
#define XTOS_ENABLE_INTERRUPTS		XTOS_SET_INTLEVEL(0)
/*  Disable low priority level interrupts (they can interact with the OS):  */
#define XTOS_DISABLE_LOWPRI_INTERRUPTS	XTOS_SET_INTLEVEL(XCHAL_NUM_LOWPRI_LEVELS)
#define XTOS_MASK_LOWPRI_INTERRUPTS	XTOS_SET_MIN_INTLEVEL(XCHAL_NUM_LOWPRI_LEVELS)
/*  Disable interrupts that can interact with the OS:  */
#define XTOS_DISABLE_EXCM_INTERRUPTS	XTOS_SET_INTLEVEL(XCHAL_EXCM_LEVEL)
#define XTOS_MASK_EXCM_INTERRUPTS	XTOS_SET_MIN_INTLEVEL(XCHAL_EXCM_LEVEL)
#if 0 /* XTOS_LOCK_LEVEL is not exported to applications */
/*  Disable interrupts that can interact with the OS, or manipulate virtual INTENABLE:  */
#define XTOS_DISABLE_LOCK_INTERRUPTS	XTOS_SET_INTLEVEL(XTOS_LOCK_LEVEL)
#define XTOS_MASK_LOCK_INTERRUPTS	XTOS_SET_MIN_INTLEVEL(XTOS_LOCK_LEVEL)
#endif
/*  Disable ALL interrupts (not for common use, particularly if one's processor
 *  configuration has high-level interrupts and one cares about their latency):  */
#define XTOS_DISABLE_ALL_INTERRUPTS	XTOS_SET_INTLEVEL(15)


extern unsigned int	_xtos_ints_off( unsigned int mask );
extern unsigned int	_xtos_ints_on( unsigned int mask );
extern unsigned		_xtos_set_intlevel( int intlevel );
extern unsigned		_xtos_set_min_intlevel( int intlevel );
extern unsigned		_xtos_restore_intlevel( unsigned restoreval );
extern unsigned		_xtos_restore_just_intlevel( unsigned restoreval );
extern _xtos_handler	_xtos_set_interrupt_handler( int n, _xtos_handler f );
extern _xtos_handler	_xtos_set_interrupt_handler_arg( int n, _xtos_handler f, void *arg );
extern _xtos_handler	_xtos_set_exception_handler( int n, _xtos_handler f );

extern void		_xtos_memep_initrams( void );
extern void		_xtos_memep_enable( int flags );

/*  Deprecated (but kept because they were documented):  */
extern unsigned int	_xtos_read_ints( void );		/* use xthal_get_interrupt() instead */
extern void		_xtos_clear_ints( unsigned int mask );	/* use xthal_set_intclear() instead */

#if XCHAL_NUM_CONTEXTS > 1
extern unsigned		_xtos_init_context(int context_num, int stack_size,
					    _xtos_handler_func *start_func, int arg1);
#endif

/*  Deprecated:  */
#if XCHAL_NUM_TIMERS > 0
extern void		_xtos_timer_0_delta( int cycles );
#endif
#if XCHAL_NUM_TIMERS > 1
extern void		_xtos_timer_1_delta( int cycles );
#endif
#if XCHAL_NUM_TIMERS > 2
extern void		_xtos_timer_2_delta( int cycles );
#endif
#if XCHAL_NUM_TIMERS > 3
extern void		_xtos_timer_3_delta( int cycles );
#endif

#ifdef __cplusplus
}
#endif

#endif /* !_ASMLANGUAGE && !__ASSEMBLER__ */

#endif /* XTRUNTIME_H */

