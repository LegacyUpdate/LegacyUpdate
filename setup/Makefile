SIGN ?= 0

all: setup activex

setup:
	makensis -DSIGN=$(SIGN) setup.nsi

activex:
	cp LegacyUpdate-*.exe codebase/setup.exe
	cd codebase && makecab.exe /f lucontrl.ddf
ifeq ($(SIGN),1)
	../build/sign.sh codebase/lucontrl.cab
endif
	rm codebase/setup.exe codebase/setup.rpt

clean:
	@# Not needed

.PHONY: all setup activex
