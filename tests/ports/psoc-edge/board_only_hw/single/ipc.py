import time
from machine import IPC

ipc = IPC(src_core=IPC.CM33, target_core=IPC.CM55)
ipc.init()
print("IPC initialized successfully")
ipc.register_sender_client(1, cb)


def cb():
    print("cb called")


ipc.irq(cb, 1)
print("IRQ registered successfully")

ipc.enable_core(IPC.CM55)
time.sleep(1)

ipc.send(IPC.CMD_LED_ON)
time.sleep(2)
