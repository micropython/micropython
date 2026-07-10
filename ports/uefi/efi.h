/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Nicko van Someren
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* Minimal UEFI definitions for the MicroPython port.
 *
 * Hand-rolled subset (no external dependency). Compiled with a *-unknown-windows
 * target, so the UEFI/MS calling convention is the default and EFIAPI is empty.
 * Structures pad unused slots with void* so the typed members land at the correct
 * vtable offsets. Expand as more protocols are needed.
 */
#ifndef MPY_UEFI_EFI_H
#define MPY_UEFI_EFI_H

#include <stdint.h>
#include <stddef.h>

typedef uint64_t UINTN;
typedef int64_t INTN;
typedef UINTN EFI_STATUS;
typedef void *EFI_HANDLE;
typedef void *EFI_EVENT;
typedef uint16_t CHAR16;
typedef uint8_t BOOLEAN;
typedef uint64_t UINT64;
typedef uint32_t UINT32;
typedef uint16_t UINT16;
typedef int16_t INT16;
typedef int32_t INT32;
typedef uint8_t UINT8;
#define EFIAPI
#define IN
#define OUT
#define OPTIONAL

#define EFI_SUCCESS            ((EFI_STATUS)0)
#define EFI_HIGH_BIT           ((EFI_STATUS)1 << (sizeof(EFI_STATUS) * 8 - 1))
#define EFI_ERROR(s)           (((INTN)(EFI_STATUS)(s)) < 0)
#define EFIERR(n)              (EFI_HIGH_BIT | (n))
#define EFI_BUFFER_TOO_SMALL   EFIERR(5)
#define EFI_NOT_READY          EFIERR(6)
#define EFI_OUT_OF_RESOURCES   EFIERR(9)

typedef enum {
    EfiReservedMemoryType, EfiLoaderCode, EfiLoaderData, EfiBootServicesCode,
    EfiBootServicesData, EfiRuntimeServicesCode, EfiRuntimeServicesData,
    EfiConventionalMemory, EfiUnusableMemory, EfiACPIReclaimMemory,
    EfiACPIMemoryNVS, EfiMemoryMappedIO, EfiMemoryMappedIOPortSpace,
    EfiPalCode, EfiPersistentMemory, EfiMaxMemoryType
} EFI_MEMORY_TYPE;

typedef enum { EfiResetCold, EfiResetWarm, EfiResetShutdown, EfiResetPlatformSpecific } EFI_RESET_TYPE;

/* ---- Task Priority Levels (TPL) ---- */
typedef UINTN EFI_TPL;
#define TPL_APPLICATION 4
#define TPL_CALLBACK    8
#define TPL_NOTIFY      16
#define TPL_HIGH_LEVEL  31

/* ---- Event types (flags passed to CreateEvent) ---- */
#define EVT_TIMER                         0x80000000
#define EVT_RUNTIME                       0x40000000
#define EVT_NOTIFY_WAIT                   0x00000100
#define EVT_NOTIFY_SIGNAL                 0x00000200
#define EVT_SIGNAL_EXIT_BOOT_SERVICES     0x00000201
#define EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE 0x60000202

/* ---- Timer delay mode for SetTimer ---- */
typedef enum { TimerCancel, TimerPeriodic, TimerRelative } EFI_TIMER_DELAY;

typedef struct {
    uint64_t Signature;
    uint32_t Revision;
    uint32_t HeaderSize;
    uint32_t CRC32;
    uint32_t Reserved;
} EFI_TABLE_HEADER;

