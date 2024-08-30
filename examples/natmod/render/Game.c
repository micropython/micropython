 
#include "py/dynruntime.h"
#include <math.h>
 
 static mp_obj_t cleargray(
	mp_obj_fun_bc_t* self, 
	size_t n_args, 
	size_t n_kw, 
	mp_obj_t* args) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_RW);
	unsigned short* colorddata = (unsigned short*)bufinfo.buf;

	int width = 128;
	int height = 128;
	// your rendering here
	
	// render gray
	int rint = 128.0f * (31.0f / 255.0f);
	int gint = 128.0f * (63.0f / 255.0f);
	int bint = 128.0f * (31.0f / 255.0f);
	int color565 = (rint << 11 | gint << 5 | bint);
	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < height; ++j) {
			colorddata[(height - j - 1) * height + i] = color565;
		}
	}
	return mp_const_none;
}


// This is the entry point and is called when the module is imported
mp_obj_t mpy_init(mp_obj_fun_bc_t* self, size_t n_args, size_t n_kw, mp_obj_t* args) {
    // This must be first, it sets up the globals dict and other things
    MP_DYNRUNTIME_INIT_ENTRY
	
	mp_store_global(MP_QSTR_cleargray, MP_DYNRUNTIME_MAKE_FUNCTION(cleargray));

    // This must be last, it restores the globals dict
    MP_DYNRUNTIME_INIT_EXIT
}