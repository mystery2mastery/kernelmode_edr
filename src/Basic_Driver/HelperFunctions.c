#include "HelperFunctions.h"

#include <ntddk.h>

BOOLEAN IsValidUnicodeString(PCUNICODE_STRING str)
{
    // Check if the pointer is NULL or the buffer pointer is NULL
    return (str != NULL && str->Buffer != NULL);
}

BOOLEAN UnicodeStringEndsWith(PCUNICODE_STRING FullString, PCUNICODE_STRING SuffixStr, BOOLEAN ExactMatch)
{
    // Validate pointers
    if (!IsValidUnicodeString(FullString) || !IsValidUnicodeString(SuffixStr))
    {
        return FALSE;
    }

    // Ensure the full image name is at least as long as the suffix
    if (FullString->Length < SuffixStr->Length)
    {
        return FALSE;
    }

    // Calculate the starting position for comparison
    USHORT offset = FullString->Length - SuffixStr->Length;

    // Create a UNICODE_STRING that points to the suffix in FullImageName
    UNICODE_STRING subString;
    subString.Buffer = FullString->Buffer + (offset / sizeof(WCHAR));
    subString.Length = SuffixStr->Length;
    subString.MaximumLength = SuffixStr->Length;

    // Compare the suffixes
    return RtlEqualUnicodeString(&subString, SuffixStr, !ExactMatch); // ExactMatch => case-sensitive. The !ExactMatch is used because RtlEqualUnicodeString expects a boolean TRUE value for case-insensitive comparison.
}

BOOLEAN IsMappedByLdrLoadDll(PCUNICODE_STRING ModuleShortName)
{
    // Check if the thread is running from within LdrLoadDll() to load a module
    // Undocumented feature: LdrLoadDll() sets Teb->ArbitraryUserPointer to non NULL. If it is NULL then it is not executed by LdrLoadDll()
    // The NT_TIB.ArbitraryUserPointer is temporarily replaced with a string pointer (in Windows NT, this is always a unicode string) to the original filename passed to LdrLoadDll. Reference: http://www.nynaeve.net/?p=98
    // https://codemachine.com/articles/arbitraryuserpointer_usage.html
    // INFO: Otherwise the thread could be executing ZwMapViewOfSection with parameter Win32Protect as SEC_IMAGE attribute.
    //      Ex: smss.exe can map kernel32.dll during creation of \\KnownDlls (in that case ArbitraryUserPointer will be 0)
    //      Ex: WoW64 processes map kernel32.dll several time (32 bit and 64bit versions) with WOW64_IMAGE_SECTION or NOT_AN_IMAGE

    // Validate input pointers
    if (!IsValidUnicodeString(ModuleShortName))
    {
        return FALSE;
    }

    UNICODE_STRING Name;
    __try
    {
        // Get current thread's TEB
        PNT_TIB Teb = (PNT_TIB)PsGetCurrentThreadTeb();
        if (
            !Teb ||     // Failed to get Teb
            !Teb->ArbitraryUserPointer  // Teb->ArbitraryUserPointer == NULL => NOT executed by LdrLoadDll()
            )
        {
            // TEB or ArbitraryUserPointer is NULL. This is NOT our requirement.
            return FALSE;
        }

        // Get the Unicode string from ArbitraryUserPointer
        Name.Buffer = (PWSTR)Teb->ArbitraryUserPointer;

        // Check that the Teb->ArbitraryUserPointer actually resides in user-mode and is a valid user-mode address.
        // If the LoadLibray() has been called from user-mode then while calling LdrLoadDll() internally, Teb->ArbitraryUserPointer will be filled with address of a user-mode address containing the module path.
        ProbeForRead(Name.Buffer, sizeof(WCHAR), __alignof(WCHAR));

        // Check buffer length. We are checking for maximum possible path length for Unicode versions i.e., 32767 (0x7FFF) => MAXSHORT
        // wcsnlen => safe function
        Name.Length = (USHORT)wcsnlen(Name.Buffer, MAXSHORT);
        if (Name.Length == MAXSHORT)
        {
            // Name is too long
            return FALSE;
        }

        Name.Length *= sizeof(WCHAR);
        Name.MaximumLength = Name.Length;

        // See if its our needed module
        return UnicodeStringEndsWith(&Name, ModuleShortName, FALSE );

    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        // something failed
        MyKdPrint("#EXCEPTION: (0x%X) IsMappedByLdrLoadDll", GetExceptionCode());
    }

    return FALSE;
    

}