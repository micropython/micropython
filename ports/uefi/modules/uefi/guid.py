# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# uefi.guid — the GUID type and a registry of well-known UEFI GUIDs.
#
# An EFI_GUID is 16 bytes in a mixed-endian layout: Data1 (u32) and Data2/Data3
# (u16) are little-endian; the final 8 bytes (Data4) are stored in order. The
# canonical text form is "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx".
#
# GUID exposes its packed form as the `bytes` attribute, which is what the
# uefi.raw functions expect (they take a 16-byte buffer).


def _pack(s):
    # Accept the canonical hyphenated form, a bare 32-hex-digit string, or either
    # wrapped in {braces}; case-insensitive, surrounding whitespace ignored. The
    # byte layout is the mixed-endian EFI_GUID: Data1/2/3 little-endian, Data4
    # (final 8 bytes) stored in order.
    t = s.strip().strip("{}").replace("-", "")
    if len(t) != 32:
        raise ValueError("bad GUID string: %r" % s)
    try:
        d1 = int(t[0:8], 16)
        d2 = int(t[8:12], 16)
        d3 = int(t[12:16], 16)
        d4 = int(t[16:20], 16)  # 2 bytes, stored big-endian
        d5 = int(t[20:32], 16)  # 6 bytes, stored big-endian
    except ValueError:
        raise ValueError("bad GUID string: %r" % s)
    return bytes(
        (
            d1 & 0xFF,
            (d1 >> 8) & 0xFF,
            (d1 >> 16) & 0xFF,
            (d1 >> 24) & 0xFF,
            d2 & 0xFF,
            (d2 >> 8) & 0xFF,
            d3 & 0xFF,
            (d3 >> 8) & 0xFF,
            (d4 >> 8) & 0xFF,
            d4 & 0xFF,
            (d5 >> 40) & 0xFF,
            (d5 >> 32) & 0xFF,
            (d5 >> 24) & 0xFF,
            (d5 >> 16) & 0xFF,
            (d5 >> 8) & 0xFF,
            d5 & 0xFF,
        )
    )


class GUID:
    # Accepts a GUID (returned as-is), 16 raw bytes, or a string — canonical
    # "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx", a bare 32-hex-digit string, or either
    # in {braces}. Every uefi API that takes a GUID coerces through here, so any of
    # these forms works anywhere a GUID is expected.
    def __init__(self, value):
        if isinstance(value, GUID):
            self.bytes = value.bytes
        elif isinstance(value, (bytes, bytearray)):
            if len(value) != 16:
                raise ValueError("GUID needs 16 bytes, got %d" % len(value))
            self.bytes = bytes(value)
        elif isinstance(value, str):
            self.bytes = _pack(value)
        else:
            raise TypeError("GUID expects a GUID, 16 bytes, or a string")

    def __bytes__(self):
        return self.bytes

    def __eq__(self, other):
        return isinstance(other, GUID) and other.bytes == self.bytes

    def __hash__(self):
        return hash(self.bytes)

    def __str__(self):
        b = self.bytes
        return "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x" % (
            b[3],
            b[2],
            b[1],
            b[0],
            b[5],
            b[4],
            b[7],
            b[6],
            b[8],
            b[9],
            b[10],
            b[11],
            b[12],
            b[13],
            b[14],
            b[15],
        )

    def __repr__(self):
        return "GUID('%s')" % self.__str__()


# ── Well-known GUIDs (trailing _GUID dropped per the naming convention) ──────
# Transcribed from the UEFI 2.x spec / EDK2 MdePkg. A typo here just makes a lookup
# silently miss (no crash), so spot-check against your firmware's headers if unsure.

