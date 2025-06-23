#include "stdafx.h"
#include "main.h"
#include "resource.h"

#undef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_VISTA
#include <commctrl.h>

typedef HRESULT (WINAPI *_TaskDialogIndirect)(const TASKDIALOGCONFIG *pTaskConfig, int *pnButton, int *pnRadioButton, BOOL *pfVerificationFlagChecked);

static BOOL _loadedTaskDialog = FALSE;
static _TaskDialogIndirect $TaskDialogIndirect;

int MsgBox(HWND hwnd, LPCTSTR instruction, LPCTSTR body, UINT type) {
	if (!_loadedTaskDialog) {
		_loadedTaskDialog = TRUE;
		$TaskDialogIndirect = (_TaskDialogIndirect)GetProcAddress(LoadLibrary(L"comctl32.dll"), "TaskDialogIndirect");
	}

	// Play the sound matching the icon, because MB_USERICON doesn't play a sound
	MessageBeep(type & 0x000000F0);
	type = (type & ~0x000000F0) | MB_USERICON;

	if (!$TaskDialogIndirect) {
		LPWSTR finalBody = (LPWSTR)instruction;
		if (body && lstrlen(body) > 0) {
			size_t length = lstrlen(instruction) + lstrlen(body) + 3;
			finalBody = (LPWSTR)LocalAlloc(LPTR, length * sizeof(TCHAR));
			wsprintf(finalBody, L"%ls\n\n%ls", instruction, body);
		}

		MSGBOXPARAMS params = {0};
		params.cbSize = sizeof(MSGBOXPARAMS);
		params.hwndOwner = hwnd;
		params.hInstance = GetModuleHandle(NULL);
		params.lpszText = finalBody;
		params.lpszCaption = L"Legacy Update";
		params.dwStyle = type;
		params.lpszIcon = MAKEINTRESOURCE(IDI_APPICON);
		int result = MessageBoxIndirect(&params);

		if (finalBody != body) {
			LocalFree(finalBody);
		}

		return result;
	}

	TASKDIALOG_COMMON_BUTTON_FLAGS buttons = 0;
	DWORD flags = TDF_POSITION_RELATIVE_TO_WINDOW;

	switch (type & 0x0000000F) {
	case MB_OK:
		buttons = TDCBF_OK_BUTTON;
		flags |= TDF_ALLOW_DIALOG_CANCELLATION;
		break;
	case MB_OKCANCEL:
		buttons = TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON;
		flags |= TDF_ALLOW_DIALOG_CANCELLATION;
		break;
	case MB_YESNO:
		buttons = TDCBF_YES_BUTTON | TDCBF_NO_BUTTON;
		break;
	default:
		break;
	}

	TASKDIALOGCONFIG config = {0};
	config.cbSize = sizeof(TASKDIALOGCONFIG);
	config.hwndParent = hwnd;
	config.hInstance = GetModuleHandle(NULL);
	config.dwFlags = flags;
	config.dwCommonButtons = buttons;
	config.pszWindowTitle = L"Legacy Update";
	config.pszMainInstruction = instruction;
	config.pszContent = body;
	config.pszMainIcon = MAKEINTRESOURCE(IDI_APPICON);

	int button = 0;
	$TaskDialogIndirect(&config, &button, NULL, NULL);
	return button;
}
