#include "IntrinsicsWrapper.h"
#ifdef __ICCARM__
#include <intrinsics.h>
#endif

void Interrupt_Enable(void)
{
#ifdef __ICCARM__
  __enable_interrupt();
#endif
}

void Interrupt_Disable(void)
{
#ifdef __ICCARM__
  __disable_interrupt();
#endif
}
