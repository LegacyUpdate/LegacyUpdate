SIGN ?= 0

all:
	+$(MAKE) -C ../nsisplugin
	+$(MAKE) setup activex

setup:
	makensis -DSIGN=$(SIGN) setup.nsi

activex:
ifeq ($(SIGN),1)
	cp LegacyUpdate-*.exe codebase/setup.exe
	cd codebase && makecab.exe /f lucontrl.ddf
	../build/sign.sh codebase/lucontrl.cab
	rm codebase/setup.exe codebase/setup.rpt
endif

clean:
	+$(MAKE) -C ../nsisplugin clean
	rm -f LegacyUpdate-*.exe codebase/{lucontrl.cab,setup.exe,setup.rpt}

.PHONY: all nsisplugin setup activex clean
