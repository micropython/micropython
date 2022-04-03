"""
File name: esp8266.py
Michal Pindiak, hobbyastro}at[gmail(com
Inspired by https://github.com/Circuit-Digest/Raspberry_Pi_Pico_Tutorial/tree/main/T7_Interfacing_ESP8266-01_WiFi
"""

from machine import UART, Pin
import time
"""
Enables controlling the ESP8266 board via Python and in the background AT commands via UART.
"""

ESP8266_OK_STATUS = "OK\r\n"
ESP8266_ERROR_STATUS = "ERROR\r\n"
ESP8266_FAIL_STATUS = "FAIL\r\n"
ESP8266_WIFI_CONNECTED = "WIFI CONNECTED\r\n"
ESP8266_WIFI_GOT_IP_CONNECTED = "WIFI GOT IP\r\n"
ESP8266_WIFI_DISCONNECTED = "WIFI DISCONNECTED\r\n"
ESP8266_WIFI_AP_NOT_PRESENT = "WIFI AP NOT FOUND\r\n"
ESP8266_WIFI_AP_WRONG_PWD = "WIFI AP WRONG PASSWORD\r\n"
ESP8266_BUSY_STATUS = "DEVICE BUSY\r\n"
UART_Tx_BUFFER_LENGTH = 1024
UART_Rx_BUFFER_LENGTH = 2048  # increase in case the http responses you are getting are truncated
received_data = None


