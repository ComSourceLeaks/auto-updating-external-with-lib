#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
uintptr_t virtualaddy;

#define read_write CTL_CODE(FILE_DEVICE_UNKNOWN, 0x30AD, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define base_addr CTL_CODE(FILE_DEVICE_UNKNOWN, 0x31AD, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define dtbf CTL_CODE(FILE_DEVICE_UNKNOWN, 0x32AD, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

typedef struct _rw {
    INT32 process_id;
    ULONGLONG address;
    ULONGLONG buffer;
    ULONGLONG size;
    BOOLEAN write;
} rw, * prw;

typedef struct _ba {
    INT32 process_id;
    ULONGLONG* address;
} ba, * pba;
typedef struct _dtb {
    INT32 process_id;
    bool* operation;
} dtb, * dtbl;
typedef struct _ga {
    ULONGLONG* address;
} ga, * pga;

namespace driver {
    HANDLE driver_handle;
    INT32 process_id;

    bool init() {
        driver_handle = CreateFileW(L"\\\\.\\??unicode??010101", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

        if (!driver_handle || (driver_handle == INVALID_HANDLE_VALUE))
            return false;

        return true;
    }

    void read_physical(PVOID address, PVOID buffer, DWORD size) {
        _rw arguments = { 0 };

        arguments.address = (ULONGLONG)address;
        arguments.buffer = (ULONGLONG)buffer;
        arguments.size = size;
        arguments.process_id = process_id;
        arguments.write = FALSE;

        DeviceIoControl(driver_handle, read_write, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
    }

    void write_physical(PVOID address, PVOID buffer, DWORD size) {
        _rw arguments = { 0 };

        arguments.address = (ULONGLONG)address;
        arguments.buffer = (ULONGLONG)buffer;
        arguments.size = size;
        arguments.process_id = process_id;
        arguments.write = TRUE;

        DeviceIoControl(driver_handle, read_write, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
    }

    /*void write_physical(PVOID address, PVOID buffer, DWORD size);
    void read_physical(PVOID address, PVOID buffer, DWORD size);*/

    uintptr_t get_base_address() {
        uintptr_t image_address = { NULL };
        _ba arguments = { NULL };

        arguments.process_id = process_id;
        arguments.address = (ULONGLONG*)&image_address;

        DeviceIoControl(driver_handle, base_addr, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);

        return image_address;
    }

    bool CR3() {
        bool ret = false;
        _dtb arguments = { 0 };
        arguments.process_id = process_id;
        arguments.operation = (bool*)&ret;
        DeviceIoControl(driver_handle, dtbf, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);

        return ret;
    }

    INT32 get_process_id(LPCTSTR process_name) {
        PROCESSENTRY32 pt;
        HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        pt.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hsnap, &pt)) {
            do {
                if (!lstrcmpi(pt.szExeFile, process_name)) {
                    CloseHandle(hsnap);
                    process_id = pt.th32ProcessID;
                    return pt.th32ProcessID;
                }
            } while (Process32Next(hsnap, &pt));
        }
        CloseHandle(hsnap);

        return { NULL };
    }


    template <typename T>
    T write(uint64_t address, T buffer) {
        driver::write_physical((PVOID)address, &buffer, sizeof(T));
        return buffer;
    }

    inline bool valid_check(const uint64_t adress)
    {
        if (adress <= 0x400000 || adress == 0xCCCCCCCCCCCCCCCC || reinterpret_cast<void*>(adress) == nullptr || adress >
            0x7FFFFFFFFFFFFFFF) {
            return false;
        }

        return true;
    }
}
template <typename T>
T read(uint64_t address) {
    T buffer{ };
    driver::read_physical((PVOID)address, &buffer, sizeof(T));
    return buffer;
}