/* ---- SIMPLE_TEXT_OUTPUT (only Reset, OutputString needed) ---- */
struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;
typedef EFI_STATUS (EFIAPI *EFI_TEXT_RESET)(struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, BOOLEAN Ext);
typedef EFI_STATUS (EFIAPI *EFI_TEXT_STRING)(struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, CHAR16 *String);
typedef EFI_STATUS (EFIAPI *EFI_TEXT_QUERY_MODE)(struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    UINTN ModeNumber, UINTN *Columns, UINTN *Rows);
typedef EFI_STATUS (EFIAPI *EFI_TEXT_SET_CURSOR_POSITION)(struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    UINTN Column, UINTN Row);
typedef EFI_STATUS (EFIAPI *EFI_TEXT_CLEAR_SCREEN)(struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This);
// Live console state; CursorColumn/CursorRow track where OutputString left the cursor.
typedef struct {
    INT32 MaxMode;
    INT32 Mode;
    INT32 Attribute;
    INT32 CursorColumn;
    INT32 CursorRow;
    BOOLEAN CursorVisible;
} SIMPLE_TEXT_OUTPUT_MODE;
typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
    EFI_TEXT_RESET Reset;
    EFI_TEXT_STRING OutputString;
    void *TestString;
    EFI_TEXT_QUERY_MODE QueryMode;
    void *SetMode;
    void *SetAttribute;
    EFI_TEXT_CLEAR_SCREEN ClearScreen;
    EFI_TEXT_SET_CURSOR_POSITION SetCursorPosition;
    void *EnableCursor;
    SIMPLE_TEXT_OUTPUT_MODE *Mode;
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

/* ---- SIMPLE_TEXT_INPUT ---- */
typedef struct { UINT16 ScanCode;
                 CHAR16 UnicodeChar;
} EFI_INPUT_KEY;

/* EFI_INPUT_KEY.ScanCode values for non-text keys (UnicodeChar == 0). */
#define SCAN_NULL      0x0000
#define SCAN_UP        0x0001
#define SCAN_DOWN      0x0002
#define SCAN_RIGHT     0x0003
#define SCAN_LEFT      0x0004
#define SCAN_HOME      0x0005
#define SCAN_END       0x0006
#define SCAN_INSERT    0x0007
#define SCAN_DELETE    0x0008
#define SCAN_PAGE_UP   0x0009
#define SCAN_PAGE_DOWN 0x000A
#define SCAN_ESC       0x0017
struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL;
typedef EFI_STATUS (EFIAPI *EFI_INPUT_RESET)(struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This, BOOLEAN Ext);
typedef EFI_STATUS (EFIAPI *EFI_INPUT_READ_KEY)(struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This, EFI_INPUT_KEY *Key);
typedef struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL {
    EFI_INPUT_RESET Reset;
    EFI_INPUT_READ_KEY ReadKeyStroke;
    EFI_EVENT WaitForKey;
} EFI_SIMPLE_TEXT_INPUT_PROTOCOL;

/* ---- EFI_SERIAL_IO_PROTOCOL (byte-clean serial, bypassing TerminalDxe) ---- */
#define EFI_SERIAL_IO_PROTOCOL_GUID \
    {0xBB25CF6F, 0xF1D4, 0x11D2, {0x9A, 0x0C, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0xFD}}
#define EFI_SERIAL_INPUT_BUFFER_EMPTY 0x00000004   /* GetControl bit: no RX data */
typedef struct _EFI_SERIAL_IO_PROTOCOL EFI_SERIAL_IO_PROTOCOL;
struct _EFI_SERIAL_IO_PROTOCOL {
    UINT32 Revision;
    EFI_STATUS(EFIAPI * Reset)(EFI_SERIAL_IO_PROTOCOL * This);
    /* enum params typed as UINT32 — we only call Write/Read/GetControl. */
    EFI_STATUS(EFIAPI * SetAttributes)(EFI_SERIAL_IO_PROTOCOL * This, UINT64 BaudRate,
        UINT32 ReceiveFifoDepth, UINT32 Timeout, UINT32 Parity, UINT8 DataBits, UINT32 StopBits);
    EFI_STATUS(EFIAPI * SetControl)(EFI_SERIAL_IO_PROTOCOL * This, UINT32 Control);
    EFI_STATUS(EFIAPI * GetControl)(EFI_SERIAL_IO_PROTOCOL * This, UINT32 *Control);
    EFI_STATUS(EFIAPI * Write)(EFI_SERIAL_IO_PROTOCOL * This, UINTN *BufferSize, void *Buffer);
    EFI_STATUS(EFIAPI * Read)(EFI_SERIAL_IO_PROTOCOL * This, UINTN *BufferSize, void *Buffer);
    void *Mode;
};

