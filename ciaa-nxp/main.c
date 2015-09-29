#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "ciaanxp_mphal.h"

#include "modpyb.h"
#include "ciaanxp_mphal.h"

//prueba uart, sacar
#include "chip.h"
#include "board.h"

#define __SECTION_EXT(type, bank, name) __attribute__ ((section("." #type ".$" #bank "." #name)))
#define __SECTION(type, bank) __attribute__ ((section("." #type ".$" #bank)))

#define __DATA_EXT(bank, name) __SECTION_EXT(data, bank, name)
#define __DATA(bank) __SECTION(data, bank)

// maximum heap for device with 8k RAM
static char *stack_top;
static __DATA(RAM2) char heap[16*1024];

extern const char programScript[];

void do_str(const char *src, mp_parse_input_kind_t input_kind) {
    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
    if (lex == NULL) {
        return;
    }

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        qstr source_name = lex->source_name;
        mp_parse_node_t pn = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(pn, source_name, MP_EMIT_OPT_NONE, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}

int main(int argc, char **argv) {
    int stack_dummy;
    stack_top = (char*)&stack_dummy;

    int i;
    for(i=0;i<sizeof(heap);i++)
   	heap[i]=0;
    gc_init(heap, heap + sizeof(heap));

    mp_init();
    mp_hal_init();

    // prueba uart 485
	/*
    while(1)
	{
		if(mp_hal_rs232_charAvailable()==1)
		{
			int32_t data = mp_hal_rs232_getChar();
			char aux[48];
    			sprintf(aux,"llego: %d ",data);
    			Board_UARTPutSTR(aux);
		}
	}*/
    //while(1)
	//char rxBuffer[512];
	//Board_UART_setRxBuffer(LPC_USART3,rxBuffer,sizeof(rxBuffer),10000,NULL);
	//Board_UART_setConfig(LPC_USART3,115200, 1,0);
        //Board_UART_Write(LPC_USART3,"HOLA", 4);
	/*
	while(1)
	{
		if(Board_UART_isNewPacket(LPC_USART3))
		{
			char aux[1024];
			sprintf(aux,"llegaron %d bytes, contenido:%s",Board_UART_getRxSize(LPC_USART3),rxBuffer);
			Board_UARTPutSTR(aux);
			Board_UART_resetRx(LPC_USART3);
		}
	}*/
    //____________


    // prueba dac ***************************
	/*
    //	Board_DAC_writeValue(512);
    uint16_t samples[1000];
    uint16_t samples2[1000];
    for(int i=0; i<1000; i++)
         //samples[i]= ((uint16_t)i)*10;
	 if(i&0x01==0x01)
             samples[i]=1023;
	else
             samples[i]=0;

    for(int i=0; i<1000; i++)
         samples2[i]= ((uint16_t)i)*10;

    Board_DAC_setSampleRate(5000);
    Board_DAC_writeDMA(samples, 1000,0);
    bool f=0;
    while (1)
    {
        //Board_DAC_setSampleRate(5000);
        //Board_DAC_writeDMA(samples, 100);
       mp_hal_milli_delay(2000);
	if(f==0)
	{
		f=1;
		Board_DAC_writeDMA(samples2, 1000,1);
		Board_UARTPutSTR("sample 2 ");
	}
	else
	{
		f=0;
		Board_DAC_writeDMA(samples, 1000,1);
		Board_UARTPutSTR("sample 1 ");
	}
    }
	*/
     //_____________________________________


    do_str(programScript, MP_PARSE_FILE_INPUT);
    pyexec_friendly_repl();
    mp_deinit();
    return 0;
}


void gc_collect(void) {
    void *dummy;
    gc_collect_start();
    gc_collect_root(&dummy, ((mp_uint_t)stack_top - (mp_uint_t)&dummy) / sizeof(mp_uint_t));
    gc_collect_end();
    //gc_dump_info();
}


mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    return NULL;
}

mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(uint n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

void nlr_jump_fail(void *val) {
}

void NORETURN __fatal_error(const char *msg) {
    while (1);
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif

#if 0
int _lseek() {return 0;}
int _read() {return 0;}
int _write() {return 0;}
int _close() {return 0;}
void _exit(int x) {for(;;){}}
int _sbrk() {return 0;}
int _kill() {return 0;}
int _getpid() {return 0;}
int _fstat() {return 0;}
int _isatty() {return 0;}
#endif

#if 0
void *malloc(size_t n) {return NULL;}
void *calloc(size_t nmemb, size_t size) {return NULL;}
void *realloc(void *ptr, size_t size) {return NULL;}
void free(void *p) {}
int printf(const char *m, ...) {return 0;}
void *memcpy(void *dest, const void *src, size_t n) {return NULL;}
int memcmp(const void *s1, const void *s2, size_t n) {return 0;}
void *memmove(void *dest, const void *src, size_t n) {return NULL;}
void *memset(void *s, int c, size_t n) {return NULL;}
int strcmp(const char *s1, const char* s2) {return 0;}
int strncmp(const char *s1, const char* s2, size_t n) {return 0;}
size_t strlen(const char *s) {return 0;}
char *strcat(char *dest, const char *src) {return NULL;}
char *strchr(const char *dest, int c) {return NULL;}
#include <stdarg.h>
int vprintf(const char *format, va_list ap) {return 0;}
int vsnprintf(char *str,  size_t  size,  const  char  *format, va_list ap) {return 0;}

#undef putchar
int putchar(int c) {return 0;}
int puts(const char *s) {return 0;}

void _start(void) {main(0, NULL);}
#endif
