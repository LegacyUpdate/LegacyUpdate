#pragma once

#include <windows.h>

EXTERN_C struct HResultMessage {
	HRESULT hr;
	LPWSTR message;
};

EXTERN_C HResultMessage WUErrorMessages[];
