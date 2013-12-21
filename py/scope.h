enum {
    ID_INFO_KIND_GLOBAL_IMPLICIT,
    ID_INFO_KIND_GLOBAL_EXPLICIT,
    ID_INFO_KIND_LOCAL, // in a function f, written and only referenced by f
    ID_INFO_KIND_CELL,  // in a function f, read/written by children of f
    ID_INFO_KIND_FREE,  // in a function f, belongs to the parent of f
};

typedef struct _id_info_t {
    // TODO compress this info to make structure smaller in memory
    bool param;
    int kind;
    qstr qstr;

    // when it's an ID_INFO_KIND_LOCAL this is the unique number of the local
    // whet it's an ID_INFO_KIND_CELL/FREE this is the unique number of the closed over variable
    int local_num;
} id_info_t;

// taken from python source, Include/code.h
#define SCOPE_FLAG_OPTIMISED    0x0001
#define SCOPE_FLAG_NEWLOCALS    0x0002
#define SCOPE_FLAG_VARARGS      0x0004
#define SCOPE_FLAG_VARKEYWORDS  0x0008
#define SCOPE_FLAG_NESTED       0x0010
#define SCOPE_FLAG_GENERATOR    0x0020
/* The SCOPE_FLAG_NOFREE flag is set if there are no free or cell variables.
   This information is redundant, but it allows a single flag test
   to determine whether there is any extra work to be done when the
   call frame is setup.
*/
#define SCOPE_FLAG_NOFREE       0x0040

// scope is a "block" in Python parlance
typedef enum { SCOPE_MODULE, SCOPE_FUNCTION, SCOPE_LAMBDA, SCOPE_LIST_COMP, SCOPE_DICT_COMP, SCOPE_SET_COMP, SCOPE_GEN_EXPR, SCOPE_CLASS } scope_kind_t;
typedef struct _scope_t {
    scope_kind_t kind;
    struct _scope_t *parent;
    struct _scope_t *next;
    mp_parse_node_t pn;
    qstr simple_name;
    int id_info_alloc;
    int id_info_len;
    id_info_t *id_info;
    int flags;
    int num_params;
    /* not needed
    int num_default_params;
    int num_dict_params;
    */
    int num_locals;
    int num_cells;
    int stack_size;
    uint unique_code_id;
    uint emit_options;
} scope_t;

scope_t *scope_new(scope_kind_t kind, mp_parse_node_t pn, uint unique_code_id, uint emit_options);
id_info_t *scope_find_or_add_id(scope_t *scope, qstr qstr, bool *added);
id_info_t *scope_find(scope_t *scope, qstr qstr);
id_info_t *scope_find_global(scope_t *scope, qstr qstr);
id_info_t *scope_find_local_in_parent(scope_t *scope, qstr qstr);
void scope_close_over_in_parents(scope_t *scope, qstr qstr);
void scope_declare_global(scope_t *scope, qstr qstr);
void scope_declare_nonlocal(scope_t *scope, qstr qstr);
void scope_print_info(scope_t *s);
