extern uint32_t _ram_start;
extern uint32_t _heap_start;
extern uint32_t _ram_end;
extern uint32_t _heap_end;

void gc_collect(void);

MP_DECLARE_CONST_FUN_OBJ(pyb_gc_obj);
