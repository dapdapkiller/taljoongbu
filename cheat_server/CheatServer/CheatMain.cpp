#include <iostream>
#include <windows.h>
#include <winternl.h> // NtXXXX
#include <TlHelp32.h> // Get Process List..
#include "undocument.h"
#include "pipe.h"

#pragma comment (lib, "ntdll.dll")

#define PIPE_NAME "\\\\.\\pipe\\mypipe"
#define ERROR_MACRO(x) std::cout << "[DBG] Can't call " << x << " Reason: "<< GetLastError() <<  std::endl;

// TODO: GetHandleList from csrss
// -> NtQuerySystemInformation
PSYSTEM_HANDLE_INFORMATION GetSystemHandleInfomation();
DWORD ImageNameToProcessId(const char* ImageName);
HANDLE GetTargetProcessHandle(PSYSTEM_HANDLE_INFORMATION pshi, DWORD pid);

BOOL ServerThread(LPVOID lParam)
{
	char proc_name[256] = { 0, };
	HANDLE hPipe, hTargetProcess;
	REQUEST_HEADER req;
	PSYSTEM_HANDLE_INFORMATION HandleInfo;
	DWORD TargetPid;

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
	if (!hPipe){
		ERROR_MACRO("CreateNamedPipe");
		return false;
	}
	while (1)
	{
		if (ConnectNamedPipe(hPipe, NULL)){
			ReadFile(hPipe, &req, sizeof(REQUEST_HEADER), NULL, NULL);
			switch (req.type)
			{
			case CHEAT_LIST::GetTargetHandle:
				if (!ReadFile(hPipe, proc_name, 256, NULL, NULL)) ERROR_MACRO("ReadFile");
				HandleInfo = GetSystemHandleInfomation();
				TargetPid = ImageNameToProcessId(proc_name);
				hTargetProcess = GetTargetProcessHandle(HandleInfo, TargetPid);
				break;
			case CHEAT_LIST::Read:
				break;
			case CHEAT_LIST::Write:
				break;
			case CHEAT_LIST::ScriptOn:
				break;
			case CHEAT_LIST::ScriptOff:
				break;
			default:
				std::cout << "Unknown command" << std::endl;
			}
			/*
			if (req.type == 1)
			{
				if (ReadProcessMemory(hProcess, (LPCVOID)req->Address, &buf, req->nRead, NULL))
				{
					std::cout << "RPM Success Sending.." << std::endl;
					memcpy(rep->buf, buf, 10);
					rep->nRead = req->nRead;
					WriteFile(hPipe, rep, sizeof(RPMRep), NULL, NULL);
				}
				else
				{
					std::cout << "RPM Failed.. " << std::endl;
				}
			}
			else
			{
				cout << "Unknown Commands" << endl;
			}
			*/
		}
		else{
			std::cout << "Waiting client.." << std::endl;
			Sleep(1000);
		}
		Sleep(10);
	}
	free(HandleInfo);
}
BOOL WINAPI DllMain(HINSTANCE hinstDLL,  DWORD fdwReason, LPVOID lpReserved)  
{
	HANDLE hThread;
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		hThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ServerThread, NULL, NULL, NULL);
		break;

	case DLL_THREAD_ATTACH:
		std::cout << "Create MyServerThread :)" << std::endl;
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
	return TRUE;
}
PSYSTEM_HANDLE_INFORMATION GetSystemHandleInfomation(){
	ULONG temp_len;
	BYTE* buffer;

	// Get length of struct
	if (!NtQuerySystemInformation(
		SYSTEM_INFORMATION_CLASS::SystemExtendedHandleInformation,
		NULL,
		NULL,
		&temp_len
	)) {
		ERROR_MACRO("Get NtQuerySystemInformation Length");
		return 0;
	}
	else {
		buffer = (BYTE*)malloc(temp_len);
	}
	if (!NtQuerySystemInformation(
		SYSTEM_INFORMATION_CLASS::SystemExtendedHandleInformation,
		(PVOID)buffer,
		temp_len,
		&temp_len
	)){
		ERROR_MACRO("Set NtQuerySystemInformation");
		return 0;
	}
	return (PSYSTEM_HANDLE_INFORMATION)buffer;
}
DWORD ImageNameToProcessId(const char* ImageName) {
	PROCESSENTRY32 ProcessEntry;
	HANDLE hSnapShot;

	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (!hSnapShot){
		ERROR_MACRO("CreateToolhelp32Snapshot");
		return 0;
	}
	Process32First(hSnapShot, &ProcessEntry);
	do {
		if (!strncmp(ProcessEntry.szExeFile, ImageName, 9))
		{
			return ProcessEntry.th32ProcessID;
		}
	} while (Process32Next(hSnapShot, &ProcessEntry));

	return 0;
}
HANDLE GetTargetProcessHandle(PSYSTEM_HANDLE_INFORMATION pshi, DWORD pid) {
	ULONG HandleCount = pshi->NumberOfHandles;
	for (ULONG i = 0; i < HandleCount; i++)
	{
		if (pshi->Handle[i].UniqueProcessId == pid)
		{
			return (HANDLE)(pshi->Handle[i].HandleValue);
		}
	}
	std::cout << "can't find pid's handle" << std::endl;
	return 0;
}