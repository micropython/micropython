# Copyright (c) Quectel Wireless Solution, Co., Ltd.All Rights Reserved.
#  
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#  
#     http://www.apache.org/licenses/LICENSE-2.0
#  
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#!python3
# -*- coding:utf-8 -*-

import uos
import ujson
import ql_fs
import checksum
from app_fota_mount import AppFotaPkgMount
app_fota_pkg_mount = AppFotaPkgMount()

def get_updater_dir():
    return app_fota_pkg_mount.fota_dir+ '/usr/.updater'

def get_download_stat_file():
    return app_fota_pkg_mount.fota_dir+'/usr/.updater/download.stat'

def get_update_flag_file():
    return app_fota_pkg_mount.fota_dir+'/usr/.updater/update.flag'

updater_dir = get_updater_dir()
download_stat_file = get_download_stat_file()
download_stat_file_max_size = 16384
update_flag_file = get_update_flag_file()

def _check_update_flag():
    try:
        if ql_fs.path_exists(get_update_flag_file()):
            return 1
        else:
            return 0
    except Exception as e:
        return 0

def update():
    if _check_update_flag():
        try:
            if not ql_fs.path_exists(get_download_stat_file()):
                return -1

            fp = open(get_download_stat_file(), 'rt')
            if not fp:
                return -1

            fr = fp.read(download_stat_file_max_size)
            fp.close()
            if not fr:
                return -1
            download_stat = ujson.loads(fr)
            print(download_stat)
            download_stat_tmp = download_stat[:]
            print(download_stat_tmp)
            for item in download_stat:
                file_name = item['name']
                download_file_name = get_updater_dir() + file_name
                if ql_fs.path_exists(download_file_name):
                    ql_fs.mkdirs(ql_fs.path_dirname(file_name))
                    ql_fs.file_copy(file_name, download_file_name)
                    checksum.update(file_name)
                download_stat_tmp.remove(item)
                json_str = ujson.dumps(download_stat_tmp)
                fp = open(get_download_stat_file(), 'wt')
                if not fp:
                    return -1
                fp.write(json_str)
                fp.close()
                uos.remove(download_file_name)
            uos.remove(get_update_flag_file())
            uos.remove(get_download_stat_file())
            ql_fs.rmdirs(get_updater_dir())
            app_fota_pkg_mount.umount_disk()
            return 0
                
        except Exception as e:
            return -1
    else:
        app_fota_pkg_mount.umount_disk()
    return -1