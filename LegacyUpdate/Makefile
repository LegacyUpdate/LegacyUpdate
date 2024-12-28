override DEBUG := $(or $(DEBUG),1)

MSBUILDCONFIG32 = $(if $(filter 1,$(DEBUG)),Debug-VC08,Release)
MSBUILDCONFIG64 = $(if $(filter 1,$(DEBUG)),Debug-VC17,Release)

MSBUILDFLAGS = /v:minimal /m

ifeq ($(CI),1)
ifeq ($(DEBUG),1)
	MSBUILDCONFIG32 = Debug-VC17
endif
	MSBUILDFLAGS += /p:PlatformToolset=v141_xp
endif

MSBUILDFLAGS32 = $(MSBUILDFLAGS) /p:Configuration=$(MSBUILDCONFIG32) /p:Platform=Win32
MSBUILDFLAGS64 = $(MSBUILDFLAGS) /p:Configuration=$(MSBUILDCONFIG64) /p:Platform=x64

VSPATH ?= $(shell wslpath "$(shell '/mnt/c/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe' -latest -property installationPath)")
PATH := $(VSPATH)/MSBuild/Current/Bin:$(PATH)

MSBUILD = msbuild.exe

all:
	@# Workaround for "dlldatax.obj: LNK2001: unresolved external symbol _LegacyUpdate_ProxyFileInfo"
	rm -f LegacyUpdate_p.c
	cd ..; $(MSBUILD) $(MSBUILDFLAGS32) LegacyUpdate.sln
	rm -f LegacyUpdate_p.c
	cd ..; $(MSBUILD) $(MSBUILDFLAGS64) LegacyUpdate.sln

ifeq ($(SIGN),1)
	../build/sign.sh \
		../Release/LegacyUpdate.dll \
		../x64/Release/LegacyUpdate.dll
endif

clean:
	rm -rf \
		Debug-VC08 Debug-VC17 Release x64 \
		LegacyUpdate_i.c LegacyUpdate_i.h LegacyUpdate_p.c LegacyUpdateidl.h
	cd ..; $(MSBUILD) $(MSBUILDFLAGS32) LegacyUpdate.sln /t:Clean
	cd ..; $(MSBUILD) $(MSBUILDFLAGS64) LegacyUpdate.sln /t:Clean

.PHONY: all clean
