/* ICMLuaUtil.h - header for COM object ICMLuaUtil*/
#pragma once
#include <initguid.h> 

// define the export for linking
DEFINE_GUID(IID_ICMLuaUtil, 0x6EDD6D74, 0xC007, 0x4E75, 0xB7, 0x6A, 0xE5, 0x74, 0x09, 0x95, 0xE2, 0x4C);

// CLSID string
#define T_IID_ICMLuaUtil L"{6EDD6D74-C007-4E75-B76A-E5740995E24C}"
#define T_CLSID_CMSTPLUA L"{3E5FC7F9-9A51-4367-9063-A120244FBEC7}"

// interface
typedef interface ICMLuaUtil ICMLuaUtil;

typedef struct ICMLuaUtilVtbl {

    BEGIN_INTERFACE

        HRESULT(STDMETHODCALLTYPE* QueryInterface)(
            __RPC__in ICMLuaUtil* This,
            __RPC__in REFIID riid,
            _COM_Outptr_  void** ppvObject);

    ULONG(STDMETHODCALLTYPE* AddRef)(
        __RPC__in ICMLuaUtil* This);

    ULONG(STDMETHODCALLTYPE* Release)(
        __RPC__in ICMLuaUtil* This);

    //incomplete definition
    HRESULT(STDMETHODCALLTYPE* SetRasCredentials)(
        __RPC__in ICMLuaUtil* This);

    //incomplete definition
    HRESULT(STDMETHODCALLTYPE* SetRasEntryProperties)(
        __RPC__in ICMLuaUtil* This);

    //incomplete definition
    HRESULT(STDMETHODCALLTYPE* DeleteRasEntry)(
        __RPC__in ICMLuaUtil* This);

    //incomplete definition
    HRESULT(STDMETHODCALLTYPE* LaunchInfSection)(
        __RPC__in ICMLuaUtil* This);

    //incomplete definition
    HRESULT(STDMETHODCALLTYPE* LaunchInfSectionEx)(
        __RPC__in ICMLuaUtil* This);

    //incomplete definition
    HRESULT(STDMETHODCALLTYPE* CreateLayerDirectory)(
        __RPC__in ICMLuaUtil* This);

    HRESULT(STDMETHODCALLTYPE* ShellExec)(
        __RPC__in ICMLuaUtil* This,
        _In_     LPCTSTR lpFile,
        _In_opt_  LPCTSTR lpParameters,
        _In_opt_  LPCTSTR lpDirectory,
        _In_      ULONG fMask,
        _In_      ULONG nShow);

    HRESULT(STDMETHODCALLTYPE* SetRegistryStringValue)(
        __RPC__in ICMLuaUtil* This,
        _In_      HKEY hKey,
        _In_opt_  LPWSTR lpSubKey,
        _In_opt_  LPWSTR lpValueName,
        _In_      LPWSTR lpValueString);

    HRESULT(STDMETHODCALLTYPE* DeleteRegistryStringValue)(
        __RPC__in ICMLuaUtil* This,
        _In_      HKEY hKey,
        _In_      LPWSTR lpSubKey,
        _In_      LPWSTR lpValueName);

    //incomplete definition
    HRESULT(STDMETHODCALLTYPE* DeleteRegKeysWithoutSubKeys)(
        __RPC__in ICMLuaUtil* This);

    //incomplete definition
    HRESULT(STDMETHODCALLTYPE* DeleteRegTree)(
        __RPC__in ICMLuaUtil* This);

    HRESULT(STDMETHODCALLTYPE* ExitWindowsFunc)(
        __RPC__in ICMLuaUtil* This);

    //incomplete definition
    HRESULT(STDMETHODCALLTYPE* AllowAccessToTheWorld)(
        __RPC__in ICMLuaUtil* This);

    //incomplete definition
    HRESULT(STDMETHODCALLTYPE* CreateFileAndClose)(
        __RPC__in ICMLuaUtil* This);

    //incomplete definition
    HRESULT(STDMETHODCALLTYPE* DeleteHiddenCmProfileFiles)(
        __RPC__in ICMLuaUtil* This);

    //incomplete definition
    HRESULT(STDMETHODCALLTYPE* CallCustomActionDll)(
        __RPC__in ICMLuaUtil* This);

    HRESULT(STDMETHODCALLTYPE* RunCustomActionExe)(
        __RPC__in       ICMLuaUtil* This,
        _In_            LPCTSTR lpFile,
        _In_opt_        LPCTSTR lpParameters,
        _COM_Outptr_    LPCTSTR* pszHandleAsHexString);

    //incomplete definition
    HRESULT(STDMETHODCALLTYPE* SetRasSubEntryProperties)(
        __RPC__in ICMLuaUtil* This);

    //incomplete definition
    HRESULT(STDMETHODCALLTYPE* DeleteRasSubEntry)(
        __RPC__in ICMLuaUtil* This);

    //incomplete definition
    HRESULT(STDMETHODCALLTYPE* SetCustomAuthData)(
        __RPC__in ICMLuaUtil* This);

    END_INTERFACE
} *PICMLuaUtilVtbl;

// vtable for object from struct
interface ICMLuaUtil
{
    CONST_VTBL struct ICMLuaUtilVtbl* lpVtbl;
};