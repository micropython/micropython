# uefi.system services: memory map, configuration tables, firmware identity, watchdog,
# monotonic counter and driver-name resolution. (RTC get/set is covered by machine_rtc.py.)
try:
    import uefi
    from uefi import system as SY, driver as D
except ImportError:
    print("SKIP")
    raise SystemExit

mkey, mm = SY.memory_map()
print(len(mm) > 0)
conv = sum(d.num_pages for d in mm if d.type == 7)  # EfiConventionalMemory
print(conv > 0)
ct = SY.configuration_tables()
print(len(ct) > 0)
print((SY.acpi_rsdp() or SY.smbios() or SY.device_tree()) != 0)  # >= 1 present on both arches
print(bool(SY.firmware_vendor()))
print(isinstance(SY.firmware_revision(), int))
SY.set_watchdog(0)  # disarm (idempotent)
c1 = SY.monotonic_count()
c2 = SY.monotonic_count()
print(c2 > c1)
dbs = uefi.handle.locate_handles(uefi.guid.DRIVER_BINDING_PROTOCOL)
print(len(dbs) > 0)
print(sum(1 for h in dbs if D.driver_name(h)) > 0)
print("system ok")
