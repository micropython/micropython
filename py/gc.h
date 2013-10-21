void gc_init(void *start, void *end);
void gc_collect_start();
void gc_collect_root(void **ptrs, machine_uint_t len);
void gc_collect_end();
void gc_collect();
void *gc_alloc(machine_uint_t n_bytes);
machine_uint_t gc_nbytes(void *ptr_in);
void *gc_realloc(void *ptr, machine_uint_t n_bytes);
