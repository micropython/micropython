/*******************************************************************************
 * Trace Recorder Library for Tracealyzer v3.1.2
 * Percepio AB, www.percepio.com
 *
 * trcStreamingRecorder.c
 *
 * The generic core of the trace recorder's streaming mode.
 *
 * Terms of Use
 * This file is part of the trace recorder library (RECORDER), which is the 
 * intellectual property of Percepio AB (PERCEPIO) and provided under a
 * license as follows.
 * The RECORDER may be used free of charge for the purpose of recording data
 * intended for analysis in PERCEPIO products. It may not be used or modified
 * for other purposes without explicit permission from PERCEPIO.
 * You may distribute the RECORDER in its original source code form, assuming
 * this text (terms of use, disclaimer, copyright notice) is unchanged. You are
 * allowed to distribute the RECORDER with minor modifications intended for
 * configuration or porting of the RECORDER, e.g., to allow using it on a 
 * specific processor, processor family or with a specific communication
 * interface. Any such modifications should be documented directly below
 * this comment block.  
 *
 * Disclaimer
 * The RECORDER is being delivered to you AS IS and PERCEPIO makes no warranty
 * as to its use or performance. PERCEPIO does not and cannot warrant the 
 * performance or results you may obtain by using the RECORDER or documentation.
 * PERCEPIO make no warranties, express or implied, as to noninfringement of
 * third party rights, merchantability, or fitness for any particular purpose.
 * In no event will PERCEPIO, its technology partners, or distributors be liable
 * to you for any consequential, incidental or special damages, including any
 * lost profits or lost savings, even if a representative of PERCEPIO has been
 * advised of the possibility of such damages, or for any claim by any third
 * party. Some jurisdictions do not allow the exclusion or limitation of
 * incidental, consequential or special damages, or the exclusion of implied
 * warranties or limitations on how long an implied warranty may last, so the
 * above limitations may not apply to you.
 *
 * Tabs are used for indent in this file (1 tab = 4 spaces)
 *
 * Copyright Percepio AB, 2017.
 * www.percepio.com
 ******************************************************************************/

#include "trcRecorder.h"

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

typedef struct{
	uint16_t EventID;
	uint16_t EventCount;
	uint32_t TS;
} BaseEvent;

typedef struct{
  BaseEvent base;
  uint32_t param1;
} EventWithParam_1;

typedef struct{
  BaseEvent base;
  uint32_t param1;
  uint32_t param2;
} EventWithParam_2;

typedef struct{
  BaseEvent base;
  uint32_t param1;
  uint32_t param2;
  uint32_t param3;
} EventWithParam_3;

/* Used in event functions with variable number of parameters. */
typedef struct
{
  BaseEvent base;
  uint32_t data[15]; /* maximum payload size */
} largestEventType;

typedef struct{
  uint32_t psf;
  uint16_t version;
  uint16_t platform;
  uint32_t options;
  uint16_t symbolSize;
  uint16_t symbolCount;
  uint16_t objectDataSize;
  uint16_t objectDataCount;
} PSFHeaderInfo;


/* The size of each slot in the Symbol Table */
#define SYMBOL_TABLE_SLOT_SIZE (sizeof(uint32_t) + (((TRC_CFG_SYMBOL_MAX_LENGTH)+(sizeof(uint32_t)-1))/sizeof(uint32_t))*sizeof(uint32_t))

#define OBJECT_DATA_SLOT_SIZE (sizeof(uint32_t) + sizeof(uint32_t))

/* The total size of the Symbol Table */
#define SYMBOL_TABLE_BUFFER_SIZE ((TRC_CFG_SYMBOL_TABLE_SLOTS) * SYMBOL_TABLE_SLOT_SIZE)

/* The total size of the Object Data Table */
#define OBJECT_DATA_TABLE_BUFFER_SIZE ((TRC_CFG_OBJECT_DATA_SLOTS) * OBJECT_DATA_SLOT_SIZE)

/* The Symbol Table type - just a byte array */
typedef struct{
  union
  {
    uint32_t pSymbolTableBufferUINT32[SYMBOL_TABLE_BUFFER_SIZE / sizeof(uint32_t)];
    uint8_t pSymbolTableBufferUINT8[SYMBOL_TABLE_BUFFER_SIZE];
  } SymbolTableBuffer;
} SymbolTable;

/* The Object Data Table type - just a byte array */
typedef struct{
  union
  {
    uint32_t pObjectDataTableBufferUINT32[OBJECT_DATA_TABLE_BUFFER_SIZE / sizeof(uint32_t)];
    uint8_t pObjectDataTableBufferUINT8[OBJECT_DATA_TABLE_BUFFER_SIZE];
  } ObjectDataTableBuffer;
} ObjectDataTable;

typedef struct{
	uint8_t Status;
	uint16_t BytesRemaining;
	char* WritePointer;
} PageType;

/* Code used for "task address" when no task has started. (NULL = idle task) */
#define HANDLE_NO_TASK 2

#define PAGE_STATUS_FREE 0
#define PAGE_STATUS_WRITE 1
#define PAGE_STATUS_READ 2

#define PSF_ASSERT(_assert, _err) if (! (_assert)){ prvTraceError(_err); return; }

#define PSF_ERROR_NONE 0
#define PSF_ERROR_EVENT_CODE_TOO_LARGE 1
#define PSF_ERROR_ISR_NESTING_OVERFLOW 2
#define PSF_ERROR_DWT_NOT_SUPPORTED 3
#define PSF_ERROR_DWT_CYCCNT_NOT_SUPPORTED 4
#define PSF_ERROR_AUTO_ISR_END 5

/* Part of the PSF format - encodes the number of 32-bit params in an event */
#define PARAM_COUNT(n) ((n & 0xF) << 12)

/* The Symbol Table instance - keeps names of tasks and other named objects. */
static SymbolTable symbolTable = { { { 0 } } };

/* This points to the first unused entry in the symbol table. */
static uint32_t firstFreeSymbolTableIndex = 0;

/* The Object Data Table instance - keeps initial priorities of tasks. */
static ObjectDataTable objectDataTable = { { { 0 } } };

/* This points to the first unused entry in the object data table. */
static uint32_t firstFreeObjectDataTableIndex = 0;

/* Keeps track of ISR nesting */
static uint32_t ISR_stack[TRC_CFG_MAX_ISR_NESTING];

/* Keeps track of ISR nesting */
static int8_t ISR_stack_index = -1;

/* Any error that occurred in the recorder (also creates User Event) */
static int errorCode = 0;

/* Counts the number of trace sessions (not yet used) */
static uint32_t SessionCounter = 0u;

/* Master switch for recording (0 => Disabled, 1 => Enabled) */
uint32_t RecorderEnabled = 0u;

/* Used to determine endian of data (big/little) */
static uint32_t PSFEndianessIdentifier = 0x50534600;

/* Used to interpret the data format */
static uint16_t FormatVersion = 0x0004;

/* The number of events stored. Used as event sequence number. */
static uint32_t eventCounter = 0;

/* The user event channel for recorder warnings, defined in trcKernelPort.c */
extern char* trcWarningChannel;

/* Remembers if an earlier ISR in a sequence of adjacent ISRs has triggered a task switch.
In that case, vTraceStoreISREnd does not store a return to the previously executing task. */
int32_t isPendingContextSwitch = 0;

uint32_t uiTraceTickCount = 0;
uint32_t timestampFrequency = 0;
uint32_t DroppedEventCounter = 0;	// Total number of dropped events (failed allocations)
uint32_t TotalBytesRemaining_LowWaterMark = TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT * TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE;
uint32_t TotalBytesRemaining = TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT * TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE;

PageType PageInfo[TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT];

char* EventBuffer = NULL;

/*******************************************************************************
 * NoRoomForSymbol
 *
 * Incremented on prvTraceSaveSymbol if no room for saving the symbol name. This
 * is used for storing the names of:
 * - Tasks
 * - Named ISRs (xTraceSetISRProperties)
 * - Named kernel objects (vTraceStoreKernelObjectName)
 * - User event channels (xTraceRegisterString)
 *
 * This variable should be zero. If not, it shows the number of missing slots so
 * far. In that case, increment SYMBOL_TABLE_SLOTS with (at least) this value.
 ******************************************************************************/
