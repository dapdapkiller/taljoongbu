#include "AntiCheat.h"
#include "undocument.h"
#include "Utility.h"

// static function pointer 
// Initilization at call DriverEntry
MyZwQuerySystemInformation fpZwQuerySystemInformation;
MyZwQueryInformationProcess fpZwQueryInformationProcess; 

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath){
	NTSTATUS Status;
	UNICODE_STRING SymbolicLinkName, DeviceName;
	UNICODE_STRING ZwQuerySystmeInformationName, ZwQueryInformationProcessName;
	PDEVICE_OBJECT DeviceObject;

	UNREFERENCED_PARAMETER(RegistryPath);
	
	// DriverName Initalization
	RtlInitUnicodeString(&SymbolicLinkName, L"\\??\\AntiCheatLinker");
	RtlInitUnicodeString(&DeviceName, L"\\Device\\AntiCheatDriver");
	// Undocument Function Intialization
	RtlInitUnicodeString(&ZwQuerySystmeInformationName, L"ZwQuerySystemInformation");
	RtlInitUnicodeString(&ZwQueryInformationProcessName, L"ZwQueryInformationProcess");

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
	
	fpZwQuerySystemInformation = (MyZwQuerySystemInformation)MmGetSystemRoutineAddress(&ZwQuerySystmeInformationName);
	fpZwQueryInformationProcess = (MyZwQueryInformationProcess)MmGetSystemRoutineAddress(&ZwQueryInformationProcessName);

	return STATUS_SUCCESS;
}

NTSTATUS create_dispatcher(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP irp) {
	OB_CALLBACK_REGISTRATION ocr;
	OB_OPERATION_REGISTRATION oor;
	UNICODE_STRING Altitude;
	NTSTATUS Status;
	PVOID recevier;

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
	ULONG ControlCode;
	PIO_STACK_LOCATION CurrentStackLocation;
	PPROCESS_HANDLE_SNAPSHOT_INFORMATION pphsi;
	PVOID CheckPID;
	HANDLE CheckHandle;
	CLIENT_ID ci;
	NTSTATUS Status;
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(irp);

	CurrentStackLocation = IoGetCurrentIrpStackLocation(irp);
	ControlCode = CurrentStackLocation->Parameters.DeviceIoControl.IoControlCode;

	switch (ControlCode) {
	case IO_CTL_PROCESS_PIPE_FIND:
		// TODO: GetPID, GetProcessHandle, NtQueryInformationProcess, GetHandleList, GetPipe, PipeCounting and monitoring..
		CheckPID = GetProcessId(L"csrss.exe");
		ci.UniqueProcess = (HANDLE)CheckPID;
		Status = NtOpenProcess(&CheckHandle, PROCESS_ALL_ACCESS, NULL, &ci);
		if (Status)
			DbgPrint("[DBG] NtOpenProcess called failed.. Reason : %d :D\n", Status);
		pphsi = GetInsideProcessHandle(CheckHandle);
		if (IsCsrssHaveDangerPipe(pphsi) > 1) {
			// TODO: Logging.. AutoBan.. Something..!
			DbgPrint("[DBG] Hacked.. :( \n");
		}
		ExFreePool(pphsi);
		break;
	case IO_CTL_PROCESS_CALLBACK_REGISTER:
		break;
	default:
		DbgPrint("[DBG] Meh.. this control code does not exist\n");
	}

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