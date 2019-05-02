#include<windows.h>
#pragma once

#define STATUS_INFO_LENGTH_MISMATCH		0xC0000004 // If NtQueryInformationProcess returns this code, the necessary size is set in the buffer
#define OBJECT_TYPE_PROCESS				5
#define OBJECT_TYPE_JOB					7
#define ProcessHandleInformation		51

// Request of all system handles
typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO {
	USHORT UniqueProcessId;
	USHORT CreatorBackTraceIndex;
	UCHAR ObjectTypeIndex;
	UCHAR HandleAttributes;
	USHORT HandleValue;
	PVOID Object;
	ULONG GrantedAccess;
}SYSTEM_HANDLE_TABLE_ENTRY_INFO, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO;
// Request of target handle 
typedef struct _PROCESS_HANDLE_TABLE_ENTRY_INFO{
	HANDLE HandleValue;
	ULONG_PTR HandleCount;
	ULONG_PTR PointerCount;
	ULONG GrantedAccess;
	ULONG ObjectTypeIndex;
	ULONG HandleAttributes;
	ULONG Reserved;
} PROCESS_HANDLE_TABLE_ENTRY_INFO, *PPROCESS_HANDLE_TABLE_ENTRY_INFO;
// Request of all system handles
typedef struct _SYSTEM_HANDLE_INFORMATION {
	ULONG NumberOfHandles;
	SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[1];
}SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;
// Request of target handle 
typedef struct _PROCESS_HANDLE_SNAPSHOT_INFORMATION{
	ULONG_PTR NumberOfHandles;
	ULONG_PTR Reserved;
	PROCESS_HANDLE_TABLE_ENTRY_INFO Handles[1];
} PROCESS_HANDLE_SNAPSHOT_INFORMATION, *PPROCESS_HANDLE_SNAPSHOT_INFORMATION;
