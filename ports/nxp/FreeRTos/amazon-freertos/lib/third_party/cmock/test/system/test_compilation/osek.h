/* ==========================================
    CMock Project - Automatic Mock Generation for C
    Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
    [Released under MIT License. Please refer to license.txt for details]
========================================== */

typedef unsigned char OSServiceIdType;
typedef void (*OSEKOS_VOIDFUNCPTR)(void);

typedef unsigned char StatusType;
typedef unsigned char OSEK_U8;
typedef unsigned short OSEK_U16;
typedef unsigned long OSEK_U32;

void OSEKOSDisableAll(void);
void OSEKOSEnableAll(void);

typedef unsigned long * OSEKOSSaveType;
typedef void OSEK_TASK;
typedef OSEK_U8 OSEKOSPrioType;

enum {
Task_DbgCAN
,
Task_ALS
,
CalibrateMagTask
,
Task_IAQ
,
SmartBeam
,
Task_QbertTestImage
,
Task_TestQbertMem
,
Task_Cyclic1000
,
ProcessMagForCompass
,
ReadMag
,
Task_Cyclic10
,
Task_Wdm
,
BackgroundTask
,
Task_Cyclic20
,
Task_Cyclic2
};

OSEK_TASK OSEKOS_T_Task_DbgCAN(void);
OSEK_TASK OSEKOS_T_Task_ALS(void);
OSEK_TASK OSEKOS_T_CalibrateMagTask(void);
OSEK_TASK OSEKOS_T_Task_IAQ(void);
OSEK_TASK OSEKOS_T_SmartBeam(void);
OSEK_TASK OSEKOS_T_Task_QbertTestImage(void);
OSEK_TASK OSEKOS_T_Task_TestQbertMem(void);
OSEK_TASK OSEKOS_T_Task_Cyclic1000(void);
OSEK_TASK OSEKOS_T_ProcessMagForCompass(void);
OSEK_TASK OSEKOS_T_ReadMag(void);
OSEK_TASK OSEKOS_T_Task_Cyclic10(void);
OSEK_TASK OSEKOS_T_Task_Wdm(void);
OSEK_TASK OSEKOS_T_BackgroundTask(void);
OSEK_TASK OSEKOS_T_Task_Cyclic20(void);
OSEK_TASK OSEKOS_T_Task_Cyclic2(void);
OSEK_TASK OSEKOS_Twrap_Task_DbgCAN(void);
OSEK_TASK OSEKOS_Twrap_Task_ALS(void);
OSEK_TASK OSEKOS_Twrap_CalibrateMagTask(void);
OSEK_TASK OSEKOS_Twrap_Task_IAQ(void);
OSEK_TASK OSEKOS_Twrap_SmartBeam(void);
OSEK_TASK OSEKOS_Twrap_Task_QbertTestImage(void);
OSEK_TASK OSEKOS_Twrap_Task_TestQbertMem(void);
OSEK_TASK OSEKOS_Twrap_Task_Cyclic1000(void);
OSEK_TASK OSEKOS_Twrap_ProcessMagForCompass(void);
OSEK_TASK OSEKOS_Twrap_ReadMag(void);
OSEK_TASK OSEKOS_Twrap_Task_Cyclic10(void);
OSEK_TASK OSEKOS_Twrap_Task_Wdm(void);
OSEK_TASK OSEKOS_Twrap_BackgroundTask(void);
OSEK_TASK OSEKOS_Twrap_Task_Cyclic20(void);
OSEK_TASK OSEKOS_Twrap_Task_Cyclic2(void);

typedef OSEK_U8 TaskType;
typedef OSEK_U8 TaskStateType;
typedef OSEK_U16 EventMaskType;
typedef OSEK_U8 ResourceType;

void OSEKOSEnableSystemTimers(void);

typedef OSEK_U8 CounterType;
typedef OSEK_U32 TickType;
typedef OSEK_U8 AlarmType;

