#include "KernelOSVersionHelpers.h"

#include "HelperFunctions.h"

#include <ntddk.h>


/*
typedef struct _OSVERSIONINFOEXW {
  ULONG  dwOSVersionInfoSize;
  ULONG  dwMajorVersion;
  ULONG  dwMinorVersion;
  ULONG  dwBuildNumber;
  ULONG  dwPlatformId;
  WCHAR  szCSDVersion[128];
  USHORT wServicePackMajor;
  USHORT wServicePackMinor;
  USHORT wSuiteMask;
  UCHAR  wProductType;
  UCHAR  wReserved;
} OSVERSIONINFOEXW, *POSVERSIONINFOEXW, *LPOSVERSIONINFOEXW, RTL_OSVERSIONINFOEXW, *PRTL_OSVERSIONINFOEXW;

szCSDVersion[128]:
The service-pack version string. This member contains a null-terminated string, such as "Service Pack 3", which 
indicates the latest service pack installed on the system. If no service pack is installed, RtlGetVersion might 
not initialize this string. Initialize szCSDVersion to zero (empty string) before the call to RtlGetVersion.

*/

VOID GetKernelModeOSVersion(void)
{
    RTL_OSVERSIONINFOEXW versionInfo = {0};
    NTSTATUS status;

    // Initialize the versionInfo structure
    //RtlZeroMemory(&versionInfo, sizeof(RTL_OSVERSIONINFOEXW));
    versionInfo.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);

    // Call RtlGetVersion
    status = RtlGetVersion((PRTL_OSVERSIONINFOW)&versionInfo);

    if (NT_SUCCESS(status)) {
        MyKdPrint("[API: RtlGetVersion] Major Version: %lu\n", versionInfo.dwMajorVersion);
        MyKdPrint("[API: RtlGetVersion] Minor Version: %lu\n", versionInfo.dwMinorVersion);
        MyKdPrint("[API: RtlGetVersion] Build Number: %lu\n", versionInfo.dwBuildNumber);
        MyKdPrint("[API: RtlGetVersion] Platform ID: %lu\n", versionInfo.dwPlatformId);
        MyKdPrint("[API: RtlGetVersion] CSD Version: %ws\n", versionInfo.szCSDVersion); // Service Pack string. It will be initialized to zero as I filled the full structure with zero.
        MyKdPrint("[API: RtlGetVersion] Service Pack Major: %u\n", versionInfo.wServicePackMajor);
        MyKdPrint("[API: RtlGetVersion] Service Pack Minor: %u\n", versionInfo.wServicePackMinor);
        MyKdPrint("[API: RtlGetVersion] Suite Mask: 0x%zX\n", versionInfo.wSuiteMask);
        MyKdPrint("[API: RtlGetVersion] Product Type: %u\n", versionInfo.wProductType);
    }
    else {
        MyKdPrint("RtlGetVersion failed with status: 0x%x\n", status);
    }    
}