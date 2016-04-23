  .module_info :
    {
        . = ALIGN(4);
        link_module_info_start = .;
        KEEP(*.o(.modinfo.module_info))
        link_module_info_end = .;
    }>APP_FLASH  AT> APP_FLASH
