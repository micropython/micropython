#
# definitions
#
MACHINE_RA4M1_CLICKER = "RA4M1_CLICKER with RA4M1"
MACHINE_RA4M1_EK = "RA4M1_EK with RA4M1"
MACHINE_RA4W1_EK = "RA4W1_EK with RA4W1"
MACHINE_VK_RA4W1 = "VK-RA4W1 with RA4W1"
MACHINE_RA6M1_EK = "RA6M1_EK with RA6M1"
MACHINE_RA6M2_EK = "RA6M2_EK with RA6M2"
MACHINE_VK_RA6M3 = "VK-RA6M3 with RA6M3"
MACHINE_VK_RA6M5 = "VK-RA6M5 with RA6M5"
SYSCLK_RA4M1_CLICKER = 48000000
SYSCLK_RA4M1_EK = 48000000
SYSCLK_RA4W1_EK = 48000000
SYSCLK_VK_RA4W1 = 48000000
SYSCLK_RA6M1_EK = 120000000
SYSCLK_RA6M2_EK = 120000000
SYSCLK_VK_RA6M3 = 120000000
SYSCLK_VK_RA6M5 = 200000000

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

if m == MACHINE_VK_RA4W1:
    if f == SYSCLK_VK_RA4W1:
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

if m == MACHINE_VK_RA6M3:
    if f == SYSCLK_VK_RA6M3:
        print("freq: OK")
    else:
        print("freq: NG")

if m == MACHINE_VK_RA6M5:
    if f == SYSCLK_VK_RA6M5:
        print("freq: OK")
    else:
        print("freq: NG")