void OSEKOS_ISR_CanTxInterrupt(void);
void OSEKOS_ISR_CanRxInterrupt(void);
void OSEKOS_ISR_CanErrInterrupt(void);
void OSEKOS_ISR_SCIRxInterrupt(void);
void OSEKOS_ISR_SCITxInterrupt(void);
void OSEKOS_ISR_UP_DMA_Interrupt_0(void);
void OSEKOS_ISR_UP_DMA_Interrupt_1(void);
void OSEKOS_ISR_UP_DMA_Interrupt_2(void);
void OSEKOS_ISR_UP_DMA_Interrupt_3(void);
void OSEKOS_ISR_CompFreqHandler(void);
void OSEKOS_ISR_AmbientReturnInt(void);
void OSEKOS_ISR_GlareReturnInt(void);
void OSEKOS_ISR_ALSTimeoutInt(void);
void OSEKOS_ISR_LINTimerInt(void);
void OSEKOS_ISR_LINDelayInt(void);
void OSEKOS_ISR_TimerMExpire(void);
void OSEKOS_ISR_LINRxTx_SCI1(void);
void OSEKOS_ISR_CanRxInterrupt_1(void);
void OSEKOS_ISR_LINError_SCI1(void);
void OSEKOS_ISR_SysCounter(void);


// defined multiple times (slightly different forms)  These should be ignored because they are externed
extern void OSEKOS_ISR_CanTxInterrupt( void );
extern void OSEKOS_ISR_CanRxInterrupt( void );


unsigned long OSEKOSrtcGetSeconds ( void );
void OSEKOSrtcIncrement ( unsigned long nsec );

enum
{
   E_OS_ACCESS = 1,
   E_OS_CALLEVEL = 2,
   E_OS_ID = 3,
   E_OS_LIMIT = 4,
   E_OS_NOFUNC = 5,
   E_OS_RESOURCE = 6,
   E_OS_STATE = 7,
   E_OS_VALUE = 8,
   E_OS_SYS_StackOverflow = 20,
   E_OS_SYS_StackUnderflow = 21,
   E_OS_SYS_INIT = 22,
   E_OS_SYS_CONFIG = 23,
   E_OS_SYS_CODE = 24,
   E_OS_SYS_TOOL = 25,
   E_OS_SYS_TimerRange = 26
};

enum
{
   SUSPENDED = 0x00,
   READY = 0x01,
   RUNNING = 0x02,
   WAITING = 0x03,
   INTSTART = 0x08,
   SETSTART = 0x10,
   NPRTASK = 0x20,
   USEFP = 0x40
};

typedef struct
{
   TickType maxallowedvalue;
   TickType ticksperbase;
} AlarmBaseType;

typedef TaskType *TaskRefType;
typedef TaskStateType *TaskStateRefType;
typedef EventMaskType *EventMaskRefType;
typedef TickType *TickRefType;
typedef AlarmBaseType *AlarmBaseRefType;
typedef OSEK_U8 AppModeType;
typedef OSEK_U8 OSEKOSTaskActCntType;

TaskType OSEKOStidact;
OSEKOSPrioType OSEKOSrunprio;

StatusType OSEKOSError ( register StatusType );
void ErrorHook ( StatusType );
void StartupHook ( void );
void ShutdownHook ( StatusType );

int getUsedTaskStack ( TaskType );
int getUnusedTaskStack ( TaskType );
int getUsedIsrStack ( void );
int getUnusedIsrStack ( void );
void OSEKOStaskStackCheckInit ( void );
signed char OSEKOStaskStackCheck ( OSEK_U8 * );
signed char OSEKOSisrStackCheck ( OSEK_U8 * );
void OSEKOStaskStackCheckFatal ( OSEK_U8 * );
void OSEKOSisrStackCheckFatal ( OSEK_U8 * );
OSEK_U8 * OSEKOSgetStackPointer ( void );
void OSEKOSTaskSwitch ( void );
StatusType OSEKOSReturn ( StatusType );
StatusType OSEKOSActivateTask ( register TaskType );
void OSEKOSTerminateTask ( TaskType, TaskType );

   extern void OSEKOSGetResource ( ResourceType );
   extern void OSEKOSReleaseResource ( ResourceType );

