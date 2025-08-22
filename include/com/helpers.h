#pragma once

#include <combaseapi.h>
#include <oleauto.h>
#include <ocidl.h>

#define DEFINE_UUIDOF(cls, uuid) \
	template<> const GUID &__mingw_uuidof<cls>() { \
		return uuid; \
	}

#define HIMETRIC_PER_INCH 2540
#define MAP_PIX_TO_LOGHIM(x, ppli) MulDiv(HIMETRIC_PER_INCH, (x), (ppli))
#define MAP_LOGHIM_TO_PIX(x, ppli) MulDiv((ppli), (x), HIMETRIC_PER_INCH)
