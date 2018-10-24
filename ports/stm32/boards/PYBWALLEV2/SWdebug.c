#include "SWdebug.h"

extern fs_user_mount_t fs_user_mount_flash; 
FIL debug_log_fp;

#if MICROPY_PY_DEBUG
int init_trace(char* debug_storage) {
	FRESULT ret;
	if(!strcmp(debug_storage, "flash")) {
		fs_user_mount_t *vfs_fat = &fs_user_mount_flash;
		ret = f_open(&vfs_fat->fatfs, &debug_log_fp, "/debug.log", FA_WRITE | FA_CREATE_ALWAYS | FA_OPEN_APPEND);
		if(ret != FR_OK)
			return 1;
	}
	else
		;//TODO	

	return 0;		
}

int write_trace(char* fmt, ...){

	if(fmt == NULL)
		return 1;

	va_list arg_ptr;
	char buf[TRACE_BUFFER_SIZE_C];

	memset(buf, 0, sizeof(buf));

	va_start(arg_ptr, fmt);
	vsnprintf(&buf[strlen(buf)], sizeof(buf) - strlen(buf), fmt, arg_ptr);
	va_end(arg_ptr);

	UINT n;
	f_write(&debug_log_fp, buf, strlen(buf), &n);
	if(n != strlen(buf))
		return 1;
	f_sync(&debug_log_fp);

	return 0;
}

int close_trace() {
	f_close(&debug_log_fp);

	return 0;
}
#else
int init_trace(char* debug_storage) {
	;
}

int write_trace(char* fmt, ...) {
	;
}
int close_trace() {
	;
}
#endif
