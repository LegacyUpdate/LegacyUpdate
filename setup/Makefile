DEBUG ?= 1
SIGN ?= 0
CI ?= 0

MAKENSIS = makensis
NSISFLAGS = -DDEBUG=$(DEBUG) -DSIGN=$(SIGN) -DCI=$(CI)

all:
	+$(MAKE) -C ../nsisplugin
	+$(MAKE) setup
	+$(MAKE) activex

setup:
	$(MAKENSIS) $(NSISFLAGS) setup.nsi

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

test:
	+$(MAKE)
	sudo.exe LegacyUpdate-*.exe

.PHONY: all nsisplugin setup activex clean
