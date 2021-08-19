import ffi

print = ffi.func("v", "ef_print_env", "v")
set = ffi.func("i", "ef_set_env", "ss")
get = ffi.func("s", "ef_get_env", "s")
remove = ffi.func("i", "ef_del_env", "s")
