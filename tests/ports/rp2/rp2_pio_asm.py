"""function tests for pio asm without hw-access"""

import rp2


@rp2.asm_pio()
def sending_sm_code():
    pull(block)
    irq(0)
    mov(isr, osr)  # copy input -> output
    irq(clear, 0)
    push()


@rp2.asm_pio()
def receiving_sm_code():
    set(x, 10)  # set a fix value above 4 and below 31
    wait(1, irq, 0)
    label("push_x")
    mov(isr, x)
    push(block)
    jmp(x_dec, "push_x")


@rp2.asm_pio()
def fix10_sm_code():
    set(x, 10)  # set a dynamic value
    label("push_x")
    mov(isr, x)
    push(block)
    jmp(x_dec, "push_x")
    label("end")
    jmp("end")


def dynamic_sm_code(x_value):
    set(x, x_value)  # set a dynamic value
    label("push_x")
    mov(isr, x)
    push(block)
    jmp(x_dec, "push_x")
    label("end")
    jmp("end")


def test_fifo():
    """Checking the correct behaviour of the default fifo config"""
    print()
    sending_sm = rp2.StateMachine(0, sending_sm_code)
    print("Checking TX-Filling before active")
    for i in range(4):
        print(sending_sm.tx_fifo() == i)
        sending_sm.put(i)
    sending_sm.active(1)

    print("Checking tx->rx transfer")
    print(sending_sm.tx_fifo() == 0)
    print(sending_sm.rx_fifo() == 4)

    print("Checking receive order and value")
    for i in range(4):
        print(i == sending_sm.get())


def test_isr_communication():
    """Checking the communication via isr between two state machines"""
    print()
    sending_sm = rp2.StateMachine(0, sending_sm_code)
    receive_sm = rp2.StateMachine(1, receiving_sm_code)
    sending_sm.active(1)
    receive_sm.active(1)
    print("Checking initial fifo state on receive:")
    print(receive_sm.rx_fifo() == 0)  # sm wait for irq
    sending_sm.put(0)  # sending sm fires irq
    print(
        receive_sm.rx_fifo() == 4
    )  # rx-fifo full | set(x, <value>) <= assuming value always be higher than 4!
    initial_x = receive_sm.get()
    for count in range(initial_x):
        print(
            receive_sm.get() == initial_x - (count + 1)
        )  # count+1 -> first value taken for initial get


def test_sm_runtime_parameter():
    """Check the runtime parameter on pio asm"""
    print()
    asm_list = {}
    for i in [5, 10, 15]:
        asm_list[i] = rp2.asm_pio()(dynamic_sm_code, x_value=i)

    print("Check if parameter assembly leads to the same code as static")
    print(asm_list[10] == fix10_sm_code)

    for expected_x, compiled_pio in asm_list.items():
        print(f"Checking the state machine with x={expected_x}")
        dynamic_sm = rp2.StateMachine(0, compiled_pio)
        dynamic_sm.active(1)
        initial_x = dynamic_sm.get()
        print(expected_x == initial_x)
        for _ in range(initial_x - 1):
            dynamic_sm.get()
        print(f"Check if sm works as expected ({expected_x} gets() lead to 0)")
        print(dynamic_sm.get() == 0)


test_fifo()
test_isr_communication()
test_sm_runtime_parameter()
