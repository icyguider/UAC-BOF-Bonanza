/* IColorDataProxy.h - header for COM object IColorDataProxy*/
#pragma once
#include <initguid.h> 
#include <combaseapi.h>

// define the export for linking
DEFINE_GUID(IID_IColorDataProxy, 0x0A16D195, 0x6F47, 0x4964, 0x92, 0x87, 0x9F, 0x4B, 0xAB, 0x6D, 0x98, 0x27);

// CLSID string
#define T_CLSID_ColorDataProxy L"{D2E7041B-2927-42fb-8E9F-7CE93B6DC937}"

// interface
typedef interface IColorDataProxy IColorDataProxy;

typedef struct IColorDataProxyVtbl {

    BEGIN_INTERFACE

        HRESULT(STDMETHODCALLTYPE* QueryInterface)(
            __RPC__in IColorDataProxy* This,
            __RPC__in REFIID riid,
            _COM_Outptr_  void** ppvObject);

    ULONG(STDMETHODCALLTYPE* AddRef)(
        __RPC__in IColorDataProxy* This);

    ULONG(STDMETHODCALLTYPE* Release)(
        __RPC__in IColorDataProxy* This);

    HRESULT(STDMETHODCALLTYPE* Method1)(
        __RPC__in IColorDataProxy* This);

    HRESULT(STDMETHODCALLTYPE* Method2)(
        __RPC__in IColorDataProxy* This);

    HRESULT(STDMETHODCALLTYPE* Method3)(
        __RPC__in IColorDataProxy* This);

    HRESULT(STDMETHODCALLTYPE* Method4)(
        __RPC__in IColorDataProxy* This);

    HRESULT(STDMETHODCALLTYPE* Method5)(
        __RPC__in IColorDataProxy* This);

    HRESULT(STDMETHODCALLTYPE* Method6)(
        __RPC__in IColorDataProxy* This);

    HRESULT(STDMETHODCALLTYPE* Method7)(
        __RPC__in IColorDataProxy* This);

    HRESULT(STDMETHODCALLTYPE* Method8)(
        __RPC__in IColorDataProxy* This);

    HRESULT(STDMETHODCALLTYPE* Method9)(
        __RPC__in IColorDataProxy* This);

    HRESULT(STDMETHODCALLTYPE* Method10)(
        __RPC__in IColorDataProxy* This);

    HRESULT(STDMETHODCALLTYPE* Method11)(
        __RPC__in IColorDataProxy* This);

    HRESULT(STDMETHODCALLTYPE* LaunchDccw)(
        __RPC__in IColorDataProxy* This,
        _In_      HWND hwnd);

    END_INTERFACE

} *PIColorDataProxyVtbl;

// object vtable
interface IColorDataProxy 
{ 
    CONST_VTBL struct IColorDataProxyVtbl* lpVtbl; 
};