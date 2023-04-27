#
# definitions
#
MACHINE_RA4M1_CLICKER = "RA4M1 CLICKER with RA4M1"
MACHINE_EK_RA4M1 = "EK-RA4M1 with RA4M1"
MACHINE_EK_RA4W1 = "EK-RA4W1 with RA4W1"
MACHINE_EK_RA6M1 = "EK-RA6M1 with RA6M1"
MACHINE_EK_RA6M2 = "EK-RA6M2 with RA6M2"
SYSCLK_RA4M1_CLICKER = 48000000
SYSCLK_EK_RA4M1 = 48000000
SYSCLK_EK_RA4W1 = 48000000
SYSCLK_EK_RA6M1 = 120000000
SYSCLK_EK_RA6M2 = 120000000

#
# machine
#

import os

try:
    import machine
except:
    print("machine module is not found")
    raise SystemExit

m = os.uname().machine
f = machine.freq()

if m == MACHINE_RA4M1_CLICKER:
    if f == SYSCLK_RA4M1_CLICKER:
        print("freq: OK")
    else:
        print("freq: NG")


if m == MACHINE_EK_RA4M1:
    if f == SYSCLK_EK_RA4M1:
        print("freq: OK")
    else:
        print("freq: NG")

if m == MACHINE_EK_RA4W1:
    if f == SYSCLK_EK_RA4W1:
        print("freq: OK")
    else:
        print("freq: NG")

if m == MACHINE_EK_RA6M1:
    if f == SYSCLK_EK_RA6M1:
        print("freq: OK")
    else:
        print("freq: NG")

if m == MACHINE_EK_RA6M2:
    if f == SYSCLK_EK_RA6M2:
        print("freq: OK")
    else:
        print("freq: NG")
