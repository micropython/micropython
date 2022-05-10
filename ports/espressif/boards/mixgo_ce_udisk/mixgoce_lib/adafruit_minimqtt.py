# SPDX-FileCopyrightText: 2019-2021 Brent Rubell for Adafruit Industries
#
# SPDX-License-Identifier: MIT

# Original Work Copyright (c) 2016 Paul Sokolovsky, uMQTT
# Modified Work Copyright (c) 2019 Bradley Beach, esp32spi_mqtt
# Modified Work Copyright (c) 2012-2019 Roger Light and others, Paho MQTT Python

"""
`adafruit_minimqtt`
================================================================================

A minimal MQTT Library for CircuitPython.

* Author(s): Brent Rubell

Implementation Notes
--------------------

Adapted from https://github.com/micropython/micropython-lib/tree/master/umqtt.simple/umqtt

**Software and Dependencies:**

* Adafruit CircuitPython firmware for the supported boards:
  https://github.com/adafruit/circuitpython/releases

"""
import errno
import struct
import time
from random import randint
from micropython import const
from matcher import MQTTMatcher

__version__ = "0.0.0-auto.0"
__repo__ = "https://github.com/adafruit/Adafruit_CircuitPython_MiniMQTT.git"

# Client-specific variables
MQTT_MSG_MAX_SZ = const(268435455)
MQTT_MSG_SZ_LIM = const(10000000)
MQTT_TOPIC_LENGTH_LIMIT = const(65535)
MQTT_TCP_PORT = const(1883)
MQTT_TLS_PORT = const(8883)

# MQTT Commands
MQTT_PINGREQ = b"\xc0\0"
MQTT_PINGRESP = const(0xD0)
MQTT_SUB = b"\x82"
MQTT_UNSUB = b"\xA2"
MQTT_DISCONNECT = b"\xe0\0"

# Variable CONNECT header [MQTT 3.1.2]
MQTT_HDR_CONNECT = bytearray(b"\x04MQTT\x04\x02\0\0")


CONNACK_ERRORS = {
    const(0x01): "Connection Refused - Incorrect Protocol Version",
    const(0x02): "Connection Refused - ID Rejected",
    const(0x03): "Connection Refused - Server unavailable",
    const(0x04): "Connection Refused - Incorrect username/password",
    const(0x05): "Connection Refused - Unauthorized",
}

_default_sock = None  # pylint: disable=invalid-name
_fake_context = None  # pylint: disable=invalid-name


class MMQTTException(Exception):
    """MiniMQTT Exception class."""

    # pylint: disable=unnecessary-pass
    # pass


# Legacy ESP32SPI Socket API
def set_socket(sock, iface=None):
    """Legacy API for setting the socket and network interface.

    :param sock: socket object.
    :param iface: internet interface object

    """
    global _default_sock  # pylint: disable=invalid-name, global-statement
    global _fake_context  # pylint: disable=invalid-name, global-statement
    _default_sock = sock
    if iface:
        _default_sock.set_interface(iface)
        _fake_context = _FakeSSLContext(iface)


class _FakeSSLSocket:
    def __init__(self, socket, tls_mode):
        self._socket = socket
        self._mode = tls_mode
        self.settimeout = socket.settimeout
        self.send = socket.send
        self.recv = socket.recv
        self.close = socket.close

    def connect(self, address):
        """connect wrapper to add non-standard mode parameter"""
        try:
            return self._socket.connect(address, self._mode)
        except RuntimeError as error:
            raise OSError(errno.ENOMEM) from error


class _FakeSSLContext:
    def __init__(self, iface):
        self._iface = iface

    def wrap_socket(self, socket, server_hostname=None):
        """Return the same socket"""
        # pylint: disable=unused-argument
        return _FakeSSLSocket(socket, self._iface.TLS_MODE)


