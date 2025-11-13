# UDP throughput test, sends large packets as fast as possible.
# Runs under CPython, MicroPython unix and MicroPython bare-metal.

import sys
import time
import socket
import struct

if hasattr(time, "ticks_us"):
    from micropython import const

    ticks_us = time.ticks_us
    ticks_diff = time.ticks_diff
else:
    const = lambda x: x
    ticks_us = lambda: time.monotonic_ns() // 1000
    ticks_diff = lambda a, b: a - b

    class micropython:
        def native(f):
            return f


MAGIC = 0x5AB68EF0
PORT = 8234


@micropython.native
def client(ip, _udp_packet_size):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.settimeout(1)
    addr = socket.getaddrinfo(ip, PORT)[0][-1]
    s.connect(addr)
    N = 1000
    s.send(struct.pack("<III", MAGIC, _udp_packet_size, N))
    try:
        resp, server_addr = s.recvfrom(2)
    except OSError:
        s.close()
        print("server did not respond")
        return

    if resp != b"OK":
        s.close()
        print("invalid server response", resp)
        return

    # print(server_addr, end=" ")
    print(_udp_packet_size, end=" ")

    data = bytearray(_udp_packet_size)
    t0 = ticks_us()
    for i in range(0, N, 4):
        data[1] = (i >> 8) & 0xFF

        if 1:
            data[0] = i & 0xFF
            n = s.send(data)
            # assert(n == _udp_packet_size)

            data[0] = (i + 1) & 0xFF
            n = s.send(data)
            # assert(n == _udp_packet_size)

            data[0] = (i + 2) & 0xFF
            n = s.send(data)
            # assert(n == _udp_packet_size)

            data[0] = (i + 3) & 0xFF
            n = s.send(data)
            # assert(n == _udp_packet_size)
        else:
            data[0] = i & 0xFF
            s.send(data, 4)

    dt = ticks_diff(ticks_us(), t0)
    print(N * len(data) / dt * 8, "Mbit/s")

    try:
        resp, _ = s.recvfrom(4)
    except OSError:
        pass

    s.close()


def server_single():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.settimeout(0.2)
    s.bind(socket.getaddrinfo("0.0.0.0", PORT)[0][-1])
    print("waiting...", end=" ")
    sys.stdout.flush()

    while True:
        try:
            data, addr = s.recvfrom(12)
        except OSError:
            continue
        except KeyboardInterrupt:
            s.close()
            print("finish")
            return False
        magic, packet_size, num_packet = struct.unpack("<III", data)
        if magic != MAGIC:
            print("incorrect magic")
            return True
        break

    print(f"connect {addr}, packet_size {packet_size}")
    s.sendto(b"OK", addr)

    np = 0
    nbytes = 0
    t_start = None
    t_last = None
    t0 = ticks_us()
    success = True
    while ticks_diff(ticks_us(), t0) < 500_000:
        try:
            x = s.recvfrom(packet_size)
        except OSError:
            continue
        except KeyboardInterrupt:
            success = False
            break
        t0 = ticks_us()
        if t_start is None:
            t_start = t0
        t_last = t0
        np += 1
        nbytes += len(x[0])
        idx = x[0][0] | x[0][1] << 8
        # print(len(x[0]), idx, t_last - t_start, nbytes, end="\r")

    s.sendto(b"DONE", addr)

    if t_start is not None and t_last > t_start:
        dt = t_last - t_start
        print(np, nbytes, dt)
        print(100 * np / num_packet, "%  ", nbytes / dt * 8, "MBit/s")
    s.close()
    return success


def server():
    while True:
        if not server_single():
            break


def main():
    opt_mode = "-s"
    sys.argv.pop(0)
    while sys.argv:
        opt = sys.argv.pop(0)
        if opt == "-s":
            opt_mode = opt
        elif opt == "-c":
            opt_mode = opt
            opt_host = sys.argv.pop(0)
        else:
            print("unknown option:", opt)
            raise SystemExit(1)

    if opt_mode == "-s":
        server()
    else:
        client(opt_host, 9000 - 44)


if sys.platform == "linux":
    # Run on host PC.
    if __name__ == "__main__":
        main()
else:
    # Run on board.
    IP = "10.0.0.88"
    import network

    lan = network.LAN()
    if 0:
        # lan.config(dump=0)
        client(IP, 1450)
        client(IP, 1450)
        # lan.config(dump=0)
        ps = 8 * 1450
        print("packet size", ps)
        client(IP, ps)
        # lan.config(dump=1)

    if 1:
        for u in range(1, 20):
            client(IP, u * 100)
        for u in range(2, 20):
            client(IP, u * 1000)
