import sys
try:
    import ffi
except ImportError:
    print("SKIP")
    sys.exit()

kernel32 = ffi.open('kernel32')
GetVersion = kernel32.func('I', 'GetVersion', 'v')

dwVersion = GetVersion(None)
dwMajorVersion = (dwVersion & 0x00FF)
dwMinorVersion = ((dwVersion & 0xFF00) >> 8)
dwBuild = 0
if dwVersion < 0x80000000:
    dwBuild  = ((dwVersion >> 16) & 0xFFFF)
print("Version is %d.%d (%d)" % (dwMajorVersion, dwMinorVersion, dwBuild))
