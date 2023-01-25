// dllmain.h : Declaration of module class.

class CLegacyUpdateModule : public CAtlDllModuleT<CLegacyUpdateModule> {
public:
	DECLARE_LIBID(LIBID_LegacyUpdateLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_LEGACYUPDATEOCX, "{D0A82CD0-B6F0-4101-83ED-DA47D0D04830}")
};

extern class CLegacyUpdateModule _AtlModule;
