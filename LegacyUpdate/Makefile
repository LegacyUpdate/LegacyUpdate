override DEBUG := $(or $(DEBUG),1)
MSBUILDCONFIG = $(if $(filter 1,$(DEBUG)),Debug-VC08,Release)

ifeq ($(CI)$(DEBUG),11)
	MSBUILDCONFIG = Debug-VC17
endif

VSPATH ?= $(shell wslpath "$(shell '/mnt/c/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe' -latest -property installationPath)")
PATH := $(VSPATH)/MSBuild/Current/Bin:$(PATH)

MSBUILD = msbuild.exe

all:
	@# Unfortunately always needs to be a clean build because of "dlldatax.obj: error LNK2001:
	@# unresolved external symbol _LegacyUpdate_ProxyFileInfo"
	+$(MAKE) clean
	cd ..; $(MSBUILD) LegacyUpdate.sln /v:minimal /m /p:Configuration=$(MSBUILDCONFIG) /p:Platform=Win32
	cd ..; $(MSBUILD) LegacyUpdate.sln /v:minimal /m /p:Configuration=$(MSBUILDCONFIG) /p:Platform=x64

ifeq ($(SIGN),1)
	../build/sign.sh \
		../Release/LegacyUpdate.dll \
		../x64/Release/LegacyUpdate.dll
endif

clean:
	cd ..; $(MSBUILD) LegacyUpdate.sln /v:quiet /m /p:Configuration=$(MSBUILDCONFIG) /p:Platform=Win32 /t:clean
	cd ..; $(MSBUILD) LegacyUpdate.sln /v:quiet /m /p:Configuration=$(MSBUILDCONFIG) /p:Platform=x64 /t:clean

.PHONY: all clean
