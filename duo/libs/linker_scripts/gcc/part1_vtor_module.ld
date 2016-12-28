/**
 * Symbols that define the location of the VTOR table and the module info
 */

/**
 * The load address of the part1 module image
 */
system_part1_start = 0x8020000;

system_part1_boot_table_size = 0x184;

system_part1_module_info_size = 24;

system_part1_boot_table_start = system_part1_start;

system_part1_module_info_start = system_part1_start + system_part1_boot_table_size;

system_part1_module_info_end = system_part1_module_info_start + system_part1_module_info_size;

/**
 * The location of the module export table.
 */
system_part1_module_table = system_part1_module_info_end;
