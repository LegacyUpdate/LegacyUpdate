#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include "main.h"
#include "resource.h"
#include "CertRolloverDeadline.h"
#include "Options.h"
#include "Exec.h"
#include "HResult.h"
#include "LoadImage.h"
#include "MsgBox.h"
#include "Registry.h"
#include "SelfElevate.h"
#include "User.h"
#include "VersionInfo.h"
#include "Wow64.h"
#include "WsusServer.h"

static WsusServer g_wsusServer = e_none;

HRESULT OpenWuSettings() {
#if !_WIN64
	// Some issues arise from the working directory being SysWOW64 rather than System32. Notably,
	// Windows Vista - 8.1 don't have wuauclt.exe in SysWOW64. Disable WOW64 redirection temporarily
	// to work around this.
	PVOID oldValue = NULL;
	BOOL isRedirected = DisableWow64FsRedirection(&oldValue);
#endif

	HRESULT hr = E_FAIL;

	if (AtLeastWin10()) {
		// Windows 10+: Open Settings app
		hr = Exec(NULL, L"ms-settings:windowsupdate-options", NULL, NULL, SW_SHOWDEFAULT, FALSE, NULL);
	} else if (AtLeastWinVista()) {
		// Windows Vista, 7, 8: Open Windows Update control panel
		WCHAR control[MAX_PATH];
		ExpandEnvironmentStrings(L"%SystemRoot%\\System32\\control.exe", control, ARRAYSIZE(control));
		hr = Exec(NULL, control, L"/name Microsoft.WindowsUpdate /page pageSettings", NULL, SW_SHOWDEFAULT, FALSE, NULL);
	} else {
		// Windows 2000, XP: Open Automatic Updates control panel
		WCHAR wuaucpl[MAX_PATH];
		ExpandEnvironmentStrings(L"%SystemRoot%\\System32\\wuaucpl.cpl", wuaucpl, ARRAYSIZE(wuaucpl));
		hr = Exec(NULL, wuaucpl, NULL, NULL, SW_SHOWDEFAULT, FALSE, NULL);
	}

#if !_WIN64
	// Revert WOW64 redirection if we changed it
	if (isRedirected) {
		RevertWow64FsRedirection(oldValue);
	}
#endif

	return hr;
}

void LaunchWuSettings() {
	PostQuitMessage(OpenWuSettings());
}

static INT_PTR CALLBACK ServerPageProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_INITDIALOG: {
		HICON icon = LoadSystemIcon(L"%SystemRoot%\\System32\\shell32.dll", MAKEINTRESOURCE(18));
		SendDlgItemMessage(hwnd, IDC_PAGEICON, STM_SETICON, (WPARAM)icon, 0);

		UINT wuTextId = 0;
		BOOL wuBroken = AtMostWinVista() || !IsBeforeCertRolloverDeadline();
		if (wuBroken) {
			wuTextId = IDS_WURADIO_DEADLINE;
			EnableWindow(GetDlgItem(hwnd, IDC_WURADIO), FALSE);

			// Make the other radio the tab stop
			HWND luRadio = GetDlgItem(hwnd, IDC_LURADIO);
			SetWindowLongPtr(luRadio, GWL_STYLE, GetWindowLongPtr(luRadio, GWL_STYLE) | WS_TABSTOP);
		} else if (AtLeastWin10_1809() && !AtLeastWin11_25H2()) {
			wuTextId = IDS_WURADIO_WIN10_1809;
		}

		if (wuTextId != 0) {
			WCHAR wuText[4096];
			LoadString(g_hInstance, wuTextId, wuText, ARRAYSIZE(wuText));
			SetDlgItemText(hwnd, IDC_WUTEXT, wuText);
		}

		GetWsusServer(&g_wsusServer);
		switch (g_wsusServer) {
		case e_none:
			if (wuBroken) {
				CheckDlgButton(hwnd, IDC_LURADIO, BST_CHECKED);
				PropSheet_Changed(GetParent(hwnd), hwnd);
			} else {
				CheckDlgButton(hwnd, IDC_WURADIO, BST_CHECKED);
			}
			break;

		case e_legacyUpdateProxy:
			CheckDlgButton(hwnd, IDC_LURADIO, BST_CHECKED);
			break;

		default:
			break;
		}

		return TRUE;
	}

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_WURADIO:
		case IDC_LURADIO:
			if (HIWORD(wParam) == BN_CLICKED) {
				PropSheet_Changed(GetParent(hwnd), hwnd);
			}

			break;
		}

		break;

	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code) {
		case PSN_APPLY: {
			WsusServer server = g_wsusServer;
			if (IsDlgButtonChecked(hwnd, IDC_WURADIO) == BST_CHECKED) {
				server = e_none;
			} else if (IsDlgButtonChecked(hwnd, IDC_LURADIO) == BST_CHECKED) {
				server = e_legacyUpdateProxy;
			}

			// Make sure we don’t mess with an existing WSUS server
			// TODO: Display this state more clearly
			if (server != g_wsusServer) {
				HRESULT hr = SetWsusServer(server);
				if (!SUCCEEDED(hr)) {
					LPWSTR message = GetMessageForHresult(hr);
					MsgBox(hwnd, message, NULL, MB_ICONEXCLAMATION);
					LocalFree(message);

					SetWindowLongPtr(hwnd, DWLP_MSGRESULT, PSNRET_INVALID_NOCHANGEPAGE);
					return TRUE;
				}

				g_wsusServer = server;
			}

			SetWindowLongPtr(hwnd, DWLP_MSGRESULT, PSNRET_NOERROR);
			return TRUE;
		}
		}
		break;

	case WM_DESTROY:
		DestroyIcon((HICON)SendDlgItemMessage(hwnd, IDC_PAGEICON, STM_GETICON, 0, 0));
		break;
	}

	return FALSE;
}

