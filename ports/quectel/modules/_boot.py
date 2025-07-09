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

import uos
import ujson
import dial

'''
Mark.Zhu - 2022/12/02
Added the littlefs2 mount function,it will be called after a failed attempt to mount littlefs1
'''
# global datacall_flag
datacall_flag = 1

def _auto_activate_pdpcontext():
    if "datacall_config.json" not in uos.listdir('/usr'):
        # raise ValueError("Not found datacall_config.json.")
        dial.setAutoConnect(1, 1)
        dial.setAsynMode(1)
        dial.start(1, 0, '', '', '', 0)
        dial.setAsynMode(0)
    else:
        retval = dial.getPdpRange()
        max_profile = retval[1]

        with open("/usr/datacall_config.json", "r", encoding='utf-8') as fd:
            try:
                datacall_config = ujson.load(fd)
                if not isinstance(datacall_config, dict):
                    raise ValueError("The format of the datacall_config.json is incorrect.")

                for profile_id in range(1, max_profile+1):
                    value_of_key_profileid = datacall_config.get(str(profile_id), None)
                    if value_of_key_profileid is not None:
                        if not isinstance(value_of_key_profileid, dict):
                            raise ValueError("The format of the datacall_config.json is incorrect.")
                        value_of_key_autoActivate = value_of_key_profileid.get("autoActivate", 0)
                        value_of_key_autoConnect = value_of_key_profileid.get("autoConnect", 0)
                        if value_of_key_autoConnect==1:
                            dial.setAutoConnect(profile_id, value_of_key_autoConnect)
                        if value_of_key_autoActivate == 1:
                            dial.setAsynMode(1)
                            ret = -1
                            repeat_count = 2
                            while (ret == -1) and (repeat_count >= 0):
                                repeat_count -= 1
                                ret = dial.start(profile_id, 0, '', '', '', 0)
                            if ret == -1:
                                print('Activation of the {} PDP Context failed.'.format(profile_id))
                            dial.setAsynMode(0)
                    else:
                        pass
            except Exception:
                raise ValueError("The format of the datacall_config.json is incorrect.")


def _check_data_call():
    retval = dial.getPdpRange()
    min = retval[0]
    max = retval[1]

    for pdp in range(min, max+1):
        nw_sta = dial.getInfo(pdp, 2)
        if (nw_sta != -1) and (nw_sta[2][0] == 0) and (nw_sta[3][0] == 0):
            continue
        elif (nw_sta != -1) and ((nw_sta[2][0] == 1) or (nw_sta[3][0] == 1)):
            return 1
    return 0


def _repl_enable():
    global datacall_flag
    if "system_config.json" in uos.listdir("/usr/"):
        with open("/usr/system_config.json", "r", encoding='utf-8') as fd:
            try:
                json_data = ujson.load(fd)
                repl_flag = json_data.get("replFlag", 0)
                datacall_flag = json_data.get("datacallFlag", 1)
                repl_pswd = json_data.get("replPswd",None)
                
                import misc
                if repl_pswd:
                    misc.replUpdatePassswd(repl_pswd,repl_pswd)
                    misc.replEnable(repl_flag, repl_pswd)
                else:
                    misc.replEnable(repl_flag)
            except ValueError:
                with open("/usr/system_config.json", "w", encoding='utf-8') as fdw:
                    new_json_data = ujson.dumps({"replFlag": 0, "datacallFlag": datacall_flag})
                    fdw.write(new_json_data)
    else:
        with open("/usr/system_config.json", "w+", encoding='utf-8') as fd:
            repl_data = ujson.dumps({"replFlag": 0})
            fd.write(repl_data)

try: 
    udev = None
    bdev = None
    try:
        from uos import VfsLfs1 as VfsLfs
    except Exception:
        from uos import VfsLfs2 as VfsLfs

    udev = uos.FlashDevice('customer_fs', 4096)
    try:
        uos.mount(udev, '/usr')
    except Exception as e:
        if 'ENODEV' in str(e):
            VfsLfs.mkfs(udev)
            uos.mount(udev, '/usr')
        else:
            raise ValueError("LFS usr mount fail".format(e))
    
    bdev = uos.FlashDevice('customer_backup_fs', 4096)
    try:
        uos.mount(bdev, '/bak')
    except Exception as e:
        if 'ENODEV' in str(e):
            VfsLfs.mkfs(bdev)
            uos.mount(bdev, '/bak')
        else:
            raise ValueError("LFS bak mount fail".format(e))


except Exception:
    print('error ocurs in boot step.')

finally:
    _repl_enable()
    if datacall_flag == 1:
        ret = _check_data_call()
        if ret == 0:
            _auto_activate_pdpcontext()
