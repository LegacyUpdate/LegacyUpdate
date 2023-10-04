all:
	cmd.exe /c build.cmd

ifeq ($(SIGN),1)
	../build/sign.sh \
		../Release/LegacyUpdate.dll \
		../x64/Release/LegacyUpdate.dll
endif

.PHONY: all
