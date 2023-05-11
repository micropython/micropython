"""Source this file into gdb `source ../../tools/cortex-m-fault-gdb.py` then run
   `cortex-m-fault` to print basic info about the fault registers."""

SCS = 0xE000E000
SCB = SCS + 0x0D00
CPUID = SCB + 0x000  # (R/ )  CPUID Base Register */
ICSR = SCB + 0x004  # (R/W)  Interrupt Control and State Register */
VTOR = SCB + 0x008  # (R/W)  Vector Table Offset Register */
AIRCR = SCB + 0x00C  # (R/W)  Application Interrupt and Reset Control Register */
SCR = SCB + 0x010  # (R/W)  System Control Register */
CCR = SCB + 0x014  # (R/W)  Configuration Control Register */
SHCSR = SCB + 0x024  # (R/W)  System Handler Control and State Register */
CFSR = SCB + 0x028  # (R/W)  Configurable Fault Status Register */
HFSR = SCB + 0x02C  # (R/W)  HardFault Status Register */
DFSR = SCB + 0x030  # (R/W)  Debug Fault Status Register */
MMFAR = SCB + 0x034  # (R/W)  MemManage Fault Address Register */
BFAR = SCB + 0x038  # (R/W)  BusFault Address Register */
AFSR = SCB + 0x03C  # (R/W)  Auxiliary Fault Status Register */

PARTS = {0xC27: "Cortex M7", 0xC60: "Cortex M0+"}

EXCEPTIONS = {
    0: "Thread mode",
    2: "Non Maskable Interrupt",
    3: "Hard Fault",
    4: "MemManage Fault",
    5: "Bus Fault",
    6: "Usage Fault",
    11: "SVCAll",
    14: "PendSV",
    15: "SysTick",
}


class CortexMFault(gdb.Command):
    def __init__(self):
        super(CortexMFault, self).__init__("cortex-m-fault", gdb.COMMAND_USER)

    def _read(self, address):
        i = gdb.selected_inferior()
        return i.read_memory(address, 4).cast("I")[0]

    def _armv6m_fault(self):
        vtor = self._read(VTOR)
        print("vtor", hex(vtor))

        icsr = self._read(ICSR)
        vectactive = icsr & 0x1FF
        print("icsr", hex(icsr), vectactive)
        if vectactive != 0:
            if vectactive in EXCEPTIONS:
                vectactive = EXCEPTIONS[vectactive]
            else:
                vectactive -= 16

            print("Active interrupt:", vectactive)

        vectpending = (icsr >> 12) & 0x1FF
        if vectpending != 0:
            if vectpending in EXCEPTIONS:
                vectpending = EXCEPTIONS[vectpending]
            else:
                vectpending -= 16

            print("Pending interrupt:", vectpending)

    def _armv7m_fault(self):
        icsr = self._read(ICSR)
        if (icsr & (1 << 11)) != 0:
            print("No preempted exceptions")
        else:
            print("Another exception was preempted")
        print("icsr", hex(icsr))
        vectactive = icsr & 0x1FF
        if vectactive != 0:
            if vectactive in EXCEPTIONS:
                print(EXCEPTIONS[vectactive])
            else:
                print(vectactive - 16)
        vectpending = (icsr >> 12) & 0x1FF
        if vectpending != 0:
            if vectpending in EXCEPTIONS:
                print(EXCEPTIONS[vectpending])
            else:
                print(vectpending - 16)

        vtor = self._read(VTOR)
        print("vtor", hex(vtor))
        # print(hex(self._read(SHCSR)))
        cfsr = self._read(CFSR)
        ufsr = cfsr >> 16
        bfsr = (cfsr >> 8) & 0xFF
        mmfsr = cfsr & 0xFF
        print("ufsr", hex(ufsr), "bfsr", hex(bfsr), "mmfsr", hex(mmfsr))
        if (bfsr & (1 << 7)) != 0:
            print("Bad address", hex(self._read(BFAR)))
        if (bfsr & (1 << 3)) != 0:
            print("Unstacking from exception error")
        if (bfsr & (1 << 2)) != 0:
            print("Imprecise data bus error")
        if (bfsr & (1 << 1)) != 0:
            print("Precise data bus error")
        if (bfsr & (1 << 0)) != 0:
            print("Instruction bus error")

        if (mmfsr & (1 << 7)) != 0:
            print("Bad address", hex(self._read(MMFAR)))
        if (mmfsr & (1 << 3)) != 0:
            print("Unstacking from exception error")
        if (mmfsr & (1 << 1)) != 0:
            print("Data access violation")
        if (mmfsr & (1 << 0)) != 0:
            print("Instruction access violation")

        if (ufsr & (1 << 8)) != 0:
            print("Unaligned access")
        if (ufsr & (1 << 0)) != 0:
            print("Undefined instruction")
        hfsr = self._read(HFSR)
        if (hfsr & (1 << 30)) != 0:
            print("Forced hard fault")
        if (hfsr & (1 << 1)) != 0:
            print("Bus fault when reading vector table")
            print("VTOR", hex(vtor))

    def invoke(self, arg, from_tty):
        cpuid = self._read(CPUID)
        implementer = cpuid >> 24
        if implementer != 0x41:
            raise RuntimeError()
        variant = (cpuid >> 20) & 0xF
        architecture = (cpuid >> 16) & 0xF
        revision = cpuid & 0xF
        part_no = (cpuid >> 4) & 0xFFF
        print(PARTS[part_no])

        if architecture == 0xF:
            self._armv7m_fault()
        elif architecture == 0xC:
            self._armv6m_fault()
        else:
            raise RuntimeError(f"Unknown architecture {architecture:x}")


CortexMFault()
