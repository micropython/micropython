@micropython.asm_thumb
def getIPSR():
    mrs(r0, IPSR)


@micropython.asm_thumb
def getBASEPRI():
    mrs(r0, BASEPRI)


print(getBASEPRI())
print(getIPSR())
