#include "Types.h"
#include "TaskScheduler.h"

typedef struct _Task
{
  bool    doIt;
  uint32  period;
  uint32  startTime;
} Task;

typedef struct _TaskSchedulerInstance
{
  Task usart;
  Task adc;
} TaskSchedulerInstance;

static TaskSchedulerInstance this;

void TaskScheduler_Init(void)
{
  this.usart.doIt = FALSE;
  this.usart.startTime = 0;

  //The correct period
  this.usart.period = 1000;

  this.adc.doIt = FALSE;
  this.adc.startTime = 0;
  this.adc.period = 100;
}

void TaskScheduler_Update(uint32 time)
{
  if ((time - this.usart.startTime) >= this.usart.period)
  {
    this.usart.doIt = TRUE;
    this.usart.startTime = time - (time % this.usart.period);
  }

  if ((time - this.adc.startTime) >= this.adc.period)
  {
    this.adc.doIt = TRUE;
    this.adc.startTime = time - (time % this.adc.period);
  }
}

bool TaskScheduler_DoUsart(void)
{
  bool doIt = FALSE;

  if (this.usart.doIt)
  {
    doIt = TRUE;
    this.usart.doIt = FALSE;
  }

  return doIt;
}

bool TaskScheduler_DoAdc(void)
{
  bool doIt = FALSE;

  if (this.adc.doIt)
  {
    doIt = TRUE;
    this.adc.doIt = FALSE;
  }

  return doIt;
}

