# -*- coding:utf-8 -*-
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


"""
jayceon 2021/03/23
"""

import dial

def setAutoActivate(profileID, enable,cur_simid=None):

    """
    if profileID < 1 or profileID > 3:
        raise ValueError("invalid value, profileID should be in [1,3].")
    if enable != 0 and enable != 1:
        raise ValueError("invalid value, enable should be 0 or 1.")
    if cur_simid != None and cur_simid != 0 and cur_simid != 1:
        raise ValueError("invalid value, enable should be 0 or 1.")
    
    if "datacall_config.json" in uos.listdir('/usr'):
        with open("/usr/datacall_config.json", "r", encoding='utf-8') as fd:
            try:
                datacall_config = ujson.load(fd)
                if not isinstance(datacall_config, dict):
                    raise ValueError("The format of the datacall_config.json is incorrect.")

                value_of_key_profileid = datacall_config.get(str(profileID), None)
                if value_of_key_profileid is not None:
                    if not isinstance(value_of_key_profileid, dict):
                        raise ValueError("The format of the datacall_config.json is incorrect.")
                    if cur_simid ==1:
                        datacall_config[str(profileID)]["autoActivate_1"] = enable
                    else:
                        datacall_config[str(profileID)]["autoActivate"] = enable
                    # value_of_key_autoConnect = value_of_key_profileid.get("autoConnect", 0)
                    # datacall_config[str(profileID)] = {"autoActivate": enable, "autoConnect": value_of_key_autoConnect}
                else:
                    # datacall_config[str(profileID)] = {"autoActivate": enable, "autoConnect": 0}
                    if cur_simid ==1:
                        datacall_config[str(profileID)] = {"autoActivate": 0, "autoConnect": 0,"autoActivate_1": enable}
                    else:
                        datacall_config[str(profileID)] = {"autoActivate": enable,"autoConnect": 0}
            except Exception:
                raise ValueError("The format of the datacall_config.json is incorrect.")
        with open("/usr/datacall_config.json", "w", encoding='utf-8') as fd:
            datacall_config_json = ujson.dumps(datacall_config)
            fd.write(datacall_config_json)
    else:
        # print('[Warning]The datacall_config.json file does not exist, create it now.')
        default_config = {
            "1": {"autoActivate": 0, "autoConnect": 0},
            "2": {"autoActivate": 0, "autoConnect": 0},
            "3": {"autoActivate": 0, "autoConnect": 0}
        }
        if cur_simid ==1:
            default_config[str(profileID)]["autoActivate_1"] = enable
        else:
            default_config[str(profileID)]["autoActivate"] = enable
        with open("/usr/datacall_config.json", "w", encoding='utf-8') as fd:
            default_config_json = ujson.dumps(default_config)
            fd.write(default_config_json)
    """
    return -1


def setAutoConnect(profileID, enable, cur_simid=None):
    """
    if profileID < 1 or profileID > 3:
        raise ValueError("invalid value, profileID should be in [1,3].")
    if enable != 0 and enable != 1:
        raise ValueError("invalid value, enable should be 0 or 1.")
    if cur_simid != None and cur_simid != 0 and cur_simid != 1:
        raise ValueError("invalid value, enable should be 0 or 1.")
    
    if "datacall_config.json" in uos.listdir('/usr'):
        with open("/usr/datacall_config.json", "r", encoding='utf-8') as fd:
            try:
                datacall_config = ujson.load(fd)
                if not isinstance(datacall_config, dict):
                    raise ValueError("The format of the datacall_config.json is incorrect.")
                value_of_key_profileid = datacall_config.get(str(profileID), None)
                if value_of_key_profileid is not None:
                    if not isinstance(value_of_key_profileid, dict):
                        raise ValueError("The format of the datacall_config.json is incorrect.")
                    if cur_simid ==1:
                        datacall_config[str(profileID)]["autoConnect_1"] = enable
                    else:
                        datacall_config[str(profileID)]["autoConnect"] = enable
                else:
                    if cur_simid ==1:
                        datacall_config[str(profileID)] = {"autoActivate": 0, "autoConnect": 0,"autoConnect_1": enable}
                    else:
                        datacall_config[str(profileID)] = {"autoActivate": 0,"autoConnect": enable}
            except Exception:
                raise ValueError("The format of the datacall_config.json is incorrect.")
        with open("/usr/datacall_config.json", "w", encoding='utf-8') as fd:
            datacall_config_json = ujson.dumps(datacall_config)
            fd.write(datacall_config_json)
    else:
        # print('[Warning]The datacall_config.json file does not exist, create it now.')
        default_config = {
            "1": {"autoActivate": 0, "autoConnect": 0},
            "2": {"autoActivate": 0, "autoConnect": 0},
            "3": {"autoActivate": 0, "autoConnect": 0}
        }
        if cur_simid ==1:
            default_config[str(profileID)]["autoConnect_1"] = enable
        else:
            default_config[str(profileID)]["autoConnect"] = enable
        with open("/usr/datacall_config.json", "w", encoding='utf-8') as fd:
            default_config_json = ujson.dumps(default_config)
            fd.write(default_config_json)
    """
    if cur_simid != None:
        dial.setAutoConnect(profileID, enable,cur_simid)
    else:
        dial.setAutoConnect(profileID, enable)



