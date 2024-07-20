/****************************************************************************************
*                                                                                       *
*  KernelCallbackRegistrations.c -- Contains functions to register callbacks.			*
*                                                                                       *
*  Copyright (c) Gorla Narasimha Reddy.  All rights reserved.                           *
*                                                                                       *
*****************************************************************************************/

#include "KernelCallbackRegistrations.h"

#include "HelperFunctions.h"


enum REGISTER_CALLBACK_FLAGS g_CallbackFlags;	// global callback notification flags

// Actual callback routine that is called on Process Creation
// The process-notify routine runs at PASSIVE_LEVEL, meaning it cannot perform certain operations that require higher IRQL levels.
// PCREATE_PROCESS_NOTIFY_ROUTINE
void OnProcessCreate(
	_In_ HANDLE ParentId,
	_In_ HANDLE ProcessId,
	_In_ BOOLEAN Create
)
{
	UNREFERENCED_PARAMETER(ParentId);
	UNREFERENCED_PARAMETER(ProcessId);
	UNREFERENCED_PARAMETER(Create);

	// When a process is created, the process-notify routine runs in the context of the thread that created the new process. 
	// If you have a process A that calls CreateProcess to create a new process B, the process-notify routine will be executed in the context of the thread in process A that called CreateProcess.
	if (Create) // Process created
	{
		MyKdPrint("[ProcessCreate] ParentId: (0x%X), ProcessId: (0x%X)", ParentId, ProcessId);
	}
	// When a process is deleted, the process-notify routine runs in the context of the last thread to exit from the process.
	// Example: If process B is terminating and the last thread in process B exits, the process-notify routine will be executed in the context of this last thread of process B.
	else // Process destroyed
	{
		MyKdPrint("[ProcessClose] ParentId: (0x%X), ProcessId: (0x%X)", ParentId, ProcessId);
	}	

}
// Actual callback routine that is called on Process Creation
// PCREATE_PROCESS_NOTIFY_ROUTINE_EX
// Note: If you use Ex version, then you need to have IMAGE_DLLCHARACTERISTICS_FORCE_INTEGRITY set in the driver's image header. 
// Use the linker option
void OnProcessCreateEx(
	_Inout_     PEPROCESS Process,
	_In_        HANDLE ProcessId,
	_Inout_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
)
{
	UNREFERENCED_PARAMETER(Process);
	UNREFERENCED_PARAMETER(ProcessId);
	UNREFERENCED_PARAMETER(CreateInfo);

	// Process create
	if (CreateInfo != NULL)
	{
		MyKdPrint("[ProcessCreate] EPROCESS: 0x%X, ProcessId: (0x%X), ProcessCreationInfo: {cmdline: %wZ, ParentPid: (0x%X)}", Process, ProcessId, CreateInfo->CommandLine, CreateInfo->ParentProcessId);
		
	}
	else // Process exit
	{
		MyKdPrint("[ProcessExit] ProcessId: (0x%X)", ProcessId);
	}


}


// Actual callback routine that is called on Thread Creation
void OnThreadCreate(
	_In_ HANDLE ProcessId,
	_In_ HANDLE ThreadId,
	_In_ BOOLEAN Create
)
{
	UNREFERENCED_PARAMETER(ProcessId);
	UNREFERENCED_PARAMETER(ThreadId);
	UNREFERENCED_PARAMETER(Create);
}

// Actual callback routine that is called on Image Load
void OnImageLoad(
	_In_opt_  PUNICODE_STRING FullImageName,
	_In_      HANDLE ProcessId,
	_In_      PIMAGE_INFO ImageInfo
)
{
	UNREFERENCED_PARAMETER(FullImageName);
	UNREFERENCED_PARAMETER(ProcessId);
	UNREFERENCED_PARAMETER(ImageInfo);

	//NTSTATUS status = STATUS_SUCCESS;

	MyKdPrint("[ImageLoad] FullImageName: %wZ", FullImageName);
	MyKdPrint("[ImageLoad] ProcessId: (0x%X)", ProcessId);
	//MyKdPrint("[ImageLoad] ImageInfo: ");


	// We are looking for loading of kernel32.dll to inject our dll.
	// SystemModeImage => something that is getting loaded/mapped into 'System' (PID 4) process => An image that is getting loaded into kernel space. We are NOT looking for images getting loaded/mapped into kernel space. We are looking for images mapped into user space.  	// 
	// ProcessId == PsGetCurrentProcessId()
	// We are looking for loads initiated within the process as part of the process creation BUT not the remotely injected dlls.
	// When a driver or another process maps an image into the address space of a different process, the ProcessId parameter will be the ID of the target process where the image is being loaded. However, PsGetCurrentProcessId will return the ID of the process context in which the callback is executing, which is usually the process performing the image load operation.
	// We need to ensure that we are NOT looking at loads from ZwMapViewOfSection() BUT from LoadLibrary()->LdrLoadDll().
	// 


	STATIC_UNICODE_STRING(kernel32, "\\kernel32.dll"); // The slash needs to be escaped so its a double slash. It is NOT two unique slashes.

	if (
		!ImageInfo->SystemModeImage &&				// the image is loaded into user space
		ProcessId == PsGetCurrentProcessId() &&		// not a remote load (process into which the image is getting loaded == process that initiated the image load)
		UnicodeStringEndsWith(FullImageName, &kernel32, FALSE) &&	// full name ends with kernel32.dll (not case-sensitive)
		IsMappedByLdrLoadDll(&kernel32)				// whether the module is loaded by LdrLoadDll() or not.
		)
	{
		MyKdPrint("Module loaded by LdrLoadDll\n");
		
	}





}