volatile uint32_t NoRoomForSymbol = 0;

/*******************************************************************************
 * NoRoomForObjectData
 *
 * Incremented on prvTraceSaveObjectData if no room for saving the object data,
 * i.e., the base priorities of tasks. There must be one slot for each task.
 * If not, this variable will show the difference.
 *
 * This variable should be zero. If not, it shows the number of missing slots so
 * far. In that case, increment OBJECT_DATA_SLOTS with (at least) this value.
 ******************************************************************************/
volatile uint32_t NoRoomForObjectData = 0;

/*******************************************************************************
 * LongestSymbolName
 *
 * Updated in prvTraceSaveSymbol. Should not exceed SYMBOL_MAX_LENGTH, otherwise
 * symbol names will be truncated. In that case, set SYMBOL_MAX_LENGTH to (at
 * least) this value.
 ******************************************************************************/
volatile uint32_t LongestSymbolName = 0;

/*******************************************************************************
 * MaxBytesTruncated
 *
 * Set in prvTraceStoreStringEvent if the total data payload exceeds 60 bytes,
 * including data arguments and the string. For user events, that is 52 bytes
 * for string and data arguments. In that is exceeded, the event is  truncated
 * (usually only the string, unless more than 15 parameters) and this variable
 * holds the maximum number of truncated bytes, from any event.
 ******************************************************************************/
volatile uint32_t MaxBytesTruncated = 0;

/* Internal common function for storing string events */
static void prvTraceStoreStringEventHelper(	int nArgs,
										uint16_t eventID,
										traceString userEvtChannel,
										const char* str,
										va_list* vl);
static void prvTraceStoreSimpleStringEventHelper(	traceString userEvtChannel,
										const char* str);

/* Stores the header information on Start */
static void prvTraceStoreHeader(void);

/* Stores the symbol table on Start */
static void prvTraceStoreSymbolTable(void);

/* Stores the object table on Start */
static void prvTraceStoreObjectDataTable(void);

/* Store the Timestamp Config on Start */
static void prvTraceStoreTSConfig(void);

/* Store the current warnings */
static void prvTraceStoreWarnings(void);

/* Internal function for starting/stopping the recorder. */
static void prvSetRecorderEnabled(uint32_t isEnabled);

/* Mark the page read as complete. */
static void prvPageReadComplete(int pageIndex);

/* Retrieve a buffer page to write to. */
static int prvAllocateBufferPage(int prevPage);

/* Get the current buffer page index and remaining number of bytes. */
static int prvGetBufferPage(int32_t* bytesUsed);

/* Performs timestamping using definitions in trcHardwarePort.h */
static uint32_t prvGetTimestamp32(void);

/* Signal an error. */
void prvTraceError(int errCode);

/******************************************************************************
 * vTraceInstanceFinishedNow
 *
 * Creates an event that ends the current task instance at this very instant.
 * This makes the viewer to splits the current fragment at this point and begin
 * a new actor instance, even if no task-switch has occurred.
 *****************************************************************************/
void vTraceInstanceFinishedNow(void)
{
	prvTraceStoreEvent0(PSF_EVENT_IFE_DIRECT);
}

/******************************************************************************
 * vTraceInstanceFinishedNext
 *
 * Marks the current "task instance" as finished on the next kernel call.
 *
 * If that kernel call is blocking, the instance ends after the blocking event
 * and the corresponding return event is then the start of the next instance.
 * If the kernel call is not blocking, the viewer instead splits the current
 * fragment right before the kernel call, which makes this call the first event
 * of the next instance.
 *****************************************************************************/
void vTraceInstanceFinishedNext(void)
{
	prvTraceStoreEvent0(PSF_EVENT_IFE_NEXT);
}

/*******************************************************************************
 * xTraceRegisterString
 *
 * Stores a name for a user event channel, returns the handle.
 ******************************************************************************/
traceString xTraceRegisterString(const char* name)
{
    prvTraceSaveSymbol((const void*)name, name);

	/* Always save in symbol table, if the recording has not yet started */
	prvTraceStoreStringEvent(1, PSF_EVENT_OBJ_NAME, (const char*)name, (uint32_t)name);

	return (traceString)name;
}

/*******************************************************************************
 * vTraceStoreKernelObjectName
 *
 * Parameter object: pointer to the Event Group that shall be named
 * Parameter name: the name to set (const string literal)
 *
 * Sets a name for a kernel object for display in Tracealyzer.
 ******************************************************************************/
void vTraceStoreKernelObjectName(void* object, const char* name)
{
	/* Always save in symbol table, if the recording has not yet started */
	prvTraceSaveSymbol(object, name);

	prvTraceStoreStringEvent(1, PSF_EVENT_OBJ_NAME, name, (uint32_t)object);
}


/******************************************************************************
* vTraceSetFrequency
*
* Registers the clock rate of the time source for the event timestamping.
* This is normally not required, but if the default value (TRC_HWTC_FREQ_HZ)
* should be incorrect for your setup, you can override it using this function.
*
* Must be called prior to vTraceEnable, and the time source is assumed to
* have a fixed clock frequency after the startup.
*****************************************************************************/
void vTraceSetFrequency(uint32_t frequency)
{
	timestampFrequency = frequency;
}

/******************************************************************************
 * vTracePrint
 *
 * Generates "User Events", with unformatted text.
 *
 * User Events can be used for very efficient application logging, and are shown
 * as yellow labels in the main trace view.
 *
 * You may group User Events into User Event Channels. The yellow User Event 
 * labels shows the logged string, preceded by the channel  name within 
 * brackets. For example:
 *
 *  "[MyChannel] Hello World!"
 *
 * The User Event Channels are shown in the View Filter, which makes it easy to
 * select what User Events you wish to display. User Event Channels are created
 * using xTraceRegisterString().
 *
 * Example:
 *
 *	 traceString chn = xTraceRegisterString("MyChannel");
 *	 ...
 *	 vTracePrint(chn, "Hello World!");
 *
 ******************************************************************************/
void vTracePrint(traceString chn, const char* str)
{
	prvTraceStoreSimpleStringEventHelper(chn, str);
}

/******************************************************************************
 * vTracePrintF
 *
 * Generates "User Events", with formatted text and data, similar to a "printf".
 * It is very fast since the actual formatting is done on the host side when the
 * trace is displayed.
 *
 * User Events can be used for very efficient application logging, and are shown
 * as yellow labels in the main trace view.
 * An advantage of User Events is that data can be plotted in the "User Event
 * Signal Plot" view, visualizing any data you log as User Events, discrete
 * states or control system signals (e.g. system inputs or outputs).
 *
 * You may group User Events into User Event Channels. The yellow User Event 
 * labels show the logged string, preceded by the channel name within brackets.
 * 
 * Example:
 *
 *  "[MyChannel] Hello World!"
 *
 * The User Event Channels are shown in the View Filter, which makes it easy to
 * select what User Events you wish to display. User Event Channels are created
 * using xTraceRegisterString().
 *
 * Example:
 *
 *	 traceString adc_uechannel = xTraceRegisterString("ADC User Events");
 *	 ...
 *	 vTracePrintF(adc_uechannel,
 *				 "ADC channel %d: %d volts",
 *				 ch, adc_reading);
 *
 * All data arguments are assumed to be 32 bit wide. The following formats are
 * supported:
 * %d - signed integer. The following width and padding format is supported: "%05d" -> "-0042" and "%5d" -> "  -42"
 * %u - unsigned integer. The following width and padding format is supported: "%05u" -> "00042" and "%5u" -> "   42"
 * %X - hexadecimal (uppercase). The following width and padding format is supported: "%04X" -> "002A" and "%4X" -> "  2A"
 * %x - hexadecimal (lowercase). The following width and padding format is supported: "%04x" -> "002a" and "%4x" -> "  2a"
 * %s - string (currently, this must be an earlier stored symbol name)
 *
 * Up to 15 data arguments are allowed, with a total size of maximum 60 byte
 * including 8 byte for the base event fields and the format string. So with
 * one data argument, the maximum string length is 48 chars. If this is exceeded
 * the string is truncated (4 bytes at a time).
 *
 ******************************************************************************/
