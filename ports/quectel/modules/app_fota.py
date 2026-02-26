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

import app_fota_download
import app_fota_updater
from app_fota_mount import AppFotaPkgMount
class new(object):
    def __init__(self):
        self.app_fota_pkg_mount = AppFotaPkgMount()
        self.app_fota_pkg_mount.mount_disk()        
    def download(self, url, file_name, headers=None):
        return app_fota_download.download(url, file_name, headers)
    def bulk_download(self, info=[], headers=None):
        return app_fota_download.bulk_download(info, headers)
    def set_update_flag(self):
        app_fota_download.set_update_flag()
    def update(self):
        return app_fota_updater.update()
    