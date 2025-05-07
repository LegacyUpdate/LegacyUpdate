// Based on https://nsis.sourceforge.io/TaskbarProgress_plug-in - zlib licensed
// Cleaned up and refactored into C by Legacy Update
#undef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#include <windows.h>
#include <nsis/pluginapi.h>
#include <commctrl.h>
#include <objbase.h>
#include <shobjidl.h>
#include "main.h"
#include "VersionInfo.h"

static extra_parameters *g_extra;
static ITaskbarList3 *g_taskbarList;
static UINT g_totalRange;
static WNDPROC g_progressOrigWndProc;
static WNDPROC g_dialogOrigWndProc;

LRESULT CALLBACK ProgressBarWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (!g_progressOrigWndProc) {
		return 0;
	}

	switch (uMsg) {
	case PBM_SETRANGE:
		g_totalRange = LOWORD(lParam) + HIWORD(lParam);
		break;

	case PBM_SETRANGE32:
		g_totalRange = wParam + lParam;
		break;

	case PBM_SETPOS:
		if (g_taskbarList) {
			ITaskbarList3_SetProgressValue(g_taskbarList, g_hwndParent, wParam, g_totalRange);
		}
		break;

	case WM_DESTROY:
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)g_progressOrigWndProc);

		if (g_taskbarList) {
			ITaskbarList3_SetProgressState(g_taskbarList, g_hwndParent, TBPF_NOPROGRESS);
			ITaskbarList3_Release(g_taskbarList);
			g_taskbarList = NULL;
		}

		g_progressOrigWndProc = NULL;
		break;
	}

	return CallWindowProc(g_progressOrigWndProc, hwnd, uMsg, wParam, lParam);
}

static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (!g_dialogOrigWndProc) {
		return 0;
	}

	switch (uMsg) {
	case WM_NOTIFY_OUTER_NEXT:
		if (g_extra->exec_flags->abort) {
			// Set the progress bar to error state (red)
			HWND innerWindow = FindWindowEx(hwnd, NULL, L"#32770", NULL);
			HWND progressBar = FindWindowEx(innerWindow, NULL, L"msctls_progress32", NULL);
			if (progressBar) {
				SendMessage(progressBar, PBM_SETSTATE, PBST_ERROR, 0);
			}

			if (g_taskbarList) {
				ITaskbarList3_SetProgressState(g_taskbarList, g_hwndParent, TBPF_ERROR);
			}
		}
		break;

	case WM_DESTROY:
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)g_dialogOrigWndProc);
		g_dialogOrigWndProc = NULL;
		break;
	}

	return CallWindowProc(g_dialogOrigWndProc, hwnd, uMsg, wParam, lParam);
}

static UINT_PTR NSISPluginCallback(enum NSPIM event) {
	// Does nothing, but keeping a callback registered prevents NSIS from unloading the plugin
	return 0;
}

PLUGIN_METHOD(InitTaskbarProgress) {
	PLUGIN_INIT();

	if (!AtLeastWinVista()) {
		return;
	}

	g_extra = extra;
	extra->RegisterPluginCallback(g_hInstance, NSISPluginCallback);

	if (g_progressOrigWndProc) {
		// Already initialised
		return;
	}

	HWND innerWindow = FindWindowEx(g_hwndParent, NULL, L"#32770", NULL);
	HWND progressBar = FindWindowEx(innerWindow, NULL, L"msctls_progress32", NULL);
	PBRANGE range = {0};
	HRESULT hr = E_FAIL;

	if (!progressBar) {
		goto fail;
	}

	hr = CoCreateInstance(&CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, &IID_ITaskbarList3, (void **)&g_taskbarList);
	if (!SUCCEEDED(hr)) {
		goto fail;
	}

	hr = ITaskbarList3_HrInit(g_taskbarList);
	if (!SUCCEEDED(hr)) {
		goto fail;
	}

	// Get the initial progress bar range
	SendMessage(progressBar, PBM_GETRANGE, 0, (LPARAM)&range);
	g_totalRange = range.iLow + range.iHigh;

	// Add our own window procedure so we can respond to progress bar updates
	g_progressOrigWndProc = (WNDPROC)SetWindowLongPtr(progressBar, GWLP_WNDPROC, (LONG_PTR)ProgressBarWndProc);
	g_dialogOrigWndProc = (WNDPROC)SetWindowLongPtr(g_hwndParent, GWLP_WNDPROC, (LONG_PTR)MainWndProc);
	if (!g_progressOrigWndProc || !g_dialogOrigWndProc) {
		goto fail;
	}
	return;

fail:
	if (g_taskbarList) {
		ITaskbarList3_Release(g_taskbarList);
		g_taskbarList = NULL;
	}

	g_progressOrigWndProc = NULL;
	g_dialogOrigWndProc = NULL;
}