// Register Process Creation Notification 
NTSTATUS RegisterProcessCallback()
{
	//NTSTATUS status = PsSetCreateProcessNotifyRoutine(OnProcessCreate, FALSE); // FALSE => routine is added to the list.
	//if (NT_SUCCESS(status))
	//{
	//	MyKdPrint("PsSetCreateProcessNotifyRoutine sucesss\n");
	//	_bittestandset((LONG*)&g_CallbackFlags, flProcessCallbackSet); // to set the bit atomically.
	//}		
	//else // Error
	//	MyKdPrint("[X] PsSetCreateProcessNotifyRoutine add failed with status: (0x%X) %s", status, STATUS_TO_STRING(status));


	NTSTATUS status = PsSetCreateProcessNotifyRoutineEx(OnProcessCreateEx, FALSE);
	if (NT_SUCCESS(status))
	{
		MyKdPrint("PsSetCreateProcessNotifyRoutineEx sucesss\n");
		_bittestandset((LONG*)&g_CallbackFlags, flProcessCallbackSet); // to set the bit atomically.
	}		
	else // Error
		MyKdPrint("[X] PsSetCreateProcessNotifyRoutineEx add failed with status: (0x%X) %s", status, STATUS_TO_STRING(status));

	return status;
}

// Register Thread Creation Notification 
NTSTATUS RegisterThreadCallback()
{
	NTSTATUS status = PsSetCreateThreadNotifyRoutine(OnThreadCreate);
	if (NT_SUCCESS(status))
	{
		MyKdPrint("PsSetCreateThreadNotifyRoutine  sucesss\n");
		_bittestandset((LONG*)&g_CallbackFlags, flThreadCallbackSet); // to set the bit atomically.
	}
	else // Error
		MyKdPrint("[X] PsSetCreateThreadNotifyRoutine failed with status: (0x%X) %s", status, STATUS_TO_STRING(status));

	return status;
}

// Register Image Load Notification 
NTSTATUS RegisterImageLoadCallback()
{
	NTSTATUS status = PsSetLoadImageNotifyRoutine(OnImageLoad);
	if (NT_SUCCESS(status))
	{
		MyKdPrint("PsSetLoadImageNotifyRoutine sucesss\n");
		_bittestandset((LONG*)&g_CallbackFlags, flImageLoadCallbackSet); // to set the bit atomically.
	}
	else // Error
		MyKdPrint("[X] PsSetLoadImageNotifyRoutine failed with status: (0x%X) %s", status, STATUS_TO_STRING(status));

	return status;
}

// Unregister Process Callback
NTSTATUS UnRegisterProcessCallback()
{
	NTSTATUS status = STATUS_SUCCESS;
	
	// Remove the callback only if it was set.
	if (_bittestandreset((LONG*)&g_CallbackFlags, flProcessCallbackSet))
	{
		//status = PsSetCreateProcessNotifyRoutine(OnProcessCreate, TRUE); // TRUE => routine is removed from the list.
		status = PsSetCreateProcessNotifyRoutineEx(OnProcessCreateEx, TRUE);
		if (!NT_SUCCESS(status))
		{
			MyKdPrint("[X] PsSetCreateProcessNotifyRoutineEx removal failed: (0x%X)", status);
		}
		else
			MyKdPrint("PsSetCreateProcessNotifyRoutineEx removal success\n");
	}

	return status;
}

// Unregister Thread Callback
NTSTATUS UnRegisterThreadCallback()
{	
	NTSTATUS status = STATUS_SUCCESS;

	// Remove the callback only if it was set.
	if (_bittestandreset((LONG*)&g_CallbackFlags, flThreadCallbackSet))
	{
		status = PsRemoveCreateThreadNotifyRoutine(OnThreadCreate);
		if (!NT_SUCCESS(status))
		{
			MyKdPrint("[X] PsRemoveCreateThreadNotifyRoutine removal failed: (0x%X)", status);
		}
		else
			MyKdPrint("PsRemoveCreateThreadNotifyRoutine success");
	}

	return status;
}

// Unregister Image Load Callback
NTSTATUS UnRegisterImageLoadCallback()
{
	NTSTATUS status = STATUS_SUCCESS;

	// Remove the callback only if it was set.
	if (_bittestandreset((LONG*)&g_CallbackFlags, flImageLoadCallbackSet))
	{
		status = PsRemoveLoadImageNotifyRoutine(OnImageLoad);
		if (!NT_SUCCESS(status))
		{
			MyKdPrint("[X] PsRemoveLoadImageNotifyRoutine removal failed: (0x%X)", status);
		}
		else
			MyKdPrint("PsRemoveLoadImageNotifyRoutine success");
	}

	return status;
}

VOID RegisterCallbackNotifications()
{
	RegisterProcessCallback();
	RegisterThreadCallback();
	RegisterImageLoadCallback();
}

VOID UnRegisterCallbackNotifications()
{
	UnRegisterProcessCallback();
	UnRegisterThreadCallback();
	UnRegisterImageLoadCallback();
}