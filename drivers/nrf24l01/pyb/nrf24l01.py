"""NRF24L01 driver for Micro Python
"""

from micropython import const
import pyb

# nRF24L01+ registers
CONFIG      = const(0x00)
EN_RXADDR   = const(0x02)
SETUP_AW    = const(0x03)
SETUP_RETR  = const(0x04)
RF_CH       = const(0x05)
RF_SETUP    = const(0x06)
STATUS      = const(0x07)
RX_ADDR_P0  = const(0x0a)
TX_ADDR     = const(0x10)
RX_PW_P0    = const(0x11)
FIFO_STATUS = const(0x17)
DYNPD	    = const(0x1c)

# CONFIG register
EN_CRC      = const(0x08) # enable CRC
CRCO        = const(0x04) # CRC encoding scheme; 0=1 byte, 1=2 bytes
PWR_UP      = const(0x02) # 1=power up, 0=power down
PRIM_RX     = const(0x01) # RX/TX control; 0=PTX, 1=PRX

# RF_SETUP register
POWER_0     = const(0x00) # -18 dBm
POWER_1     = const(0x02) # -12 dBm
POWER_2     = const(0x04) # -6 dBm
POWER_3     = const(0x06) # 0 dBm
SPEED_1M    = const(0x00)
SPEED_2M    = const(0x08)
SPEED_250K  = const(0x20)

# STATUS register
RX_DR       = const(0x40) # RX data ready; write 1 to clear
TX_DS       = const(0x20) # TX data sent; write 1 to clear
MAX_RT      = const(0x10) # max retransmits reached; write 1 to clear

# FIFO_STATUS register
RX_EMPTY    = const(0x01) # 1 if RX FIFO is empty

# constants for instructions
R_RX_PL_WID  = const(0x60) # read RX payload width
R_RX_PAYLOAD = const(0x61) # read RX payload
W_TX_PAYLOAD = const(0xa0) # write TX payload
FLUSH_TX     = const(0xe1) # flush TX FIFO
FLUSH_RX     = const(0xe2) # flush RX FIFO
NOP          = const(0xff) # use to read STATUS register

