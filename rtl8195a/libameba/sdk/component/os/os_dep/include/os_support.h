 /******************************************************************************
  *
  * Name: sys-support.h - System type support for Linux
  *       $Revision: 1.1.1.1 $
  *
  *****************************************************************************/

#ifndef __OS_SUPPORT_H__
#define __OS_SUPPORT_H__


#include <FreeRTOS.h>
#include <basic_types.h>
#include "os_support.h"
//#include "diag.h"



#if 0
#define __init
#define __exit
#define __devinit
#define __devexit
#endif
#define RTL_HZ                          100

#define SemaInit(sem, value)            vSemaphoreCreateBinary(sem)
#define SemaPost(sem)                   xSemaphoreGive(sem)
#define SemaWait(sem, block_time)       xSemaphoreTake(sem, block_time)
//#define printk                          DiagPrintf

#define SpinLockInit(lock)              do { } while (0)
#define SpinLock(x)                     do { } while (0)
#define SpinUnlock(x)                   do { } while (0)
#define SpinLockBh(x)                   do { } while (0)
#define SpinUnlockBh(x)                 do { } while (0)
#ifdef PLATFORM_FREERTOS
#define RestoreFlags()                  portEXIT_CRITICAL()
#define SaveAndCli()			        portENTER_CRITICAL()
#define SpinLockIrqSave(lock, flags)	SaveAndCli()
#define SpinUnlockIrqRestore(l, f)	    RestoreFlags()
#else
#define RestoreFlags(x)                 portENABLE_INTERRUPTS()		
#define SaveAndCli(x)			        portDISABLE_INTERRUPTS()
#define SpinLockIrqSave(lock, flags)	SaveAndCli(flags)
#define SpinUnlockIrqRestore(l, f)	    RestoreFlags(f)
#endif


//#define RtlKmalloc(size, flag)          pvPortMallocAligned(size, 0)
#define RtlKmalloc(size, flag)          pvPortMalloc(size)
#define RtlKfree(pv)                    vPortFreeAligned(pv)



#ifdef CONFIG_TIMER_MODULE
extern _LONG_CALL_ u32 HalDelayUs(u32 us);
#define __Delay(t)                  HalDelayUs(t)
#else
static __inline__ u32 __Delay(u32 us)
{
    DBG_8195A("No Delay: please enable hardware Timer\n");
}
#endif


#define Mdelay(t)					__Delay(t*1000)
#define Udelay(t)					__Delay(t)


#define ASSERT(_bool_)					do { } while (0)

//#define panic_printk				DiagPrintf
//#define sprintf						DiagPrintf
//#define diag_sprintf                DiagPrintf


//1TODO: Need check again; the below just for compile ok ; chris

/*
 * ATOMIC_READ - read atomic variable
 * @v: pointer of type atomic_t
 *
 * Atomically reads the value of @v.  Note that the guaranteed
 * useful range of an atomic_t is only 24 bits.
 */
//#define AtomicRead(v)  ((*v))

static __inline__ u32
AtomicRead(
    IN  atomic_t * v
)
{
#ifdef PLATFORM_FREERTOS
    u32 Temp;

    SaveAndCli();
    Temp = v->counter;
    RestoreFlags();

    return Temp;

#else
   u32 Temp, Flags;
   
   SaveAndCli(Flags);
   Temp = v->counter;
   RestoreFlags(Flags);
   
   return Temp;
#endif
}

/*
 * ATOMIC_SET - set atomic variable
 * @v: pointer of type atomic_t
 * @i: required value
 *
 * Atomically sets the value of @v to @i.  Note that the guaranteed
 * useful range of an atomic_t is only 24 bits.
 */
//#define AtomicSet(v,i) ((v)->counter = (i))

static __inline__ VOID
AtomicSet(
    IN  u32 i, 
    IN  atomic_t * v
)
{
#ifdef PLATFORM_FREERTOS
    SaveAndCli();
    v->counter = i;
    RestoreFlags();
#else
    u32 Flags;
    
    SaveAndCli(Flags);
    v->counter = i;
    RestoreFlags(Flags);
#endif
}

/*
 * The MIPS I implementation is only atomic with respect to
 * interrupts.  R3000 based multiprocessor machines are rare anyway ...
 *
 * AtomicAdd - add integer to atomic variable
 * @i: integer value to add
 * @v: pointer of type atomic_t
 *
 * Atomically adds @i to @v.  Note that the guaranteed useful range
 * of an atomic_t is only 24 bits.
 */
