#pragma once

// LegacyUpdateCtrl.h : Declaration of the CLegacyUpdateCtrl ActiveX Control class.

// CLegacyUpdateCtrl : See LegacyUpdateCtrl.cpp for implementation.

#include <afxctl.h>
#include <MsHTML.h>

class CLegacyUpdateCtrl : public COleControl {
	DECLARE_DYNCREATE(CLegacyUpdateCtrl)

public:
	// Constructor
	CLegacyUpdateCtrl();

	typedef enum {
		e_majorVer = 0,
		e_minorVer = 1,
		e_buildNumber = 2,
		e_platform = 3,
		e_SPMajor = 4,
		e_SPMinor = 5,
		e_productSuite = 6,
		e_productType = 7,
		e_systemMetric = 8,
		e_SPVersionString = 9,
		e_controlVersionString = 10,
		e_VistaProductType = 11,
		e_maxOSVersionFieldValue = 11
	} OSVersionField;

	typedef enum {
		e_nonAdmin = 0,
		e_admin = 2
	} UserType;

	// COleControl
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();

protected:
	~CLegacyUpdateCtrl();

	DECLARE_OLECREATE_EX(CLegacyUpdateCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CLegacyUpdateCtrl)      // GetTypeInfo
	DECLARE_OLECTLTYPE(CLegacyUpdateCtrl)      // Type name and misc status

	// Message maps
	DECLARE_MESSAGE_MAP()

	// Dispatch maps
	DECLARE_DISPATCH_MAP()

	// Event maps
	DECLARE_EVENT_MAP()

	IHTMLDocument2 *GetHTMLDocument();
	BOOL IsPermitted();

public:
	// Dispatch and event IDs
	enum {
		dispidCheckControl = 1L,
		dispidMessageForHresult = 2L,
		dispidGetOSVersionInfo = 3L,
		dispidCreateObject = 4L,
		dispidGetUserType = 5L,
		dispidIsRebootRequired = 6L,
		dispidIsWindowsUpdateDisabled = 7L,
		dispidRebootIfRequired = 8L,
		dispidViewWindowsUpdateLog = 9L,
		dispidIsUsingWsusServer = 10L,
		dispidWsusServerUrl = 11L,
		dispidWsusStatusServerUrl = 12L
	};

protected:
	VARIANT_BOOL CheckControl(void);
	BSTR MessageForHresult(LONG inHresult);
	VARIANT GetOSVersionInfo(OSVersionField osField, LONG systemMetric);
	IDispatch *CreateComObject(BSTR progID);
	UserType GetUserType(void);
	VARIANT_BOOL IsRebootRequired(void);
	VARIANT_BOOL IsWindowsUpdateDisabled(void);
	void RebootIfRequired(void);
	void ViewWindowsUpdateLog(void);
	VARIANT_BOOL IsUsingWsusServer(void);
	BSTR WsusServerUrl(void);
	BSTR WsusStatusServerUrl(void);
};

