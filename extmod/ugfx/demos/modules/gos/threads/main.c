#include "gfx.h"

#if defined(WIN32)
	#include <windows.h>

	static DWORD nres;

	// On Win32 don't use the C library fprintf or write as they crash.
	// Maybe we just need to add the multi-thread C library options to the compile.
	// Instead we use the Win32 API directly as that always works.
	#define DEBUGWRITE(str)		WriteFile(GetStdHandle(STD_ERROR_HANDLE), str, strlen(str), &nres, 0)
#else
	#warning "You must alter this demo to define a DEBUGWRITE macro for your platform."
	#warning "Be careful of using C library functions as they sometimes crash if they are not expecting stack changes (if possible use a multi-thread aware C library)"
	#warning "You might flash LED's instead if that is better for your platform."
	#error "--"
#endif

threadreturn_t heartbeat1(void* param)
{
    (void)param;

    while (TRUE) {
    	DEBUGWRITE("thread 1\n");
        gfxSleepMilliseconds(500);
    }

    return (threadreturn_t)0;
}

threadreturn_t heartbeat2(void* param)
{
    (void)param;

    while (TRUE) {
    	DEBUGWRITE("thread 2\n");
        gfxSleepMilliseconds(900);
    }

    return (threadreturn_t)0;
}

int main(void)
{
    gfxInit();

    // Give this plenty of stack. Stack size optimisation should be a production change only
    gfxThreadCreate(0, 2048, NORMAL_PRIORITY, heartbeat1, 0);
    gfxThreadCreate(0, 2048, NORMAL_PRIORITY, heartbeat2, 0);

    while (TRUE) {
    	DEBUGWRITE("thread main\n");
        gfxSleepMilliseconds(1400);
    }
}