void vTracePrintF(traceString chn, const char* fmt, ...)
{
	va_list vl;
	int i = 0;

	int nArgs = 0;

	/* Count the number of arguments in the format string (e.g., %d) */
	for (i = 0; (fmt[i] != 0) && (i < 52); i++)
	{
		if (fmt[i] == '%')
		{
			if (fmt[i + 1] != '%')
			{
				nArgs++;        /* Found an argument */
			}
			
			i++;      /* Move past format specifier or non-argument '%' */
		}
	}

	va_start(vl, fmt);
	
	if (chn != NULL)
	{
		prvTraceStoreStringEventHelper(nArgs, (uint16_t)(PSF_EVENT_USER_EVENT + nArgs + 1), chn, fmt, &vl);
	}
	else
	{
		prvTraceStoreStringEventHelper(nArgs, (uint16_t)(PSF_EVENT_USER_EVENT + nArgs), chn, fmt, &vl);
	}

	va_end(vl);
}

/*******************************************************************************
 * xTraceSetISRProperties
 *
 * Stores a name and priority level for an Interrupt Service Routine, to allow
 * for better visualization. Returns a traceHandle used by vTraceStoreISRBegin. 
 *
 * Example:
 *	 #define PRIO_ISR_TIMER1 3 // the hardware priority of the interrupt
 *	 ...
 *	 traceHandle Timer1Handle = xTraceSetISRProperties("ISRTimer1", PRIO_ISR_TIMER1);
 *	 ...
 *	 void ISR_handler()
 *	 {
 *		 vTraceStoreISRBegin(Timer1Handle);
 *		 ...
 *		 vTraceStoreISREnd(0);
 *	 }
 *
 ******************************************************************************/
traceHandle xTraceSetISRProperties(const char* name, uint8_t priority)
{
	/* Save object data in object data table */
	prvTraceSaveObjectData((const void*)name, priority);
        
	/* Note: "name" is used both as a string argument, and the address as ID */
	prvTraceStoreStringEvent(2, PSF_EVENT_DEFINE_ISR, name, name, priority);
        
	/* Always save in symbol table, if the recording has not yet started */
	prvTraceSaveSymbol((const void*)name, name);
	
	return (traceHandle)name;
}

/*******************************************************************************
 * vTraceStoreISRBegin
 *
 * Registers the beginning of an Interrupt Service Routine, using a traceHandle
 * provided by xTraceSetISRProperties.
 *
 * Example:
 *	 #define PRIO_ISR_TIMER1 3 // the hardware priority of the interrupt
 *	 ...
 *	 traceHandle Timer1Handle = xTraceSetISRProperties("ISRTimer1", PRIO_ISR_TIMER1);
 *	 ...
 *	 void ISR_handler()
 *	 {
 *		 vTraceStoreISRBegin(Timer1Handle);
 *		 ...
 *		 vTraceStoreISREnd(0);
 *	 }
 *
 ******************************************************************************/
void vTraceStoreISRBegin(traceHandle handle)
{
	TRACE_ALLOC_CRITICAL_SECTION();

	TRACE_ENTER_CRITICAL_SECTION();

	/* We are at the start of a possible ISR chain. 
	No context switches should have been triggered now. */
	if (ISR_stack_index == -1)
		isPendingContextSwitch = 0; 
	
	if (ISR_stack_index < TRC_CFG_MAX_ISR_NESTING - 1)
	{
		ISR_stack_index++;
		ISR_stack[ISR_stack_index] = (uint32_t)handle;
		prvTraceStoreEvent1(PSF_EVENT_ISR_BEGIN, (uint32_t)handle);
		TRACE_EXIT_CRITICAL_SECTION();
	}
	else
	{
		TRACE_EXIT_CRITICAL_SECTION();
		prvTraceError(PSF_ERROR_ISR_NESTING_OVERFLOW);
	}
}

/*******************************************************************************
 * vTraceStoreISREnd
 *
 * Registers the end of an Interrupt Service Routine.
 *
 * The parameter pendingISR indicates if the interrupt has requested a
 * task-switch (= 1), e.g., by signaling a semaphore. Otherwise (= 0) the 
 * interrupt is assumed to return to the previous context.
 *
 * Example:
 *	 #define PRIO_OF_ISR_TIMER1 3 // the hardware priority of the interrupt
 *	 traceHandle traceHandleIsrTimer1 = 0; // The ID set by the recorder
 *	 ...
 *	 traceHandleIsrTimer1 = xTraceSetISRProperties("ISRTimer1", PRIO_OF_ISR_TIMER1);
 *	 ...
 *	 void ISR_handler()
 *	 {
 *		 vTraceStoreISRBegin(traceHandleIsrTimer1);
 *		 ...
 *		 vTraceStoreISREnd(0);
 *	 }
 *
 ******************************************************************************/
void vTraceStoreISREnd(int isTaskSwitchRequired)
{
	TRACE_ALLOC_CRITICAL_SECTION();

	TRACE_ENTER_CRITICAL_SECTION();

	/* Is there a pending task-switch? (perhaps from an earlier ISR) */
	isPendingContextSwitch |= isTaskSwitchRequired;

	if (ISR_stack_index > 0)
	{
		ISR_stack_index--;

		/* Store return to interrupted ISR (if nested ISRs)*/
		prvTraceStoreEvent1(PSF_EVENT_ISR_RESUME, (uint32_t)ISR_stack[ISR_stack_index]);
	}
	else
	{
		ISR_stack_index--;
		
		/* Store return to interrupted task, if no context switch will occur in between. */
		if ((isPendingContextSwitch == 0) || (prvTraceIsSchedulerSuspended()))
		{
			prvTraceStoreEvent1(PSF_EVENT_TS_RESUME, (uint32_t)TRACE_GET_CURRENT_TASK());
		}
	}

	TRACE_EXIT_CRITICAL_SECTION();
}


/*******************************************************************************
 * xTraceGetLastError
 *
 * Returns the last error, if any.
 *****************************************************************************/
const char* xTraceGetLastError(void)
{
	if (NoRoomForSymbol > 0)
	{
		return "TRC_CFG_SYMBOL_TABLE_SLOTS too small.";
	}

	if (LongestSymbolName > (TRC_CFG_SYMBOL_MAX_LENGTH))
	{
		return "TRC_CFG_SYMBOL_MAX_LENGTH too small.";
	}

	if (NoRoomForObjectData > 0)
	{
		return "TRC_CFG_OBJECT_DATA_SLOTS too small.";
	}

	if (MaxBytesTruncated > 0)
	{
		return "String or User Event too long.";
	}

	switch (errorCode)
	{
	case PSF_ERROR_EVENT_CODE_TOO_LARGE:
		return "An invalid event code was used.";
	case PSF_ERROR_ISR_NESTING_OVERFLOW:
		return "Too much ISR nesting.";
	case PSF_ERROR_DWT_NOT_SUPPORTED:
		return "DWT not supported by this chip.";
	case PSF_ERROR_DWT_CYCCNT_NOT_SUPPORTED:
		return "DWT_CYCCNT not supported by this chip.";
	}
	
	return "";
}

/*******************************************************************************
 * vTraceClearError
 *
 * Clears any errors.
 *****************************************************************************/
void vTraceClearError(void)
{
	NoRoomForSymbol = 0;
	LongestSymbolName = 0;
	NoRoomForObjectData = 0;
	MaxBytesTruncated = 0;
	errorCode = PSF_ERROR_NONE;
}

/*******************************************************************************
 * vTraceStop
 *
 * Stops the tracing.
 *****************************************************************************/
void vTraceStop(void)
{
	prvSetRecorderEnabled(0);
}

/*******************************************************************************
 * vTraceSetRecorderDataBuffer
 *
 * If custom allocation is used, this function must be called so the recorder
 * library knows where to save the trace data.
 ******************************************************************************/
