import kernel_ffi

kernel_ffi.auto_globals(True)

print(type(printk))  # automatic global binding works

x = printk
del printk

kernel_ffi.auto_globals(False)

try:
    printk  # now automatic doesn't work
except NameError:
    print("NameError")

# different objects but equality test should work
print(kernel_ffi.symbol("printk") == x)

kernel_ffi.auto_globals(True)

print(printk == x)

try:
    # should raise a NameError
    there_is_no_kernel_symbol_named_this_way
except NameError:
    print("NameError")

try:
    # should also raise a NameError
    kernel_ffi.symbol("there_is_no_kernel_symbol_named_this_way")
except NameError:
    print("NameError")
