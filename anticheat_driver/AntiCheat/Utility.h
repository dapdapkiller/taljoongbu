#pragma once
#include "undocument.h"

PVOID GetProcessId(WCHAR* proc_name);
PPROCESS_HANDLE_SNAPSHOT_INFORMATION GetInsideProcessHandle(HANDLE hProcess);
UCHAR IsCsrssHaveDangerPipe(PPROCESS_HANDLE_SNAPSHOT_INFORMATION phsi);
UCHAR IsCsrssPPLDisable();