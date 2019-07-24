#include "Types.h"
#include "TimerInterruptConfigurator.h"
#include "TimerInterruptHandler.h"

static inline void SetInterruptHandler(void);
static inline void ConfigureInterruptSourceModeRegister(void);
static inline void ClearInterrupt(void);
static inline void EnableCompareInterruptForRegisterC(void);

void Timer_DisableInterrupt(void)
{
  AT91C_BASE_AIC->AIC_IDCR = TIMER0_ID_MASK;
}

void Timer_ResetSystemTime(void)
{
  Timer_SetSystemTime(0);
}

void Timer_ConfigureInterrupt(void)
{
  SetInterruptHandler();
  ConfigureInterruptSourceModeRegister();
  ClearInterrupt();
  EnableCompareInterruptForRegisterC();
}

void Timer_EnableInterrupt(void)
{
  AT91C_BASE_AIC->AIC_IECR = TIMER0_ID_MASK;
}

//
// Helpers
//

static inline void SetInterruptHandler(void)
{
  AT91C_BASE_AIC->AIC_SVR[AT91C_ID_TC0] = (uint32)Timer_InterruptHandler;
}

static inline void ConfigureInterruptSourceModeRegister(void)
{
  AT91C_BASE_AIC->AIC_SMR[AT91C_ID_TC0] = 1;
}

static inline void ClearInterrupt(void)
{
  AT91C_BASE_AIC->AIC_ICCR = TIMER0_ID_MASK;
}

static inline void EnableCompareInterruptForRegisterC(void)
{
  AT91C_BASE_TC0->TC_IER = AT91C_TC_CPCS;
}
