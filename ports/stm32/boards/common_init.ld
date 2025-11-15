/* This linker script fragment is intended to be included in SECTIONS. */

/* Multi-level init/deinit sections - 4 levels */

.mp_init_early :
{
    . = ALIGN(4);
    __mp_init_early_start = .;
    KEEP(*(SORT(.mp_init_early.*)));
    __mp_init_early_end = .;
    . = ALIGN(4);
} >FLASH_TEXT

.mp_init_core :
{
    . = ALIGN(4);
    __mp_init_core_start = .;
    KEEP(*(SORT(.mp_init_core.*)));
    __mp_init_core_end = .;
    . = ALIGN(4);
} >FLASH_TEXT

.mp_init_driver :
{
    . = ALIGN(4);
    __mp_init_driver_start = .;
    KEEP(*(SORT(.mp_init_driver.*)));
    __mp_init_driver_end = .;
    . = ALIGN(4);
} >FLASH_TEXT

.mp_init_service :
{
    . = ALIGN(4);
    __mp_init_service_start = .;
    KEEP(*(SORT(.mp_init_service.*)));
    __mp_init_service_end = .;
    . = ALIGN(4);
} >FLASH_TEXT

.mp_deinit_early :
{
    . = ALIGN(4);
    __mp_deinit_early_start = .;
    KEEP(*(SORT(.mp_deinit_early.*)));
    __mp_deinit_early_end = .;
    . = ALIGN(4);
} >FLASH_TEXT

.mp_deinit_core :
{
    . = ALIGN(4);
    __mp_deinit_core_start = .;
    KEEP(*(SORT(.mp_deinit_core.*)));
    __mp_deinit_core_end = .;
    . = ALIGN(4);
} >FLASH_TEXT

.mp_deinit_driver :
{
    . = ALIGN(4);
    __mp_deinit_driver_start = .;
    KEEP(*(SORT(.mp_deinit_driver.*)));
    __mp_deinit_driver_end = .;
    . = ALIGN(4);
} >FLASH_TEXT

.mp_deinit_service :
{
    . = ALIGN(4);
    __mp_deinit_service_start = .;
    KEEP(*(SORT(.mp_deinit_service.*)));
    __mp_deinit_service_end = .;
    . = ALIGN(4);
} >FLASH_TEXT
