ifeq ($(CI),1)
	MSBUILDFLAGS += /p:PlatformToolset=v141_xp
endif

all:
	cmd.exe /c build.cmd $(MSBUILDFLAGS)

ifeq ($(SIGN),1)
	../build/sign.sh \
		../Release/LegacyUpdate.dll \
		../x64/Release/LegacyUpdate.dll
endif

.PHONY: all
