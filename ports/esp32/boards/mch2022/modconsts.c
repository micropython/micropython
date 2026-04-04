#include "sdkconfig.h"
#include "board_kconfig.h"
#include <time.h>
#include <string.h>
#include "py/builtin.h"
#include "py/objlist.h"
#include "py/objtuple.h"
#include "py/objstr.h"
#include "py/objint.h"
#include "py/objtype.h"
#include "py/stream.h"
#include "py/smallint.h"
#include "py/runtime.h"
#include "shared/runtime/pyexec.h"

#define INT_TO_STR_EX(number) #number
#define INT_TO_STR(number) INT_TO_STR_EX(number)

#ifdef CONFIG_OTA_WEB_USE_HTTPS
#define OTA_PROTOCOL "https"
#else
#define OTA_PROTOCOL "http"
#endif

#if defined(CONFIG_DEFAULT_DISPLAY_ORIENTATION_PORTRAIT)
	#define DEFAULT_ORIENTATION 90
#elif defined(CONFIG_DEFAULT_DISPLAY_ORIENTATION_REVERSE_LANDSCAPE)
	#define DEFAULT_ORIENTATION 180
#elif defined(CONFIG_DEFAULT_DISPLAY_ORIENTATION_REVERSE_PORTRAIT)
	#define DEFAULT_ORIENTATION 270
#else
	#define DEFAULT_ORIENTATION 0
#endif

static const MP_DEFINE_STR_OBJ( info_firmware_name_obj,        CONFIG_INFO_FIRMWARE_NAME        );
static const MP_DEFINE_STR_OBJ( info_hardware_name_obj,        CONFIG_INFO_HARDWARE_NAME        );
static const MP_DEFINE_STR_OBJ( info_hardware_folder_obj,      CONFIG_MICROPY_FROZEN_MANIFEST      );
static const MP_DEFINE_STR_OBJ( ota_web_server_obj,            CONFIG_OTA_WEB_SERVER            );
static const MP_DEFINE_STR_OBJ( ota_web_port_obj,              INT_TO_STR(CONFIG_OTA_WEB_PORT)  );
static const MP_DEFINE_STR_OBJ( ota_web_protocol_obj,          OTA_PROTOCOL                     );
static const MP_DEFINE_STR_OBJ( ota_web_path_obj,              CONFIG_OTA_WEB_PATH              );
static const MP_DEFINE_STR_OBJ( ota_web_version_path_obj,      CONFIG_OTA_WEB_VERSION_PATH      );
static const MP_DEFINE_STR_OBJ( info_woezel_web_server_obj,    CONFIG_WOEZEL_WEB_SERVER         );
static const MP_DEFINE_STR_OBJ( info_hardware_woezel_name_obj, CONFIG_INFO_HARDWARE_WOEZEL_NAME );
static const MP_DEFINE_STR_OBJ( wifi_ssid_obj,                 CONFIG_WIFI_SSID                 );
static const MP_DEFINE_STR_OBJ( wifi_pass_obj,                 CONFIG_WIFI_PASSWORD             );

static const mp_rom_map_elem_t consts_module_globals_table[] = {
	{ MP_ROM_QSTR(MP_QSTR_INFO_FIRMWARE_NAME        ), MP_ROM_PTR( &info_firmware_name_obj        ) },
	{ MP_ROM_QSTR(MP_QSTR_INFO_FIRMWARE_BUILD       ), MP_ROM_INT( CONFIG_INFO_FIRMWARE_BUILD     ) },
	{ MP_ROM_QSTR(MP_QSTR_INFO_HARDWARE_NAME        ), MP_ROM_PTR( &info_hardware_name_obj        ) },
	{ MP_ROM_QSTR(MP_QSTR_INFO_HARDWARE_FOLDER      ), MP_ROM_PTR( &info_hardware_folder_obj      ) },
	{ MP_ROM_QSTR(MP_QSTR_WOEZEL_WEB_SERVER         ), MP_ROM_PTR( &info_woezel_web_server_obj    ) },
	{ MP_ROM_QSTR(MP_QSTR_INFO_HARDWARE_WOEZEL_NAME ), MP_ROM_PTR( &info_hardware_woezel_name_obj ) },
	{ MP_ROM_QSTR(MP_QSTR_OTA_WEB_SERVER            ), MP_ROM_PTR( &ota_web_server_obj            ) },
	{ MP_ROM_QSTR(MP_QSTR_OTA_WEB_PORT              ), MP_ROM_PTR( &ota_web_port_obj              ) },
	{ MP_ROM_QSTR(MP_QSTR_OTA_WEB_PROTOCOL          ), MP_ROM_PTR( &ota_web_protocol_obj          ) },
	{ MP_ROM_QSTR(MP_QSTR_OTA_WEB_PATH              ), MP_ROM_PTR( &ota_web_path_obj              ) },
	{ MP_ROM_QSTR(MP_QSTR_OTA_WEB_VERSION_PATH      ), MP_ROM_PTR( &ota_web_version_path_obj      ) },
	{ MP_ROM_QSTR(MP_QSTR_WIFI_SSID                 ), MP_ROM_PTR( &wifi_ssid_obj                 ) },
	{ MP_ROM_QSTR(MP_QSTR_WIFI_PASSWORD             ), MP_ROM_PTR( &wifi_pass_obj                 ) },
	{ MP_ROM_QSTR(MP_QSTR_DEFAULT_ORIENTATION       ), MP_ROM_INT( DEFAULT_ORIENTATION            ) },
};

static MP_DEFINE_CONST_DICT(consts_module_globals, consts_module_globals_table);

const mp_obj_module_t consts_module = {
	.base = { &mp_type_module },
	.globals = (mp_obj_dict_t*)&consts_module_globals,
};
MP_REGISTER_MODULE(MP_QSTR_consts, consts_module);
