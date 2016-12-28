    .module_info_suffix :
    {
        . = ALIGN(., 2);
        link_module_info_static_start = .;
        SHORT(0xffff)
        SHORT(0xffff)
        KEEP(*.o(.modinfo.product_id))
        KEEP(*.o(.modinfo.product_version))

        link_module_info_suffix_start = .;
        KEEP(*.o(.modinfo.module_info_suffix))
        link_module_info_suffix_end = .;
    }>APP_FLASH

    .module_end :
    {
        link_module_end = .;
    }>APP_FLASH

    .module_info_crc :
    {
        link_module_info_crc_start = .;
        KEEP(*.o(.modinfo.module_info_crc))
        link_module_info_crc_end = .;
    }>APP_FLASH
