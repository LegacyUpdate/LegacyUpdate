export CI ?= 0
export SIGN ?= 0
export DEBUG ?= 1

all: before-all setup

before-all:
ifeq ($(DEBUG),0)
	+$(MAKE) clean
endif

activex:
	+$(MAKE) -C LegacyUpdate

launcher:
	+$(MAKE) -C launcher

nsisplugin:
	+$(MAKE) -C nsisplugin

setup: activex launcher nsisplugin
	+$(MAKE) -C setup

nt4:
ifeq ($(DEBUG),0)
	+$(MAKE) clean-nt4
endif

	+$(MAKE) -C setup nt4

clean:
	+$(MAKE) -C LegacyUpdate clean
	+$(MAKE) -C launcher clean
	+$(MAKE) -C nsisplugin clean
	+$(MAKE) -C setup clean

clean-nt4:
	+$(MAKE) -C setup clean

.PHONY: all before-all activex launcher nsisplugin setup nt4 clean clean-nt4
