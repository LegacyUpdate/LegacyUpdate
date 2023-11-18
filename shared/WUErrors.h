#pragma once
#include <windows.h>

struct HResultMessage {
	HRESULT hr;
	const LPWSTR message;
};

EXTERN_C HResultMessage WUErrorMessages[];
