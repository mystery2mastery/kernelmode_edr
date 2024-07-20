#include "DriverRoutines.h"

#include "common.h"
#include "HelperFunctions.h"
#include "KernelCallbackRegistrations.h"

// Driver Unload routine
void Driver_Unload(PDRIVER_OBJECT pDriverObject)
{
	MyKdPrint("Unloading Driver ...\n");

	// Unregister callbacks
	UnRegisterCallbackNotifications();

	UNICODE_STRING usSymlinkName;

	// RtlInitUnicodeString() is used to create a UNICODE_STRING from a string literal.
	RtlInitUnicodeString(&usSymlinkName, SYMLINK_NAME);
	
	IoDeleteSymbolicLink(&usSymlinkName);
	IoDeleteDevice(pDriverObject->DeviceObject);
}

// IRP_MJ_CREATE = To open handle to a device object
NTSTATUS Driver_Create(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	MyKdPrint("Driver IRP_MJ_Create\n");
	UNREFERENCED_PARAMETER(pDeviceObject);

	// Generic completion
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, 0);

	return STATUS_SUCCESS;
}

// IRP_MJ_CLOSE = To close handle to a device object
NTSTATUS Driver_Close(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	MyKdPrint("Driver IRP_MJ_Close\n");
	UNREFERENCED_PARAMETER(pDeviceObject);

	// Generic completion
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, 0);

	return STATUS_SUCCESS;
}