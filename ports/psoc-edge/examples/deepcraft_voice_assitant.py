"""
deepcraft_voice_assistant.py — Host-side (MicroPython) Voice Assistant application.

Uses the DEEPCRAFTModel wrapper library so application code never touches
raw IPC commands or client/endpoint IDs directly.

Layer:
    deepcraft_voice_assistant.py
        │  uses high-level events (VA_EVENT_*)
        ▼
    deepcraft_model.DEEPCRAFTModel    ← wrapper library (deepcraft_interface.c)
        │  calls send / register_client on the injected transport object
        ▼
    machine.IPC                       ← MicroPython IPC module (machine_ipc.c)
        │
        ▼ IPC pipe
    Target (CM55 running main.c + deepcraft_ipc_cm55.c)

To use a different transport, pass any object with matching method signatures
(send, register_client, enable_target) to DEEPCRAFTModel(); nothing else changes.
"""

from machine import IPC, Pin
from deepcraft_model import DEEPCRAFTModel
import time

# ---------------------------------------------------------------------------
# Hardware output — update to match your board/model
# ---------------------------------------------------------------------------
pin_out = Pin("P17_1", Pin.OUT, value=1)

# Intent index → action mapping.
# Update these to match the commands defined in your DEEPCRAFT model.
INTENT_ACTIONS = {
    0: lambda: (pin_out.value(0), print("[HOST] P17_1 -> LOW")),
    1: lambda: (pin_out.value(1), print("[HOST] P17_1 -> HIGH")),
}

# ---------------------------------------------------------------------------
# Transport setup — IPC
# DEEPCRAFTModel constructor calls ipc.init() automatically.
# ---------------------------------------------------------------------------
ipc = IPC(src_core=IPC.CM33, target_core=IPC.CM55)

# ---------------------------------------------------------------------------
# DEEPCRAFTModel — wraps the transport and exposes VA events
#
# Optional keyword arguments (shown with their defaults):
#   model            = DEEPCRAFTModel.MODEL_VA
#   target_client_id = 5    (target IPC pipe client ID)
#   recv_client_id   = 3    (host    IPC pipe client ID)
#   recv_ep_addr     = 1    (host    IPC pipe endpoint address)
#   target_id        = 1    (IPC.CM55 — passed to ipc.enable_target())
# ---------------------------------------------------------------------------
model = DEEPCRAFTModel(ipc)


# ---------------------------------------------------------------------------
# Event callback — invoked by the wrapper with (va_model_events_t, value)
# ---------------------------------------------------------------------------
def on_va_event(event, value):
    if event == DEEPCRAFTModel.VA_EVENT_READY:
        print("[HOST] VA ready and listening")

    elif event == DEEPCRAFTModel.VA_EVENT_WAKEWORD_DETECTED:
        print("[HOST] Wake-word detected!")

    elif event == DEEPCRAFTModel.VA_EVENT_INTENT:
        intent_idx = value
        print("[HOST] Intent received: index =", intent_idx)
        action = INTENT_ACTIONS.get(intent_idx)
        if action:
            action()
        else:
            print("[HOST] Unknown intent index:", intent_idx)

    elif event == DEEPCRAFTModel.VA_EVENT_TIMEOUT:
        print("[HOST] Command timeout — say the wake-word again")

    elif event == DEEPCRAFTModel.VA_EVENT_STOPPED:
        print("[HOST] VA stopped")

    elif event == DEEPCRAFTModel.VA_EVENT_ERROR:
        print("[HOST] Fatal VA error")

    else:
        print("[HOST] Unknown event:", event, "value:", value)


model.set_event_cb(on_va_event)

# ---------------------------------------------------------------------------
# Boot the target and start the VA
# ---------------------------------------------------------------------------
model.enable_target()  # powers on / boots the target processor
time.sleep_ms(500)  # allow target boot time

model.start()  # sends DEEPCRAFT_CMD_START to the target
print('\n[HOST] Say the wake-word "OK test" followed by a command:')
print("       0) Make P17_1 LOW")
print("       1) Make P17_1 HIGH\n")

# ---------------------------------------------------------------------------
# Main event loop — poll the flag set by the IPC callback
# ---------------------------------------------------------------------------
# DEEPCRAFTModel routes all incoming IPC messages to on_va_event above.
# The loop below just keeps the interpreter alive; add your own application
# logic here (networking, display updates, etc.).
while True:
    # Stop after one complete cycle (both commands seen)
    if model.state() == DEEPCRAFTModel.STATE_IDLE:
        # VA sent VA_EVENT_STOPPED — clean exit
        break

    time.sleep_ms(10)

# Cleanup
pin_out.value(1)
time.sleep_ms(20)
print("\nVA Assistant model execution stopped")
