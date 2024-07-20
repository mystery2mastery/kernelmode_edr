/******************************************************************
*                                                                   
*  KernelVersionHelpers.h -- This module defines helper functions   
*                            to promote version check with proper   
*                            comparisons.                           
*                         -- Inspired from usermode VersionHelpers.h
*                                                                 
*  Copyright (c) Gorla Narasimha Reddy.  All rights reserved.     
*                                                                 
******************************************************************/



#ifdef _MSC_VER
#pragma once
#endif  // _MSC_VER


#include <wdm.h> // contains OSVERSIONINFOEXW, VerSetConditionMask() 
#include <minwindef.h> // contains definitions for WORD etc.

#if (NTDDI_VERSION >= NTDDI_WINXP)
#ifdef __cplusplus

#define KERNELVERSIONHELPERAPI inline bool

#else  // __cplusplus

#define KERNELVERSIONHELPERAPI FORCEINLINE BOOL

#endif // __cplusplus

KERNELVERSIONHELPERAPI
IsWindowsVersionOrGreater(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor)
{
    NTSTATUS status = STATUS_SUCCESS;     
    RTL_OSVERSIONINFOEXW OSVersionInfo = { 0 };
    ULONG TypeMask = 0;
    ULONGLONG ConditionMask = 0;

    OSVersionInfo.dwOSVersionInfoSize = sizeof(OSVersionInfo);
    OSVersionInfo.dwMajorVersion = wMajorVersion;
    OSVersionInfo.dwMinorVersion = wMinorVersion;
    OSVersionInfo.wServicePackMajor = wServicePackMajor;

    TypeMask = VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR;
    VER_SET_CONDITION(ConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    VER_SET_CONDITION(ConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    VER_SET_CONDITION(ConditionMask, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

    status = RtlVerifyVersionInfo(&OSVersionInfo, TypeMask, ConditionMask);

    if (NT_SUCCESS(status)) {

        //
        // The call to RtlVerifyVersionInfo succeeded, so the running OS
        // version and service pack number is greater than or equal to the value
        // specified. Do appropriate action for newer OS versions.
        //
        return TRUE;

    }
    else if (status == STATUS_REVISION_MISMATCH) {

        //
        // The running OS version is less than the value specified. Do
        // appropriate action for older OS versions.
        //
        return FALSE;
    }
    else {

        //
        // There was an error comparing to the running OS version. Do
        // appropriate action for when the OS version is not known.
        //
        return FALSE;
    }


}

KERNELVERSIONHELPERAPI
IsWindowsXPOrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 0);
}

KERNELVERSIONHELPERAPI
IsWindowsXPSP1OrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 1);
}

KERNELVERSIONHELPERAPI
IsWindowsXPSP2OrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 2);
}

KERNELVERSIONHELPERAPI
IsWindowsXPSP3OrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 3);
}

KERNELVERSIONHELPERAPI
IsWindowsVistaOrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 0);
}

KERNELVERSIONHELPERAPI
IsWindowsVistaSP1OrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 1);
}

KERNELVERSIONHELPERAPI
IsWindowsVistaSP2OrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 2);
}

KERNELVERSIONHELPERAPI
IsWindows7OrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN7), LOBYTE(_WIN32_WINNT_WIN7), 0);
}

KERNELVERSIONHELPERAPI
IsWindows7SP1OrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN7), LOBYTE(_WIN32_WINNT_WIN7), 1);
}

KERNELVERSIONHELPERAPI
IsWindows8OrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN8), LOBYTE(_WIN32_WINNT_WIN8), 0);
}

KERNELVERSIONHELPERAPI
IsWindows8Point1OrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINBLUE), LOBYTE(_WIN32_WINNT_WINBLUE), 0);
}

KERNELVERSIONHELPERAPI
IsWindowsThresholdOrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINTHRESHOLD), LOBYTE(_WIN32_WINNT_WINTHRESHOLD), 0);
}

KERNELVERSIONHELPERAPI
IsWindows10OrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINTHRESHOLD), LOBYTE(_WIN32_WINNT_WINTHRESHOLD), 0);
}

KERNELVERSIONHELPERAPI
IsWindowsServer()
{
    NTSTATUS status = STATUS_SUCCESS;
    RTL_OSVERSIONINFOEXW OSVersionInfo = { 0 };
    ULONG TypeMask = 0;
    ULONGLONG ConditionMask = 0;

    OSVersionInfo.dwOSVersionInfoSize = sizeof(OSVersionInfo);
    OSVersionInfo.wProductType = VER_NT_WORKSTATION; // (non server OS) The operating system is Windows 8, Windows 7, Windows Vista, Windows XP Professional, Windows XP Home Edition, or Windows 2000 Professional. 
    
    TypeMask = VER_PRODUCT_TYPE;
    VER_SET_CONDITION(ConditionMask, VER_PRODUCT_TYPE, VER_EQUAL);

    status = RtlVerifyVersionInfo(&OSVersionInfo, TypeMask, ConditionMask);    

    if (NT_SUCCESS(status)) {

        //
        // The call to RtlVerifyVersionInfo succeeded, so the product type of the
        // OS is equal to the value specified. Since we have specified VER_NT_WORKSTATION,
        // the current OS is a workstation and not a server. So, we return FALSE.
        //
        //MyKdPrint("IsWindowsServer RtlVerifyVersionInfo status: STATUS_SUCCESS %zX\n", status);
        return FALSE;

    }
    else if (status == STATUS_REVISION_MISMATCH) {

        //
        // The running OS product type did not match the specified value. Since we have 
        // specified VER_NT_WORKSTATION, the current OS is not a workstation but a server.
        // So, we return TRUE.
        //
        //MyKdPrint("IsWindowsServer RtlVerifyVersionInfo status: STATUS_REVISION_MISMATCH %zX\n", status);
        return TRUE;
    }
    else {

        //
        // There was an error comparing to the running OS product type. Do
        // appropriate action for when the OS product type is not known.
        //
        //MyKdPrint("IsWindowsServer RtlVerifyVersionInfo status: UNKNOWN %zX\n", status);
        return TRUE;
    }
}

VOID GetKernelModeOSVersion(void);

#endif // NTDDI_VERSION