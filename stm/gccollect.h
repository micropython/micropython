// variables defining memory layout
// (these probably belong somewhere else...)
extern uint32_t _text_end;
extern uint32_t _data_start_init;
extern uint32_t _ram_start;
extern uint32_t _data_start;
extern uint32_t _data_end;
extern uint32_t _bss_start;
extern uint32_t _bss_end;
extern uint32_t _heap_start;
extern uint32_t _heap_end;
extern uint32_t _stack_end;
extern uint32_t _ram_end;

void gc_collect(void);

MP_DECLARE_CONST_FUN_OBJ(pyb_gc_obj);
