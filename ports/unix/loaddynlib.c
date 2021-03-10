#include "py/mpconfig.h"
#include "py/obj.h"
#include <dlfcn.h>
#include <string.h>

#if 0 // print debugging info
#define DEBUG_PRINT (1)
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_PRINT (0)
#define DEBUG_printf(...) (void)0
#endif

typedef mp_obj_module_t * (*fun)();

#if defined(MICROPY_UNIX_COVERAGE)
mp_obj_module_t *mp_load_dynlib(const char *mod_name, vstr_t *path);
#endif

mp_obj_module_t *mp_load_dynlib(const char *mod_name, vstr_t *path) {
    void *lib = dlopen(vstr_null_terminated_str(path), RTLD_LAZY | RTLD_LOCAL);
    if (!lib) {
        DEBUG_printf("dlopen %s failed: %s\n", vstr_null_terminated_str(path), dlerror());
        return NULL;
    }
    const size_t nameLen = strlen(mod_name);
    char *initFunc = (char *)alloca(nameLen + 6);
    memcpy(initFunc, "init_", 5);
    strcpy(initFunc + 5, mod_name);
    fun f = dlsym(lib, initFunc);
    if (!f) {
        DEBUG_printf("dlsym %s failed: %s\n", vstr_null_terminated_str(path), dlerror());
        dlclose(lib);
        return NULL;
    }
    return f();
}
