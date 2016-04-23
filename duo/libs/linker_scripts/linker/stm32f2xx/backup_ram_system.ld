    
    .backup_system :
    {
        link_global_retained_system_start = .;
        *(.retained_system*)
        link_global_retained_system_end = .;
    }>BACKUPSRAM_SYSTEM AT> APP_FLASH
    /* even though we don't initialize the system backup RAM presently, it has to be
     placed at APP_FLASH or the resulting file ends up being hundreds of megabytes in size */
