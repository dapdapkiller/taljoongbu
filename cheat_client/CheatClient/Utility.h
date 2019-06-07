#pragma once
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <winternl.h>
#include <TlHelp32.h>
#include "pipe.h"
#include "print.h"


// #define ERROR_MACRO(x) printf("[DBG] Can't call %s Reason: %d\n",x,GetLastError())
#define ERROR_MACRO(x) println("[ERR ] Can't call " x " Reason: ", GetLastError())

HWND FindConsoleHandle();
BOOL set_privilege(LPSTR prv_name);
BOOL DriverLoader(HANDLE service_mgr, LPSTR service_name, LPSTR driver_name);
DWORD ImageNameToProcessId(const char* ImageName);
BOOL DllLoader(LPSTR dll_name, LPSTR proc_name);

// This is a function that uses a pipe
BOOL MakeSendPacket(PREQUEST_HEADER req, uint8_t type, uint32_t address, char* buffer);

// This is a function that uses a vulnerable driver
HANDLE GetDeviceHandleFromVulnDrv();
HANDLE GetProcessHandleFromVulnDrv(HANDLE hDevice, ULONG pid);
HANDLE GetThreadHandleFromVulnDrv(HANDLE hDevice, ULONG tid);
BOOL RegisterProcessToVulnDrv(HANDLE hDevice, ULONG pid);