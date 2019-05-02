#pragma once
#include <ntddk.h>
#include <wdm.h>

#define PROCESS_TERMINATE                  (0x0001)  
#define PROCESS_CREATE_THREAD              (0x0002)  
#define PROCESS_SET_SESSIONID              (0x0004)  
#define PROCESS_VM_OPERATION               (0x0008)  
#define PROCESS_VM_READ                    (0x0010)  
#define PROCESS_VM_WRITE                   (0x0020)  
#define PROCESS_DUP_HANDLE                 (0x0040)  
#define PROCESS_CREATE_PROCESS             (0x0080)  
#define PROCESS_SET_QUOTA                  (0x0100)  
#define PROCESS_SET_INFORMATION            (0x0200)  
#define PROCESS_QUERY_INFORMATION          (0x0400)  
#define PROCESS_SUSPEND_RESUME             (0x0800)  
#define PROCESS_QUERY_LIMITED_INFORMATION  (0x1000)  
#define PROCESS_SET_LIMITED_INFORMATION    (0x2000)  

#define IO_CTL_PROCESS_PIPE_FIND CTL_CODE(FILE_DEVICE_UNKNOWN, 0x814, METHOD_BUFFERED , FILE_SPECIAL_ACCESS)
#define IO_CTL_PROCESS_CALLBACK_REGISTER CTL_CODE(FILE_DEVICE_UNKNOWN, 0x815, METHOD_BUFFERED , FILE_SPECIAL_ACCESS)

NTSTATUS create_dispatcher(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP irp);
NTSTATUS device_control_dispatcher(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP irp);
OB_PREOP_CALLBACK_STATUS PobPreOperationCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation);
void PobPostOperationCallback(PVOID RegistrationContext, POB_POST_OPERATION_INFORMATION OperationInformation);

