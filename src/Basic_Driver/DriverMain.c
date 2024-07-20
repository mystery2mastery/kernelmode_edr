#include <ntddk.h>

#include "common.h"
#include "HelperFunctions.h"
#include "DriverRoutines.h"
#include "KernelOSVersionHelpers.h"
#include "KernelCallbackRegistrations.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pusRegistrypath)
{
	//UNREFERENCED_PARAMETER(pDriverObject);
	//UNREFERENCED_PARAMETER(pusRegistrypath);
	MyKdPrint("Registry Path: %wZ\n", pusRegistrypath); // %wZ to print UNICODE_STRING
	
	NTSTATUS status;

	UNICODE_STRING usDeviceName;
	UNICODE_STRING usSymlinkName;

	RtlInitUnicodeString(&usDeviceName, DEVICE_NAME);
	RtlInitUnicodeString(&usSymlinkName, SYMLINK_NAME);

	//  ----------------------- Create Device Object ------------------------
	
	PDEVICE_OBJECT pDeviceObject = NULL;

	status = IoCreateDevice(
		pDriverObject,
		0,
		&usDeviceName,
		FILE_DEVICE_UNKNOWN,
		FILE_DEVICE_SECURE_OPEN, // 0 = No security; 0x100 = FILE_DEVICE_SECURE_OPEN
		FALSE,
		&pDeviceObject
	);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("IoCreateDevice failed with error: 0x%X\n", status));
		return status;
	}

	// ------------ Create Symbolic Link to the Device Object --------------
	
	status = IoCreateSymbolicLink(&usSymlinkName, &usDeviceName);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("IoCreateSymbolicLink failed with error: 0x%X\n", status));
		IoDeleteDevice(pDeviceObject);
		return status;
	}

	// ------------------ Initialize Dispatch Routine Functions -----------------
	
	pDriverObject->DriverUnload = Driver_Unload;

	pDriverObject->MajorFunction[IRP_MJ_CREATE] = Driver_Create;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = Driver_Close;


	// ------------------ test execution of my functions -----------------

	// Decide OS
	GetKernelModeOSVersion();

	if (IsWindowsServer())
		MyKdPrint("[Function: IsWindowsServer] The system is running a Windows Server edition\n");
	else
		MyKdPrint("[Function: IsWindowsServer] The system is NOT running a Windows Server edition\n");

	if (!IsWindows10OrGreater())
		MyKdPrint("[Function: IsWindows10OrGreater] Version Not Supported: You need at least Windows 10.\n");
	else
		MyKdPrint("[Function: IsWindows10OrGreater] Windows 10 found\n");
	
	// Register notification callbacks. Dont forget to add unregister to driver unload.
	RegisterCallbackNotifications();

	STATIC_UNICODE_STRING(kernel, "\\kernel32.dll");

	return STATUS_SUCCESS;
}