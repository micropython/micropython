import ffi

show = ffi.func("v", "native_module_show", "s")
add = ffi.func("i", "native_module_add", "ii")
