
/**
 * The load address of the part2 module image
 */
system_part2_start = 0x8040000;

/**
 * A fake VTOR table so that the bootloader can jump to this module and set up the stack,
 * plus that the module_info resides at the correct offset.
*/
system_part2_vtor_table_size = 0x184;

system_part2_module_info_size = 24;

system_part2_vtor_table_start = system_part2_start;

system_part2_module_info_start = system_part2_start + system_part2_vtor_table_size;

system_part2_module_info_end = system_part2_module_info_start + system_part2_module_info_size;

/**
 * The location of the module export table.
 */
system_part2_module_table = system_part2_module_info_end;


dynamic_reset_handler_location = system_part2_vtor_table_start + 4;

stack_size = 4096;


/**
 * Stack end at top of SRAM
 */
stack_end = 0x20020000;
stack_start = ALIGN(stack_end - stack_size, 4);
/** 8K just a guess at how much WICED might need */
min_heap_size = 16K;

/*
 * Adjust this as the static RAM needs of this module change.
 * Can be expanded or reduced as necessary. The heap dynamically expands from
 * it's static base to the heap top, which is the bottom of this module's static ram.
 */
system_static_ram_size = 46K;

/* Place system part2 static RAM immediately below stack. */
system_static_ram_start = ALIGN( stack_start - system_static_ram_size, 512);
system_static_ram_end = stack_start;

system_heap_end = system_static_ram_start;

PROVIDE ( dynalib_location_hal = system_part2_module_table + 0 );
PROVIDE ( dynalib_location_rt = system_part2_module_table + 4 );
PROVIDE ( dynalib_location_system = system_part2_module_table + 8 );
PROVIDE ( dynalib_location_hal_peripherals = system_part2_module_table + 12 );
PROVIDE ( dynalib_location_hal_i2c = system_part2_module_table + 16 );
PROVIDE ( dynalib_location_hal_gpio = system_part2_module_table + 20 );
PROVIDE ( dynalib_location_hal_spi = system_part2_module_table + 24 );
PROVIDE ( dynalib_location_hal_core = system_part2_module_table + 28 );
PROVIDE ( dynalib_location_hal_socket = system_part2_module_table + 32 );
PROVIDE ( dynalib_location_hal_wlan = system_part2_module_table + 36 );
PROVIDE ( dynalib_location_hal_usart = system_part2_module_table + 40 );
PROVIDE ( dynalib_location_hal_hci_usart = system_part2_module_table + 44 );
PROVIDE ( dynalib_location_hal_btstack = system_part2_module_table + 48 );
PROVIDE ( dynalib_location_system_net = system_part2_module_table + 52 );
PROVIDE ( dynalib_location_system_cloud = system_part2_module_table + 56 );
PROVIDE ( dynalib_location_hal_concurrent = system_part2_module_table + 60 );
PROVIDE ( dynalib_location_hal_can = system_part2_module_table + 64 );
