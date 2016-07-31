# This example shows how to access Video BIOS memory area via machine.mem
# It requires root privilege and x86 legacy harfware (which has mentioned
# Video BIOS at all).
# It is expected to print 0xaa55, which is a signature at the start of
# Video BIOS.

import umachine as machine

print(hex(machine.mem16[0xc0000]))
