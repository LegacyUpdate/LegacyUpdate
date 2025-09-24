DEBUG ?= 1
SIGN ?= 0
CI ?= 0

MAKENSIS = makensis
NSISFLAGS = -DDEBUG=$(DEBUG) -DSIGN=$(SIGN) -DCI=$(CI)

FILES   = resource.c
RCFILES = resource.rc
BIN     = obj/resource.dll

include ../build/shared.mk

CFLAGS  += -mdll
LDFLAGS += -Wl,-e_DllMain

all:: internal-all nsisplugin setup activex

nt4: internal-all nsisplugin setup-nt4

nsisplugin:
ifeq ($(MAKELEVEL),0)
	+$(MAKE) -C ../nsisplugin
endif

setup: internal-all nsisplugin
	$(MAKENSIS) $(NSISFLAGS) -DNT4=0 setup.nsi

setup-nt4: internal-all nsisplugin
	$(MAKENSIS) $(NSISFLAGS) -DNT4=1 setup-nt4.nsi

activex: setup
ifeq ($(SIGN),1)
	cp LegacyUpdate-*.exe codebase/setup.exe
	cd codebase && makecab.exe /f lucontrl.ddf
	../build/sign.sh codebase/lucontrl.cab
	rm codebase/setup.exe codebase/setup.rpt
endif

clean::
	rm -rf obj
	rm -f LegacyUpdate-*.exe codebase/{lucontrl.cab,setup.exe,setup.rpt}
ifeq ($(MAKELEVEL),0)
	+$(MAKE) -C ../nsisplugin clean
endif

test:
	+$(MAKE)
	sudo.exe LegacyUpdate-*.exe

.PHONY: all internal-all nt4 nsisplugin setup setup-nt4 activex clean
