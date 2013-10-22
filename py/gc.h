void gc_init(void *start, void *end);
void gc_collect_start();
void gc_collect_root(void **ptrs, machine_uint_t len);
void gc_collect_end();
void gc_collect();
void *gc_alloc(machine_uint_t n_bytes);
machine_uint_t gc_nbytes(void *ptr_in);
void *gc_realloc(void *ptr, machine_uint_t n_bytes);

typedef struct _gc_info_t {
    machine_uint_t total;
    machine_uint_t used;
    machine_uint_t free;
    machine_uint_t num_1block;
    machine_uint_t num_2block;
    machine_uint_t max_block;
} gc_info_t;

void gc_info(gc_info_t *info);
