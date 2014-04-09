enum {
    ID_INFO_KIND_GLOBAL_IMPLICIT,
    ID_INFO_KIND_GLOBAL_EXPLICIT,
    ID_INFO_KIND_LOCAL, // in a function f, written and only referenced by f
    ID_INFO_KIND_CELL,  // in a function f, read/written by children of f
    ID_INFO_KIND_FREE,  // in a function f, belongs to the parent of f
};

typedef struct _id_info_t {
    uint8_t param;
    uint8_t kind;
    // when it's an ID_INFO_KIND_LOCAL this is the unique number of the local
    // whet it's an ID_INFO_KIND_CELL/FREE this is the unique number of the closed over variable
    uint16_t local_num;
    qstr qstr;
} id_info_t;

// scope is a "block" in Python parlance
typedef enum { SCOPE_MODULE, SCOPE_FUNCTION, SCOPE_LAMBDA, SCOPE_LIST_COMP, SCOPE_DICT_COMP, SCOPE_SET_COMP, SCOPE_GEN_EXPR, SCOPE_CLASS } scope_kind_t;
typedef struct _scope_t {
    scope_kind_t kind;
    struct _scope_t *parent;
    struct _scope_t *next;
    mp_parse_node_t pn;
    qstr source_file;
    qstr simple_name;
    uint unique_code_id;
    uint8_t scope_flags;  // see runtime0.h
    uint8_t emit_options; // see compile.h
    uint16_t num_params;
    uint16_t num_locals;
    uint16_t stack_size;     // maximum size of the locals stack
    uint16_t exc_stack_size; // maximum size of the exception stack
    uint16_t id_info_alloc;
    uint16_t id_info_len;
    id_info_t *id_info;
} scope_t;

scope_t *scope_new(scope_kind_t kind, mp_parse_node_t pn, qstr source_file, uint unique_code_id, uint emit_options);
void scope_free(scope_t *scope);
id_info_t *scope_find_or_add_id(scope_t *scope, qstr qstr, bool *added);
id_info_t *scope_find(scope_t *scope, qstr qstr);
id_info_t *scope_find_global(scope_t *scope, qstr qstr);
id_info_t *scope_find_local_in_parent(scope_t *scope, qstr qstr);
void scope_close_over_in_parents(scope_t *scope, qstr qstr);
void scope_declare_global(scope_t *scope, qstr qstr);
void scope_declare_nonlocal(scope_t *scope, qstr qstr);
void scope_print_info(scope_t *s);