class MQTT:
    """MQTT Client for CircuitPython.

    :param str broker: MQTT Broker URL or IP Address.
    :param int port: Optional port definition, defaults to 8883.
    :param str username: Username for broker authentication.
    :param str password: Password for broker authentication.
    :param network_manager: NetworkManager object, such as WiFiManager from ESPSPI_WiFiManager.
    :param str client_id: Optional client identifier, defaults to a unique, generated string.
    :param bool is_ssl: Sets a secure or insecure connection with the broker.
    :param int keep_alive: KeepAlive interval between the broker and the MiniMQTT client.
    :param socket socket_pool: A pool of socket resources available for the given radio.
    :param ssl_context: SSL context for long-lived SSL connections.
    :param bool use_binary_mode: Messages are passed as bytearray instead of string to callbacks.

    """

    # pylint: disable=too-many-arguments,too-many-instance-attributes, not-callable, invalid-name, no-member
    def __init__(
        self,
        broker,
        port=None,
        username=None,
        password=None,
        client_id=None,
        is_ssl=True,
        keep_alive=60,
        socket_pool=None,
        ssl_context=None,
        use_binary_mode=False,
    ):

        self._socket_pool = socket_pool
        self._ssl_context = ssl_context
        self._sock = None
        self._backwards_compatible_sock = False
        self._use_binary_mode = use_binary_mode

        self.keep_alive = keep_alive
        self._user_data = None
        self._is_connected = False
        self._msg_size_lim = MQTT_MSG_SZ_LIM
        self._pid = 0
        self._timestamp = 0
        self.logger = None

        self.broker = broker
        self._username = username
        self._password = password
        if (
            self._password and len(password.encode("utf-8")) > MQTT_TOPIC_LENGTH_LIMIT
        ):  # [MQTT-3.1.3.5]
            raise MMQTTException("Password length is too large.")

        self.port = MQTT_TCP_PORT
        if is_ssl:
            self.port = MQTT_TLS_PORT
        if port:
            self.port = port

        # define client identifer
        if client_id:
            # user-defined client_id MAY allow client_id's > 23 bytes or
            # non-alpha-numeric characters
            self.client_id = client_id
        else:
            # assign a unique client_id
            self.client_id = "cpy{0}{1}".format(
                randint(0, int(time.monotonic() * 100) % 1000), randint(0, 99)
            )
            # generated client_id's enforce spec.'s length rules
            if len(self.client_id.encode("utf-8")) > 23 or not self.client_id:
                raise ValueError("MQTT Client ID must be between 1 and 23 bytes")

        # LWT
        self._lw_topic = None
        self._lw_qos = 0
        self._lw_topic = None
        self._lw_msg = None
        self._lw_retain = False

        # List of subscribed topics, used for tracking
        self._subscribed_topics = []
        self._on_message_filtered = MQTTMatcher()

        # Default topic callback methods
        self._on_message = None
        self.on_connect = None
        self.on_disconnect = None
        self.on_publish = None
        self.on_subscribe = None
        self.on_unsubscribe = None

    # pylint: disable=too-many-branches
    def _get_connect_socket(self, host, port, *, timeout=1):
        """Obtains a new socket and connects to a broker.

        :param str host: Desired broker hostname
        :param int port: Desired broker port
        :param int timeout: Desired socket timeout
        """
        # For reconnections - check if we're using a socket already and close it
        if self._sock:
            self._sock.close()
            self._sock = None

        # Legacy API - use the interface's socket instead of a passed socket pool
        if self._socket_pool is None:
            self._socket_pool = _default_sock

        # Legacy API - fake the ssl context
        if self._ssl_context is None:
            self._ssl_context = _fake_context

        if not isinstance(port, int):
            raise RuntimeError("Port must be an integer")

        if port == 8883 and not self._ssl_context:
            raise RuntimeError(
                "ssl_context must be set before using adafruit_mqtt for secure MQTT."
            )

        if self.logger is not None and port == MQTT_TLS_PORT:
            self.logger.info(
                "Establishing a SECURE SSL connection to {0}:{1}".format(host, port)
            )
        elif self.logger is not None:
            self.logger.info(
                "Establishing an INSECURE connection to {0}:{1}".format(host, port)
            )

        addr_info = self._socket_pool.getaddrinfo(
            host, port, 0, self._socket_pool.SOCK_STREAM
        )[0]

        sock = None
        retry_count = 0
        while retry_count < 5 and sock is None:
            retry_count += 1

            try:
                sock = self._socket_pool.socket(addr_info[0], addr_info[1])
            except OSError:
                continue

            connect_host = addr_info[-1][0]
            if port == 8883:
                sock = self._ssl_context.wrap_socket(sock, server_hostname=host)
                connect_host = host
            sock.settimeout(timeout)

            try:
                sock.connect((connect_host, port))
            except MemoryError:
                sock.close()
                sock = None
            except OSError:
                sock.close()
                sock = None

        if sock is None:
            raise RuntimeError("Repeated socket failures")

        self._backwards_compatible_sock = not hasattr(sock, "recv_into")
        return sock

    def __enter__(self):
        return self

    def __exit__(self, exception_type, exception_value, traceback):
        self.deinit()

    def _sock_exact_recv(self, bufsize):
        """Reads _exact_ number of bytes from the connected socket. Will only return
        string with the exact number of bytes requested.

        The semantics of native socket receive is that it returns no more than the
        specified number of bytes (i.e. max size). However, it makes no guarantees in
        terms of the minimum size of the buffer, which could be 1 byte. This is a
        wrapper for socket recv() to ensure that no less than the expected number of
        bytes is returned or trigger a timeout exception.

        :param int bufsize: number of bytes to receive
        """
        stamp = time.monotonic()
        rc = self._sock.recv(bufsize)
        to_read = bufsize - len(rc)
        assert to_read >= 0
        read_timeout = self.keep_alive
        while to_read > 0:
            recv = self._sock.recv(to_read)
            to_read -= len(recv)
            rc += recv
            if time.monotonic() - stamp > read_timeout:
                raise MMQTTException(
                    "Unable to receive {} bytes within {} seconds.".format(
                        to_read, read_timeout
                    )
                )
        return rc

    def deinit(self):
        """De-initializes the MQTT client and disconnects from the mqtt broker."""
        self.disconnect()

    @property
    def mqtt_msg(self):
        """Returns maximum MQTT payload and topic size."""
        return self._msg_size_lim, MQTT_TOPIC_LENGTH_LIMIT

    @mqtt_msg.setter
    def mqtt_msg(self, msg_size):
        """Sets the maximum MQTT message payload size.

        :param int msg_size: Maximum MQTT payload size.
        """
        if msg_size < MQTT_MSG_MAX_SZ:
            self._msg_size_lim = msg_size

    def will_set(self, topic=None, payload=None, qos=0, retain=False):
        """Sets the last will and testament properties. MUST be called before `connect()`.

        :param str topic: MQTT Broker topic.
        :param int|float|str payload: Last will disconnection payload.
            payloads of type int & float are converted to a string.
        :param int qos: Quality of Service level, defaults to
            zero. Conventional options are ``0`` (send at most once), ``1``
            (send at least once), or ``2`` (send exactly once).

            .. note:: Only options ``1`` or ``0`` are QoS levels supported by this library.
        :param bool retain: Specifies if the payload is to be retained when
            it is published.
        """
        if self.logger is not None:
            self.logger.debug("Setting last will properties")
        self._valid_qos(qos)
        if self._is_connected:
            raise MMQTTException("Last Will should only be called before connect().")
        if payload is None:
            payload = ""
        if isinstance(payload, (int, float, str)):
            payload = str(payload).encode()
        else:
            raise MMQTTException("Invalid message data type.")
        self._lw_qos = qos
        self._lw_topic = topic
        self._lw_msg = payload
        self._lw_retain = retain

    def add_topic_callback(self, mqtt_topic, callback_method):
        """Registers a callback_method for a specific MQTT topic.

        :param str mqtt_topic: MQTT topic identifier.
        :param function callback_method: The callback method.
        """
        if mqtt_topic is None or callback_method is None:
            raise ValueError("MQTT topic and callback method must both be defined.")
        self._on_message_filtered[mqtt_topic] = callback_method

    def remove_topic_callback(self, mqtt_topic):
        """Removes a registered callback method.

        :param str mqtt_topic: MQTT topic identifier string.
        """
        if mqtt_topic is None:
            raise ValueError("MQTT Topic must be defined.")
        try:
            del self._on_message_filtered[mqtt_topic]
        except KeyError:
            raise KeyError(
                "MQTT topic callback not added with add_topic_callback."
            ) from None

    @property
    def on_message(self):
        """Called when a new message has been received on a subscribed topic.

        Expected method signature is ``on_message(client, topic, message)``
        """
        return self._on_message

    @on_message.setter
    def on_message(self, method):
        self._on_message = method

    def _handle_on_message(self, client, topic, message):
        matched = False
        if topic is not None:
            for callback in self._on_message_filtered.iter_match(topic):
                callback(client, topic, message)  # on_msg with callback
                matched = True

        if not matched and self.on_message:  # regular on_message
            self.on_message(client, topic, message)

    def username_pw_set(self, username, password=None):
        """Set client's username and an optional password.

        :param str username: Username to use with your MQTT broker.
        :param str password: Password to use with your MQTT broker.

        """
        if self._is_connected:
            raise MMQTTException("This method must be called before connect().")
        self._username = username
        if password is not None:
            self._password = password

    # pylint: disable=too-many-branches, too-many-statements, too-many-locals
    def connect(self, clean_session=True, host=None, port=None, keep_alive=None):
        """Initiates connection with the MQTT Broker.

        :param bool clean_session: Establishes a persistent session.
        :param str host: Hostname or IP address of the remote broker.
        :param int port: Network port of the remote broker.
        :param int keep_alive: Maximum period allowed for communication, in seconds.

        """
        if host:
            self.broker = host
        if port:
            self.port = port
        if keep_alive:
            self.keep_alive = keep_alive

        if self.logger is not None:
            self.logger.debug("Attempting to establish MQTT connection...")

        # Get a new socket
        self._sock = self._get_connect_socket(self.broker, self.port)

        # Fixed Header
        fixed_header = bytearray([0x10])

        # NOTE: Variable header is
        # MQTT_HDR_CONNECT = bytearray(b"\x04MQTT\x04\x02\0\0")
        # because final 4 bytes are 4, 2, 0, 0
        var_header = MQTT_HDR_CONNECT
        var_header[6] = clean_session << 1

        # Set up variable header and remaining_length
        remaining_length = 12 + len(self.client_id.encode("utf-8"))
        if self._username:
            remaining_length += (
                2
                + len(self._username.encode("utf-8"))
                + 2
                + len(self._password.encode("utf-8"))
            )
            var_header[6] |= 0xC0
        if self.keep_alive:
            assert self.keep_alive < MQTT_TOPIC_LENGTH_LIMIT
            var_header[7] |= self.keep_alive >> 8
            var_header[8] |= self.keep_alive & 0x00FF
        if self._lw_topic:
            remaining_length += (
                2 + len(self._lw_topic.encode("utf-8")) + 2 + len(self._lw_msg)
            )
            var_header[6] |= 0x4 | (self._lw_qos & 0x1) << 3 | (self._lw_qos & 0x2) << 3
            var_header[6] |= self._lw_retain << 5

        # Remaining length calculation
        large_rel_length = False
        if remaining_length > 0x7F:
            large_rel_length = True
            # Calculate Remaining Length [2.2.3]
            while remaining_length > 0:
                encoded_byte = remaining_length % 0x80
                remaining_length = remaining_length // 0x80
                # if there is more data to encode, set the top bit of the byte
                if remaining_length > 0:
                    encoded_byte |= 0x80
                fixed_header.append(encoded_byte)
        if large_rel_length:
            fixed_header.append(0x00)
        else:
            fixed_header.append(remaining_length)
            fixed_header.append(0x00)

        if self.logger is not None:
            self.logger.debug("Sending CONNECT to broker...")
            self.logger.debug(
                "Fixed Header: %s\nVariable Header: %s", fixed_header, var_header
            )
        self._sock.send(fixed_header)
        self._sock.send(var_header)
        # [MQTT-3.1.3-4]
        self._send_str(self.client_id)
        if self._lw_topic:
            # [MQTT-3.1.3-11]
            self._send_str(self._lw_topic)
            self._send_str(self._lw_msg)
        if self._username is None:
            self._username = None
        else:
            self._send_str(self._username)
            self._send_str(self._password)
        if self.logger is not None:
            self.logger.debug("Receiving CONNACK packet from broker")
        while True:
            op = self._wait_for_msg()
            if op == 32:
                rc = self._sock_exact_recv(3)
                assert rc[0] == 0x02
                if rc[2] != 0x00:
                    raise MMQTTException(CONNACK_ERRORS[rc[2]])
                self._is_connected = True
                result = rc[0] & 1
                if self.on_connect is not None:
                    self.on_connect(self, self._user_data, result, rc[2])
                return result

    def disconnect(self):
        """Disconnects the MiniMQTT client from the MQTT broker."""
        self.is_connected()
        if self.logger is not None:
            self.logger.debug("Sending DISCONNECT packet to broker")
        try:
            self._sock.send(MQTT_DISCONNECT)
        except RuntimeError as e:
            if self.logger is not None:
                self.logger.warning("Unable to send DISCONNECT packet: {}".format(e))
        if self.logger is not None:
            self.logger.debug("Closing socket")
        self._sock.close()
        self._is_connected = False
        self._subscribed_topics = []
        if self.on_disconnect is not None:
            self.on_disconnect(self, self._user_data, 0)

    def ping(self):
        """Pings the MQTT Broker to confirm if the broker is alive or if
        there is an active network connection.
        Returns response codes of any messages received while waiting for PINGRESP.
        """
        self.is_connected()
        if self.logger is not None:
            self.logger.debug("Sending PINGREQ")
        self._sock.send(MQTT_PINGREQ)
        ping_timeout = self.keep_alive
        stamp = time.monotonic()
        rc, rcs = None, []
        while rc != MQTT_PINGRESP:
            rc = self._wait_for_msg()
            if rc:
                rcs.append(rc)
            if time.monotonic() - stamp > ping_timeout:
                raise MMQTTException("PINGRESP not returned from broker.")
        return rcs

    # pylint: disable=too-many-branches, too-many-statements
    def publish(self, topic, msg, retain=False, qos=0):
        """Publishes a message to a topic provided.

        :param str topic: Unique topic identifier.
        :param str|int|float|bytes msg: Data to send to the broker.
        :param bool retain: Whether the message is saved by the broker.
        :param int qos: Quality of Service level for the message, defaults to zero.

        """
        self.is_connected()
        self._valid_topic(topic)
        if "+" in topic or "#" in topic:
            raise MMQTTException("Publish topic can not contain wildcards.")
        # check msg/qos kwargs
        if msg is None:
            raise MMQTTException("Message can not be None.")
        if isinstance(msg, (int, float)):
            msg = str(msg).encode("ascii")
        elif isinstance(msg, str):
            msg = str(msg).encode("utf-8")
        elif isinstance(msg, bytes):
            pass
        else:
            raise MMQTTException("Invalid message data type.")
        if len(msg) > MQTT_MSG_MAX_SZ:
            raise MMQTTException("Message size larger than %d bytes." % MQTT_MSG_MAX_SZ)
        assert (
            0 <= qos <= 1
        ), "Quality of Service Level 2 is unsupported by this library."

        # fixed header. [3.3.1.2], [3.3.1.3]
        pub_hdr_fixed = bytearray([0x30 | retain | qos << 1])

        # variable header = 2-byte Topic length (big endian)
        pub_hdr_var = bytearray(struct.pack(">H", len(topic.encode("utf-8"))))
        pub_hdr_var.extend(topic.encode("utf-8"))  # Topic name

        remaining_length = 2 + len(msg) + len(topic.encode("utf-8"))
        if qos > 0:
            # packet identifier where QoS level is 1 or 2. [3.3.2.2]
            remaining_length += 2
            self._pid = self._pid + 1 if self._pid < 0xFFFF else 1
            pub_hdr_var.append(self._pid >> 8)
            pub_hdr_var.append(self._pid & 0xFF)

        # Calculate remaining length [2.2.3]
        if remaining_length > 0x7F:
            while remaining_length > 0:
                encoded_byte = remaining_length % 0x80
                remaining_length = remaining_length // 0x80
                if remaining_length > 0:
                    encoded_byte |= 0x80
                pub_hdr_fixed.append(encoded_byte)
        else:
            pub_hdr_fixed.append(remaining_length)

        if self.logger is not None:
            self.logger.debug(
                "Sending PUBLISH\nTopic: %s\nMsg: %s\
                                \nQoS: %d\nRetain? %r",
                topic,
                msg,
                qos,
                retain,
            )
        self._sock.send(pub_hdr_fixed)
        self._sock.send(pub_hdr_var)
        self._sock.send(msg)
        if qos == 0 and self.on_publish is not None:
            self.on_publish(self, self._user_data, topic, self._pid)
        if qos == 1:
            while True:
                op = self._wait_for_msg()
                if op == 0x40:
                    sz = self._sock_exact_recv(1)
                    assert sz == b"\x02"
                    rcv_pid = self._sock_exact_recv(2)
                    rcv_pid = rcv_pid[0] << 0x08 | rcv_pid[1]
                    if self._pid == rcv_pid:
                        if self.on_publish is not None:
                            self.on_publish(self, self._user_data, topic, rcv_pid)
                        return

    def subscribe(self, topic, qos=0):
        """Subscribes to a topic on the MQTT Broker.
        This method can subscribe to one topics or multiple topics.

        :param str|tuple|list topic: Unique MQTT topic identifier string. If
                                     this is a `tuple`, then the tuple should
                                     contain topic identifier string and qos
                                     level integer. If this is a `list`, then
                                     each list element should be a tuple containing
                                     a topic identifier string and qos level integer.
        :param int qos: Quality of Service level for the topic, defaults to
                        zero. Conventional options are ``0`` (send at most once), ``1``
                        (send at least once), or ``2`` (send exactly once).

        """
        self.is_connected()
        topics = None
        if isinstance(topic, tuple):
            topic, qos = topic
            self._valid_topic(topic)
            self._valid_qos(qos)
        if isinstance(topic, str):
            self._valid_topic(topic)
            self._valid_qos(qos)
            topics = [(topic, qos)]
        if isinstance(topic, list):
            topics = []
            for t, q in topic:
                self._valid_qos(q)
                self._valid_topic(t)
                topics.append((t, q))
        # Assemble packet
        packet_length = 2 + (2 * len(topics)) + (1 * len(topics))
        packet_length += sum(len(topic.encode("utf-8")) for topic, qos in topics)
        packet_length_byte = packet_length.to_bytes(1, "big")
        self._pid = self._pid + 1 if self._pid < 0xFFFF else 1
        packet_id_bytes = self._pid.to_bytes(2, "big")
        # Packet with variable and fixed headers
        packet = MQTT_SUB + packet_length_byte + packet_id_bytes
        # attaching topic and QOS level to the packet
        for t, q in topics:
            topic_size = len(t.encode("utf-8")).to_bytes(2, "big")
            qos_byte = q.to_bytes(1, "big")
            packet += topic_size + t.encode() + qos_byte
        if self.logger is not None:
            for t, q in topics:
                self.logger.debug("SUBSCRIBING to topic %s with QoS %d", t, q)
        self._sock.send(packet)
        while True:
            op = self._wait_for_msg()
            if op == 0x90:
                rc = self._sock_exact_recv(4)
                assert rc[1] == packet[2] and rc[2] == packet[3]
                if rc[3] == 0x80:
                    raise MMQTTException("SUBACK Failure!")
                for t, q in topics:
                    if self.on_subscribe is not None:
                        self.on_subscribe(self, self._user_data, t, q)
                    self._subscribed_topics.append(t)
                return

    def unsubscribe(self, topic):
        """Unsubscribes from a MQTT topic.

        :param str|list topic: Unique MQTT topic identifier string or list.

        """
        topics = None
        if isinstance(topic, str):
            self._valid_topic(topic)
            topics = [(topic)]
        if isinstance(topic, list):
            topics = []
            for t in topic:
                self._valid_topic(t)
                topics.append((t))
        for t in topics:
            if t not in self._subscribed_topics:
                raise MMQTTException(
                    "Topic must be subscribed to before attempting unsubscribe."
                )
        # Assemble packet
        packet_length = 2 + (2 * len(topics))
        packet_length += sum(len(topic.encode("utf-8")) for topic in topics)
        packet_length_byte = packet_length.to_bytes(1, "big")
        self._pid = self._pid + 1 if self._pid < 0xFFFF else 1
        packet_id_bytes = self._pid.to_bytes(2, "big")
        packet = MQTT_UNSUB + packet_length_byte + packet_id_bytes
        for t in topics:
            topic_size = len(t.encode("utf-8")).to_bytes(2, "big")
            packet += topic_size + t.encode()
        if self.logger is not None:
            for t in topics:
                self.logger.debug("UNSUBSCRIBING from topic %s", t)
        self._sock.send(packet)
        if self.logger is not None:
            self.logger.debug("Waiting for UNSUBACK...")
        while True:
            op = self._wait_for_msg()
            if op == 176:
                rc = self._sock_exact_recv(3)
                assert rc[0] == 0x02
                # [MQTT-3.32]
                assert rc[1] == packet_id_bytes[0] and rc[2] == packet_id_bytes[1]
                for t in topics:
                    if self.on_unsubscribe is not None:
                        self.on_unsubscribe(self, self._user_data, t, self._pid)
                    self._subscribed_topics.remove(t)
                return

    def reconnect(self, resub_topics=True):
        """Attempts to reconnect to the MQTT broker.

        :param bool resub_topics: Resubscribe to previously subscribed topics.

        """
        if self.logger is not None:
            self.logger.debug("Attempting to reconnect with MQTT broker")
        self.connect()
        if self.logger is not None:
            self.logger.debug("Reconnected with broker")
        if resub_topics:
            if self.logger is not None:
                self.logger.debug(
                    "Attempting to resubscribe to previously subscribed topics."
                )
            subscribed_topics = self._subscribed_topics.copy()
            self._subscribed_topics = []
            while subscribed_topics:
                feed = subscribed_topics.pop()
                self.subscribe(feed)

    def loop(self, timeout=1):
        """Non-blocking message loop. Use this method to
        check incoming subscription messages.
        Returns response codes of any messages received.

        :param int timeout: Socket timeout, in seconds.

        """
        if self._timestamp == 0:
            self._timestamp = time.monotonic()
        current_time = time.monotonic()
        if current_time - self._timestamp >= self.keep_alive:
            self._timestamp = 0
            # Handle KeepAlive by expecting a PINGREQ/PINGRESP from the server
            if self.logger is not None:
                self.logger.debug(
                    "KeepAlive period elapsed - requesting a PINGRESP from the server..."
                )
            rcs = self.ping()
            return rcs
        self._sock.settimeout(timeout)
        rc = self._wait_for_msg()
        return [rc] if rc else None

    def _wait_for_msg(self, timeout=0.1):
        """Reads and processes network events."""
        # CPython socket module contains a timeout attribute
        if hasattr(self._socket_pool, "timeout"):
            try:
                res = self._sock_exact_recv(1)
            except self._socket_pool.timeout:
                return None
        else:  # socketpool, esp32spi
            try:
                res = self._sock_exact_recv(1)
            except OSError as error:
                if error.errno == errno.ETIMEDOUT:
                    # raised by a socket timeout if 0 bytes were present
                    return None
                raise MMQTTException from error

        # Block while we parse the rest of the response
        self._sock.settimeout(timeout)
        if res in [None, b""]:
            # If we get here, it means that there is nothing to be received
            return None
        if res[0] == MQTT_PINGRESP:
            if self.logger is not None:
                self.logger.debug("Got PINGRESP")
            sz = self._sock_exact_recv(1)[0]
            if sz != 0x00:
                raise MMQTTException(
                    "Unexpected PINGRESP returned from broker: {}.".format(sz)
                )
            return MQTT_PINGRESP
        if res[0] & 0xF0 != 0x30:
            return res[0]
        sz = self._recv_len()
        # topic length MSB & LSB
        topic_len = self._sock_exact_recv(2)
        topic_len = (topic_len[0] << 8) | topic_len[1]
        topic = self._sock_exact_recv(topic_len)
        topic = str(topic, "utf-8")
        sz -= topic_len + 2
        pid = 0
        if res[0] & 0x06:
            pid = self._sock_exact_recv(2)
            pid = pid[0] << 0x08 | pid[1]
            sz -= 0x02
        # read message contents
        raw_msg = self._sock_exact_recv(sz)
        msg = raw_msg if self._use_binary_mode else str(raw_msg, "utf-8")
        if self.logger is not None:
            self.logger.debug(
                "Receiving SUBSCRIBE \nTopic: %s\nMsg: %s\n", topic, raw_msg
            )
        self._handle_on_message(self, topic, msg)
        if res[0] & 0x06 == 0x02:
            pkt = bytearray(b"\x40\x02\0\0")
            struct.pack_into("!H", pkt, 2, pid)
            self._sock.send(pkt)
        elif res[0] & 6 == 4:
            assert 0
        return res[0]

    def _recv_len(self):
        """Unpack MQTT message length."""
        n = 0
        sh = 0
        b = bytearray(1)
        while True:
            b = self._sock_exact_recv(1)[0]
            n |= (b & 0x7F) << sh
            if not b & 0x80:
                return n
            sh += 7

    def _recv_into(self, buf, size=0):
        """Backwards-compatible _recv_into implementation."""
        if self._backwards_compatible_sock:
            size = len(buf) if size == 0 else size
            b = self._sock.recv(size)
            read_size = len(b)
            buf[:read_size] = b
            return read_size
        return self._sock.recv_into(buf, size)

    def _sock_exact_recv(self, bufsize):
        """Reads _exact_ number of bytes from the connected socket. Will only return
        string with the exact number of bytes requested.

        The semantics of native socket receive is that it returns no more than the
        specified number of bytes (i.e. max size). However, it makes no guarantees in
        terms of the minimum size of the buffer, which could be 1 byte. This is a
        wrapper for socket recv() to ensure that no less than the expected number of
        bytes is returned or trigger a timeout exception.

        :param int bufsize: number of bytes to receive

        """
        if not self._backwards_compatible_sock:
            # CPython/Socketpool Impl.
            rc = bytearray(bufsize)
            self._sock.recv_into(rc, bufsize)
        else:  # ESP32SPI Impl.
            stamp = time.monotonic()
            read_timeout = self.keep_alive
            # This will timeout with socket timeout (not keepalive timeout)
            rc = self._sock.recv(bufsize)
            if not rc:
                if self.logger is not None:
                    self.logger.debug("_sock_exact_recv timeout")
                # If no bytes waiting, raise same exception as socketpool
                raise OSError(errno.ETIMEDOUT)
            # If any bytes waiting, try to read them all,
            # or raise exception if wait longer than read_timeout
            to_read = bufsize - len(rc)
            assert to_read >= 0
            read_timeout = self.keep_alive
            while to_read > 0:
                recv = self._sock.recv(to_read)
                to_read -= len(recv)
                rc += recv
                if time.monotonic() - stamp > read_timeout:
                    raise MMQTTException(
                        "Unable to receive {} bytes within {} seconds.".format(
                            to_read, read_timeout
                        )
                    )
        return rc

    def _send_str(self, string):
        """Encodes a string and sends it to a socket.

        :param str string: String to write to the socket.

        """
        if isinstance(string, str):
            self._sock.send(struct.pack("!H", len(string.encode("utf-8"))))
            self._sock.send(str.encode(string, "utf-8"))
        else:
            self._sock.send(struct.pack("!H", len(string)))
            self._sock.send(string)

    @staticmethod
    def _valid_topic(topic):
        """Validates if topic provided is proper MQTT topic format.

        :param str topic: Topic identifier

        """
        if topic is None:
            raise MMQTTException("Topic may not be NoneType")
        # [MQTT-4.7.3-1]
        if not topic:
            raise MMQTTException("Topic may not be empty.")
        # [MQTT-4.7.3-3]
        if len(topic.encode("utf-8")) > MQTT_TOPIC_LENGTH_LIMIT:
            raise MMQTTException("Topic length is too large.")

    @staticmethod
    def _valid_qos(qos_level):
        """Validates if the QoS level is supported by this library

        :param int qos_level: Desired QoS level.

        """
        if isinstance(qos_level, int):
            if qos_level < 0 or qos_level > 2:
                raise MMQTTException("QoS must be between 1 and 2.")
        else:
            raise MMQTTException("QoS must be an integer.")

    def is_connected(self):
        """Returns MQTT client session status as True if connected, raises
        a `MMQTTException` if `False`.
        """
        if self._sock is None or self._is_connected is False:
            raise MMQTTException("MiniMQTT is not connected.")
        return self._is_connected

    # Logging
    def enable_logger(self, logger, log_level=20):
        """Enables library logging provided a logger object.

        :param logger: A python logger pacakge.
        :param log_level: Numeric value of a logging level, defaults to INFO.

        """
        self.logger = logger.getLogger("log")
        self.logger.setLevel(log_level)

    def disable_logger(self):
        """Disables logging."""
        if not self.logger:
            raise MMQTTException("Can not disable logger, no logger found.")
        self.logger = None
