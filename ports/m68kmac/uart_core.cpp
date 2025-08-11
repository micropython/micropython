#include <unistd.h>

#include "py/mpconfig.h"
extern "C" {
#include "uart_core.h"
}


#include "retro/Console.h"
#include "retro/ConsoleWindow.h"
/*
 * Core UART functions to implement for a port
 */

using namespace retro;
namespace retro
{
    void InitConsole();
}

#define USE_CONSOLE (1)

void mp_hal_stdin_init(void) {
#if USE_CONSOLE
    if(!Console::currentInstance)
        InitConsole();
#endif
}

// Receive single character
int mp_hal_stdin_rx_chr(void) {
#if USE_CONSOLE
    mp_console_showhide(true);
    int c = Console::currentInstance->WaitNextChar();
#else
    int c = *(char*)0xc0006a;
#endif
    return c;
}

int show_state = -1;
void mp_console_showhide(bool shown) {
    if (shown == show_state) return;
    show_state = shown;
    if(!Console::currentInstance)
        InitConsole();
    if(Console::currentInstance == (Console*)-1)
        return;
    Console::currentInstance->setVisibility(shown);
}
bool mp_hal_stdin_available(void) {
    if(!Console::currentInstance)
        InitConsole();
    if(Console::currentInstance == (Console*)-1)
        return false;
    return Console::currentInstance->Available(1);
}

mp_uint_t debug_uart_tx_strn(const char *str, mp_uint_t len) {
    mp_uint_t result = len;
    // debug hack, needs patched umac
    while(len--) {
        *(char*)0xc0006a = *str++;
    }
    return result;
}

void debug_print_fn(void *data, const char *str, size_t len) {
    debug_uart_tx_strn(str, len);
}

mp_print_t debug_print = { NULL, debug_print_fn };

// Send string of given length
mp_uint_t mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    debug_uart_tx_strn(str, len);

#if USE_CONSOLE
    if(!Console::currentInstance)
        InitConsole();
    if(Console::currentInstance == (Console*)-1)
        return 0;

    Console::currentInstance->write(str, (size_t)len);
#endif

    return len;
}
void mp_console_idle() {
    if(!Console::currentInstance)
        InitConsole();
    if(Console::currentInstance == (Console*)-1)
        return;

    Console::currentInstance->Idle();
}