# Console, input & graphics
SIMPLE_TEXT_INPUT_PROTOCOL = GUID("387477c1-69c7-11d2-8e39-00a0c969723b")
SIMPLE_TEXT_INPUT_EX_PROTOCOL = GUID("dd9e7534-7762-4698-8c14-f58517a625aa")
SIMPLE_TEXT_OUTPUT_PROTOCOL = GUID("387477c2-69c7-11d2-8e39-00a0c969723b")
SIMPLE_POINTER_PROTOCOL = GUID("31878c87-0b75-11d5-9a4f-0090273fc14d")
ABSOLUTE_POINTER_PROTOCOL = GUID("8d59d32b-c655-4ae9-9b15-f25904992a43")
GRAPHICS_OUTPUT_PROTOCOL = GUID("9042a9de-23dc-4a38-96fb-7aded080516a")
EDID_DISCOVERED_PROTOCOL = GUID("1c0c34f6-d380-41fa-a049-8ad06c1a66aa")
EDID_ACTIVE_PROTOCOL = GUID("bd8c1056-9f36-44ec-92a8-a6337f817986")
SERIAL_IO_PROTOCOL = GUID("bb25cf6f-f1d4-11d2-9a0c-0090273fc1fd")

# Storage & filesystem
BLOCK_IO_PROTOCOL = GUID("964e5b21-6459-11d2-8e39-00a0c969723b")
BLOCK_IO2_PROTOCOL = GUID("a77b2472-e282-4e9f-a245-c2c0e27bbcc1")
DISK_IO_PROTOCOL = GUID("ce345171-ba0b-11d2-8e4f-00a0c969723b")
DISK_IO2_PROTOCOL = GUID("151c8eae-7f2c-472c-9e54-9828194f6a88")
SIMPLE_FILE_SYSTEM_PROTOCOL = GUID("964e5b22-6459-11d2-8e39-00a0c969723b")
PARTITION_INFO_PROTOCOL = GUID("8cf2f62c-bc9b-4821-808d-ec9ec421a1a0")
ERASE_BLOCK_PROTOCOL = GUID("95a9a93e-a86e-4926-aaef-9919e775940b")
NVME_PASS_THRU_PROTOCOL = GUID("52c78312-8edc-4233-98f2-1a1aa5e388a5")
ATA_PASS_THRU_PROTOCOL = GUID("1d3de7f0-0807-424f-aa69-11a54e19a46f")
EXT_SCSI_PASS_THRU_PROTOCOL = GUID("143b7632-b81b-4cb7-abd3-b625a5b9bffe")
FILE_INFO = GUID("09576e92-6d3f-11d2-8e39-00a0c969723b")
FILE_SYSTEM_INFO = GUID("09576e93-6d3f-11d2-8e39-00a0c969723b")

# Buses & devices
DEVICE_PATH_PROTOCOL = GUID("09576e91-6d3f-11d2-8e39-00a0c969723b")
DEVICE_PATH_UTILITIES_PROTOCOL = GUID("0379be4e-d706-437d-b037-edb82fb772a4")
PCI_IO_PROTOCOL = GUID("4cf5b200-68b8-4ca5-9eec-b23e3f50029a")
PCI_ROOT_BRIDGE_IO_PROTOCOL = GUID("2f707ebb-4a1a-11d4-9a38-0090273fc14d")
USB_IO_PROTOCOL = GUID("2b2f68d6-0cd2-44cf-8e8b-bba20b1b5b75")
USB2_HC_PROTOCOL = GUID("3e745226-9818-45b6-a2ac-d7cd0e8ba2bc")

# Image & driver model (the GUIDs `dh`/`drivers`/`devices` key on)
LOADED_IMAGE_PROTOCOL = GUID("5b1b31a1-9562-11d2-8e3f-00a0c969723b")
LOADED_IMAGE_DEVICE_PATH_PROTOCOL = GUID("bc62157e-3e33-4fec-9920-2d3b36d750df")
DRIVER_BINDING_PROTOCOL = GUID("18a031ab-b443-4d1a-a5c0-0c09261e9f71")
COMPONENT_NAME_PROTOCOL = GUID("107a772c-d5e1-11d4-9a46-0090273fc14d")
COMPONENT_NAME2_PROTOCOL = GUID("6a7a5cff-e8d9-4f70-bada-75ab3025ce14")
DRIVER_DIAGNOSTICS2_PROTOCOL = GUID("4d330321-025f-4aac-90d8-5ed900173b63")
PLATFORM_DRIVER_OVERRIDE_PROTOCOL = GUID("6b30c738-a391-11d4-9a3b-0090273fc14d")
BUS_SPECIFIC_DRIVER_OVERRIDE_PROTOCOL = GUID("3bc1b285-8a15-4a82-aabf-4d7d13fb3265")
DEVICE_PATH_TO_TEXT_PROTOCOL = GUID("8b843e20-8132-4852-90cc-551a4e4a7f1c")
DEVICE_PATH_FROM_TEXT_PROTOCOL = GUID("05c99a21-c70f-4ad2-8a5f-35df3343f51e")

