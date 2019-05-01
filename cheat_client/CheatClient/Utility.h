#pragma once
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <winternl.h>
#include <TlHelp32.h>

#define ERROR_MACRO(x) printf("[DBG] Can't call %s Reason: %d\n",x,GetLastError())

HWND FindConsoleHandle();
BOOL set_privilege(LPSTR prv_name);
BOOL DriverLoader(HANDLE service_mgr, LPSTR service_name, LPSTR driver_name);
HANDLE GetPPLHandle(DWORD pid);
DWORD ImageNameToProcessId(const char* ImageName);
BOOL DllLoader(LPSTR dll_name, LPSTR proc_name);