
/* location of the module_info block */
user_module_info = 0x80C0000 ;

/**
 * End of the module info block.
 */
user_module_info_end = user_module_info + 24 ;

user_module_table = user_module_info_end ;

PROVIDE ( dynalib_location_user = user_module_table + 0 );

