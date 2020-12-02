# this file is executed "inline" in the global context, so all imported names
# are available afterwards.
from struct_access import update_structs, set_accessors, partial_struct, lookup_struct
from kernel_ffi import p8, p16, p32, p64, symbol, Symbol
from all import structs

update_structs(structs)


def call(ptr, args):
    return Symbol("", ptr)(*args)


set_accessors(symbol("memcpy"), p8, p16, p32, p64, call)

del set_accessors, update_structs, call
