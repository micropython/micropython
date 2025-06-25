#include <py/obj.h>
#include "shared/timeutils/timeutils.h"
#include "Multiverse.h"


// The value to *subtract* from a mac epoch to get a microypython epoch, or to
// *add* to a microypython epoch to get a mac epoch. Note that the 32-bit
// unsigned mac epoch ends in 2040.
#define mac_unix_epoch_offset (2082844800lu)
#define mac_2000_epoch_offset (mac_unix_epoch_offset + TIMEUTILS_SECONDS_1970_TO_2000)
#define mac_micropython_epoch_offset (MICROPY_EPOCH_IS_1970 ? mac_unix_epoch_offset : mac_2000_epoch_offset)

#define from_mac_timestamp(x) ((x) - mac_micropython_epoch_offset)
#define to_mac_timestamp(x) ((x) + mac_micropython_epoch_offset)

void check_mac_err(OSErr e);
MP_NORETURN void raise_mac_err(OSErr e);
int convert_mac_err(OSErr e);
mp_obj_t new_str_from_pstr(Byte *pStr);
mp_obj_t new_bytes_from_pstr(Byte *pStr);
Byte *pstr_from_str(Byte *pStr, size_t pStr_size, mp_obj_t obj);
Byte *pstr_from_data(Byte *pStr, size_t pStr_size, const char *str_data, size_t str_len);
Byte *pstr_from_cstr(Byte *pStr, size_t pStr_size, const char *str_data);
Byte *pstr_cat_str(Byte *pStr, size_t pStr_size, mp_obj_t obj);
Byte *pstr_cat_cstr(Byte *pStr, size_t pStr_size, const char *str_data);
Byte *pstr_cat_data(Byte *pStr, size_t pStr_size, const char *str_data, size_t str_len);

#define PSTR_FROM_STR(pStr, obj) pstr_from_str(pStr, MP_ARRAY_SIZE(pStr), obj)
#define PSTR_FROM_CSTR(pStr, str) pstr_from_cstr(pStr, MP_ARRAY_SIZE(pStr), str)
#define PSTR_FROM_DATA(pStr, ptr, len) pstr_from_data(pStr, MP_ARRAY_SIZE(pStr), ptr, len)

#define PSTR_LEN(p) (*(p))
#define PSTR_DATA(p) ((char *)((p) + 1))
