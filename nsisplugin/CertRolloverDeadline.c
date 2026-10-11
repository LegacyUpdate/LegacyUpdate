#include <windows.h>
#include <nsis/pluginapi.h>
#include <CertRolloverDeadline.h>

PLUGIN_METHOD(BeforeCertRolloverDeadline) {
	PLUGIN_INIT();
	pushint(IsBeforeCertRolloverDeadline());
}
