#define HEAP_END  (0x2001c000) // tunable
#define RAM_END   (0x20020000) // fixed for chip

extern uint32_t _ram_start;
extern uint32_t _heap_start;

void gc_collect(void);

MP_DECLARE_CONST_FUN_OBJ(pyb_gc_obj);
