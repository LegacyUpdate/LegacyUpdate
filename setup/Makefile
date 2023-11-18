SIGN ?= 0

all:
	makensis -DSIGN=$(SIGN) setup.nsi

activex:
	cp $(wildcard LegacyUpdate-*.exe) codebase/setup.exe
	cd codebase && makecab.exe /f lucontrl.ddf
ifeq ($(SIGN),1)
	../build/sign.sh codebase/lucontrl.cab
endif
	rm codebase/setup.exe codebase/setup.rpt

.PHONY: all activex
