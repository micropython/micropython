typedef enum {
    PYEXEC_MODE_RAW_REPL,
    PYEXEC_MODE_FRIENDLY_REPL,
} pyexec_mode_kind_t;

extern pyexec_mode_kind_t pyexec_mode_kind;

int pyexec_raw_repl(void);
int pyexec_friendly_repl(void);
bool pyexec_file(const char *filename);

MP_DECLARE_CONST_FUN_OBJ(pyb_set_repl_info_obj);
