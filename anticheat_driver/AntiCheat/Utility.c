#include "Utility.h"

// from AntiCheat.c
extern MyZwQuerySystemInformation fpZwQuerySystemInformation;
extern MyZwQueryInformationProcess fpZwQueryInformationProcess;

PVOID GetProcessId(WCHAR* proc_name) {
	char checker = 0;
	ULONG ReturnLength = 0;
	PVOID TargetPID = 0;
	UNICODE_STRING TargetName;
	PVOID SizeFinder = NULL64;
	PSYSTEM_PROCESS_INFORMATION pspi;

	RtlInitUnicodeString(&TargetName, proc_name);
	do {
		if (checker) {
			ExFreePool(SizeFinder);
		}
		checker = 1;
		SizeFinder = ExAllocatePool(NonPagedPool, ReturnLength);
	} while (fpZwQuerySystemInformation(SystemProcessInformation, SizeFinder, ReturnLength, &ReturnLength));
	pspi = (PSYSTEM_PROCESS_INFORMATION)SizeFinder;

	while (pspi->NextEntryOffset) {
		if (!RtlCompareUnicodeString(&(pspi->ImageName), &TargetName, TRUE))
		{
			if ((pspi->SessionId) == 1) {
				TargetPID = pspi->UniqueProcessId;
				break;
			}
		}
		pspi = (PSYSTEM_PROCESS_INFORMATION)((char*)pspi + pspi->NextEntryOffset); // next struct
	}
	if (!TargetPID) DbgPrint("[DBG] Meh -_- Can't find target prcoess.. work harder :D\n");

	return TargetPID;
}
PSYSTEM_HANDLE_INFORMATION_EX GetAllSystemHandle() {
	char checker = 0;
	ULONG ReturnLength = 0;
	PVOID SizeFinder = NULL64; // base

	DbgPrint("[DBG] fpZwQuerySystemInformation : %p\n", fpZwQuerySystemInformation);

	do {
		if (checker)
			ExFreePool(SizeFinder);
		checker = 1;
		SizeFinder = ExAllocatePool(NonPagedPool, ReturnLength);
	} while(fpZwQuerySystemInformation(SystemExtendedHandleInformation, SizeFinder, ReturnLength, &ReturnLength));
	DbgPrint("[DBG] ReturnLength : %d\n", ReturnLength);
	return (PSYSTEM_HANDLE_INFORMATION_EX)SizeFinder;
}
UCHAR IsCsrssHaveDangerPipe(PSYSTEM_HANDLE_INFORMATION_EX pshie, HANDLE pid) {
	PFILE_OBJECT FileObject;
	UNICODE_STRING PipeName;
	POBJECT_NAME_INFORMATION poni;
	UCHAR PipeCount = 0;
	WCHAR GetNameBuffer[1028] = { 0, };
	ULONG ReturnLength = 0;
	NTSTATUS Status;

	RtlInitUnicodeString(&PipeName, L"\\Device\\NamedPipe");
	for (ULONG i = 0; i < pshie->NumberOfHandles; i++) {
		if (((HANDLE)(pshie->Handles[i].UniqueProcessId) == pid)
			&& (pshie->Handles[i].ObjectTypeIndex == OBJECT_TYPE_FILE)){
				FileObject = (PFILE_OBJECT)pshie->Handles[i].Object;
				if ((FileObject->Flags) && FO_NAMED_PIPE) {
					// GetLength
					poni = (POBJECT_NAME_INFORMATION)GetNameBuffer;
					Status = ObQueryNameString(FileObject->DeviceObject, poni, sizeof(GetNameBuffer), &ReturnLength);
					if (Status)
						DbgPrint("[DBG] ObQueryNameString call failed.. :( Reason : %x\n", Status);
					DbgPrint("[DBG] PipeNameLength : %d\n", PipeName.Length);
					DbgPrint("[DBG] GetPipeNameLength : %d\n", poni->Name.Length);
					DbgPrint("[DBG] GetPipeName : %ws\n", poni->Name.Buffer);
					if (!RtlCompareUnicodeString(&PipeName, &(poni->Name), TRUE))
						++PipeCount;
				}
			}
	}
	return PipeCount;
}