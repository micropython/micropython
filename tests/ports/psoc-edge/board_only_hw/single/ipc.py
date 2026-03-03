import time
from machine import IPC

# ---------------------------------------------------------------------------
# IPC ping-pong test -- 2 independent services on one endpoint
#
# Each service has its own client_id on both sides:
#   Service 1:  CM33 client_id=3  <-->  CM55 client_id=5
#   Service 2:  CM33 client_id=4  <-->  CM55 client_id=6
#
# Both CM33 clients are registered on the same endpoint (EP_ADDR=1),
# demonstrating how multiple independent services share one IPC endpoint.
# ---------------------------------------------------------------------------

ipc = IPC(src_core=IPC.CM33, target_core=IPC.CM55)
ipc.init()

# Per-service echo state
svc1 = {"received": False, "cmd": None}  # Service 1
svc2 = {"received": False, "cmd": None}  # Service 2


def svc1_cb(cmd, val, cid):
    svc1["received"] = True
    svc1["cmd"] = cmd


def svc2_cb(cmd, val, cid):
    svc2["received"] = True
    svc2["cmd"] = cmd


# Register both services on CM33's endpoint (EP_ADDR=1)
r1 = ipc.register_client(3, svc1_cb, 1, 1)  # Service 1 -- CM33 client_id=3
print("Service1 registered:", r1)
time.sleep_ms(100)
r2 = ipc.register_client(4, svc2_cb, 1, 1)  # Service 2 -- CM33 client_id=4
print("Service2 registered:", r2)
time.sleep_ms(100)

print("\n")
# Boot CM55
ipc.enable_core(IPC.CM55)
print("CM55 enabled successfully \n")
time.sleep(1)  # Allow CM55 to complete initialisation


def ipc_test(cmd, cm55_client, label, state):
    """Send cmd to the given CM55 client and verify the echo arrives in state."""
    state["received"] = False
    ipc.send(cmd, 0, cm55_client)
    print("Sent cmd=0x{:02X} to CM55 client_id={}".format(cmd, cm55_client))
    deadline = 3000  # ms
    while not state["received"] and deadline > 0:
        time.sleep_ms(10)
        deadline -= 10
    if state["received"] and state["cmd"] == cmd:
        print("PASS: {} received; cmd=0x{:02X}".format(label, state["cmd"]))
    else:
        print("FAIL: {} echo not received".format(label))


ipc_test(IPC.CMD_START, 5, "Service1 CMD_START", svc1)  # CM55 client_id=5
ipc_test(IPC.CMD_STOP, 6, "Service2 CMD_STOP", svc2)  # CM55 client_id=6
