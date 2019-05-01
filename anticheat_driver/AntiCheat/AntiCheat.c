#include "AntiCheat.h"

PVOID recevier;

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	NTSTATUS Status;
	UNICODE_STRING SymbolicLinkName, DeviceName;
	PDEVICE_OBJECT DeviceObject;

	UNREFERENCED_PARAMETER(RegistryPath);

	RtlInitUnicodeString(&SymbolicLinkName, L"\\??\\AntiCheatLinker");
	RtlInitUnicodeString(&DeviceName, L"\\Device\\AntiCheatDriver");

	Status = IoCreateDevice( DriverObject,
					0,
					&DeviceName,
					FILE_DEVICE_UNKNOWN,
					FILE_DEVICE_SECURE_OPEN,
					0,
					&DeviceObject
	);
	if (Status) {
		DbgPrint("[DBG] IoCreateDevice failed.. Reason : %d\n", Status);
		return Status;
	}
	Status = IoCreateSymbolicLink(&SymbolicLinkName, &DeviceName);
	if (Status) {
		DbgPrint("[DBG] IoCreateSymblicLink failed.. Reason : %d\n", Status);
		return Status;
	}
	DbgPrint("[DBG] AntiCheat DriverEntry Called\n");
	DriverObject->MajorFunction[IRP_MJ_CREATE] = create_dispatcher;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = device_control_dispatcher;

	return STATUS_SUCCESS;
}

NTSTATUS create_dispatcher(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP irp) {
	OB_CALLBACK_REGISTRATION ocr;
	OB_OPERATION_REGISTRATION oor;
	UNICODE_STRING Altitude;
	NTSTATUS Status;
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(irp);

	RtlInitUnicodeString(&Altitude, L"312345");
	// TODO : Create RegistrationContext struct must define exception_process_name like csrss, lsass.. whatever.
	// oor set
	oor.Operations = OB_OPERATION_HANDLE_CREATE;
	oor.ObjectType = PsProcessType;
	oor.PreOperation = PobPreOperationCallback;
	oor.PostOperation = PobPostOperationCallback;
	// ocr set
	ocr.OperationRegistration = &oor;
	ocr.OperationRegistrationCount = 1;
	ocr.Version = ObGetFilterVersion();
	ocr.RegistrationContext = NULL;
	ocr.Altitude = Altitude;

	Status = ObRegisterCallbacks(&ocr, &recevier);
	if (Status) {
		DbgPrint("[DBG] ObRegisterCallbacks failed.. Reason : %d\n", Status);
	}
	return STATUS_SUCCESS;
}
NTSTATUS device_control_dispatcher(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP irp){
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(irp);

	return STATUS_SUCCESS;
}

OB_PREOP_CALLBACK_STATUS PobPreOperationCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation) {
	UNREFERENCED_PARAMETER(RegistrationContext);
	CHAR* ProcName;
	CHAR* ProtectProcess = "MapleStory.exe";

	if (OperationInformation->KernelHandle == TRUE)
		return OB_PREOP_SUCCESS;

	// 핸들을 '요청한 대상'(메이플스토리)의 EPROCESS 오브젝트가 넘어온다.
	ProcName = PsGetProcessImageFileName((PEPROCESS)OperationInformation->Object);
	
	if (RtlCompareMemory(ProcName, ProtectProcess, strlen(ProtectProcess)) 
		== strlen(ProtectProcess)){
		OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = (SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION);
	}
	return OB_PREOP_SUCCESS;
}

void PobPostOperationCallback(PVOID RegistrationContext, POB_POST_OPERATION_INFORMATION OperationInformation) {
	UNREFERENCED_PARAMETER(RegistrationContext);
	UNREFERENCED_PARAMETER(OperationInformation);
	//OperationInformation->Parameters->CreateHandleInformation.GrantedAccess = PROCESS_TERMINATE;
	//DbgPrint("[DBG] PobPostOperationCallback\n");
}
