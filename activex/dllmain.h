// dllmain.h : Declaration of module class.

class CLegacyUpdateOCXModule : public CAtlDllModuleT<CLegacyUpdateOCXModule> {
public:
	DECLARE_LIBID(LIBID_LegacyUpdateOCXLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_LEGACYUPDATEOCX, "{D0A82CD0-B6F0-4101-83ED-DA47D0D04830}")
};

extern class CLegacyUpdateOCXModule _AtlModule;