static __inline__ VOID
AtomicAdd(
    IN  u32 i, 
    IN  atomic_t * v
)
{
#ifdef PLATFORM_FREERTOS
    SaveAndCli();
    v->counter += i;
    RestoreFlags();
#else
    u32 Flags;
    
    SaveAndCli(Flags);
    v->counter += i;
    RestoreFlags(Flags);
#endif
}

/*
 * AtomicSub - subtract the atomic variable
 * @i: integer value to subtract
 * @v: pointer of type atomic_t
 *
 * Atomically subtracts @i from @v.  Note that the guaranteed
 * useful range of an atomic_t is only 24 bits.
 */
static __inline__ void
AtomicSub(
    IN  u32 i, 
    IN  atomic_t * v
)
{
#ifdef PLATFORM_FREERTOS
    SaveAndCli();
    v->counter -= i;
    RestoreFlags();
#else
    u32 Flags;

    SaveAndCli(Flags);
    v->counter -= i;
    RestoreFlags(Flags);    
#endif
}

static __inline__ u32
AtomicAddReturn(
    IN  u32 i, 
    IN  atomic_t * v
)
{
#ifdef PLATFORM_FREERTOS
    u32 Temp;

    SaveAndCli();
    Temp = v->counter;
    Temp += i;
    v->counter = Temp;
    RestoreFlags();

    return Temp;

#else
       u32 Temp, Flags;
       
       SaveAndCli(Flags);
       Temp = v->counter;
       Temp += i;
       v->counter = Temp;
       RestoreFlags(Flags);
       
       return Temp;
#endif
}

static __inline__ u32
AtomicSubReturn(
    IN  u32 i, 
    IN  atomic_t * v
)
{
#ifdef PLATFORM_FREERTOS
    u32 Temp;       

    SaveAndCli();
    Temp = v->counter;
    Temp -= i;
    v->counter = Temp;
    RestoreFlags();

    return Temp;

#else
    
       u32 Temp, Flags;       
       
       SaveAndCli(Flags);
       Temp = v->counter;
       Temp -= i;
       v->counter = Temp;
       RestoreFlags(Flags);
       
       return Temp;
#endif
}

/*
 * ATOMIC_INC - increment atomic variable
 * @v: pointer of type atomic_t
 *
 * Atomically increments @v by 1.  Note that the guaranteed
 * useful range of an atomic_t is only 24 bits.
 */
#define AtomicInc(v) AtomicAdd(1,(v))

#define AtomicIncReturn(v) AtomicAddReturn(1,(v))

/*
 * ATOMIC_DEC - decrement and test
 * @v: pointer of type atomic_t
 *
 * Atomically decrements @v by 1.  Note that the guaranteed
 * useful range of an atomic_t is only 24 bits.
 */
#define AtomicDec(v) AtomicSub(1,(v))

#define AtomicDecReturn(v) AtomicSubReturn(1,(v))

/*
 * ATOMIC_DEC_AND_TEST - decrement by 1 and test
 * @v: pointer of type atomic_t
 *
 * Atomically decrements @v by 1 and
 * returns true if the result is 0, or false for all other
 * cases.  Note that the guaranteed
 * useful range of an atomic_t is only 24 bits.
 */
#define AtomicDecAndTest(v) (AtomicSubReturn(1, (v)) == 0)

/* Not needed on 64bit architectures */
static __inline__ u32 
__Div64_32(
    IN  __uint64_t *n, 
    IN  u32 base
)
{
	__uint64_t rem = *n;
	__uint64_t b = base;
	__uint64_t res, d = 1;
	u32 high = rem >> 32;

	/* Reduce the thing a bit first */
	res = 0;
	if (high >= base) {
		high /= base;
		res = (__uint64_t) high << 32;
		rem -= (__uint64_t) (high*base) << 32;
	}

	while ((__int64_t)b > 0 && b < rem) {
		b = b+b;
		d = d+d;
	}

	do {
		if (rem >= b) {
			rem -= b;
			res += d;
		}
		b >>= 1;
		d >>= 1;
	} while (d);

	*n = res;
	return rem;
}

#define DO_DIV(n,base) ({				\
	unsigned int __base = (base);			\
	unsigned int __rem;				\
	(void)(((typeof((n)) *)0) == ((__uint64_t *)0));	\
	if (((n) >> 32) == 0) {			\
		__rem = (unsigned int)(n) % __base;		\
		(n) = (unsigned int)(n) / __base;		\
	} else 						\
		__rem = __Div64_32(&(n), __base);	\
	__rem;						\
 })

#endif /* __SYS_SUPPORT_H__ */
