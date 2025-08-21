DEBUG ?= 1
SIGN ?= 0
CI ?= 0
NT4 ?= 0

MAKENSIS = makensis
NSISFLAGS = -DDEBUG=$(DEBUG) -DSIGN=$(SIGN) -DCI=$(CI) -DNT4=$(NT4)

all: nsisplugin setup activex

nt4: nsisplugin setup-nt4

nsisplugin:
ifeq ($(MAKELEVEL),0)
	+$(MAKE) -C ../nsisplugin
endif

setup: nsisplugin
	$(MAKENSIS) $(NSISFLAGS) setup.nsi

setup-nt4: nsisplugin
	$(MAKENSIS) $(NSISFLAGS) setup-nt4.nsi

activex: setup
ifeq ($(SIGN),1)
	cp LegacyUpdate-*.exe codebase/setup.exe
	cd codebase && makecab.exe /f lucontrl.ddf
	../build/sign.sh codebase/lucontrl.cab
	rm codebase/setup.exe codebase/setup.rpt
endif

clean:
ifeq ($(MAKELEVEL),0)
	+$(MAKE) -C ../nsisplugin clean
endif
	rm -f LegacyUpdate-*.exe codebase/{lucontrl.cab,setup.exe,setup.rpt}

test:
	+$(MAKE)
	sudo.exe LegacyUpdate-*.exe

.PHONY: all nt4 nsisplugin setup setup-nt4 activex clean
