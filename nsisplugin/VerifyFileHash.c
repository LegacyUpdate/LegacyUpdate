#include <windows.h>
#include <nsis/pluginapi.h>
#include "sha256.h"

static BOOL hexToBinary(const char *hex, uint8_t *binary, int binaryLength) {
	if (lstrlenA(hex) != binaryLength * 2) {
		return FALSE;
	}

	for (int i = 0; i < binaryLength; i++) {
		char high = hex[i * 2];
		char low = hex[i * 2 + 1];

		if (high >= '0' && high <= '9') {
			high -= '0';
		} else if (high >= 'a' && high <= 'f') {
			high -= 'a' - 10;
		} else {
			// Invalid character
			return FALSE;
		}

		if (low >= '0' && low <= '9') {
			low -= '0';
		} else if (low >= 'a' && low <= 'f') {
			low -= 'a' - 10;
		} else {
			// Invalid character
			return FALSE;
		}

		binary[i] = (uint8_t)((high << 4) | low);
	}

	return TRUE;
}

static BOOL calculateFileSha256(const LPWSTR path, uint8_t hash[SIZE_OF_SHA_256_HASH]) {
	HANDLE file = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	struct Sha_256 sha256;
	sha_256_init(&sha256, hash);

	uint8_t buffer[8192];
	DWORD bytesRead = 0;
	while (ReadFile(file, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0) {
		sha_256_write(&sha256, buffer, bytesRead);
	}

	sha_256_close(&sha256);
	CloseHandle(file);
	return TRUE;
}

PLUGIN_METHOD(VerifyFileHash) {
	PLUGIN_INIT();

	WCHAR filename[MAX_PATH], expectedHashHex[256];
	popstringn(filename, sizeof(filename) / sizeof(WCHAR));
	popstringn(expectedHashHex, sizeof(expectedHashHex) / sizeof(WCHAR));

	char expectedHashHexA[256];
	WideCharToMultiByte(CP_ACP, 0, expectedHashHex, -1, expectedHashHexA, sizeof(expectedHashHexA), NULL, NULL);

	uint8_t expectedHash[SIZE_OF_SHA_256_HASH], calculatedHash[SIZE_OF_SHA_256_HASH];

	if (!hexToBinary(expectedHashHexA, expectedHash, SIZE_OF_SHA_256_HASH)) {
		pushint(-1);
		return;
	}

	if (!calculateFileSha256(filename, calculatedHash)) {
		pushint(-1);
		return;
	}

	WCHAR calculatedHashHex[SIZE_OF_SHA_256_HASH * 2 + 1];
	for (int i = 0; i < SIZE_OF_SHA_256_HASH; i++) {
		wsprintf(&calculatedHashHex[i * 2], L"%02x", calculatedHash[i]);
	}
	pushstring(calculatedHashHex);

	for (int i = 0; i < SIZE_OF_SHA_256_HASH; i++) {
		if (expectedHash[i] != calculatedHash[i]) {
			pushint(0);
			return;
		}
	}

	pushint(1);
}
