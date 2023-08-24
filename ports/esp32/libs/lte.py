'''Controller for using Sequans LTE Modem'''

import time
import machine
from machine import Pin

StatusCode = int

class LTETimeoutError(Exception):

    '''Exception raised on timeout of an action'''

class LTEConnectionError(Exception):

    '''Exception raised due to a connection error'''

class LTEModemError(Exception):

    '''Exception raised when the modem outputs ERROR instead of OKAY'''

class LTE:

    '''Interface for using LTE modem'''

    @staticmethod
    def close_modem():
        '''Turn the Sequans Modem off'''

    def _flush(self):
        self._pause(0.3)
        while self.uart.any() > 0:
            self.uart.read()
            self._pause(0.01)

        self._pause(0.3)
        while self.uart.any() > 0:
            self.uart.read()

    def _write(self, msg : str):
        self.uart.write(msg + "\r\n")

    def _read(self) -> str:
        self._pause(0.1)
        result = ""
        while self.uart.any() > 0:
            result += self.uart.read().decode("ascii")
        return result

    def _wait_msg(self, msg : str, timeout_s : float = 5, except_on_error = True):
        start_time = time.time()
        while time.time() < start_time + timeout_s:
            while self.uart.any() > 0:
                try:
                    for line in self.uart.read().decode("ascii").split("\r\n"):
                        print(line)
                        if msg in line:
                            self._flush()
                            return
                        if except_on_error and 'ERROR' in line:
                            raise LTEModemError
                except UnicodeError:
                    ...
        raise LTETimeoutError

    def _wait_ok(self, timeout_s : int = 5, except_on_error = True):
        start_time = time.time()
        while time.time() < start_time + timeout_s:
            while self.uart.any() > 0:
                try:
                    for line in self.uart.read().decode("ascii").split("\r\n"):
                        print(line)
                        if 'OK' in line:
                            return
                        if except_on_error and 'ERROR' in line:
                            raise LTEModemError
                except UnicodeError:
                    ...
        raise LTETimeoutError

    def _get_line_with(self, msg : str, timeout_s : int = 5, except_on_error = True):
        start_time = time.time()
        found = False
        found_line = ''
        while not found:
            if time.time() > start_time + timeout_s:
                raise LTETimeoutError
            while self.uart.any() > 0:
                for line in self.uart.read().decode("ascii").split("\r\n"):
                    print(line)
                    if msg in line:
                        found_line = line
                        return line
                    elif except_on_error and 'ERROR' in line:
                        raise LTEConnectionError
        return found_line

    def _pause(self, time_s : float):
        time.sleep(time_s)

    def _start_modem(self):
        '''Wake the modem up from sleep mode'''
        # Wake up the modem
        # Connect to the internet
        # TODO implement wake up modem

        self.p = Pin(5,Pin.OUT)
        self.p.value(1)
        self._pause(0.1)
        self.p.value(0)
        self._pause(0.3)
        self.p.value(1)
        self._pause(1)
        if self.UART is not None or LTE.UART is not None:
            self.uart.deinit()
        self.uart = machine.UART(2,115200)
        LTE.UART = self.uart
        self.stop_count = 0
        self.uart.init(
            tx = 17,
            rx = 16,
            cts = 15,
            rts = 14,
            timeout_char = 1,
            flow=machine.UART.RTS|machine.UART.CTS
        )
        self.UART = self.uart
        print('waiting message start')
        self._wait_msg("START")
        # self._pause(1)
        # self._write("AT^RESET")

    def _receive_response(self, timeout_s : int) -> tuple[StatusCode, str]:
        '''Wait for a response to an HTTP Requests'''
        print('receiving response')
        start_time = time.time()
        ring_line = self._get_line_with('SQNHTTPRING', timeout_s)
        print(ring_line)
        print('received ring')
        response_data = ring_line.replace('\r\n','').split(',')
        status_code = response_data[1] if len(response_data) > 2 else 0
        response_length = response_data[-1] if len(response_data) > 2 else 0
        response_length = int(response_length)
        status_code = int(status_code)
        # check the status code and message length are correct
        # TODO check if this should be removed
        if status_code == 200 and response_length == 0:
            raise LTEConnectionError
        if status_code != 200:
            raise LTEConnectionError
        self._pause(0.)
        # clear the uart connection
        self._flush()
        # request the response body
        self._write('AT+SQNHTTPRCV=1')
        # read from the server until the expected response length has been received
        response = ""
        while True:
            data = self._read()
            response += data
            if len(response) >= response_length:
                break
            if time.time() > start_time + timeout_s:
                raise LTETimeoutError
        return status_code, response[5:-5] if len(response) > 10 else response

    def _setup(self):
        print('starting modem')
        self._start_modem()
        print('started modem')
        # Set up APN and bands
        # self._read()
        # print('setting apn')
        # self._write(f'AT+CGDCONT=1,"IP","{self.apn}"')
        # self._read()
        # print('setting band')
        self._write(f'AT+SQNBANDSEL=1,"standard","{str(self.bands)[1:-1]}"')
        self._read()
        # Set the modem to scan network
        print('setting cfun')
        self._write('AT+CFUN=1')
        # self._wait_msg('+CEREG: 5', timeout_s=10)
        self._read()
        # print('setting cops')
        # self._write('AT+COPS=0')
        self._read()
        print('waiting connection')
        self.wait_connection(timeout_s=360000)

    UART : machine.UART | None = None

    def __init__(self, apn : str, bands : list[int]) -> None:
        self.apn = apn
        self.bands = bands
        self.uart = None
        self.p = None
        # wait for the modem to start
        self._setup()

    def close(self):
        '''Close the uart connection'''
        self.uart.deinit()
        self._pause(0.2)


    def sleep_modem(self):
        '''Put the modem in sleep mode'''
        raise NotImplementedError

    def check_connection(self) -> bool:
        '''Return True if connected to the network'''
        # Return true if connected

        self._write('AT!="fsm"')
        self._pause(0.1)
        while self.uart.any() > 0:
            for line in self.uart.read().decode("ascii").split("\r\n"):
                print(line)
                if 'CONNECTED' in line:
                    self._flush()
                    return True
                if 'STOPPED' in line or 'ERROR' in line:
                    self.stop_count += 1
                    print(f'stopped{self.stop_count}')
                    if self.stop_count > 10:
                        print('Resetting')
                        self._setup()
                        self._pause(1)
                        self.stop_count = 0
        return False

    def wait_connection(self, timeout_s : float = 30):
        '''Pause execution until a connection is made or until the timeout is reached'''
        # Await till connected is true or raise a a timeout exception
        start_time = time.time_ns() / 1000000000
        while time.time_ns() / 1000000000 < start_time + timeout_s:
            if self.check_connection():
                self._flush()
                return
            self._pause(0.5)
            self._flush()
        raise LTETimeoutError

    def http_get(self,
        address : str,
        resource : str = '/',
        timeout_s : int = 20,
        retry_count : int = 5) -> tuple[StatusCode, str]:
        '''Make an http GET request to the given URL and return the response and status code'''

        start_time = time.time()
        count = 0
        while count < retry_count:
            if not self.check_connection():
                self._write('AT+CFUN=0')
                self._pause(0.2)
                self._write('AT+CFUN=1')
                self._pause(0.2)
                self.wait_connection(timeout_s=30)
            try:
                if not self.check_connection():
                    self._write('AT+CFUN=1')
                    self._read()
                    self._write('AT+COPS=0')
                    self._read()
                    self._pause(0.1)
                    raise LTEConnectionError
                if time.time() > start_time + timeout_s:
                    raise LTETimeoutError
                # clear the uart connection
                self._flush()
                self._pause(0.2)
                print(timeout_s - (time.time() - start_time))
                # Set up connection to address and endpoint
                self._write(f'AT+SQNHTTPCFG=1,"{address}"')
                print(f'AT+SQNHTTPCFG=1,"{address}"')
                self._wait_ok(timeout_s=min(5,timeout_s - (time.time() - start_time)))
                print('received okay')
                self._pause(0.3)
                self._write(f'AT+SQNHTTPQRY=1,0,"{resource}"')
                print(f'AT+SQNHTTPQRY=1,0,"{resource}"')
                # self._wait_ok(timeout_s=min(5,timeout_s - (time.time() - start_time)))
                return self._receive_response(timeout_s - (time.time() - start_time))

            except (LTEConnectionError,LTEModemError):
                # sleep with a back off
                print('Error, retrying')
                count += 1
                self._pause(1*count)

        self._setup()
        raise LTEConnectionError

    def http_post(self,
        address : str,
        resource : str,
        body : bytes | str,
        timeout_s : float = 30,
        retry_count : int = 3) -> tuple[StatusCode, str]:
        '''Make an HTTP POST request to the given URL and return the response and status code'''

        if isinstance(body, str):
            body = bytes(body, 'utf-8')

        data_len = len(body) + 1

        start_time = time.time()
        count = 0
        print('Dtarting')
        while count < retry_count:
            if not self.check_connection():
                self._write('AT+CFUN=0')
                self._pause(0.2)
                self._write('AT+CFUN=1')
                self._pause(0.2)
                self.wait_connection(timeout_s=30)
            try:

                if not self.check_connection():
                    print('not connected')
                    raise LTEConnectionError
                print(time.time() > start_time + timeout_s)
                if time.time() > start_time + timeout_s:
                    raise LTETimeoutError
                # clear the uart connection
                self._flush()
                self._pause(0.2)
                self._flush()
                # Set up connection to address and endpoint
                print('writing address' + f'AT+SQNHTTPCFG=1,"{address}"')
                self._write(f'AT+SQNHTTPCFG=1,"{address}"')

                # AT+SQNHTTPCFG=1,"httpbin.org",80,0,"","",0,120,1


                print('Awaiting Okay')
                # self._wait_ok(timeout_s=min(5,timeout_s - (time.time() - start_time)))
                self._wait_ok(timeout_s=10)
                print('writing data len')
                print(f'AT+SQNHTTPSND=1,0,"{resource}",{data_len}')
                self._write(
                    f'AT+SQNHTTPSND=1,0,"{resource}",{data_len}'
                )
                print('waitign >>>')
                self._wait_msg('>', timeout_s=5)
                # send bytes
                print('writing body')
                print(body)
                self.uart.write(body)
                self._pause(0.1)
                return self._receive_response(timeout_s - (time.time() - start_time))

            except (LTEConnectionError,LTEModemError):
                # sleep with a back off
                print('Error, ')
                count += 1
                self._pause(1*count)

        self._setup()
        raise LTEConnectionError

    def signal_strength(self) -> str:
        '''Return the signal strength'''
        self._flush()
        self._write('AT+CSQ')
        return self._get_line_with('CSQ')
