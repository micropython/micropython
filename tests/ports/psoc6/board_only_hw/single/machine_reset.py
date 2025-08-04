import machine

print("*****Test :Reset Validation after a soft Reset*****")
if machine.reset_cause() == machine.SOFT_RESET:
    print("PASS")
else:
    print(machine.reset_cause())
    print("FAIL")