#if (TRC_CFG_RECORDER_BUFFER_ALLOCATION == TRC_RECORDER_BUFFER_ALLOCATION_CUSTOM)

extern char* _TzTraceData;

void vTraceSetRecorderDataBuffer(void* pRecorderData)
{
	_TzTraceData = pRecorderData;
}
#endif

/******************************************************************************/
/*** INTERNAL FUNCTIONS *******************************************************/
/******************************************************************************/

/* Internal function for starting/stopping the recorder. */
static void prvSetRecorderEnabled(uint32_t isEnabled)
{
  	void* currentTask;
	
	TRACE_ALLOC_CRITICAL_SECTION();
	
	currentTask = TRACE_GET_CURRENT_TASK();

	TRACE_ENTER_CRITICAL_SECTION();

    RecorderEnabled = isEnabled;

    if (currentTask == NULL)
    {
		currentTask = (void*)HANDLE_NO_TASK;
	}

	if (RecorderEnabled)
	{
        prvTraceOnBegin();
        
     	eventCounter = 0;
        ISR_stack_index = -1;
        prvTraceStoreHeader();
		prvTraceStoreSymbolTable();
    	prvTraceStoreObjectDataTable();
        prvTraceStoreEvent3(	PSF_EVENT_TRACE_START,
							(uint32_t)TRACE_GET_OS_TICKS(),
							(uint32_t)currentTask,
							SessionCounter++);
        prvTraceStoreTSConfig();
		prvTraceStoreWarnings();
	}
    else
    {
        prvTraceOnEnd();
    }

	TRACE_EXIT_CRITICAL_SECTION();
}

/* Stores the symbol table on Start */
static void prvTraceStoreSymbolTable()
{
	uint32_t i = 0;
	uint32_t j = 0;
	TRACE_ALLOC_CRITICAL_SECTION();

	TRACE_ENTER_CRITICAL_SECTION();
	
	if (RecorderEnabled)
	{
		for (i = 0; i < (sizeof(SymbolTable) / sizeof(uint32_t)); i += (SYMBOL_TABLE_SLOT_SIZE / sizeof(uint32_t)))
		{
            TRC_STREAM_PORT_ALLOCATE_EVENT(uint32_t, data, SYMBOL_TABLE_SLOT_SIZE);
            if (data != NULL)
            {
                for (j = 0; j < (SYMBOL_TABLE_SLOT_SIZE / sizeof(uint32_t)); j++)
                {
                        data[j] = symbolTable.SymbolTableBuffer.pSymbolTableBufferUINT32[i+j];
                }
			    TRC_STREAM_PORT_COMMIT_EVENT(data, SYMBOL_TABLE_SLOT_SIZE);
			}
		}
	}
	TRACE_EXIT_CRITICAL_SECTION();
}

/* Stores the object table on Start */
static void prvTraceStoreObjectDataTable()
{
	uint32_t i = 0;
	uint32_t j = 0;
	TRACE_ALLOC_CRITICAL_SECTION();

	TRACE_ENTER_CRITICAL_SECTION();

	if (RecorderEnabled)
	{
		for (i = 0; i < (sizeof(ObjectDataTable) / sizeof(uint32_t)); i += (OBJECT_DATA_SLOT_SIZE / sizeof(uint32_t)))
        {
            TRC_STREAM_PORT_ALLOCATE_EVENT(uint32_t, data, OBJECT_DATA_SLOT_SIZE);
            if (data != NULL)
            {
                for (j = 0; j < (OBJECT_DATA_SLOT_SIZE / sizeof(uint32_t)); j++)
                {
                        data[j] = objectDataTable.ObjectDataTableBuffer.pObjectDataTableBufferUINT32[i+j];
                }
                TRC_STREAM_PORT_COMMIT_EVENT(data, OBJECT_DATA_SLOT_SIZE);
			}
        }
	}
	TRACE_EXIT_CRITICAL_SECTION();
}

/* Stores the header information on Start */
static void prvTraceStoreHeader()
{
  	TRACE_ALLOC_CRITICAL_SECTION();

	TRACE_ENTER_CRITICAL_SECTION();

	if (RecorderEnabled)
	{
	  	TRC_STREAM_PORT_ALLOCATE_EVENT(PSFHeaderInfo, header, sizeof(PSFHeaderInfo));
		if (header != NULL)
		{
			header->psf = PSFEndianessIdentifier;
			header->version = FormatVersion;
			header->platform = TRACE_KERNEL_VERSION;
            header->options = 0;
            /* Lowest bit used for TRC_IRQ_PRIORITY_ORDER */
            header->options = header->options | (TRC_IRQ_PRIORITY_ORDER << 0);
			header->symbolSize = SYMBOL_TABLE_SLOT_SIZE;
			header->symbolCount = (TRC_CFG_SYMBOL_TABLE_SLOTS);
			header->objectDataSize = 8;
			header->objectDataCount = TRC_CFG_OBJECT_DATA_SLOTS;
			TRC_STREAM_PORT_COMMIT_EVENT(header, sizeof(PSFHeaderInfo));
		}
	}
	TRACE_EXIT_CRITICAL_SECTION();
}

/* Store the current warnings */
static void prvTraceStoreWarnings()
{
	TRACE_ALLOC_CRITICAL_SECTION();

	TRACE_ENTER_CRITICAL_SECTION();

	if (RecorderEnabled)
	{
		if (NoRoomForSymbol > 0)
		{
			vTracePrintF(trcWarningChannel, "TRC_CFG_SYMBOL_TABLE_SLOTS too small. Add %d slots.", NoRoomForSymbol);
		}

		if (LongestSymbolName > 0)
		{
			if (LongestSymbolName > (TRC_CFG_SYMBOL_MAX_LENGTH))
			{
				vTracePrintF(trcWarningChannel, "TRC_CFG_SYMBOL_MAX_LENGTH too small. Add %d chars.", LongestSymbolName - (TRC_CFG_SYMBOL_MAX_LENGTH));
			}
		}

		if (NoRoomForObjectData > 0)
		{
			/* We don't know how many objects we actually need to make room for since NoRoomForObjectData can be increased multiple times for the same object! */
			vTracePrintF(trcWarningChannel, "TRC_CFG_OBJECT_DATA_SLOTS too small. Add more slots.");
		}

		if (MaxBytesTruncated > 0)
		{
			/* Some string event generated a too long string that was truncated.
			This may happen for the following functions:
			- vTracePrintF
			- vTraceStoreKernelObjectName
			- vTraceStoreUserEventChannelName
			- vTraceSetISRProperties

			A PSF event may store maximum 60 bytes payload, including data arguments
			and string characters. For User Events, also the User Event Channel ptr
			must be squeezed in, if a channel is specified. */

			vTracePrintF(trcWarningChannel, "String event too long, up to %d bytes truncated.", MaxBytesTruncated);
		}

		switch (errorCode)
		{
		case PSF_ERROR_EVENT_CODE_TOO_LARGE:
			break;
		case PSF_ERROR_ISR_NESTING_OVERFLOW:			
			break;
		case PSF_ERROR_DWT_NOT_SUPPORTED:
			vTracePrintF(trcWarningChannel, "DWT not supported, see prvTraceInitCortexM.");
			break;
		case PSF_ERROR_DWT_CYCCNT_NOT_SUPPORTED:
			vTracePrintF(trcWarningChannel, "DWT_CYCCNT not supported, see prvTraceInitCortexM.");
			break;
		}
	}
	TRACE_EXIT_CRITICAL_SECTION();
}

/* Store an event with zero parameters (event ID only) */
void prvTraceStoreEvent0(uint16_t eventID)
{
  	TRACE_ALLOC_CRITICAL_SECTION();

	PSF_ASSERT(eventID < 4096, PSF_ERROR_EVENT_CODE_TOO_LARGE);

	TRACE_ENTER_CRITICAL_SECTION();

	if (RecorderEnabled)
	{
		eventCounter++;

		{
			TRC_STREAM_PORT_ALLOCATE_EVENT(BaseEvent, event, sizeof(BaseEvent));
			if (event != NULL)
			{
				event->EventID = eventID | PARAM_COUNT(0);
				event->EventCount = (uint16_t)eventCounter;
				event->TS = prvGetTimestamp32();
				TRC_STREAM_PORT_COMMIT_EVENT(event, sizeof(BaseEvent));
			}
		}
	}
	TRACE_EXIT_CRITICAL_SECTION();
}