def setPDPContext(profileID, ipType, apn, username, password, authType, cur_simid=None):
    if profileID < 1 or profileID > 3:
        raise ValueError("invalid value, profileID should be in [1,3].")
    if authType < 0 or authType >3:
        raise ValueError("invalid value.")
    # Users can set apn, username, and password to None. 
    # When it is set to None, it means that the original values will not be changed and the original values will be used instead.
    if apn is None or username is None or password is None:
        pdp_context = dial.getPDPContext(profileID)
        if not isinstance(pdp_context,tuple) or len(pdp_context) < 3:
            raise ValueError("getPDPContext error.")
        if apn is None:
            apn = pdp_context[1]
        if username is None:
            username = pdp_context[2]
        if password is None:
            password = pdp_context[3]
    # if len(username) == 0 and len(password) == 0 and authType != 0:
    #     authType = 0
    if (len(username) != 0 or len(password) != 0) and authType == 0:
        authType = 2
    if cur_simid == None:
        return dial.setPDPContext(profileID, ipType, apn, username, password, authType)
    else:
        return dial.setPDPContext(profileID, ipType, apn, username, password, authType, cur_simid)


def getPDPContext(profileID, cur_simid=None):
    if profileID < 1 or profileID > 3:
        raise ValueError("invalid value, profileID should be in [1,3].")
    if cur_simid == None:
        return dial.getPDPContext(profileID)
    else:
        return dial.getPDPContext(profileID, cur_simid)
    


def setDNSServer(profileID, simID, priDNS, secDNS):
    if profileID < 1 or profileID > 3:
        raise ValueError("invalid value, profileID should be in [1,3].")
    return dial.setDnsserver(profileID, simID, priDNS, secDNS)


def activate(profileID, cur_simid=None):
    if profileID < 1 or profileID > 3:
        raise ValueError("invalid value, profileID should be in [1,3].")
    """
    if "datacall_config.json" in uos.listdir('/usr'):
        with open("/usr/datacall_config.json", "r", encoding='utf-8') as fd:
            try:
                datacall_config = ujson.load(fd)
                if not isinstance(datacall_config, dict):
                    raise ValueError("The format of the datacall_config.json is incorrect.")
                value_of_key_profileid = datacall_config.get(str(profileID), None)
                if value_of_key_profileid is not None:
                    if not isinstance(value_of_key_profileid, dict):
                        raise ValueError("The format of the datacall_config.json is incorrect.")
                    auto_connect = value_of_key_profileid.get("autoConnect", 0)
                    dial.setAutoConnect(profileID, auto_connect)
                else:
                    raise ValueError("No configuration information for profileID {}.".format(profileID))
            except Exception:
                raise ValueError("The format of the datacall_config.json is incorrect.")
    """
    if cur_simid == None:
        return dial.start(profileID, 0, "", "", "", 0)
    else:
        return dial.start(profileID, 0, "", "", "", 0,cur_simid)


def deactivate(profileID, cur_simid=None):
    if profileID < 1 or profileID > 3:
        raise ValueError("invalid value, profileID should be in [1,3].")

    pdpctx = dial.getPDPContext(profileID)
    if pdpctx != -1:
        iptype = pdpctx[0]
        if cur_simid == None:
            return dial.stop(profileID, iptype)
        else:
            return dial.stop(profileID, iptype,cur_simid)
    else:
        return -1

######################################################################################

def start(profileidx, iptype=0, apn="", username="", password="", authtype=0, cur_simid=None):
    ret = dial.setPDPContext(profileidx, iptype, apn, username, password, authtype)
    if ret == 0:
        if cur_simid == None:
            return dial.start(profileidx, iptype, apn, username, password, authtype)
        else:
            return dial.start(profileidx, iptype, apn, username, password, authtype, cur_simid)
    else:
        return -1


def stop(profileidx, iptype,cur_simid=None):
    if cur_simid == None:
        return dial.stop(profileidx, iptype)
    else:
        return dial.stop(profileidx, iptype, cur_simid)


def getInfo(profileidx, iptype, cur_simid=None):
    if cur_simid == None:
        ret = dial.getInfo(profileidx, iptype)
    else:
        ret = dial.getInfo(profileidx, iptype, cur_simid)
    if ret == -1:
        ipv4 = [0, 0, '0.0.0.0', '0.0.0.0', '0.0.0.0']
        ipv6 = [0, 0, '::', '::', '::']
        if iptype ==2:
            return (profileidx, iptype, ipv4, ipv6)
        elif iptype == 1:
            return (profileidx, iptype, ipv6)
        elif iptype == 0:
            return (profileidx, iptype, ipv4)
    return ret 

def getAddressinfo(profileidx, iptype):
    ret = dial.getAddressinfo(profileidx, iptype)
    if ret == -1:
        ipv4 = ['00-00-00-00-00-00', '0.0.0.0', '0.0.0.0']
        ipv6 = ['00-00-00-00-00-00', '::', '::']
        if iptype ==2:
            return (ipv4, ipv6)
        elif iptype == 1:
            return (ipv6)
        elif iptype == 0:
            return (ipv4)
    return ret

def getSiminfo(simID):
    ret = dial.getSiminfo(simID)
    if ret == -1:
        return (0, 0, 0, 0)
    return ret

def setAsynMode(mode):
    return dial.setAsynMode(mode)

def setCallback(usrfun):
    return dial.setCallback(usrfun)

def setDnsserver(profileidx, simid, new_pri, new_sec):
    return dial.setDnsserver(profileidx, simid, new_pri, new_sec)

def getSpeed():
    return dial.getSpeed()

