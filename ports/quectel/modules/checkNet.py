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


'''
jayceon 2020/12/31
'''

import sim
import net
import utime
import modem
import dial
import dataCall
# from misc import Power

class CheckNetwork():
    def __init__(self, proj_name, proj_version):
        self.PROJECT_NAME = proj_name
        self.PROJECT_VERSION = proj_version
        self.FIRMWARE_VERSION = modem.getDevFwVersion()
        # self.POWERON_REASON = Power.powerOnReason()

    def poweron_print_once(self):
        sim_sta = sim.getStatus()
        print('==================================================')
        print('PROJECT_NAME     : {}'.format(self.PROJECT_NAME))
        print('PROJECT_VERSION  : {}'.format(self.PROJECT_VERSION))
        print('FIRMWARE_VERSION : {}'.format(self.FIRMWARE_VERSION))
        # print('POWERON_REASON   : {}'.format(self.POWERON_REASON))
        print('SIM_CARD_STATUS  : {}'.format(sim_sta))
        print('==================================================')

    @staticmethod
    def check_datacall_status():
        retval = dial.getPdpRange()
        min = retval[0]
        max = retval[1]
        for pdp in range(min, max+1):
            datacall_sta = dataCall.getInfo(pdp, 2)
            if (datacall_sta != -1) and ((datacall_sta[2][0] == 1) or (datacall_sta[3][0] == 1)):
                return 1
            elif (datacall_sta != -1) and (datacall_sta[2][0] == 0) and (datacall_sta[3][0] == 0):
                continue
        return 0

    '''
    return format: stagecode, subcode, time
    stagecode - 
        1:Currently in the stage of getting SIM card state
        2:Currently in the stage of getting network state
        3:Currently in the stage of getting datacall state
    subcode -
        when stagecode = 1, subcode represents the state of the SIM card
        when stagecode = 2, subcode represents the state of the network
        when stagecode = 3, subcode represents the state of the datacall
    '''
    def wait_network_connected(self, timeout_s=60):
        if (timeout_s < 1) or (timeout_s > 3600):
            raise OSError("timeout_s should be in [1, 3600]s!")
        timeout_ms = timeout_s * 1000
        
        '''
        stage_code = 1
        while True:
            sim_sta = sim.getStatus()
            if sim_sta == 1:
                break
            elif sim_sta == 0:
                return stage_code, sim_sta
            elif (sim_sta == 18) or (sim_sta == 20) or (sim_sta == 21):
                utime.sleep_ms(100)
                timeout_ms -= 100
                if timeout_ms <= 0:
                    return stage_code, sim_sta
            else:
                return stage_code, sim_sta

        stage_code = 2
        while True:
            net_sta = net.getState()
            if net_sta == -1:
                utime.sleep_ms(100)
                timeout_ms -= 100
                if timeout_ms <= 0:
                    return stage_code, -1
            else:
                if (net_sta[1][0] == 1) or (net_sta[1][0] == 5):
                    break
                else:
                    utime.sleep_ms(100)
                    timeout_ms -= 100
                    if timeout_ms <= 0:
                        return stage_code, net_sta[1][0]

        stage_code = 3
        while True:
            datacall_sta = self.check_datacall_status()
            if datacall_sta == 1:
                return stage_code, datacall_sta
            else:
                utime.sleep_ms(100)
                timeout_ms -= 100
                if timeout_ms <= 0:
                    return stage_code, datacall_sta
        '''
        current_time = utime.ticks_ms()
        last_time = current_time
        stage_code = 1
        while True:
            sim_sta = sim.getStatus()
            if sim_sta == 1:
                break
            else:
                utime.sleep_ms(100)
                current_time = utime.ticks_ms()
                timeout_ms -= utime.ticks_diff(current_time, last_time)
                last_time = current_time
                if timeout_ms <= 0:
                    return stage_code, sim_sta
                    
        current_time = utime.ticks_ms()
        last_time = current_time
        stage_code = 2
        while True:
            net_sta = net.getState()
            if net_sta == -1:
                utime.sleep_ms(100)
                #timeout_ms -= 100
                current_time = utime.ticks_ms()
                timeout_ms -= utime.ticks_diff(current_time, last_time)
                last_time = current_time
                if timeout_ms <= 0:
                    return stage_code, -1
            else:
                if (net_sta[1][0] == 1) or (net_sta[1][0] == 5):
                    break
                else:
                    utime.sleep_ms(100)
                    #timeout_ms -= 100
                    current_time = utime.ticks_ms()
                    timeout_ms -= utime.ticks_diff(current_time, last_time)
                    last_time = current_time
                    if timeout_ms <= 0:
                        return stage_code, net_sta[1][0]
                        
        current_time = utime.ticks_ms()
        last_time = current_time
        stage_code = 3
        while True:
            datacall_sta = self.check_datacall_status()
            if datacall_sta == 1:
                return stage_code, datacall_sta
            else:
                utime.sleep_ms(100)
                #timeout_ms -= 100
                current_time = utime.ticks_ms()
                timeout_ms -= utime.ticks_diff(current_time, last_time)
                last_time = current_time
                if timeout_ms <= 0:
                    return stage_code, datacall_sta


def wait_network_connected(timeout_s=60):
    if (timeout_s < 1) or (timeout_s > 3600):
        raise OSError("timeout_s should be in [1, 3600]s!")
    timeout_ms = timeout_s * 1000

    current_time = utime.ticks_ms()
    last_time = current_time
    stage_code = 1
    while True:
        sim_sta = sim.getStatus()
        if sim_sta == 1:
            break
        else:
            utime.sleep_ms(100)
            current_time = utime.ticks_ms()
            timeout_ms -= utime.ticks_diff(current_time, last_time)
            last_time = current_time
            if timeout_ms <= 0:
                return stage_code, sim_sta

    current_time = utime.ticks_ms()
    last_time = current_time
    stage_code = 2
    while True:
        net_sta = net.getState()
        if net_sta == -1:
            utime.sleep_ms(100)
            current_time = utime.ticks_ms()
            timeout_ms -= utime.ticks_diff(current_time, last_time)
            last_time = current_time
            if timeout_ms <= 0:
                return stage_code, -1
        else:
            if (net_sta[1][0] == 1) or (net_sta[1][0] == 5):
                break
            else:
                utime.sleep_ms(100)
                current_time = utime.ticks_ms()
                timeout_ms -= utime.ticks_diff(current_time, last_time)
                last_time = current_time
                if timeout_ms <= 0:
                    return stage_code, net_sta[1][0]

    current_time = utime.ticks_ms()
    last_time = current_time
    stage_code = 3
    while True:
        datacall_sta = CheckNetwork.check_datacall_status()
        if datacall_sta == 1:
            return stage_code, datacall_sta
        else:
            utime.sleep_ms(100)
            current_time = utime.ticks_ms()
            timeout_ms -= utime.ticks_diff(current_time, last_time)
            last_time = current_time
            if timeout_ms <= 0:
                return stage_code, datacall_sta