/* Store an event with one 32-bit parameter (pointer address or an int) */
void prvTraceStoreEvent1(uint16_t eventID, uint32_t param1)
{
  	TRACE_ALLOC_CRITICAL_SECTION();

	PSF_ASSERT(eventID < 4096, PSF_ERROR_EVENT_CODE_TOO_LARGE);

	TRACE_ENTER_CRITICAL_SECTION();

	if (RecorderEnabled)
	{
		eventCounter++;
		
		{
			TRC_STREAM_PORT_ALLOCATE_EVENT(EventWithParam_1, event, sizeof(EventWithParam_1));
			if (event != NULL)
			{
				event->base.EventID = eventID | PARAM_COUNT(1);
				event->base.EventCount = (uint16_t)eventCounter;
				event->base.TS = prvGetTimestamp32();
				event->param1 = (uint32_t)param1;
				TRC_STREAM_PORT_COMMIT_EVENT(event, sizeof(EventWithParam_1));
			}
		}
	}
	TRACE_EXIT_CRITICAL_SECTION();
}

/* Store an event with two 32-bit parameters */
void prvTraceStoreEvent2(uint16_t eventID, uint32_t param1, uint32_t param2)
{
  	TRACE_ALLOC_CRITICAL_SECTION();

	PSF_ASSERT(eventID < 4096, PSF_ERROR_EVENT_CODE_TOO_LARGE);

	TRACE_ENTER_CRITICAL_SECTION();

	if (RecorderEnabled)
	{
		eventCounter++;

		{
			TRC_STREAM_PORT_ALLOCATE_EVENT(EventWithParam_2, event, sizeof(EventWithParam_2));
			if (event != NULL)
			{
				event->base.EventID = eventID | PARAM_COUNT(2);
				event->base.EventCount = (uint16_t)eventCounter;
				event->base.TS = prvGetTimestamp32();
				event->param1 = (uint32_t)param1;
				event->param2 = param2;
				TRC_STREAM_PORT_COMMIT_EVENT(event, sizeof(EventWithParam_2));
			}
		}
	}
	TRACE_EXIT_CRITICAL_SECTION();
}

/* Store an event with three 32-bit parameters */
void prvTraceStoreEvent3(	uint16_t eventID,
						uint32_t param1,
						uint32_t param2,
						uint32_t param3)
{
  	TRACE_ALLOC_CRITICAL_SECTION();

	PSF_ASSERT(eventID < 4096, PSF_ERROR_EVENT_CODE_TOO_LARGE);

	TRACE_ENTER_CRITICAL_SECTION();

	if (RecorderEnabled)
	{
  		eventCounter++;

		{
			TRC_STREAM_PORT_ALLOCATE_EVENT(EventWithParam_3, event, sizeof(EventWithParam_3));
			if (event != NULL)
			{
				event->base.EventID = eventID | PARAM_COUNT(3);
				event->base.EventCount = (uint16_t)eventCounter;
				event->base.TS = prvGetTimestamp32();
				event->param1 = (uint32_t)param1;
				event->param2 = param2;
				event->param3 = param3;
				TRC_STREAM_PORT_COMMIT_EVENT(event, sizeof(EventWithParam_3));
			}
		}
	}
	TRACE_EXIT_CRITICAL_SECTION();
}

/* Stores an event with <nParam> 32-bit integer parameters */
void prvTraceStoreEvent(int nParam, uint16_t eventID, ...)
{
	va_list vl;
	int i;
    TRACE_ALLOC_CRITICAL_SECTION();

	PSF_ASSERT(eventID < 4096, PSF_ERROR_EVENT_CODE_TOO_LARGE);

	TRACE_ENTER_CRITICAL_SECTION();

	if (RecorderEnabled)
	{
	  	int eventSize = (int)sizeof(BaseEvent) + nParam * (int)sizeof(uint32_t);

		eventCounter++;

		{
			TRC_STREAM_PORT_ALLOCATE_DYNAMIC_EVENT(largestEventType, event, eventSize);
			if (event != NULL)
			{
				event->base.EventID = eventID | (uint16_t)PARAM_COUNT(nParam);
				event->base.EventCount = (uint16_t)eventCounter;
				event->base.TS = prvGetTimestamp32();

				va_start(vl, eventID);
				for (i = 0; i < nParam; i++)
				{
					uint32_t* tmp = (uint32_t*) &(event->data[i]);
					*tmp = va_arg(vl, uint32_t);
				}
				va_end(vl);

				TRC_STREAM_PORT_COMMIT_EVENT(event, (uint32_t)eventSize);
			}
		}
	}
	TRACE_EXIT_CRITICAL_SECTION();
}

/* Stories an event with a string and <nParam> 32-bit integer parameters */
void prvTraceStoreStringEvent(int nArgs, uint16_t eventID, const char* str, ...)
{
  	va_list vl;

	va_start(vl, str);
	prvTraceStoreStringEventHelper(nArgs, eventID, NULL, str, &vl);
	va_end(vl);
}

/* Internal common function for storing string events */
static void prvTraceStoreStringEventHelper(	int nArgs,
										uint16_t eventID,
										traceString userEvtChannel,
										const char* str, va_list* vl)
{
	int len;
  	int nWords;
	int nStrWords;
	int i;
	int offset = 0;
  	TRACE_ALLOC_CRITICAL_SECTION();

	PSF_ASSERT(eventID < 4096, PSF_ERROR_EVENT_CODE_TOO_LARGE);

	for (len = 0; (str[len] != 0) && (len < 52); len++); /* empty loop */
	
	/* The string length in multiples of 32 bit words (+1 for null character) */
	nStrWords = (len+1+3)/4;

	/* If a user event channel is specified, add in the list */
	if (userEvtChannel)
		nArgs++;

	offset = nArgs * 4;

	/* The total number of 32-bit words needed for the whole payload */
	nWords = nStrWords + nArgs;

	if (nWords > 15) /* if attempting to store more than 60 byte (= max) */
	{
		/* Truncate event if too large. The	string characters are stored
		last, so usually only the string is truncated, unless there a lot
		of parameters... */

		/* Diagnostics ... */
		uint32_t bytesTruncated = (uint32_t)(nWords - 15) * 4;

		if (bytesTruncated > MaxBytesTruncated)
		{
			MaxBytesTruncated = bytesTruncated;
		}

		nWords = 15;
		len = 15 * 4 - offset;
	}

	TRACE_ENTER_CRITICAL_SECTION();

	if (RecorderEnabled)
	{
		int eventSize = (int)sizeof(BaseEvent) + nWords * (int)sizeof(uint32_t);

		eventCounter++;

		{
			TRC_STREAM_PORT_ALLOCATE_DYNAMIC_EVENT(largestEventType, event, eventSize);
			if (event != NULL)
			{
				uint32_t* data32;
				uint8_t* data8;
				event->base.EventID = (eventID) | (uint16_t)PARAM_COUNT(nWords);
				event->base.EventCount = (uint16_t)eventCounter;
				event->base.TS = prvGetTimestamp32();

				/* 32-bit write-pointer for the data argument */
				data32 = (uint32_t*) &(event->data[0]);

				for (i = 0; i < nArgs; i++)
				{
					if ((userEvtChannel != NULL) && (i == 0))
					{
						/* First, add the User Event Channel if not NULL */
						data32[i] = (uint32_t)userEvtChannel;
					}
					else
					{
						/* Add data arguments... */
						data32[i] = va_arg(*vl, uint32_t);
					}
				}
				data8 = (uint8_t*)&(event->data[0]);
				for (i = 0; i < len; i++)
				{
					data8[offset + i] = str[i];
				}

				if (len < (15 * 4 - offset))
					data8[offset + len] = 0;	/* Only truncate if we don't fill up the buffer completely */
				TRC_STREAM_PORT_COMMIT_EVENT(event, (uint32_t)eventSize);
			}
		}
	}
	
	TRACE_EXIT_CRITICAL_SECTION();
}

