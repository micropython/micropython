mp_state_ctx_t *micropy_create(void *heap, size_t heap_size);
void micropy_destroy(mp_state_ctx_t *mp);
int micropy_exec_str(mp_state_ctx_t *mp, const char *str);
