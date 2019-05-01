#include <iostream>
#include <windows.h>
#include <winternl.h> // NtXXXX
#include <TlHelp32.h> // Get Process List..
#include "undocument.h"
#include "pipe.h"

#pragma comment (lib, "ntdll.lib")

#define PIPE_NAME "\\\\.\\pipe\\CheatPipe"
#define ERROR_MACRO(x) std::cout << "[DBG] Can't call " << x << " Reason: "<< GetLastError() <<  std::endl;
static int howmuch;

// TODO: GetHandleList from csrss
// -> NtQuerySystemInformation
PPROCESS_HANDLE_SNAPSHOT_INFORMATION GetInsideProcessHandle();
DWORD ImageNameToProcessId(const char* ImageName);
HANDLE GetTargetProcessHandle(PPROCESS_HANDLE_SNAPSHOT_INFORMATION phsi, DWORD pid);

BOOL ServerThread(LPVOID lParam)
{
	char proc_name[256] = { 0, };
	HANDLE hPipe, hTargetProcess;
	PPROCESS_HANDLE_SNAPSHOT_INFORMATION HandleInfo;
	DWORD TargetPid;
	DWORD read_byte = 0;

	REPLY_HEADER rep;

	hPipe = CreateNamedPipe(
		PIPE_NAME, 
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		0,
		0,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL
	);
	if (ConnectNamedPipe(hPipe, NULL)){
		while (1) {
			ReadFile(hPipe, &rep, sizeof(REPLY_HEADER), NULL, NULL);
			switch (rep.type)
			{
			case CHEAT_LIST::GetTargetHandle:
				HandleInfo = GetInsideProcessHandle();
				TargetPid = ImageNameToProcessId(rep.buffer);
				hTargetProcess = GetTargetProcessHandle(HandleInfo, TargetPid);
				REQUEST_HEADER req;
				req.type = CHEAT_LIST::GetTargetHandle;
				req.address = 0;
				req.return_value = (uint32_t)hTargetProcess;
				if (!WriteFile(hPipe, &req, sizeof(REQUEST_HEADER), NULL, NULL)) ERROR_MACRO("WriteFile");
				free(HandleInfo);
				break;
			case CHEAT_LIST::Read:
				break;
			case CHEAT_LIST::Write:
				break;
			case CHEAT_LIST::ScriptOn:
				break;
			case CHEAT_LIST::ScriptOff:
				break;
			}
		}
	}
	return 0;
}
BOOL WINAPI DllMain(HINSTANCE hinstDLL,  DWORD fdwReason, LPVOID lpReserved)  
{
	HANDLE hThread = 0;
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		hThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ServerThread, NULL, NULL, NULL);
		break;

	case DLL_THREAD_ATTACH:
		// Do thread-specific initialization.
		break;

	case DLL_THREAD_DETACH:
		CloseHandle(hThread);
		// Do thread-specific cleanup.
		break;

	case DLL_PROCESS_DETACH:
		// Perform any necessary cleanup.
		break;
	}
	Sleep(2000);
	//SwitchToThread();
	//Sleep(2000);
	return TRUE;
}
#define BASIC_BUFFER_CHECKER

PPROCESS_HANDLE_SNAPSHOT_INFORMATION GetInsideProcessHandle() {
	NTSTATUS status = STATUS_INFO_LENGTH_MISMATCH; // must intialize mismatch
	ULONG handle_len = 0;
	char* buffer = (char*)malloc(handle_len); // base

	do {
		if (status == STATUS_INFO_LENGTH_MISMATCH) {
			free(buffer);
			buffer = (char*)malloc(handle_len);
		}
	} while (status = NtQueryInformationProcess(
		GetCurrentProcess(),
		(PROCESSINFOCLASS)51,
		buffer,
		handle_len,
		&handle_len
	));
	return (PPROCESS_HANDLE_SNAPSHOT_INFORMATION)buffer;
}
DWORD ImageNameToProcessId(const char* ImageName) {
	PROCESSENTRY32 ProcessEntry;
	HANDLE hSnapShot;

	ProcessEntry.dwSize = sizeof(PROCESSENTRY32);
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (!hSnapShot){
		ERROR_MACRO("CreateToolhelp32Snapshot");
		return 0;
	}
	Process32First(hSnapShot, &ProcessEntry);
	do {
		if (!strncmp(ProcessEntry.szExeFile, ImageName, strlen(ImageName))){
			DWORD session_id;
			ProcessIdToSessionId(ProcessEntry.th32ProcessID, &session_id);
			if (session_id == 1) // check only session1
				return ProcessEntry.th32ProcessID;
		}
	} while (Process32Next(hSnapShot, &ProcessEntry));
	return 0;
}
HANDLE GetTargetProcessHandle(PPROCESS_HANDLE_SNAPSHOT_INFORMATION phsi, DWORD pid){
	for (ULONG i = 0; i < phsi->NumberOfHandles; i++){
		if (phsi->Handles[i].ObjectTypeIndex == OBJECT_TYPE_PROCESS || 
			phsi->Handles[i].ObjectTypeIndex == OBJECT_TYPE_JOB){
			if (GetProcessId(phsi->Handles[i].HandleValue) == pid)
			{
				return (HANDLE)(phsi->Handles[i].HandleValue);
			}
		}
	}
	std::cout << "can't find pid's handle" << std::endl;
	return 0;
}