class ESP8266:
    """
    Enables controlling the ESP8266 board via Python and in the background AT commands via UART.
    """

    def __init__(self, uart_port=0, uart_baud_rate=115200, uart_tx_pin=0, uart_rx_pin=1):
        """
        Initialisation.

        Parameters:
            uart_port (int)
            uart_baud_rate (int)
            uart_tx_pin (int)
            uart_rx_pin (int)
        """

        self.uart_port = uart_port
        self.uart_baud_rate = uart_baud_rate
        self.uart_tx_pin = uart_tx_pin
        self.uart_rx_pin = uart_rx_pin
        self.uart_object = UART(uart_port, baudrate=self.uart_baud_rate, tx=Pin(self.uart_tx_pin), rx=Pin(self.uart_rx_pin), txbuf=UART_Tx_BUFFER_LENGTH, rxbuf=UART_Rx_BUFFER_LENGTH)

    def write_to_uart(self, at_command, delay=0.5):
        """
        For communicating with ESP8266 board via UART bus.
        """

        self.received_data = str()
        self.sent_data = at_command
        self.uart_object.write(self.sent_data)
        self.received_data = bytes()
        time.sleep(delay)
        while True:
            if self.uart_object.any() > 0:
                break
        while self.uart_object.any() > 0:
            self.received_data += self.uart_object.read(UART_Rx_BUFFER_LENGTH)
        if ESP8266_OK_STATUS in self.received_data:
            return self.received_data
        elif ESP8266_ERROR_STATUS in self.received_data:
            return self.received_data
        elif ESP8266_FAIL_STATUS in self.received_data:
            return self.received_data
        elif ESP8266_BUSY_STATUS in self.received_data:
            return "ESP BUSY\r\n"
        else:
            return None

    def initialise_ESP8266(self):
        """
        Sending the "AT" command to ESP8266.

        Returns:
            "Startup ok" on success.
            "Startup failed" otherwise.
        """

        received_data = self.write_to_uart("AT\r\n")
        if(received_data is not None):
            if ESP8266_OK_STATUS in received_data:
                return "Startup ok"
            else:
                return "Startup failed"
        else:
            "Startup failed"

    def reset_ESP8266(self):
        """
        Sending the "AT+RST" command to ESP8266. Resets the ESP8266. For restoring the factory settings, use restore_ESP8266().

        Returns:
            "Reset ok" if reset of ESP8266 was successfull.
            "Reset failed" otherwise.
        """

        received_data = self.write_to_uart("AT+RST\r\n")
        if(received_data is not None):
            if ESP8266_OK_STATUS in received_data:
                time.sleep(0.5)
                return "Reset ok"
            else:
                return "Reset failed"
        else:
            "Reset failed"

    def echo_commands(self, enable=False):
        """
        A function to enable/disable AT command echo.

        Returns:
            "Echo on" / "Echo off" if the command was successful.
            "Setting echo failed" if echo off/on command failed.
        """

        if enable is False:
            received_data = self.write_to_uart("ATE0\r\n")
            if(received_data is not None):
                if ESP8266_OK_STATUS in received_data:
                    return "Echo off"
                else:
                    return "Setting echo failed"
            else:
                return "Setting echo failed"
        else:
            received_data = self.write_to_uart("ATE1\r\n")
            if(received_data is not None):
                if ESP8266_OK_STATUS in received_data:
                    return "Echo on"
                else:
                    return "Setting echo failed"
            else:
                return "Setting echo failed"

    def view_version_info(self):
        """
        Sending the "AT+GMR" command to ESP8266.

        Returns:
            Version details on success.
            None otherwise.
        """

        received_data = self.write_to_uart("AT+GMR\r\n")
        if(received_data is not None):
            if ESP8266_OK_STATUS in received_data:
                received_data = str(received_data).partition(r"OK")[0]
                received_data = received_data.split(r"\r\n")
                received_data[0] = received_data[0].replace("b'", "")
                received_data = str(received_data[0]+"\r\n"+received_data[1]+"\r\n"+received_data[2])
                return received_data
            else:
                return None
        else:
            return None

    def get_status(self):
        """
        Command "AT+CIPSTATUS" to get ESP8266 connection status.

        Returns:
            2: ESP8266 connected to an AP and got an IP address.
            3: ESP8266 opened a TCP or UDP connection.
            4: TCP or UDP connection disconnected.
            5: ESP8266 not connected to any AP.
            "Getting the connection status failed" on error.
        """

        received_data = self.write_to_uart("AT+CIPSTATUS\r\n")
        if(received_data is not None):
            if ESP8266_OK_STATUS in received_data:
                received_data = str(received_data).partition(r"OK")[0]
                received_data = received_data.split(r"\r\n")
                received_data[0] = received_data[0].replace("b'", "")
                received_data = str(received_data[0]+"\r\n"+received_data[1]+"\r\n"+received_data[2])
                if "2" in received_data:
                    received_status = "2 = ESP8266 is connected to an AP and got an IP address"
                if "3" in received_data:
                    received_status = "3 = ESP8266 opened a TCP or UDP connection"
                if "4" in received_data:
                    received_status = "4 = TCP or UDP connection was closed"
                if "5" in received_data:
                    received_status = "5 = ESP8266 is not connected to any AP"
                return received_status
            else:
                return "Getting the connection status failed"
        else:
            return "Getting the connection status failed"

    def get_ip(self):
        """
        Command "AT+CIFSR" to get IP addresses of ESP8266 board.

        Returns:
            +CIFSR:APIP: SoftAP IP address.
            +CIFSR:APMAC: SoftAP MAC address.
            +CIFSR:STAIP: Station IP address.
            +CIFSR:STAMAC: Station MAC address.
            "Error getting IP addresses" in case of error.
        """

        received_data = self.write_to_uart("AT+CIFSR\r\n")
        if(received_data is not None):
            if ESP8266_OK_STATUS in received_data:
                received_data = str(received_data).partition(r"OK")[0]
                received_data = received_data.split(r"\r\n")
                received_data[0] = received_data[0].replace("b'", "")
                received_data = str(received_data[0]+"\r\n"+received_data[1]+"\r\n"+received_data[2])
                return received_data
            else:
                return "Error getting IP addresses"
        else:
            return "Error getting IP addresses"

    def restore_ESP8266(self):
        """
        A function to restore the ESP8266 board into factory settings via "AT+RESTORE" command. For resetting the board, use reset_ESP8266().

        Returns:
            "Restore ok" if ESP8266 restore was successful.
            "Restore failed" otherwise.
        """

        received_data = self.write_to_uart("AT+RESTORE\r\n")
        if(received_data is not None):
            if ESP8266_OK_STATUS in received_data:
                return "Restore ok"
            else:
                return "Restore failed"
        else:
            return "Restore failed"

    def get_current_mode(self):
        """
        Get the current ESP8266 mode via "AT+CWMODE_CUR?".

        Returns:
            "STA" if the current mode = Station.
            "SoftAP" if the current mode = SoftAP.
            "SoftAP+STA" if the current mode = Station & SoftAP.
            "Getting the current mode failed" if failed to detect the current mode.
        """

        received_data = self.write_to_uart("AT+CWMODE_CUR?\r\n")
        if(received_data is not None):
            if "1" in received_data:
                return "1 STA"
            elif "2" in received_data:
                return "2 SoftAP"
            elif "3" in received_data:
                return "3 SoftAP+STA"
            else:
                return "Getting the current mode failed"
        else:
            return "Getting the current mode failed"

    def set_current_mode(self, mode=3):
        """
        A function to set the current ESP8266 WiFi mode via "AT+CWMODE_CUR=" command.

        Parameters:
            mode (int): (1 = STA, 2: = SoftAP, 3 = SoftAP+STA).

        Returns:
            True after successfully setting the current mode.
            False otherwise.
        """

        transmission_data = "AT+CWMODE_CUR="+str(mode)+"\r\n"
        received_data = self.write_to_uart(transmission_data)
        if(received_data is not None):
            if ESP8266_OK_STATUS in received_data:
                return True
            else:
                return False
        else:
            return False

    def get_default_mode(self):
        """
        Get the default ESP8266 mode via "AT+CWMODE_DEF?".

        Returns:
            "STA" if the current mode = Station.
            "SoftAP" if the default mode = SoftAP.
            "SoftAP+STA" if the default mode = Station & SoftAP.
            "Getting the default mode failed" if failed to detect the default mode.
        """

        received_data = self.write_to_uart("AT+CWMODE_DEF?\r\n")
        if(received_data is not None):
            if "1" in received_data:
                return "1 STA"
            elif "2" in received_data:
                return "2 SoftAP"
            elif "3" in received_data:
                return "3 SoftAP+STA"
            else:
                return "Getting the default mode failed"
        else:
            return "Getting the default mode failed"

    def set_default_mode(self, mode=3):
        """
        A function to set the default ESP8266 WiFi mode via "AT+CWMODE_DEF=" command.

        Parameters:
            mode (int): (1 = STA, 2: = SoftAP, 3 = SoftAP+STA).

        Returns:
            True after successfully setting the default mode.
            False otherwise.
        """

        transmission_data = "AT+CWMODE_DEF="+str(mode)+"\r\n"
        received_data = self.write_to_uart(transmission_data)
        if(received_data is not None):
            if ESP8266_OK_STATUS in received_data:
                return True
            else:
                return False
        else:
            return False

    def list_available_access_points(self):
        """
        A function to list available WiFi access points via "AT+CWLAP" command.

        Returns:
            List of available access points or None.
        """

        received_data = str(self.write_to_uart("AT+CWLAP\r\n", delay=10))
        if(received_data is not None):
            received_data = received_data.replace("+CWLAP:", "")
            received_data = received_data.replace(r"\r\n\r\nOK\r\n", "")
            received_data = received_data.replace(r"\r\n", "@")
            received_data = received_data.replace("b'(", "(").replace("'", "")
            received_data = received_data.split("@")
            received_data = list(received_data)
            access_points = list()

            for items in received_data:
                data = str(items).replace("(", "").replace(")", "").split(",")
                data = tuple(data)
                access_points.append(data)
            return access_points
        else:
            return None

    def connect_wifi(self, ssid, pwd):
        """
        Used to connect ESP8266 board to a WiFi AP via "AT+CWJAP_CUR=" command.

        Parameters:
            ssid (str)
            pwd (str)

        Returns:
            WIFI CONNECTED on success.
            WIFI AP WRONG PASSWORD for wrong password.
            WIFI AP NOT FOUND when the specified SSID is not found.
            WIFI DISCONNECTED when the connection is not successful.
        """

        transmission_data = "AT+CWJAP_CUR="+'"'+ssid+'"'+','+'"'+pwd+'"'+"\r\n"
        received_data = self.write_to_uart(transmission_data, delay=15)
        if(received_data is not None):
            if "+CWJAP" in received_data:
                if "1" in received_data:
                    return ESP8266_WIFI_DISCONNECTED
                elif "2" in received_data:
                    return ESP8266_WIFI_AP_WRONG_PWD
                elif "3" in received_data:
                    return ESP8266_WIFI_AP_NOT_PRESENT
                elif "4" in received_data:
                    return ESP8266_WIFI_DISCONNECTED
                else:
                    return None
            elif ESP8266_WIFI_CONNECTED in received_data:
                if ESP8266_WIFI_GOT_IP_CONNECTED in received_data:
                    return ESP8266_WIFI_CONNECTED
                else:
                    return ESP8266_WIFI_DISCONNECTED
            else:
                return ESP8266_WIFI_DISCONNECTED
        else:
            return ESP8266_WIFI_DISCONNECTED

    def disconnect_wifi(self):
        """
        Used to disconnect from a connected WiFi AP via "AT+CWQAP" command.

        Returns:
            "OK" on successful disconnect.
            "Failed" otherwise.
        """

        received_data = self.write_to_uart("AT+CWQAP\r\n")
        if(received_data is not None):
            if ESP8266_OK_STATUS in received_data:
                return "OK"
            else:
                return "Failed"
        else:
            return "Failed"

    def parseHTTP(self, data):
        """
        A function to process the response for http requests (get, post, put,...).

        Returns:
            On success: dict {"code":"", "headers":"", "body":""}
            On error: dict {"code":"0", "headers":"0", "body":"0"}
        """

        failsafe = {"code": "0", "headers": "0", "body": "0"}
        if(data is not None):
            data = data.decode("utf-8").split("\r\n")
            raw = []
            for _ in data:
                if _ != "":
                    raw.append(_)
            try:
                response_code = (raw[2].split(" "))[1]
            except IndexError:
                response_code = "0"
            try:
                header = raw[3:-1]
            except IndexError:
                header = "0"
            try:
                body = raw[-1]
                if body[-6:] == "CLOSED":
                    body = body[:-6]
            except IndexError:
                body = "0"
            return {"code": response_code, "headers": header, "body": body}
        else:
            return failsafe

    def _createTCPConnection(self, link, port=80):
        """
        A function to open a TCP connection between ESP8266 and target.

        Return:
            True on success.
            False if failed to create a connection.
        """

        transmission_data = "AT+CIPSTART="+'"'+"TCP"+'"'+','+'"'+link+'"'+','+str(port)+"\r\n"
        received_data = self.write_to_uart(transmission_data)
        if(received_data is not None):
            if ESP8266_OK_STATUS in received_data:
                return True
            else:
                return False
        else:
            False

    def doHttpGet(self, target, path, user_agent="ESP8622", content_type="text/plain", payload="", port=80):
        """
        A function to perform a get request.

        Parameters:
            target (str): target IP (like "192.168.1.1") or URL (like "www.httpbin.org") only, without /path
            path (str): path ("/get")
            user-agent (str), optional, default = "ESP8622"
            content_type (str), optional, default = "text/plain"
            payload (str), optional, default = ""
            port (int) optional, default = 80

        Returns:
            On success: dict {"code":"", "headers":"", "body":""}
            On error: dict {"code":"0", "headers":"0", "body":"0"}
        """

        failsafe = {"code": "0", "headers": "0", "body": "0"}
        if(self._createTCPConnection(target, port) is True):
            request = "GET "+path+" HTTP/1.1\r\n"+"Host: "+target+"\r\n"+"User-Agent: "+user_agent+"\r\n"+"Content-Type: "+content_type+"\r\n"+"Content-Length: "+str(len(payload))+"\r\n"+"\r\n"+payload+"\r\n"
            # print(request)
            transmission_data = "AT+CIPSEND="+str(len(request))+"\r\n"
            received_data = self.write_to_uart(transmission_data)
            if(received_data is not None):
                if ">" in received_data:
                    received_data = self.write_to_uart(request, delay=1)
                    self.write_to_uart("AT+CIPCLOSE\r\n")
                    received_data = self.parseHTTP(received_data)
                    return received_data
                else:
                    return failsafe
            else:
                return failsafe
        else:
            self.write_to_uart("AT+CIPCLOSE\r\n")
            return failsafe

    def doHttpPut(self, target, path, user_agent="ESP8622", content_type="text/plain", payload="", port=80):
        """
        A function to perform a put request.

        Parameters:
            target (str): target IP (like "192.168.1.1") or URL (like "www.httpbin.org") only, without /path
            path (str): path ("/put")
            user-agent (str), optional, default = "ESP8622"
            content_type (str), optional, default = "text/plain"
            payload (str), optional, default = ""
            port (int) optional, default = 80

        Returns:
            On success: dict {"code":"", "headers":"", "body":""}
            On error: dict {"code":"0", "headers":"0", "body":"0"}
        """

        failsafe = {"code": "0", "headers": "0", "body": "0"}
        if(self._createTCPConnection(target, port) is True):
            request = "PUT "+path+" HTTP/1.1\r\n"+"Host: "+target+"\r\n"+"User-Agent: "+user_agent+"\r\n"+"Content-Type: "+content_type+"\r\n"+"Content-Length: "+str(len(payload))+"\r\n"+"\r\n"+payload+"\r\n"
            # print(request)
            transmission_data = "AT+CIPSEND="+str(len(request))+"\r\n"
            received_data = self.write_to_uart(transmission_data)
            if(received_data is not None):
                if ">" in received_data:
                    received_data = self.write_to_uart(request, delay=1)
                    self.write_to_uart("AT+CIPCLOSE\r\n")
                    received_data = self.parseHTTP(received_data)
                    return received_data
                else:
                    return failsafe
            else:
                return failsafe
        else:
            self.write_to_uart("AT+CIPCLOSE\r\n")
            return failsafe

    def doHttppost(self, target, path, user_agent="ESP8622", content_type="text/plain", payload="", port=80):
        """
        A function to perform a post request.

        Parameters:
            target (str): target IP (like "192.168.1.1") or URL (like "www.httpbin.org") only, without /path
            path (str): path ("/post")
            user-agent (str), optional, default = "ESP8622"
            content_type (str), optional, default = "text/plain"
            payload (str), optional, default = ""
            port (int) optional, default = 80

        Returns:
            On success: dict {"code":"", "headers":"", "body":""}
            On error: dict {"code":"0", "headers":"0", "body":"0"}
        """

        failsafe = {"code": "0", "headers": "0", "body": "0"}
        if(self._createTCPConnection(target, port) is True):
            request = "POST "+path+" HTTP/1.1\r\n"+"Host: "+target+"\r\n"+"User-Agent: "+user_agent+"\r\n"+"Content-Type: "+content_type+"\r\n"+"Content-Length: "+str(len(payload))+"\r\n"+"\r\n"+payload+"\r\n"
            # print(request)
            transmission_data = "AT+CIPSEND="+str(len(request))+"\r\n"
            received_data = self.write_to_uart(transmission_data)
            if(received_data is not None):
                if ">" in received_data:
                    received_data = self.write_to_uart(request, delay=1)
                    self.write_to_uart("AT+CIPCLOSE\r\n")
                    received_data = self.parseHTTP(received_data)
                    return received_data
                else:
                    return failsafe
            else:
                return failsafe
        else:
            self.write_to_uart("AT+CIPCLOSE\r\n")
            return failsafe