// EFI_TIMER_ARCH_PROTOCOL (PI spec): the DXE core's low-level periodic tick that drives all
// BootServices timer events. We only Get/SetTimerPeriod (100ns units) to make the tick finer
// than the ~10ms firmware default; RegisterHandler/GenerateSoftInterrupt are typed as void*
// and MUST NOT be called (the handler slot is the DXE core's, single-owner).
#define EFI_TIMER_ARCH_PROTOCOL_GUID \
    {0x26baccb3, 0x6f42, 0x11d4, {0xbc, 0xe7, 0x00, 0x80, 0xc7, 0x3c, 0x88, 0x81}}
typedef struct _EFI_TIMER_ARCH_PROTOCOL EFI_TIMER_ARCH_PROTOCOL;
struct _EFI_TIMER_ARCH_PROTOCOL {
    void *RegisterHandler;
    EFI_STATUS(EFIAPI * SetTimerPeriod)(EFI_TIMER_ARCH_PROTOCOL * This, UINT64 TimerPeriod);
    EFI_STATUS(EFIAPI * GetTimerPeriod)(EFI_TIMER_ARCH_PROTOCOL * This, UINT64 *TimerPeriod);
    void *GenerateSoftInterrupt;
};

/* ---- BOOT SERVICES (typed: pool/pages/memmap/stall/exit/watchdog/wait) ---- */
typedef uint64_t EFI_PHYSICAL_ADDRESS;
typedef uint64_t EFI_VIRTUAL_ADDRESS;
#define EFI_PAGE_SIZE 4096

typedef struct {
    UINT32 Type;                       // EFI_MEMORY_TYPE (NumberOfPages at +24 after pad)
    EFI_PHYSICAL_ADDRESS PhysicalStart;
    EFI_VIRTUAL_ADDRESS VirtualStart;
    UINT64 NumberOfPages;
    UINT64 Attribute;
} EFI_MEMORY_DESCRIPTOR;

typedef enum { AllocateAnyPages, AllocateMaxAddress, AllocateAddress, MaxAllocateType } EFI_ALLOCATE_TYPE;
typedef EFI_STATUS (EFIAPI *EFI_ALLOCATE_PAGES)(EFI_ALLOCATE_TYPE Type, EFI_MEMORY_TYPE MemoryType,
    UINTN Pages, EFI_PHYSICAL_ADDRESS *Memory);
typedef EFI_STATUS (EFIAPI *EFI_FREE_PAGES)(EFI_PHYSICAL_ADDRESS Memory, UINTN Pages);
typedef EFI_STATUS (EFIAPI *EFI_GET_MEMORY_MAP)(UINTN *MemoryMapSize, EFI_MEMORY_DESCRIPTOR *MemoryMap,
    UINTN *MapKey, UINTN *DescriptorSize, UINT32 *DescriptorVersion);
typedef EFI_STATUS (EFIAPI *EFI_ALLOCATE_POOL)(EFI_MEMORY_TYPE PoolType, UINTN Size, void **Buffer);
typedef EFI_STATUS (EFIAPI *EFI_FREE_POOL)(void *Buffer);
typedef EFI_STATUS (EFIAPI *EFI_WAIT_FOR_EVENT)(UINTN NumberOfEvents, EFI_EVENT *Event, UINTN *Index);
typedef EFI_STATUS (EFIAPI *EFI_STALL)(UINTN Microseconds);
typedef EFI_STATUS (EFIAPI *EFI_SET_WATCHDOG_TIMER)(UINTN Timeout, UINT64 Code, UINTN Size, CHAR16 *Data);
typedef EFI_STATUS (EFIAPI *EFI_GET_NEXT_MONOTONIC_COUNT)(UINT64 *Count);
typedef EFI_STATUS (EFIAPI *EFI_EXIT)(EFI_HANDLE Image, EFI_STATUS Status, UINTN Size, CHAR16 *Data);
typedef EFI_STATUS (EFIAPI *EFI_LOCATE_PROTOCOL)(void *Protocol, void *Registration, void **Interface);

/* Image loading / driver binding. DevicePath / RemainingDevicePath are raw
 * EFI_DEVICE_PATH_PROTOCOL pointers (this port treats device paths as opaque
 * byte buffers, so they are typed void*). DriverImageHandle is a NULL-terminated
 * array of driver image handles (or NULL to let the firmware choose). */