/* Internal common function for storing string events without additional arguments */
static void prvTraceStoreSimpleStringEventHelper(		traceString userEvtChannel,
										const char* str)
{
	int len;
  	int nWords;
	int nStrWords;
	int i;
	int nArgs = 0;
	int offset = 0;
	uint16_t eventID = PSF_EVENT_USER_EVENT;
  	TRACE_ALLOC_CRITICAL_SECTION();

	PSF_ASSERT(eventID < 4096, PSF_ERROR_EVENT_CODE_TOO_LARGE);

	for (len = 0; (str[len] != 0) && (len < 52); len++); /* empty loop */
	
	/* The string length in multiples of 32 bit words (+1 for null character) */
	nStrWords = (len+1+3)/4;

	/* If a user event channel is specified, add in the list */
	if (userEvtChannel)
	{
		nArgs++;
		eventID++;
	}

	offset = nArgs * 4;

	/* The total number of 32-bit words needed for the whole payload */
	nWords = nStrWords + nArgs;

	if (nWords > 15) /* if attempting to store more than 60 byte (= max) */
	{
		/* Truncate event if too large. The	string characters are stored
		last, so usually only the string is truncated, unless there a lot
		of parameters... */

		/* Diagnostics ... */
		uint32_t bytesTruncated = (uint32_t)(nWords - 15) * 4;

		if (bytesTruncated > MaxBytesTruncated)
		{
			MaxBytesTruncated = bytesTruncated;
		}

		nWords = 15;
		len = 15 * 4 - offset;
	}

	TRACE_ENTER_CRITICAL_SECTION();

	if (RecorderEnabled)
	{
		int eventSize = (int)sizeof(BaseEvent) + nWords * (int)sizeof(uint32_t);

		eventCounter++;

		{
			TRC_STREAM_PORT_ALLOCATE_DYNAMIC_EVENT(largestEventType, event, eventSize);
			if (event != NULL)
			{
				uint32_t* data32;
				uint8_t* data8;
				event->base.EventID = (eventID) | (uint16_t)PARAM_COUNT(nWords);
				event->base.EventCount = (uint16_t)eventCounter;
				event->base.TS = prvGetTimestamp32();

				/* 32-bit write-pointer for the data argument */
				data32 = (uint32_t*) &(event->data[0]);

				if (userEvtChannel != NULL)
				{
					/* First, add the User Event Channel if not NULL */
					data32[0] = (uint32_t)userEvtChannel;
				}

				data8 = (uint8_t*) &(event->data[0]);
				for (i = 0; i < len; i++)
				{
					data8[offset + i] = str[i];
				}

				if (len < (15 * 4 - offset))
					data8[offset + len] = 0;	/* Only truncate if we don't fill up the buffer completely */
				TRC_STREAM_PORT_COMMIT_EVENT(event, (uint32_t)eventSize);
			}
		}
	}
	
	TRACE_EXIT_CRITICAL_SECTION();
}

/* Saves a symbol name (task name etc.) in symbol table */
void prvTraceSaveSymbol(const void *address, const char *name)
{
	uint32_t i;
	uint32_t foundSlot;
	uint32_t *ptrAddress;
	uint8_t *ptrSymbol;
	TRACE_ALLOC_CRITICAL_SECTION();

	TRACE_ENTER_CRITICAL_SECTION();
	
	foundSlot = firstFreeSymbolTableIndex;

	/* First look for previous entries using this address */
	for (i = 0; i < firstFreeSymbolTableIndex; i += SYMBOL_TABLE_SLOT_SIZE)
	{
		/* We access the symbol table via the union member pSymbolTableBufferUINT32 to avoid strict-aliasing issues */
		ptrAddress = &symbolTable.SymbolTableBuffer.pSymbolTableBufferUINT32[i / sizeof(uint32_t)];
		if (*ptrAddress == (uint32_t)address)
		{
			foundSlot = i;
			break;
		}
	}

	if (foundSlot < SYMBOL_TABLE_BUFFER_SIZE)
	{
		/* We access the symbol table via the union member pSymbolTableBufferUINT32 to avoid strict-aliasing issues */
		symbolTable.SymbolTableBuffer.pSymbolTableBufferUINT32[foundSlot / sizeof(uint32_t)] = (uint32_t)address;
		
		/* We access the symbol table via the union member pSymbolTableBufferUINT8 to avoid strict-aliasing issues */
		ptrSymbol = &symbolTable.SymbolTableBuffer.pSymbolTableBufferUINT8[foundSlot + sizeof(uint32_t)];
		for (i = 0; i < (TRC_CFG_SYMBOL_MAX_LENGTH); i++)
        {
			ptrSymbol[i] = (uint8_t)name[i];	/* We do this first to ensure we also get the 0 termination, if there is one */

			if (name[i] == 0)
				break;
		}

		/* Check the length of "name", if longer than SYMBOL_MAX_LENGTH */
		while ((name[i] != 0) && i < 128)
		{
			i++;
		}

		/* Remember the longest symbol name, for diagnostic purposes */
		if (i > LongestSymbolName)
		{
			LongestSymbolName = i;
		}

		/* Is this the last entry in the symbol table? */
		if (foundSlot == firstFreeSymbolTableIndex)
		{
			firstFreeSymbolTableIndex += SYMBOL_TABLE_SLOT_SIZE;
		}
	}
	else
	{
		NoRoomForSymbol++;
	}

	TRACE_EXIT_CRITICAL_SECTION();
}

/* Deletes a symbol name (task name etc.) from symbol table */
void prvTraceDeleteSymbol(void *address)
{
	uint32_t i, j;
	uint32_t *ptr, *lastEntryPtr;
	TRACE_ALLOC_CRITICAL_SECTION();

	TRACE_ENTER_CRITICAL_SECTION();

	for (i = 0; i < firstFreeSymbolTableIndex; i += SYMBOL_TABLE_SLOT_SIZE)
	{
		/* We access the symbol table via the union member pSymbolTableBufferUINT32 to avoid strict-aliasing issues */
		ptr = &symbolTable.SymbolTableBuffer.pSymbolTableBufferUINT32[i / sizeof(uint32_t)];
		if (*ptr == (uint32_t)address)
		{
			/* See if we have another entry in the table, and that this isn't already the last entry */
			if (firstFreeSymbolTableIndex > SYMBOL_TABLE_SLOT_SIZE && i != (firstFreeSymbolTableIndex - SYMBOL_TABLE_SLOT_SIZE))
			{
				/* Another entry is available, get pointer to the last one */
				/* We access the symbol table via the union member pSymbolTableBufferUINT32 to avoid strict-aliasing issues */
				lastEntryPtr = &symbolTable.SymbolTableBuffer.pSymbolTableBufferUINT32[(firstFreeSymbolTableIndex - SYMBOL_TABLE_SLOT_SIZE) / sizeof(uint32_t)];
				
				/* Copy last entry to this position */
				for (j = 0; j < (SYMBOL_TABLE_SLOT_SIZE) / sizeof(uint32_t); j++)
				{
					ptr[j] = lastEntryPtr[j];
				}

				/* For good measure we also zero out the original position */
				*lastEntryPtr = 0;
			}
			else
				*ptr = 0; /* No other entry found, or this is the last entry */

			/* Lower index */
			firstFreeSymbolTableIndex -= SYMBOL_TABLE_SLOT_SIZE;

			break;
		}
	}

	TRACE_EXIT_CRITICAL_SECTION();
}

