"""
Amazon FreeRTOS
Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 
http://aws.amazon.com/freertos
http://www.FreeRTOS.org 

"""
import subprocess
import serial
import os
from shutil import copyfile
from time import sleep
from threading import Thread, Condition

class FlashSerialComm:
    """Manage flashing and reading the output through the serial port of the board.

    Attributes:
        _flashConfig (dict): flash configuration defined in board.json under the filed 'flash_config'.
        _serialThread (Thread): Long running thread that stores a log of the data read.
    Example:
        flashComm = FlashSerialComm(flashConfig)
        flashComm.flashAndRead()
        # Do some stuff..
        flashComm.stopSerialRead()
        log = flashComm.getSerialLog()
        flashComm.cleanup()
    """
    def __init__(self, flashConfig, initial_image = None, reset_image = None):
        self._flashConfig = flashConfig
        if flashConfig.get('run_executable', False):
            self._serialThread = ReadSerialThread(
                flashConfig['serial_port'],
                flashConfig['serial_baudrate'],
                flashConfig['serial_timeout_sec'],
                initial_image,
                reset_image
            )
        else:
            self._serialThread = ReadSerialThread(
                flashConfig['serial_port'],
                flashConfig['serial_baudrate'],
                flashConfig['serial_timeout_sec']
            )
        # Start the serial reading thread, it will wait until we tell it to start reading.
        self._serialThread.start()

    def flashApplication(self):
        """Flash the application to the board. 
        
        We are expecting the flash commands to not only program the board, but to also
        take the board out of reset after programming.
        """
        # Save the system path to restore.
        system_path = os.environ['PATH']
        # Add the tool_paths to the system PATH
        for path in self._flashConfig['tool_paths']:
            os.environ['PATH'] = path + os.pathsep + os.environ['PATH']

        flashCommands = self._flashConfig['commands']
        print('Flashing test application to board...')
        for command in flashCommands:
            command = command.format(**self._flashConfig)
            subprocess.Popen(command, shell=True).wait()
        print('Done with flash commands. Now running...')

        # Restore the system path
        os.environ['PATH'] = system_path
        
    def flashAndRead(self):
        """Flash program the board and also open a serial port for reading.
        """
        retryCount = 0
        testOutput = ''

        while (not testOutput) and retryCount < self._flashConfig['flash_num_retry']:
            # Clear the serial log before each new program of the board.
            self._serialThread.clearLog()
            if self._flashConfig.get('run_executable', False):
                # Start the thread to read the output of the executable
                self._serialThread.startRead()
            elif self._flashConfig['flash_after_serial_open']:
                # Open the serial port because it takes a while. We do this before flashing
                # so that we don't miss any output logs.
                self._serialThread.startRead()
                self.flashApplication()
            else:
                # Some boards, especially where the output is USB, do not need for the 
                # serial port to be open before flashing to not miss logs; or have the same
                # port for flashing and serial communication, so we must flash before opening
                # the serial port.
                self.flashApplication()
                self._serialThread.startRead()
                
            # Wait the serial communication timeout to get an initial log output.
            sleep(self._flashConfig['serial_timeout_sec'])
            # Get the initial log, it should not be blank, if it is we will repeat flashing.
            testOutput = self._serialThread.getLog()
            retryCount += 1

    def stopSerialRead(self):
        """Stop the serial thread from reading. This will close the serial port.
        """
        self._serialThread.stopRead()

    def getSerialLog(self):
        """Get the most recent log from the serial thread.
        """
        return self._serialThread.getLog()

    def cleanup(self):
        """Clean up resources. Once cleaned this object cannot be used again.
        """
        self._serialThread.close()
        self._serialThread.join()