class NRF24L01:
    def __init__(self, spi, cs, ce, channel=46, payload_size=16):
        assert payload_size <= 32

        # init the SPI bus and pins
        spi.init(spi.MASTER, baudrate=4000000, polarity=0, phase=0, firstbit=spi.MSB)
        cs.init(cs.OUT_PP, cs.PULL_NONE)
        ce.init(ce.OUT_PP, ce.PULL_NONE)

        # store the pins
        self.spi = spi
        self.cs = cs
        self.ce = ce

        # reset everything
        self.ce.low()
        self.cs.high()
        self.payload_size = payload_size
        self.pipe0_read_addr = None
        pyb.delay(5)

        # set address width to 5 bytes and check for device present
        self.reg_write(SETUP_AW, 0b11)
        if self.reg_read(SETUP_AW) != 0b11:
            raise OSError("nRF24L01+ Hardware not responding")

        # disable dynamic payloads
        self.reg_write(DYNPD, 0)

        # auto retransmit delay: 1750us
        # auto retransmit count: 8
        self.reg_write(SETUP_RETR, (6 << 4) | 8)

        # set rf power and speed
        self.set_power_speed(POWER_3, SPEED_250K) # Best for point to point links

        # init CRC
        self.set_crc(2)

        # clear status flags
        self.reg_write(STATUS, RX_DR | TX_DS | MAX_RT)

        # set channel
        self.set_channel(channel)

        # flush buffers
        self.flush_rx()
        self.flush_tx()

    def reg_read(self, reg):
        self.cs.low()
        self.spi.send_recv(reg)
        buf = self.spi.recv(1)
        self.cs.high()
        return buf[0]

    def reg_write(self, reg, buf):
        self.cs.low()
        status = self.spi.send_recv(0x20 | reg)[0]
        self.spi.send(buf)
        self.cs.high()
        return status

    def flush_rx(self):
        self.cs.low()
        self.spi.send(FLUSH_RX)
        self.cs.high()

    def flush_tx(self):
        self.cs.low()
        self.spi.send(FLUSH_TX)
        self.cs.high()

    # power is one of POWER_x defines; speed is one of SPEED_x defines
    def set_power_speed(self, power, speed):
        setup = self.reg_read(RF_SETUP) & 0b11010001
        self.reg_write(RF_SETUP, setup | power | speed)

    # length in bytes: 0, 1 or 2
    def set_crc(self, length):
        config = self.reg_read(CONFIG) & ~(CRCO | EN_CRC)
        if length == 0:
            pass
        elif length == 1:
            config |= EN_CRC
        else:
            config |= EN_CRC | CRCO
        self.reg_write(CONFIG, config)

    def set_channel(self, channel):
        self.reg_write(RF_CH, min(channel, 125))

    # address should be a bytes object 5 bytes long
    def open_tx_pipe(self, address):
        assert len(address) == 5
        self.reg_write(RX_ADDR_P0, address)
        self.reg_write(TX_ADDR, address)
        self.reg_write(RX_PW_P0, self.payload_size)

    # address should be a bytes object 5 bytes long
    # pipe 0 and 1 have 5 byte address
    # pipes 2-5 use same 4 most-significant bytes as pipe 1, plus 1 extra byte
    def open_rx_pipe(self, pipe_id, address):
        assert len(address) == 5
        assert 0 <= pipe_id <= 5
        if pipe_id == 0:
            self.pipe0_read_addr = address
        if pipe_id < 2:
            self.reg_write(RX_ADDR_P0 + pipe_id, address)
        else:
            self.reg_write(RX_ADDR_P0 + pipe_id, address[0])
        self.reg_write(RX_PW_P0 + pipe_id, self.payload_size)
        self.reg_write(EN_RXADDR, self.reg_read(EN_RXADDR) | (1 << pipe_id))

    def start_listening(self):
        self.reg_write(CONFIG, self.reg_read(CONFIG) | PWR_UP | PRIM_RX)
        self.reg_write(STATUS, RX_DR | TX_DS | MAX_RT)

        if self.pipe0_read_addr is not None:
            self.reg_write(RX_ADDR_P0, self.pipe0_read_addr)

        self.flush_rx()
        self.flush_tx()
        self.ce.high()
        pyb.udelay(130)

    def stop_listening(self):
        self.ce.low()
        self.flush_tx()
        self.flush_rx()

    # returns True if any data available to recv
    def any(self):
        return not bool(self.reg_read(FIFO_STATUS) & RX_EMPTY)

    def recv(self):
        # get the data
        self.cs.low()
        self.spi.send(R_RX_PAYLOAD)
        buf = self.spi.recv(self.payload_size)
        self.cs.high()
        # clear RX ready flag
        self.reg_write(STATUS, RX_DR)

        return buf

    # blocking wait for tx complete
    def send(self, buf, timeout=500):
        send_nonblock = self.send_start(buf)
        start = pyb.millis()
        result = None
        while result is None and pyb.elapsed_millis(start) < timeout:
            result = self.send_done() # 1 == success, 2 == fail
        if result == 2:
            raise OSError("send failed")

    # non-blocking tx
    def send_start(self, buf):
        # power up
        self.reg_write(CONFIG, (self.reg_read(CONFIG) | PWR_UP) & ~PRIM_RX)
        pyb.udelay(150)
        # send the data
        self.cs.low()
        self.spi.send(W_TX_PAYLOAD)
        self.spi.send(buf)
        if len(buf) < self.payload_size:
            self.spi.send(b'\x00' * (self.payload_size - len(buf))) # pad out data
        self.cs.high()

        # enable the chip so it can send the data
        self.ce.high()
        pyb.udelay(15) # needs to be >10us
        self.ce.low()

    # returns None if send still in progress, 1 for success, 2 for fail
    def send_done(self):
        if not (self.reg_read(STATUS) & (TX_DS | MAX_RT)):
            return None # tx not finished

        # either finished or failed: get and clear status flags, power down
        status = self.reg_write(STATUS, RX_DR | TX_DS | MAX_RT)
        self.reg_write(CONFIG, self.reg_read(CONFIG) & ~PWR_UP)
        return 1 if status & TX_DS else 2
