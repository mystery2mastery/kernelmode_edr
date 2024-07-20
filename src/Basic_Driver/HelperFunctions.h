#pragma once

#include "common.h"

#include <ntddk.h>

/************************************************************************
*   Macro to:
*   1. Add a string infront of all KdPrint() outputs.
*   Ex: MyKdPrint("Registry Path: %wZ\n", RegistryPath); // Usage exactly same as KdPrint() but without the double ().
* 
*   Credits: Gorla Narasimha Reddy
*************************************************************************/
    // Add this prefix infront of every KdPrint() output. Ex: "[MyDriver]: "
    #define DBG_PREFIX "[" MY_DRIVER_NAME "]: "

    // #define MyKdPrint(format, ...) KdPrint((DBG_PREFIX format, __VA_ARGS__))
    // Above macro is giving errors when there are no arguments. So, created two macros to deal with no args and args case.
    #define MyKdPrint0(format) KdPrint((DBG_PREFIX format))
    #define MyKdPrint1(format, ...) KdPrint((DBG_PREFIX format, __VA_ARGS__))

    #define GET_MACRO(_1, _2, NAME, ...) NAME
    #define MyKdPrint(...) GET_MACRO(__VA_ARGS__, MyKdPrint1, MyKdPrint0)(__VA_ARGS__)


/************************************************************************
*   Macros to create:
*   1. static UNICODE_STRING
*   2. static OBJECT_ATTRIBUTES*
*  
*************************************************************************/

    // Macro to concatenate the line number with a label
    #define echo(x) x
    #define linelabel(x) echo(x)##__LINE__

    // Macro to initialize a UNICODE_STRING with a wide string literal
    #define RTL_CONSTANT_STRINGW_(s) { sizeof(s) - sizeof((s)[0]), sizeof(s), (PWSTR)(s) }

    // Macro to define a static UNICODE_STRING
    #define STATIC_UNICODE_STRING(varname, ansistr) \
        static const WCHAR linelabel(__)[] = echo(L)ansistr; \
        static const UNICODE_STRING varname = RTL_CONSTANT_STRINGW_(linelabel(__))

    // Macro to define a static OBJECT_ATTRIBUTES structure
    #define STATIC_OBJECT_ATTRIBUTES(oa, name) \
        STATIC_UNICODE_STRING(linelabel(m), name); \
        static OBJECT_ATTRIBUTES oa = { sizeof(oa), 0, (PUNICODE_STRING)(&label(m)), OBJ_CASE_INSENSITIVE }


/*********************************************************
*   UNICODE_STRING helper functions
* 
* 
* 
**********************************************************/

// Check if a UNICODE_STRING is valid or not
BOOLEAN IsValidUnicodeString(PCUNICODE_STRING str);

// Check if a UNICODE_STRING ends with a given suffix string
BOOLEAN UnicodeStringEndsWith(PCUNICODE_STRING FullString, PCUNICODE_STRING SuffixStr, BOOLEAN ExactMatch);

// Determine if the module is being loaded by LdrLoadDll() or not
BOOLEAN IsMappedByLdrLoadDll(PCUNICODE_STRING ModuleShortName);