# Network stack (UEFI 2.x). Each xxx_SERVICE_BINDING_PROTOCOL spawns child
# instances of the matching protocol via CreateChild.
SIMPLE_NETWORK_PROTOCOL = GUID("a19832b9-ac25-11d3-9a2d-0090273fc14d")
NETWORK_INTERFACE_IDENTIFIER_PROTOCOL = GUID("1aced566-76ed-4218-bc81-767f1f977a89")
PXE_BASE_CODE_PROTOCOL = GUID("03c4e603-ac28-11d3-9a2d-0090273fc14d")
MANAGED_NETWORK_PROTOCOL = GUID("7ab33a91-ace5-4326-b572-e7ee33d39f16")
MANAGED_NETWORK_SERVICE_BINDING_PROTOCOL = GUID("f36ff770-a7e1-42cf-9ed2-56f0f271f44c")
ARP_PROTOCOL = GUID("f4ccbfb7-f6c0-4f50-9aa9-92f0596e9b0e")
ARP_SERVICE_BINDING_PROTOCOL = GUID("f44c00ee-1f2c-4a00-aa09-1c9f3e0800a3")
VLAN_CONFIG_PROTOCOL = GUID("9e23d768-d2f3-4366-9fc3-3a7aba864374")
DHCP4_PROTOCOL = GUID("8a219718-4ef5-4761-91c8-c0f04bda9e56")
DHCP4_SERVICE_BINDING_PROTOCOL = GUID("9d9a39d8-bd42-4a73-a4d5-8ee94be11380")
DHCP6_PROTOCOL = GUID("87c8bad7-0595-4053-8297-dede395f5d5b")
DHCP6_SERVICE_BINDING_PROTOCOL = GUID("9fb9a8a1-2f4a-43a6-889c-d0f7b6c47ad5")
IP4_PROTOCOL = GUID("41d94cd2-35b6-455a-8258-d4e51334aadd")
IP4_SERVICE_BINDING_PROTOCOL = GUID("c51711e7-b4bf-404a-bfb8-0a048ef1ffe4")
IP4_CONFIG2_PROTOCOL = GUID("5b446ed1-e30b-4faa-871a-3654eca36080")
IP6_PROTOCOL = GUID("2c8759d5-5c2d-66ef-925f-b66c101957e2")
IP6_SERVICE_BINDING_PROTOCOL = GUID("ec835dd3-fe0f-617b-a621-b350c3e13388")
IP6_CONFIG_PROTOCOL = GUID("937fe521-95ae-4d1a-8929-48bcd90ad31a")
UDP4_PROTOCOL = GUID("3ad9df29-4501-478d-b1f8-7f7fe70e50f3")
UDP4_SERVICE_BINDING_PROTOCOL = GUID("83f01464-99bd-45e5-b383-af6305d8e9e6")
UDP6_PROTOCOL = GUID("4f948815-b4b9-43cb-8a33-90e060b34955")
UDP6_SERVICE_BINDING_PROTOCOL = GUID("66ed4721-3c98-4d3e-81e3-d03dd39a7254")
TCP4_PROTOCOL = GUID("65530bc7-a359-410f-b010-5aadc7ec2b62")
TCP4_SERVICE_BINDING_PROTOCOL = GUID("00720665-67eb-4a99-baf7-d3c33a1c7cc9")
TCP6_PROTOCOL = GUID("46e44855-bd60-4ab7-ab0d-a679b9447d77")
TCP6_SERVICE_BINDING_PROTOCOL = GUID("ec20eb79-6c1a-4664-9a0d-d2e4cc16d664")
MTFTP4_PROTOCOL = GUID("78247c57-63db-4708-99c2-a8b4a9a61f6b")
MTFTP4_SERVICE_BINDING_PROTOCOL = GUID("2fe800be-8f01-4aa6-946b-d71388e1833f")
DNS4_PROTOCOL = GUID("ae3d28cc-e05b-4fa1-a011-7eb55a3f1401")
DNS4_SERVICE_BINDING_PROTOCOL = GUID("b625b186-e063-44f7-8905-6a74dc6f52b4")
DNS6_PROTOCOL = GUID("ca37bc1f-a327-4ae9-828a-8c40d8506a17")
DNS6_SERVICE_BINDING_PROTOCOL = GUID("7f1647c8-b76e-44b2-a565-f70ff19cd19e")
HTTP_PROTOCOL = GUID("7a59b29b-910b-4171-8242-a85a0df25b5b")
HTTP_SERVICE_BINDING_PROTOCOL = GUID("bdc8e6af-d9bc-4379-a72a-e0c4e75dae1c")
TLS_PROTOCOL = GUID("1682fe44-bd7a-4407-b7c7-dca37ca3922d")
TLS_SERVICE_BINDING_PROTOCOL = GUID("952cb795-ff36-48cf-a249-4df486d6ab8d")

