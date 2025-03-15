# Driver for Mboot, the MicroPython boot loader
# MIT license; Copyright (c) 2018 Damien P. George

import struct, time, os, hashlib


I2C_CMD_ECHO = 1
I2C_CMD_GETID = 2
I2C_CMD_GETCAPS = 3
I2C_CMD_RESET = 4
I2C_CMD_CONFIG = 5
I2C_CMD_GETLAYOUT = 6
I2C_CMD_MASSERASE = 7
I2C_CMD_PAGEERASE = 8
I2C_CMD_SETRDADDR = 9
I2C_CMD_SETWRADDR = 10
I2C_CMD_READ = 11
I2C_CMD_WRITE = 12
I2C_CMD_COPY = 13
I2C_CMD_CALCHASH = 14
I2C_CMD_MARKVALID = 15


class Bootloader:
    def __init__(self, i2c, addr):
        self.i2c = i2c
        self.addr = addr
        self.buf1 = bytearray(1)
        try:
            self.i2c.writeto(addr, b"")
        except OSError:
            raise Exception("no I2C mboot device found")

    def wait_response(self):
        start = time.ticks_ms()
        while 1:
            try:
                self.i2c.readfrom_into(self.addr, self.buf1)
                n = self.buf1[0]
                break
            except OSError:
                time.sleep_us(500)
            if time.ticks_diff(time.ticks_ms(), start) > 5000:
                raise Exception("timeout")
        if n >= 129:
            raise Exception(n)
        if n == 0:
            return b""
        else:
            return self.i2c.readfrom(self.addr, n)

    def wait_empty_response(self):
        ret = self.wait_response()
        if ret:
            raise Exception("expected empty response got %r" % ret)
        else:
            return None

    def echo(self, data):
        self.i2c.writeto(self.addr, struct.pack("<B", I2C_CMD_ECHO) + data)
        return self.wait_response()

    def getid(self):
        self.i2c.writeto(self.addr, struct.pack("<B", I2C_CMD_GETID))
        ret = self.wait_response()
        unique_id = ret[:12]
        mcu_name, board_name = ret[12:].split(b"\x00")
        return unique_id, str(mcu_name, "ascii"), str(board_name, "ascii")

    def reset(self):
        self.i2c.writeto(self.addr, struct.pack("<B", I2C_CMD_RESET))
        # we don't expect any response

    def getlayout(self):
        self.i2c.writeto(self.addr, struct.pack("<B", I2C_CMD_GETLAYOUT))
        layout = self.wait_response()
        id, flash_addr, layout = layout.split(b"/")
        assert id == b"@Internal Flash  "
        flash_addr = int(flash_addr, 16)
        pages = []
        for chunk in layout.split(b","):
            n, sz = chunk.split(b"*")
            n = int(n)
            assert sz.endswith(b"Kg")
            sz = int(sz[:-2]) * 1024
            for i in range(n):
                pages.append((flash_addr, sz))
                flash_addr += sz
        return pages

    def pageerase(self, addr):
        self.i2c.writeto(self.addr, struct.pack("<BI", I2C_CMD_PAGEERASE, addr))
        self.wait_empty_response()

    def setrdaddr(self, addr):
        self.i2c.writeto(self.addr, struct.pack("<BI", I2C_CMD_SETRDADDR, addr))
        self.wait_empty_response()

    def setwraddr(self, addr):
        self.i2c.writeto(self.addr, struct.pack("<BI", I2C_CMD_SETWRADDR, addr))
        self.wait_empty_response()

    def read(self, n):
        self.i2c.writeto(self.addr, struct.pack("<BB", I2C_CMD_READ, n))
        return self.wait_response()

    def write(self, buf):
        self.i2c.writeto(self.addr, struct.pack("<B", I2C_CMD_WRITE) + buf)
        self.wait_empty_response()

    def calchash(self, n):
        self.i2c.writeto(self.addr, struct.pack("<BI", I2C_CMD_CALCHASH, n))
        return self.wait_response()

    def markvalid(self):
        self.i2c.writeto(self.addr, struct.pack("<B", I2C_CMD_MARKVALID))
        self.wait_empty_response()

    def deployfile(self, filename, addr):
        pages = self.getlayout()
        page_erased = [False] * len(pages)
        buf = bytearray(128)  # maximum payload supported by I2C protocol
        start_addr = addr
        self.setwraddr(addr)
        fsize = os.stat(filename)[6]
        local_sha = hashlib.sha256()
        print("Deploying %s to location 0x%08x" % (filename, addr))
        with open(filename, "rb") as f:
            t0 = time.ticks_ms()
            while True:
                n = f.readinto(buf)
                if n == 0:
                    break

                # check if we need to erase the page
                for i, p in enumerate(pages):
                    if p[0] <= addr < p[0] + p[1]:
                        # found page
                        if not page_erased[i]:
                            print(
                                "\r% 3u%% erase 0x%08x"
                                % (100 * (addr - start_addr) // fsize, addr),
                                end="",
                            )
                            self.pageerase(addr)
                            page_erased[i] = True
                        break
                else:
                    raise Exception("address 0x%08x not valid" % addr)

                # write the data
                self.write(buf)

                # update local SHA256, with validity bits set
                if addr == start_addr:
                    buf[0] |= 3
                if n == len(buf):
                    local_sha.update(buf)
                else:
                    local_sha.update(buf[:n])

                addr += n
                ntotal = addr - start_addr
                if ntotal % 2048 == 0 or ntotal == fsize:
                    print("\r% 3u%% % 7u bytes   " % (100 * ntotal // fsize, ntotal), end="")
            t1 = time.ticks_ms()
        print()
        print("rate: %.2f KiB/sec" % (1024 * ntotal / (t1 - t0) / 1000))

        local_sha = local_sha.digest()
        print("Local SHA256: ", "".join("%02x" % x for x in local_sha))

        self.setrdaddr(start_addr)
        remote_sha = self.calchash(ntotal)
        print("Remote SHA256:", "".join("%02x" % x for x in remote_sha))

        if local_sha == remote_sha:
            print("Marking app firmware as valid")
            self.markvalid()

        self.reset()
