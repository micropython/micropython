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

# !python3
# -*- coding:utf-8 -*-

import ujson
import uos
import file_crc32
import ql_fs

checksum_file = '/usr/checksum.json'
checksum_file_max_size = 16384
backup_checksum_file = '/bak/checksum.json'


class _checkError(Exception):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return repr(self.value)


def check():
    if not ql_fs.path_exists(checksum_file):
        if not ql_fs.path_exists(backup_checksum_file):
            raise _checkError('%s not exist' % checksum_file)
        else:
            ql_fs.mkdirs(ql_fs.path_dirname(checksum_file))
            ql_fs.file_copy(checksum_file, backup_checksum_file)

    fp = open(checksum_file, 'rt+')
    fp.seek(0)
    fr = fp.read(checksum_file_max_size)
    fp.close()
    if not fr:
        raise _checkError('%s is empty' % checksum_file)
    return fr


def retrieve(file_name):
    # Pawn 2021-01-14 for JIRA STASR3601-2428 begin
    if not file_name.startswith("/"):
        file_name = "/" + file_name
    # Pawn 2021-01-14 for JIRA STASR3601-2428 end

    json_str = check()
    checksum = ujson.loads(json_str)
    for item in checksum:
        if item['name'].lower() == file_name.lower():
            return item['crc32']
    return None


def _flush_checksum(checksum=[], file_name=checksum_file):
    json_str = ujson.dumps(checksum)
    fp = open(file_name, 'wt+')
    fp.seek(0)
    wl = fp.write(json_str)
    fp.close()
    if wl > 0:
        return checksum
    return None


def bak_update(file_name):
    # add by Jaxsen xu
    checksum = ql_fs.read_json(backup_checksum_file)

    if not file_name.startswith("/"):
        file_name = "/" + file_name

    src_file = file_name[file_name.find("/", 1):]
    exist_file = False
    for item in checksum:
        if item['name'].lower() == src_file.lower():
            exist_file = True
            file_crc32_value = file_crc32.calc(file_name)
            item['crc32'] = file_crc32_value
    if not exist_file:
        checksum.append(dict(name=src_file.lower(), crc32=file_crc32.calc(file_name)))
    return _flush_checksum(checksum, file_name=file_name[:file_name.find("/", 1)] + "/checksum.json")


def usr_update(file_name):
    try:
        data = ql_fs.read_json(checksum_file)
        if data is not None:
            exist_file = False
            for item in data:
                if item['name'].lower() == file_name.lower():
                    exist_file = True
                    item['crc32'] = file_crc32.calc(file_name)
            if not exist_file:
                data.append(dict(name=file_name.lower(), crc32=file_crc32.calc(file_name)))
            return ql_fs.touch(checksum_file, data)
        return None
    except Exception as e:
        return None


def update(file_name):
    try:
        json_str = check()
        checksum = ujson.loads(json_str)

        # Pawn 2021-01-14 for JIRA STASR3601-2428 begin
        if not file_name.startswith("/"):
            file_name = "/" + file_name
        # Pawn 2021-01-14 for JIRA STASR3601-2428 end

        for item in checksum:
            if item['name'].lower() == file_name.lower():
                file_crc32_value = file_crc32.calc(file_name)
                if file_crc32_value:
                    item['crc32'] = file_crc32_value
                    return _flush_checksum(checksum)
        return None
    except Exception:
        return None


def bulk_update(file_name_list=[]):
    try:
        json_str = check()
        checksum = ujson.loads(json_str)
        need_update = 0

        for file_name in file_name_list:
            # Pawn 2021-01-14 for JIRA STASR3601-2428 begin
            if not file_name.startswith("/"):
                file_name = "/" + file_name
            # Pawn 2021-01-14 for JIRA STASR3601-2428 end
            for item in checksum:
                if item['name'].lower() == file_name.lower():
                    file_crc32_value = file_crc32.calc(file_name)
                    if file_crc32_value:
                        item['crc32'] = file_crc32_value
                        need_update = 1
                        break
        if need_update:
            return _flush_checksum(checksum)
        return None
    except Exception:
        return None