# Security & crypto
RNG_PROTOCOL = GUID("3152bca5-eade-433d-862e-c01cdc291f44")
HASH2_PROTOCOL = GUID("55b1d734-c5e1-49db-9647-b16afb0e305b")

# HII (forms / strings / runtime config)
HII_DATABASE_PROTOCOL = GUID("ef9fc172-a1b2-4693-b327-6d32fc416042")
HII_STRING_PROTOCOL = GUID("0fd96974-23aa-4cdc-b9cb-98d17750322a")
HII_CONFIG_ROUTING_PROTOCOL = GUID("587e72d7-cc50-4f79-8209-ca291fc1a10f")
HII_CONFIG_ACCESS_PROTOCOL = GUID("330d4706-f2a0-4e4f-a369-b66fa8d54385")

# Shell
SHELL_PROTOCOL = GUID("6302d008-7f9b-4f30-87ac-60c9fef5da4e")
SHELL_PARAMETERS_PROTOCOL = GUID("752f3136-4e16-4fdc-a22a-e5f46812f4ca")

# Misc services
TIMESTAMP_PROTOCOL = GUID("afbfde41-2e6e-4262-ba65-62b9236e5495")
UNICODE_COLLATION2_PROTOCOL = GUID("a4c751fc-23ae-4c3e-92e9-4964cf63f349")

# Variable namespaces & policy
GLOBAL_VARIABLE = GUID("8be4df61-93ca-11d2-aa0d-00e098032b8c")
VARIABLE_POLICY_PROTOCOL = GUID("81d1675c-86f6-48df-bd95-9a6e4f0925c3")

# Configuration tables (found via the System Table, not LocateProtocol)
ACPI_TABLE_GUID = GUID("eb9d2d30-2d88-11d3-9a16-0090273fc14d")
ACPI_20_TABLE_GUID = GUID("8868e871-e4f1-11d3-bc22-0080c73c8881")
SMBIOS_TABLE_GUID = GUID("eb9d2d31-2d88-11d3-9a16-0090273fc14d")
SMBIOS3_TABLE_GUID = GUID("f2fd1544-9794-4a2c-992e-e5bbcf20e394")
MPS_TABLE_GUID = GUID("eb9d2d2f-2d88-11d3-9a16-0090273fc14d")
DTB_TABLE_GUID = GUID("b1b621d5-f19c-41a5-830b-d9152c69aae0")


# ── Registry introspection ───────────────────────────────────────────────────
def registry():
    """{name: GUID} for every well-known GUID defined in this module."""
    return {k: v for k, v in globals().items() if isinstance(v, GUID)}


def name_for(value):
    """The registry name for a GUID (e.g. 'BLOCK_IO_PROTOCOL'), or None if unknown.

    Handy for labelling handles/protocols in discovery output (a `dh`-style name
    lookup without needing the firmware's Component Name protocols).
    """
    g = value if isinstance(value, GUID) else GUID(value)
    for k, v in globals().items():
        if isinstance(v, GUID) and v == g:
            return k
    return None
