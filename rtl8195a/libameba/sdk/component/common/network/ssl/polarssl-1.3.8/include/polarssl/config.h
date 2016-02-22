#define CONFIG_SSL_RSA	1

#if !defined (__GNUC__)
#include "platform_stdlib.h"
#endif
#include "section_config.h"
#include "rom_ssl_ram_map.h"

#if defined (__GNUC__)
#include "polarssl/config_rom.h"
#include "polarssl/ssl_rom_lib.h"
#else
#if CONFIG_SSL_RSA
#include "polarssl/config_rsa.h"
#else
#include "polarssl/config_all.h"
#endif
#endif
