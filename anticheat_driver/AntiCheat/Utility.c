#include "undocument.h"

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
PPROCESS_HANDLE_SNAPSHOT_INFORMATION GetInsideProcessHandle(HANDLE hProcess) {
	char checker = 0;
	ULONG handle_len = 0;
	PVOID buffer = NULL64; // base

	do {
		if (checker)
			ExFreePool(buffer);
		checker = 1;
		buffer = ExAllocatePool(NonPagedPool, handle_len);
	} while(fpZwQueryInformationProcess(
		hProcess,
		(PROCESSINFOCLASS)51,
		buffer,
		handle_len,
		&handle_len
	));
	return (PPROCESS_HANDLE_SNAPSHOT_INFORMATION)buffer;
}
UCHAR IsCsrssHaveDangerPipe(PPROCESS_HANDLE_SNAPSHOT_INFORMATION phsi) {
	PFILE_NAME_INFORMATION fni;
	UNICODE_STRING PipeName, GetFileName;
	NTSTATUS Status;
	UCHAR PipeCount = 0;

	RtlInitUnicodeString(&PipeName, L"\\Device\\NamedPipe");

	for (ULONG i = 0; i < phsi->NumberOfHandles; i++) {
		if (phsi->Handles[i].ObjectTypeIndex == OBJECT_TYPE_FILE) {
			fni = ExAllocatePool(NonPagedPool, 64);
			Status = ZwQueryInformationFile(
				phsi->Handles[i].HandleValue,
				0,
				fni,
				60,
				FileNameInformation
			);
			DbgPrint("%s\n", fni->FileName[fni->FileNameLength]);
			if (Status)
				DbgPrint("[DBG] ZwQueryInformationFile Error :D Reason : %d\n", Status);
			RtlInitUnicodeString(&GetFileName, &(fni->FileName[fni->FileNameLength]));
			if (!RtlCompareUnicodeString(&GetFileName, &PipeName, TRUE))
				PipeCount++;
			ExFreePool(fni);
		}
	}
	return PipeCount;
}