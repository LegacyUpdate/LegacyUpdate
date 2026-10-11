#include <windows.h>
#include <nsis/pluginapi.h>
#include <VersionInfo.h>

// 2027-06-19T23:03:47Z, the Not After date of Microsoft Update Signing CA 2.1
#define CERT_DEADLINE_UPDATE_CA_2_1  (134579198270000000ULL)

// 2027-05-17T19:40:02Z, the Not After date of Microsoft Time-Stamp Service
#define CERT_DEADLINE_TSTAMP_SERVICE (134550564020000000ULL)

PLUGIN_METHOD(IsBeforeCertRolloverDeadline) {
	PLUGIN_INIT();

	if (AtLeastWin11_25H2()) {
		// We’re good, for now
		pushint(1);
		return;
	}

	FILETIME currentTime = {0};
	GetSystemTimeAsFileTime(&currentTime);

	ULONGLONG currentTime64 = 0;
	memcpy(&currentTime64, &currentTime, sizeof(currentTime64));

	ULONGLONG deadline = AtLeastWin10_1809() ? CERT_DEADLINE_TSTAMP_SERVICE : CERT_DEADLINE_UPDATE_CA_2_1;

	pushint(currentTime64 < deadline ? 1 : 0);
}