class ReadSerialThread(Thread):
    """A class for reading the serial port.
    
    A serial thread class is created so that we can:
        - save logs for analyzing test out out.
        - Stop the thread in it's infinite serial read loop. We want to be able to stop
          reading after tests are over and to give up the serial port when necessary.
    
    Attributes:
        _executable (str): The path to the executable to run instead of opening the serial port.
        _port (str): The serial communication port defined in board.json under the 
            flash_config field. This port is 'COMx' in Windows and '/dev/xxx' in Unix
            systems.
        _baudrate (int): The bps of the serial port.
        _timeout (int): Timeout in seconds on the serial port read if we don't recieve any data.
        _stopRead (bool): Initially set to false so that we can begin reading as soon 
            as we are told to grab the serial port. set to true in the middle of a read
            to let go of the serial port. The thread does not exit we go to the 
            _holdBeforeOpen state.
        _exitRun (bool): Thread exits if this is set to True.
        _holdBeforeOpen (bool): variable to wait on before opening the serial port for 
            reading.
        _log (str): All of the serial output in the last run. 

    Example:
        serialThread = ReadSerialThread(
            'COM5',
            '115200',
            30
        )
        serialThread.start()
        serialThread.startRead()
        # Do some other stuff.
        log = serialThread.getLog()
        serialThread.close()
    """
    def __init__(self, port, baudrate, serialTimeout, initial_executable = None, reset_executable = None):
        Thread.__init__(self)
        self._initial_executable = initial_executable
        self._reset_executable = reset_executable
        self._port = port
        self._baudrate = baudrate
        self._timeout = serialTimeout
        self._stopRead = False
        self._exitRun = False
        self._holdBeforeOpen = True
        self._log = ''

    def stopRead(self):
        """ Stop reading from the serial port. This will not exit the thread. It will
        let go of the serial COM port.
        """
        self._stopRead = True

    def __exitRun(self):
        """ Exit the thread.
        """
        self._exitRun = True

    def startRead(self):
        """ Start the serial reading. This will open the serial port and begin reading.
        """
        self._holdBeforeOpen = False

    def getLog(self):
        """ Get the current serial log.
        """
        return self._log

    def clearLog(self):
        """ Clear the serial log.
        """
        self._log = ''

    def _readOutput(self, outStream):
        # Read until we are told to stop.
        self._stopRead = False 
        while self._stopRead == False and not self._exitRun:
            try:
                line = outStream.readline().decode()
                # For windows simulator once we reach a reboot place, then we will manually reboot.
                if 'Failed to activate new image (0x00000000). Please reset manually.' in line:
                    return 'reset'
            except UnicodeDecodeError:
                # Discard data if fails to decode.
                pass
            except Exception:
                # All other exceptions stop the read.
                self._stopRead = True
            else:
                if not line:
                    continue
                print(line, end='')
                self._log += line

    def run(self):
        """ Run until this object is closed. 
        """
        while not self._exitRun:
            # Start out each new read paused.
            self._holdBeforeOpen = True
            # Nonblocking wait for the open of the serial port.
            while self._holdBeforeOpen and not self._exitRun: 
                sleep(0.5)
            # Exit the run if we are told to do so.
            if self._exitRun: continue

            proc = None
            if self._initial_executable:
                proc = subprocess.Popen(self._initial_executable.replace('/', '\\'), stdout=subprocess.PIPE)
                while(self._readOutput(proc.stdout) == 'reset'):
                    proc.kill()
                    proc = subprocess.Popen(self._reset_executable.replace('/', '\\'), stdout=subprocess.PIPE)
                    # If we come out from a reset needed. then we need to reset to the OTA image
                proc.kill()
            else:
                with serial.Serial(port=self._port, baudrate=self._baudrate, timeout=self._timeout) as outStream:
                    self._readOutput(outStream)

    def close(self):
        """ Close this object by stopping the read and freeing resources.
        Once closed, the thread cannot be restart.
        """
        # Let go of the serial port.
        self.stopRead()
        # Exit the thread run().
        self.__exitRun()
        # Clear the log file.
        self.clearLog()