typedef EFI_STATUS (EFIAPI *EFI_IMAGE_LOAD)(BOOLEAN BootPolicy, EFI_HANDLE ParentImageHandle,
    void *DevicePath, void *SourceBuffer, UINTN SourceSize, EFI_HANDLE *ImageHandle);
typedef EFI_STATUS (EFIAPI *EFI_IMAGE_START)(EFI_HANDLE ImageHandle, UINTN *ExitDataSize, CHAR16 **ExitData);
typedef EFI_STATUS (EFIAPI *EFI_IMAGE_UNLOAD)(EFI_HANDLE ImageHandle);
typedef EFI_STATUS (EFIAPI *EFI_CONNECT_CONTROLLER)(EFI_HANDLE ControllerHandle,
    EFI_HANDLE *DriverImageHandle, void *RemainingDevicePath, BOOLEAN Recursive);
typedef EFI_STATUS (EFIAPI *EFI_DISCONNECT_CONTROLLER)(EFI_HANDLE ControllerHandle,
    EFI_HANDLE DriverImageHandle, EFI_HANDLE ChildHandle);

/* Event services. EFI_EVENT_NOTIFY runs at raised TPL — treat it as a hard ISR:
 * no heap/VM access, only async-safe work (see README.md). */
typedef void (EFIAPI *EFI_EVENT_NOTIFY)(EFI_EVENT Event, void *Context);
typedef EFI_TPL (EFIAPI *EFI_RAISE_TPL)(EFI_TPL NewTpl);
typedef void (EFIAPI *EFI_RESTORE_TPL)(EFI_TPL OldTpl);
typedef EFI_STATUS (EFIAPI *EFI_CREATE_EVENT)(UINT32 Type, EFI_TPL NotifyTpl,
    EFI_EVENT_NOTIFY NotifyFunction, void *NotifyContext, EFI_EVENT *Event);
typedef EFI_STATUS (EFIAPI *EFI_SET_TIMER)(EFI_EVENT Event, EFI_TIMER_DELAY Type, UINT64 TriggerTime);
typedef EFI_STATUS (EFIAPI *EFI_SIGNAL_EVENT)(EFI_EVENT Event);
typedef EFI_STATUS (EFIAPI *EFI_CLOSE_EVENT)(EFI_EVENT Event);
typedef EFI_STATUS (EFIAPI *EFI_CHECK_EVENT)(EFI_EVENT Event);

/* ---- GUIDs + protocol lookup ---- */
typedef struct { UINT32 Data1;
                 UINT16 Data2;
                 UINT16 Data3;
                 UINT8 Data4[8];
} EFI_GUID;
typedef EFI_STATUS (EFIAPI *EFI_HANDLE_PROTOCOL)(EFI_HANDLE Handle, EFI_GUID *Protocol, void **Interface);

/* Handle-database services used by the `uefi.raw` low-level module. */
typedef enum { AllHandles, ByRegisterNotify, ByProtocol } EFI_LOCATE_SEARCH_TYPE;
typedef EFI_STATUS (EFIAPI *EFI_LOCATE_HANDLE_BUFFER)(EFI_LOCATE_SEARCH_TYPE SearchType,
    EFI_GUID *Protocol, void *SearchKey, UINTN *NoHandles, EFI_HANDLE **Buffer);
typedef EFI_STATUS (EFIAPI *EFI_OPEN_PROTOCOL)(EFI_HANDLE Handle, EFI_GUID *Protocol,
    void **Interface, EFI_HANDLE AgentHandle, EFI_HANDLE ControllerHandle, UINT32 Attributes);
typedef EFI_STATUS (EFIAPI *EFI_PROTOCOLS_PER_HANDLE)(EFI_HANDLE Handle,
    EFI_GUID ***ProtocolBuffer, UINTN *ProtocolBufferCount);
typedef EFI_STATUS (EFIAPI *EFI_CLOSE_PROTOCOL)(EFI_HANDLE Handle, EFI_GUID *Protocol,
    EFI_HANDLE AgentHandle, EFI_HANDLE ControllerHandle);
