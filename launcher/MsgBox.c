#include "stdafx.h"
#include "main.h"
#include "resource.h"

#undef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_VISTA
#include <commctrl.h>

typedef HRESULT (WINAPI *_TaskDialogIndirect)(const TASKDIALOGCONFIG *pTaskConfig, int *pnButton, int *pnRadioButton, BOOL *pfVerificationFlagChecked);

static BOOL _loadedTaskDialog = FALSE;
static _TaskDialogIndirect $TaskDialogIndirect;

int MsgBox(HWND hWnd, LPCTSTR instruction, LPCTSTR body, UINT type) {
	if (!_loadedTaskDialog) {
		_loadedTaskDialog = TRUE;
		$TaskDialogIndirect = (_TaskDialogIndirect)GetProcAddress(LoadLibrary(L"comctl32.dll"), "TaskDialogIndirect");
	}

	if (!$TaskDialogIndirect) {
		LPCTSTR finalBody = body;
		if (instruction) {
			size_t length = lstrlen(instruction) + lstrlen(body) + 3;
			finalBody = (LPCTSTR)LocalAlloc(LPTR, length * sizeof(TCHAR));
			wsprintf((LPTSTR)finalBody, L"%s\n\n%s", instruction, body);
		}

		MSGBOXPARAMS params = { 0 };
		params.cbSize = sizeof(MSGBOXPARAMS);
		params.hwndOwner = hWnd;
		params.hInstance = g_hInstance;
		params.lpszText = finalBody;
		params.lpszCaption = L"Legacy Update";
		params.dwStyle = type | MB_USERICON;
		params.lpszIcon = MAKEINTRESOURCE(IDI_APPICON);
		int result = MessageBoxIndirect(&params);

		if (finalBody != body) {
			LocalFree((HLOCAL)finalBody);
		}

		return result;
	}

	TASKDIALOG_COMMON_BUTTON_FLAGS buttons;

	switch (type & 0x0000000F) {
	case MB_OK:
		buttons = TDCBF_OK_BUTTON;
		break;
	case MB_OKCANCEL:
		buttons = TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON;
		break;
	case MB_YESNO:
		buttons = TDCBF_YES_BUTTON | TDCBF_NO_BUTTON;
		break;
	default:
		break;
	}

	TASKDIALOGCONFIG config = { 0 };
	config.cbSize = sizeof(TASKDIALOGCONFIG);
	config.hwndParent = hWnd;
	config.hInstance = GetModuleHandle(NULL);
	config.dwCommonButtons = buttons;
	config.pszWindowTitle = L"Legacy Update";
	config.pszMainInstruction = instruction;
	config.pszContent = body;
	config.pszMainIcon = MAKEINTRESOURCE(IDI_APPICON);

	int button;
	$TaskDialogIndirect(&config, &button, NULL, NULL);
	return button;
}
