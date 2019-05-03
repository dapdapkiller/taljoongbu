#pragma once
#include "undocument.h"

PVOID GetProcessId(WCHAR* proc_name);
PSYSTEM_HANDLE_INFORMATION_EX GetAllSystemHandle();
UCHAR IsCsrssHaveDangerPipe(PSYSTEM_HANDLE_INFORMATION_EX pshie, HANDLE pid);
UCHAR IsCsrssPPLDisable();