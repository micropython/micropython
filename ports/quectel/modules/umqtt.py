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
name, platform = uos.uname()[1].split("=",1)

import utime
import log
import _thread
import usocket as socket
import ustruct as struct
import osTimer

if platform != "FCM360W" and platform != "FCM362K":
    import net
    import dataCall

log.basicConfig(level=log.INFO)
mqtt_log = log.getLogger("MQTT")
# bob.xian 2023/9/20 [FCM360W may be a bit unstable when using the log module, so print is temporarily used] begin
if platform == "FCM360W":
    mqtt_log.info = print
    mqtt_log.warning = print
# bob.xian 2023/9/20 [FCM360W may be a bit unstable when using the log module, so print is temporarily used] end

#ALIYDEVREGISTER = False
#TOPICLIST = list()
#SERVERACK = set()  # PUBACK and SUBACK pids awaiting ACK response
#PING = False

class MQTTException(Exception):
    pass

def pid_gen():
    pid = 0
    while True:
        pid = pid + 1 if pid < 65535 else 1
        yield pid

class BaseMqtt(object):
    CONNECTEXCE = -1
    CONNECTSUCCESS = 0
    ARECONNECT = 1
    SEVCLOSE = 2

    def __init__(self, client_id, server, port=0, user=None, password=None, keepalive=0,
                 ssl=False, ssl_params={}, reconn=True, version=4, pingmaxnum=0, manage=False, ipvtype=0):
        if port == 0:
            port = 8883 if ssl else 1883
        self.client_id = client_id
        self.sock = None
        self.server = server
        self.port = port
        self.ssl = ssl
        self.ssl_params = ssl_params
        self.pid = 0
        self.__newpid = pid_gen()
        self.__rcv_pids = set()  # PUBACK and SUBACK pids awaiting ACK response
        self.cb = None
        self.user = user
        self.pswd = password
        self.keepalive = keepalive
        self.lw_topic = None
        self.lw_msg = None
        self.lw_qos = 0
        self.lw_retain = False
        self.last_time = None
        self.timerFlag = True
        self.pingFlag = False
        self.connSta = False
        self.reconn = reconn
        self.topic_list = []
        self.addr = None
        self.qos = 0
        self.mqttversion = version
        self.clean_session = True
        self.ping_outstanding = False
        self.pingmaxnum = pingmaxnum
        self.pingnum = 0
        self.mqttlock = None
        self.mqttsendlock = None
        self.mqttmsglock = None
        self.threadId = None
        self.__response_time = 5000
        self.wait_flag = 0
        self.ipvtype = ipvtype
        self.manage = manage
        self.looptask_id = None
        self.PING = False
        self.SERVERACK = set()  # PUBACK and SUBACK pids awaiting ACK response
        self.TOPICLIST = list()
        self.ALIYDEVREGISTER = False
        if keepalive != 0 and keepalive < 5:
            raise ValueError("inport parameter error : keepalive >= 5S")

    def _send_str(self, s):
        self.sock.write(struct.pack("!H", len(s)))
        self.sock.write(s)

    def _recv_len(self):
        n = 0
        sh = 0
        while 1:
            b = self.sock.read(1)[0]
            n |= (b & 0x7f) << sh
            if not b & 0x80:
                return n
            sh += 7

    def set_callback(self, f):
        self.cb = f

    def set_last_will(self, topic, msg, retain=False, qos=0):
        assert 0 <= qos <= 2
        assert topic
        self.lw_topic = topic
        self.lw_msg = msg
        self.lw_qos = qos
        self.lw_retain = retain

    def connect(self, clean_session=True):
        if self.mqttlock is None:
            self.mqttlock = _thread.allocate_lock()
        if self.mqttsendlock is None:
            self.mqttsendlock = _thread.allocate_lock()
        if self.mqttmsglock is None:
            self.mqttmsglock = _thread.allocate_lock()
        j = 1
        while True:
            if j > 5:
                raise ValueError("DNS Parsing NULL")
            try:
                addrall = socket.getaddrinfo(self.server, self.port)
            except Exception as e:
                raise ValueError("DNS Parsing '{}'".format(self.server))
            if not addrall:
                j += 1
                utime.sleep_ms(500)
                continue
            if self.ipvtype == 1:
                for i in addrall:
                    if i[0] == 10:
                        self.addr = i[-1]
                        break
                if self.addr is None:
                    raise ValueError("DNS Parsing IPV6 Fail")
            else:
                try:
                    self.addr = addrall[0][-1]
                except Exception as e:
                    raise ValueError("DNS Parsing IPV4 Fail")
            break

        if self.ipvtype == 1:
            if platform == "FCM360W" or platform == "FCM362K":
                print("Not Support!")
                return
            self.sock = socket.socket(socket.AF_INET6, socket.SOCK_STREAM, socket.TCP_CUSTOMIZE_PORT)
            call_state = dataCall.getInfo(1,1)
            if call_state == -1:
                raise ValueError("MQTT Connect dataCall get Info FAIL")
            elif call_state[2][0] != 1:
                raise ValueError("MQTT Connect dataCall get IPV6 Info FAIL")
            try:
                self.sock.bind((call_state[2][2],0))
            except Exception as e:
                raise ValueError("sock bind IPV6 IP FAIL")
        else:
            self.sock = socket.socket()
        self.sock.settimeout(20)
        self.sock.connect(self.addr)
        if self.ssl:
            import ussl
            self.sock = ussl.wrap_socket(self.sock, **self.ssl_params, server_hostname = self.server)

        if self.mqttversion == 3:
            protocol = b"MQIsdp"
        else:
            protocol = b"MQTT"

        connect_flags = 0
        connect_flags = clean_session << 1
        remaining_length = 2 + len(protocol) + 1 + 1 + 2

        if self.client_id:
            remaining_length = remaining_length + 2 + len(self.client_id)
        if self.user:
            remaining_length = remaining_length + 2 + len(self.user)
            connect_flags = connect_flags | 0x80
        if self.pswd:
            connect_flags = connect_flags | 0x40
            remaining_length = remaining_length + 2 + len(self.pswd)
        if self.lw_topic:
            connect_flags = connect_flags | 0x04 | ((self.lw_qos & 0x03) << 3) | ((self.lw_retain & 0x01) << 5)
            remaining_length += 2 + len(self.lw_topic) + 2 + len(self.lw_msg)

        command = 0x10
        packet = bytearray()
        packet.extend(struct.pack("!B", command))
        remaining_bytes = []
        while True:
            byte = remaining_length % 128
            remaining_length = remaining_length // 128
            if remaining_length > 0:
                byte = byte | 0x80

            remaining_bytes.append(byte)
            packet.extend(struct.pack("!B", byte))
            if remaining_length == 0:
                break

        if self.mqttversion == 3:
            packet.extend(struct.pack("!H" + str(len(protocol)) + "sBBH", len(protocol), protocol, 3, connect_flags,
                                      self.keepalive))
        else:
            packet.extend(struct.pack("!H" + str(len(protocol)) + "sBBH", len(protocol), protocol, 4, connect_flags,
                                      self.keepalive))
        if self.client_id:
            packet.extend(struct.pack("!H" + str(len(self.client_id)) + "s", len(self.client_id), self.client_id))
        if self.lw_topic:
            packet.extend(struct.pack("!H" + str(len(self.lw_topic)) + "s", len(self.lw_topic), self.lw_topic))
            packet.extend(struct.pack("!H" + str(len(self.lw_msg)) + "s", len(self.lw_msg), self.lw_msg))
        if self.user:
            packet.extend(struct.pack("!H" + str(len(self.user)) + "s", len(self.user), self.user))
        if self.pswd:
            packet.extend(struct.pack("!H" + str(len(self.pswd)) + "s", len(self.pswd), self.pswd))

        self.sock.write(packet)

        resp = self.sock.read(4)
        self.sock.setblocking(True)
        assert resp[0] == 0x20 and resp[1] == 0x02
        if resp[3] != 0:
            raise MQTTException(resp[3])
        # self.last_time = utime.mktime(utime.localtime())
        self.last_time = utime.ticks_ms() // 1000
        self.connSta = True
        self.clean_session = clean_session
        self.PING = False
        self.pingnum = self.pingmaxnum
        self.ALIYDEVREGISTER = self.sock
        return resp[2] & 1

    def disconnect(self):
        # Pawn.zhou 2021/1/12 for JIRA STASR3601-2523 begin
        try:
            self.timerFlag = False
            self.pingFlag = False
            self.connSta = False
            self.PING = False
            self.pingnum = self.pingmaxnum
            self.topic = []
            self.sock.write(b"\xe0\0")
        # Pawn.zhou 2021/1/12 for JIRA STASR3601-2523 end
        except:
            mqtt_log.warning("Error send mqtt_dis FAIL.")
        try:
            if self.mqttsendlock.locked():
                self.mqttsendlock.release()
            if self.mqttlock.locked():
                self.mqttlock.release()
            if self.mqttmsglock.locked():
                self.mqttmsglock.release()
            if self.mqttlock is not None:
                _thread.delete_lock(self.mqttlock)
                self.mqttlock = None
            if self.mqttsendlock is not None:
                _thread.delete_lock(self.mqttsendlock)
                self.mqttsendlock = None
            if self.mqttmsglock is not None:
                _thread.delete_lock(self.mqttmsglock)
                self.mqttmsglock = None
        except:
            mqtt_log.warning("Error delete_lock FAIL.")
        utime.sleep_ms(500)
        try:
            self.sock.close()
        except:
            mqtt_log.warning("Error socket close FAIL.")

    def close(self):
        self.sock.close()

    def ping(self):
        # self.last_time = utime.mktime(utime.localtime())
        self.last_time = utime.ticks_ms() // 1000
        self.sock.write(b"\xc0\0")
        self.PING = True

    def publish(self, topic, msg, retain=False, qos=0):
        pkt = bytearray(b"\x30\0\0\0")
        pid = next(self.__newpid)
        pkt[0] |= qos << 1 | retain
        sz = 2 + len(topic) + len(msg)
        if qos > 0:
            sz += 2
        assert sz < 2097152
        i = 1
        while sz > 0x7f:
            pkt[i] = (sz & 0x7f) | 0x80
            sz >>= 7
            i += 1
        pkt[i] = sz
        self.sock.write(pkt, i + 1)
        self._send_str(topic)
        if qos > 0:
            self.SERVERACK.add(pid)
            struct.pack_into("!H", pkt, 0, pid)
            self.sock.write(pkt, 2)
        self.sock.write(msg)
        # self.last_time = utime.mktime(utime.localtime())
        if qos == 0:
            if self.keepalive >= 30:
                # time = utime.mktime(utime.localtime())
                time = utime.ticks_ms() // 1000
                if (time - self.last_time) > (self.keepalive - 15):
                    self.last_time = time - self.keepalive
            return True

        if qos == 1:
            if self.wait_flag == 0:
                self.wait_msg()
            if not self._await_pid(pid):
                mqtt_log.warning("publish Pid[%s] QOS1 message was not received correctly"%pid)
                return False
            else:
                # self.last_time = utime.mktime(utime.localtime())
                self.last_time = utime.ticks_ms() // 1000
                return True
        elif qos == 2:
            mqtt_log.warning("publish QOS2 Not Support")
            assert 0

    def __subscribe(self, topic, qos, sock):
        if sock:
            self.sock = sock
        pkt = bytearray(b"\x82\0\0\0")
        pid = next(self.__newpid)
        self.SERVERACK.add(pid)
        struct.pack_into("!BH", pkt, 1, 2 + 2 + len(topic) + 1, pid)
        self.sock.write(pkt)
        self._send_str(topic)
        self.sock.write(qos.to_bytes(1, "little"))
        if self.ssl == False:
            self.sock.settimeout(5)
        while True:
            if self.wait_flag == 0:
                if self.mqttmsglock.locked():
                    return True
                op = self.wait_msg()
                if op == 0x90:
                    return True
                else:
                    return False
            else:
                if not self._await_pid(pid):
                    return False
                else:
                    return True

    def subscribe(self, topic, qos=0, sock=None):
        assert self.cb is not None, "Subscribe callback is not set"
        if topic not in self.topic_list:
            self.topic_list.append(topic)
            self.TOPICLIST = self.topic_list
        self.qos = qos
        self.__subscribe(topic, qos=qos, sock=sock)

    def publish_ack(self, pid):
        pkt = bytearray(b"\x40\x02\0\0")
        struct.pack_into("!H", pkt, 2, pid)
        while True:
            try:
                if self.mqttsendlock.locked():
                    utime.sleep_ms(10)
                    continue
                self.mqttsendlock.acquire()
                self.sock.write(pkt)
                self.mqttsendlock.release()
                return
            except Exception as e:
                mqtt_log.warning("Publish Pid[%s] ACK OSError[%s]." %(pid,  str(e)))
                if self.mqttsendlock.locked():
                    self.mqttsendlock.release()
                return

    # Wait for a single incoming MQTT message and process it.
    # Subscribed messages are delivered to a callback previously
    # set by .set_callback() method. Other (internal) MQTT
    # messages processed internally.
    def wait_msg(self):
        global SUBACK
        SUBACK = True
        if self.ssl:
            res = self.sock.read(1)
        else:
            res = self.sock.recv(1)
        self.sock.setblocking(True)
        if res is None:
            return None
        if res == b"":
            # raise OSError(-1)
            # Pawn 2020/11/14 - WFZT mqttBUg -1
            return None

        if res == b"\xd0":  # PINGRESP
            self.PING = False
            sz = self.sock.read(1)[0]
            assert sz == 0
            return res

        if res == b'\x40':  # PUBACK: save pid
            sz = self.sock.read(1)
            if sz != b"\x02":
                mqtt_log.warning("Publish message does not return ACK correctly")
                return
            rcv_pid = self.sock.read(2)
            pid = rcv_pid[0] << 8 | rcv_pid[1]
            if pid in self.SERVERACK:
                self.SERVERACK.discard(pid)
                return
            else:
                return

        sub_ack = res[0]
        if (sub_ack & 0xf0) == 0x90:  # SUBSCRIBE:
            resp = self.sock.read(4)
            pid = resp[1] << 8 | resp[2]
            if pid in self.SERVERACK:
                self.SERVERACK.discard(pid)
            if resp[3] == 0x80:
                mqtt_log.warning("subscribe topic ACK Fail")
                return 0x80
            else:
                return 0x90

        op = res[0]
        if op & 0xf0 != 0x30:
            return op
        sz = self._recv_len()
        topic_len = self.sock.read(2)
        topic_len = (topic_len[0] << 8) | topic_len[1]
        topic = self.sock.read(topic_len)
        sz -= topic_len + 2
        if op & 6:
            pid = self.sock.read(2)
            pid = pid[0] << 8 | pid[1]
            sz -= 2
        msg = self.sock.read(sz)
        # self.last_time = utime.mktime(utime.localtime())
        if op & 6 == 2:
            task_stacksize = _thread.stack_size()
            name,platform = uos.uname()[1].split("=",1)
            if platform == "EC600E" or platform == "EC800E" or platform == "FCM360W" or platform == "FCM362K":
                _thread.stack_size(2*1024)
            _thread.start_new_thread(self.publish_ack, (pid,))
            _thread.stack_size(task_stacksize)
        if op & 6 == 4:
            assert 0
        try:
            self.cb(topic, msg)
        except Exception as e:
            mqtt_log.warning("set_callback OSError[%s]." % str(e))

    # Checks whether a pending message from server is available.
    # If not, returns immediately with None. Otherwise, does
    # the same processing as wait_msg.
    def check_msg(self):
        self.sock.setblocking(False)
        return self.wait_msg()

    def _timeout(self, t):
        return utime.ticks_diff(utime.ticks_ms(), t) > self.__response_time

    def _await_pid(self, pid):
        t = utime.ticks_ms()
        while pid in self.SERVERACK:  # local copy
            if self._timeout(t):
                break  # Must repub or bail out
            utime.sleep_ms(500)
        else:
            return True  # PID received. All done.
        return False

    def get_mqttsta(self):
        '''
        Get the MQTT connection status
        CONNECTEXCE     -1:Connect the interrupt
        CONNECTSUCCESS  0:connection is successful
        ARECONNECT      1:In the connection
        SEVCLOSE        2:server closes the connection
        '''
        socket_sta = self.sock.getsocketsta()
        if socket_sta == 0:
            return self.ARECONNECT
        elif (socket_sta == 2) or (socket_sta == 3):
            return self.ARECONNECT
        elif (socket_sta == 4) and self.connSta:
            return self.CONNECTSUCCESS
        elif (socket_sta == 7) or (socket_sta == 8):
            return self.SEVCLOSE
        else:
            return self.CONNECTEXCE


