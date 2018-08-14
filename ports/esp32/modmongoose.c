/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Paul Sokolovsky
 * Copyright (c) 2016 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <string.h>

#include "rom/gpio.h"
#include "esp_log.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"

#include "lib/oofatfs/ff.h"
#include "extmod/vfs_fat.h"
#include "user_mongoose.h"

STATIC FATFS *lookup_path(const TCHAR **path) 
{
    mp_vfs_mount_t *fs = mp_vfs_lookup_path(*path, path);
    // printf("lookup_path %s fs %.*s\n", *path, fs->len, fs->str);
    if (fs == MP_VFS_NONE || fs == MP_VFS_ROOT) 
    {
        return NULL;
    }
    return &((fs_user_mount_t*)MP_OBJ_TO_PTR(fs->obj))->fatfs;
}

static void test_fatfs()
{
	const char * path = "/flashbdev";
	FATFS *vfs = lookup_path(&path);
	FRESULT res;
	if(NULL != (vfs))
	{
		FF_DIR dir;
		res = f_opendir(vfs, &dir, ".");
		printf("f_opendir %d\n", res);
		if (FR_OK == res)
		{
			for (;;) {
				FILINFO fno;
				FRESULT res = f_readdir(&dir, &fno);
				printf("f_readdir %d\n", res);
				char *fn = fno.fname;
				if (res != FR_OK || fn[0] == 0) 
				{
					// stop on error or end of dir
					break;
				}
				else
				{
					printf("fn %.*s\n", strlen(fn), fn);
					if (fno.fattrib & AM_DIR) 
					{
						// dir
						printf("dir\n");
					} 
					else 
					{
						// file
						printf("file\n");
					}
					printf("size %d \n", fno.fsize);
				}
			}
			
			FIL fp = { 0 };
			
			res = f_open(vfs, &fp, "./main.py", FA_WRITE | FA_CREATE_NEW);
			printf("write f_open res %d\n", res);
			if (FR_OK == res) 
			{
			    printf("write f_close res %d\n", res);
				f_close(&fp);
			}
			
			res = f_open(vfs, &fp, "./main.py", FA_READ);
			printf("read vfs %p f_open res %d\n", vfs, res);
			if (FR_OK == res) 
			{
				printf("obj %p flag %hhd err %hhd fptr %d clust %d sect %d\n", \
					&fp.obj, fp.flag, fp.err, fp.fptr, fp.clust, fp.sect);
					
				printf("f_read p %p\n", f_read);
				UINT n = 0;
				char buf[128];
				res = f_read(&fp, (void *)buf, 128, &n);
				printf("f_read res %d\n", res);
				if (res == FR_OK) 
				{
					ets_printf("f_read n %d buf %s\n", n, buf);
				}
			    printf("read f_close res %d\n", res);
				f_close(&fp);
			}
			f_closedir(&dir);
		}
	}
}

bool MicroPythonCheckFile()
{
    const char *path = "/flashbdev";
    FATFS *vfs = lookup_path(&path);
    if(NULL != (vfs))
    {
        FIL fp = { 0 };
        FRESULT res = f_open(vfs, &fp, "./system.py", FA_READ | FA_WRITE | FA_CREATE_NEW);
        if (FR_OK == res)
        {
            #define MpcfDefaultText "# This File Will Loop execute.\nimport time\ntime.sleep(0.2)\n"
            UINT written = 0;
            res = f_write(&fp, (void *)MpcfDefaultText, sizeof(MpcfDefaultText) - 1, &written);
            if (FR_OK == res && written == (sizeof(MpcfDefaultText) - 1))
            {
                f_close(&fp);
                return true;
            }
        }
    }
    return false;
}
extern FATFS *mongoose_vfs;

STATIC mp_obj_t mongoose_init(void) {

    const char * oofatfs_path = "/flashbdev";
    mongoose_vfs = lookup_path(&oofatfs_path);
    mg_init();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mongoose_init_obj, mongoose_init);

STATIC mp_obj_t mongoose_poll(void) {
    mg_poll();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mongoose_poll_obj, mongoose_poll);

STATIC const mp_rom_map_elem_t mongoose_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_mongoose) },

    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&mongoose_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_poll), MP_ROM_PTR(&mongoose_poll_obj) },
    
};

STATIC MP_DEFINE_CONST_DICT(mongoose_module_globals, mongoose_module_globals_table);

const mp_obj_module_t mongoose_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mongoose_module_globals,
};

