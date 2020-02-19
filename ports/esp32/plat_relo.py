#! /usr/bin/python3

# Linking table for platform-specific functions.
#
# The entry points below list a number of functions in the MP firmware that are to be exposed to
# native modules. This is typically used to expose ESP-IDF functions so they can be called directly
# from C code in dynamically loaded native modules. The native module can then #include the esp-idf
# header files as usual and call the functions normally. One major limitation is that only
# functions with 6 arguments or fewer can be called due to the way the linking is done.
#
# The way the linking is done is as follows:
# - in the firmware a `plat_relo_tab` table is created with the addresses of the target functions.
#   This table works just like the `mp_fun_table` and could be merged with it, but is kept separate
#   here in order not to deal with #ifdefs and #includes to include platform-specific functions.
# - in the native module a `plat_relo_stubs` section is created which contains an 8-byte code stub
#   for each target function. Each code stub loads the index into the plat_relo_tab for the function
#   and jumps to a common stub.
# - a common stub then retrieves the address of the plat_relo_tab from the mp_fun_table, indexes
#   into the plat_relo_tab to retrieve the function's address, and calls the function.
# - the result of all this is that a call from a native module to an esp-idf function goes through
#   an intermediate function call, which is the stub code. The stub code passes the 6 argument
#   registers through, but does not handle any additional stack-based arguments, hence the
#   limitation to 6 arguments.
# - given the way the xtensa calling convention works it does not appear possible to eliminate the
#   intermediate function, i.e. write stubs that only do jumps such that the target function can
#   return directly to the native module call site, the reason is that after a call instruction
#   there is no register available to perform the mp_fun_table and plat_relo_tab traversal.
# - the code below generates the plat_relo_tab for inclusion in the MP firmware (--tab option),
#   generates the assembler code for inclusion in native modules (no cmdline option), and can
#   optionally also generate equivalent, but larger, C stub code for inclusion in native modules
#   (-c option).
# - note that everything here is untyped, i.e., there is no regard for the C type of the target
#   function and it is assumed that arguments and return value can be passed through the stub
#   function by copying some registers.

import sys

entry_points = [
    "pcnt_unit_config",
    "pcnt_get_counter_value",
    "pcnt_counter_pause",
    "pcnt_counter_resume",
    "pcnt_counter_clear",
    "pcnt_intr_disable",
    "mp_micropython_mem_info",
    ]

if len(sys.argv) > 1 and sys.argv[1] == '--tab':
    # Platform relocation table compiled into the MP firmware
    print('\t.file\t"plat_relo.py"')
    print('\t.section\t.rodata.plat_relo_tab,"a",@progbits')
    print('\t.align\t4')
    print('\t.global\tplat_relo_tab')
    print('\t.type\tplat_relo_tab, @object')
    print('plat_relo_tab:')
    for ep in entry_points:
        print('\t.word\t' + ep)
    print('\t.size\tplat_relo_tab, .-plat_relo_tab')

elif len(sys.argv) > 1 and sys.argv[1] == '-c':
    # Proof-of-concept C stubs compiled into the native module
    print('#include "py/nativeglue.h"')
    print('typedef int six_arg_fun(int a1, int a2, int a3, int a4, int a5, int a6);')
    print('')
    for i, ep in enumerate(entry_points):
        print('int {}(int a1, int a2, int a3, int a4, int a5, int a6) {{'.format(ep))
        print('  return ((six_arg_fun**)mp_fun_table.plat_relo_tab) [{}](a1, a2, a3, a4, a5, a6);'.format(i))
        print('}');

else:
    # Assembly stubs compiled into the native module
    print('\t.file\t"plat_relo.py"')
    print('\t.section\t.text.plat_relo_stubs,"ax",@progbits')
    print('\t.global\tplat_relo_tab')

    print('plat_relo_stubs:')
    for i, ep in enumerate(entry_points):
        print('\t.global\t' + ep)
        #print('\t.type\t' + ep + ',@function')
        print('\t.align\t4')
        print(ep + ':')
        print('\tentry\tsp,32') # enter subroutine, 3-byte instruction
        print('\tmovi.n\ta15,{}'.format(i)) # move immediate, 2-byte instr, plat_relo_tab index
        print('\tj\t.L1') # jump, 3-byte instr

    print('\t.literal_position')
    print('\t.literal\t.LC0,mp_fun_table')
    print('\t.align\t4')
    print('.L1:')
    for i in range(6):
        dest = 11-i # destination register, start with reg 11 which is the 5th arg
        print('\tmov\ta{},a{}'.format(dest, dest-4)) # moves a7->a11, a6->a10, ..., a2->a6
    print('\tl32r\ta3,.LC0') # load mp_fun_table base address
    print('\tslli\ta2,a15,2') # multiply index by 4
    print('\tl32i\ta3,a3,320') # load plat_relo_tab address
    print('\tadd.n\ta2,a3,a2') # index into plat_relo_tab
    print('\tl32i\ta2,a2,0') # load target function address
    print('\tcallx4\ta2')
    print('\tmov\ta2,a6') # move return value from a6 to a2
    print('\tretw.n')
    print('\t.size\tplat_relo_stubs, .-plat_relo_stubs')
