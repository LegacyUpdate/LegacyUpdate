#pragma once

#include <windows.h>

EXTERN_C struct HResultMessage {
	HRESULT hr;
	LPWSTR message;
} HResultMessage;

EXTERN_C struct HResultMessage WUErrorMessages[];
