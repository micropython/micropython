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

import os
import ujson

"""
Mark.Zhu - 2022/12/02
Added the littlefs2 mount function,it will be called after a failed attempt to mount littlefs1
"""
# global datacall_flag
datacall_flag = 1


def _repl_enable():
    global datacall_flag
    if "system_config.json" in os.listdir("/"):
        with open("/system_config.json", "r", encoding="utf-8") as fd:
            try:
                json_data = ujson.load(fd)
                datacall_flag = json_data.get("datacallFlag", 1)
            except ValueError:
                with open("/usr/system_config.json", "w", encoding="utf-8") as fdw:
                    new_json_data = ujson.dumps({"replFlag": 0, "datacallFlag": datacall_flag})
                    fdw.write(new_json_data)
    else:
        with open("/system_config.json", "w+", encoding="utf-8") as fd:
            repl_data = ujson.dumps({"replFlag": 0})
            fd.write(repl_data)


try:
    udev = None
    try:
        from os import VfsLfs1 as VfsLfs
    except Exception:
        from os import VfsLfs2 as VfsLfs

    udev = os.FlashDevice("customer_fs", 4096)
    try:
        os.mount(udev, "/")
    except Exception as e:
        if "ENODEV" in str(e):
            VfsLfs.mkfs(udev)
            os.mount(udev, "/")
    if "usr" not in os.listdir():
        os.mkdir("usr")
    if "bak" not in os.listdir():
        os.mkdir("bak")

except Exception:
    print("error occurs in boot step.")

finally:
    _repl_enable()
