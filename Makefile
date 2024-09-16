export SIGN ?= 0
export DEBUG ?= 1

all:
	+$(MAKE) -C LegacyUpdate
	+$(MAKE) -C launcher
	+$(MAKE) -C setup

clean:
	+$(MAKE) -C LegacyUpdate clean
	+$(MAKE) -C launcher clean
	+$(MAKE) -C setup clean

.PHONY: all clean
