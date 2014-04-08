void gc_init(void *start, void *end);

// These lock/unlock functions can be nested.
// They can be used to prevent the GC from allocating/freeing.
void gc_lock(void);
void gc_unlock(void);

// A given port must implement gc_collect by using the other collect functions.
void gc_collect(void);
void gc_collect_start(void);
void gc_collect_root(void **ptrs, machine_uint_t len);
void gc_collect_end(void);

void *gc_alloc(machine_uint_t n_bytes, bool has_finaliser);
void gc_free(void *ptr);
machine_uint_t gc_nbytes(void *ptr);
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
void gc_dump_info(void);
void gc_dump_alloc_table(void);
