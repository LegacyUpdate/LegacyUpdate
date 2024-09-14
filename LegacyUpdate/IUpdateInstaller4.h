#pragma once
#include <wuapi.h>

// Copied from wuapi.h in Windows SDK 10.0.19041.0

#ifndef __IUpdateInstaller3_FWD_DEFINED__
#define __IUpdateInstaller3_FWD_DEFINED__
typedef interface IUpdateInstaller3 IUpdateInstaller3;

#endif 	/* __IUpdateInstaller3_FWD_DEFINED__ */

#ifndef __IUpdateInstaller4_FWD_DEFINED__
#define __IUpdateInstaller4_FWD_DEFINED__
typedef interface IUpdateInstaller4 IUpdateInstaller4;

#endif 	/* __IUpdateInstaller4_FWD_DEFINED__ */

// {16d11c35-099a-48d0-8338-5fae64047f8e}
DEFINE_GUID(IID_IUpdateInstaller3,0x16d11c35,0x099a,0x48d0,0x83,0x38,0x5f,0xae,0x64,0x04,0x7f,0x8e);

// {EF8208EA-2304-492D-9109-23813B0958E1}
DEFINE_GUID(IID_IUpdateInstaller4, 0xef8208ea, 0x2304, 0x492d, 0x91, 0x9, 0x23, 0x81, 0x3b, 0x9, 0x58, 0xe1);

#ifndef __IUpdateInstaller3_INTERFACE_DEFINED__
#define __IUpdateInstaller3_INTERFACE_DEFINED__

/* interface IUpdateInstaller3 */
/* [hidden][unique][uuid][nonextensible][dual][oleautomation][object][helpstring] */ 