typedef struct {
    EFI_HANDLE AgentHandle;
    EFI_HANDLE ControllerHandle;
    UINT32 Attributes;
    UINT32 OpenCount;
} EFI_OPEN_PROTOCOL_INFORMATION_ENTRY;
typedef EFI_STATUS (EFIAPI *EFI_OPEN_PROTOCOL_INFORMATION)(EFI_HANDLE Handle, EFI_GUID *Protocol,
    EFI_OPEN_PROTOCOL_INFORMATION_ENTRY **EntryBuffer, UINTN *EntryCount);
typedef enum { EFI_NATIVE_INTERFACE } EFI_INTERFACE_TYPE;
typedef EFI_STATUS (EFIAPI *EFI_INSTALL_PROTOCOL_INTERFACE)(EFI_HANDLE *Handle, EFI_GUID *Protocol,
    EFI_INTERFACE_TYPE InterfaceType, void *Interface);
typedef EFI_STATUS (EFIAPI *EFI_UNINSTALL_PROTOCOL_INTERFACE)(EFI_HANDLE Handle, EFI_GUID *Protocol,
    void *Interface);
typedef EFI_STATUS (EFIAPI *EFI_REGISTER_PROTOCOL_NOTIFY)(EFI_GUID *Protocol, EFI_EVENT Event,
    void **Registration);
typedef EFI_STATUS (EFIAPI *EFI_LOCATE_HANDLE)(EFI_LOCATE_SEARCH_TYPE SearchType, EFI_GUID *Protocol,
    void *SearchKey, UINTN *BufferSize, EFI_HANDLE *Buffer);

typedef struct {
    UINT16 Year;
    UINT8 Month, Day, Hour, Minute, Second, Pad1;
    UINT32 Nanosecond;
    INT16 TimeZone;
    UINT8 Daylight, Pad2;                                    // TimeZone is INT16 (16 bytes total)
} EFI_TIME;

#define EFI_LOADED_IMAGE_PROTOCOL_GUID \
    {0x5b1b31a1, 0x9562, 0x11d2, {0x8e, 0x3f, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}}
#define EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID \
    {0x964e5b22, 0x6459, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}}
#define EFI_FILE_INFO_ID \
    {0x09576e92, 0x6d3f, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}}
#define EFI_FILE_SYSTEM_INFO_ID \
    {0x09576e93, 0x6d3f, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}}

/* ---- EFI_LOADED_IMAGE_PROTOCOL (we need DeviceHandle: the boot volume) ---- */
typedef struct {
    UINT32 Revision;
    EFI_HANDLE ParentHandle;
    void *SystemTable;
    EFI_HANDLE DeviceHandle; // the volume our .efi was loaded from
    void *FilePath;
    void *Reserved;
    UINT32 LoadOptionsSize;
    void *LoadOptions;
    void *ImageBase;
    UINT64 ImageSize;
    EFI_MEMORY_TYPE ImageCodeType;
    EFI_MEMORY_TYPE ImageDataType;
    void *Unload;
} EFI_LOADED_IMAGE_PROTOCOL;

/* ---- EFI_FILE_PROTOCOL + SIMPLE_FILE_SYSTEM ---- */
struct _EFI_FILE_PROTOCOL;
typedef struct _EFI_FILE_PROTOCOL EFI_FILE_PROTOCOL;
typedef struct _EFI_FILE_PROTOCOL {
    UINT64 Revision;
    EFI_STATUS(EFIAPI * Open)(EFI_FILE_PROTOCOL * This, EFI_FILE_PROTOCOL **NewHandle, CHAR16 *FileName, UINT64 OpenMode, UINT64 Attributes);
    EFI_STATUS(EFIAPI * Close)(EFI_FILE_PROTOCOL * This);
    EFI_STATUS(EFIAPI * Delete)(EFI_FILE_PROTOCOL * This);
    EFI_STATUS(EFIAPI * Read)(EFI_FILE_PROTOCOL * This, UINTN *BufferSize, void *Buffer);
    EFI_STATUS(EFIAPI * Write)(EFI_FILE_PROTOCOL * This, UINTN *BufferSize, void *Buffer);
    EFI_STATUS(EFIAPI * GetPosition)(EFI_FILE_PROTOCOL * This, UINT64 *Position);
    EFI_STATUS(EFIAPI * SetPosition)(EFI_FILE_PROTOCOL * This, UINT64 Position);
    EFI_STATUS(EFIAPI * GetInfo)(EFI_FILE_PROTOCOL * This, EFI_GUID *InformationType, UINTN *BufferSize, void *Buffer);
    EFI_STATUS(EFIAPI * SetInfo)(EFI_FILE_PROTOCOL * This, EFI_GUID *InformationType, UINTN BufferSize, void *Buffer);
    EFI_STATUS(EFIAPI * Flush)(EFI_FILE_PROTOCOL * This);
} EFI_FILE_PROTOCOL;

struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;
typedef struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;
typedef struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL {
    UINT64 Revision;
    EFI_STATUS(EFIAPI * OpenVolume)(EFI_SIMPLE_FILE_SYSTEM_PROTOCOL * This, EFI_FILE_PROTOCOL **Root);
} EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

/* EFI_FILE_INFO (variable-length FileName tail) */
typedef struct {
    UINT64 Size;
    UINT64 FileSize;
    UINT64 PhysicalSize;
    EFI_TIME CreateTime;
    EFI_TIME LastAccessTime;
    EFI_TIME ModificationTime;
    UINT64 Attribute;
    CHAR16 FileName[1];
} EFI_FILE_INFO;

typedef struct {
    UINT64 Size;
    BOOLEAN ReadOnly;
    UINT64 VolumeSize;
    UINT64 FreeSpace;
    UINT32 BlockSize;
    CHAR16 VolumeLabel[1];
} EFI_FILE_SYSTEM_INFO;

/* Open modes / attributes */
#define EFI_FILE_MODE_READ   0x0000000000000001ULL
#define EFI_FILE_MODE_WRITE  0x0000000000000002ULL
#define EFI_FILE_MODE_CREATE 0x8000000000000000ULL
#define EFI_FILE_READ_ONLY   0x01ULL
#define EFI_FILE_DIRECTORY   0x10ULL

#define EFI_NOT_FOUND        EFIERR(14)

typedef struct {
    EFI_TABLE_HEADER Hdr;
    EFI_RAISE_TPL RaiseTPL;
    EFI_RESTORE_TPL RestoreTPL;
    EFI_ALLOCATE_PAGES AllocatePages;
    EFI_FREE_PAGES FreePages;
    EFI_GET_MEMORY_MAP GetMemoryMap;
    EFI_ALLOCATE_POOL AllocatePool;
    EFI_FREE_POOL FreePool;
    EFI_CREATE_EVENT CreateEvent;
    EFI_SET_TIMER SetTimer;
    EFI_WAIT_FOR_EVENT WaitForEvent;
    EFI_SIGNAL_EVENT SignalEvent;
    EFI_CLOSE_EVENT CloseEvent;
    EFI_CHECK_EVENT CheckEvent;
    EFI_INSTALL_PROTOCOL_INTERFACE InstallProtocolInterface;
    void *ReinstallProtocolInterface;
    EFI_UNINSTALL_PROTOCOL_INTERFACE UninstallProtocolInterface;
    EFI_HANDLE_PROTOCOL HandleProtocol;
    void *Reserved;
    EFI_REGISTER_PROTOCOL_NOTIFY RegisterProtocolNotify;
    EFI_LOCATE_HANDLE LocateHandle;
    void *LocateDevicePath;
    void *InstallConfigurationTable;
    EFI_IMAGE_LOAD LoadImage;
    EFI_IMAGE_START StartImage;
    EFI_EXIT Exit;
    EFI_IMAGE_UNLOAD UnloadImage;
    void *ExitBootServices;
    EFI_GET_NEXT_MONOTONIC_COUNT GetNextMonotonicCount;
    EFI_STALL Stall;
    EFI_SET_WATCHDOG_TIMER SetWatchdogTimer;
    EFI_CONNECT_CONTROLLER ConnectController;
    EFI_DISCONNECT_CONTROLLER DisconnectController;
    EFI_OPEN_PROTOCOL OpenProtocol;
    EFI_CLOSE_PROTOCOL CloseProtocol;
    EFI_OPEN_PROTOCOL_INFORMATION OpenProtocolInformation;
    EFI_PROTOCOLS_PER_HANDLE ProtocolsPerHandle;
    EFI_LOCATE_HANDLE_BUFFER LocateHandleBuffer;
    EFI_LOCATE_PROTOCOL LocateProtocol;
    void *InstallMultipleProtocolInterfaces;
    void *UninstallMultipleProtocolInterfaces;
    void *CalculateCrc32;
    void *CopyMem;
    void *SetMem;
    void *CreateEventEx;
} EFI_BOOT_SERVICES;

