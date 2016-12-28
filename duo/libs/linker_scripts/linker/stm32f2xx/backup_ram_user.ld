.backup :
{
    link_global_retained_initial_values = LOADADDR( .backup ); /* This is the location in flash of the initial values of retained global variables */
    link_global_retained_start = .;
    *(.retained_user*)
    link_global_retained_end = .;
    . = ALIGN(., 4);
}>BACKUPSRAM AT> APP_FLASH