/* Saves an object data entry (current task priority) in object data table */
void prvTraceSaveObjectData(const void *address, uint32_t data)
{
	uint32_t i;
	uint32_t foundSlot;
	uint32_t *ptr;
	TRACE_ALLOC_CRITICAL_SECTION();

	TRACE_ENTER_CRITICAL_SECTION();
	
	foundSlot = firstFreeObjectDataTableIndex;

	/* First look for previous entries using this address */
	for (i = 0; i < firstFreeObjectDataTableIndex; i += OBJECT_DATA_SLOT_SIZE)
	{
		/* We access the data table via the union member pObjectDataTableBufferUINT32 to avoid strict-aliasing issues */
		ptr = &objectDataTable.ObjectDataTableBuffer.pObjectDataTableBufferUINT32[i / sizeof(uint32_t)];
		if (*ptr == (uint32_t)address)
		{
			foundSlot = i;
			break;
		}
	}

	if (foundSlot < OBJECT_DATA_TABLE_BUFFER_SIZE)
	{
		/* We access the data table via the union member pObjectDataTableBufferUINT32 to avoid strict-aliasing issues */
		objectDataTable.ObjectDataTableBuffer.pObjectDataTableBufferUINT32[foundSlot / sizeof(uint32_t)] = (uint32_t)address;
		objectDataTable.ObjectDataTableBuffer.pObjectDataTableBufferUINT32[foundSlot / sizeof(uint32_t) + 1] = data;

		/* Is this the last entry in the object data table? */
		if (foundSlot == firstFreeObjectDataTableIndex)
		{
			firstFreeObjectDataTableIndex += OBJECT_DATA_SLOT_SIZE;
		}
	}
	else
	{
		NoRoomForObjectData++;
	}

	TRACE_EXIT_CRITICAL_SECTION();
}

/* Removes an object data entry (task base priority) from object data table */
void prvTraceDeleteObjectData(void *address)
{
	uint32_t i, j;
	uint32_t *ptr, *lastEntryPtr;
	TRACE_ALLOC_CRITICAL_SECTION();

	TRACE_ENTER_CRITICAL_SECTION();

	for (i = 0; i < firstFreeObjectDataTableIndex; i += OBJECT_DATA_SLOT_SIZE)
	{
		/* We access the data table via the union member pObjectDataTableBufferUINT32 to avoid strict-aliasing issues */
		ptr = &objectDataTable.ObjectDataTableBuffer.pObjectDataTableBufferUINT32[i / sizeof(uint32_t)];
		if (*ptr == (uint32_t)address)
		{
			/* See if we have another entry in the table, and that this isn't already the last entry */
			if (firstFreeObjectDataTableIndex > OBJECT_DATA_SLOT_SIZE && i != (firstFreeObjectDataTableIndex - OBJECT_DATA_SLOT_SIZE))
			{
				/* Another entry is available, get pointer to the last one */
				/* We access the data table via the union member pObjectDataTableBufferUINT32 to avoid strict-aliasing issues */
				lastEntryPtr = &objectDataTable.ObjectDataTableBuffer.pObjectDataTableBufferUINT32[(firstFreeObjectDataTableIndex - OBJECT_DATA_SLOT_SIZE) / sizeof(uint32_t)];
				
				/* Copy last entry to this position */
				for (j = 0; j < (OBJECT_DATA_SLOT_SIZE) / sizeof(uint32_t); j++)
				{
					ptr[j] = lastEntryPtr[j];
				}

				/* For good measure we also zero out the original position */
				*lastEntryPtr = 0;
			}
			else
				*ptr = 0; /* No other entry found, or this is the last entry */

			/* Lower index */
			firstFreeObjectDataTableIndex -= OBJECT_DATA_SLOT_SIZE;

			break;
		}
	}

	TRACE_EXIT_CRITICAL_SECTION();
}

/* Checks if the provided command is a valid command */
int prvIsValidCommand(TracealyzerCommandType* cmd)
{
  	uint16_t checksum = (uint16_t)(0xFFFF - (	cmd->cmdCode +
												cmd->param1 +
												cmd->param2 +
												cmd->param3 +
												cmd->param4 +
												cmd->param5));

	if (cmd->checksumMSB != (unsigned char)(checksum >> 8))
		return 0;

	if (cmd->checksumLSB != (unsigned char)(checksum & 0xFF))
		return 0;

	if (cmd->cmdCode > CMD_LAST_COMMAND)
		return 0;

	return 1;
}

/* Executed the received command (Start or Stop) */
void prvProcessCommand(TracealyzerCommandType* cmd)
{
  	switch(cmd->cmdCode)
	{
		case CMD_SET_ACTIVE:
		  	prvSetRecorderEnabled(cmd->param1);
		  	break;
		default:
		  	break;
	}
}

/* Called on critical errors in the recorder. Stops the recorder! */
void prvTraceError(int errCode)
{
	if (! errorCode)
	{
		errorCode = errCode;
		prvTraceStoreWarnings();
		vTracePrintF(trcWarningChannel, "Error detected. Stopped recorder.");

		prvSetRecorderEnabled(0);
	}
}

/* If using DWT timestamping (default on ARM Cortex-M3, M4 and M7), make sure the DWT unit is initialized. */
#ifndef TRC_CFG_ARM_CM_USE_SYSTICK
#if ((TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_ARM_Cortex_M) && (defined (__CORTEX_M) && (__CORTEX_M >= 0x03)))

void prvTraceInitCortexM()
{
	/* Make sure the DWT registers are unlocked, in case the debugger doesn't do this. */
	TRC_REG_ITM_LOCKACCESS = TRC_ITM_LOCKACCESS_UNLOCK;

	/* Make sure DWT is enabled is enabled, if supported */
	TRC_REG_DEMCR |= TRC_DEMCR_TRCENA;

	do
	{
		/* Verify that DWT is supported */
		if (TRC_REG_DEMCR == 0)
		{
			/* This function is called on Cortex-M3, M4 and M7 devices to initialize
			the DWT unit, assumed present. The DWT cycle counter is used for timestamping. 
			
			If the below error is produced, the DWT unit does not seem to be available.
			
			In that case, define the macro TRC_CFG_ARM_CM_USE_SYSTICK in your build
			to use SysTick timestamping instead, or define your own timestamping by 
			setting TRC_CFG_HARDWARE_PORT to TRC_HARDWARE_PORT_APPLICATION_DEFINED
			and make the necessary definitions, as explained in trcHardwarePort.h.*/
			
			prvTraceError(PSF_ERROR_DWT_NOT_SUPPORTED);
			break;
		}

		/* Verify that DWT_CYCCNT is supported */
		if (TRC_REG_DWT_CTRL & TRC_DWT_CTRL_NOCYCCNT)
		{
			/* This function is called on Cortex-M3, M4 and M7 devices to initialize
			the DWT unit, assumed present. The DWT cycle counter is used for timestamping. 
			
			If the below error is produced, the cycle counter does not seem to be available.
			
			In that case, define the macro TRC_CFG_ARM_CM_USE_SYSTICK in your build
			to use SysTick timestamping instead, or define your own timestamping by 
			setting TRC_CFG_HARDWARE_PORT to TRC_HARDWARE_PORT_APPLICATION_DEFINED
			and make the necessary definitions, as explained in trcHardwarePort.h.*/

			prvTraceError(PSF_ERROR_DWT_CYCCNT_NOT_SUPPORTED);
			break;
		}

		/* Reset the cycle counter */
		TRC_REG_DWT_CYCCNT = 0;

		/* Enable the cycle counter */
		TRC_REG_DWT_CTRL |= TRC_DWT_CTRL_CYCCNTENA;

	} while(0);	/* breaks above jump here */
}
#endif
#endif

/* Performs timestamping using definitions in trcHardwarePort.h */
static uint32_t prvGetTimestamp32(void)
{
#if ((TRC_HWTC_TYPE == TRC_FREE_RUNNING_32BIT_INCR) || (TRC_HWTC_TYPE == TRC_FREE_RUNNING_32BIT_DECR))
	return TRC_HWTC_COUNT;
#endif
	
#if ((TRC_HWTC_TYPE == TRC_CUSTOM_TIMER_INCR) || (TRC_HWTC_TYPE == TRC_CUSTOM_TIMER_DECR))
	return TRC_HWTC_COUNT;
#endif
	
#if ((TRC_HWTC_TYPE == TRC_OS_TIMER_INCR) || (TRC_HWTC_TYPE == TRC_OS_TIMER_DECR))
	uint32_t ticks = TRACE_GET_OS_TICKS();
	return (TRC_HWTC_COUNT & 0x00FFFFFFU) + ((ticks & 0x000000FFU) << 24);
#endif
}

