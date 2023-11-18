#pragma once
#include <windows.h>

struct HResultMessage {
	HRESULT hr;
	const LPWSTR message;
};

extern HResultMessage WUErrorMessages[];
