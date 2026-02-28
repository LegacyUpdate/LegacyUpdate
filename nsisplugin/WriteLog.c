#include "stdafx.h"
#include <nsis/pluginapi.h>
#include <commctrl.h>
#include "Log.h"

static HWND g_hwndLogList = 0;
static WNDPROC g_pfnListViewProc = NULL;

static LRESULT CALLBACK LogListViewProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == LVM_INSERTITEM) {
		LVITEM *item = (LVITEM *)lParam;
		if (item && (item->mask & LVIF_TEXT) && item->pszText) {
			LogInternal(item->pszText);
		}
	}

	return CallWindowProc(g_pfnListViewProc, hwnd, msg, wParam, lParam);
}

PLUGIN_METHOD(InitLog) {
	PLUGIN_INIT();
	OpenLog();

	// Hook progress log to send items to the log file
	if (!g_hwndLogList && g_hwndParent) {
		HWND innerWindow = FindWindowEx(g_hwndParent, NULL, L"#32770", NULL);
		HWND listView = FindWindowEx(innerWindow, NULL, L"SysListView32", NULL);

		if (listView) {
			g_hwndLogList = listView;
			g_pfnListViewProc = (WNDPROC)SetWindowLong(g_hwndLogList, GWL_WNDPROC, (LONG_PTR)LogListViewProc);
		}
	}
}

PLUGIN_METHOD(WriteLog) {
	PLUGIN_INIT();

	WCHAR text[NSIS_MAX_STRLEN];
	popstring(text);
	LogInternal(text);
}