int OSEKOSSetEvent ( TaskType, EventMaskType );
int OSEKOSWaitEvent ( EventMaskType );
TickType OSEKOSGetAlarm(register AlarmType);
void OSEKOSSetAlarm ( AlarmType, TickType, TickType );
StatusType OSEKOSSetAbsAlarm ( AlarmType a, TickType b, TickType c );

StatusType OSEKOSCancelAlarm ( register AlarmType );
void OSEKOSAdvCntr ( void );
AppModeType GetActiveApplicationMode ( void );

void StartOS ( AppModeType );

void OSEKOSShutdownOS ( StatusType );

StatusType ActivateTask ( TaskType A );
StatusType TerminateTask ( void );
StatusType ChainTask ( TaskType A );
StatusType GetTaskState ( TaskType A, TaskStateRefType B );
StatusType GetTaskID ( TaskRefType A );
StatusType Schedule ( void );
StatusType GetResource ( ResourceType A );
StatusType ReleaseResource ( ResourceType A );
StatusType SetEvent ( TaskType A, EventMaskType B );
StatusType ClearEvent ( EventMaskType A );
StatusType WaitEvent ( EventMaskType A );
StatusType GetEvent ( TaskType A, EventMaskRefType B );
StatusType GetAlarm ( AlarmType A, TickRefType B );
StatusType GetAlarmBase ( AlarmType A, AlarmBaseRefType B );
StatusType SetRelAlarm ( AlarmType A, TickType B, TickType C );
StatusType SetAbsAlarm ( AlarmType A, TickType B, TickType C );
StatusType CancelAlarm ( AlarmType A );
StatusType AdvCntr ( CounterType A );
StatusType IAdvCntr ( CounterType A );
void SuspendOSInterrupts ( void );
void ResumeOSInterrupts ( void );
void SuspendAllInterrupts ( void );
void ResumeAllInterrupts ( void );
void DisableAllInterrupts ( void );
void EnableAllInterrupts ( void );

void OSEKOSDisable(void);
void OSEKOSEnable(void);
void OSEKOSAsmIDispatch(unsigned long *);
void OSEKOSAsmDispatch(OSEKOSPrioType p);
void OSEKOSStartupEnable(void);
void OSEKOSNop(void);
unsigned int OSEKOSV850CheckIsrSwitch(void);
void OSEKOSV850InitInterrupts(void);
void OSEKOSV850SetupInterrupts();
void OSEKOSV850SyncContextLoad(OSEKOSSaveType);
void OSEKOSV850SyncContextLoadFromIRQ(OSEKOSSaveType);
void OSEKOSV850ASyncContextLoad(OSEKOSSaveType);
void OSEKOSV850ASyncContextLoadFromIRQ(OSEKOSSaveType);

// arrays of function pointers - they look like function prototypes
void ( ( * const OSEKOStaskStartAddress [10] ) ( void ) );
StatusType (* OSEKOStaskStatuses [10][5]) ( void );

void OSEKOSV850StartContext
(
    OSEK_TASK (( * const ) ( void )),
    OSEK_U8 * const
);
void OSEKOSV850StartContextFromIRQ
(
    OSEK_TASK (( * const ) ( void )),
    OSEK_U8 * const
);

void OSEKOSSuspendOSInterrupts(void);
void OSEKOSResumeOSInterrupts(void);
void OSEKOSSuspendAllInterrupts(void);
void OSEKOSResumeAllInterrupts(void);
void OSEKOScheckSuspendResumeNesting(void);


void OSEKOSgetSR(void);
void OSEKOSEnableInterrupt_intern(int nr);
void OSEKOSDisableInterrupt_intern(int nr);
