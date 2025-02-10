DEBUG ?= 1
SIGN ?= 0
CI ?= 0
NT4 ?= 0

MAKENSIS = makensis
NSISFLAGS = -DDEBUG=$(DEBUG) -DSIGN=$(SIGN) -DCI=$(CI) -DNT4=$(NT4)

all:
	+$(MAKE) -C ../nsisplugin
	+$(MAKE) setup
	+$(MAKE) activex

nt4:
	+$(MAKE) -C ../nsisplugin
	+$(MAKE) NT4=1 setup-nt4

setup:
	$(MAKENSIS) $(NSISFLAGS) setup.nsi

setup-nt4:
	$(MAKENSIS) $(NSISFLAGS) setup-nt4.nsi

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
