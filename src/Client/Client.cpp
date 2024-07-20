#include <windows.h>

#include <stdio.h>
#include "..\Basic_Driver\common.h"

int main()
{
    HANDLE hDevice = CreateFile(CLIENT_DEVICE, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL); // L"\\\\.\\EDRDriver"

    if (hDevice == INVALID_HANDLE_VALUE)
    {
        printf("CreateFile failed with error %u\n", GetLastError());
        return 1;
    }
    
    printf("Successfully got handle to device\n");
    
    CloseHandle(hDevice);
	return 0;
}