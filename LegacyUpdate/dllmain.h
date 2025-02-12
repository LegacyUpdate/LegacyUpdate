// dllmain.h : Declaration of module class.

DEFINE_GUID(LIBID_LegacyUpdateLib, 0x05D22F33, 0xC7C3, 0x4C90, 0xBD, 0xD9, 0xCE, 0xDC, 0x86, 0xEA, 0x8F, 0xBE);

class CLegacyUpdateModule : public CAtlDllModuleT<CLegacyUpdateModule> {
public:
	DECLARE_LIBID(LIBID_LegacyUpdateLib)

	static LPCOLESTR GetAppId() {
		return GetAppIdT();
	}

	static const TCHAR *GetAppIdT() {
		return L"{D0A82CD0-B6F0-4101-83ED-DA47D0D04830}";
	}
};

extern class CLegacyUpdateModule _AtlModule;
extern HINSTANCE g_hInstance;
