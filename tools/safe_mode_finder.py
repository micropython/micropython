# This uses pyocd to control a Cortex M core, set a breakpoint and dump the stack.
# It is expected that you modify it for your particular case.

from pyocd.core.helpers import ConnectHelper
from pyocd.core.target import Target
from pyocd.debug.elf.symbols import ELFSymbolProvider
from pyocd.flash.file_programmer import FileProgrammer

import logging

# logging.basicConfig(level=logging.DEBUG)

import sys
import time

board = sys.argv[1]

# Connect to the target.
with ConnectHelper.session_with_chosen_probe(target_override="nrf52840") as session:
    target = session.target

    # Set ELF file on target.
    filename = f"build-{board}/firmware.elf"
    target.elf = filename

    e = target.elf._elf

    # Look up address of reset_into_safe_mode().
    provider = ELFSymbolProvider(target.elf)
    addr = provider.get_symbol_value("reset_into_safe_mode")
    print("reset_into_safe_mode() address: 0x%X" % addr)

    for section in target.elf.sections:
        print(section)
        print()

    print("loading", filename)
    # FileProgrammer(session).program(filename, file_format="elf")
    print("load done")

    # Set breakpoint.
    target.set_breakpoint(addr)
    # target.set_watchpoint(0x20010558, 4, Target.WatchpointType.WRITE)

    # Reset and run.
    target.reset_and_halt()
    for i in range(1):
        target.resume()

        print("resuming")
        start = time.monotonic()
        # Wait 10s until breakpoint is hit.
        while target.get_state() != Target.State.HALTED and time.monotonic() - start < 10:
            pass

    #     value = target.read_memory(0x20010558)
    #     print(f"{value:08x}")

    # time.sleep(2)

    target.halt()

    # print("_sidata", hex(provider.get_symbol_value("_sidata")))

    # flash_start = 0x000affe8
    # ram_start =   0x20010000
    # for i in range(0, 0x55c, 4):
    #     flash_value = target.read_memory(flash_start + i)
    #     value = target.read_memory(ram_start + i)
    #     diff = ""
    #     if flash_value != value:
    #         diff = "*"
    #     print(f"{ram_start + i:08x} {flash_value:08x} {value:08x} {diff}")

    # Print PC.
    pc = target.read_core_register("pc")
    print("pc: 0x%X" % pc)
    print(target.elf.symbol_decoder.get_symbol_for_address(pc))
    sp = target.read_core_register("sp")
    print("sp: 0x%X" % sp)
    msp = target.read_core_register("msp")
    print("msp: 0x%X" % msp)
    psp = target.read_core_register("psp")
    print("psp: 0x%X" % psp)

    print(e.has_dwarf_info())
    d = e.get_dwarf_info()
    # print(dir(d))
    aranges = d.get_aranges()
    cu_offset = aranges.cu_offset_at_addr(pc)
    if not cu_offset:
        cu_offset = 0
    main_cu = d.get_CU_at(cu_offset)
    lines = d.line_program_for_CU(main_cu).get_entries()

    lines_by_address = {}
    for line in lines:
        if not line.state:
            # print(line)
            continue
        lines_by_address[line.state.address] = line.state
    call_frames = None
    # if d.has_CFI():
    #     print("CFI")
    #     call_frames = d.CFI_entries()
    # if d.has_EH_CFI():
    #     print("EH CFI")
    #     call_frames = d.EH_CFI_entries()
    all_frames = {}
    # for frame in call_frames:
    #     decoded = frame.get_decoded()
    #     for entry in decoded.table:
    #         entry_pc = entry["pc"]
    #         all_frames[entry_pc] = decoded
    # for r in d.range_lists().iter_range_lists():
    #     print(r)
    if pc in all_frames:
        print(all_frames[pc])
    ad = target.elf.address_decoder
    function = ad.get_function_for_address(pc)
    if function:
        print("   ", function)
    line = ad.get_line_for_address(pc)
    if line:
        print("   ", line)

    mm = target.get_memory_map()

    ram = mm.get_region_for_address(sp)
    if not ram:
        sp_guess = 0x20013BCC
        print("stack pointer bad using 0x{%08x}")
        ram = mm.get_region_for_address(sp_guess)
    stack_address = sp
    offset = 0
    while ram and sp + offset <= ram.end:
        stack_address = sp + offset
        value = target.read_memory(stack_address)
        symbol = target.elf.symbol_decoder.get_symbol_for_address(value)
        print(f"{stack_address:08x} {offset:04x} {value:08x} {symbol}")
        function = ad.get_function_for_address(value)
        if function:
            print("   ", function)
        line = ad.get_line_for_address(value)
        if line:
            print("   ", line)
        # value -= 0x27000
        # if value in all_frames:
        #     print(all_frames[value])
        # if value in lines_by_address:
        #     print(lines_by_address[value])
        offset += 4

    top_symbol = target.elf.symbol_decoder.get_symbol_for_address(target.read_memory(sp))
    if True or (top_symbol and top_symbol.name == "HardFault_Handler"):
        lr = target.read_core_register("lr")
        print("lr: 0x%08X" % lr)
        cfsr = target.read_memory(0xE000ED28)
        print("cfsr: 0x%08X" % cfsr)
        hfsr = target.read_memory(0xE000ED2C)
        print("hfsr: 0x%08X" % hfsr)
        if hfsr & 0x4000_0000:
            print("hard fault forced")
        bfsr = (cfsr >> 8) & 0xFF
        if bfsr & 0x80 != 0:
            bad_address = target.read_memory(0xE000ED38)
            print(f"bus fault with address 0x{bad_address:08x}")
        bits = ["IBUSERR", "PRECISERR", "IMPRECISERR", "UNSTKERR", "STKERR", "LSPERR"]
        for i, bit in enumerate(bits):
            if bfsr & (1 << i):
                print(bit)

    for i in range(13):
        reg = f"r{i}"
        value = target.read_core_register(reg)
        print(f"{reg} 0x{value:08x}")

    # print(hex(target.read_memory(provider.get_symbol_value("prescaler"))))

    # Remove breakpoint.
    target.remove_breakpoint(addr)
