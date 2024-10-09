#ifdef MBOOT_VER
const char mboot_ver[] __attribute__((section(".mboot_ver"))) __attribute__ ((__used__)) = MBOOT_VER;
#endif
