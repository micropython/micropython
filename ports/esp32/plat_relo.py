#! /usr/bin/python3

# Linking table for platform-specific functions.
#
# The entry points below list a number of functions in the MP firmware that are to be exposed to
# native modules. This is typically used to expose ESP-IDF functions so they can be called directly
# from C code in dynamically loaded native modules. The native module can then #include the esp-idf
# header files as usual and call the functions normally.

import sys

# TODO: this needs to be loaded from a file, not hard-coded here
entry_points = [
    "pcnt_unit_config",
    "pcnt_get_counter_value",
    "pcnt_counter_pause",
    "pcnt_counter_resume",
    "pcnt_counter_clear",
    "pcnt_intr_disable",
    "mp_micropython_mem_info",
    ]

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
