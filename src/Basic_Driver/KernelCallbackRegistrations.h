#pragma once

#include <ntddk.h>

enum REGISTER_CALLBACK_FLAGS
{
	flProcessCallbackSet,	// is set when RegisterProcessCallback->PsSetLoadImageNotifyRoutine is sucessfully enabled.
	flThreadCallbackSet,
	flImageLoadCallbackSet,

};

#define STATUS_TO_STRING(x) ((x) == STATUS_SUCCESS ? "STATUS_SUCCESS" : \
                            (x) == STATUS_INVALID_PARAMETER ? "STATUS_INVALID_PARAMETER" : \
							(x) == STATUS_INSUFFICIENT_RESOURCES ? "STATUS_INSUFFICIENT_RESOURCES" : \
                            (x) == STATUS_ACCESS_DENIED ? "STATUS_ACCESS_DENIED" : "UNKNOWN_STATUS")

VOID RegisterCallbackNotifications();

VOID UnRegisterCallbackNotifications();