/* Store the Timestamp Config event */
static void prvTraceStoreTSConfig(void)
{
	/* If not overridden using vTraceSetFrequency, use default value */
	if (timestampFrequency == 0)
	{
		timestampFrequency = TRC_HWTC_FREQ_HZ;
	}
	
	if (TRC_HWTC_TYPE == TRC_CUSTOM_TIMER_INCR || TRC_HWTC_TYPE == TRC_CUSTOM_TIMER_DECR)
	{
		prvTraceStoreEvent(5, 
							PSF_EVENT_TS_CONFIG,
							(uint32_t)timestampFrequency,	                    
							(uint32_t)TRACE_TICK_RATE_HZ,
							(uint32_t)TRC_HWTC_TYPE,
							(uint32_t)TRC_CFG_ISR_TAILCHAINING_THRESHOLD,
							(uint32_t)TRC_HWTC_PERIOD);
	}
	else
	{
	prvTraceStoreEvent(4, 
						PSF_EVENT_TS_CONFIG,
						(uint32_t)timestampFrequency,	                    
						(uint32_t)TRACE_TICK_RATE_HZ,
						(uint32_t)TRC_HWTC_TYPE,
						(uint32_t)TRC_CFG_ISR_TAILCHAINING_THRESHOLD);
	}
}

/* Retrieve a buffer page to write to. */
static int prvAllocateBufferPage(int prevPage)
{
	int index;
	int count = 0;

	index = (prevPage + 1) % TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT;

	while((PageInfo[index].Status != PAGE_STATUS_FREE) && (count ++ < TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT))
	{
		index = (index + 1) % TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT;
	}

	if (PageInfo[index].Status == PAGE_STATUS_FREE)
	{
		return index;
	}

	return -1;
}

/* Mark the page read as complete. */
static void prvPageReadComplete(int pageIndex)
{
  	TRACE_ALLOC_CRITICAL_SECTION();

	TRACE_ENTER_CRITICAL_SECTION();
	PageInfo[pageIndex].BytesRemaining = TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE;
	PageInfo[pageIndex].WritePointer = &EventBuffer[pageIndex * TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE];
	PageInfo[pageIndex].Status = PAGE_STATUS_FREE;

	TotalBytesRemaining += TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE;

	TRACE_EXIT_CRITICAL_SECTION();
}

/* Get the current buffer page index and remaining number of bytes. */
static int prvGetBufferPage(int32_t* bytesUsed)
{
	static int8_t lastPage = -1;
	int count = 0;
  	int8_t index = (int8_t) ((lastPage + 1) % TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT);

	while((PageInfo[index].Status != PAGE_STATUS_READ) && (count++ < TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT))
	{
		index = (int8_t)((index + 1) % TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT);
	}

	if (PageInfo[index].Status == PAGE_STATUS_READ)
	{
		*bytesUsed = TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE - PageInfo[index].BytesRemaining;
		lastPage = index;
		return index;
	}

	*bytesUsed = 0;

	return -1;
}

/*******************************************************************************

int32_t prvPagedEventBufferTransfer(int32_t (*writeFunc)(void* data,
                                                        uint32_t size),
                                            int32_t* nofBytes)

Transfers one block of trace data, if available for reading. Returns the number
of bytes transfered, or a negative error code. If data was transferred (return
value > 0), it can be good to call this function again until all data available
has been transfered.

This function is intended to be called by a periodic task with a suitable
delay (e.g. 10-100 ms).

Return value: as returned from writeFunc (0 == OK)

Parameters:

- writeFunc
Function pointer (example: int32_t write(void* data, uint32_t size))
The function passed as writeFunc should write "size" bytes from "data" to the
socket/file/channel, and return a status code where 0 means OK,
and any other non-zero value means an error.

- int32_t* nofBytes
Pointer to an integer assigned the number of bytes that was transfered.

*******************************************************************************/
int32_t prvPagedEventBufferTransfer(int32_t (*writeFunc)(void* data, uint32_t size, int32_t* ptrBytesWritten), int32_t* nofBytes)
{
	int8_t pageToTransfer = -1;
    int32_t transferred = 0;
    int32_t size = 0;
    
    pageToTransfer = (int8_t)prvGetBufferPage(nofBytes);
    size = *nofBytes;	// The number of bytes we want to transfer
    transferred = 0;	// The number of bytes we have transferred so far

    if (pageToTransfer > -1)
    {
        while (1) // Keep going until we have transferred all that we intended to
        {
			if (writeFunc(&EventBuffer[pageToTransfer * TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE + transferred], (uint32_t)(size - transferred), nofBytes) == 0)
			{
				// Write was successful. Update the number of transferred bytes.
				transferred += *nofBytes;
				if (size == transferred)
				{
					// All bytes have been transferred. Mark as Complete and return.
                    *nofBytes = transferred;
					prvPageReadComplete(pageToTransfer);
					return 0;
				}
			}
			else
			{
				*nofBytes = 0;
				return 1;
			}
		}
	}
	return 0;
}

/*******************************************************************************

void* prvPagedEventBufferGetWritePointer(int sizeOfEvent)

Returns a pointer to an available location in the buffer able to store the
requested size.

Return value: The pointer.

Parameters:

- sizeOfEvent
The size of the event that is to be placed in the buffer.

*******************************************************************************/
void* prvPagedEventBufferGetWritePointer(int sizeOfEvent)
{
	void* ret;
	static int currentWritePage = -1;

	if (currentWritePage == -1)
	{
	    currentWritePage = prvAllocateBufferPage(currentWritePage);
		if (currentWritePage == -1)
		{
		  	DroppedEventCounter++;
			return NULL;
		}
	}

    if (PageInfo[currentWritePage].BytesRemaining - sizeOfEvent < 0)
	{
		PageInfo[currentWritePage].Status = PAGE_STATUS_READ;

		TotalBytesRemaining -= PageInfo[currentWritePage].BytesRemaining; // Last trailing bytes

		if (TotalBytesRemaining < TotalBytesRemaining_LowWaterMark)
		  TotalBytesRemaining_LowWaterMark = TotalBytesRemaining;

		currentWritePage = prvAllocateBufferPage(currentWritePage);
		if (currentWritePage == -1)
		{
		  DroppedEventCounter++;
		  return NULL;
		}
	}
	ret = PageInfo[currentWritePage].WritePointer;
	PageInfo[currentWritePage].WritePointer += sizeOfEvent;
	PageInfo[currentWritePage].BytesRemaining = (uint16_t)(PageInfo[currentWritePage].BytesRemaining -sizeOfEvent);

	TotalBytesRemaining = (TotalBytesRemaining-(uint16_t)sizeOfEvent);

	if (TotalBytesRemaining < TotalBytesRemaining_LowWaterMark)
		TotalBytesRemaining_LowWaterMark = TotalBytesRemaining;

	return ret;
}

/*******************************************************************************

void prvPagedEventBufferInit(char* buffer)

Assigns the buffer to use and initializes the PageInfo structure.

Return value: void

Parameters:

- buffer
Pointer to the buffer location that is dynamically or statically allocated by
the caller.

*******************************************************************************/
void prvPagedEventBufferInit(char* buffer)
{
  	int i;
  	TRACE_ALLOC_CRITICAL_SECTION();
    
    EventBuffer = buffer;
    
	TRACE_ENTER_CRITICAL_SECTION();
	for (i = 0; i < TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT; i++)
	{
		PageInfo[i].BytesRemaining = TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE;
		PageInfo[i].WritePointer = &EventBuffer[i * TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE];
		PageInfo[i].Status = PAGE_STATUS_FREE;
	}
	TRACE_EXIT_CRITICAL_SECTION();
}

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/

#endif /*(TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)*/
