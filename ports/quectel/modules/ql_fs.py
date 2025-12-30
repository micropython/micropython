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


class FileNotFoundError(Exception):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return repr(self.value)


def path_exists(path):
    if not path:
        return False
    else:
        try:
            if uos.stat(path):
                return True
            else:
                return False
        except Exception as e:
            return False


def file_copy(dstFile, srcFile):
    if not path_exists(srcFile):
        return False

    dstFp = open(dstFile, 'wb+')
    srcFp = open(srcFile, 'rb')
    srcFr = srcFp.read(4096)
    while srcFr:
        dstFp.write(srcFr)
        srcFr = srcFp.read(4096)
    dstFp.close()
    srcFp.close()
    return True


def path_dirname(path):
    if not path:
        return ''

    pos = path.rfind('/')
    if pos < 0:
        return ''
    if pos == 0:
        return '/'

    dirname = ''
    for i in range(0, len(path)):
        if i == pos:
            break
        dirname = dirname + path[i]
    return dirname


def path_getsize(path):
    if path_exists(path):
        return uos.stat(path)[-4]
    else:
        raise FileNotFoundError("can not find: '%s'" % path)


def mkdirs(dir):
    dir_level_list = dir.split('/')
    dir_step_up = dir_level_list[0]
    for index in enumerate(dir_level_list):
        if dir_step_up and (not path_exists(dir_step_up)):
            uos.mkdir(dir_step_up)
        if index[0] == (len(dir_level_list) - 1):
            break
        dir_step_up = dir_step_up + '/' + dir_level_list[index[0] + 1]


def rmdirs(dir):
    ls = uos.listdir(dir)
    if not ls:
        uos.remove(dir)
    else:
        for item in ls:
            item = dir + '/' + item
            if int(uos.stat(item)[0]) & 0x4000:
                rmdirs(item)
            else:
                uos.remove(item)
        rmdirs(dir)


def touch(file, data, i=1, file_type="json"):
    i = file.find("/", i)
    if i != -1:
        if not path_exists(file[:i]):
            uos.mkdir(file[:i])
        return touch(file, data, i=i + 1, file_type=file_type)
    else:
        try:
            with open(file, "w") as f:
                if file_type == "json":
                    f.write(ujson.dumps(data))
                else:
                    f.write(data)
        except Exception as e:
            return -1
        else:
            return 0


def write_json(file, data):
    return touch(file, data)


def read_json(file):
    if path_exists(file):
        with open(file, "r") as f:
            return ujson.load(f)
    else:
        return None

