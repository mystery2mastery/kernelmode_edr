#pragma once
#include <ntddk.h>




void Driver_Unload(PDRIVER_OBJECT pDriverObject);

NTSTATUS Driver_Create(PDEVICE_OBJECT pDeviceObject, PIRP pIrp);

NTSTATUS Driver_Close(PDEVICE_OBJECT pDeviceObject, PIRP pIrp);