static INT_PTR CALLBACK AutoUpdatePageProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_INITDIALOG: {
		SendDlgItemMessage(hwnd, IDC_PAGEICON, STM_SETICON, (WPARAM)LoadWuIcon(), 0);
		return TRUE;
	}

	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_WUSETTINGS && HIWORD(wParam) == BN_CLICKED) {
			HRESULT hr = OpenWuSettings();
			if (!SUCCEEDED(hr)) {
				LPWSTR message = GetMessageForHresult(hr);
				MsgBox(hwnd, message, NULL, MB_ICONEXCLAMATION);
				LocalFree(message);
			}
		}
		break;

	case WM_DESTROY:
		DestroyIcon((HICON)SendDlgItemMessage(hwnd, IDC_PAGEICON, STM_GETICON, 0, 0));
		break;
	}

	return FALSE;
}

static BOOL CALLBACK AddPageCallback(HPROPSHEETPAGE page, LPARAM lParam) {
	PROPSHEETHEADER *header = (PROPSHEETHEADER *)lParam;
	if (header->nPages > 2) {
		return FALSE;
	}

	header->phpage[header->nPages++] = page;
	return TRUE;
}

static IShellPropSheetExt *AddAutoUpdatePage(PROPSHEETHEADER *header) {
	LPWSTR clsidString = NULL;
	HRESULT hr = GetRegistryString(HKEY_LOCAL_MACHINE, REGPATH_PROPSHEET_AU, NULL, 0, &clsidString, NULL);
	if (!SUCCEEDED(hr)) {
		return NULL;
	}

	CLSID clsid;
	hr = CLSIDFromString(clsidString, &clsid);
	LocalFree(clsidString);
	if (!SUCCEEDED(hr)) {
		return NULL;
	}

	IShellPropSheetExt *propSheet = NULL;
	hr = CoCreateInstance(&clsid, NULL, CLSCTX_INPROC_SERVER, &IID_IShellPropSheetExt, (void **)&propSheet);
	if (!SUCCEEDED(hr)) {
		CHECK_HR(L"CoCreateInstance IID_IShellPropSheetExt");
		return NULL;
	}

	IShellExtInit *init = NULL;
	if (SUCCEEDED(IShellPropSheetExt_QueryInterface(propSheet, &IID_IShellExtInit, (void **)&init))) {
		IShellExtInit_Initialize(init, NULL, NULL, NULL);
		IShellExtInit_Release(init);
	}

	UINT pageCount = header->nPages;
	IShellPropSheetExt_AddPages(propSheet, AddPageCallback, (LPARAM)header);
	if (header->nPages == pageCount) {
		IShellPropSheetExt_Release(propSheet);
		return NULL;
	}

	return propSheet;
}

static HPROPSHEETPAGE CreatePage(UINT templateId, DLGPROC dlgProc) {
	PROPSHEETPAGE page = {0};
	page.dwSize = sizeof(PROPSHEETPAGE);
	page.hInstance = g_hInstance;
	page.pszTemplate = MAKEINTRESOURCE(templateId);
	page.pfnDlgProc = dlgProc;
	return CreatePropertySheetPage(&page);
}

static int CALLBACK PropSheetCallback(HWND hwnd, UINT message, LPARAM lParam) {
	switch (message) {
	case PSCB_INITIALIZED: {
		// Fix taskbar icon being 16x16 only
		HICON icon = LoadImage(g_hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_SHARED);
		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)icon);
		break;
	}
	}

	return 0;
}

void LaunchOptions() {
	// Ensure elevation
	HRESULT hr = S_OK;
	HPROPSHEETPAGE pages[2] = {0};
	IShellPropSheetExt *autoUpdateExt = NULL;

	if (!IsUserAdmin()) {
		hr = SelfElevate(L"/options", NULL);

		// Access denied happens when the user clicks No/Cancel.
		if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
			hr = S_OK;
		}

		goto end;
	}

	WCHAR title[256];
	LoadString(g_hInstance, IDS_LEGACYUPDATE, title, ARRAYSIZE(title));

	PROPSHEETHEADER header = {0};
	header.dwSize = sizeof(PROPSHEETHEADER);
	header.dwFlags = PSH_USEICONID | PSH_USECALLBACK | PSH_NOCONTEXTHELP;
	header.hInstance = g_hInstance;
	header.pszIcon = MAKEINTRESOURCE(IDI_APPICON);
	header.pszCaption = title;
	header.phpage = pages;
	header.pfnCallback = PropSheetCallback;

	pages[header.nPages++] = CreatePage(IDD_OPTIONS_SERVER, ServerPageProc);

	// Use Automatic Updates control panel if possible, otherwise show our own page
	autoUpdateExt = AddAutoUpdatePage(&header);
	if (!autoUpdateExt) {
		pages[header.nPages++] = CreatePage(IDD_OPTIONS_AUTOUPDATE, AutoUpdatePageProc);
	}

	if (PropertySheet(&header) == -1) {
		hr = HRESULT_FROM_WIN32(GetLastError());
	}

end:
	if (autoUpdateExt) {
		IShellPropSheetExt_Release(autoUpdateExt);
	}

	PostQuitMessage(hr);
}
