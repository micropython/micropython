import pyb
import time

@micropython.asm_thumb
def getAPSR():
    MRS(r0, APSR)

@micropython.asm_thumb
def getIAPSR():
    MRS(r0, IAPSR)

@micropython.asm_thumb
def getEAPSR():
    MRS(r0, EAPSR)

@micropython.asm_thumb
def getXPSR():
    MRS(r0, XPSR)

@micropython.asm_thumb
def getIPSR():
    MRS(r0, IPSR)

@micropython.asm_thumb
def getEPSR():
    MRS(r0, EPSR)

@micropython.asm_thumb
def getIEPSR():
    MRS(r0, IEPSR)

@micropython.asm_thumb
def getMSP():
    MRS(r0, MSP)

@micropython.asm_thumb
def getPSP():
    MRS(r0, PSP)

@micropython.asm_thumb
def getPRIMASK():
    MRS(r0, PRIMASK)

@micropython.asm_thumb
def getBASEPRI():
    MRS(r0, BASEPRI)

@micropython.asm_thumb
def getBASEPRI_MAX():
    MRS(r0, BASEPRI_MAX)

@micropython.asm_thumb
def getFAULTMASK():
    MRS(r0, FAULTMASK)

@micropython.asm_thumb
def getCONTROL():
    MRS(r0, CONTROL)
    
@micropython.asm_thumb
def getSP():
    mov(r0, r13)

sp = getSP()
MSP = getMSP()
if sp == MSP:
    print('MSP==SP')
else:
    print('MSP!=SP')


t = pyb.Timer(4)
t.init(freq=10)

def cb(t):
    print(getIPSR())
    t.callback(None)
    
t.callback(cb)


time.sleep(1)
del(t)