class MQTTClient(BaseMqtt):
    DELAY = 2
    DEBUG = False

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.error_callback = None
        self.subtimer = osTimer()

    def delay(self, i):
        utime.sleep(self.DELAY)

    def disconnect(self):
        try:
            self.subtimer.stop()
            self.subtimer.delete_timer()
        except:
            pass
        super().disconnect()

    def sub_timer_task(self, arg):
        if not self.topic_list:
            self.topic_list = self.TOPICLIST
        for topic_re in self.topic_list:
            super().subscribe(topic_re, qos=self.qos, sock=self.ALIYDEVREGISTER)

    def error_register_cb(self, func):
        self.error_callback = func

    def base_reconnect(self):
        i = 0
        if self.mqttlock.locked():
            return
        self.mqttlock.acquire()
        if self.looptask_id is not None and self.PING != True:
            if self.mqttsendlock.locked():
                self.mqttsendlock.release()
            if self.wait_flag == 0:
                _thread.stop_thread(self.looptask_id)
            self.looptask_id = None
        if self.reconn:
            if self.error_callback is not None:
                e = "reconnect_start"
                self.error_callback(e)#start reconnect
        while 1:
            try:
                if not self.timerFlag:
                    break
                #felix.hou 2024.5.11 add ,close() may cause reconnecting to the mqtt server to fail
                if platform == "FCM360W" or platform == "FCM362K":
                    self.sock.shutdown()
                    utime.sleep_ms(20)
                    self.sock.close()
                else:
                    self.sock.close()
                #felix.hou 2024.5.11 add ,close() may cause reconnecting to the mqtt server to fail
                time_info = self.logTime()
                mqtt_log.info(
                    "[%s] The network condition has been restored and an attempt will be made to reconnect" % time_info)
                self.pingFlag = False
                self.connSta = False
                self.connect(self.clean_session)
                mqtt_log.info("[%s] Reconnection successful!" % time_info)
                if self.manage:
                    self.subtimer.start(3000, 0, self.sub_timer_task)
                else:
                    if not self.topic_list:
                        self.topic_list = self.TOPICLIST
                    for topic_re in self.topic_list:
                        super().subscribe(topic_re, self.qos)
                time_info = self.logTime()
                if self.mqttlock.locked():
                    self.mqttlock.release()
                if self.reconn:
                    if self.error_callback is not None:
                        e = "reconnect_success"
                        try:
                            self.error_callback(e)#reconnect success
                        except Exception as e:
                            mqtt_log.warning("error_callback OSError[%s]." % str(e))
                return
            except Exception as e:
                if not self.timerFlag:
                    break
                i += 1
                time_info = self.logTime()
                mqtt_log.warning(
                    "[%s] The connection attempt failed [%s] and will be tried again after %d seconds." % (time_info, str(e), 5 + i))
                utime.sleep(5 + i)
                if platform != "FCM360W" and platform != "FCM362K":
                    net_sta = net.getState()
                    if net_sta != -1 and ((net_sta[1][0] == 1) or (net_sta[1][0] == 5)):
                        call_state = dataCall.getInfo(1, 0)
                        if call_state == -1:
                            mqtt_log.info("LTE datacall Fail.")
                        elif call_state[2][0] != 1:
                            mqtt_log.info("LTE datacall IPV4 Fail.")
                    else:
                        mqtt_log.info("LTE Net unregistered.")

    def publish(self, topic, msg, retain=False, qos=0):
        while True:
            try:
                if self.mqttsendlock.locked():
                    utime.sleep_ms(10)
                    continue
                self.mqttsendlock.acquire()
                ret = super().publish(topic, msg, retain, qos)
                self.mqttsendlock.release()
                return ret
            except Exception as e:
                mqtt_log.warning("Publish Fail OSError[%s]." % str(e))
                if self.mqttsendlock.locked():
                    self.mqttsendlock.release()
                return False

    def wait_msg(self):
        while True:
            try:
                # The state changes when disconnect is called
                if not self.timerFlag:
                    break
                self.wait_flag = 1
                if self.mqttmsglock.locked():
                    utime.sleep_ms(10)
                    continue
                self.mqttmsglock.acquire()
                ret = super().wait_msg()
                self.mqttmsglock.release()
                self.wait_flag = 0
                return ret
            except Exception as e:
                if self.mqttmsglock is not None and self.mqttmsglock.locked():
                    self.mqttmsglock.release()
                if not self.timerFlag:
                    break
                if not self.reconn:
                    raise e
                # Whether to use the built-in reconnect mechanism
                time_info = self.logTime()
                self.wait_flag = 0
                if self.ping == True:
                    mqtt_log.warning("[%s] wait msg, send ping timeout. Trying to reconnect" % time_info)
                else:
                    mqtt_log.warning("[%s] wait msg OSError[%s] . Trying to reconnect" % (time_info, str(e)))
                utime.sleep(1)
                self.base_reconnect()

    def connect(self, clean_session=True):
        try:
            super().connect(clean_session)
        except Exception as e:
            if str(e) == "104":
                raise ValueError(
                    "MQTT Connect Error='{}' Server Actively RST Disconnected.Need Check addr&port".format(e))
            elif str(e) == "107" or str(e) == "4":
                raise ValueError(
                    "MQTT Connect Error='{}' Server Actively FIN Disconnected.Need Check Connection parameters".format(
                        e))
            elif str(e) == "110":
                raise ValueError("MQTT Connect Error='{}' Timeout.Need Check Network".format(e))
            raise ValueError("MQTT Connect error='{}' FAIL".format(e))
        if self.looptask_id is None:
            if self.keepalive > 0 and not self.pingFlag:
                # Pawn.zhou 2021/1/12 for JIRA STASR3601-2523 begin
                task_stacksize = _thread.stack_size()
                name,platform = uos.uname()[1].split("=",1)
                if platform == "EC600E" or platform == "EC800E":
                    _thread.stack_size(8 * 1024)
                elif platform == "FCM360W" or platform == "FCM362K":
                    _thread.stack_size(2 * 1024)
                else:
                    _thread.stack_size(16 * 1024)
                self.looptask_id = _thread.start_new_thread(self.__loop_forever, ())
                _thread.stack_size(task_stacksize)
                self.pingFlag = True
                # Pawn.zhou 2021/1/12 for JIRA STASR3601-2523 end
        return 0

    def __loop_forever(self):
        while True:
            if self.keepalive >= 5:
                keepalive = self.keepalive - 3
            try:
                if not self.timerFlag:
                    if self.mqttsendlock is not None and self.mqttsendlock.locked():
                        self.mqttsendlock.release()
                    self.looptask_id = None
                    break
                # time = utime.mktime(utime.localtime())
                time = utime.ticks_ms() // 1000

                if time - self.last_time > keepalive:
                    if self.mqttlock.locked():
                        utime.sleep(5)
                        continue
                    if self.PING == True:
                        mqtt_log.warning("[%s] Send ping timeout 2. Trying to reconnect" % time)
                        if not self.reconn:
                            if self.mqttsendlock is not None and self.mqttsendlock.locked():
                                self.mqttsendlock.release()
                            self.looptask_id = None
                            raise Exception("Send ping timeout.")
                        else:
                            if self.pingnum <= 0:
                                mqtt_log.warning("[%s] Send ping timeout 1. Trying to reconnect" % time)
                                self.sock.settimeout(3)
                                self.pingFlag = False
                                if self.mqttsendlock is not None and self.mqttsendlock.locked():
                                    self.mqttsendlock.release()
                                self.looptask_id = None
                                break
                            else:
                                mqtt_log.warning("[%s] Trying to resend ping(%d)" % (time, self.pingnum))
                                self.pingnum = self.pingnum - 1
                    while True:
                        if self.mqttsendlock is not None and self.mqttsendlock.locked():
                            utime.sleep_ms(10)
                            continue
                        self.mqttsendlock.acquire()
                        super().ping()
                        self.mqttsendlock.release()
                        utime.sleep(5)
                        break
                else:
                    utime.sleep(5)
                    continue
            except Exception as e:
                if self.mqttsendlock is not None and self.mqttsendlock.locked():
                    self.mqttsendlock.release()
                if not self.timerFlag:
                    self.looptask_id = None
                    break
                # Network normal, take the initiative to throw exception
                if not self.reconn:
                    if self.error_callback is not None:
                        self.error_callback(str(e))
                        self.pingFlag = False
                        self.looptask_id = None
                        break
                    else:
                        self.pingFlag = False
                        self.looptask_id = None
                        break
                # Whether to use the built-in reconnect mechanism
                time_info = self.logTime()
                if self.PING == True:
                    mqtt_log.warning("[%s] Send ping timeout 3. Trying to reconnect" % time_info)
                else:
                    mqtt_log.warning("[%s] Send ping, Network exception. Trying to reconnect" % time_info)
                utime.sleep(2)
                self.base_reconnect()
                break

    def logTime(self):
        log_time = utime.localtime()
        time_info = "%d-%d-%d %d:%d:%d" % (
            log_time[0], log_time[1], log_time[2], log_time[3], log_time[4], log_time[5],)
        return time_info