EXTERN_C const IID IID_IUpdateInstaller3;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("16d11c35-099a-48d0-8338-5fae64047f8e")
    IUpdateInstaller3 : public IUpdateInstaller2
    {
    public:
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_AttemptCloseAppsIfNecessary( 
            /* [retval][out] */ __RPC__out VARIANT_BOOL *retval) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_AttemptCloseAppsIfNecessary( 
            /* [in] */ VARIANT_BOOL value) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUpdateInstaller3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IUpdateInstaller3 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IUpdateInstaller3 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [out] */ __RPC__out UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
            /* [range][in] */ __RPC__in_range(0,16384) UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUpdateInstaller3 * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ClientApplicationID )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [retval][out] */ __RPC__deref_out_opt BSTR *retval);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ClientApplicationID )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [in] */ __RPC__in BSTR value);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_IsForced )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *retval);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_IsForced )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [in] */ VARIANT_BOOL value);
        
        /* [helpstring][restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ParentHwnd )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [retval][out] */ __RPC__deref_out_opt HWND *retval);
        
        /* [helpstring][restricted][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ParentHwnd )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [unique][in] */ __RPC__in_opt HWND value);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ParentWindow )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [unique][in] */ __RPC__in_opt IUnknown *value);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ParentWindow )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [retval][out] */ __RPC__deref_out_opt IUnknown **retval);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Updates )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [retval][out] */ __RPC__deref_out_opt IUpdateCollection **retval);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Updates )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [in] */ __RPC__in_opt IUpdateCollection *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *BeginInstall )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [in] */ __RPC__in_opt IUnknown *onProgressChanged,
            /* [in] */ __RPC__in_opt IUnknown *onCompleted,
            /* [in] */ VARIANT state,
            /* [retval][out] */ __RPC__deref_out_opt IInstallationJob **retval);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *BeginUninstall )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [in] */ __RPC__in_opt IUnknown *onProgressChanged,
            /* [in] */ __RPC__in_opt IUnknown *onCompleted,
            /* [in] */ VARIANT state,
            /* [retval][out] */ __RPC__deref_out_opt IInstallationJob **retval);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EndInstall )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [in] */ __RPC__in_opt IInstallationJob *value,
            /* [retval][out] */ __RPC__deref_out_opt IInstallationResult **retval);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EndUninstall )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [in] */ __RPC__in_opt IInstallationJob *value,
            /* [retval][out] */ __RPC__deref_out_opt IInstallationResult **retval);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Install )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [retval][out] */ __RPC__deref_out_opt IInstallationResult **retval);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RunWizard )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [defaultvalue][unique][in] */ __RPC__in_opt BSTR dialogTitle,
            /* [retval][out] */ __RPC__deref_out_opt IInstallationResult **retval);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_IsBusy )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *retval);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Uninstall )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [retval][out] */ __RPC__deref_out_opt IInstallationResult **retval);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_AllowSourcePrompts )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *retval);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_AllowSourcePrompts )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [in] */ VARIANT_BOOL value);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_RebootRequiredBeforeInstallation )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *retval);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ForceQuiet )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *retval);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ForceQuiet )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [in] */ VARIANT_BOOL value);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_AttemptCloseAppsIfNecessary )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *retval);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_AttemptCloseAppsIfNecessary )( 
            __RPC__in IUpdateInstaller3 * This,
            /* [in] */ VARIANT_BOOL value);
        
        END_INTERFACE
    } IUpdateInstaller3Vtbl;

    interface IUpdateInstaller3
    {
        CONST_VTBL struct IUpdateInstaller3Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUpdateInstaller3_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUpdateInstaller3_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUpdateInstaller3_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUpdateInstaller3_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUpdateInstaller3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUpdateInstaller3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUpdateInstaller3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUpdateInstaller3_get_ClientApplicationID(This,retval)	\
    ( (This)->lpVtbl -> get_ClientApplicationID(This,retval) ) 

#define IUpdateInstaller3_put_ClientApplicationID(This,value)	\
    ( (This)->lpVtbl -> put_ClientApplicationID(This,value) ) 

#define IUpdateInstaller3_get_IsForced(This,retval)	\
    ( (This)->lpVtbl -> get_IsForced(This,retval) ) 

#define IUpdateInstaller3_put_IsForced(This,value)	\
    ( (This)->lpVtbl -> put_IsForced(This,value) ) 

#define IUpdateInstaller3_get_ParentHwnd(This,retval)	\
    ( (This)->lpVtbl -> get_ParentHwnd(This,retval) ) 

#define IUpdateInstaller3_put_ParentHwnd(This,value)	\
    ( (This)->lpVtbl -> put_ParentHwnd(This,value) ) 

#define IUpdateInstaller3_put_ParentWindow(This,value)	\
    ( (This)->lpVtbl -> put_ParentWindow(This,value) ) 

#define IUpdateInstaller3_get_ParentWindow(This,retval)	\
    ( (This)->lpVtbl -> get_ParentWindow(This,retval) ) 

#define IUpdateInstaller3_get_Updates(This,retval)	\
    ( (This)->lpVtbl -> get_Updates(This,retval) ) 

#define IUpdateInstaller3_put_Updates(This,value)	\
    ( (This)->lpVtbl -> put_Updates(This,value) ) 

#define IUpdateInstaller3_BeginInstall(This,onProgressChanged,onCompleted,state,retval)	\
    ( (This)->lpVtbl -> BeginInstall(This,onProgressChanged,onCompleted,state,retval) ) 

#define IUpdateInstaller3_BeginUninstall(This,onProgressChanged,onCompleted,state,retval)	\
    ( (This)->lpVtbl -> BeginUninstall(This,onProgressChanged,onCompleted,state,retval) ) 

#define IUpdateInstaller3_EndInstall(This,value,retval)	\
    ( (This)->lpVtbl -> EndInstall(This,value,retval) ) 

#define IUpdateInstaller3_EndUninstall(This,value,retval)	\
    ( (This)->lpVtbl -> EndUninstall(This,value,retval) ) 

#define IUpdateInstaller3_Install(This,retval)	\
    ( (This)->lpVtbl -> Install(This,retval) ) 

#define IUpdateInstaller3_RunWizard(This,dialogTitle,retval)	\
    ( (This)->lpVtbl -> RunWizard(This,dialogTitle,retval) ) 

#define IUpdateInstaller3_get_IsBusy(This,retval)	\
    ( (This)->lpVtbl -> get_IsBusy(This,retval) ) 

#define IUpdateInstaller3_Uninstall(This,retval)	\
    ( (This)->lpVtbl -> Uninstall(This,retval) ) 

#define IUpdateInstaller3_get_AllowSourcePrompts(This,retval)	\
    ( (This)->lpVtbl -> get_AllowSourcePrompts(This,retval) ) 

#define IUpdateInstaller3_put_AllowSourcePrompts(This,value)	\
    ( (This)->lpVtbl -> put_AllowSourcePrompts(This,value) ) 

#define IUpdateInstaller3_get_RebootRequiredBeforeInstallation(This,retval)	\
    ( (This)->lpVtbl -> get_RebootRequiredBeforeInstallation(This,retval) ) 


#define IUpdateInstaller3_get_ForceQuiet(This,retval)	\
    ( (This)->lpVtbl -> get_ForceQuiet(This,retval) ) 

#define IUpdateInstaller3_put_ForceQuiet(This,value)	\
    ( (This)->lpVtbl -> put_ForceQuiet(This,value) ) 


#define IUpdateInstaller3_get_AttemptCloseAppsIfNecessary(This,retval)	\
    ( (This)->lpVtbl -> get_AttemptCloseAppsIfNecessary(This,retval) ) 

#define IUpdateInstaller3_put_AttemptCloseAppsIfNecessary(This,value)	\
    ( (This)->lpVtbl -> put_AttemptCloseAppsIfNecessary(This,value) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUpdateInstaller3_INTERFACE_DEFINED__ */


#ifndef __IUpdateInstaller4_INTERFACE_DEFINED__
#define __IUpdateInstaller4_INTERFACE_DEFINED__

/* interface IUpdateInstaller4 */
/* [hidden][unique][uuid][nonextensible][dual][oleautomation][object][helpstring] */ 


EXTERN_C const IID IID_IUpdateInstaller4;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EF8208EA-2304-492D-9109-23813B0958E1")
    IUpdateInstaller4 : public IUpdateInstaller3
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Commit( 
            /* [in] */ DWORD dwFlags) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUpdateInstaller4Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IUpdateInstaller4 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IUpdateInstaller4 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [out] */ __RPC__out UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
            /* [range][in] */ __RPC__in_range(0,16384) UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUpdateInstaller4 * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ClientApplicationID )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [retval][out] */ __RPC__deref_out_opt BSTR *retval);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ClientApplicationID )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [in] */ __RPC__in BSTR value);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_IsForced )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *retval);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_IsForced )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [in] */ VARIANT_BOOL value);
        
        /* [helpstring][restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ParentHwnd )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [retval][out] */ __RPC__deref_out_opt HWND *retval);
        
        /* [helpstring][restricted][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ParentHwnd )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [unique][in] */ __RPC__in_opt HWND value);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ParentWindow )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [unique][in] */ __RPC__in_opt IUnknown *value);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ParentWindow )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [retval][out] */ __RPC__deref_out_opt IUnknown **retval);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Updates )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [retval][out] */ __RPC__deref_out_opt IUpdateCollection **retval);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Updates )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [in] */ __RPC__in_opt IUpdateCollection *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *BeginInstall )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [in] */ __RPC__in_opt IUnknown *onProgressChanged,
            /* [in] */ __RPC__in_opt IUnknown *onCompleted,
            /* [in] */ VARIANT state,
            /* [retval][out] */ __RPC__deref_out_opt IInstallationJob **retval);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *BeginUninstall )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [in] */ __RPC__in_opt IUnknown *onProgressChanged,
            /* [in] */ __RPC__in_opt IUnknown *onCompleted,
            /* [in] */ VARIANT state,
            /* [retval][out] */ __RPC__deref_out_opt IInstallationJob **retval);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EndInstall )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [in] */ __RPC__in_opt IInstallationJob *value,
            /* [retval][out] */ __RPC__deref_out_opt IInstallationResult **retval);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EndUninstall )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [in] */ __RPC__in_opt IInstallationJob *value,
            /* [retval][out] */ __RPC__deref_out_opt IInstallationResult **retval);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Install )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [retval][out] */ __RPC__deref_out_opt IInstallationResult **retval);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RunWizard )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [defaultvalue][unique][in] */ __RPC__in_opt BSTR dialogTitle,
            /* [retval][out] */ __RPC__deref_out_opt IInstallationResult **retval);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_IsBusy )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *retval);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Uninstall )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [retval][out] */ __RPC__deref_out_opt IInstallationResult **retval);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_AllowSourcePrompts )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *retval);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_AllowSourcePrompts )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [in] */ VARIANT_BOOL value);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_RebootRequiredBeforeInstallation )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *retval);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ForceQuiet )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *retval);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ForceQuiet )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [in] */ VARIANT_BOOL value);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_AttemptCloseAppsIfNecessary )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *retval);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_AttemptCloseAppsIfNecessary )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [in] */ VARIANT_BOOL value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Commit )( 
            __RPC__in IUpdateInstaller4 * This,
            /* [in] */ DWORD dwFlags);
        
        END_INTERFACE
    } IUpdateInstaller4Vtbl;

    interface IUpdateInstaller4
    {
        CONST_VTBL struct IUpdateInstaller4Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUpdateInstaller4_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUpdateInstaller4_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUpdateInstaller4_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUpdateInstaller4_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUpdateInstaller4_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUpdateInstaller4_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUpdateInstaller4_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUpdateInstaller4_get_ClientApplicationID(This,retval)	\
    ( (This)->lpVtbl -> get_ClientApplicationID(This,retval) ) 

#define IUpdateInstaller4_put_ClientApplicationID(This,value)	\
    ( (This)->lpVtbl -> put_ClientApplicationID(This,value) ) 

#define IUpdateInstaller4_get_IsForced(This,retval)	\
    ( (This)->lpVtbl -> get_IsForced(This,retval) ) 

#define IUpdateInstaller4_put_IsForced(This,value)	\
    ( (This)->lpVtbl -> put_IsForced(This,value) ) 

#define IUpdateInstaller4_get_ParentHwnd(This,retval)	\
    ( (This)->lpVtbl -> get_ParentHwnd(This,retval) ) 

#define IUpdateInstaller4_put_ParentHwnd(This,value)	\
    ( (This)->lpVtbl -> put_ParentHwnd(This,value) ) 

#define IUpdateInstaller4_put_ParentWindow(This,value)	\
    ( (This)->lpVtbl -> put_ParentWindow(This,value) ) 

#define IUpdateInstaller4_get_ParentWindow(This,retval)	\
    ( (This)->lpVtbl -> get_ParentWindow(This,retval) ) 

#define IUpdateInstaller4_get_Updates(This,retval)	\
    ( (This)->lpVtbl -> get_Updates(This,retval) ) 

#define IUpdateInstaller4_put_Updates(This,value)	\
    ( (This)->lpVtbl -> put_Updates(This,value) ) 

#define IUpdateInstaller4_BeginInstall(This,onProgressChanged,onCompleted,state,retval)	\
    ( (This)->lpVtbl -> BeginInstall(This,onProgressChanged,onCompleted,state,retval) ) 

#define IUpdateInstaller4_BeginUninstall(This,onProgressChanged,onCompleted,state,retval)	\
    ( (This)->lpVtbl -> BeginUninstall(This,onProgressChanged,onCompleted,state,retval) ) 

#define IUpdateInstaller4_EndInstall(This,value,retval)	\
    ( (This)->lpVtbl -> EndInstall(This,value,retval) ) 

#define IUpdateInstaller4_EndUninstall(This,value,retval)	\
    ( (This)->lpVtbl -> EndUninstall(This,value,retval) ) 

#define IUpdateInstaller4_Install(This,retval)	\
    ( (This)->lpVtbl -> Install(This,retval) ) 

#define IUpdateInstaller4_RunWizard(This,dialogTitle,retval)	\
    ( (This)->lpVtbl -> RunWizard(This,dialogTitle,retval) ) 

#define IUpdateInstaller4_get_IsBusy(This,retval)	\
    ( (This)->lpVtbl -> get_IsBusy(This,retval) ) 

#define IUpdateInstaller4_Uninstall(This,retval)	\
    ( (This)->lpVtbl -> Uninstall(This,retval) ) 

#define IUpdateInstaller4_get_AllowSourcePrompts(This,retval)	\
    ( (This)->lpVtbl -> get_AllowSourcePrompts(This,retval) ) 

#define IUpdateInstaller4_put_AllowSourcePrompts(This,value)	\
    ( (This)->lpVtbl -> put_AllowSourcePrompts(This,value) ) 

#define IUpdateInstaller4_get_RebootRequiredBeforeInstallation(This,retval)	\
    ( (This)->lpVtbl -> get_RebootRequiredBeforeInstallation(This,retval) ) 


#define IUpdateInstaller4_get_ForceQuiet(This,retval)	\
    ( (This)->lpVtbl -> get_ForceQuiet(This,retval) ) 

#define IUpdateInstaller4_put_ForceQuiet(This,value)	\
    ( (This)->lpVtbl -> put_ForceQuiet(This,value) ) 


#define IUpdateInstaller4_get_AttemptCloseAppsIfNecessary(This,retval)	\
    ( (This)->lpVtbl -> get_AttemptCloseAppsIfNecessary(This,retval) ) 

#define IUpdateInstaller4_put_AttemptCloseAppsIfNecessary(This,value)	\
    ( (This)->lpVtbl -> put_AttemptCloseAppsIfNecessary(This,value) ) 


#define IUpdateInstaller4_Commit(This,dwFlags)	\
    ( (This)->lpVtbl -> Commit(This,dwFlags) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUpdateInstaller4_INTERFACE_DEFINED__ */