/* ---- RUNTIME SERVICES (typed: GetTime, variable services, ResetSystem) ---- */
typedef EFI_STATUS (EFIAPI *EFI_GET_TIME)(EFI_TIME *Time, void *Capabilities);
typedef EFI_STATUS (EFIAPI *EFI_SET_TIME)(EFI_TIME *Time);
typedef void (EFIAPI *EFI_RESET_SYSTEM)(EFI_RESET_TYPE Type, EFI_STATUS Status, UINTN Size, void *Data);
typedef EFI_STATUS (EFIAPI *EFI_GET_VARIABLE)(CHAR16 *VariableName, EFI_GUID *VendorGuid,
    UINT32 *Attributes, UINTN *DataSize, void *Data);
typedef EFI_STATUS (EFIAPI *EFI_GET_NEXT_VARIABLE_NAME)(UINTN *VariableNameSize,
    CHAR16 *VariableName, EFI_GUID *VendorGuid);
typedef EFI_STATUS (EFIAPI *EFI_SET_VARIABLE)(CHAR16 *VariableName, EFI_GUID *VendorGuid,
    UINT32 Attributes, UINTN DataSize, void *Data);

typedef struct {
    EFI_TABLE_HEADER Hdr;
    EFI_GET_TIME GetTime;
    EFI_SET_TIME SetTime;
    void *GetWakeupTime;
    void *SetWakeupTime;
    void *SetVirtualAddressMap;
    void *ConvertPointer;
    EFI_GET_VARIABLE GetVariable;
    EFI_GET_NEXT_VARIABLE_NAME GetNextVariableName;
    EFI_SET_VARIABLE SetVariable;
    void *GetNextHighMonotonicCount;
    EFI_RESET_SYSTEM ResetSystem;
} EFI_RUNTIME_SERVICES;

typedef struct {
    EFI_TABLE_HEADER Hdr;
    CHAR16 *FirmwareVendor;
    UINT32 FirmwareRevision;
    EFI_HANDLE ConsoleInHandle;
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ConIn;
    EFI_HANDLE ConsoleOutHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut;
    EFI_HANDLE StandardErrorHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *StdErr;
    EFI_RUNTIME_SERVICES *RuntimeServices;
    EFI_BOOT_SERVICES *BootServices;
    UINTN NumberOfTableEntries;
    struct _EFI_CONFIGURATION_TABLE *ConfigurationTable;
} EFI_SYSTEM_TABLE;

/* ---- Configuration table (used to find SMBIOS for machine.unique_id) ---- */
typedef struct _EFI_CONFIGURATION_TABLE {
    EFI_GUID VendorGuid;
    void *VendorTable;
} EFI_CONFIGURATION_TABLE;

#define SMBIOS3_TABLE_GUID \
    {0xf2fd1544, 0x9794, 0x4a2c, {0x99, 0x2e, 0xe5, 0xbb, 0xcf, 0x20, 0xe3, 0x94}}

/* ---- Shell parameters: argc/argv when launched from the UEFI shell ---- */
#define EFI_SHELL_PARAMETERS_PROTOCOL_GUID \
    {0x752f3136, 0x4e16, 0x4fdc, {0xa2, 0x2a, 0xe5, 0xf4, 0x68, 0x12, 0xf4, 0xca}}
typedef struct {
    CHAR16 **Argv;   // Argv[0] is the image name; Argv[1..] the arguments
    UINTN Argc;
    void *StdIn;
    void *StdOut;
    void *StdErr;
} EFI_SHELL_PARAMETERS_PROTOCOL;
#define SMBIOS_TABLE_GUID \
    {0xeb9d2d31, 0x2d88, 0x11d3, {0x9a, 0x16, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d}}

#endif /* MPY_UEFI_EFI_H */
