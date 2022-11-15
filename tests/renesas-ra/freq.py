#
# definitions
#
MACHINE_RA4M1_CLICKER = "RA4M1_CLICKER with RA4M1"
MACHINE_RA4M1_EK = "RA4M1_EK with RA4M1"
MACHINE_RA4W1_EK = "RA4W1_EK with RA4W1"
MACHINE_RA6M1_EK = "RA6M1_EK with RA6M1"
MACHINE_RA6M2_EK = "RA6M2_EK with RA6M2"
SYSCLK_RA4M1_CLICKER = 48000000
SYSCLK_RA4M1_EK = 48000000
SYSCLK_RA4W1_EK = 48000000
SYSCLK_RA6M1_EK = 120000000
SYSCLK_RA6M2_EK = 120000000

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


if m == MACHINE_RA4M1_EK:
    if f == SYSCLK_RA4M1_EK:
        print("freq: OK")
    else:
        print("freq: NG")

if m == MACHINE_RA4W1_EK:
    if f == SYSCLK_RA4W1_EK:
        print("freq: OK")
    else:
        print("freq: NG")

if m == MACHINE_RA6M1_EK:
    if f == SYSCLK_RA6M1_EK:
        print("freq: OK")
    else:
        print("freq: NG")

if m == MACHINE_RA6M2_EK:
    if f == SYSCLK_RA6M2_EK:
        print("freq: OK")
    else:
        print("freq: NG")
