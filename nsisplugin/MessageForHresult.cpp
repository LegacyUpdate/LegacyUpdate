#include <windows.h>
#include <nsis/pluginapi.h>
#include "../shared/HResult.h"

EXTERN_C __declspec(dllexport)
void __cdecl MessageForHresult(HWND hwndParent, int string_size, TCHAR *variables, stack_t **stacktop, extra_parameters *extra) {
	EXDLL_INIT();
	g_hwndParent = hwndParent;

	HRESULT hr = popint();
	LPWSTR message = GetMessageForHresult(hr);
	pushstring(message);
	LocalFree(message);
}
